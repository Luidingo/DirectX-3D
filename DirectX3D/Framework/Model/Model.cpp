#include "Framework.h"
#include "Model.h"

Model::Model(string name) : name(name)
{
    ReadMaterial(); // �������ڸ��� �׸� �θ���
    ReadMesh();     // ���� �����

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
    worldBuffer->Set(world); // Ʈ�������� ���� ������ ���۷� �����ϰ�
    worldBuffer->SetVS(0);   // ���۰� �� ������ ���� ������ ����

    for (ModelMesh* mesh : meshes)
        mesh->Render();
}

void Model::GUIRender()
{
    Transform::GUIRender(); // Ʈ���������μ��� ������ ImGui���� Ȯ��
    for (Material* material : materials)
        material->GUIRender();
}

void Model::SetShader(wstring file)
{
    for (Material* material : materials)
        material->SetShader(file); // ���� �� ��� ��Ƽ���� ���ư��鼭 �Ű����� �佺
}

void Model::ReadMaterial()
{
    string file = "Models/Materials/" + name + "/" + name + ".mats";
                // .mat, .mats : ���� ���� ��� ����ϴ� 3D ��Ƽ���� ������ Ȯ��������.... (���� ReadMesh ����)

    BinaryReader* reader = new BinaryReader(file);

    if (reader->IsFailed()) assert(false);

    UINT size = reader->UInt();

    materials.reserve(size);

    FOR(size)
    {
        Material* material = new Material(); // �ð� ���� ����
        material->Load(reader->String());    // �ð� ������ .mats ���Ͽ��� �б�

        materials.push_back(material);       // �ҷ��� ���� ���Ϳ� ���
    }

    delete reader;
}

void Model::ReadMesh()
{
    string file = "Models/Meshes/" + name + ".mesh";
                // �� Ŭ�������� �д� Ȯ���ڴ� ������ FBX�� �ƴ϶�, �ش� �����͸� �������� .mesh
                // .mesh : ���� ���� ��� ����ϴ� 3D ������ ������ Ȯ��������....
                //         ���⼭�� �������� ���� ��ſ� �츮�� ���� ������� �б⸦ �õ��Ѵ�

    //���� �б� ����
    BinaryReader* reader = new BinaryReader(file); // 2�� ���� �б� Ŭ������ ������ �б� �غ�

    if (reader->IsFailed()) assert(false); // �бⰡ �����̶� �����ϸ� ��ü �� ����

    UINT size = reader->UInt(); // (�츮�� ����) .mesh�� ù ������ : ���� ���� ������ ����

    meshes.reserve(size); // ���� ����ŭ ���¸� ����

    FOR(size) //���� ����ŭ �ݺ��� ������
    {
        ModelMesh* mesh = new ModelMesh(reader->String()); // <- ���� �̸�
        mesh->SetMaterial(materials[reader->UInt()]);

        UINT vertexCount = reader->UInt();
        ModelVertex* vertices = new ModelVertex[vertexCount]; // �����͸� ����µ� ó������ �迭�� ����� (��ü ũ�Ⱑ ����)
        reader->Byte((void**)&vertices, sizeof(ModelVertex) * vertexCount);

        UINT indexCount = reader->UInt();
        UINT* indices = new UINT[indexCount]; // �迭�� ������ �����
        reader->Byte((void**)&indices, sizeof(UINT) * indexCount);

        // �Ʒ� void*�� �������� ���� �迭(= ���� ũ�Ⱑ ������ ������)�� ���� ������
        // �Լ� ��ü�� �ڱⰡ ���� �ڷ����� ��Ȯ���� �𸣰� �ְ�����, ��·�� ũ�Ⱑ ��Ȯ�� �־������ϱ�
        // �־��� ��ŭ �����͸� ó���� ���̰�, ��������δ� �ش� �ڷ����� ���߾ �����͸� �ְ� �� ��
        // -> void*�� ����ϴ� �� �ٸ� ���(�̶�� ���� �ļ���� �д´�)
        mesh->CreateMesh(vertices, vertexCount, indices, indexCount);

        //�� ���� �޽��� ���Ϳ� �߰�
        meshes.push_back(mesh);
    }

    size = reader->UInt();
    nodes.resize(size);
    for (NodeData& node : nodes)
    {
        //��� ������ �о���鼭 �д� ���� ������ �Ҵ� ����
        node.index = reader->Int();
        node.name = reader->String();
        node.parent = reader->Int();
        node.transform = reader->Matrix();
    }

    size = reader->UInt();
    bones.resize(size);
    for (BoneData& bone : bones)
    {
        //�� �ڷ����� �д� �ڷ����� ��� �����ش�
        bone.index = reader->Int();
        bone.name = reader->String();
        bone.offset = reader->Matrix();

        // ����� ����� ���� �ʿ��� �߰�
        boneMap[bone.name] = bone.index; // �̸����� �ε��� �߰�
    }

    //�� �� ���� ������ �����ʹ� �ı�
    delete reader;
}
