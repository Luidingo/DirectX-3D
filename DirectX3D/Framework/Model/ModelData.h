#pragma once

// 모델 3D 구현 과정

// (기준 포맷은 FBX,            : 많이 쓰기도 하고, 이 안에 들어갈 수 있는 데이터 종류가 많아서
//                               (정점, 관절, 시간별 자세, 뼈대, 이런 데이터를 조합해서 예측 가능한 "동작"까지)
//  기준 데이터 확장자는 .mesh,  : 많이 써서
//  내부 데이터는 직접 구현, : 강의중 유지보수 + 확장이 편하려고
//  추출은 제공 함수로)     : 사용이 편하려고

// -> FBX를 읽고, .mesh 파일을 쓰고, 직접 나열한 스타일로 데이터를 다루는 과정으로 진행

// * 이 프레임워크에는 AssImp를 쓰고 있다. 이 라이브러리는 프리웨어이며, 어디서든 받을 수 있다
//   -> 그러나 이 라이브러리를 개인이 각자 설치할 경우 : 컴퓨터에 바로 깔린다
//   -> 개발자가 개발용 컴퓨터를 쓰고 있을 때는 편하지만... 작업한 파일을 외부에 가져가면
//      외부에서 이 라이브러리를 쓸 수 있다는 보장은 없다 -> 그래서 프레임워크에 라이브러리 포함


//------------------------------

// 모델이 만들어지기 위해서는 여러 데이터가 동시에 필요하다
// 형태에 대한 데이터, 입힐 그림 데이터, 동작 데이터, 위치, 동작에 대한 이벤트 등.....

// 이런 자료를 빠르게 취급하기 위해서, 모델에 쓰이는 데이터 집합은 전역 형태로 만든다

//------------------------------

// 모델용 정점 타입 (각 도형 클래스와 같지만 전역 작성이라는 점이 차이)

typedef VertexUVNormalTangentBlend ModelVertex;
        // 정점 타입 : 기본정점 + 텍스처 + 법선 + 화소보정 + 추가정점보정

struct MeshData // 모델의 기본 형태
{
    string name; // 메쉬의 이름
    UINT materialIndex; // 메쉬 표현에 쓰이기 위한 매티리얼...의 인덱스 (=매티리얼이 하나가 아닐 수도 있다)

    vector<ModelVertex> vertices; // 메쉬의 정점
    vector<UINT> indices; //정점의 인덱스
};

struct NodeData // 모델 내의 세부 부위
{
    int index; //노드의 인덱스 (가장 위에서부터 몇 번째인가)
    string name; // 노드 이름
    int parent; //부모 노드의 인덱스 (부모가 있다면 몇 번 노드에 붙었는가?)
    Matrix transform; // 이 노드가 가진 공간의 행렬 = 현재의 공간적 상태
};

struct BoneData // 모델의 관절 관계, 다른 말로 뼈대
                // 모델이 뼈대만 있고 다른 데이터가 없다면 그 모델을 "스켈레톤"이라고 한다
{
    int index; // 이 뼈(부위)의 인덱스
    string name; // 이 뼈의 이름
    Matrix offset; //이 뼈가 다른 뼈나 기준 공간에 대해 갖는 관계(보정치)
};

struct VertexWeights
{
    // 정점가중치 : 어느 쪽 정점이, 얼마나 많이(혹은 고의로 적게) 움직여야 하는지 나타내는 값들
    //             이 가중치에 따라 같은 이동 코드가 입력돼도 상대적으로 많이/적게 움직이는 정점이 나온다

    UINT indices[4] = {}; // 계산 중에 확인할 뼈의 인덱스들
    float weights[4] = {}; // 보간 계산에 따라 확인하고 할당할 가중치

    void Add(const UINT& index, const float& weight) //구조체도 함수 작성은 가능
    {
        //for (int i = 0; i < 4; ++i)
        FOR(4)
        {
            if (weights[i] == 0) // 가중치에 대한 사전값이 없었다면
            {
                //가까운 인덱스 값에 가중치를 주고 종료
                indices[i] = index;
                weights[i] = weight;
                return;
            }
        }
    }

    void Normalize()
    {
        float sum = 0; // 합계치 초기화

        FOR(4)
            sum += weights[i]; //주어진 인덱스 기준 +0~+3까지의 가중치를 모두 더한 다음

        FOR(4)
            weights[i] /= sum; //합계로 가중치를 다시 나눈다 (평준화)
    }
};

struct KeyTransform // 중심이 되는 기본 트랜스폼 데이터
{
    Float3 scale; // 크기. Float3(DX)인 이유 : FBX와의 호환성 의도가 좀 더 크다 (밑의 회전하고 좀 더 밀접)
    Float4 rot;   // 회전. 사원수. FBX의 데이터를 그대로 가져와서, 필요하면 DX를 통해서 쓰거나 벡터로 만든다
    Float3 pos;   // 위치.
};

struct KeyFrame // 시점별 해당 모델의 중심 형상 (어떤 시점에서 어떤 모습으로 있는가)
                // 해당 모델에 어떤 시점(순간)이 얼마나 있는가? 그리고 그 때 어떤 모습인가?
{
    string boneName; //쓰인 뼈대 이름
    vector<KeyTransform> transforms; //키 트랜스폼 집합
};

struct ClipNode //각 키 프레임에 의해 노드들의 변화를 모으면 어떤 "움직임"이 되는가?
{
    //라이브러리에 의한 새 자료형 aiString 사용
    aiString name; // 애셋 임포터 문자열 : 제공 함수와의 호환성 높이기가 의도
    vector<KeyTransform> transforms;
};

struct Clip // 각 노드의 움직임을 모아 만든, 모델의 움직임
{
    string name; // 클립(움직임)의 이름

    UINT frameCount; // 이 클립을 구성 중인 프레임의 개수
    float tickPerSecond; // 초당 갱신 횟수 (≒기본 재생 속도)

    vector<KeyFrame*> keyFrame; //움직임을 구성하기 위한 키 프레임들
};

struct ClipTransform
{
    Matrix transform[MAX_FRAME][MAX_BONE]; // 클립을 구성하는 트랜스폼들
                                           // 컴퓨터 혹은 매크로가 허용 가능한 최대 수만큼 미리 준비
                                           // 이 프레임워크에서도 framework.h에서 매크로 확인 가능
                                           // 그 외 많은 개발 환경에서 따로 설정해둔 경우가 많아
                                           // 먼저 자기가 쓰는 개발환경에서 한계 설정을 어떻게 했는지 보면 좋다
};