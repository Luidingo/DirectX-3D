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
    //vector<Cube*> myBlocks; //��ȣ�ۿ��� �ߴٰ� ġ�� ��� ������ "�� ���" ����
                              //������ ���� ���� = ���� �ȿ� ���� ���� = �� �������� ������
};

