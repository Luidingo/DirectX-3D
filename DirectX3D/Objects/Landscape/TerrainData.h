#pragma once

class TerrainData
{
	// 터레인 데이터 : 지형(을 분할한 or 분할하기 위한) 데이터를 갖는 클래스

private:
	float MAX_HEIGHT = 20;											// 높이 맵 등이 적용될 때, 높이의 상한
	typedef VertexUVNormal VertexType;

public:
	TerrainData(wstring heightFile);								// 지형이 평평했으면 복잡한 데이터도 필요 없으므로
	~TerrainData();												

	vector<VertexType> GetVertices() { return alignedVertices; }	// 데이터를 호출해서 보기 위한 함수
	Vector2 GetSize() { return Vector2(width, height); }			// 지형의 크기를 알기 위한 함수

private:
	void MakeMesh();
	void MakeNormal();
	void AlignVertexData();											// 정점 데이터 정리하기

private:
	vector<VertexType> vertices;									// 형태에 대한 정점 정보
	vector<VertexType> alignedVertices;								// "정리된" "옆자리와 합쳐진" 정점들 
																	// (즉, 각 점이 주변에 어떤 점들이 있는지 기록한 정점 확장 정보)
																	// -> 쿼드트리는 메모리 부담을 준다 (대신 연산 속도 증가)
	vector<UINT> indices;											// 렌더용 정점 순번의 목록

	UINT width;
	UINT height;

	Texture* heightMap;												// 높이 맵 적용
};

