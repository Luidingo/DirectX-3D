#include "Framework.h"

QuadTreeTerrain::QuadTreeTerrain(wstring heightFile, wstring textureFile)
{
	terrainData = new TerrainData(heightFile);
	width = terrainData->GetSize().x;
	height = terrainData->GetSize().y;

	// 매티리얼 (디퓨즈맵) 할당
	//material->SetDiffuseMap(L"Textures/Landscape/Dirt2.png"); // 필요하면 매개변수로 전환
	material->SetDiffuseMap(textureFile);

	// 정점과 삼각형 미리 준비
	UINT vertexCount = vertices.size();
	triangleCount = vertexCount / 3;

	float centerX = 0;
	float centerZ = 0;
	float size = 0;

	CalcMeshDimension(vertexCount, centerX, centerZ, size);
	
	root = new Node(); // 루트 노드 만들기
	CreateTreeNode(root, centerX, centerZ, size); // 트리를 실제로 만들기
}

QuadTreeTerrain::~QuadTreeTerrain()
{
	DeleteNode(root); // 루트 밑에 있는 트리 데이터를 모두 지우고
	delete root; // 그 다음에 루트를 지우기
}

void QuadTreeTerrain::Render()
{
}

void QuadTreeTerrain::GUIRender()
{
}

void QuadTreeTerrain::RenderNode(Node* node)
{
}

void QuadTreeTerrain::DeleteNode(Node* node)
{
	FOR(4) // 노드 밑에 있는 자식 수만큼 반복을 수행
	{
		if (node->children[i]) // 노드 밑에 자식이 할당되어 있는 경우
		{
			DeleteNode(node->children[i]); // 해당 자식에게 재귀 함수 실행을 먼저 수행
			delete node->children[i]; // 해당 자식의 메모리 할당을 해제
		}
	}

	// 먼저 자식이 있는 경우 자식부터 하나하나 추적하면서 삭제 내용을 선행
	// 
	// 더 자식이 없으면 자기 노드의 내부 데이터 삭제
	delete node->mesh; // 노드 구조체 안에 있었던 메쉬 데이터 삭제

	// * 지금은 소멸자 용이지만 나중에 노드에 수동 편집이 이루어질 수 있다면
	//   이 함수를 다른 곳에서도 호출해야 할 수 있다 (노드 삭제이므로)
}

void QuadTreeTerrain::CalcMeshDimension(UINT vertexCount, float& centerX, float& centerZ, float& size)
{
	// 받은 정점만큼 반복문을 돌리면서 정점의 위치를 통해 중간값 내기
	FOR(vertexCount)
	{
		centerX += vertices[i].pos.x; // 위치의 x값 전부 더하기
		centerZ += vertices[i].pos.z; // 위치의 z값 전부 더하기
	}
	// 개수로 나누어서 중간값 내기
	centerX /= (float)vertexCount;
	centerZ /= (float)vertexCount;

	// 검산 및 정점의 실제 위치를 재반영
	float maxX = 0;
	float maxZ = 0;

	FOR(vertexCount)
	{
		float width = abs(vertices[i].pos.x - centerX); // 정점 값에 중간 값을 뺀 결과를 받으면서
		float depth = abs(vertices[i].pos.z - centerZ); // 결과 확인 + 정점 실제 위치에서 가로 세로를 다시 도출

		if (width > maxX) maxX = width; // width와 maxX 중 큰 값을 max값에 갱신
		if (depth > maxZ) maxZ = depth; // depth와 maxZ 중 큰 값을 max값에 갱신
	}

	// 여기까지 오면
	// 1. 센터 값 구함
	// 2. 센터 값이 적절한지 정점에 다시 빼면서 연산
	// 3. 2.의 결과로 최대 범위 구함
	// 4. 3.의 결과에서 가로 세로의 크기 범위까지 예측 가능
	//
	// 결과로 나온 4.의 값을 size에 저장

	size = max(maxX, maxZ) * 2; // 현재 지형의 최대 크기 범위 (더 큰 쪽 변이 얼마나 긴가?)
}

bool QuadTreeTerrain::IsTriangleContained(UINT index, float x, float z, float size)
{
	return false;
}

UINT QuadTreeTerrain::ContainedTriangleCount(float x, float z, float size)
{
	return 0;
}

