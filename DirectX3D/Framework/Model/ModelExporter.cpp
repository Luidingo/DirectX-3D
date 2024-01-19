#include "Framework.h"

ModelExporter::ModelExporter(string name, string file) : name(name)
{
	importer = new Assimp::Importer();

	importer->SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	importer->SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_TANGENTS_AND_BITANGENTS);
	// SetProperty<자료형> 함수 : assimp 임포터에서 수행 중 옵션을 변경하는 함수
	// AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS : "FBX 파일이 원래 갖고 있던 회전축 정보도 가져올까요?"
	// AI_CONFIG_PP_RVC_FLAGS : "리소스 데이터를 가져올 때, 회전 정보와 공간 계산의 기준은 어떻게 할까요?"
	// aiComponent_TANGENTS_AND_BITANGENTS : "X, Y, Z 좌표와 이에 따른 각도 위주로." (= 삼각함수의 탄젠트, 바이탄젠트)
	// -> 이 외에도 더 많은 옵션, 함수가 있지만... 외운다는 건 지금은 비현실적이고, 필요하면 오늘 코드를 키워드 삼아 찾아봅시다.

	scene = importer->ReadFile(file,				// 원본 파일
		aiProcessPreset_TargetRealtime_MaxQuality |	// 모델 연산 그래픽 수준 : 최상
		aiProcess_Triangulate |						// 기준 도형 : 삼각형
		aiProcess_GenSmoothNormals |				// 법선 생성은 성능에 무리가 덜 가는 선에서
		aiProcess_FixInfacingNormals |				// 완전 직각으로 카메라를 향하는 법선은 고치기 (연산에서 빼기)
		aiProcess_RemoveRedundantMaterials |		// 파일에서 부적절한 매티리얼이 있었으면 연산에서 빼기
		aiProcess_OptimizeMeshes |					// 메시 형태(폴리곤, 정점 중복 등) 최적화하기 (AssImp 방식으로)
		aiProcess_ValidateDataStructure |			// 데이터 구조체들을 맞는 것만 통과시키기 (적절성 검사)
		aiProcess_ImproveCacheLocality |			// 데이터를 메모리에 담을 때 되도록 가깝게 저장하기
		aiProcess_JoinIdenticalVertices |			// 모든 정보가 중복인 정점이 있으면 그냥 하나로 계산하기
		aiProcess_FindInvalidData |					// 부적절한 데이터가 있는지 또 검색하기
		aiProcess_TransformUVCoords |				// 텍스처에 대한 UVW 좌표가 있으면 그것도 읽기 (UVW = 텍스처판 XYZ 좌표)
		aiProcess_FlipUVs |							// UV가 읽었는데 음수가 나온다면 뒤집어라
		aiProcess_ConvertToLeftHanded				// 왼손 좌표계가 아니라면 왼손 좌표계로 바꿔라
	);
	// 위와 같이 수많은 보험 처리를 거치면서 파일을 읽어들인다

	// 위 과정에서 오류가 있었으면 익스포터 강제 종료 (=문제없는 FBX만 읽고 싶다)
	assert(scene != nullptr);
}

ModelExporter::~ModelExporter()
{
	delete importer;
}

//익스포트 기능 자체는 간단
//읽고 -> 쓰면 -> 끝 (중요한 건 읽기와 쓰기)

void ModelExporter::ExportMaterial()
{
	ReadMaterial();
	WriteMaterial();
	// 끝
}

void ModelExporter::ExportMesh()
{
	ReadNode(scene->mRootNode, -1, -1); // FBX에서 파일 내 설정된 최초의 부위부터, 인덱스 없이 시작
	ReadMesh(scene->mRootNode);
	WriteMesh();
}

void ModelExporter::ExportClip(string clipName)
{
	FOR(scene->mNumAnimations) // FBX에서 설정되었던 애니메이션 개수만큼
	{
		Clip* clip = ReadClip(scene->mAnimations[i]);
		WriteClip(clip, clipName, i);
	}
}

