#include "Framework.h"
#include "InstancingAScene.h"

InstancingAScene::InstancingAScene()
{
	// 모델 인스턴싱 하나 만들기
	models = new ModelAnimatorInstancing("Naruto");

	models->ReadClip("Idle");		// 대기 모션
	models->ReadClip("Punching");	// 공격 모션
	models->ReadClip("Throw");		// 던지기 모션

	// 헤더에서 만든 크기(SIZE)만큼 인스턴스 추가로 만들기
	for (float z = 0; z < HEIGHT; z++)
	{
		for (float x = 0; x < WIDTH; x++)
		{
			Transform* transform = models->Add();
			transform->Pos() = { x, 0, z };
			transform->Scale() *= 0.01f; // 크기를 1%로 줄이기
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
	ImGui::SliderInt("ID", &instanceID, 0, WIDTH * HEIGHT);	// 인스턴스 ID
	ImGui::SliderInt("Clip", &clip, 0, 2);					// 수행할 동작

	if (ImGui::Button("Play")) // 버튼 생성하기 + 만약 이 버튼을 눌렀으면
	{
		models->PlayClip(instanceID, clip); // ImGui 패널로 설정해둔 id에 설정해둔 동작 수행시키기
	}
	models->GUIRender(); // 각 모델 렌더 따로
}
