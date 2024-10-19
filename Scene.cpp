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
std::vector< DescriptorHandle*> materialHandles; // �e�N�X�`���p�̃n���h���ꗗ

Scene* g_Scene;

using namespace DirectX;

VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT];
RootSignature* rootSignature;
PipelineState* pipelineState;
IndexBuffer* indexBuffer;

const wchar_t* modelFile = L"Assets/Alicia/FBX/Alicia_solid_Unity.FBX";

std::vector<Mesh> meshes;                 // ���b�V���̔z��
std::vector<VertexBuffer*> vertexBuffers; // ���b�V���̐����̒��_�o�b�t�@
std::vector<IndexBuffer*> indexBuffers;   // ���b�V���̐����̃C���f�b�N�X�o�b�t�@

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

	// ���_�o�b�t�@�̐���
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

	// �C���f�b�N�X�o�b�t�@�̐���
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

	// �ϊ��s��̐���
	constexpr auto fov = XMConvertToRadians(60);                                        // ����p
	auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // �A�X�y�N�g��

	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		constantBuffer[i] = new ConstantBuffer(sizeof(Transform));
		if (!constantBuffer[i]->IsValid())
		{
			return false;
		}

		// �ϊ��s��̓o�^
		auto ptr = constantBuffer[i]->GetPtr<Transform>();
		ptr->World = XMMatrixIdentity();

		// �J�����̈ʒu�Ǝ��_��ݒ�
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

	// DescriptorHeap�̏������ƃ}�e���A���̓ǂݍ���
	descriptorHeap = new DescriptorHeap();
	materialHandles.clear();

	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto texPath = ReplaceExtension(meshes[i].DiffuseMap, "tga");
		auto mainTex = Texture2D::Get(texPath);
		auto handle = descriptorHeap->Register(mainTex);
		materialHandles.push_back(handle);
	}

	// ���[�g�V�O�l�`���̐���
	rootSignature = new RootSignature();
	if (!rootSignature->IsValid())
	{
		return false;
	}

	// �p�C�v���C���X�e�[�g�̐���
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
		// �J�����̈ʒu��ǂݍ���
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

	// ���b�V���̐������C���f�b�N�X���̕`����s����������
	for (size_t i = 0; i < meshes.size(); i++)
	{
		auto vbView = vertexBuffers[i]->View(); // ���̃��b�V���ɑΉ����钸�_�o�b�t�@
		auto ibView = indexBuffers[i]->View();  // ���̃��b�V���ɑΉ����钸�_�o�b�t�@

		commandList->SetGraphicsRootSignature(rootSignature->Get());
		commandList->SetPipelineState(pipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView);

		commandList->SetDescriptorHeaps(1, &materialHeap);
		commandList->SetGraphicsRootDescriptorTable(1, materialHandles[i]->HandleGPU);

		commandList->DrawIndexedInstanced(meshes[i].Indices.size(), 1, 0, 0, 0); // �C���f�b�N�X�̐����`��
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

		// 3D���f������]�A�ړ�
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

		// 3D���f�������̈ʒu��
		ptr->World = translationMatrix;
	}
}