void ModelExporter::ReadMaterial()
{
	FOR(scene->mNumMaterials)
	{
		aiMaterial* srcMaterial = scene->mMaterials[i]; // srcMaterial : 원본(소스)

		Material* material = new Material();

		material->GetName() = srcMaterial->GetName().C_Str();

		aiColor3D color;
		Material::MaterialBuffer::Data& data = material->GetData(); // 매티리얼의 데이터를 줄임말로 재선언

		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color); // 원본의 색상 정보 키워드를 color 변수로 바꾸어서 빼내기
		data.diffuse = Float4(color.r, color.g, color.b, 1); // 색상(디퓨즈) 정보 넣기

		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color); // 반사강도(스페큘러) 얻기
		data.specular = Float4(color.r, color.g, color.b, 1); // 정보 넣기

		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color); // 배경색상 (기본 색조)
		data.ambient = Float4(color.r, color.g, color.b, 1);

		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color); // 기본 밝기(이미시브)
		data.emissive = Float4(color.r, color.g, color.b, 1);

		srcMaterial->Get(AI_MATKEY_SHININESS, data.shininess); // 전체 밝기

		aiString file;
		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file); 
			// 파일을 가져와서 텍스처에 예약하고 쓰기
		material->SetDiffuseMap(ToWString(CreateTexture(file.C_Str()))); 
			// 만들어진 파일에 따라 텍스처를 만들고, 다시 매티리얼에 등록하기
		file.Clear(); // 파일 내용 지우고 초기화

		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SetSpecularMap(ToWString(CreateTexture(file.C_Str())));
		file.Clear();

		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->SetNormalMap(ToWString(CreateTexture(file.C_Str())));
		file.Clear();

		// 여기까지 문제가 없으면 비로소 매티리얼 정보가 완성된다 (텍스처 + 어떻게 보일 건가 설정 수치)
		materials.push_back(material);
	}
}

void ModelExporter::WriteMaterial()
{
	string savePath = "Models/Materials/" + name + "/";
	string file = name + ".mats";

	CreateFolders(savePath); // 폴더가 없으면 만들고, 있으면 그대로 두는 윈도우 함수
	
	BinaryWriter* writer = new BinaryWriter(savePath + file); // 2진 쓰기 클래스 : 읽기 클래스의 반대
															 // 매개변수는 경로명 (폴더명 + 파일명)

	writer->UInt(materials.size()); // 제일 먼저 읽어야 할 횟수 = 매티리얼의 양부터 적기
									// -> 그렇게 쓰기 때문에 또 읽을 때도 양부터 읽는 것

	for (Material* material : materials)
	{
		string path = savePath + material->GetName() + ".mat";
		material->Save(path); // 매티리얼 정보를 파일로 저장하고 path 데이터에 경로명 저장

		writer->String(path); // 지금 받은 경로명을 전체 모델용 파일에 남겨서 다시 로드할 수 있도록 준비
							  // = 지금 저장하는 .mat 파일은 실제 데이터, 파일명은 곧 거대한 포인터로 쓰는 것

		// 여기까지 오면 매티리얼을 읽고, 파일로 써서 내보내기까지 한 셈
		// 그러므로 용도가 다한 매티리얼 데이터를 삭제해도 된다

		delete material;
		// 그렇게 하나하나 저장 후 포인터를 파기한다
	}

	// 모든 쓰기가 끝난 후, 내부 요소가 모두 사라진 벡터도 초기화한디
	materials.clear();

	delete writer; // 쓰기 객체도 삭제
}

