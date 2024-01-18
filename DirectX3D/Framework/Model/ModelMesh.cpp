#include "Framework.h"

ModelMesh::ModelMesh(string name) : name(name)
{
    //this->name = name;
}

ModelMesh::~ModelMesh()
{
    delete mesh;
    // delete material; // 그림이 다른 곳에서 만들어졌거나, 텍스처 클래스에서 관리 중일 가능성이 높다
}

void ModelMesh::Render()
{
    material->Set(); // 렌더 세팅
    mesh->Draw(); //그리기
}

void ModelMesh::RenderInstanced(UINT instanceCount)
{
    // TODO : RenderInstanced의 의미를 살펴볼 때 돌아와서 작성하거나, 코드를 채워서 리뷰
}

void ModelMesh::CreateMesh(void* vertexData, UINT vertexCount, void* indexData, UINT indexCount)
{
    mesh = new Mesh<ModelVertex>();

    mesh->GetVertices().resize(vertexCount); // 매개변수로 받은 만큼 벡터 늘리기
    mesh->GetIndices().resize(indexCount); // 매개변수로 받은 만큼 인덱스 목록 늘리기

    //매개변수로 받은 (아직 원지 모르는) "데이터"를 참고해서 해당 *메모리 내용*을 그대로 벡터에 복사

    memcpy(mesh->GetVertices().data(), vertexData, sizeof(ModelVertex) * vertexCount);
    //기대 결과물 = mesh의 정점. 원본 = vertexData. 메모리 크기 = 모델정점(단위크기) x 정점의 수

    memcpy(mesh->GetIndices().data(), indexData, sizeof(UINT) * indexCount);

    // 여기까지 오면, 정확히 어떤 형태일지는 아직 모르지만 (*원본 파일에 달렸다)
    // 어쨌든 정점과 인덱스 정보를 얻었다

    // 이에 따라 메쉬 만들기
    mesh->CreateMesh();
}
