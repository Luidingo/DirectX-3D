#include "Framework.h"
#include "BlockManager.h"

BlockManager::~BlockManager()
{
    for (Cube* block : blocks)
        delete block;

    blocks.clear(); // �ڱ����. �ʼ��� �ƴ�����
}

void BlockManager::Update()
{
    for (Cube* block : blocks)
        block->Update();
}

void BlockManager::Render()
{
    for (Cube* block : blocks)
        block->Render();
}

void BlockManager::CreateBlocks(UINT width, UINT height)
{
    //�ؽ�ó(�� ����) ����
    vector<wstring> textures(5);

    for (UINT i = 0; i < 5; ++i)
    {
        textures[i] = L"Textures/Block/block" + to_wstring(i) + L".png";
    }

    for (UINT z = 0; z < height; ++z)
    {
        for (UINT x = 0; x < width; ++x)
        {
            Vector3 pos = { (float)x, 0, (float)z };

            wstring file = textures[Random(0, textures.size())];

            //�������� ���� ���� �̸��� �ؽ�ó�� ���鼭 ���(=ť��) ����
            Cube* block = new Cube();
            block->GetMaterial()->SetDiffuseMap(file);
            block->Pos() = pos;
            block->Update(); // ���� ������Ʈ (���� �����ϰ� ���� ���̰�)

            blocks.push_back(block);
        }
    }
    //������� ���� ��� �����ؼ� �����ϱ� �Ϸ�
    //�� ������ ��� ������ �� �Ʒ� �ۼ� (������ ����)
}

float BlockManager::GetHeight(Vector3& pos)
{
    // ���� ����� ���� (�� ��, ��� ��ü�� ���ο� ���� �޶��� ��)

    Ray ray(pos, Vector3::Down()); //�Ű������� ���� ������ �� �Ʒ��� �������� ����
    Contact contact; //��������

    float currentMaxHeight = 0;

    for (Cube* block : blocks)
    {
        if (block->GetCollider()->IsRayCollision(ray, &contact))
        {
            if (contact.hitPoint.y > currentMaxHeight)
            {
                currentMaxHeight = contact.hitPoint.y;
            }
        }
    }
    //������� ���� �� ����� ����� ��
    // 1. ��� ���� ��ü�� �־��ٸ� -> ���� ���� ���� ���� �ִ� ����� ���� �����
    // 2. ��� �ۿ� ��ü�� �־��ٸ� -> �ʱⰪ 0�� ����

    // ��� ���̵�, �� ����� ��ȯ
    return currentMaxHeight;
}

Cube* BlockManager::GetCollisionBlock()
{
    // ȭ�鿡��, Ȥ�� ĳ������ �þ߿� ���ؼ� Ŭ���� �Ͼ�� ��, ���� ��ȣ����� �ۿ��� �Ǵ°�

    // Ray ray(/*��򰡿��� �޾ƿ� ĳ���� ��ġ, �� ĳ������ ���� ����*/);
    Ray ray = CAM->ScreenPointToRay(Vector3(CENTER_X, CENTER_Y)); // ����� ������Ʈ�� ���� ȭ���� �߾��� ��������

    float minDistance = FLT_MAX;
    Contact contact;

    //�ݺ� ��� ������(�ݺ���)�غ�
    list<Cube*>::iterator iter = blocks.begin();
    list<Cube*>::iterator collisionBlock = blocks.end(); // �� ��° �ݺ��ڸ� .end()�� = "����"
    // .end() = �ڷᱸ�� ���� ������ ������ ���ϴ� �� �ƴ϶�, �ش� �������� ��¥ ������
    //          ("�� �ڷδ� ����Ʈ ����")

    for (iter; iter != blocks.end(); ++iter) // for���� ù ��° ���ǰ� �� ��° ������ �ʼ� �ƴ� (�ִ� ���� ����)
    {
        if ((*iter)->GetCollider()->IsRayCollision(ray, &contact))
        {
            // �浹 �ÿ� ��ϵ� �ּ� �Ÿ��� �ٽ� �˻��ؼ� ���� ����
            if (contact.distance < minDistance)
            {
                minDistance = contact.distance;
                collisionBlock = iter; // �浹 ����� ������ ������� ����
            }
        }
    }
    // ������� ���� �� ����� �� �� �ϳ�
    // 1. ��� �� �ϳ��� ���ο� collisionBlock�̴�
    // 2. collisionBlock�� ��� blocks.end()�� = �� ���� "�����Ͱ� ��Ī�� ����� ����"

    if (collisionBlock != blocks.end())
    {
        Cube* block = *collisionBlock; // block�� ���� ������ iter�� �� = ����� ������

        blocks.erase(collisionBlock); // ���� �ڵ� : ��ȣ�ۿ��� ������ �� �Ŵ����� �� ��
        // **�Ŵ����� ���� ��ȣ�ۿ��� �ʿ��ϸ� ���⼭ �߰� �ۼ�

        return block; // ��ǻ�Ϳ� ť�� �����͵� ��ȯ
    }
    else
    {
        return nullptr; // �����ϱ� ��
    }
}

