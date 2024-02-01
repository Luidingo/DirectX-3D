#include "Framework.h"
#include "AStar.h"

AStar::AStar(UINT width, UINT height)
    : width(width), height(height)
{
    heap = new Heap();

    //SetNode(); //기본 노드 세팅이 있다면 여기서 나중에 호출
                 //지금은 지형에 대응시킬 거라서 생성자에서는 노드 설치 안 함
}

AStar::~AStar()
{
    for (Node* node : nodes)
        delete node;
    delete heap;
}

void AStar::Update()
{
}

void AStar::Render()
{
}

void AStar::SetNode(Terrain* terrain)
{
    // 지형의 크기 (노드를 설치할 규모)
    //Vector2 mapSize = terrain->GetMaterial()->GetDiffuseMap()->GetSize(); //지도의 크기
    Vector2 mapSize = terrain->GetSize(); //지형 클래스에 직접 지정된 가로세로 받기

    // 노드 배치 간격 구하기
    interval.x = mapSize.x / width;
    interval.y = mapSize.y / height;

    // 노드 생성 자리를 벡터에서 예약
    nodes.reserve((width + 1) * (height + 1)); //벡터는 0부터 만드니까

    for (UINT z = 0; z <= height; ++z)
    {
        for (UINT x = 0; x <= width; ++x)
        {
            Vector3 pos = Vector3(x * interval.x, 0, z * interval.y);
            //pos.y = 0; // 기본값
            pos.y = terrain->GetHeight(pos); // A*는 직선이동이 아니라 노드들을 경유하기 때문에
                                             // 지형이 가지는 높이 변화에도 대응 가능

            //노드 추가
            nodes.push_back(new Node(pos, nodes.size())); //위치와, 벡터의 현재 마지막 순번을 차례로 부여
            nodes.back()->Scale() = { interval.x, 50, interval.y }; //간격을 적용하고, 위아래로 길게
            nodes.back()->UpdateWorld();

            // 높이에 변화가 있을 경우, 이 밑에 코드를 추가하면 된다
            // 샘플 시나리오 : 높은 곳은 곧 장애물이다
            if (pos.y > 0)
            {
                nodes.back()->SetState(Node::OBSTACLE); //장애물로 설정하고
                AddObstacle(nodes.back()); //장애물 추가 함수 호출
            }
        }
    }
    // 여기까지 오면 가로세로 번갈아 가면서 노드 설치가 끝난다

    SetEdge(); // 인접지 설정만 해주면 여기서 할 작업은 끝
}

int AStar::FindCloseNode(Vector3 pos)
{
    float minDist = FLT_MAX;
    int result = -1;

    for (UINT i = 0; i < nodes.size(); ++i)
    {
        if (nodes[i]->state == Node::OBSTACLE)
            continue; //장애물이거나, (샘플) 높이가 0보다 크면 취급 안 한다

        float distance = Distance(pos, nodes[i]->GlobalPos());
        if (distance < minDist) //최소거리가 갱신될 때마다
        {
            minDist = distance; //갱신결과 반영
            result = i; //결과에 인덱스 기록
        }
    }
    return result;
}

int AStar::FindRandomPos(Vector3 pos, float range)
{
    // 지정된 좌표에서 가까운 범위 내 랜덤 좌표를 찾아내기
    vector<Node*> findNodes;

    for (Node* node : nodes)
    {
        float distance = Distance(pos, node->GlobalPos());
        if (distance < range) findNodes.push_back(node); // 범위 내 노드 벡터에 추가하기
    }
    //여기까지 오면 범위 안의 모든 노드를 벡터로 받는다
    //여기서 아무거나 랜덤으로 하나 뽑기

    Node* findNode = findNodes[Random(0, findNodes.size())];

    return findNode->index; // 랜덤으로 나온 노드의 인덱스
}

