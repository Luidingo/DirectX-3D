#include "Framework.h"

ModelExporter::ModelExporter(string name, string file) : name(name)
{
	importer = new Assimp::Importer();

	importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	importer->SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_TANGENTS_AND_BITANGENTS);
	// SetProperty<�ڷ���> �Լ� : assimp �����Ϳ��� ���� �� �ɼ��� �����ϴ� �Լ�
	// AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS : "FBX ������ ���� ���� �ִ� ȸ���� ������ �����ñ��?"
	// AI_CONFIG_PP_RVC_FLAGS : "���ҽ� �����͸� ������ ��, ȸ�� ������ ���� ����� ������ ��� �ұ��?"
	// aiComponent_TANGENTS_AND_BITANGENTS : "X, Y, Z ��ǥ�� �̿� ���� ���� ���ַ�." (= �ﰢ�Լ��� ź��Ʈ, ����ź��Ʈ)
	// -> �� �ܿ��� �� ���� �ɼ�, �Լ��� ������... �ܿ�ٴ� �� ������ ���������̰�, �ʿ��ϸ� ���� �ڵ带 Ű���� ��� ã�ƺ��ô�.

	scene = importer->ReadFile(file,				// ���� ����
		aiProcessPreset_TargetRealtime_MaxQuality |	// �� ���� �׷��� ���� : �ֻ�
		aiProcess_Triangulate |						// ���� ���� : �ﰢ��
		aiProcess_GenSmoothNormals |				// ���� ������ ���ɿ� ������ �� ���� ������
		aiProcess_FixInfacingNormals |				// ���� �������� ī�޶� ���ϴ� ������ ��ġ�� (���꿡�� ����)
		aiProcess_RemoveRedundantMaterials |		// ���Ͽ��� �������� ��Ƽ������ �־����� ���꿡�� ����
		aiProcess_OptimizeMeshes |					// �޽� ����(������, ���� �ߺ� ��) ����ȭ�ϱ� (AssImp �������)
		aiProcess_ValidateDataStructure |			// ������ ����ü���� �´� �͸� �����Ű�� (������ �˻�)
		aiProcess_ImproveCacheLocality |			// �����͸� �޸𸮿� ���� �� �ǵ��� ������ �����ϱ�
		aiProcess_JoinIdenticalVertices |			// ��� ������ �ߺ��� ������ ������ �׳� �ϳ��� ����ϱ�
		aiProcess_FindInvalidData |					// �������� �����Ͱ� �ִ��� �� �˻��ϱ�
		aiProcess_TransformUVCoords |				// �ؽ�ó�� ���� UVW ��ǥ�� ������ �װ͵� �б� (UVW = �ؽ�ó�� XYZ ��ǥ)
		aiProcess_FlipUVs |							// UV�� �о��µ� ������ ���´ٸ� �������
		aiProcess_ConvertToLeftHanded				// �޼� ��ǥ�谡 �ƴ϶�� �޼� ��ǥ��� �ٲ��
	);
	// ���� ���� ������ ���� ó���� ��ġ�鼭 ������ �о���δ�

	// �� �������� ������ �־����� �ͽ����� ���� ���� (=�������� FBX�� �а� �ʹ�)
	assert(scene != nullptr);
}

ModelExporter::~ModelExporter()
{
	delete importer;
}

//�ͽ���Ʈ ��� ��ü�� ����
//�а� -> ���� -> �� (�߿��� �� �б�� ����)

void ModelExporter::ExportMaterial()
{
	ReadMaterial();
	WriteMaterial();
	// ��
}

void ModelExporter::ExportMesh()
{
	ReadNode(scene->mRootNode, -1, -1); // FBX���� ���� �� ������ ������ ��������, �ε��� ���� ����
	ReadMesh(scene->mRootNode);
	WriteMesh();
}

void ModelExporter::ExportClip(string clipName)
{
	FOR(scene->mNumAnimations) // FBX���� �����Ǿ��� �ִϸ��̼� ������ŭ
	{
		Clip* clip = ReadClip(scene->mAnimations[i]);
		WriteClip(clip, clipName, i);
	}
}

