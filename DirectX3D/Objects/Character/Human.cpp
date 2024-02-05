#include "Framework.h"
#include "Human.h"

Human::Human() : ModelAnimator("Human")
{
    ReadClip("Idle", 1);
    ReadClip("Run", 1);
    ReadClip("Jump", 1);

    action = (ACTION)frameBuffer->Get().cur.clip;

    crowbar = new Crowbar();
    crowbar->Load();

    mainHand = new Transform();
    crowbar->SetParent(mainHand);

    Scale() *= 0.1f;

    // 궤적 사용하기
    startEdge = new Transform(); // 선언만 하기
    endEdge = new Transform();
    trail = new Trail(L"Textures/Effect/Trail.png", startEdge, endEdge, 10, 10.f);
                // 궤적 생성 (이미지 파일, 시작점, 종점, 자를 그림 가로 크기, 재생/진행 속도)

    trail->SetActive(trailToggle);
}

Human::~Human()
{
    delete crowbar;
    delete mainHand;
    delete trail;
}

void Human::Update()
{
    ClipSync();
    Control();
    Jump();

    mainHand->SetWorld(GetTransformByNode(51));
    crowbar->Update();

    // -------------------------------------------------------------------
    
    // 모델 업데이트 전에 (모든 것이 반영되기 전에) 궤적 준비 진행

    startEdge->Pos() = crowbar->GlobalPos() + crowbar->Up() * 20.f; // 20.f : 모델 크기(10%) 반영
    endEdge->Pos() = crowbar->GlobalPos() + crowbar->Down() * 20.f; // Down만큼 더하거나 Up만큼 빼거나

    // 찾아낸 꼭짓점 위치를 업데이트
    startEdge->UpdateWorld();
    endEdge->UpdateWorld();
    // 업데이트하고 나면 트랜스폼에 위치가 담김

    // -------------------------------------------------------------------
    ModelAnimator::Update();

    trail->Update();    // 위에서 준비한 궤적 + 궤적을 진행하는 모델까지 업데이트 후, 본체 업데이트
}

void Human::Render()
{
    crowbar->Render();
    ModelAnimator::Render();

    trail->Render(); // 궤적 렌더
}

void Human::GUIRender()
{
    crowbar->GUIRender();
    ModelAnimator::GUIRender();
}

void Human::ClipSync()
{
    if ((ACTION)frameBuffer->Get().cur.clip != action)
    {
        PlayClip((int)action);
    }
}

void Human::Control()
{
    if (!isJump)
    {
        if (KEY_PRESS('W'))
        {
            action = ACTION::RUN;
        }

        if (KEY_UP('W'))
        {
            action = ACTION::IDLE;
        }

        if (KEY_DOWN(VK_SPACE))
        {
            action = ACTION::JUMP;
            jumpVelocity = jumpForce;
            isJump = true;
        }

        if (KEY_DOWN('T'))
        {
            trailToggle = !trailToggle;
            trail->SetActive(trailToggle);
        }
    }
}

void Human::Jump()
{
    jumpVelocity -= 9.8f * gravityMult * DELTA;
    Pos().y += jumpVelocity;

    if (Pos().y > 0)
    {
        if (action != ACTION::JUMP) action = ACTION::JUMP;
        isJump = true;
    }

    if (Pos().y < 0)
    {
        Pos().y = 0;
        jumpVelocity = 0;
        if (action == ACTION::JUMP) action = ACTION::IDLE;
        isJump = false;
    }
}
