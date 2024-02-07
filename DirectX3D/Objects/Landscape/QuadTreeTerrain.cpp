#include "Framework.h"

QuadTreeTerrain::QuadTreeTerrain(wstring heightFile, wstring textureFile)
{
	terrainData = new TerrainData(heightFile);
	width = terrainData->GetSize().x;
	height = terrainData->GetSize().y;

	// ��Ƽ���� (��ǻ���) �Ҵ�
	//material->SetDiffuseMap(L"Textures/Landscape/Dirt2.png"); // �ʿ��ϸ� �Ű������� ��ȯ
	material->SetDiffuseMap(textureFile);

	// ������ �ﰢ�� �̸� �غ�
	UINT vertexCount = vertices.size();
	triangleCount = vertexCount / 3;

	float centerX = 0;
	float centerZ = 0;
	float size = 0;

	CalcMeshDimension(vertexCount, centerX, centerZ, size);
	
	root = new Node(); // ��Ʈ ��� �����
	CreateTreeNode(root, centerX, centerZ, size); // Ʈ���� ������ �����
}

QuadTreeTerrain::~QuadTreeTerrain()
{
	DeleteNode(root); // ��Ʈ �ؿ� �ִ� Ʈ�� �����͸� ��� �����
	delete root; // �� ������ ��Ʈ�� �����
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
	FOR(4) // ��� �ؿ� �ִ� �ڽ� ����ŭ �ݺ��� ����
	{
		if (node->children[i]) // ��� �ؿ� �ڽ��� �Ҵ�Ǿ� �ִ� ���
		{
			DeleteNode(node->children[i]); // �ش� �ڽĿ��� ��� �Լ� ������ ���� ����
			delete node->children[i]; // �ش� �ڽ��� �޸� �Ҵ��� ����
		}
	}

	// ���� �ڽ��� �ִ� ��� �ڽĺ��� �ϳ��ϳ� �����ϸ鼭 ���� ������ ����
	// 
	// �� �ڽ��� ������ �ڱ� ����� ���� ������ ����
	delete node->mesh; // ��� ����ü �ȿ� �־��� �޽� ������ ����

	// * ������ �Ҹ��� �������� ���߿� ��忡 ���� ������ �̷���� �� �ִٸ�
	//   �� �Լ��� �ٸ� �������� ȣ���ؾ� �� �� �ִ� (��� �����̹Ƿ�)
}

