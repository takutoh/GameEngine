#include "Gui.h"
#include "Engine.h"
#include "Scene.h"
#include <tchar.h>

static int const NUM_FRAMES_IN_FLIGHT = 3;

ID3D12Device* g_pd3dDevice = nullptr;
ID3D12DescriptorHeap* g_pd3dSrvDescHeap = nullptr;
Gui* g_Gui = nullptr;

extern Scene* g_Scene;
extern Engine* g_Engine;

bool Gui::Init(HWND hwnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    // Direct3D�f�o�C�X�̐���
    IDXGIFactory1* factory = nullptr;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        return false;
    }

    IDXGIAdapter1* adapter = nullptr;
    hr = factory->EnumAdapters1(0, &adapter);
    if (FAILED(hr)) {
        return false;
    }

    hr = D3D12CreateDevice(
        adapter,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&g_pd3dDevice)
    );

    if (FAILED(hr)) {
        return false;
    }

    // �f�X�N���v�^�q�[�v�̐���
    D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
    descHeapDesc.NumDescriptors = 1;
    descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    hr = g_pd3dDevice->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&g_pd3dSrvDescHeap));
    if (FAILED(hr)) {
        return false;
    }

    // ImGui�̏�����
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    return true;
}

void Gui::Update()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Gui::Draw()
{
    // �Đ�����E�B�W�F�b�g
    ImGui::Begin("Play");

    if (ImGui::Button(isPlaying ? "Stop" : "Play")) {
        isPlaying = !isPlaying;
    }

    ImGui::End();

    // �J�����̈ʒu�Ǝ��_�A�w�i�F��ύX����E�B�W�F�b�g
    ImGui::Begin("Camera");

    ImGui::Text("Position");
    ImGui::InputFloat("X##Position", &g_Scene->cameraPosition.x);
    ImGui::InputFloat("Y##Position", &g_Scene->cameraPosition.y);
    ImGui::InputFloat("Z##Position", &g_Scene->cameraPosition.z);

    ImGui::Text("Rotation");
    ImGui::InputFloat("X##Rotation", &g_Scene->cameraRotation.x);
    ImGui::InputFloat("Y##Rotation", &g_Scene->cameraRotation.y);
    ImGui::InputFloat("Z##Rotation", &g_Scene->cameraRotation.z);

    ImGui::Text("Background");
    ImGui::ColorEdit4("", g_Engine->m_ClearColor);

    ImGui::End();

    // 3D���f���̃E�B�W�F�b�g
    ImGui::Begin("Alicia");

    ImGui::Text("Position");
    ImGui::InputFloat("X##Position", &g_Scene->positionX);
    ImGui::InputFloat("Y##Position", &g_Scene->positionY);
    ImGui::InputFloat("Z##Position", &g_Scene->positionZ);

    ImGui::Text("Rotate");
    ImGui::InputFloat("X##Rotate", &g_Scene->speedX);
    ImGui::InputFloat("Y##Rotate", &g_Scene->speedY);
    ImGui::InputFloat("Z##Rotate", &g_Scene->speedZ);

    ImGui::End();

    // �t���[�����[�g��\������E�B�W�F�b�g
    ImGui::Begin("Information");

    ImGuiIO& io = ImGui::GetIO();
    float fps = io.Framerate;
    ImGui::Text("FPS = %.1f", fps);

    ImGui::End();

    ID3D12GraphicsCommandList* commandList = g_Engine->CommandList();

    ImGui::Render();

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void Gui::End()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
