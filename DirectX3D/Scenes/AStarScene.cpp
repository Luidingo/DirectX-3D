#include "Framework.h"
#include "AStarScene.h"

AStarScene::AStarScene()
{
    terrain = new TerrainEditor();
    aStar = new AStar(20, 20);
    aStar->SetNode(terrain); //지형에 맞추어서 노드 설치

    aStarRobot = new AStar(20, 20);
    aStarRobot->SetNode(terrain); //지형에 맞추어서 노드 설치

    fox = new FoxA();
    fox->SetTerrain(terrain);
    fox->SetAStar(aStar);

    robot = new RobotA();
    robot->Scale() *= 0.05f;
    robot->Pos() = { 30, 0, 0 };
    robot->SetTerrain(terrain);
    robot->SetAStar(aStarRobot);
    robot->SetTarget(fox);
}

AStarScene::~AStarScene()
{
    delete terrain;
    delete aStar;
    delete aStarRobot;
    delete fox;
    delete robot;
}

void AStarScene::Update()
{
    aStar->Update();
    aStarRobot->Update();
    fox->Update();
    robot->Update();
}

void AStarScene::PreRender()
{
}

void AStarScene::Render()
{
    terrain->Render();
    aStar->Render();
    aStarRobot->Render();
    fox->Render();
    robot->Render();
}

void AStarScene::PostRender()
{
}

void AStarScene::GUIRender()
{
}