string ModelExporter::CreateTexture(string file)
{
	// 매티리얼 정보를 텍스처로 만들고 그 이름을 반환하는 함수
	// -> 위의 매티리얼 읽기 함수에서 호출

	if (file.length() == 0) return ""; // 원본 파일이 없으면 아무 것도 없는 문자열을 반환

	string fileName = GetFileNameWithoutExtension(file) + ".png"; // 파일 이름 잘라서 받기
	string path = "Textures/Model/" + name + "/" + fileName; // 새 폴더에 파일 이름 배정하기

	CreateFolders(path);

	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str()); 
			// 내부에 포함된(Embedded) 텍스처가 있으면 받기

	if (texture == nullptr) return ""; // 그렇게 받은 텍스처가 없었으면 종료

	if (texture->mHeight < 1) // 세로가 없는 특수한 텍스처라면
	{
		BinaryWriter writer(path); // 포인터 말고 잠깐 쓰는 용도로 쓰기 클래스
		writer.Byte(texture->pcData, texture->mWidth); // 텍스처의 픽셀 데이터와 너비 데이터 넣기
	}
	else // 세로가 있는 평범한 이미지 텍스처라면
	{
		Image image; // DX의 이미지 클래스
		image.width = texture->mWidth;
		image.height = texture->mHeight;
		image.pixels = (uint8_t*)(texture->pcData); // RGBA로 바꿔 연산한 픽셀 데이터
		image.rowPitch = image.width * 4; // 가로 피치를 4배로 주기
		image.slicePitch = (image.width * image.height) * 4; // 슬라이스 피치를 4배로 주기

		// DX 이미지 정보를 모두 채운 후, 파일로 저장 (높이맵 저장 때와 비슷하게)
		SaveToWICFile(image, WIC_FLAGS_NONE, // 이미지를, 별도의 보정 없이,
			GetWICCodec(WIC_CODEC_PNG), ToWString(path).c_str()); // PNG로, path 위치에
	}

	return path; // 경로명 반환
}

void ModelExporter::ReadMesh(aiNode* node)
{
	// 메쉬 읽기

	FOR(node->mNumMeshes)
	{
		MeshData* mesh = new MeshData();
		mesh->name = node->mName.C_Str();

		UINT index = node->mMeshes[i]; // 부위에 대응된 메쉬 번호 찾기
		aiMesh* srcMesh = scene->mMeshes[index]; // 번호에 따른 메쉬 데이터 찾기

		mesh->materialIndex = srcMesh->mMaterialIndex; // 원본의 매티리얼 번호를 메쉬 데이터에 전달

		vector<VertexWeights> vertexWeights(srcMesh->mNumVertices); // 버텍스 숫자만큼 가중치 벡터 만들기
		ReadBone(srcMesh, vertexWeights); // 밑에 있는 뼈 읽기 함수를 끌어서 호출하기

		mesh->vertices.resize(srcMesh->mNumVertices); // 벡터 리사이즈 (원본 데이터를 대입만 하면 되니까)
		//FOR(srcMesh->mNumVertices) <- 불가능 : 매크로 FOR는 i 순번을 전제하니까
		for (int v = 0; v < srcMesh->mNumVertices; ++v)
		{
			ModelVertex vertex;

			memcpy(&vertex.pos, &srcMesh->mVertices[v], sizeof(Float3)); // 원본 정점 정보를 정점 위치 정보에 복제

			if (srcMesh->HasTextureCoords(0)) // UV좌표가 배열상 하나 이상 있으면
				memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Float2)); // W는 안 봄
			
			if (srcMesh->HasNormals()) // 법선 정보가 있었으면
				memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Float3));

			if (srcMesh->HasTangentsAndBitangents()) // 탄젠트 각 정보가 있으면
				memcpy(&vertex.tangent, &srcMesh->mTangents[v], sizeof(Float3));

			if (!vertexWeights.empty()) // 미리 파일에 생성된 가중치 정보가 있었다면
										// -> 멤버 변수를 통해 와 있을 것
			{
				vertexWeights[i].Normalize(); // i번째 점과 주변 점의 가중치를 평준화

				vertex.indices.x = (float)vertexWeights[v].indices[0]; // 가중치의 x
				vertex.indices.y = (float)vertexWeights[v].indices[1]; // 가중치의 y
				vertex.indices.z = (float)vertexWeights[v].indices[2]; // 가중치의 z
				vertex.indices.w = (float)vertexWeights[v].indices[3]; // 가중치의 적용 강도

				vertex.weights.x = vertexWeights[v].weights[0];
				vertex.weights.y = vertexWeights[v].weights[1];
				vertex.weights.z = vertexWeights[v].weights[2];
				vertex.weights.w = vertexWeights[v].weights[3];
			}
			// 여기까지 오면, 원본 모델의 정점 위치, 각도, 법선, 텍스처 정보, 가중치 정보 등이 모두 들어간

			// ...정점 1개가 나온다

			// 그 정점 1개를 벡터에 추가
			mesh->vertices[v] = vertex;
		}

		// 정점 추출이 끝났으니 인덱스 추출과 대입 시작

		mesh->indices.resize(srcMesh->mNumFaces * 3); // 메쉬가 가진 "면"의 개수 x 3
		for (UINT f = 0; f < srcMesh->mNumFaces; ++f)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT k = 0; k < face.mNumIndices; ++k) // k를 다시 해당 면이 가진 인덱스 숫자까지 반복
			{
				// 인덱스의 (면x3)+현재인덱스 위치에 면의 인덱스 순번 값을 대입한다
				// 혹은 .... "인덱스의 순번은 면이 1번 바뀔 때마다 3씩 건너뛰고,
				// k가 바뀔 때마다 1씩 올라간다"
				mesh->indices[(f * 3) + k] = face.mIndices[k]; // 해당 목록에 적힌 정점의 인덱스가 들어간다
			}
		}

		// 여기까지 오면 인덱스 대입 끝
	
		// = 메쉬에 필요한 정보 추출 끝

		// 그 메쉬를 메쉬 벡터에 넣는다

		meshes.push_back(mesh);
	}

	// 여기까지 오면 매개변수로 받았던 부위 하나에 대한 모든 메쉬 정보가 들어온다

	// -> 이에 따라 모델이 가진 메쉬 정보가 완성....되지 않는다.
	//    왜냐면 지금 진행한 건 아직 부위 하나니까.

	// -> 지금 부위 밑에 자식 부위가 또 있다면 그 부위에 대한 추출을 해야 한다

	for (UINT c = 0; c < node->mNumChildren; ++c) // 지금 부위의 자식 노드 숫자만큼
	{
		ReadMesh(node->mChildren[c]); // 자식 노드 하나하나마다 재귀함수로 메쉬 추출 또 하기
	}

	// 여기까지 왔을 때, 비로소 받은 부위에 대한 메쉬 추출이 끝나게 된다
	// (그리고 처음 받은 노드가 만약 root라면 -> 전체 메쉬 추출이 가능)
}

