#include "Framework.h"

ActorUI::ActorUI()
{
    crosshair = new Quad(L"Textures/UI/cursor.png");
    // 아래 네 코드의 결과는 모두 같다
    crosshair->Pos() = { CENTER_X, CENTER_Y, 0 };                   // 미리 정해진 절반의 숫자를 입력
    //crosshair->Pos() = { WIN_WIDTH * 0.5f, WIN_HEIGHT * 0.5f, 0 };  // 숫자에 0.5라는 소수를 곱한 것
    //crosshair->Pos() = { WIN_WIDTH / 2 , WIN_HEIGHT / 2, 0 };       // 숫자를 2라는 정수로 나눈 것
    //crosshair->Pos() = { WIN_WIDTH >> 1, WIN_HEIGHT >> 1, 0 };      // 비트 데이터를 오른쪽으로 한 번 밀어버리기
                                                                      // (칸에서 벗어난 데이터 없애기)
    crosshair->UpdateWorld();

    // * 곁다리 짜투리 팁 : 나누기에는 위와 같이 사전에 계산해서 데이터 주기,
    //                     1/2 곱하기, 2로 나누기, 비트 밀기의 방식이 있다
    //                     속도 : 사전변수 >>>>> 비트 연산 > 곱하기 > 나누기
    //                     그러나 사전 변수는 중간에 기준 데이터가 바뀌면 대응을 못 한다

    //                     비트 연산 : 기준 데이터를 2차 계산하는 연산 중에서는 가장 빠르다
    //                                정확한 값으로는 나눌 수 없다는 단점 있음 (/2, /4, /8,
    //                                혹은 근사값 /5 /10 등을 구할 때 가장 좋다)
    //                     곱하기 : 그 다음으로 빠르고, 계산이 대체로 정밀하다
    //                              100% 정밀하지는 않고, 기본적으로 float 연산이라 int 등에 쓰면
    //                              의도치 않은 결과를 내기도
    //                     나누기 : 가장 직관적, 가장 보고 이해하기 쉽다
    //                              나누기 연산 중에서는 제일 느리다(차이는 미미하지만....)
    //                              매개변수로 0이 들어오면 망한다

    // 조준선에 이어 퀵슬롯 만들기

    quickSlot = new Quad(L"Textures/UI/QuickSlot.png");
    quickSlot->SetTag("Quickslot");
    quickSlot->Load(); // 만약 사전 설정된 데이터가 있었다면 호출하는 함수 (없으면 그냥 그대로)
    quickSlot->UpdateWorld();

    // 아이콘의 프레임 만들고 대기시키기

    iconFrame = new Quad(L"Textures/UI/IconFrame.png");
    iconFrame->Scale() *= 0.7f; // 크기가 너무 커서 줄여놓고 불러오기
    iconFrame->SetActive(false); // 우선은 비활성화

    // 블록 아이콘을 만들고, 대기시키기 (처음에는 인벤토리에 아무 것도 없으니까)

    for (int i = 0; i < 5; ++i) // 블록 종류를 5종류까지 만들 것임
    {
        wstring file = L"Textures/Block/BlockIcon" + to_wstring(i) + L".png";
        Quad* blockIcon = new Quad(file);

        blockIcon->Scale() *= 0.1f;
        blockIcon->SetParent(quickSlot); // 단축바를 부모 트랜스폼으로
        blockIcon->SetActive(false);     // 일단 비활성화 (필수가 아닐지 모르지만 보험처럼)
        blockIcon->SetTag("Icon " + to_string(i));

        blockIcons.push_back(blockIcon); //벡터에 아이콘 추가
    }

    // 여기까지 오면 상호작용 이전에 존재할 데이터 = 그림, 아이콘, 벡터 생성 끝
}

ActorUI::~ActorUI()
{
    delete crosshair;
    delete quickSlot;
    delete iconFrame;

    for (Quad* blockIcon : blockIcons)
        delete blockIcon;
}

void ActorUI::Update()
{
    // 빌드, 마이닝 호출은 실제로 블록을 다뤄야 할.... 캐릭터 등이 호출하는 게 나을 것

    // 여기서는 인벤토리의 자체 조작을 작성

    if (KEY_DOWN('1'))
    {
        //현재 단축바의 0번 슬롯(가장 먼저 들어왔던 블록) 내용물 개수가 1 이상이면 0번 슬롯 선택
        if (iconData[0].second > 0) selectedIconIndex = 0;
    }

    if (KEY_DOWN('2'))
    {
        if (iconData[1].second > 0) selectedIconIndex = 1;
    }

    if (KEY_DOWN('3'))
    {
        if (iconData[2].second > 0) selectedIconIndex = 2;
    }

    if (KEY_DOWN('4'))
    {
        if (iconData[3].second > 0) selectedIconIndex = 3;
    }

    if (KEY_DOWN('5'))
    {
        if (iconData[4].second > 0) selectedIconIndex = 4;
    }
}

void ActorUI::Render()
{
    // 2D 위주라서 보류 : 3D UI가 생기면 아마 여기서 렌더가 되어야 할 것
}

