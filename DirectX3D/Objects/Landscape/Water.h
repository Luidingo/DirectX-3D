#pragma once

class Water : public GameObject
{
public:
	Water(wstring normalFile, float width = 100.f, float height = 100.f);
	~Water();

	void Update();
	void Render();
	void GUIRender();

	// �Ϸ���, �ݻ� ������
	void SetRefraction();
	void SetReflection();

private:
	void MakeMesh();

private:
	Vector2 size;

	Mesh<VertexUV>* mesh;

	Refraction* refraction;
	Reflection* reflection;

	// ��� ����(����, ȥ��)
	BlendState* blendState[2];
};