void ModelExporter::ReadMaterial()
{
	FOR(scene->mNumMaterials)
	{
		aiMaterial* srcMaterial = scene->mMaterials[i]; // srcMaterial : ����(�ҽ�)

		Material* material = new Material();

		material->GetName() = srcMaterial->GetName().C_Str();

		aiColor3D color;
		Material::MaterialBuffer::Data& data = material->GetData(); // ��Ƽ������ �����͸� ���Ӹ��� �缱��

		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color); // ������ ���� ���� Ű���带 color ������ �ٲپ ������
		data.diffuse = Float4(color.r, color.g, color.b, 1); // ����(��ǻ��) ���� �ֱ�

		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color); // �ݻ簭��(����ŧ��) ���
		data.specular = Float4(color.r, color.g, color.b, 1); // ���� �ֱ�

		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color); // ������ (�⺻ ����)
		data.ambient = Float4(color.r, color.g, color.b, 1);

		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color); // �⺻ ���(�̹̽ú�)
		data.emissive = Float4(color.r, color.g, color.b, 1);

		srcMaterial->Get(AI_MATKEY_SHININESS, data.shininess); // ��ü ���

		aiString file;
		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file); 
			// ������ �����ͼ� �ؽ�ó�� �����ϰ� ����
		material->SetDiffuseMap(ToWString(CreateTexture(file.C_Str()))); 
			// ������� ���Ͽ� ���� �ؽ�ó�� �����, �ٽ� ��Ƽ���� ����ϱ�
		file.Clear(); // ���� ���� ����� �ʱ�ȭ

		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SetSpecularMap(ToWString(CreateTexture(file.C_Str())));
		file.Clear();

		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->SetNormalMap(ToWString(CreateTexture(file.C_Str())));
		file.Clear();

		// ������� ������ ������ ��μ� ��Ƽ���� ������ �ϼ��ȴ� (�ؽ�ó + ��� ���� �ǰ� ���� ��ġ)
		materials.push_back(material);
	}
}

void ModelExporter::WriteMaterial()
{
	string savePath = "Models/Materials/" + name + "/";
	string file = name + ".mats";

	CreateFolders(savePath); // ������ ������ �����, ������ �״�� �δ� ������ �Լ�
	
	BinaryWriter* writer = new BinaryWriter(savePath + file); // 2�� ���� Ŭ���� : �б� Ŭ������ �ݴ�
															 // �Ű������� ��θ� (������ + ���ϸ�)

	writer->UInt(materials.size()); // ���� ���� �о�� �� Ƚ�� = ��Ƽ������ ����� ����
									// -> �׷��� ���� ������ �� ���� ���� ����� �д� ��

	for (Material* material : materials)
	{
		string path = savePath + material->GetName() + ".mat";
		material->Save(path); // ��Ƽ���� ������ ���Ϸ� �����ϰ� path �����Ϳ� ��θ� ����

		writer->String(path); // ���� ���� ��θ��� ��ü �𵨿� ���Ͽ� ���ܼ� �ٽ� �ε��� �� �ֵ��� �غ�
							  // = ���� �����ϴ� .mat ������ ���� ������, ���ϸ��� �� �Ŵ��� �����ͷ� ���� ��

		// ������� ���� ��Ƽ������ �а�, ���Ϸ� �Ἥ ����������� �� ��
		// �׷��Ƿ� �뵵�� ���� ��Ƽ���� �����͸� �����ص� �ȴ�

		delete material;
		// �׷��� �ϳ��ϳ� ���� �� �����͸� �ı��Ѵ�
	}

	// ��� ���Ⱑ ���� ��, ���� ��Ұ� ��� ����� ���͵� �ʱ�ȭ�ѵ�
	materials.clear();

	delete writer; // ���� ��ü�� ����
}

