#pragma once

class CubeFieldScene : public Scene
{
public:
    CubeFieldScene();
    ~CubeFieldScene();

    virtual void Update() override;
    virtual void PreRender() override;
    virtual void Render() override;
    virtual void PostRender() override;
    virtual void GUIRender() override;

private:
    //void MineBuild();
    //void BlockTester();

    TestActor* tester;
    //vector<Cube*> myBlocks; //상호작용을 했다고 치고 사용 가능한 "내 블록" 벡터
                              //씬에서 만든 이유 = 액터 안에 없는 이유 = 이 씬에서만 쓰려고
};

