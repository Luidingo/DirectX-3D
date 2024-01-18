#pragma once

class ModelMesh
{
    //FBX에서 읽기 기능을 거쳤거나, .mesh 파일에서 직접 읽은 메쉬 형태를 설정하는 클래스

public:
    ModelMesh(string name); // 무엇의 모델인지 지칭 필요
    ~ModelMesh();

    void Render(); // 일반 렌더
    void RenderInstanced(UINT instanceCount); // 특수한 관리를 거치는 렌더

    //모델 메쉬는 메쉬 제작을 따로 한다
    // void* : 자료형을 지정하지 않고, 메모리 위치만 지정하는 특수한 포인터
    //         1. 자료형을 지정하기 싫을 때 (호환성 높이기) // 2. 자료형을 받고 싶지만, 뭐가 나올지 모를 때 쓴다
    //         사용 방법은 일반 포인터와 비슷하고, 호출 시에는 반드시 자료형 변환을 시켜줘야 한다
    void CreateMesh(void* vertexData, UINT vertexCount, void* indexData, UINT indexCount);

    void SetMaterial(Material* material) { this->material = material; }

private:

    string name; //이 메쉬의 이름

    Material* material; // 매티리얼
    Mesh<ModelVertex>* mesh; // 여기서 만든 메쉬
};