void ModelExporter::ReadNode(aiNode* node, int index, int parent)
{
	// 노드 데이터 변수 만들어서 매개변수 전달하기
	NodeData* nodeData = new NodeData();
	nodeData->index = index;
	nodeData->parent = parent;
	nodeData->name = node->mName.C_Str();

	// 트랜스폼 정보를 추출하기
	Matrix matrix(node->mTransformation[0]); // 원본 노드의 트랜스폼
	nodeData->transform = XMMatrixTranspose(matrix); // 변수로 전달

	// 추출한 노드 데이터를 벡터에 넣고
	nodes.push_back(nodeData);

	// 자식 노드가 있다면 (자식 노드 개수가 0보다 크면) 반복문 + 재귀함수로 똑같이 노드 읽기 진행
	FOR(node->mNumChildren)
		ReadNode(node->mChildren[i], nodes.size(), index);
}

void ModelExporter::ReadBone(aiMesh* mesh, vector<VertexWeights>& vertexWeights)
{
	FOR(mesh->mNumBones)
	{
		UINT boneIndex = 0; // 뼈대 인덱스 초기화
		string name = mesh->mBones[i]->mName.C_Str(); // 이름 받기

		if (boneMap.count(name) == 0) // 추출한 이름에 대응하는 (현재 저장된) 뼈대가 본 맵에 없었으면
		{
			boneIndex = boneCount++; // 0에서 시작했을 본 카운트의 값을 대입해주고, 이어서 카운트의 값 +1
			boneMap[name] = boneIndex; // 받은 이름으로 된 새 페어를 맵에 추가한다 (name, boneIndex)

			BoneData* boneData = new BoneData();
			boneData->name = name;
			boneData->index = boneIndex;

			Matrix matrix(mesh->mBones[i]->mOffsetMatrix[0]); // 뼈대 트랜스폼
			boneData->offset = XMMatrixTranspose(matrix);

			// 추출한 뼈대를 벡터에 넣기
			bones.push_back(boneData);

			// 여기까지 진행하면 뼈대 데이터를 모두 모아서 벡터에도 넣고 맵에도 넣고....가 끝
		}
		else
		{
			// 여기로 들어왔다는 말은 추출한 이름에 대응하는 뼈대가 맵에 있었다는 이야기
			// -> 윗줄에서 코드를 통해서 뼈대 데이터도 인덱스도 트랜스폼도 다 받아줬다는 이야기
			// 이후 계산을 위해서 뼈대의 인덱스만 맵 안에서 다시 받으면 된다
			boneIndex = boneMap[name];
		}

		// 여기까지 오면 세 가지 데이터가 모인다
		// 1. 뼈대 데이터 그 자체
		// 2. 뼈대들에 대한 2차 데이터 (개수, 관계, 그 뼈대들의 각 인덱스...)
		// 3. 마지막 뼈대의 인덱스 (boneIndex)

		// TODO : 마지막 뼈대 boneIndex가 필요한 추가 연산이 혹 있다면 여기서 쓰면 된다

		// -> 이 함수에서 마지막 할 일 : 뼈대의 가중치를 찾아서 저장 (정점 가중치 준비)
		for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
		{
			UINT index = mesh->mBones[i]->mWeights[j].mVertexId; // 가중치의 기준이 되어야 할 정점의 ID
			vertexWeights[index].Add(boneIndex,			// 마지막 뼈대의 인덱스 = 뼈대 구분용 (전체 뼈의 ID)
				mesh->mBones[i]->mWeights[j].mWeight);	// 해당 전체의 뼈대의 i번째 관절의 j번째 순서 가중치를
														// 정점 index에 대응하도록 추가한다
		}
	}
}

