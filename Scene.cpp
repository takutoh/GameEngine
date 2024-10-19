#include "App.h"
#include "Engine.h"
#include "Scene.h"
#include "d3dx12.h"
#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "IndexBuffer.h"
#include "AssimpLoader.h"
#include "DescriptorHeap.h"
#include "Texture2D.h"

#include <filesystem>

namespace fs = std::filesystem;
std::wstring ReplaceExtension(const std::wstring& origin, const char* ext)
{
	fs::path p = origin.c_str();
	return p.replace_extension(ext).c_str();
}

DescriptorHeap* descriptorHeap;
std::vector< DescriptorHandle*> materialHandles; // テクスチャ用のハンドル一覧

Scene* g_Scene;

using namespace DirectX;

VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT];
RootSignature* rootSignature;
PipelineState* pipelineState;
IndexBuffer* indexBuffer;

const wchar_t* modelFile = L"Assets/Alicia/FBX/Alicia_solid_Unity.FBX";

std::vector<Mesh> meshes;                 // メッシュの配列
std::vector<VertexBuffer*> vertexBuffers; // メッシュの数分の頂点バッファ
std::vector<IndexBuffer*> indexBuffers;   // メッシュの数分のインデックスバッファ

static UINT WINDOW_WIDTH = Application::WINDOW_WIDTH;
static UINT WINDOW_HEIGHT = Application::WINDOW_HEIGHT;

bool Scene::Init()
{
	ImportSettings importSetting =
	{
		modelFile,
		meshes,
		false,
		true
	};

	AssimpLoader loader;
	if (!loader.Load(importSetting))
	{
		return false;
	}

	// 頂点バッファの生成
	vertexBuffers.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto size = sizeof(Vertex) * meshes[i].Vertices.size();
		auto stride = sizeof(Vertex);
		auto vertices = meshes[i].Vertices.data();
		auto pVB = new VertexBuffer(size, stride, vertices);
		if (!pVB->IsValid())
		{
			return false;
		}

		vertexBuffers.push_back(pVB);
	}

	// インデックスバッファの生成
	indexBuffers.reserve(meshes.size());
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto size = sizeof(uint32_t) * meshes[i].Indices.size();
		auto indices = meshes[i].Indices.data();
		auto pIB = new IndexBuffer(size, indices);
		if (!pIB->IsValid())
		{
			return false;
		}

		indexBuffers.push_back(pIB);
	}

	// 変換行列の生成
	constexpr auto fov = XMConvertToRadians(60);                                        // 視野角
	auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // アスペクト比

	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		constantBuffer[i] = new ConstantBuffer(sizeof(Transform));
		if (!constantBuffer[i]->IsValid())
		{
			return false;
		}

		// 変換行列の登録
		auto ptr = constantBuffer[i]->GetPtr<Transform>();
		ptr->World = XMMatrixIdentity();

		// カメラの位置と視点を設定
		auto eyePos = XMLoadFloat3(&cameraPosition);
		XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(cameraRotation.x),
			XMConvertToRadians(cameraRotation.y),
			XMConvertToRadians(cameraRotation.z)));
		auto targetPos = eyePos + forward;
		auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
	}

	// DescriptorHeapの初期化とマテリアルの読み込み
	descriptorHeap = new DescriptorHeap();
	materialHandles.clear();

	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto texPath = ReplaceExtension(meshes[i].DiffuseMap, "tga");
		auto mainTex = Texture2D::Get(texPath);
		auto handle = descriptorHeap->Register(mainTex);
		materialHandles.push_back(handle);
	}

	// ルートシグネチャの生成
	rootSignature = new RootSignature();
	if (!rootSignature->IsValid())
	{
		return false;
	}

	// パイプラインステートの生成
	pipelineState = new PipelineState();
	pipelineState->SetInputLayout(Vertex::InputLayout);
	pipelineState->SetRootSignature(rootSignature->Get());
#ifdef _DEBUG
	pipelineState->SetVS(L"../GameEngine/x64/Debug/SimpleVS.cso");
	pipelineState->SetPS(L"../GameEngine/x64/Debug/SimplePS.cso");
#else
	pipelineState->SetVS(L"../GameEngine/x64/Release/SimpleVS.cso");
	pipelineState->SetPS(L"../GameEngine/x64/Release/SimplePS.cso");
#endif
	pipelineState->Create();
	if (!pipelineState->IsValid())
	{
		return false;
	}

	return true;
}

void Scene::Update()
{
	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++) {
		auto ptr = constantBuffer[i]->GetPtr<Transform>();

		auto eyePos = XMLoadFloat3(&cameraPosition);
		// カメラの位置を読み込み
		XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(cameraRotation.x),
			XMConvertToRadians(cameraRotation.y),
			XMConvertToRadians(cameraRotation.z)));
		auto targetPos = eyePos + forward;
		auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
	}
}

void Scene::Draw()
{
	auto currentIndex = g_Engine->CurrentBackBufferIndex();
	auto commandList = g_Engine->CommandList();
	auto materialHeap = descriptorHeap->GetHeap();

	// メッシュの数だけインデックス分の描画を行う処理を回す
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto vbView = vertexBuffers[i]->View(); // そのメッシュに対応する頂点バッファ
		auto ibView = indexBuffers[i]->View();  // そのメッシュに対応する頂点バッファ

		commandList->SetGraphicsRootSignature(rootSignature->Get());
		commandList->SetPipelineState(pipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);

		commandList->SetDescriptorHeaps(1, &materialHeap);
		commandList->SetGraphicsRootDescriptorTable(1, materialHandles[i]->HandleGPU);

		commandList->DrawIndexedInstanced(meshes[i].Indices.size(), 1, 0, 0, 0); // インデックスの数分描画
	}
}

void Scene::Play()
{
	XMMATRIX translationMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

	rotationAngleX += XMConvertToRadians(speedX);
	rotationAngleY += XMConvertToRadians(speedY);
	rotationAngleZ += XMConvertToRadians(speedZ);

	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++) {
		auto ptr = constantBuffer[i]->GetPtr<Transform>();

		XMMATRIX rotationMatrix = XMMatrixRotationX(rotationAngleX) *
			XMMatrixRotationY(rotationAngleY) *
			XMMatrixRotationZ(rotationAngleZ);

		// 3Dモデルを回転、移動
		ptr->World = rotationMatrix * translationMatrix;
	}
}

void Scene::Stop()
{
	XMMATRIX translationMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

	rotationAngleX = 0.0;
	rotationAngleY = 0.0;
	rotationAngleZ = 0.0;

	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++) {
		auto ptr = constantBuffer[i]->GetPtr<Transform>();

		// 3Dモデルを元の位置に
		ptr->World = translationMatrix;
	}
}
