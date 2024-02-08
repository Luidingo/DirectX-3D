#include "Framework.h"

QuadTreeTerrain::QuadTreeTerrain(wstring heightFile)
{
	terrainData = new TerrainData(heightFile);
	width = terrainData->GetSize().x;
	height = terrainData->GetSize().y;

	// ��Ƽ���� (��ǻ���) �Ҵ�
	material->SetDiffuseMap(L"Textures/Landscape/Dirt2.png"); // �ʿ��ϸ� �Ű������� ��ȯ
	//material->SetDiffuseMap(textureFile);

	// ���� �����Ϳ��� ������ ���� �ޱ�
	vertices = terrainData->GetVertices();

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
	// ��带 �����ϴ� �Լ��� ���� �����Ƿ� ȣ��
	// �ش� �Լ��� ����� ��ü ���� ������ ������ ȣ��

	drawCount = 0; // ��ο� �� Ƚ�� �ʱ�ȭ (����� ���� ��� �߿� �� ��)
	SetRender(); // ���� ������Ʈ�μ�
	RenderNode(root); // ��Ʈ���� ���� �����ϱ�
}

void QuadTreeTerrain::GUIRender()
{
	ImGui::Text("Draw Count : %d", drawCount);
}

void QuadTreeTerrain::RenderNode(Node* node)
{
	// ����� �߽� ������ �ޱ�
	Vector3 center(node->x, 0, node->z);
	float radius = node->size * 0.5f;

	// ����ü ����
	if (!CAM->ContainPoint(center, radius)) // �߽ɰ� �ݰ���� ����ü �˻縦 �ؼ�
	{
		// ����ü �ȿ� ������
		return; // ���� ��Ȱ��ȭ
	}

	UINT count = 0;
	FOR(4)
	{
		// �ڽ� ��� ���� �Ǻ�
		if (node->children[i])
		{
			count++;
			RenderNode(node->children[i]); // ��� ����
		}
	}

	if (count != 0) return; // �ڽ� ���� �ɰ��� ��忴���� ���� �� �̻� ���� ����
							// �ݴ�� �θ� ��带 ���� �����ų ���� �ش� �κп��� �ڵ� �߰� �ۼ�

	// ������� ���� �ڽ� ����� �����ϰ�, �ش� �ڽ� ���μ��� ���� ����
	node->mesh->Draw(); // �޽��� ���
	drawCount += node->triangleCount; // ��尡 ���� �־��� �ﰢ�� ������ ��ο� �� �ջ꿡 �߰� (�׽�Ʈ��)
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
	// ���õ� ���� �ȿ�, �ε����� ���õ� �ﰢ��(����)�� �������� �ϴ� �ﰢ���� �ִ��� ���� �Ǻ� �Լ�

	UINT vertexIndex = index * 3; // �ﰢ���� ������ �ε���
	float halfSize = size * 0.5f; // ã�� ���� ���õ� size�� ���� (�߽ɿ��� ���۵Ǵ� ������ �ִ� �Ÿ�)

	// �� ������ �����ͼ� �˻��� �غ�
	float x1 = vertices[vertexIndex].pos.x; // ������ ���� �����Ϳ� �ִ� ���� �������� ��ġ
	float z1 = vertices[vertexIndex].pos.z;

	vertexIndex++; // ���� �ε��� �˻�
	float x2 = vertices[vertexIndex].pos.x;
	float z2 = vertices[vertexIndex].pos.z;

	vertexIndex++; // ���� �ε��� �˻�
	float x3 = vertices[vertexIndex].pos.x;
	float z3 = vertices[vertexIndex].pos.z;

	// -> �� ���� �� x, z�� �Ҽ��� ����

	// �ش� ������ ����(�߽� x, z�κ��� �ִ� �Ÿ� half) ���� ������ -> �ﰢ���� ���� �ȿ� ����

	float minX = min(x1, min(x2, x3)); // x1, x2, x3 �� �ּҰ� minX�� ����
	if (minX > (x + halfSize)) return false; 
	// ���� ���ʿ� �־�� �� ���� ���� �����ʺ��� �������̸� -> �ﰢ���� �ش� ������ ����

	float minZ = min(z1, min(z2, z3));
	if (minZ > (z + halfSize)) return false;

	float maxX = max(x1, max(x2, x3));
	if (maxX < (x - halfSize)) return false;

	float maxZ = max(z1, max(z2, z3));
	if (maxZ < (z - halfSize)) return false;

	// ������� ���� ������ �ﰢ���� �����ϴ� ���� �ϳ��� ���� ��Ȳ
	// -> �� ���, ������ �ﰢ���� ���� �ӿ� ����.

	// ������� ���� �ϴ� �ﰢ���� �� �� �� �ϳ��� ���� ���� �ִ� ��
	// �� ����, �κ� ���� ���θ� ���캼 �� �ִ�

	bool countBoundary = true; // ������ �����ϱ� ���� �ɼ�

	if(!countBoundary) // ������ �������� �ʰڴٸ�
	{
		// �� �ȿ��� ���� �ɼ� : �ﰢ�� �� �� �ϳ��� ������ ���� �ִ°�? (�������� ��� ��)

		float maxX = max(x1, max(x2, x3));
		if (maxX > (x + halfSize)) return false; // ���� ū ���� ���� ���̴� = �� �ϳ��� ���� ���̴�

		float maxZ = max(z1, max(z2, z3));
		if (maxZ > (z + halfSize)) return false;

		float minX = min(x1, min(x2, x3));
		if (minX < (x - halfSize)) return false;

		float minZ = min(z1, min(z2, z3));
		if (minZ < (z - halfSize)) return false;
	}

	// ���� ���� ���ο� ������� ������� �Դٸ�,
	// �ﰢ���� �̷�� �� ���� ������ ������ ������ �Ǵ� ���ؿ� ������ ������ ��
	// -> false�� �� ���� ���� �ʾҴ�

	// ���� ������ �����ٸ� : ��� �ϳ� �̻��� ���� ���� �ȿ� �ִ�
	// ������ �����Ѵٸ� : ��� ���� ���� �ȿ� �ִ�
	
	return true;
}