void ModelExporter::WriteMesh()
{
	// 지금까지 모인 데이터를 .mesh 파일로 만들어서 다른 씬에서 쓸 수 있게 대비

	string path = "Models/Meshes/" + name + ".mesh";

	CreateFolders(path); // 경로 (및 파일) 없으면 만들기

	BinaryWriter* writer = new BinaryWriter(path);

	writer->UInt(meshes.size()); // 메쉬를 얼마나 읽어야 할지부터 저장
	for (MeshData* mesh : meshes)
	{
		// 모델 cpp에서 읽었던 데이터 순서대로 이번에는 저장

		writer->String(mesh->name); // 메쉬의 이름
		writer->UInt(mesh->materialIndex); // 메쉬에 저장될 매티리얼...의 인덱스

		writer->UInt(mesh->vertices.size()); // 정점 총 개수
		writer->Byte(mesh->vertices.data(), sizeof(ModelVertex) * mesh->vertices.size()); // 실제 정점 벡터

		writer->UInt(mesh->indices.size()); // 인덱스 목록 길이
		writer->Byte(mesh->indices.data(), sizeof(UINT) * mesh->indices.size()); // 실제 인덱스 목록

		// 다 쓴 메쉬 데이터 삭제
		delete mesh; // 어차피 익스포터 내에선 더 쓸 일 없음. 실제 모델은 .mesh에서 다시 읽는다
	}
	meshes.clear(); // 벡터 초기화

	// 노드 저장
	writer->UInt(nodes.size());
	for (NodeData* node : nodes)
	{
		writer->Int(node->index);			// 노드의 인덱스
		writer->String(node->name);			// 노드의 이름
		writer->Int(node->parent);			// 노드의 부모
		writer->Matrix(node->transform);	// 노드의 트랜스폼 정보

		delete node;
	}
	nodes.clear();

	writer->UInt(bones.size());
	for (BoneData* bone : bones)
	{
		writer->Int(bone->index);		// 뼈대 인덱스
		writer->String(bone->name);		// 뼈대 이름
		writer->Matrix(bone->offset);	// 뼈대 조정치 (트랜스폼)
	}
	bones.clear();

	// 메쉬 저장 끝, 노드 저장 끝, 뼈대 저장 끝....데이터 저장 끝

	delete writer;
}

