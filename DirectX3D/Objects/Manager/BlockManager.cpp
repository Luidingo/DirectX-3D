#include "Framework.h"
#include "BlockManager.h"

BlockManager::~BlockManager()
{
    for (Cube* block : blocks)
        delete block;

    blocks.clear(); // 자기삭제. 필수는 아니지만
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
    //텍스처(의 파일) 정보
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

            //랜덤으로 정한 파일 이름을 텍스처로 쓰면서 블록(=큐브) 생성
            Cube* block = new Cube();
            block->GetMaterial()->SetDiffuseMap(file);
            block->Pos() = pos;
            block->Update(); // 최초 업데이트 (정점 적용하고 눈에 보이게)

            blocks.push_back(block);
        }
    }
    //여기까지 오면 블록 생성해서 나열하기 완료
    //그 다음에 어떻게 할지는 이 아래 작성 (지금은 보류)
}

float BlockManager::GetHeight(Vector3& pos)
{
    // 현재 블록의 높이 (층 수, 블록 자체의 여부에 따라 달라질 것)

    Ray ray(pos, Vector3::Down()); //매개변수로 받은 곳에서 정 아래로 내려가는 광선
    Contact contact; //접점정보

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
    //여기까지 왔을 때 기대할 결과는 둘
    // 1. 블록 위에 물체가 있었다면 -> 현재 가장 높은 곳에 있는 블록의 높이 도출됨
    // 2. 블록 밖에 물체가 있었다면 -> 초기값 0이 도출

    // 어느 쪽이든, 그 결과를 반환
    return currentMaxHeight;
}

Cube* BlockManager::GetCollisionBlock()
{
    // 화면에서, 혹은 캐릭터의 시야에 의해서 클릭이 일어났을 때, 블럭이 상호대상의 작용이 되는가

    // Ray ray(/*어딘가에서 받아온 캐릭터 위치, 그 캐릭터의 정면 방향*/);
    Ray ray = CAM->ScreenPointToRay(Vector3(CENTER_X, CENTER_Y)); // 모니터 뷰포인트를 쓰되 화면의 중앙을 광원으로

    float minDistance = FLT_MAX;
    Contact contact;

    //반복 담당 데이터(반복자)준비
    list<Cube*>::iterator iter = blocks.begin();
    list<Cube*>::iterator collisionBlock = blocks.end(); // 두 번째 반복자를 .end()로 = "없다"
    // .end() = 자료구조 안의 마지막 순번을 뜻하는 게 아니라, 해당 데이터의 진짜 마지막
    //          ("이 뒤로는 리스트 없음")

    for (iter; iter != blocks.end(); ++iter) // for문의 첫 번째 조건과 세 번째 조건은 필수 아님 (있는 편이 좋음)
    {
        if ((*iter)->GetCollider()->IsRayCollision(ray, &contact))
        {
            // 충돌 시에 기록된 최소 거리를 다시 검사해서 최종 갱신
            if (contact.distance < minDistance)
            {
                minDistance = contact.distance;
                collisionBlock = iter; // 충돌 블록을 현재의 블록으로 갱신
            }
        }
    }
    // 여기까지 왔을 때 결과는 둘 중 하나
    // 1. 블록 중 하나가 새로운 collisionBlock이다
    // 2. collisionBlock이 계속 blocks.end()다 = 이 경우는 "포인터가 지칭할 블록이 없다"

    if (collisionBlock != blocks.end())
    {
        Cube* block = *collisionBlock; // block은 현재 지정된 iter의 값 = 블록의 포인터

        blocks.erase(collisionBlock); // 샘플 코드 : 상호작용이 생겼을 때 매니저가 할 일
        // **매니저에 의한 상호작용이 필요하면 여기서 추가 작성

        return block; // 컴퓨터에 큐브 포인터도 반환
    }
    else
    {
        return nullptr; // 없으니까 널
    }
}

bool BlockManager::BuildBlock(Cube* block)
{
    //위 함수와 마찬가지로 광선을 받아서 진행
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
                //충돌이 일어난 큐브 찾아서 갱신
                minDistance = contact.distance;
                hitPoint = contact.hitPoint;
                collisionBlock = *iter;
            }
        }
    }

    // 여기까지 왔을 때 결과는 둘 중 하나
    // 1. 거리가 최소인 (가장 조준된 곳과 가까운) 큐브가 나온다 = 광선으로 맞춘 큐브가 나온다
    // 2. 충돌한 큐브가 없었다

    // 없었으면 곧바로 끝내고 bool 값을 거짓 반환
    if (collisionBlock == nullptr) return false;

    //충돌한 대상과 접점 간의 방향
    Vector3 dir = hitPoint - collisionBlock->GlobalPos();

    int maxIndex = 0;
    float maxValue = 0;

    for (int i = 0; i < 3; ++i)
    {
        if (abs(dir[i]) > maxValue) //위에서 받은 방향의 각 축 값(의 절대값)이 기록된 최대값을 넘어서면
        {
            // 최대값을 가진 축정보를 index, value 별로 기록
            maxIndex = i; //x 혹은 y 혹은 z
            maxValue = abs(dir[i]);
        }
    }

    // 여기서 한 가지 정보가 나온다
    // -> 현재 충돌한 대상이, 큐브의 어느 쪽에 있는가? 상하 / 좌우 / 전후

    // 해당 부분에 따른 단위벡터를 다시 구해서 적용한다 -> 큐브의 축에 의한 법선을 구한다

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

    // 그러면 부딪친 큐브와, 그 큐브로부터 받아낼 수 있는 축에 의한 법선까지 모두 얻는다
    // -> 상호작용을 위한 준비 완료

    // 이하 코드는 샘플
    // 샘플 상황 : 매개변수로 블록을 받았다 + 그 블록을 여기에 추가해서 쌓는다

    block->SetActive(true);
    block->Pos() = collisionBlock->GlobalPos() + dir; // 충돌한 블록의 바로 언저리(바로 옆, 바로 위 등)
    // 블록 사이즈가 1이 아닌 경우, size 받기와 함께 여기 계수 추가
    block->Update(); //새 블록 업데이트)

    blocks.push_back(block); // 블록 리스트에 추가


    return true;  // 참으로 반환
}

void BlockManager::Push(Collider* collider)
{
    // 충돌이 일어났을 때, 밀거나, 혹은 밀림을 제어하는 함수
    // 일단은 블록이 상대를 밀어내는 것을 의도하고 구현

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

            // 아래 코드는 방향을 정확히 축에 의한 v(0, 0, 1) 등, 좁은 의미의 단위 벡터만을 적용한다
            // 이유 : 애초에 여기에 적용되는 배경과 이 블록 매니저 자체를 격자 연산에 맞추기 위해서

            // 만약 배경이 회전한다면 아래에서 적용해야 할 방향에도 변화가 필요할 것

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

            // --- 이하 샘플 코드 : 대상은 블록으로부터 밀림 ---

            if (NearlyEqual(dir.y, 1.0f)) continue; // 법선이 밑인 경우

            collider->GetParent()->Pos() += dir * 50.0f * DELTA;
            // 대상은 주어진 방향으로 계수만큼 힘을 받아 밀린다

        // --- 샘플 코드 종료
        }
    }
}
