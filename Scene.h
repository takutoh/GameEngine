#pragma once

class Scene
{
public:
	bool Init(); // 初期化

	void Update(); // 更新処理
	void Draw();   // 描画処理

private:
	float rotateY = 0.0f; // Y軸回転の角度を表す変数
};

extern Scene* g_Scene;