string ModelExporter::CreateTexture(string file)
{
	// ��Ƽ���� ������ �ؽ�ó�� ����� �� �̸��� ��ȯ�ϴ� �Լ�
	// -> ���� ��Ƽ���� �б� �Լ����� ȣ��

	if (file.length() == 0) return ""; // ���� ������ ������ �ƹ� �͵� ���� ���ڿ��� ��ȯ

	string fileName = GetFileNameWithoutExtension(file) + ".png"; // ���� �̸� �߶� �ޱ�
	string path = "Textures/Model/" + name + "/" + fileName; // �� ������ ���� �̸� �����ϱ�

	CreateFolders(path);

	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str()); 
			// ���ο� ���Ե�(Embedded) �ؽ�ó�� ������ �ޱ�

	if (texture == nullptr) return ""; // �׷��� ���� �ؽ�ó�� �������� ����

	if (texture->mHeight < 1) // ���ΰ� ���� Ư���� �ؽ�ó���
	{
		BinaryWriter writer(path); // ������ ���� ��� ���� �뵵�� ���� Ŭ����
		writer.Byte(texture->pcData, texture->mWidth); // �ؽ�ó�� �ȼ� �����Ϳ� �ʺ� ������ �ֱ�
	}
	else // ���ΰ� �ִ� ����� �̹��� �ؽ�ó���
	{
		Image image; // DX�� �̹��� Ŭ����
		image.width = texture->mWidth;
		image.height = texture->mHeight;
		image.pixels = (uint8_t*)(texture->pcData); // RGBA�� �ٲ� ������ �ȼ� ������
		image.rowPitch = image.width * 4; // ���� ��ġ�� 4��� �ֱ�
		image.slicePitch = (image.width * image.height) * 4; // �����̽� ��ġ�� 4��� �ֱ�

		// DX �̹��� ������ ��� ä�� ��, ���Ϸ� ���� (���̸� ���� ���� ����ϰ�)
		SaveToWICFile(image, WIC_FLAGS_NONE, // �̹�����, ������ ���� ����,
			GetWICCodec(WIC_CODEC_PNG), ToWString(path).c_str()); // PNG��, path ��ġ��
	}

	return path; // ��θ� ��ȯ
}

void ModelExporter::ReadMesh(aiNode* node)
{
	// �޽� �б�

	FOR(node->mNumMeshes)
	{
		MeshData* mesh = new MeshData();
		mesh->name = node->mName.C_Str();

		UINT index = node->mMeshes[i]; // ������ ������ �޽� ��ȣ ã��
		aiMesh* srcMesh = scene->mMeshes[index]; // ��ȣ�� ���� �޽� ������ ã��

		mesh->materialIndex = srcMesh->mMaterialIndex; // ������ ��Ƽ���� ��ȣ�� �޽� �����Ϳ� ����

		vector<VertexWeights> vertexWeights(srcMesh->mNumVertices); // ���ؽ� ���ڸ�ŭ ����ġ ���� �����
		ReadBone(srcMesh, vertexWeights); // �ؿ� �ִ� �� �б� �Լ��� ��� ȣ���ϱ�

		mesh->vertices.resize(srcMesh->mNumVertices); // ���� �������� (���� �����͸� ���Ը� �ϸ� �Ǵϱ�)
		//FOR(srcMesh->mNumVertices) <- �Ұ��� : ��ũ�� FOR�� i ������ �����ϴϱ�
		for (int v = 0; v < srcMesh->mNumVertices; ++v)
		{
			ModelVertex vertex;

			memcpy(&vertex.pos, &srcMesh->mVertices[v], sizeof(Float3)); // ���� ���� ������ ���� ��ġ ������ ����

			if (srcMesh->HasTextureCoords(0)) // UV��ǥ�� �迭�� �ϳ� �̻� ������
				memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Float2)); // W�� �� ��
			
			if (srcMesh->HasNormals()) // ���� ������ �־�����
				memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Float3));

			if (srcMesh->HasTangentsAndBitangents()) // ź��Ʈ �� ������ ������
				memcpy(&vertex.tangent, &srcMesh->mTangents[v], sizeof(Float3));

			if (!vertexWeights.empty()) // �̸� ���Ͽ� ������ ����ġ ������ �־��ٸ�
										// -> ��� ������ ���� �� ���� ��
			{
				vertexWeights[i].Normalize(); // i��° ���� �ֺ� ���� ����ġ�� ����ȭ

				vertex.indices.x = (float)vertexWeights[v].indices[0]; // ����ġ�� x
				vertex.indices.y = (float)vertexWeights[v].indices[1]; // ����ġ�� y
				vertex.indices.z = (float)vertexWeights[v].indices[2]; // ����ġ�� z
				vertex.indices.w = (float)vertexWeights[v].indices[3]; // ����ġ�� ���� ����

				vertex.weights.x = vertexWeights[v].weights[0];
				vertex.weights.y = vertexWeights[v].weights[1];
				vertex.weights.z = vertexWeights[v].weights[2];
				vertex.weights.w = vertexWeights[v].weights[3];
			}
			// ������� ����, ���� ���� ���� ��ġ, ����, ����, �ؽ�ó ����, ����ġ ���� ���� ��� ��

			// ...���� 1���� ���´�

			// �� ���� 1���� ���Ϳ� �߰�
			mesh->vertices[v] = vertex;
		}

		// ���� ������ �������� �ε��� ����� ���� ����

		mesh->indices.resize(srcMesh->mNumFaces * 3); // �޽��� ���� "��"�� ���� x 3
		for (UINT f = 0; f < srcMesh->mNumFaces; ++f)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; ++k) // k�� �ٽ� �ش� ���� ���� �ε��� ���ڱ��� �ݺ�
			{
				// �ε����� (��x3)+�����ε��� ��ġ�� ���� �ε��� ���� ���� �����Ѵ�
				// Ȥ�� .... "�ε����� ������ ���� 1�� �ٲ� ������ 3�� �ǳʶٰ�,
				// k�� �ٲ� ������ 1�� �ö󰣴�"
				mesh->indices[(f * 3) + k] = face.mIndices[k]; // �ش� ��Ͽ� ���� ������ �ε����� ����
			}
		}

		// ������� ���� �ε��� ���� ��
	
		// = �޽��� �ʿ��� ���� ���� ��

		// �� �޽��� �޽� ���Ϳ� �ִ´�

		meshes.push_back(mesh);
	}

	// ������� ���� �Ű������� �޾Ҵ� ���� �ϳ��� ���� ��� �޽� ������ ���´�

	// -> �̿� ���� ���� ���� �޽� ������ �ϼ�....���� �ʴ´�.
	//    �ֳĸ� ���� ������ �� ���� ���� �ϳ��ϱ�.

	// -> ���� ���� �ؿ� �ڽ� ������ �� �ִٸ� �� ������ ���� ������ �ؾ� �Ѵ�

	for (UINT c = 0; c < node->mNumChildren; ++c) // ���� ������ �ڽ� ��� ���ڸ�ŭ
	{
		ReadMesh(node->mChildren[c]); // �ڽ� ��� �ϳ��ϳ����� ����Լ��� �޽� ���� �� �ϱ�
	}

	// ������� ���� ��, ��μ� ���� ������ ���� �޽� ������ ������ �ȴ�
	// (�׸��� ó�� ���� ��尡 ���� root��� -> ��ü �޽� ������ ����)
}

