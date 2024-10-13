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
	// ���_��4�ɂ��Ďl�p�`���`
	Vertex vertices[4] = {};
	vertices[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[3].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	// ���_�o�b�t�@�̐���
	auto vertexSize = sizeof(Vertex) * std::size(vertices);
	auto vertexStride = sizeof(Vertex);
	vertexBuffer = new VertexBuffer(vertexSize, vertexStride, vertices);
	if (!vertexBuffer->IsValid())
	{
		return false;
	}

	// �ϊ��s��̐���
	auto eyePos = XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);                                  // ���_�̈ʒu
	auto targetPos = XMVectorZero();                                                    // ���_����������W
	auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);                                  // �������\���x�N�g��
	constexpr auto fov = XMConvertToRadians(37.5);                                      // ����p
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
		ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
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

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // ����ɏ�����Ă��鏇���ŕ`��

	// �C���f�b�N�X�o�b�t�@�̐���
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
	auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ��̎擾
	auto commandList = g_Engine->CommandList();             // �R�}���h���X�g
	auto vbView = vertexBuffer->View();                     // ���_�o�b�t�@�r���[
	auto ibView = indexBuffer->View();                      // �C���f�b�N�X�o�b�t�@�r���[

	commandList->SetGraphicsRootSignature(rootSignature->Get());                                   // ���[�g�V�O�l�`�����Z�b�g
	commandList->SetPipelineState(pipelineState->Get());                                           // �p�C�v���C���X�e�[�g���Z�b�g
	commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress()); // �萔�o�b�t�@���Z�b�g

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView);                                   // �C���f�b�N�X�o�b�t�@���Z�b�g

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 6�̃C���f�b�N�X�ŕ`��
}
