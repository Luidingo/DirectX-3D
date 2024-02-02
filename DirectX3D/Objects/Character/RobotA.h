#pragma once

class RobotA : public ModelAnimator
{
private:
    enum State
    {
        WALK, RUN
    };

    //typedef Terrain LevelData;
    typedef TerrainEditor LevelData;

public:
    RobotA();
    ~RobotA();

    void Update();
    void Render();

    void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
    void SetAStar(AStar* aStar) { this->aStar = aStar; }
    void SetTarget(Transform* target) { this->target = target; }

private:
    void Control();
    void Move();
    void Rotate();

    void SetState(State state);
    void SetPath();

private:
    State curState = WALK;

    float moveSpeed = 10.f;
    float rotSpeed = 10.f;

    float deltaTime = 1.f;

    Vector3 velocity;

    Vector3 destPos;
    vector<Vector3> path;

    LevelData* terrain;
    AStar* aStar;

    Transform* target;
};