void ModelExporter::ReadNode(aiNode* node, int index, int parent)
{
	// ��� ������ ���� ���� �Ű����� �����ϱ�
	NodeData* nodeData = new NodeData();
	nodeData->index = index;
	nodeData->parent = parent;
	nodeData->name = node->mName.C_Str();

	// Ʈ������ ������ �����ϱ�
	Matrix matrix(node->mTransformation[0]); // ���� ����� Ʈ������
	nodeData->transform = XMMatrixTranspose(matrix); // ������ ����

	// ������ ��� �����͸� ���Ϳ� �ְ�
	nodes.push_back(nodeData);

	// �ڽ� ��尡 �ִٸ� (�ڽ� ��� ������ 0���� ũ��) �ݺ��� + ����Լ��� �Ȱ��� ��� �б� ����
	FOR(node->mNumChildren)
		ReadNode(node->mChildren[i], nodes.size(), index);
}

void ModelExporter::ReadBone(aiMesh* mesh, vector<VertexWeights>& vertexWeights)
{
	FOR(mesh->mNumBones)
	{
		UINT boneIndex = 0; // ���� �ε��� �ʱ�ȭ
		string name = mesh->mBones[i]->mName.C_Str(); // �̸� �ޱ�

		if (boneMap.count(name) == 0) // ������ �̸��� �����ϴ� (���� �����) ���밡 �� �ʿ� ��������
		{
			boneIndex = boneCount++; // 0���� �������� �� ī��Ʈ�� ���� �������ְ�, �̾ ī��Ʈ�� �� +1
			boneMap[name] = boneIndex; // ���� �̸����� �� �� �� �ʿ� �߰��Ѵ� (name, boneIndex)

			BoneData* boneData = new BoneData();
			boneData->name = name;
			boneData->index = boneIndex;

			Matrix matrix(mesh->mBones[i]->mOffsetMatrix[0]); // ���� Ʈ������
			boneData->offset = XMMatrixTranspose(matrix);

			// ������ ���븦 ���Ϳ� �ֱ�
			bones.push_back(boneData);

			// ������� �����ϸ� ���� �����͸� ��� ��Ƽ� ���Ϳ��� �ְ� �ʿ��� �ְ�....�� ��
		}
		else
		{
			// ����� ���Դٴ� ���� ������ �̸��� �����ϴ� ���밡 �ʿ� �־��ٴ� �̾߱�
			// -> ���ٿ��� �ڵ带 ���ؼ� ���� �����͵� �ε����� Ʈ�������� �� �޾���ٴ� �̾߱�
			// ���� ����� ���ؼ� ������ �ε����� �� �ȿ��� �ٽ� ������ �ȴ�
			boneIndex = boneMap[name];
		}

		// ������� ���� �� ���� �����Ͱ� ���δ�
		// 1. ���� ������ �� ��ü
		// 2. ����鿡 ���� 2�� ������ (����, ����, �� ������� �� �ε���...)
		// 3. ������ ������ �ε��� (boneIndex)

		// TODO : ������ ���� boneIndex�� �ʿ��� �߰� ������ Ȥ �ִٸ� ���⼭ ���� �ȴ�

		// -> �� �Լ����� ������ �� �� : ������ ����ġ�� ã�Ƽ� ���� (���� ����ġ �غ�)
		for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
		{
			UINT index = mesh->mBones[i]->mWeights[j].mVertexId; // ����ġ�� ������ �Ǿ�� �� ������ ID
			vertexWeights[index].Add(boneIndex,			// ������ ������ �ε��� = ���� ���п� (��ü ���� ID)
				mesh->mBones[i]->mWeights[j].mWeight);	// �ش� ��ü�� ������ i��° ������ j��° ���� ����ġ��
														// ���� index�� �����ϵ��� �߰��Ѵ�
		}
	}
}