bool BlockManager::BuildBlock(Cube* block)
{
    //�� �Լ��� ���������� ������ �޾Ƽ� ����
    Ray ray = CAM->ScreenPointToRay(Vector3(CENTER_X, CENTER_Y));

    float minDistance = FLT_MAX;
    Vector3 hitPoint;
    Contact contact;

    list<Cube*>::iterator iter = blocks.begin();
    Cube* collisionBlock = nullptr;

    for (iter; iter != blocks.end(); ++iter)
    {
        if ((*iter)->GetCollider()->IsRayCollision(ray, &contact))
        {
            if (contact.distance < minDistance)
            {
                //�浹�� �Ͼ ť�� ã�Ƽ� ����
                minDistance = contact.distance;
                hitPoint = contact.hitPoint;
                collisionBlock = *iter;
            }
        }
    }

    // ������� ���� �� ����� �� �� �ϳ�
    // 1. �Ÿ��� �ּ��� (���� ���ص� ���� �����) ť�갡 ���´� = �������� ���� ť�갡 ���´�
    // 2. �浹�� ť�갡 ������

    // �������� ��ٷ� ������ bool ���� ���� ��ȯ
    if (collisionBlock == nullptr) return false;

    //�浹�� ���� ���� ���� ����
    Vector3 dir = hitPoint - collisionBlock->GlobalPos();

    int maxIndex = 0;
    float maxValue = 0;

    for (int i = 0; i < 3; ++i)
    {
        if (abs(dir[i]) > maxValue) //������ ���� ������ �� �� ��(�� ���밪)�� ��ϵ� �ִ밪�� �Ѿ��
        {
            // �ִ밪�� ���� �������� index, value ���� ���
            maxIndex = i; //x Ȥ�� y Ȥ�� z
            maxValue = abs(dir[i]);
        }
    }

    // ���⼭ �� ���� ������ ���´�
    // -> ���� �浹�� �����, ť���� ��� �ʿ� �ִ°�? ���� / �¿� / ����

    // �ش� �κп� ���� �������͸� �ٽ� ���ؼ� �����Ѵ� -> ť���� �࿡ ���� ������ ���Ѵ�

    switch (maxIndex)
    {
    case 0: // x
        dir.x = dir.x > 0 ? 1.0f : -1.0f;
        dir.y = 0;
        dir.z = 0;
        break;

    case 1: // y
        dir.x = 0;
        dir.y = dir.y > 0 ? 1.0f : -1.0f;;
        dir.z = 0;
        break;

    case 2: // z
        dir.x = 0;
        dir.y = 0;
        dir.z = dir.z > 0 ? 1.0f : -1.0f;;
        break;
    }

    // �׷��� �ε�ģ ť���, �� ť��κ��� �޾Ƴ� �� �ִ� �࿡ ���� �������� ��� ��´�
    // -> ��ȣ�ۿ��� ���� �غ� �Ϸ�

    // ���� �ڵ�� ����
    // ���� ��Ȳ : �Ű������� ����� �޾Ҵ� + �� ����� ���⿡ �߰��ؼ� �״´�

    block->SetActive(true);
    block->Pos() = collisionBlock->GlobalPos() + dir; // �浹�� ����� �ٷ� ������(�ٷ� ��, �ٷ� �� ��)
    // ��� ����� 1�� �ƴ� ���, size �ޱ�� �Բ� ���� ��� �߰�
    block->Update(); //�� ��� ������Ʈ)

    blocks.push_back(block); // ��� ����Ʈ�� �߰�


    return true;  // ������ ��ȯ
}

void BlockManager::Push(Collider* collider)
{
    // �浹�� �Ͼ�� ��, �аų�, Ȥ�� �и��� �����ϴ� �Լ�
    // �ϴ��� ����� ��븦 �о�� ���� �ǵ��ϰ� ����

    Cube* collisionBlock = nullptr;

    for (Cube* block : blocks)
    {
        if (block->GetCollider()->IsCollision(collider))
        {
            collisionBlock = block;

            Vector3 dir = collider->GlobalPos() - collisionBlock->GlobalPos();

            int maxIndex = 0;
            float maxValue = 0;

            for (int i = 0; i < 3; ++i)
            {
                if (abs(dir[i]) > maxValue)
                {
                    maxIndex = i;
                    maxValue = abs(dir[i]);
                }
            }

            // �Ʒ� �ڵ�� ������ ��Ȯ�� �࿡ ���� v(0, 0, 1) ��, ���� �ǹ��� ���� ���͸��� �����Ѵ�
            // ���� : ���ʿ� ���⿡ ����Ǵ� ���� �� ��� �Ŵ��� ��ü�� ���� ���꿡 ���߱� ���ؼ�

            // ���� ����� ȸ���Ѵٸ� �Ʒ����� �����ؾ� �� ���⿡�� ��ȭ�� �ʿ��� ��

            switch (maxIndex)
            {
            case 0: // x
                dir.x = dir.x > 0 ? 1.0f : -1.0f;
                dir.y = 0;
                dir.z = 0;
                break;

            case 1: // y
                dir.x = 0;
                dir.y = dir.y > 0 ? 1.0f : -1.0f;;
                dir.z = 0;
                break;

            case 2: // z
                dir.x = 0;
                dir.y = 0;
                dir.z = dir.z > 0 ? 1.0f : -1.0f;;
                break;
            }

            // --- ���� ���� �ڵ� : ����� ������κ��� �и� ---

            if (NearlyEqual(dir.y, 1.0f)) continue; // ������ ���� ���

            collider->GetParent()->Pos() += dir * 50.0f * DELTA;
            // ����� �־��� �������� �����ŭ ���� �޾� �и���

        // --- ���� �ڵ� ����
        }
    }
}
