#include "Framework.h"

ModelClip::~ModelClip()
{
	for (auto keyFrame : keyFrames)
		delete keyFrame.second; // Ű������ �ڷ� �����
}

void ModelClip::Init()
{
	// �̺�Ʈ ������ �Ǿ���, ���� �ʾҵ�, ���۰� ���ÿ� ������ �� �ֵ��� �غ�

	eventIter = events.begin();				// �� �˻��� �ݺ��� �ʱ�ȭ
	playTime = 0;							// ��� �ð� �ʱ�ȭ
	duration = frameCount / tickPerSecond;	// �� ����ð� = �� ������ �� �� �ʴ� �����Ӽ�
}

void ModelClip::Execute()
{
	// ��� �ð��� �˻��ؼ� �̺�Ʈ�� ����� �ð��� �־����� ���� �̺�Ʈ ���� ����

	if (events.empty()) return;				// �̺�Ʈ ������ �������� ����
	if (eventIter == events.end()) return;	// �̺�Ʈ�� ������ ����Ǿ��ٰ� Ȯ�εǸ� ����

	float ratio = playTime / duration;		// ��� ���� = ��� �ð� �� �� ��� �ð�

	if (ratio < eventIter->first) return;	// ��� �ð��� ������ �������� ���ڶ�� ����

	// ������� ���� �ð��� �� �ư�, ����� �̺�Ʈ�� �ְ�, 
	// �̺�Ʈ �� �� �ݺ���(����)�� ������ ���

	// -> ������ �̺�Ʈ�� �����ϰ� ���ʸ� +1

	eventIter->second(); // �����ͷ� ������ �Լ� ����
	eventIter++;		 // ���� +1
}

KeyFrame* ModelClip::GetKeyFrame(string boneName)
{
	if (keyFrames.count(boneName) == 0) // ���� ���� �̸��� �����ϴ� ������ ��������
		return nullptr;

	// ������ �ִ´�� ��ȯ
	return keyFrames[boneName];
}

void ModelClip::SetEvent(Event event, float timeRatio)
{
	if (events.count(timeRatio) > 0) return; // ���� ����� �Լ��� ������ ����

	events[timeRatio] = event;
}
