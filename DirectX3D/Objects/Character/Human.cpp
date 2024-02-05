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

    // ���� ����ϱ�
    startEdge = new Transform(); // ���� �ϱ�
    endEdge = new Transform();
    trail = new Trail(L"Textures/Effect/Trail.png", startEdge, endEdge, 10, 10.f);
                // ���� ���� (�̹��� ����, ������, ����, �ڸ� �׸� ���� ũ��, ���/���� �ӵ�)

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
    
    // �� ������Ʈ ���� (��� ���� �ݿ��Ǳ� ����) ���� �غ� ����

    startEdge->Pos() = crowbar->GlobalPos() + crowbar->Up() * 20.f; // 20.f : �� ũ��(10%) �ݿ�
    endEdge->Pos() = crowbar->GlobalPos() + crowbar->Down() * 20.f; // Down��ŭ ���ϰų� Up��ŭ ���ų�

    // ã�Ƴ� ������ ��ġ�� ������Ʈ
    startEdge->UpdateWorld();
    endEdge->UpdateWorld();
    // ������Ʈ�ϰ� ���� Ʈ�������� ��ġ�� ���

    // -------------------------------------------------------------------
    ModelAnimator::Update();

    trail->Update();    // ������ �غ��� ���� + ������ �����ϴ� �𵨱��� ������Ʈ ��, ��ü ������Ʈ
}

void Human::Render()
{
    crowbar->Render();
    ModelAnimator::Render();

    trail->Render(); // ���� ����
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
