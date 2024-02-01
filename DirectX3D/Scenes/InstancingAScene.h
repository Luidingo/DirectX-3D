#pragma once

class InstancingAScene : public Scene
{
public:
	InstancingAScene();
	~InstancingAScene();

	// Scene을(를) 통해 상속됨
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void GUIRender() override;

private:
	ModelAnimatorInstancing* models;

	int instanceID = 0;
	int clip = 0;
	
	int SIZE = 10;

	int WIDTH = 9;
	int HEIGHT = 12;
};

