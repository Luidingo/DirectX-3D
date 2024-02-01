#include "Framework.h"

RobotA::RobotA() : ModelAnimator("Robot")
{
	ReadClip("Walk");
	ReadClip("Run");
}

RobotA::~RobotA()
{
}

void RobotA::Update()
{
    Control();
	Move();
	Rotate();

	ModelAnimator::Update();
}

void RobotA::Render()
{
	ModelAnimator::Render();
}

void RobotA::Control()
{
    if (deltaTime > 0.f)
        deltaTime -= DELTA;
    else
    {
        deltaTime = 0.5f;
        destPos = target->Pos();
        if (aStar->IsCollisionObstacle(GlobalPos(), destPos)) // �߰��� ��ֹ��� ������
        {
            SetPath(); // ��ü���� ��� ��� ����
        }
        else //��ֹ��� ���� ���
        {
            path.clear(); //3D���� ��ֹ��� ���µ� ���� ��ã�⸦ �� �ʿ� ����
            path.push_back(destPos); // ���� �� ���� ��� ���Ϳ� ����ֱ�
                                     // -> �׷��� ����� Move()�� �������� ã�ư� ��
        }
    }
}

void RobotA::Move()
{
    if (path.empty())
    {
        SetState(WALK);
        return;
    }

    SetState(RUN);

    Vector3 dest = path.back();

    Vector3 direction = dest - GlobalPos();

    direction.y = 0; // �ʿ��ϸ� ������ ����(����) �ݿ��� ���� ���� �ְ�,
                     // ���� �ʿ��ϸ� �׳� �¿�ȸ���� �ϴ� �ɷ� (y ���� �Ϻη� ���� ����)

    if (direction.Length() < 0.5f)
    {
        path.pop_back();
    }

    velocity = direction.GetNormalized();
    Pos() += velocity * moveSpeed * DELTA;
}

void RobotA::Rotate()
{
    if (curState == WALK) return;

    Vector3 forward = Forward();
    Vector3 cross = Cross(forward, velocity);

    if (cross.y < 0)
    {
        Rot().y += rotSpeed * DELTA;
    }
    else if (cross.y > 0)
    {
        Rot().y -= rotSpeed * DELTA;
    }
}

void RobotA::SetState(State state)
{
    if (state == curState) return;
    curState = state;
    PlayClip(state);
}

void RobotA::SetPath()
{
    int startIndex = aStar->FindCloseNode(GlobalPos());
    int endIndex = aStar->FindCloseNode(destPos); // �������(+������Ʈ����) ������ ������

    aStar->GetPath(startIndex, endIndex, path); // ���� �� ���� path ���Ϳ� ����

    aStar->MakeDirectionPath(GlobalPos(), destPos, path); // ��ֹ��� ����� path�� ������

    UINT pathSize = path.size(); // ó�� ���� ��� ���� ũ�⸦ ����

    while (path.size() > 2) // "������" ��� ��尡 1���� ���ϰ� �� ������
    {
        vector<Vector3> tempPath = path; // ���� �ӽ� ��� �޾ƿ���
        tempPath.erase(tempPath.begin()); // ���� ������ ����� (��ֹ��� �־��� ������ ������ �߰��� ���� ��)
        tempPath.pop_back(); // ���� ��ġ�� �������� �����ϱ� (�̹� �������� �����ϱ�)

        // ������ ������ ���� ����� ���ο� ���۰� ���� ����
        Vector3 start = path.back();
        Vector3 end = path.front();

        //�ٽ� ���� ��ο��� ��ֹ� ����� �� ���
        aStar->MakeDirectionPath(start, end, tempPath);

        //��� ��� �ǵ��
        path.clear();
        path = tempPath;

        //��� ���Ϳ� ���ο� ���۰� ���� ����
        path.insert(path.begin(), end);
        path.push_back(start);

        // ����� �ٽ� �ߴµ� ���� ũ�Ⱑ �״�ζ�� = ���� �ٲ��� �ʾҴ�
        if (pathSize == path.size()) break; // �� �̻� ����� ����� ������ ����
        else pathSize = path.size(); // �پ�� ��� ����� �ݿ��� ���ְ� �ݺ����� �ٽ� ����
    }

    // �ٽ� ������, ���� �� �� �ִ� ��ο�, ���� �������� �ٽ� �ѹ� �߰��Ѵ�
    path.insert(path.begin(), destPos);
}
