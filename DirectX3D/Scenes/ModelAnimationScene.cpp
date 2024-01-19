#include "Framework.h"
#include "ModelAnimationScene.h"

ModelAnimationScene::ModelAnimationScene()
{
	//model = new ModelAnimator("Fox");
	//model->ReadClip("Fox_Run");

	//model = new ModelAnimator("Human"); 
	//model->ReadClip("Idle");
	//model->ReadClip("Run");
	//model->ReadClip("Jump");

	// �� �ڵ�� ������ ���� �������� ����
	// ���� : �ΰ� .fbx ���Ͽ� ������ 0���� �ƴ� 1���� ���� ������
	//		  (������ ���� ������� ���� 0-1, 1-1...������ ������ �ִ�)
	//		  �̷� ������ ������ ������ ���� ���� ������ (���� Ȯ�� �ʿ�) �ڿ� ���� ����
	//		  �Ű������� �����־�� ��
	// 
	//model->ReadClip("Idle", 1); // ������ 0 �ڿ� 1�� ����
	//model->ReadClip("Run", 1);
	//model->ReadClip("Jump", 1);

	//------------------------------------------------------------

	model = new ModelAnimator("Naruto");
	model->ReadClip("Idle");
	model->ReadClip("Run");
	model->ReadClip("Throw");
}

ModelAnimationScene::~ModelAnimationScene()
{
	delete model;
}

void ModelAnimationScene::Update()
{
	model->Update();
}

void ModelAnimationScene::PreRender()
{
}

void ModelAnimationScene::Render()
{
	model->Render();
}

void ModelAnimationScene::PostRender()
{
}

void ModelAnimationScene::GUIRender()
{
	model->GUIRender();
}
