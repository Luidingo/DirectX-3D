#include "Framework.h"
#include "Model.h"

Model::Model(string name) : name(name)
{
    ReadMaterial(); // 시작하자마자 그림 부르기
    ReadMesh();     // 형태 만들기

    worldBuffer = new WorldBuffer();
}

Model::~Model()
{
    for (Material* material : materials)
        delete material;

    for (ModelMesh* mesh : meshes)
        delete mesh;

    delete worldBuffer;
}

void Model::Render()
{
    worldBuffer->Set(world); // 트랜스폼의 공간 정보를 버퍼로 이전하고
    worldBuffer->SetVS(0);   // 버퍼가 된 공간의 정점 정보를 연산

    for (ModelMesh* mesh : meshes)
        mesh->Render();
}

void Model::GUIRender()
{
    Transform::GUIRender(); // 트랜스폼으로서의 정보를 ImGui에서 확인
    for (Material* material : materials)
        material->GUIRender();
}

void Model::SetShader(wstring file)
{
    for (Material* material : materials)
        material->SetShader(file); // 벡터 내 모든 매티리얼에 돌아가면서 매개변수 토스
}

void Model::ReadMaterial()
{
    string file = "Models/Materials/" + name + "/" + name + ".mats";
                // .mat, .mats : 여러 게임 등에서 사용하는 3D 매티리얼 포맷의 확장자지만.... (이하 ReadMesh 참조)

    BinaryReader* reader = new BinaryReader(file);

    if (reader->IsFailed()) assert(false);

    UINT size = reader->UInt();

    materials.reserve(size);

    FOR(size)
    {
        Material* material = new Material(); // 시각 정보 생성
        material->Load(reader->String());    // 시각 정보를 .mats 파일에서 읽기

        materials.push_back(material);       // 불러온 정보 벡터에 담기
    }

    delete reader;
}

void Model::ReadMesh()
{
    string file = "Models/Meshes/" + name + ".mesh";
                // 모델 클래스에서 읽는 확장자는 원형인 FBX가 아니라, 해당 데이터를 재정렬한 .mesh
                // .mesh : 여러 게임 등에서 사용하는 3D 데이터 포맷의 확장자지만....
                //         여기서는 보편적인 포맷 대신에 우리가 만든 방식으로 읽기를 시도한다

    //파일 읽기 시작
    BinaryReader* reader = new BinaryReader(file); // 2진 파일 읽기 클래스로 데이터 읽기 준비

    if (reader->IsFailed()) assert(false); // 읽기가 조금이라도 실패하면 전체 앱 종료

    UINT size = reader->UInt(); // (우리가 만든) .mesh의 첫 데이터 : 모델이 가진 부위의 숫자

    meshes.reserve(size); // 부위 수만큼 형태를 예약

    FOR(size) //부위 수만큼 반복문 돌리기
    {
        ModelMesh* mesh = new ModelMesh(reader->String()); // <- 모델의 이름
        mesh->SetMaterial(materials[reader->UInt()]);

        UINT vertexCount = reader->UInt();
        ModelVertex* vertices = new ModelVertex[vertexCount]; // 포인터를 만드는데 처음부터 배열로 만들기 (전체 크기가 예약)
        reader->Byte((void**)&vertices, sizeof(ModelVertex) * vertexCount);

        UINT indexCount = reader->UInt();
        UINT* indices = new UINT[indexCount]; // 배열로 포인터 만들기
        reader->Byte((void**)&indices, sizeof(UINT) * indexCount);

        // 아래 void*에 수동으로 만든 배열(= 읽을 크기가 정해진 포인터)를 집어 넣으면
        // 함수 자체는 자기가 받은 자료형을 정확히는 모르고 있겠지만, 어쨌든 크기가 정확히 주어졌으니까
        // 주어진 만큼 데이터를 처리할 것이고, 결과적으로는 해당 자료형에 맞추어서 데이터를 넣게 될 것
        // -> void*를 사용하는 또 다른 방법(이라고 쓰고 꼼수라고 읽는다)
        mesh->CreateMesh(vertices, vertexCount, indices, indexCount);

        //다 만든 메쉬를 벡터에 추가
        meshes.push_back(mesh);
    }

    size = reader->UInt();
    nodes.resize(size);
    for (NodeData& node : nodes)
    {
        //계속 파일을 읽어나가면서 읽는 족족 변수에 할당 진행
        node.index = reader->Int();
        node.name = reader->String();
        node.parent = reader->Int();
        node.transform = reader->Matrix();
    }

    size = reader->UInt();
    bones.resize(size);
    for (BoneData& bone : bones)
    {
        //들어갈 자료형과 읽는 자료형을 계속 맞춰준다
        bone.index = reader->Int();
        bone.name = reader->String();
        bone.offset = reader->Matrix();

        // 뼈대는 만들면 뼈대 맵에도 추가
        boneMap[bone.name] = bone.index; // 이름으로 인덱스 추가
    }

    //다 쓴 파일 리더기 포인터는 파기
    delete reader;
}
