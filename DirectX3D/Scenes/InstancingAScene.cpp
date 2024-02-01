#include "Framework.h"
#include "InstancingAScene.h"

InstancingAScene::InstancingAScene()
{
	// �� �ν��Ͻ� �ϳ� �����
	models = new ModelAnimatorInstancing("Naruto");

	models->ReadClip("Idle");		// ��� ���
	models->ReadClip("Punching");	// ���� ���
	models->ReadClip("Throw");		// ������ ���

	// ������� ���� ũ��(SIZE)��ŭ �ν��Ͻ� �߰��� �����
	for (float z = 0; z < HEIGHT; z++)
	{
		for (float x = 0; x < WIDTH; x++)
		{
			Transform* transform = models->Add();
			transform->Pos() = { x, 0, z };
			transform->Scale() *= 0.01f; // ũ�⸦ 1%�� ���̱�
		}
	}
}

InstancingAScene::~InstancingAScene()
{
	delete models;
}

void InstancingAScene::Update()
{
	models->Update();
}

void InstancingAScene::PreRender()
{
}

void InstancingAScene::Render()
{
	models->Render();
}

void InstancingAScene::PostRender()
{
}

void InstancingAScene::GUIRender()
{
	ImGui::SliderInt("ID", &instanceID, 0, WIDTH * HEIGHT);	// �ν��Ͻ� ID
	ImGui::SliderInt("Clip", &clip, 0, 2);					// ������ ����

	if (ImGui::Button("Play")) // ��ư �����ϱ� + ���� �� ��ư�� ��������
	{
		models->PlayClip(instanceID, clip); // ImGui �гη� �����ص� id�� �����ص� ���� �����Ű��
	}
	models->GUIRender(); // �� �� ���� ����
}
