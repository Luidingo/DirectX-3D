#include "Framework.h"
#include "TrailScene.h"

TrailScene::TrailScene()
{
	human = new Human();
}

TrailScene::~TrailScene()
{
	delete human;
}

void TrailScene::Update()
{
	human->Update();
}

void TrailScene::PreRender()
{
}

void TrailScene::Render()
{
	human->Render();
}

void TrailScene::PostRender()
{
}

void TrailScene::GUIRender()
{
	human->GUIRender();
}
