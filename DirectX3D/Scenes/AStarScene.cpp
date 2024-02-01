#include "Framework.h"
#include "AStarScene.h"

AStarScene::AStarScene()
{
	terrain = new Terrain();
	aStar = new AStar(20, 20);
	aStar->SetNode(terrain);

	fox = new FoxA();
	fox->SetTerrain(terrain);
	fox->SetAStar(aStar);
}

AStarScene::~AStarScene()
{
	delete fox;
	delete aStar;
	delete terrain;
}

void AStarScene::Update()
{
	terrain->UpdateWorld();
	aStar->Update();
	fox->Update();
}

void AStarScene::PreRender()
{
}

void AStarScene::Render()
{
	terrain->Render();
	aStar->Render();
	fox->Render();
}

void AStarScene::PostRender()
{
}

void AStarScene::GUIRender()
{
}
