#include "Framework.h"
#include "ModelRenderScene.h"

ModelRenderScene::ModelRenderScene()
{
	model = new Model("Fox");
}

ModelRenderScene::~ModelRenderScene()
{
	delete model;
}

void ModelRenderScene::Update()
{
	model->UpdateWorld(); // Ʈ������ �����͸� ���� ��ü�μ� ������Ʈ
}

void ModelRenderScene::PreRender()
{
}

void ModelRenderScene::Render()
{
	model->Render();
}

void ModelRenderScene::PostRender()
{
}

void ModelRenderScene::GUIRender()
{
	model->GUIRender();
}
