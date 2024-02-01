#include "Framework.h"
#include "AStar.h"

AStar::AStar(UINT width, UINT height)
    : width(width), height(height)
{
    heap = new Heap();

    //SetNode(); //�⺻ ��� ������ �ִٸ� ���⼭ ���߿� ȣ��
                 //������ ������ ������ų �Ŷ� �����ڿ����� ��� ��ġ �� ��
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
    // ������ ũ�� (��带 ��ġ�� �Ը�)
    //Vector2 mapSize = terrain->GetMaterial()->GetDiffuseMap()->GetSize(); //������ ũ��
    Vector2 mapSize = terrain->GetSize(); //���� Ŭ������ ���� ������ ���μ��� �ޱ�

    // ��� ��ġ ���� ���ϱ�
    interval.x = mapSize.x / width;
    interval.y = mapSize.y / height;

    // ��� ���� �ڸ��� ���Ϳ��� ����
    nodes.reserve((width + 1) * (height + 1)); //���ʹ� 0���� ����ϱ�

    for (UINT z = 0; z <= height; ++z)
    {
        for (UINT x = 0; x <= width; ++x)
        {
            Vector3 pos = Vector3(x * interval.x, 0, z * interval.y);
            //pos.y = 0; // �⺻��
            pos.y = terrain->GetHeight(pos); // A*�� �����̵��� �ƴ϶� ������ �����ϱ� ������
                                             // ������ ������ ���� ��ȭ���� ���� ����

            //��� �߰�
            nodes.push_back(new Node(pos, nodes.size())); //��ġ��, ������ ���� ������ ������ ���ʷ� �ο�
            nodes.back()->Scale() = { interval.x, 50, interval.y }; //������ �����ϰ�, ���Ʒ��� ���
            nodes.back()->UpdateWorld();

            // ���̿� ��ȭ�� ���� ���, �� �ؿ� �ڵ带 �߰��ϸ� �ȴ�
            // ���� �ó����� : ���� ���� �� ��ֹ��̴�
            if (pos.y > 0)
            {
                nodes.back()->SetState(Node::OBSTACLE); //��ֹ��� �����ϰ�
                AddObstacle(nodes.back()); //��ֹ� �߰� �Լ� ȣ��
            }
        }
    }
    // ������� ���� ���μ��� ������ ���鼭 ��� ��ġ�� ������

    SetEdge(); // ������ ������ ���ָ� ���⼭ �� �۾��� ��
}

int AStar::FindCloseNode(Vector3 pos)
{
    float minDist = FLT_MAX;
    int result = -1;

    for (UINT i = 0; i < nodes.size(); ++i)
    {
        if (nodes[i]->state == Node::OBSTACLE)
            continue; //��ֹ��̰ų�, (����) ���̰� 0���� ũ�� ��� �� �Ѵ�

        float distance = Distance(pos, nodes[i]->GlobalPos());
        if (distance < minDist) //�ּҰŸ��� ���ŵ� ������
        {
            minDist = distance; //���Ű�� �ݿ�
            result = i; //����� �ε��� ���
        }
    }
    return result;
}

int AStar::FindRandomPos(Vector3 pos, float range)
{
    // ������ ��ǥ���� ����� ���� �� ���� ��ǥ�� ã�Ƴ���
    vector<Node*> findNodes;

    for (Node* node : nodes)
    {
        float distance = Distance(pos, node->GlobalPos());
        if (distance < range) findNodes.push_back(node); // ���� �� ��� ���Ϳ� �߰��ϱ�
    }
    //������� ���� ���� ���� ��� ��带 ���ͷ� �޴´�
    //���⼭ �ƹ��ų� �������� �ϳ� �̱�

    Node* findNode = findNodes[Random(0, findNodes.size())];

    return findNode->index; // �������� ���� ����� �ε���
}