void QuadTreeTerrain::CalcMeshDimension(UINT vertexCount, float& centerX, float& centerZ, float& size)
{
	// ���� ������ŭ �ݺ����� �����鼭 ������ ��ġ�� ���� �߰��� ����
	FOR(vertexCount)
	{
		centerX += vertices[i].pos.x; // ��ġ�� x�� ���� ���ϱ�
		centerZ += vertices[i].pos.z; // ��ġ�� z�� ���� ���ϱ�
	}
	// ������ ����� �߰��� ����
	centerX /= (float)vertexCount;
	centerZ /= (float)vertexCount;

	// �˻� �� ������ ���� ��ġ�� ��ݿ�
	float maxX = 0;
	float maxZ = 0;

	FOR(vertexCount)
	{
		float width = abs(vertices[i].pos.x - centerX); // ���� ���� �߰� ���� �� ����� �����鼭
		float depth = abs(vertices[i].pos.z - centerZ); // ��� Ȯ�� + ���� ���� ��ġ���� ���� ���θ� �ٽ� ����

		if (width > maxX) maxX = width; // width�� maxX �� ū ���� max���� ����
		if (depth > maxZ) maxZ = depth; // depth�� maxZ �� ū ���� max���� ����
	}

	// ������� ����
	// 1. ���� �� ����
	// 2. ���� ���� �������� ������ �ٽ� ���鼭 ����
	// 3. 2.�� ����� �ִ� ���� ����
	// 4. 3.�� ������� ���� ������ ũ�� �������� ���� ����
	//
	// ����� ���� 4.�� ���� size�� ����

	size = max(maxX, maxZ) * 2; // ���� ������ �ִ� ũ�� ���� (�� ū �� ���� �󸶳� �䰡?)
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
	// ��忡 �Ű������� ���� ��ġ�� ũ�� ������ ����
	node->x = x;
	node->z = z;
	node->size = size;

	// �ش� size�� ��Ī�Ǵ� ��� ���� ���� �ﰢ���� �󸶳� �ִ��� �Ǻ�
	UINT triangles = ContainedTriangleCount(x, z, size);

	if (triangles == 0) return; // �� �ڸ��� �ﰢ���� �ϳ��� ������ ������ �ǹ� ����

	// ���⿡ �ﰢ���� �ִ� = ������ ��ġ�� �ִ� ����̴�

	// -> �� ������ �ؾ��� ��� : �� ��尡 ���ҵ� �ʿ䰡 �ִ°�?
	//	  ���ҵǾ�� �Ѵٸ�, ���� �ɰ������ �ڽĿ��� �� �Լ��� ��ͽ��Ѿ� �Ѵ�

	// * ���� ���� �ó����� : �ﰢ���� ������ ���� �����̱⸸ �ϸ�
	//		(= ���� ũ�Ⱑ ���� �̻��̸�) �ɰ� ��

	// �ش� ������ ǥ���� ���ǹ�
	if (triangles > MIN_TRIANGLE) // ���� Ʈ�� ������ ������ �޶����� ���⼭ ����
								  // ex) ���̰� �ް��ϰ� �޶����� ���
	{
		FOR(4) // �ڽ� Ʈ���� ���� ���� �ݺ���
		{
			float offsetX = (((i % 2) == 0) ? -1 : +1) * (size / 4); // ���� ����ġ
			float offsetZ = ((i < 2) ? -1 : +1) * (size / 4); // ���� ����ġ

			// ����ġ ��� ��, ���ο� ��带 �迭 �� �� �ڽ� �ڸ��� �Ҵ��� �ְ�
			node->children[i] = new Node(); // ����
			CreateTreeNode(node->children[i], x + offsetX, z + offsetZ, size * 0.5f);
		}

		return; // �ڽ��� �� �ɰ������� �θ�� �� ���� �����Ƿ� �̴�� ����(�ؿ��� ��� �ۼ� �� ����)
	}
	
	// ������� ����, ������ ��·�� �θ� �ڽ� 4���� �ɰ��� �� �����ٴ� �̾߱Ⱑ ��
	// -> ���� ���� ����

	// * ������ ���, ���� ���� �ִ� �ﰢ��(���� ���) ������ ������ ���� ��ġ�� ���ߴ�

	// �׷� �� �ۼ��ؾ� �� �ٻ�� ���(���� ��Ʈ�Ӹ� ���)�� ������ �ۼ� ����

	node->triangleCount = triangleCount; // ��尡 �����־��� �ﰢ���� ����
	UINT vertexCount = triangles * 3; /// �� ���� ���� ���ϱ�

	// �� ��尡 ���� �ִ� �޽� ������ �غ�
	node->mesh = new Mesh<VertexType>();
	vector<VertexType>& vertices = node->mesh->GetVertices();
	vertices.resize(vertexCount);

	// �δ콺�� �� �� ����
	UINT index = 0;
	UINT vertexIndex = 0;
	FOR(triangleCount)
	{
		if (IsTriangleContained(i, x, z, size)) // �ݺ��� �� �ε��� i�� ������ �ﰢ���� ������ �ִ°�?
		{
			// ������ �ش� �ﰢ���� �ε����� ���� �ε����� �ޱ�
			vertexIndex = i * 3 + 0;
			// �ͷ��� �����Ϳ� �־��� ������ ���� �����ͷκ��� ������ ����
			// ���� ��� �� ������ index ������ ����ֱ�
			vertices[index] = this->vertices[vertexIndex]; 
			index++; // �ε��� + 1 (�ڷ� ����)

			// �� ����� �ﰢ���� ������ �� ������ �Ȱ��� ����
			vertexIndex++;
			//vertexIndex = i * 3 + 1;
			vertices[index] = this->vertices[vertexIndex];
			index++;

			vertexIndex++;
			//vertexIndex = i * 3 + 2;
			vertices[index] = this->vertices[vertexIndex];
			index++;

			// �� �ݺ����� �ﰢ�� ������ŭ �����ָ� �� �ﰢ��(����)�� ������ ���� �ɰ��
			// ���� �ɰ�� �� ��忡 ���� ���� �� �ְ� �� ��
		}
	}

	// ������� ���� �ڽ� ��� ������ �ɰ��� ������ �������ֱ� ���� ������ �����͸� ���ҵ� ���ͷ� ���� �� �ִ�
	// �� ���ҵ� ���� ���ͷ� �޽��� ���� �����
	node->mesh->CreateMesh();
}
