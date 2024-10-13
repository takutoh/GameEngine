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

Scene* g_Scene;

using namespace DirectX;

VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT];
RootSignature* rootSignature;
PipelineState* pipelineState;
IndexBuffer* indexBuffer;

static UINT WINDOW_WIDTH = Application::WINDOW_WIDTH;
static UINT WINDOW_HEIGHT = Application::WINDOW_HEIGHT;

bool Scene::Init()
{
	// 頂点を4つにして四角形を定義
	Vertex vertices[4] = {};
	vertices[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[3].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	// 頂点バッファの生成
	auto vertexSize = sizeof(Vertex) * std::size(vertices);
	auto vertexStride = sizeof(Vertex);
	vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	if (!vertexBuffer->IsValid())
	{
		return false;
	}

	// 変換行列の生成
	auto eyePos = XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);                                  // 視点の位置
	auto targetPos = XMVectorZero();                                                    // 視点を向ける座標
	auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);                                  // 上方向を表すベクトル
	constexpr auto fov = XMConvertToRadians(37.5);                                      // 視野角
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
		ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
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

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // これに書かれている順序で描画

	// インデックスバッファの生成
	auto size = sizeof(uint32_t) * std::size(indices);
	indexBuffer = new IndexBuffer(size, indices);
	if (!indexBuffer->IsValid())
	{
		return false;
	}

	return true;
}

void Scene::Update()
{

}

void Scene::Draw()
{
	auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号の取得
	auto commandList = g_Engine->CommandList();             // コマンドリスト
	auto vbView = vertexBuffer->View();                     // 頂点バッファビュー
	auto ibView = indexBuffer->View();                      // インデックスバッファビュー

	commandList->SetGraphicsRootSignature(rootSignature->Get());                                   // ルートシグネチャをセット
	commandList->SetPipelineState(pipelineState->Get());                                           // パイプラインステートをセット
	commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress()); // 定数バッファをセット

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView);                                   // インデックスバッファをセット

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 6個のインデックスで描画
}
