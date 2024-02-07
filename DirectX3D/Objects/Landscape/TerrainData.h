#pragma once

class TerrainData
{
	// �ͷ��� ������ : ����(�� ������ or �����ϱ� ����) �����͸� ���� Ŭ����

private:
	float MAX_HEIGHT = 20;											// ���� �� ���� ����� ��, ������ ����
	typedef VertexUVNormal VertexType;

public:
	TerrainData(wstring heightFile);								// ������ ���������� ������ �����͵� �ʿ� �����Ƿ�
	~TerrainData();												

	vector<VertexType> GetVertices() { return alignedVertices; }	// �����͸� ȣ���ؼ� ���� ���� �Լ�
	Vector2 GetSize() { return Vector2(width, height); }			// ������ ũ�⸦ �˱� ���� �Լ�

private:
	void MakeMesh();
	void MakeNormal();
	void AlignVertexData();											// ���� ������ �����ϱ�

private:
	vector<VertexType> vertices;									// ���¿� ���� ���� ����
	vector<VertexType> alignedVertices;								// "������" "���ڸ��� ������" ������ 
																	// (��, �� ���� �ֺ��� � ������ �ִ��� ����� ���� Ȯ�� ����)
																	// -> ����Ʈ���� �޸� �δ��� �ش� (��� ���� �ӵ� ����)
	vector<UINT> indices;											// ������ ���� ������ ���

	UINT width;
	UINT height;

	Texture* heightMap;												// ���� �� ����
};

