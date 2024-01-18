#include "Framework.h"

#include "Scenes/CubeScene.h"
#include "Scenes/GridScene.h"
#include "Scenes/SphereScene.h"
#include "Scenes/TerrainEditorScene.h"
#include "Scenes/CollisionScene.h"
#include "Scenes/CubeFieldScene.h"
#include "Scenes/ModelExportScene.h" // 추출 후에 출력하므로 필수는 아니지만 익스포트 씬을 렌더 씬보다 먼저 불러옴
#include "Scenes/ModelRenderScene.h"

// 새 프레임워크 추가요소 소개
// 1. AssImp, Asset Importer 라이브러리 추가
//    -> DX 혹은 VC++과 호환 가능한 외부 리소스 추출 라이브러리
//       그림, 3D모델 등 리소스로 구분 가능한 것들을 개발 환경 안으로 가져올 수 있음
//       : 주로 3D 모델 관련으로 많이 쓴다

// 2. Model, Texture 폴더 안에 강의 진행용 리소스 추가
//    (/FBX : 원형 3D 아트 디자인, /Materials, /Meshes : 추출된 3D 셰이더 데이터 등)

// 그 밖에 셰이더, 정점 구성이 조금 더 추가....

// -> 추가된 요소들을 사용하여 3D 데이터를 앱으로 구현하는 모델링 과정을 시작

// ------------------------------------------------------------------

// 과제 : 이론예습 (스킨드 메쉬가 무엇인지, 키 애니메이션이 무엇인지 조사해서 카페에 올려주세요)
//        형식은 글, 그림, PPT...무엇이든 됩니다. 다운받을 수 있는 파일로만 올려주세요
//        (영상을 곧바로 올리면 다운이 안 될 수 있어서 주의)

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
    SceneManager::Get()->Create("ModelRender", new ModelRenderScene());

    SceneManager::Get()->Add("Grid");
    SceneManager::Get()->Add("ModelExport");

    //SceneManager::Get()->Add("Terrain");
    SceneManager::Get()->Add("ModelRender");
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