UINT QuadTreeTerrain::ContainedTriangleCount(float x, float z, float size)
{
	// size�� ���� ���� ����, �ﰢ�� ������ �� �� �����ϴ°�?

	// ��� 1 : ���� ������ �ȿ� ���� �ﰢ�� ��ǥ �� �ε��� ������ ����� �״ٰ�, ���� �԰ݿ� ���� �д´�.
	// ��� 2 : ��� �ﰢ���� ���� �������鼭 ���ǿ� �����ϴ��� Ȯ���Ѵ�. ������ +1

	// -> ������ ���� ������ ��� �ϳ��ϳ� ���� ����� ���� �ִ� ������ ��� 1�� �����Ƿ�
	//	  ��� 2�� ���ؼ� �ﰢ�� ������ �������� ���Ѵ� (-> ����� �� �������� ȯ���� �ִٸ� �ű⼱ ������ ȣ��)

	UINT count = 0;

	FOR(triangleCount) // ���� Ŭ���� ���� ��ϵ� �ﰢ�� ����� ��� �ݺ������� ������
	{
		if (IsTriangleContained(i, x, z, size)) // i��° �ﰢ���� ���õ� ���� �ȿ� �־�����
			count++; // ���� +1
	}
	
	// ó������ ������ ���� ���� ���� �ȿ� �ﰢ���� �� �� �ִ��� �Ǻ� ����
	// -> �޸𸮸� ���� ����ϰ� �ܼ��� ���������, ������ �ʴ�.
	// -> �����ٳ� �� ������ ���ؼ� ���߿� ������ ��� ���� �Ǻ��� ���� �ִٸ� ������ �ӵ��δ� �̵���
	// -> �׷��� ���� Ʈ�������� (��� �ﰢ�� �������� �ƴ϶�) ��� ��ü�� ������� �ϴ� ���� �Ǻ� �Լ��� �̿��Ѵ�

	return count; // �Ǻ��� �ﰢ�� ���� ��ȯ

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

	node->triangleCount = triangles; // ��尡 �����־��� �ﰢ���� ����
									 // ó�� �ۼ� : Ŭ���� �� ��� (triangleCount)
									 // ���� -> triangles : ���� ������ �ﰢ�� ����
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
