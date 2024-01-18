#pragma once

class ModelClip
{
private:
	// 내부략용 클래스, 내장 클래스 :	외부에 공개되지 않고, 다른 클래스의 부속품으로만 돌리기 위한 클래스
	//								내부용 클래스는 특정한 용도나 데이터 집합에 특화된 경우가 많다
	//								사례 : 싱글톤 (각각 특정한 데이터만 다루고, 외부에는 "싱글톤"이라는 매체를 통해서 공개)
	//								공개된 A 클래스가 있고, 해당 클래스의 작업을 분담하기 위한 하위 클래스로서 존재

	// 주 클래스와 소유권을 공유하기 위한 결속 클래스 선언
	friend class ModelAnimator;					// 애니메이션 혹은 애니메이션을 가진 모델의 하위 클래스로서 존재

	ModelClip() = default;
	~ModelClip();

	// 애니메이터가 쓰기 위한 내부 설정용 함수
	void Init();								// 초기화, 그리고 실행 시작(대기)
	void Execute();								// 실행 판별 (조건 충족 시에 실제 실행)

	KeyFrame* GetKeyFrame(string boneName);

public:
	void SetEvent(Event event, float timeRatio);
	// Event = std::function<void()>. 아직 정해지지 않은 함수를 지칭하는 포인터

private:
	// 클래스 내부 멤버 변수들

	string name;								// 동작 이름

	UINT frameCount;							// 이 동작에 프레임이(끊어진 자세가) 몇 개 있는지
	float tickPerSecond;						// 이 동작은 1초에 몇 프레임을 재생하는지
	float duration;								// 이 동작의 기본 재생 시간은 얼마인지

	float playTime = 0;							// 현재 재생된 시간

	unordered_map<string, KeyFrame*> keyFrames;	// 이름별 키프레임들

	map<float, Event> events;					// 동작에 붙은 이벤트(동작과 동시에 수행할 코드)
	map<float, Event>::iterator eventIter;		// 이벤트의 반복자
};

