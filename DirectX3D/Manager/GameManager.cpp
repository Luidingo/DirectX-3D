#include "Framework.h"

#include "Scenes/CubeScene.h"
#include "Scenes/GridScene.h"
#include "Scenes/SphereScene.h"
#include "Scenes/TerrainEditorScene.h"
#include "Scenes/CollisionScene.h"
#include "Scenes/CubeFieldScene.h"
#include "Scenes/ModelExportScene.h" // 추출 후에 출력하므로 필수는 아니지만 익스포트 씬을 렌더 씬보다 먼저 불러옴
#include "Scenes/ModelRenderScene.h"
#include "Scenes/ModelAnimationScene.h"

// 주말의 과제
 
// 1. 복습 (쉬움) : 프레임워크 폴더에 있는 애니메이션 파일과 모델 데이터를 재생시켜 보기 
//                 더불어서 조작키에 따라 (WASD, 스페이스 등) 캐릭터가 동작을 바꾸게 해보기
//                 (예 : 달리고 점프하는 휴먼, 다른 동작을 하는 나루토 등...)
//                 * 이 과정에서 동작만 바뀌면 됨. 이동, 점프 등은 안해도 됨
//                 ** 다만 점프는 일정 시간(혹은 일정 조건) 하에 다른 동작(Idle 등)으로 바뀌면 좋을 것 같음

// 2. 데이터 추출 도전 (보통, 혹은 어려움) : 애니메이션 파일을 더 구해서 직접 추출과 구현을 해보기

GameManager::GameManager()
{
    Create();

    SceneManager::Get()->Create("Grid", new GridScene());
    SceneManager::Get()->Create("ModelExport", new ModelExportScene());

    //SceneManager::Get()->Create("Cube", new CubeScene());
    //SceneManager::Get()->Create("Sphere", new SphereScene());
    //SceneManager::Get()->Create("Terrain", new TerrainEditorScene());
    //SceneManager::Get()->Create("Collision", new CollisionScene());
    //SceneManager::Get()->Create("CubeField", new CubeFieldScene());
    //SceneManager::Get()->Create("ModelRender", new ModelRenderScene());
    SceneManager::Get()->Create("ModelAnimation", new ModelAnimationScene());

    SceneManager::Get()->Add("Grid");
    SceneManager::Get()->Add("ModelExport");

    //SceneManager::Get()->Add("Terrain");
    SceneManager::Get()->Add("ModelAnimation");
}

GameManager::~GameManager()
{
    Delete();
}

void GameManager::Update()
{
    Keyboard::Get()->Update();
    Timer::Get()->Update();    

    SceneManager::Get()->Update();

    Environment::Get()->Update();
}

void GameManager::Render()
{
    SceneManager::Get()->PreRender();
    
    Device::Get()->Clear();
    Font::Get()->GetDC()->BeginDraw();
    
    Environment::Get()->Set();    
    SceneManager::Get()->Render();
    
    Environment::Get()->PostSet();
    SceneManager::Get()->PostRender();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    string fps = "FPS : " + to_string(Timer::Get()->GetFPS());
    Font::Get()->RenderText(fps, { 100, WIN_HEIGHT - 10 });
    
    static bool isActive = true;

    if (isActive)
    {
        ImGui::Begin("Inspector", &isActive);
        Environment::Get()->GUIRender();
        SceneManager::Get()->GUIRender();
        ImGui::End();
    }    

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    
    Font::Get()->GetDC()->EndDraw();

    Device::Get()->Present();
}

void GameManager::Create()
{
    Keyboard::Get();
    Timer::Get();
    Device::Get();
    Environment::Get();
    Observer::Get();
    
    Font::Get()->AddColor("White", 1, 1, 1);
    Font::Get()->AddStyle("Default", L"배달의민족 주아");
    
    Font::Get()->SetColor("White");
    Font::Get()->SetStyle("Default");

    Texture::Add(L"Textures/Color/White.png");
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(DEVICE, DC);
}

void GameManager::Delete()
{
    Keyboard::Delete();
    Timer::Delete();
    Device::Delete();
    Shader::Delete();
    Texture::Delete();
    Environment::Delete();    
    Observer::Delete();
    Font::Delete();
    
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    
    ImGui::DestroyContext();
}
