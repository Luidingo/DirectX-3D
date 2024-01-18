#pragma once

class Model : public Transform
{
    // 3D 데이터를 메쉬로 만든 결과....를 받아서 개체로 움직이게 해주는
    // 단일대상 관리 클래스

public:
    Model(string name);
    ~Model();

    void Render();
    void GUIRender(); // ImGui 사용시 함수명을 GUI렌더로 (대부분) 통일

    void SetShader(wstring file);

private:
    void ReadMaterial();
    void ReadMesh();

protected:
    string name; //이 모델의 이름

    //모델이 존재하기 위한 하위 데이터들
    vector<Material*> materials;
    vector<ModelMesh*> meshes;
    vector<NodeData> nodes;
    vector<BoneData> bones;

    map<string, UINT> boneMap; //뼈대 구조를 맵으로 만든 것

    WorldBuffer* worldBuffer; // 트랜스폼의 공간 정보를 버퍼로 따로 빼서 계산
                              // 정점도 내부 모양도 계속 재연산이 필요한데 
                              // 공간까지 같은 연산을 같은 곳에서 반복하면 너무 느려서
};

