#pragma once

class ActorUI
{
    
public:
    ActorUI();
    ~ActorUI();

    void Update();
    void Render();

    void PostRender();
    void GUIRender();

    void Build();
    void Mining();

private:
    //��� �Լ�

private:
    //��� ����

    Quad* crosshair;
    Quad* quickSlot;
    Quad* iconFrame;

    vector<Quad*> blockIcons;
    map<int, pair<int, int>> iconData;
    map<int, vector<Cube*>> blocks;

    int selectedIconIndex = -1;
};