void AStar::GetPath(IN int start, IN int end, OUT vector<Vector3>& path)
{
    // 노드 입력을 받지 못했을 때 그대로 종료
    if (start == -1 || end == -1) return;

    // 종료되지 않았다면 = 두 노드, 출발지와 목적지를 잘 받은 것
    
    // -> 경로를 찾아내기 전에 먼저 "앞에 혹시 같은 알고리즘을 실행한 적이 있으면"
    //    해당 결과를 초기화하는 것부터 시작

    Reset(); // 알고리즘 리셋
    path.clear(); // 매개변수로 받은 (결과가 저장되어야 할) 벡터도 내용을 모두 비운다

    // 경로 계산 중에 사용할 지형 데이터도 준비하면서 초기화
    float _g = 0; // 지형 ㄷ제이터
    float _h = GetDiagonalManhattanDistance(start, end); // 휴리스틱
                                // 여기서의 휴리스틱(판단기준) = 격자 좌표 공간에서
                                // 목적지로 가는 직선 최단 경로 (가로, 세로, 격자 대각선 구성)
}

void AStar::MakeDirectionPath(IN Vector3 start, IN Vector3 end, OUT vector<Vector3>& path)
{
}

bool AStar::IsCollisionObstacle(Vector3 start, Vector3 end)
{
    return false;
}

void AStar::AddObstacle(Collider* collider)
{
    obstacles.push_back(collider);
}

void AStar::SetEdge()
{
    //지역 내에서 쓰기 위한 "가로" 변수의 의미 재정의
    UINT width = this->width + 1;

    for (UINT i = 0; i < nodes.size(); ++i)
    {
        if (i % width != width - 1) // 오른쪽 끝이 아니라면
        {
            nodes[i]->AddEdge(nodes[i + 1]); // 자기 오른쪽 노드와 인접지 정보 교환
            nodes[i + 1]->AddEdge(nodes[i]);
        }

        if (i < nodes.size() - width) //맨 아래가 아니라면
        {
            nodes[i]->AddEdge(nodes[i + width]); // 자기 아래쪽 노드와 인접지 교환
            nodes[i + width]->AddEdge(nodes[i]);
        }

        if (i < nodes.size() - width && i % width != width - 1) //맨 아래도 아닌데 오른쪽 끝도 아니라면
        {
            nodes[i]->AddEdge(nodes[i + width + 1]); //자기 대각선 오른쪽 아래와 인접지 교환
            nodes[i + width + 1]->AddEdge(nodes[i]);
        }

        if (i < nodes.size() - width && i % width != 0) //맨 아래가 아니고, 왼쪽 끝이 아니다
        {
            nodes[i]->AddEdge(nodes[i + width - 1]); //자기 대각선 왼쪽 아래와 인접지 교환
            nodes[i + width - 1]->AddEdge(nodes[i]);
        }
    }
}

int AStar::GetMinNode()
{
    return 0;
}

void AStar::Extend(int center, int end)
{
}

void AStar::Reset()
{
}

float AStar::GetManhattanDistance(int start, int end)
{
    //받은 인덱스에 대응하는 노드의 위치
    Vector3 startPos = nodes[start]->GlobalPos();
    Vector3 endPos = nodes[end]->GlobalPos();

    //위에서 나온 두 위치의 관계를 "거리"라는 이름으로 구한다
    Vector3 distance = endPos - startPos; //시작에서 끝으로 가는 벡터 그 자체

    return abs(distance.x) + abs(distance.z);
            //경로 벡터의 x값과 z값을 그대로 더해서 직각 거리를 만든다
}

float AStar::GetDiagonalManhattanDistance(int start, int end)
{
    //받은 인덱스에 대응하는 노드의 위치
    Vector3 startPos = nodes[start]->GlobalPos();
    Vector3 endPos = nodes[end]->GlobalPos();

    //위에서 나온 두 위치의 관계를 "거리"라는 이름으로 구한다
    Vector3 distance = endPos - startPos; //시작에서 끝으로 가는 벡터 그 자체

    float x = abs(distance.x); // 경로벡터의 x 좌표 (직각 가로 길이)
    float z = abs(distance.z); // 경로벡터의 z 좌표 (직각 세로 길이)

    // 더 긴 길이가 어느 쪽인지 구하기 (같으면 같은 대로 역시 결과가 나올 것)
    float maxDist = max(x, z); // 더 긴 좌표 길이
    float minDist = min(x, z); // 더 짧은 좌표 길이

    return (maxDist - minDist) + sqrt(minDist * minDist * 2);
        //대각선을 낼 수 없는 부분의 직선 거리와 대각선 길이를 합친다
}
