#include "Framework.h"

ModelMesh::ModelMesh(string name) : name(name)
{
    //this->name = name;
}

ModelMesh::~ModelMesh()
{
    delete mesh;
    // delete material; // �׸��� �ٸ� ������ ��������ų�, �ؽ�ó Ŭ�������� ���� ���� ���ɼ��� ����
}

void ModelMesh::Render()
{
    material->Set(); // ���� ����
    mesh->Draw(); //�׸���
}

void ModelMesh::RenderInstanced(UINT instanceCount)
{
    // TODO : RenderInstanced�� �ǹ̸� ���캼 �� ���ƿͼ� �ۼ��ϰų�, �ڵ带 ä���� ����
}

void ModelMesh::CreateMesh(void* vertexData, UINT vertexCount, void* indexData, UINT indexCount)
{
    mesh = new Mesh<ModelVertex>();

    mesh->GetVertices().resize(vertexCount); // �Ű������� ���� ��ŭ ���� �ø���
    mesh->GetIndices().resize(indexCount); // �Ű������� ���� ��ŭ �ε��� ��� �ø���

    //�Ű������� ���� (���� ���� �𸣴�) "������"�� �����ؼ� �ش� *�޸� ����*�� �״�� ���Ϳ� ����

    memcpy(mesh->GetVertices().data(), vertexData, sizeof(ModelVertex) * vertexCount);
    //��� ����� = mesh�� ����. ���� = vertexData. �޸� ũ�� = ������(����ũ��) x ������ ��

    memcpy(mesh->GetIndices().data(), indexData, sizeof(UINT) * indexCount);

    // ������� ����, ��Ȯ�� � ���������� ���� ������ (*���� ���Ͽ� �޷ȴ�)
    // ��·�� ������ �ε��� ������ �����

    // �̿� ���� �޽� �����
    mesh->CreateMesh();
}
