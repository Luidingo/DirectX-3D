#include "Framework.h"
#include "ParticleScene.h"

ParticleScene::ParticleScene()
{
	collider = new SphereCollider(30); // 테스트용 충돌체

	//particle = new Spark(L"Textures/Effect/star.png" , true);
	//particle = new Rain(); // 비 생성
	particle = new Sprite(L"Textures/Effect/explosion.png", 10.f, 10.f, 5, 3);
	//particle = new Sprite(L"Textures/Effect/fire_8x2.png", 5.f, 20.f, 8, 2, true);
}

ParticleScene::~ParticleScene()
{
	delete particle;
	delete collider;
}

void ParticleScene::Update()
{
	if (KEY_DOWN(VK_LBUTTON)) // 좌클릭
	{
		Ray ray = CAM->ScreenPointToRay(mousePos);
		Contact contact;

		if (collider->IsRayCollision(ray, &contact)) // 마우스 커서가 구체에 닿으면
		{
			particle->Play(contact.hitPoint); // 거기서 파티클 재생
		}
	}
	collider->UpdateWorld();
	particle->Update();
}

void ParticleScene::PreRender()
{
}

void ParticleScene::Render()
{
	collider->Render();
	particle->Render();
}

void ParticleScene::PostRender()
{
}

void ParticleScene::GUIRender()
{
	collider->GUIRender();
	particle->GUIRender();
}
