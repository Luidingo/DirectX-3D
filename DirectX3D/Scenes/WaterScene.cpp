#include "Framework.h"
#include "WaterScene.h"

WaterScene::WaterScene()
{
	skyBox = new SkyBox(L"Textures/Landscape/BlueSky.dds");
	forest = new Model("Forest");
	forest->Pos() += { 0, -10, 0 }; // ��� ��ġ ���� (�ʼ� X)

	human = new Human();

	water = new Water(L"Textures/Landscape/Wave.dds", 500, 500);
}

WaterScene::~WaterScene()
{
	delete skyBox;
	delete forest;
	delete human;
	delete water;
}

void WaterScene::Update()
{
	forest->UpdateWorld();
	human->Update();
	water->Update();
}

void WaterScene::PreRender()
{
	// ���� Ÿ�� ���� ����� �� ����� ����ϱ�

	// �Ϸ���
	water->SetRefraction();

	// �Ϸ��� �� Ŭ������ ���忡 �Ϸ����� ��� ���
	skyBox->Render();
	forest->Render();
	human->Render();

	// �ݻ�
	water->SetReflection();

	// �ݻ� ���
	skyBox->Render();
	forest->Render();
	human->Render();
}

void WaterScene::Render()
{
	skyBox->Render();
	forest->Render();
	human->Render();
	water->Render();
}

void WaterScene::PostRender()
{
	
}

void WaterScene::GUIRender()
{
	human->GUIRender();
	water->GUIRender();
}
