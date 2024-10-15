#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Scene
{
public:
	bool Init();   // ������
	void Update(); // �X�V����
	void Draw();   // �`�揈��

	XMFLOAT3 cameraPosition = { 0.0f, 100.0f, 200.0f };
	XMFLOAT3 cameraRotation = { 0.0f, 180.0f, 0.0f };
};

extern Scene* g_Scene;
