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
    void Control(); // 컨트롤 담당
    void Jump();    // 점프와 낙하 담당

private:

    float gravityMult = 10.0f;
    float moveSpeed = 10.0f;
    float rotSpeed = 10.0f;     //회전 속도
    float jumpForce = 20.0f;

    float velocity = 0;
    bool isJump = false;

    Vector3 prevMousePos; // 가장 최근의 마우스 위치 (현재와 계속 비교하기 위해서)
                          // -> 마우스가 어디서 어디로 갔는지 움직임 데이터를 만들어낼 수 있음

    // 지금 액터에 부족한 것 : 눈에 보이는 형태

    // 1. UI와 상호작용 : 내 시점에서 무엇이 보이고, 나는 무엇을 할 수 있는가
    // 2. 액터의 모델 : 나는 무엇인가

    // 2는 조금 뒤에 > 현재는 1에 집중 (UI, 상호작용, 인벤토리 등 내부 데이터)

    // 임시 단독 UI : 다음에 삭제
    //Quad* crosshair; // 조준선

    // 액터의 UI
    ActorUI* actorUI;

    //필요하진 않지만 있으면 적당히 좋은 것 : 현재 실행중인 (말단) 앱의 화면 중앙 검출
    POINT clientCenterPos = { WIN_WIDTH >> 1, WIN_HEIGHT >> 1 };
};