void ModelExporter::WriteMesh()
{
	// ���ݱ��� ���� �����͸� .mesh ���Ϸ� ���� �ٸ� ������ �� �� �ְ� ���

	string path = "Models/Meshes/" + name + ".mesh";

	CreateFolders(path); // ��� (�� ����) ������ �����

	BinaryWriter* writer = new BinaryWriter(path);

	writer->UInt(meshes.size()); // �޽��� �󸶳� �о�� �������� ����
	for (MeshData* mesh : meshes)
	{
		// �� cpp���� �о��� ������ ������� �̹����� ����

		writer->String(mesh->name); // �޽��� �̸�
		writer->UInt(mesh->materialIndex); // �޽��� ����� ��Ƽ����...�� �ε���

		writer->UInt(mesh->vertices.size()); // ���� �� ����
		writer->Byte(mesh->vertices.data(), sizeof(ModelVertex) * mesh->vertices.size()); // ���� ���� ����

		writer->UInt(mesh->indices.size()); // �ε��� ��� ����
		writer->Byte(mesh->indices.data(), sizeof(UINT) * mesh->indices.size()); // ���� �ε��� ���

		// �� �� �޽� ������ ����
		delete mesh; // ������ �ͽ����� ������ �� �� �� ����. ���� ���� .mesh���� �ٽ� �д´�
	}
	meshes.clear(); // ���� �ʱ�ȭ

	// ��� ����
	writer->UInt(nodes.size());
	for (NodeData* node : nodes)
	{
		writer->Int(node->index);			// ����� �ε���
		writer->String(node->name);			// ����� �̸�
		writer->Int(node->parent);			// ����� �θ�
		writer->Matrix(node->transform);	// ����� Ʈ������ ����

		delete node;
	}
	nodes.clear();

	writer->UInt(bones.size());
	for (BoneData* bone : bones)
	{
		writer->Int(bone->index);		// ���� �ε���
		writer->String(bone->name);		// ���� �̸�
		writer->Matrix(bone->offset);	// ���� ����ġ (Ʈ������)
	}
	bones.clear();

	// �޽� ���� ��, ��� ���� ��, ���� ���� ��....������ ���� ��

	delete writer;
}

