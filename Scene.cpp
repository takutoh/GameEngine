#include "App.h"
#include "Engine.h"
#include "Scene.h"
#include "d3dx12.h"
#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"

Scene* g_Scene;

using namespace DirectX;

VertexBuffer* vertexBuffer;
ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT];
RootSignature* rootSignature;
PipelineState* pipelineState;

static UINT WINDOW_WIDTH = Application::WINDOW_WIDTH;
static UINT WINDOW_HEIGHT = Application::WINDOW_HEIGHT;

bool Scene::Init()
{
	Vertex vertices[3] = {};
	vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[1].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[2].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

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

	return true;
}

void Scene::Update()
{
	// �A�j���[�V����
	rotateY += 0.02f;
	auto currentIndex = g_Engine->CurrentBackBufferIndex();                    // ���݂̃t���[���ԍ��̎擾
	auto currentTransform = constantBuffer[currentIndex]->GetPtr<Transform>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@�̎擾
	currentTransform->World = DirectX::XMMatrixRotationY(rotateY);             // Y���ŉ�]
}

void Scene::Draw()
{
	auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ��̎擾
	auto commandList = g_Engine->CommandList();             // �R�}���h���X�g
	auto vbView = vertexBuffer->View();                     // ���_�o�b�t�@�r���[

	commandList->SetGraphicsRootSignature(rootSignature->Get());                                   // ���[�g�V�O�l�`�����Z�b�g
	commandList->SetPipelineState(pipelineState->Get());                                           // �p�C�v���C���X�e�[�g���Z�b�g
	commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress()); // �萔�o�b�t�@���Z�b�g

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`��`�悷��ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);                           // ���_�o�b�t�@�̐ݒ�

	commandList->DrawInstanced(3, 1, 0, 0); // 3�̒��_��`��
}
