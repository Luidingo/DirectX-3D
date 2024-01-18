#pragma once
class BlockManager : public Singleton<BlockManager>
{
public:
    friend class Singleton;

    BlockManager() = default;
    ~BlockManager();

    void Update();
    void Render();

    void CreateBlocks(UINT width, UINT height); //가로 세로를 받아 나열된 블록들 생성

    float GetHeight(Vector3& pos); // 높이 받기

    Cube* GetCollisionBlock(); // 충돌한 큐브가 있는지(있다면 무엇인지) 검사

    bool BuildBlock(Cube* block); // 블록을 매개변수로 넣을 경우 있던 블록에 추가해서 쌓기

    void Push(Collider* collider); // 충돌체를 매개변수로 넣으면 충돌 결과 한쪽이 다른 한 쪽을 밀어내기
    // -> 1. 블록을 "지형"으로 활용가능 (장애물, 땅 등) - 이쪽을 우선 구현
    // -> 2. 블록이 상호작용 대상이라면 블록"을" 미는 것도 가능할지도

private:
    list<Cube*> blocks; // <-벡터 아님.
    // 큐브에 충돌체 사용

};

