#pragma once

class ModelExporter
{
public:
	ModelExporter(string name, string file);
	~ModelExporter();

	void ExportMaterial();
	void ExportMesh();
	void ExportClip(string clipName); // ������ �� ������ ���� ���� �̸��� ����ϵ���

private:
	// ���� �а� ��������(���ο� ���Ϸ� �籸���ؼ� �����ϱ�) ���� �Լ���
	// ��Ƽ����
	void ReadMaterial();
	void WriteMaterial();
	string CreateTexture(string file);

	// �޽�
	void ReadMesh(aiNode* node);
	void ReadNode(aiNode* node, int index, int parent);
	void ReadBone(aiMesh* mesh, vector<VertexWeights>& vertexWeights);
	void WriteMesh();

	// Ŭ��(���� : ���Ŀ� �� ��)
	Clip* ReadClip(aiAnimation* animation);
	void ReadKeyFrame(Clip* clip, aiNode* node, vector<ClipNode>& clipNodes);
	void WriteClip(Clip* clip, string clipName, UINT index);

private:
	// �����͸� �б� ���� �غ�
	Assimp::Importer* importer;
	const aiScene* scene;

	string name;

	// ���� �����͵�
	vector<Material*> materials;
	vector<MeshData*> meshes;
	vector<NodeData*> nodes;
	vector<BoneData*> bones;

	map<string, UINT> boneMap;
	UINT boneCount = 0;
};

