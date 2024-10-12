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
	enum { FRAME_BUFFER_COUNT = 2 }; // ダブルバッファリング

public:
	bool Init(HWND hwnd, UINT windowWidth, UINT windowHeight); // エンジンの初期化

	void BeginRender(); // 描画の開始処理
	void EndRender();   // 描画の終了処理

public:
	ID3D12Device6* Device();
	ID3D12GraphicsCommandList* CommandList();
	UINT CurrentBackBufferIndex();

// DirectX12の初期化に使う関数
private:
	bool CreateDevice();       // デバイスの生成
	bool CreateCommandQueue(); // コマンドキューの生成
	bool CreateSwapChain();    // スワップチェインの生成
	bool CreateCommandList();  // コマンドリストとコマンドアロケーターの生成
	bool CreateFence();        // フェンスの生成
	void CreateViewPort();     // ビューポートの生成
	void CreateScissorRect();  // シザー矩形の生成

// DirectX12の描画に使うオブジェクト
private:
	HWND m_hWnd;
	UINT m_FrameBufferWidth = 0;
	UINT m_FrameBufferHeight = 0;
	UINT m_CurrentBackBufferIndex = 0;

	ComPtr<ID3D12Device6> m_pDevice = nullptr;                                     // デバイス
	ComPtr<ID3D12CommandQueue> m_pQueue = nullptr;                                 // コマンドキュー
	ComPtr<IDXGISwapChain3> m_pSwapChain = nullptr;                                // スワップチェイン
	ComPtr<ID3D12CommandAllocator> m_pAllocator[FRAME_BUFFER_COUNT] = { nullptr }; // コマンドアロケーター
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList = nullptr;                    // コマンドリスト
	HANDLE m_fenceEvent = nullptr;                                                 // フェンスで使うイベント
	ComPtr<ID3D12Fence> m_pFence = nullptr;                                        // フェンス
	UINT64 m_fenceValue[FRAME_BUFFER_COUNT];                                       // フェンスの値
	D3D12_VIEWPORT m_Viewport;                                                     // ビューポート
	D3D12_RECT m_Scissor;                                                          // シザー矩形

// 描画に使うオブジェクトとその生成関数
private:
	bool CreateRenderTarget(); // レンダーターゲットの生成
	bool CreateDepthStencil(); // 深度ステンシルバッファの生成

	UINT m_RtvDescriptorSize = 0;                                              // レンダーターゲットビューのディスクリプタサイズ
	ComPtr<ID3D12DescriptorHeap> m_pRtvHeap = nullptr;                         // レンダーターゲットのディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pRenderTargets[FRAME_BUFFER_COUNT] = { nullptr }; // レンダーターゲット

	UINT m_DsvDescriptorSize = 0;                           // 深度ステンシルのディスクリプターサイズ
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap = nullptr;      // 深度ステンシルのディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pDepthStencilBuffer = nullptr; // 深度ステンシルバッファ

// 描画ループで使用
private:
	ID3D12Resource* m_currentRenderTarget = nullptr; // 現在のフレームのレンダーターゲットを一時的に保存
	void WaitRender();                               // 描画完了を待つ処理
};

extern Engine* g_Engine;
