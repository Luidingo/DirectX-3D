#include "Framework.h"
#include "ParticleScene.h"

ParticleScene::ParticleScene()
{
	collider = new SphereCollider(30); // �׽�Ʈ�� �浹ü

	//particle = new Spark(L"Textures/Effect/star.png" , true);
	//particle = new Rain(); // �� ����
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
	if (KEY_DOWN(VK_LBUTTON)) // ��Ŭ��
	{
		Ray ray = CAM->ScreenPointToRay(mousePos);
		Contact contact;

		if (collider->IsRayCollision(ray, &contact)) // ���콺 Ŀ���� ��ü�� ������
		{
			particle->Play(contact.hitPoint); // �ű⼭ ��ƼŬ ���
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
