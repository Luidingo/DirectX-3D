#include "Framework.h"

ModelClip::~ModelClip()
{
	for (auto keyFrame : keyFrames)
		delete keyFrame.second; // 키프레임 자료 지우기
}

void ModelClip::Init()
{
	// 이벤트 예약이 되었든, 되지 않았든, 동작과 동시에 수행할 수 있도록 준비

	eventIter = events.begin();				// 맵 검색용 반복자 초기화
	playTime = 0;							// 재생 시간 초기화
	duration = frameCount / tickPerSecond;	// 총 재생시간 = 총 프레임 수 ÷ 초당 프레임수
}

void ModelClip::Execute()
{
	// 재생 시간을 검사해서 이벤트가 예약된 시간이 있었으면 동시 이벤트 수행 시작

	if (events.empty()) return;				// 이벤트 예약이 없었으면 종료
	if (eventIter == events.end()) return;	// 이벤트가 끝까지 수행되었다고 확인되면 종료

	float ratio = playTime / duration;		// 경과 비율 = 재생 시간 ÷ 총 재생 시간

	if (ratio < eventIter->first) return;	// 경과 시간의 비율이 설정값에 모자라면 종료

	// 여기까지 오면 시간이 다 됐고, 예약된 이벤트가 있고, 
	// 이벤트 맵 내 반복자(차례)도 정해진 대로

	// -> 정해진 이벤트를 수행하고 차례를 +1

	eventIter->second(); // 포인터로 지정된 함수 실행
	eventIter++;		 // 차례 +1
}

KeyFrame* ModelClip::GetKeyFrame(string boneName)
{
	if (keyFrames.count(boneName) == 0) // 받은 뼈대 이름에 대응하는 동작이 없었으면
		return nullptr;

	// 있으면 있는대로 반환
	return keyFrames[boneName];
}

void ModelClip::SetEvent(Event event, float timeRatio)
{
	if (events.count(timeRatio) > 0) return; // 먼저 예약된 함수가 있으면 종료

	events[timeRatio] = event;
}