void ActorUI::PostRender()
{
    // 2D (오르토그래픽 시점) 상태에서 출력할 코드

    //일반 쿼드 곧바로 출력
    crosshair->Render();
    quickSlot->Render();

    for (int i = 0; i < 5; ++i)
    {
        Vector3 pos; // 현재 대상 위치
        pos.x = -80 + (i * 20); // 제일 왼쪽은 -80, i가 올라갈 때마다 20씩 오른쪽

        if (iconData[i].second) // 처음 들어온 아이콘에 정보 데이터가 있다면
            // 현재의 데이터 의미 : 개수로 설정
        {
            //아이콘 출력
            Quad* icon = blockIcons[iconData[i].first]; // 블록의 종류 선택
            icon->SetActive(true); // 활성화
            icon->Pos() = pos;
            icon->UpdateWorld();
            icon->Render();

            string str = to_string(iconData[i].second); // 블록의 데이터(=현재 개수) 문자화

            Vector3 tmp = icon->GlobalPos(); // 아이콘의 절대위치를 임시로 받기

            Font::Get()->RenderText(str, { tmp.x, tmp.y }); // DX 폰트 함수와 비슷하게 문자열 출력 호출

            //지금 돌아가는 순번이 "선택한 아이콘"과 같다면
            if (i == selectedIconIndex)
            {
                //해당 위치에 아이콘 프레임 이미지도 출력
                iconFrame->SetActive(true);
                iconFrame->Pos() = icon->GlobalPos();
                iconFrame->UpdateWorld();
                iconFrame->Render();
            }
            else iconFrame->SetActive(false); //프레임 그냥 출력 안함
        }
    }
}

void ActorUI::GUIRender()
{
    quickSlot->GUIRender();
    for (Quad* blockIcon : blockIcons)
        blockIcon->GUIRender();
    iconFrame->GUIRender();
}

void ActorUI::Build()
{
    // 내 안의 블록을 가져와서 블록매니저한테 토스하고 쌓기 호출

    // 내가 어떤 블록을 선택했는지 보기
    if (selectedIconIndex < 0) return; // 선택한 블록이 없었으면 종료

    //선택한 인덱스의 블록 선택 (한줄 요약버전)
    //Cube* block = blocks[iconData[selectedIconIndex].first].back();

    //자세한 버전
    //1. 인벤토리에서 현재 선택 중인 입수순서의 블록 종류를 가져온다
    int blockKey = iconData[selectedIconIndex].first;

    //2. 블록의 맵->벡터에서 블록 키로 벡터를, 해당 벡터의 가장 마지막 순서를 가져온다
    Cube* block = blocks[blockKey].back();
    //            여기까지 벡터   // 이 벡터의 마지막

    // 이 블록을 블록매니저에게 토스하고 쌓기가 호출되는지(그래서 쌓였는지) 확인해서...
    if (BlockManager::Get()->BuildBlock(block))
    {
        // 이 안으로 들어오면 쌓기가 된 것이므로 블록의 데이터를 벡터에서 삭제
        blocks[blockKey].pop_back(); // 벡터에서 마지막 블록(=방금 준 그것) 삭제
        iconData[selectedIconIndex].second--; // 아이콘 데이터 안에 있는 블록의 개수도 -1

        // 그 결과 선택 중인 종류의 블록이 텅텅 비면...
        if (iconData[selectedIconIndex].second == 0)
        {
            selectedIconIndex = -1; // 선택 해제
        }
    }
}

void ActorUI::Mining()
{
    //이 함수가 호출되는 즉시 블록 매니저에서 레이캐스트로 한 개 블록 따오기
    Cube* block = BlockManager::Get()->GetCollisionBlock();

    if (block == nullptr) return; // 받아온 게 없으면 종료

    //종료가 되지 않았으면 ->
    //    block에는 어떤 데이터가 있고 + 블록매니저도 아마 블록 하나가 없어졌을 것

    // 지금 받아온 블록을 UI의 데이터로 관리를 옮겨주기

    wstring file = block->GetMaterial()->GetDiffuseMap()->GetFile(); // 그림 파일 이름 받기

    int tmpIndex = file.find_last_of('.'); // 매개변수로 준 문자가 마지막으로 도출된 순번 구하기
    file = file.substr(tmpIndex - 1, 1); // 위에서 구한 인덱스 바로 앞 글자를, 1글자만큼 다시 도출해서 데이터 피드백

    //여기까지 오면 file에 그림 파일명에 있던 0부터 4까지의 블록 숫자가 들어간다

    int index = stoi(file);

    if (blocks[index].size() == 0) // 큐브 데이터에서 아이콘 번호로 들어간 결과가 0이다
        // 현재 큐브의 맵에, 해당 종류로 가지고 있는 재고가 없다
    {
        //그러면 새로 하나 만든다

        for (int i = 0; i < 5; ++i)
        {
            if (iconData[i].second == 0) // 아이콘 데이터 맵의 빈 자리를 첫순부터 찾아서
            {
                //찾은 자리에 아이콘과 인벤토리 내용을 등록
                iconData[i].first = index; // 아이콘의 종류는 지금 받아온 대로
                iconData[i].second++;
                break; //데이터를 넣었으니 반복문 끝내기
            }
        }
    }
    else
    {
        // 아이콘 번호에 해당하는 큐브 데이터가 큐브 맵에 있을 때
        // 새로 만들기 x 있는 데이터 갱신하기

        for (auto& data : iconData) // <-자료구조를 분석해서 그에 맞는 자료형을 자동 검출
            //   느리고, 나중에 다시 봤을 때 헷갈릴 위험 있음
            //   * 비추천이지만, 써서 효과를 볼 때도 분명 있다
//for (pair<int, pair<int, int>> data : iconData)
        {
            if (data.second.first == index) // 현재 아이콘 데이터 중 종류-개수 페어의(첫 second)
                // 종류 데이터가 index와 일치하면 
            {
                data.second.second++; // 개수 데이터를 +1
                break;
            }
        }
    }

    // 여기까지 오면 block이 뭐든 있긴 있었다 (충돌체크)
    // 그리고 해당 block의 정보를 숫자로 도출해서 아이콘에 전달도 했다 (인벤토리)

    //남은 것은 실제 큐브 데이터를 맵에 보관하는 것뿐 (나중에 이 데이터를 쌓기에 다시 활용)

    blocks[index].push_back(block); // index라는 Key으로 된 blocks의 Value를 맵에 추가
}
