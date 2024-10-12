#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include "ComPtr.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class Engine
{
public:
	enum { FRAME_BUFFER_COUNT = 2 }; // �_�u���o�b�t�@�����O

public:
	bool Init(HWND hwnd, UINT windowWidth, UINT windowHeight); // �G���W���̏�����

	void BeginRender(); // �`��̊J�n����
	void EndRender();   // �`��̏I������

public:
	ID3D12Device6* Device();
	ID3D12GraphicsCommandList* CommandList();
	UINT CurrentBackBufferIndex();

// DirectX12�̏������Ɏg���֐�
private:
	bool CreateDevice();       // �f�o�C�X�̐���
	bool CreateCommandQueue(); // �R�}���h�L���[�̐���
	bool CreateSwapChain();    // �X���b�v�`�F�C���̐���
	bool CreateCommandList();  // �R�}���h���X�g�ƃR�}���h�A���P�[�^�[�̐���
	bool CreateFence();        // �t�F���X�̐���
	void CreateViewPort();     // �r���[�|�[�g�̐���
	void CreateScissorRect();  // �V�U�[��`�̐���

// DirectX12�̕`��Ɏg���I�u�W�F�N�g
private:
	HWND m_hWnd;
	UINT m_FrameBufferWidth = 0;
	UINT m_FrameBufferHeight = 0;
	UINT m_CurrentBackBufferIndex = 0;

	ComPtr<ID3D12Device6> m_pDevice = nullptr;                                     // �f�o�C�X
	ComPtr<ID3D12CommandQueue> m_pQueue = nullptr;                                 // �R�}���h�L���[
	ComPtr<IDXGISwapChain3> m_pSwapChain = nullptr;                                // �X���b�v�`�F�C��
	ComPtr<ID3D12CommandAllocator> m_pAllocator[FRAME_BUFFER_COUNT] = { nullptr }; // �R�}���h�A���P�[�^�[
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList = nullptr;                    // �R�}���h���X�g
	HANDLE m_fenceEvent = nullptr;                                                 // �t�F���X�Ŏg���C�x���g
	ComPtr<ID3D12Fence> m_pFence = nullptr;                                        // �t�F���X
	UINT64 m_fenceValue[FRAME_BUFFER_COUNT];                                       // �t�F���X�̒l
	D3D12_VIEWPORT m_Viewport;                                                     // �r���[�|�[�g
	D3D12_RECT m_Scissor;                                                          // �V�U�[��`

// �`��Ɏg���I�u�W�F�N�g�Ƃ��̐����֐�
private:
	bool CreateRenderTarget(); // �����_�[�^�[�Q�b�g�̐���
	bool CreateDepthStencil(); // �[�x�X�e���V���o�b�t�@�̐���

	UINT m_RtvDescriptorSize = 0;                                              // �����_�[�^�[�Q�b�g�r���[�̃f�B�X�N���v�^�T�C�Y
	ComPtr<ID3D12DescriptorHeap> m_pRtvHeap = nullptr;                         // �����_�[�^�[�Q�b�g�̃f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12Resource> m_pRenderTargets[FRAME_BUFFER_COUNT] = { nullptr }; // �����_�[�^�[�Q�b�g

	UINT m_DsvDescriptorSize = 0;                           // �[�x�X�e���V���̃f�B�X�N���v�^�[�T�C�Y
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap = nullptr;      // �[�x�X�e���V���̃f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12Resource> m_pDepthStencilBuffer = nullptr; // �[�x�X�e���V���o�b�t�@

// �`�惋�[�v�Ŏg�p
private:
	ID3D12Resource* m_currentRenderTarget = nullptr; // ���݂̃t���[���̃����_�[�^�[�Q�b�g���ꎞ�I�ɕۑ�
	void WaitRender();                               // �`�抮����҂���
};

extern Engine* g_Engine;