Clip* ModelExporter::ReadClip(aiAnimation* animation)
{
	Clip* clip = new Clip(); // 변수 생성

	// 원본에서 데이터 받아와서 대입하기
	clip->name = animation->mName.C_Str();
	clip->tickPerSecond = (float)animation->mTicksPerSecond;
	clip->frameCount = (UINT)(animation->mDuration) + 1; // 0이 첫 프레임이므로

	vector<ClipNode> clipNodes;
	clipNodes.reserve(animation->mNumChannels);
	FOR(animation->mNumChannels)
	{
		aiNodeAnim* srcNode = animation->mChannels[i]; // 원본의 i번째 노드 받기

		ClipNode node;
		node.name = srcNode->mNodeName; // 원본으로 받은 부분 부위의 이름
		
		// 노드 내 지정된 트랜스폼 지정 목록 중에서 가장 많은 수(위치, 회전, 크기)를 키의 총 수로 간주
		UINT keyCount = max(srcNode->mNumPositionKeys, srcNode->mNumRotationKeys);
		keyCount = max(keyCount, srcNode->mNumScalingKeys);

		node.transforms.reserve(clip->frameCount); // 클립의 프레임 수만큼 노드 예약

		KeyTransform transform;
		for (UINT k = 0; k < keyCount; ++k) // 위에서 도출한 가장 많은 키 숫자만큼 반복
		{
			bool isFound = false; // 조건에 맞는 정보가 있었는지 여부 초기화
			float t = node.transforms.size();

			if (k < srcNode->mNumPositionKeys // k가 아직 위치의 목록을 안 벗어났고
				&& NearlyEqual((float)srcNode->mPositionKeys[k].mTime, t))
				// 지정된 위치 정보가 역시 노드의 목록 범위 안이라면
			{
				aiVectorKey key = srcNode->mPositionKeys[k]; // ai식 벡터로 위치 정보 받기
				// ai식 벡터로 존재하던 원본 노드의 데이터를 작업 중인 트랜스폼에 따오기
				memcpy_s(&transform.pos, sizeof(Float3), &key.mValue, sizeof(aiVector3D));

				// 어쨌든 조건에 맞는 정보가 있었다고 체크
				isFound = true;
			}

			// 위와 비슷한 계산을 회전, 크기에도 적용

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

			if (isFound) // 만약 조건에 맞는 정보가 있었다면...
			{
				// 어쨌든 부분적으로라도 (혹은 완전히) 트랜스폼 정보를 가진 키 프레임의 모델이
				// 발견이 된 것
				// 그러므로 벡터에 넣는다
				node.transforms.push_back(transform);
			}
		}

		// 여기까지 오면서 반복문을 마치면 조금이라도 시점에 따른 트랜스폼 변화 목록이 있는 노드는
		// 모조리 긁어서 벡터로 만들게 된다

		// 이 벡터를 클립에서 기록된 전체 프레임 수와 다시 비교를 한다
		if (node.transforms.size() < clip->frameCount) // 그렇게 긁어와도 모자라면
		{
			UINT count = clip->frameCount - node.transforms.size(); // 차이를 낸다
			KeyTransform keyTransform = node.transforms.back(); // 위에서 찾은 (키)트랜스폼 목록의 마지막 요소를 복제

			for (int d = 0; d < count; d++) // 차이만큼 
				node.transforms.push_back(keyTransform); // 노드 벡터에 복제된 키 트랜스폼을 추가
		}

		// 키 트랜스폼이 모인 노드를 동작이 있는 노드의 벡터에 추가
		clipNodes.push_back(node);
	}
	// 여기서 시점별로 움직임이 달라져야 할 트랜스폼의 목록 = 키 트랜스폼과 그 노드의 목록이 나온다
	
	// 지금 나온 노드를 원본 삼아서 키 프레임(시점별 자세)을 읽는다

	ReadKeyFrame(clip, scene->mRootNode, clipNodes);
	// clipNode를 참고하여, mRootNode로부터, clip의 키 프레임을 실제로 불러온다

	// 여기까지 오면 (오류가 안 났으면) clip이 만들어진다

	return clip;
}