void AStar::GetPath(IN int start, IN int end, OUT vector<Vector3>& path)
{
    // ��� �Է��� ���� ������ �� �״�� ����
    if (start == -1 || end == -1) return;

    // ������� �ʾҴٸ� = �� ���, ������� �������� �� ���� ��
    
    // -> ��θ� ã�Ƴ��� ���� ���� "�տ� Ȥ�� ���� �˰����� ������ ���� ������"
    //    �ش� ����� �ʱ�ȭ�ϴ� �ͺ��� ����

    Reset(); // �˰��� ����
    path.clear(); // �Ű������� ���� (����� ����Ǿ�� ��) ���͵� ������ ��� ����

    // ��� ��� �߿� ����� ���� �����͵� �غ��ϸ鼭 �ʱ�ȭ
    float _g = 0; // ���� ��������
    float _h = GetDiagonalManhattanDistance(start, end); // �޸���ƽ
                                // ���⼭�� �޸���ƽ(�Ǵܱ���) = ���� ��ǥ ��������
                                // �������� ���� ���� �ִ� ��� (����, ����, ���� �밢�� ����)
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
    //���� ������ ���� ���� "����" ������ �ǹ� ������
    UINT width = this->width + 1;

    for (UINT i = 0; i < nodes.size(); ++i)
    {
        if (i % width != width - 1) // ������ ���� �ƴ϶��
        {
            nodes[i]->AddEdge(nodes[i + 1]); // �ڱ� ������ ���� ������ ���� ��ȯ
            nodes[i + 1]->AddEdge(nodes[i]);
        }

        if (i < nodes.size() - width) //�� �Ʒ��� �ƴ϶��
        {
            nodes[i]->AddEdge(nodes[i + width]); // �ڱ� �Ʒ��� ���� ������ ��ȯ
            nodes[i + width]->AddEdge(nodes[i]);
        }

        if (i < nodes.size() - width && i % width != width - 1) //�� �Ʒ��� �ƴѵ� ������ ���� �ƴ϶��
        {
            nodes[i]->AddEdge(nodes[i + width + 1]); //�ڱ� �밢�� ������ �Ʒ��� ������ ��ȯ
            nodes[i + width + 1]->AddEdge(nodes[i]);
        }

        if (i < nodes.size() - width && i % width != 0) //�� �Ʒ��� �ƴϰ�, ���� ���� �ƴϴ�
        {
            nodes[i]->AddEdge(nodes[i + width - 1]); //�ڱ� �밢�� ���� �Ʒ��� ������ ��ȯ
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
    //���� �ε����� �����ϴ� ����� ��ġ
    Vector3 startPos = nodes[start]->GlobalPos();
    Vector3 endPos = nodes[end]->GlobalPos();

    //������ ���� �� ��ġ�� ���踦 "�Ÿ�"��� �̸����� ���Ѵ�
    Vector3 distance = endPos - startPos; //���ۿ��� ������ ���� ���� �� ��ü

    return abs(distance.x) + abs(distance.z);
            //��� ������ x���� z���� �״�� ���ؼ� ���� �Ÿ��� �����
}

float AStar::GetDiagonalManhattanDistance(int start, int end)
{
    //���� �ε����� �����ϴ� ����� ��ġ
    Vector3 startPos = nodes[start]->GlobalPos();
    Vector3 endPos = nodes[end]->GlobalPos();

    //������ ���� �� ��ġ�� ���踦 "�Ÿ�"��� �̸����� ���Ѵ�
    Vector3 distance = endPos - startPos; //���ۿ��� ������ ���� ���� �� ��ü

    float x = abs(distance.x); // ��κ����� x ��ǥ (���� ���� ����)
    float z = abs(distance.z); // ��κ����� z ��ǥ (���� ���� ����)

    // �� �� ���̰� ��� ������ ���ϱ� (������ ���� ��� ���� ����� ���� ��)
    float maxDist = max(x, z); // �� �� ��ǥ ����
    float minDist = min(x, z); // �� ª�� ��ǥ ����

    return (maxDist - minDist) + sqrt(minDist * minDist * 2);
        //�밢���� �� �� ���� �κ��� ���� �Ÿ��� �밢�� ���̸� ��ģ��
}
