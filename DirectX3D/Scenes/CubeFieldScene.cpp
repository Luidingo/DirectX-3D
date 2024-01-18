#include "Framework.h"
#include "CubeFieldScene.h"

CubeFieldScene::CubeFieldScene()
{
    BlockManager::Get()->CreateBlocks(20, 20);
    tester = new TestActor();
}

CubeFieldScene::~CubeFieldScene()
{
}

void CubeFieldScene::Update()
{
    BlockManager::Get()->Push(tester);
    BlockManager::Get()->Update();
    tester->Update();
}

void CubeFieldScene::PreRender()
{
}

void CubeFieldScene::Render()
{
    BlockManager::Get()->Render();
    tester->Render();
}

void CubeFieldScene::PostRender()
{
    tester->PostRender();
}

void CubeFieldScene::GUIRender()
{
    tester->GUIRender();
}