void ModelExporter::ReadKeyFrame(Clip* clip, aiNode* node, vector<ClipNode>& clipNodes)
{
	KeyFrame* keyFrame = new KeyFrame();
	keyFrame->boneName = node->mName.C_Str();
	keyFrame->transforms.reserve(clip->frameCount); // 프레임 수만큼 자리 예약

	FOR(clip->frameCount)
	{
		ClipNode* clipNode = nullptr;		// 작업할 클립 내 노드 준비
		for (ClipNode& tmp : clipNodes)		// 매개변수로 받은 노드 벡터 검사
		{
			if (tmp.name == node->mName)	// 매개변수로 받은(읽을) 노드와 이름이 같은 곳이 있으면
			{ 
				clipNode = &tmp;			// 그곳이 작업할 곳(포인터)이다
				break;						// 검사 종료
			}
		}

		KeyTransform keyTransform;
		if (clipNode == nullptr) // 받은 노드 정보가 없었으면
		{
			Matrix transform(node->mTransformation[0]); // 원본에서 다시 만든다(루트 기준)
														// 이 경우 원본 행렬이 원점 기준으로 딱 맞춰지지 않을 경우를 대비해서
			transform = XMMatrixTranspose(transform);	// DX 규격 기준으로 행렬 요소를 분해 -> 재구성

			// 벡터 요소 준비
			Vector3 S;
			Vector3 R;
			Vector3 T;
			// DX 규격에 맞춰져서 정리된 행렬을 다시 분해해서 벡터를 추출한다
			XMMatrixDecompose(S.GetValue(), R.GetValue(), T.GetValue(), transform);
					// 매개변수 : 결과 크기 값, 결과 회전 값, 결과 위치(전이) 값, 원본

			// 벡터를 구조체에 적용
			keyTransform.scale = S;
			XMStoreFloat4(&keyTransform.rot, R);
			keyTransform.pos = T;

			// 이렇게 원본 기준으로 트랜스폼을 만들어서라도 프레임을 담아 갈 준비를 한다
		}
		else // 트랜스폼이 있는 경우
		{
			keyTransform = clipNode->transforms[i]; // 원본을 통해 추출한 노드 정보의, i번째 트랜스폼
		}

		keyFrame->transforms.push_back(keyTransform); // 벡터에 추가
	}
	// 여기까지 만든, 트랜스폼(노드 정보)을 모아 만든 키 프레임(모델의 자세)을
	// 움직임을 위한 벡터에 넣는다 (부위를 모아 모델, 부위의 정보를 모아 자세, 시간별 자세를 모으면 움직임)
	clip->keyFrame.push_back(keyFrame);

	FOR(node->mNumChildren) // 이 노드에 자식 노드가 있었으면...
		ReadKeyFrame(clip, node->mChildren[i], clipNodes);
		// 자식 노드를 상대로 똑같은 과정을 반복 (자식이 남지 않을 때까지)
}

void ModelExporter::WriteClip(Clip* clip, string clipName, UINT index)
{
	// 클립을 먼저 다 읽은 다음, 파일로 저장하기

	string file = "Models/Clips/" + name + "/" + clipName + to_string(index) + ".clip";

	CreateFolders(file);

	BinaryWriter* writer = new BinaryWriter(file);

	writer->String(clip->name); // 어떤 동작인가
	writer->UInt(clip->frameCount); // 얼마나 데이터가 있는가
	writer->Float(clip->tickPerSecond); // 기본 재생 속도(초당 프레임 갱신량)는 얼마인가

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
