#include "Framework.h"

TestActor::TestActor() : SphereCollider(1.0f)
{
    Pos() = { 5, 5, 5 };

    //조준선 추가
    //crosshair = new Quad(L"Textures/UI/cursor.png"); // 파일에서 쿼드 생성
    //crosshair->Pos() = { CENTER_X, CENTER_Y }; // "지정된 곳"의 중앙으로
                                               // 그런데 지정된 곳이.... UI용 뷰포트라면 -> 화면의 정중앙
    //crosshair->UpdateWorld(); // 위치 곧바로 갱신

    prevMousePos = mousePos; // 마우스 위치 받기

    //씬에 있던 카메라 추적 코드를 액터 안으로 가져왔습니다. (0111)
    CAM->SetTarget(this); // 테스터의 행렬 정보를 카메라에 주기 = 표적 설정
    CAM->TargetOptionLoad("Follow");

    //클라이언트(말단 앱) 정보를 화면(DC)에 가져오기
    //ClientToScreen(hWnd, &clientCenterPos); // 윈도우에서 다시 한 번 가져와서 변수 재할당하기 (단, 다르면)
    //ShowCursor(false); // 앱에서 마우스 보이지 않게 하기

    actorUI = new ActorUI();
}

TestActor::~TestActor()
{
    //delete crosshair;
    delete actorUI;
}

void TestActor::Update()
{
    Control();
    Jump();

    UpdateWorld();

    actorUI->Update();
}

void TestActor::Render()
{
    Collider::Render();
}

void TestActor::PostRender()
{
    //3D 출력이 끝난 후, 2D 뷰포트에 텍스처 출력
    //crosshair->Render();
    actorUI->PostRender();
}

void TestActor::GUIRender()
{
    Collider::GUIRender();
    actorUI->GUIRender();
}

void TestActor::Control()
{
    if (KEY_PRESS('W'))
        Pos() += Forward() * moveSpeed * DELTA;

    if (KEY_PRESS('S'))
        Pos() += Back() * moveSpeed * DELTA;

    if (KEY_PRESS('A'))
        Pos() += Left() * moveSpeed * DELTA;

    if (KEY_PRESS('D'))
        Pos() += Right() * moveSpeed * DELTA;

    //--각도 디버그
    if (KEY_PRESS('Q'))
        CAM->Rot().y -= rotSpeed * 0.5f * DELTA;

    if (KEY_PRESS('E'))
        CAM->Rot().y += rotSpeed * 0.5f * DELTA;

    if (KEY_PRESS('R'))
        CAM->Rot().x -= rotSpeed * 0.5f * DELTA;

    if (KEY_PRESS('F'))
        CAM->Rot().x += rotSpeed * 0.5f * DELTA;
    //--각도 디버그 끝

    if (!isJump && KEY_DOWN(VK_SPACE))
    {
        velocity = jumpForce;
        isJump = true;
    }

    Vector3 delta = mousePos - prevMousePos; // 가장 최근 마우스 위치에서 현재까지 움직인 마우스의 변화량 구하기
    prevMousePos = mousePos; // 가장 최근 마우스 위치를 현재로 갱신

    //마우스 델타 재계산
    //Vector3 delta = mousePos - Vector3(CENTER_X, CENTER_Y);
    //        // clientCenterPos가 안 받아지면 CENTER_X, CENTER_Y 사용
    //SetCursorPos(clientCenterPos.x, clientCenterPos.y); // 마우스 위치를 계속 재조정하기

    //지금 찾아낸 마우스의 움직임을 카메라에 전달

    CAM->Rot().y += delta.x * rotSpeed * DELTA;
    CAM->Rot().x -= delta.y * rotSpeed * DELTA;

    //자기 자신의 회전도 카메라 따라 반영
    Rot().y = CAM->Rot().y;
    Rot().x = CAM->Rot().x; // x 각도까지 카메라에서 받게 되면 상하회전도 가능

    // 이렇게 카메라를 움직이고, 해당 카메라에 따라 캐릭터도 움직이기 가능

    // 씬 대신 UI를 통해서 직접 파기와 쌓기를 호출

    if (KEY_DOWN(VK_LBUTTON))
        actorUI->Mining();

    if (KEY_DOWN(VK_RBUTTON))
        actorUI->Build();
}

void TestActor::Jump()
{

    velocity -= 9.8f * gravityMult * DELTA;
    Pos().y += velocity * DELTA;

    //float heightLevel = 0; // 기준 높이 (현재는 0)
    float heightLevel = BlockManager::Get()->GetHeight(Pos());

    if (Pos().y - Radius() < heightLevel) // 높이가 음수가 될 것 같으면
    {
        Pos().y = heightLevel + Radius();
        velocity = 0;
        isJump = false;
    }

}