Clip* ModelExporter::ReadClip(aiAnimation* animation)
{
	Clip* clip = new Clip(); // ���� ����

	// �������� ������ �޾ƿͼ� �����ϱ�
	clip->name = animation->mName.C_Str();
	clip->tickPerSecond = (float)animation->mTicksPerSecond;
	clip->frameCount = (UINT)(animation->mDuration) + 1; // 0�� ù �������̹Ƿ�

	vector<ClipNode> clipNodes;
	clipNodes.reserve(animation->mNumChannels);
	FOR(animation->mNumChannels)
	{
		aiNodeAnim* srcNode = animation->mChannels[i]; // ������ i��° ��� �ޱ�

		ClipNode node;
		node.name = srcNode->mNodeName; // �������� ���� �κ� ������ �̸�
		
		// ��� �� ������ Ʈ������ ���� ��� �߿��� ���� ���� ��(��ġ, ȸ��, ũ��)�� Ű�� �� ���� ����
		UINT keyCount = max(srcNode->mNumPositionKeys, srcNode->mNumRotationKeys);
		keyCount = max(keyCount, srcNode->mNumScalingKeys);

		node.transforms.reserve(clip->frameCount); // Ŭ���� ������ ����ŭ ��� ����

		KeyTransform transform;
		for (UINT k = 0; k < keyCount; ++k) // ������ ������ ���� ���� Ű ���ڸ�ŭ �ݺ�
		{
			bool isFound = false; // ���ǿ� �´� ������ �־����� ���� �ʱ�ȭ
			float t = node.transforms.size();

			if (k < srcNode->mNumPositionKeys // k�� ���� ��ġ�� ����� �� �����
				&& NearlyEqual((float)srcNode->mPositionKeys[k].mTime, t))
				// ������ ��ġ ������ ���� ����� ��� ���� ���̶��
			{
				aiVectorKey key = srcNode->mPositionKeys[k]; // ai�� ���ͷ� ��ġ ���� �ޱ�
				// ai�� ���ͷ� �����ϴ� ���� ����� �����͸� �۾� ���� Ʈ�������� ������
				memcpy_s(&transform.pos, sizeof(Float3), &key.mValue, sizeof(aiVector3D));

				// ��·�� ���ǿ� �´� ������ �־��ٰ� üũ
				isFound = true;
			}

			// ���� ����� ����� ȸ��, ũ�⿡�� ����

			if (k < srcNode->mNumRotationKeys
				&& NearlyEqual((float)srcNode->mRotationKeys[k].mTime, t))
			{
				aiQuatKey key = srcNode->mRotationKeys[k];
				transform.rot.x = (float)key.mValue.x;
				transform.rot.y = (float)key.mValue.y;
				transform.rot.z = (float)key.mValue.z;
				transform.rot.w = (float)key.mValue.w;

				isFound = true;
			}

			if (k < srcNode->mNumScalingKeys
				&& NearlyEqual((float)srcNode->mScalingKeys[k].mTime, t))
			{
				aiVectorKey key = srcNode->mScalingKeys[k];
				memcpy_s(&transform.scale, sizeof(Float3), &key.mValue, sizeof(aiVector3D));

				isFound = true;
			}

			if (isFound) // ���� ���ǿ� �´� ������ �־��ٸ�...
			{
				// ��·�� �κ������ζ� (Ȥ�� ������) Ʈ������ ������ ���� Ű �������� ����
				// �߰��� �� ��
				// �׷��Ƿ� ���Ϳ� �ִ´�
				node.transforms.push_back(transform);
			}
		}

		// ������� ���鼭 �ݺ����� ��ġ�� �����̶� ������ ���� Ʈ������ ��ȭ ����� �ִ� ����
		// ������ �ܾ ���ͷ� ����� �ȴ�

		// �� ���͸� Ŭ������ ��ϵ� ��ü ������ ���� �ٽ� �񱳸� �Ѵ�
		if (node.transforms.size() < clip->frameCount) // �׷��� �ܾ�͵� ���ڶ��
		{
			UINT count = clip->frameCount - node.transforms.size(); // ���̸� ����
			KeyTransform keyTransform = node.transforms.back(); // ������ ã�� (Ű)Ʈ������ ����� ������ ��Ҹ� ����

			for (int d = 0; d < count; d++) // ���̸�ŭ 
				node.transforms.push_back(keyTransform); // ��� ���Ϳ� ������ Ű Ʈ�������� �߰�
		}

		// Ű Ʈ�������� ���� ��带 ������ �ִ� ����� ���Ϳ� �߰�
		clipNodes.push_back(node);
	}
	// ���⼭ �������� �������� �޶����� �� Ʈ�������� ��� = Ű Ʈ�������� �� ����� ����� ���´�
	
	// ���� ���� ��带 ���� ��Ƽ� Ű ������(������ �ڼ�)�� �д´�

	ReadKeyFrame(clip, scene->mRootNode, clipNodes);
	// clipNode�� �����Ͽ�, mRootNode�κ���, clip�� Ű �������� ������ �ҷ��´�

	// ������� ���� (������ �� ������) clip�� ���������

	return clip;
}

