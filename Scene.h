#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Scene
{
public:
	bool Init();   // 初期化
	void Update(); // 更新処理
	void Draw();   // 描画処理
	void Play();   // 再生処理
	void Stop();   // 停止処理

	XMFLOAT3 cameraPosition = { 0.0f, 100.0f, 200.0f };
	XMFLOAT3 cameraRotation = { 0.0f, 180.0f, 0.0f };

	float rotationAngleX = 0.0f;
	float rotationAngleY = 0.0f;
	float rotationAngleZ = 0.0f;

	float speedX = 0.0f;
	float speedY = 0.0f;
	float speedZ = 0.0f;

	float positionX = 0.0f;
	float positionY = 0.0f;
	float positionZ = 0.0f;

	float rotationX = 0.0f;
	float rotationY = 0.0f;
	float rotationZ = 0.0f;
};

extern Scene* g_Scene;
