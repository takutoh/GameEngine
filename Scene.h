#pragma once

class Scene
{
public:
	bool Init(); // ������

	void Update(); // �X�V����
	void Draw();   // �`�揈��

private:
	float rotateY = 0.0f; // Y����]�̊p�x��\���ϐ�
};

extern Scene* g_Scene;