void ModelExporter::ReadKeyFrame(Clip* clip, aiNode* node, vector<ClipNode>& clipNodes)
{
	KeyFrame* keyFrame = new KeyFrame();
	keyFrame->boneName = node->mName.C_Str();
	keyFrame->transforms.reserve(clip->frameCount); // ������ ����ŭ �ڸ� ����

	FOR(clip->frameCount)
	{
		ClipNode* clipNode = nullptr;		// �۾��� Ŭ�� �� ��� �غ�
		for (ClipNode& tmp : clipNodes)		// �Ű������� ���� ��� ���� �˻�
		{
			if (tmp.name == node->mName)	// �Ű������� ����(����) ���� �̸��� ���� ���� ������
			{ 
				clipNode = &tmp;			// �װ��� �۾��� ��(������)�̴�
				break;						// �˻� ����
			}
		}

		KeyTransform keyTransform;
		if (clipNode == nullptr) // ���� ��� ������ ��������
		{
			Matrix transform(node->mTransformation[0]); // �������� �ٽ� �����(��Ʈ ����)
														// �� ��� ���� ����� ���� �������� �� �������� ���� ��츦 ����ؼ�
			transform = XMMatrixTranspose(transform);	// DX �԰� �������� ��� ��Ҹ� ���� -> �籸��

			// ���� ��� �غ�
			Vector3 S;
			Vector3 R;
			Vector3 T;
			// DX �԰ݿ� �������� ������ ����� �ٽ� �����ؼ� ���͸� �����Ѵ�
			XMMatrixDecompose(S.GetValue(), R.GetValue(), T.GetValue(), transform);
					// �Ű����� : ��� ũ�� ��, ��� ȸ�� ��, ��� ��ġ(����) ��, ����

			// ���͸� ����ü�� ����
			keyTransform.scale = S;
			XMStoreFloat4(&keyTransform.rot, R);
			keyTransform.pos = T;

			// �̷��� ���� �������� Ʈ�������� ������ �������� ��� �� �غ� �Ѵ�
		}
		else // Ʈ�������� �ִ� ���
		{
			keyTransform = clipNode->transforms[i]; // ������ ���� ������ ��� ������, i��° Ʈ������
		}

		keyFrame->transforms.push_back(keyTransform); // ���Ϳ� �߰�
	}
	// ������� ����, Ʈ������(��� ����)�� ��� ���� Ű ������(���� �ڼ�)��
	// �������� ���� ���Ϳ� �ִ´� (������ ��� ��, ������ ������ ��� �ڼ�, �ð��� �ڼ��� ������ ������)
	clip->keyFrame.push_back(keyFrame);

	FOR(node->mNumChildren) // �� ��忡 �ڽ� ��尡 �־�����...
		ReadKeyFrame(clip, node->mChildren[i], clipNodes);
		// �ڽ� ��带 ���� �Ȱ��� ������ �ݺ� (�ڽ��� ���� ���� ������)
}

void ModelExporter::WriteClip(Clip* clip, string clipName, UINT index)
{
	// Ŭ���� ���� �� ���� ����, ���Ϸ� �����ϱ�

	string file = "Models/Clips/" + name + "/" + clipName + to_string(index) + ".clip";

	CreateFolders(file);

	BinaryWriter* writer = new BinaryWriter(file);

	writer->String(clip->name); // � �����ΰ�
	writer->UInt(clip->frameCount); // �󸶳� �����Ͱ� �ִ°�
	writer->Float(clip->tickPerSecond); // �⺻ ��� �ӵ�(�ʴ� ������ ���ŷ�)�� ���ΰ�

	writer->UInt(clip->keyFrame.size());
	for (KeyFrame* keyFrame : clip->keyFrame)
	{
		writer->String(keyFrame->boneName);
		writer->UInt(keyFrame->transforms.size());
		writer->Byte(keyFrame->transforms.data(), sizeof(KeyTransform) * keyFrame->transforms.size());

		delete keyFrame;
	}

	delete clip;
	delete writer;
}
