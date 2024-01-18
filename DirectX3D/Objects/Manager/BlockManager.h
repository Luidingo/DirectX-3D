#pragma once
class BlockManager : public Singleton<BlockManager>
{
public:
    friend class Singleton;

    BlockManager() = default;
    ~BlockManager();

    void Update();
    void Render();

    void CreateBlocks(UINT width, UINT height); //���� ���θ� �޾� ������ ��ϵ� ����

    float GetHeight(Vector3& pos); // ���� �ޱ�

    Cube* GetCollisionBlock(); // �浹�� ť�갡 �ִ���(�ִٸ� ��������) �˻�

    bool BuildBlock(Cube* block); // ����� �Ű������� ���� ��� �ִ� ��Ͽ� �߰��ؼ� �ױ�

    void Push(Collider* collider); // �浹ü�� �Ű������� ������ �浹 ��� ������ �ٸ� �� ���� �о��
    // -> 1. ����� "����"���� Ȱ�밡�� (��ֹ�, �� ��) - ������ �켱 ����
    // -> 2. ����� ��ȣ�ۿ� ����̶�� ���"��" �̴� �͵� ����������

private:
    list<Cube*> blocks; // <-���� �ƴ�.
    // ť�꿡 �浹ü ���

};

