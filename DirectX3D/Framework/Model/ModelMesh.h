#pragma once

class ModelMesh
{
    //FBX���� �б� ����� ���ưų�, .mesh ���Ͽ��� ���� ���� �޽� ���¸� �����ϴ� Ŭ����

public:
    ModelMesh(string name); // ������ ������ ��Ī �ʿ�
    ~ModelMesh();

    void Render(); // �Ϲ� ����
    void RenderInstanced(UINT instanceCount); // Ư���� ������ ��ġ�� ����

    //�� �޽��� �޽� ������ ���� �Ѵ�
    // void* : �ڷ����� �������� �ʰ�, �޸� ��ġ�� �����ϴ� Ư���� ������
    //         1. �ڷ����� �����ϱ� ���� �� (ȣȯ�� ���̱�) // 2. �ڷ����� �ް� ������, ���� ������ �� �� ����
    //         ��� ����� �Ϲ� �����Ϳ� ����ϰ�, ȣ�� �ÿ��� �ݵ�� �ڷ��� ��ȯ�� ������� �Ѵ�
    void CreateMesh(void* vertexData, UINT vertexCount, void* indexData, UINT indexCount);

    void SetMaterial(Material* material) { this->material = material; }

private:

    string name; //�� �޽��� �̸�

    Material* material; // ��Ƽ����
    Mesh<ModelVertex>* mesh; // ���⼭ ���� �޽�
};

