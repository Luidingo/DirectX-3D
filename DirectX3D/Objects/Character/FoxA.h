#pragma once

class FoxA : public ModelAnimator
{
private:
	enum State
	{
		IDLE, RUN
	};

	typedef Terrain LevelData;
	//typedef TerrainEditor LevelData;

public:
	FoxA();
	~FoxA();

	void Update();
	void Render();

	void SetTerrain(LevelData* terrain) { this->terrain = terrain; }
	void SetAStar(AStar* aStar) { this->aStar = aStar; }

private:
	void Control();
	void Move();
	void Rotate();

	void SetState(State state);
	void SetPath();

private:
	State curState = IDLE;

	float moveSpeed = 100.f;
	float rotSpeed = 10.f;

	Vector3 velocity;

	Vector3 destPos;
	vector<Vector3> path;

	LevelData* terrain;
	AStar* aStar;
};

