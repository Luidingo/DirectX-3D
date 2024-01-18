#include "Framework.h"

ActorUI::ActorUI()
{
    crosshair = new Quad(L"Textures/UI/cursor.png");
    // �Ʒ� �� �ڵ��� ����� ��� ����
    crosshair->Pos() = { CENTER_X, CENTER_Y, 0 };                   // �̸� ������ ������ ���ڸ� �Է�
    //crosshair->Pos() = { WIN_WIDTH * 0.5f, WIN_HEIGHT * 0.5f, 0 };  // ���ڿ� 0.5��� �Ҽ��� ���� ��
    //crosshair->Pos() = { WIN_WIDTH / 2 , WIN_HEIGHT / 2, 0 };       // ���ڸ� 2��� ������ ���� ��
    //crosshair->Pos() = { WIN_WIDTH >> 1, WIN_HEIGHT >> 1, 0 };      // ��Ʈ �����͸� ���������� �� �� �о������
                                                                      // (ĭ���� ��� ������ ���ֱ�)
    crosshair->UpdateWorld();

    // * ��ٸ� ¥���� �� : �����⿡�� ���� ���� ������ ����ؼ� ������ �ֱ�,
    //                     1/2 ���ϱ�, 2�� ������, ��Ʈ �б��� ����� �ִ�
    //                     �ӵ� : �������� >>>>> ��Ʈ ���� > ���ϱ� > ������
    //                     �׷��� ���� ������ �߰��� ���� �����Ͱ� �ٲ�� ������ �� �Ѵ�

    //                     ��Ʈ ���� : ���� �����͸� 2�� ����ϴ� ���� �߿����� ���� ������
    //                                ��Ȯ�� �����δ� ���� �� ���ٴ� ���� ���� (/2, /4, /8,
    //                                Ȥ�� �ٻ簪 /5 /10 ���� ���� �� ���� ����)
    //                     ���ϱ� : �� �������� ������, ����� ��ü�� �����ϴ�
    //                              100% ���������� �ʰ�, �⺻������ float �����̶� int � ����
    //                              �ǵ�ġ ���� ����� ���⵵
    //                     ������ : ���� ������, ���� ���� �����ϱ� ����
    //                              ������ ���� �߿����� ���� ������(���̴� �̹�������....)
    //                              �Ű������� 0�� ������ ���Ѵ�

    // ���ؼ��� �̾� ������ �����

    quickSlot = new Quad(L"Textures/UI/QuickSlot.png");
    quickSlot->SetTag("Quickslot");
    quickSlot->Load(); // ���� ���� ������ �����Ͱ� �־��ٸ� ȣ���ϴ� �Լ� (������ �׳� �״��)
    quickSlot->UpdateWorld();

    // �������� ������ ����� ����Ű��

    iconFrame = new Quad(L"Textures/UI/IconFrame.png");
    iconFrame->Scale() *= 0.7f; // ũ�Ⱑ �ʹ� Ŀ�� �ٿ����� �ҷ�����
    iconFrame->SetActive(false); // �켱�� ��Ȱ��ȭ

    // ��� �������� �����, ����Ű�� (ó������ �κ��丮�� �ƹ� �͵� �����ϱ�)

    for (int i = 0; i < 5; ++i) // ��� ������ 5�������� ���� ����
    {
        wstring file = L"Textures/Block/BlockIcon" + to_wstring(i) + L".png";
        Quad* blockIcon = new Quad(file);

        blockIcon->Scale() *= 0.1f;
        blockIcon->SetParent(quickSlot); // ����ٸ� �θ� Ʈ����������
        blockIcon->SetActive(false);     // �ϴ� ��Ȱ��ȭ (�ʼ��� �ƴ��� ������ ����ó��)
        blockIcon->SetTag("Icon " + to_string(i));

        blockIcons.push_back(blockIcon); //���Ϳ� ������ �߰�
    }

    // ������� ���� ��ȣ�ۿ� ������ ������ ������ = �׸�, ������, ���� ���� ��
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
    // ����, ���̴� ȣ���� ������ ����� �ٷ�� ��.... ĳ���� ���� ȣ���ϴ� �� ���� ��

    // ���⼭�� �κ��丮�� ��ü ������ �ۼ�

    if (KEY_DOWN('1'))
    {
        //���� ������� 0�� ����(���� ���� ���Դ� ���) ���빰 ������ 1 �̻��̸� 0�� ���� ����
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
    // 2D ���ֶ� ���� : 3D UI�� ����� �Ƹ� ���⼭ ������ �Ǿ�� �� ��
}

void ActorUI::PostRender()
{
    // 2D (������׷��� ����) ���¿��� ����� �ڵ�

    //�Ϲ� ���� ��ٷ� ���
    crosshair->Render();
    quickSlot->Render();

    for (int i = 0; i < 5; ++i)
    {
        Vector3 pos; // ���� ��� ��ġ
        pos.x = -80 + (i * 20); // ���� ������ -80, i�� �ö� ������ 20�� ������

        if (iconData[i].second) // ó�� ���� �����ܿ� ���� �����Ͱ� �ִٸ�
            // ������ ������ �ǹ� : ������ ����
        {
            //������ ���
            Quad* icon = blockIcons[iconData[i].first]; // ����� ���� ����
            icon->SetActive(true); // Ȱ��ȭ
            icon->Pos() = pos;
            icon->UpdateWorld();
            icon->Render();

            string str = to_string(iconData[i].second); // ����� ������(=���� ����) ����ȭ

            Vector3 tmp = icon->GlobalPos(); // �������� ������ġ�� �ӽ÷� �ޱ�

            Font::Get()->RenderText(str, { tmp.x, tmp.y }); // DX ��Ʈ �Լ��� ����ϰ� ���ڿ� ��� ȣ��

            //���� ���ư��� ������ "������ ������"�� ���ٸ�
            if (i == selectedIconIndex)
            {
                //�ش� ��ġ�� ������ ������ �̹����� ���
                iconFrame->SetActive(true);
                iconFrame->Pos() = icon->GlobalPos();
                iconFrame->UpdateWorld();
                iconFrame->Render();
            }
            else iconFrame->SetActive(false); //������ �׳� ��� ����
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
    // �� ���� ����� �����ͼ� ��ϸŴ������� �佺�ϰ� �ױ� ȣ��

    // ���� � ����� �����ߴ��� ����
    if (selectedIconIndex < 0) return; // ������ ����� �������� ����

    //������ �ε����� ��� ���� (���� ������)
    //Cube* block = blocks[iconData[selectedIconIndex].first].back();

    //�ڼ��� ����
    //1. �κ��丮���� ���� ���� ���� �Լ������� ��� ������ �����´�
    int blockKey = iconData[selectedIconIndex].first;

    //2. ����� ��->���Ϳ��� ��� Ű�� ���͸�, �ش� ������ ���� ������ ������ �����´�
    Cube* block = blocks[blockKey].back();
    //            ������� ����   // �� ������ ������

    // �� ����� ��ϸŴ������� �佺�ϰ� �ױⰡ ȣ��Ǵ���(�׷��� �׿�����) Ȯ���ؼ�...
    if (BlockManager::Get()->BuildBlock(block))
    {
        // �� ������ ������ �ױⰡ �� ���̹Ƿ� ����� �����͸� ���Ϳ��� ����
        blocks[blockKey].pop_back(); // ���Ϳ��� ������ ���(=��� �� �װ�) ����
        iconData[selectedIconIndex].second--; // ������ ������ �ȿ� �ִ� ����� ������ -1

        // �� ��� ���� ���� ������ ����� ���� ���...
        if (iconData[selectedIconIndex].second == 0)
        {
            selectedIconIndex = -1; // ���� ����
        }
    }
}

void ActorUI::Mining()
{
    //�� �Լ��� ȣ��Ǵ� ��� ��� �Ŵ������� ����ĳ��Ʈ�� �� �� ��� ������
    Cube* block = BlockManager::Get()->GetCollisionBlock();

    if (block == nullptr) return; // �޾ƿ� �� ������ ����

    //���ᰡ ���� �ʾ����� ->
    //    block���� � �����Ͱ� �ְ� + ��ϸŴ����� �Ƹ� ��� �ϳ��� �������� ��

    // ���� �޾ƿ� ����� UI�� �����ͷ� ������ �Ű��ֱ�

    wstring file = block->GetMaterial()->GetDiffuseMap()->GetFile(); // �׸� ���� �̸� �ޱ�

    int tmpIndex = file.find_last_of('.'); // �Ű������� �� ���ڰ� ���������� ����� ���� ���ϱ�
    file = file.substr(tmpIndex - 1, 1); // ������ ���� �ε��� �ٷ� �� ���ڸ�, 1���ڸ�ŭ �ٽ� �����ؼ� ������ �ǵ��

    //������� ���� file�� �׸� ���ϸ� �ִ� 0���� 4������ ��� ���ڰ� ����

    int index = stoi(file);

    if (blocks[index].size() == 0) // ť�� �����Ϳ��� ������ ��ȣ�� �� ����� 0�̴�
        // ���� ť���� �ʿ�, �ش� ������ ������ �ִ� ��� ����
    {
        //�׷��� ���� �ϳ� �����

        for (int i = 0; i < 5; ++i)
        {
            if (iconData[i].second == 0) // ������ ������ ���� �� �ڸ��� ù������ ã�Ƽ�
            {
                //ã�� �ڸ��� �����ܰ� �κ��丮 ������ ���
                iconData[i].first = index; // �������� ������ ���� �޾ƿ� ���
                iconData[i].second++;
                break; //�����͸� �־����� �ݺ��� ������
            }
        }
    }
    else
    {
        // ������ ��ȣ�� �ش��ϴ� ť�� �����Ͱ� ť�� �ʿ� ���� ��
        // ���� ����� x �ִ� ������ �����ϱ�

        for (auto& data : iconData) // <-�ڷᱸ���� �м��ؼ� �׿� �´� �ڷ����� �ڵ� ����
            //   ������, ���߿� �ٽ� ���� �� �򰥸� ���� ����
            //   * ����õ������, �Ἥ ȿ���� �� ���� �и� �ִ�
//for (pair<int, pair<int, int>> data : iconData)
        {
            if (data.second.first == index) // ���� ������ ������ �� ����-���� �����(ù second)
                // ���� �����Ͱ� index�� ��ġ�ϸ� 
            {
                data.second.second++; // ���� �����͸� +1
                break;
            }
        }
    }

    // ������� ���� block�� ���� �ֱ� �־��� (�浹üũ)
    // �׸��� �ش� block�� ������ ���ڷ� �����ؼ� �����ܿ� ���޵� �ߴ� (�κ��丮)

    //���� ���� ���� ť�� �����͸� �ʿ� �����ϴ� �ͻ� (���߿� �� �����͸� �ױ⿡ �ٽ� Ȱ��)

    blocks[index].push_back(block); // index��� Key���� �� blocks�� Value�� �ʿ� �߰�
}
