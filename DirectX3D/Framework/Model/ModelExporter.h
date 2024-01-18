#pragma once

class ModelExporter
{
public:
	ModelExporter(string name, string file);
	~ModelExporter();

	void ExportMaterial();
	void ExportMesh();
	void ExportClip(string clipName); // 동작은 각 동작이 가진 고유 이름을 사용하도록

private:
	// 모델을 읽고 내보내기(새로운 파일로 재구성해서 저장하기) 위한 함수들
	// 매티리얼
	void ReadMaterial();
	void WriteMaterial();
	string CreateTexture(string file);

	// 메쉬
	void ReadMesh(aiNode* node);
	void ReadNode(aiNode* node, int index, int parent);
	void ReadBone(aiMesh* mesh, vector<VertexWeights>& vertexWeights);
	void WriteMesh();

	// 클립(동작 : 이후에 쓸 것)
	Clip* ReadClip(aiAnimation* animation);
	void ReadKeyFrame(Clip* clip, aiNode* node, vector<ClipNode>& clipNodes);
	void WriteClip(Clip* clip, string clipName, UINT index);

private:
	// 데이터를 읽기 위한 준비
	Assimp::Importer* importer;
	const aiScene* scene;

	string name;

	// 모델의 데이터들
	vector<Material*> materials;
	vector<MeshData*> meshes;
	vector<NodeData*> nodes;
	vector<BoneData*> bones;

	map<string, UINT> boneMap;
	UINT boneCount = 0;
};