void QuadTreeTerrain::CreateTreeNode(Node* node, float x, float z, float size)
{
	// 노드에 매개변수로 받은 위치와 크기 정보를 기입
	node->x = x;
	node->z = z;
	node->size = size;

	// 해당 size로 지칭되는 노드 범위 내에 삼각형이 얼마나 있는지 판별
	UINT triangles = ContainedTriangleCount(x, z, size);

	if (triangles == 0) return; // 이 자리에 삼각형이 하나도 없으면 연산할 의미 없음

	// 여기에 삼각형이 있다 = 연산할 가치가 있는 노드이다

	// -> 이 다음에 해야할 계산 : 이 노드가 분할될 필요가 있는가?
	//	  분할되어야 한다면, 먼저 쪼개어놓고 자식에게 이 함수를 재귀시켜야 한다

	// * 현재 샘플 시나리오 : 삼각형의 개수가 일정 개수이기만 하면
	//		(= 지형 크기가 일정 이상이면) 쪼갤 것

	// 해당 조건을 표현한 조건문
	if (triangles > MIN_TRIANGLE) // 만약 트리 구분의 조건이 달라지면 여기서 수정
								  // ex) 높이가 급격하게 달라지는 경우
	{
		FOR(4) // 자식 트리를 위한 분할 반복문
		{
			float offsetX = (((i % 2) == 0) ? -1 : +1) * (size / 4); // 가로 보정치
			float offsetZ = ((i < 2) ? -1 : +1) * (size / 4); // 세로 보정치

			// 보정치 계산 후, 새로운 노드를 배열 내 각 자식 자리에 할당해 주고
			node->children[i] = new Node(); // 생성
			CreateTreeNode(node->children[i], x + offsetX, z + offsetZ, size * 0.5f);
		}

		return; // 자식이 다 쪼개졌으면 부모는 할 일이 없으므로 이대로 종료(밑에서 노드 작성 다 됐음)
	}
	
	// 여기까지 오면, 조건이 어쨌든 부모가 자식 4개로 쪼개질 수 없었다는 이야기가 됨
	// -> 조건 충족 실패

	// * 지금의 경우, 영역 내에 있는 삼각형(렌더 대상) 개수가 정해진 수에 미치지 못했다

	// 그럴 때 작성해야 할 잎사귀 노드(가장 끄트머리 노드)의 내용을 작성 진행

	node->triangleCount = triangleCount; // 노드가 갖고있었던 삼각형을 기입
	UINT vertexCount = triangles * 3; /// 총 정점 개수 구하기

	// 이 노드가 갖고 있는 메쉬 데이터 준비
	node->mesh = new Mesh<VertexType>();
	vector<VertexType>& vertices = node->mesh->GetVertices();
	vertices.resize(vertexCount);

	// 인댁스를 두 개 생성
	UINT index = 0;
	UINT vertexIndex = 0;
	FOR(triangleCount)
	{
		if (IsTriangleContained(i, x, z, size)) // 반복문 속 인덱스 i로 지정된 삼각형이 영역에 있는가?
		{
			// 있으면 해당 삼각형의 인덱스를 정점 인덱스에 받기
			vertexIndex = i * 3 + 0;
			// 터레인 데이터에 있었단 정리된 정점 데이터로부터 정보를 얻어와
			// 현재 노드 내 벡터의 index 순번에 집어넣기
			vertices[index] = this->vertices[vertexIndex]; 
			index++; // 인덱스 + 1 (뒤로 진행)

			// 위 계산을 삼각형의 나머지 두 점에도 똑같이 적용
			vertexIndex++;
			//vertexIndex = i * 3 + 1;
			vertices[index] = this->vertices[vertexIndex];
			index++;

			vertexIndex++;
			//vertexIndex = i * 3 + 2;
			vertices[index] = this->vertices[vertexIndex];
			index++;

			// 이 반복문을 삼각형 개수만큼 돌려주면 각 삼각형(정점)의 정보를 따로 쪼개어서
			// 따로 쪼개어서 각 노드에 나눠 넣을 수 있게 될 것
		}
	}

	// 여기까지 오면 자식 노드 내에서 쪼개진 지형을 렌더해주기 위한 정점의 데이터를 분할된 벡터로 가질 수 있다
	// 그 분할된 정점 벡터로 메쉬를 각각 만든다
	node->mesh->CreateMesh();
}
