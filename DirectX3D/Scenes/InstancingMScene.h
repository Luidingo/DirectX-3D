#pragma once

class InstancingMScene : public Scene
{
public :
	InstancingMScene();
	~InstancingMScene();

	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;
	
private:
	ModelInstancing* models;	// ������� �𵨵�
	int instanceID = 0;			// ���� ����(����) �ν��Ͻ�(�� ID)
	int SIZE = 10;				// ���� ���� ��� ��� ������ ���ΰ�?
};

