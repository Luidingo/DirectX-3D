#pragma once

class Water : public GameObject
{
public:
	Water(wstring normalFile, float width = 100.f, float height = 100.f);
	~Water();

	void Update();
	void Render();
	void GUIRender();

	// 일렁임, 반사 설정용
	void SetRefraction();
	void SetReflection();

private:
	void MakeMesh();

private:
	Vector2 size;

	Mesh<VertexUV>* mesh;

	Refraction* refraction;
	Reflection* reflection;

	// 출력 상태(투명, 혼합)
	BlendState* blendState[2];
};

