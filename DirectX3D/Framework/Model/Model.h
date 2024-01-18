#pragma once

class Model : public Transform
{
    // 3D �����͸� �޽��� ���� ���....�� �޾Ƽ� ��ü�� �����̰� ���ִ�
    // ���ϴ�� ���� Ŭ����

public:
    Model(string name);
    ~Model();

    void Render();
    void GUIRender(); // ImGui ���� �Լ����� GUI������ (��κ�) ����

    void SetShader(wstring file);

private:
    void ReadMaterial();
    void ReadMesh();

protected:
    string name; //�� ���� �̸�

    //���� �����ϱ� ���� ���� �����͵�
    vector<Material*> materials;
    vector<ModelMesh*> meshes;
    vector<NodeData> nodes;
    vector<BoneData> bones;

    map<string, UINT> boneMap; //���� ������ ������ ���� ��

    WorldBuffer* worldBuffer; // Ʈ�������� ���� ������ ���۷� ���� ���� ���
                              // ������ ���� ��絵 ��� �翬���� �ʿ��ѵ� 
                              // �������� ���� ������ ���� ������ �ݺ��ϸ� �ʹ� ������
};

