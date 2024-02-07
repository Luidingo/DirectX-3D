#include "Framework.h"
#include "InstancingMScene.h"

InstancingMScene::InstancingMScene()
{
	// ���ʷ� �ϳ� �����
	models = new ModelInstancing("Fox");

	// ���μ��� SIZE��ŭ ������
	for (float z = 0; z < SIZE; z++)
	{
		for (float x = 0; x < SIZE; x++)
		{
			Transform* transform = models->Add();
			transform->Pos() = { x, 0, z };
			transform->Scale() *= 0.1f; // ���� ũ���� 1/100
		}
	}
}

InstancingMScene::~InstancingMScene()
{
	delete models;
}

void InstancingMScene::Update()
{
	models->Update();
}

void InstancingMScene::PreRender()
{
}

void InstancingMScene::Render()
{
	models->Render();
}

void InstancingMScene::PostRender()
{
}

void InstancingMScene::GUIRender()
{
	ImGui::SliderInt("ID", &instanceID, 0, 100);

	models->GUIRender();
}
