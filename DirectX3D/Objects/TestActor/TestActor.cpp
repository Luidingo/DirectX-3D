#include "Framework.h"

TestActor::TestActor() : SphereCollider(1.0f)
{
    Pos() = { 5, 5, 5 };

    //���ؼ� �߰�
    //crosshair = new Quad(L"Textures/UI/cursor.png"); // ���Ͽ��� ���� ����
    //crosshair->Pos() = { CENTER_X, CENTER_Y }; // "������ ��"�� �߾�����
                                               // �׷��� ������ ����.... UI�� ����Ʈ��� -> ȭ���� ���߾�
    //crosshair->UpdateWorld(); // ��ġ ��ٷ� ����

    prevMousePos = mousePos; // ���콺 ��ġ �ޱ�

    //���� �ִ� ī�޶� ���� �ڵ带 ���� ������ �����Խ��ϴ�. (0111)
    CAM->SetTarget(this); // �׽����� ��� ������ ī�޶� �ֱ� = ǥ�� ����
    CAM->TargetOptionLoad("Follow");

    //Ŭ���̾�Ʈ(���� ��) ������ ȭ��(DC)�� ��������
    //ClientToScreen(hWnd, &clientCenterPos); // �����쿡�� �ٽ� �� �� �����ͼ� ���� ���Ҵ��ϱ� (��, �ٸ���)
    //ShowCursor(false); // �ۿ��� ���콺 ������ �ʰ� �ϱ�

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
    //3D ����� ���� ��, 2D ����Ʈ�� �ؽ�ó ���
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

    //--���� �����
    if (KEY_PRESS('Q'))
        CAM->Rot().y -= rotSpeed * 0.5f * DELTA;

    if (KEY_PRESS('E'))
        CAM->Rot().y += rotSpeed * 0.5f * DELTA;

    if (KEY_PRESS('R'))
        CAM->Rot().x -= rotSpeed * 0.5f * DELTA;

    if (KEY_PRESS('F'))
        CAM->Rot().x += rotSpeed * 0.5f * DELTA;
    //--���� ����� ��

    if (!isJump && KEY_DOWN(VK_SPACE))
    {
        velocity = jumpForce;
        isJump = true;
    }

    Vector3 delta = mousePos - prevMousePos; // ���� �ֱ� ���콺 ��ġ���� ������� ������ ���콺�� ��ȭ�� ���ϱ�
    prevMousePos = mousePos; // ���� �ֱ� ���콺 ��ġ�� ����� ����

    //���콺 ��Ÿ ����
    //Vector3 delta = mousePos - Vector3(CENTER_X, CENTER_Y);
    //        // clientCenterPos�� �� �޾����� CENTER_X, CENTER_Y ���
    //SetCursorPos(clientCenterPos.x, clientCenterPos.y); // ���콺 ��ġ�� ��� �������ϱ�

    //���� ã�Ƴ� ���콺�� �������� ī�޶� ����

    CAM->Rot().y += delta.x * rotSpeed * DELTA;
    CAM->Rot().x -= delta.y * rotSpeed * DELTA;

    //�ڱ� �ڽ��� ȸ���� ī�޶� ���� �ݿ�
    Rot().y = CAM->Rot().y;
    Rot().x = CAM->Rot().x; // x �������� ī�޶󿡼� �ް� �Ǹ� ����ȸ���� ����

    // �̷��� ī�޶� �����̰�, �ش� ī�޶� ���� ĳ���͵� �����̱� ����

    // �� ��� UI�� ���ؼ� ���� �ı�� �ױ⸦ ȣ��

    if (KEY_DOWN(VK_LBUTTON))
        actorUI->Mining();

    if (KEY_DOWN(VK_RBUTTON))
        actorUI->Build();
}

void TestActor::Jump()
{

    velocity -= 9.8f * gravityMult * DELTA;
    Pos().y += velocity * DELTA;

    //float heightLevel = 0; // ���� ���� (����� 0)
    float heightLevel = BlockManager::Get()->GetHeight(Pos());

    if (Pos().y - Radius() < heightLevel) // ���̰� ������ �� �� ������
    {
        Pos().y = heightLevel + Radius();
        velocity = 0;
        isJump = false;
    }

}
