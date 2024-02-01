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
	ModelInstancing* models;	// 만들려는 모델들
	int instanceID = 0;			// 현재 선택(중인) 인스턴스(의 ID)
	int SIZE = 10;				// 모델이 많을 경우 어떻게 정렬할 것인가?
};

