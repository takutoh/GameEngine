#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Scene
{
public:
	bool Init();   // ‰Šú‰»
	void Update(); // XVˆ—
	void Draw();   // •`‰æˆ—
	void Play();   // Ä¶ˆ—
	void Stop();   // ’â~ˆ—

	XMFLOAT3 cameraPosition = { 0.0f, 100.0f, 200.0f };
	XMFLOAT3 cameraRotation = { 0.0f, 180.0f, 0.0f };

	float rotationAngleX = 0.0f;
	float rotationAngleY = 0.0f;
	float rotationAngleZ = 0.0f;

	float speedX = 0.0f;
	float speedY = 0.0f;
	float speedZ = 0.0f;
};

extern Scene* g_Scene;
