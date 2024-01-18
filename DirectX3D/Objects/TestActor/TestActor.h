#pragma once
class TestActor : public SphereCollider
{
public:

    TestActor();
    ~TestActor();

    void Update();
    void Render();
    void PostRender();
    void GUIRender();

private:
    void Control(); // ��Ʈ�� ���
    void Jump();    // ������ ���� ���

private:

    float gravityMult = 10.0f;
    float moveSpeed = 10.0f;
    float rotSpeed = 10.0f;     //ȸ�� �ӵ�
    float jumpForce = 20.0f;

    float velocity = 0;
    bool isJump = false;

    Vector3 prevMousePos; // ���� �ֱ��� ���콺 ��ġ (����� ��� ���ϱ� ���ؼ�)
                          // -> ���콺�� ��� ���� ������ ������ �����͸� ���� �� ����

    // ���� ���Ϳ� ������ �� : ���� ���̴� ����

    // 1. UI�� ��ȣ�ۿ� : �� �������� ������ ���̰�, ���� ������ �� �� �ִ°�
    // 2. ������ �� : ���� �����ΰ�

    // 2�� ���� �ڿ� > ����� 1�� ���� (UI, ��ȣ�ۿ�, �κ��丮 �� ���� ������)

    // �ӽ� �ܵ� UI : ������ ����
    //Quad* crosshair; // ���ؼ�

    // ������ UI
    ActorUI* actorUI;

    //�ʿ����� ������ ������ ������ ���� �� : ���� �������� (����) ���� ȭ�� �߾� ����
    POINT clientCenterPos = { WIN_WIDTH >> 1, WIN_HEIGHT >> 1 };
};

