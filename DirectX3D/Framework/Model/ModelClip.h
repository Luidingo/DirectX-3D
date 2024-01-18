#pragma once

class ModelClip
{
private:
	// ���η��� Ŭ����, ���� Ŭ���� :	�ܺο� �������� �ʰ�, �ٸ� Ŭ������ �μ�ǰ���θ� ������ ���� Ŭ����
	//								���ο� Ŭ������ Ư���� �뵵�� ������ ���տ� Ưȭ�� ��찡 ����
	//								��� : �̱��� (���� Ư���� �����͸� �ٷ��, �ܺο��� "�̱���"�̶�� ��ü�� ���ؼ� ����)
	//								������ A Ŭ������ �ְ�, �ش� Ŭ������ �۾��� �д��ϱ� ���� ���� Ŭ�����μ� ����

	// �� Ŭ������ �������� �����ϱ� ���� ��� Ŭ���� ����
	friend class ModelAnimator;					// �ִϸ��̼� Ȥ�� �ִϸ��̼��� ���� ���� ���� Ŭ�����μ� ����

	ModelClip() = default;
	~ModelClip();

	// �ִϸ����Ͱ� ���� ���� ���� ������ �Լ�
	void Init();								// �ʱ�ȭ, �׸��� ���� ����(���)
	void Execute();								// ���� �Ǻ� (���� ���� �ÿ� ���� ����)

	KeyFrame* GetKeyFrame(string boneName);

public:
	void SetEvent(Event event, float timeRatio);
	// Event = std::function<void()>. ���� �������� ���� �Լ��� ��Ī�ϴ� ������

private:
	// Ŭ���� ���� ��� ������

	string name;								// ���� �̸�

	UINT frameCount;							// �� ���ۿ� ��������(������ �ڼ���) �� �� �ִ���
	float tickPerSecond;						// �� ������ 1�ʿ� �� �������� ����ϴ���
	float duration;								// �� ������ �⺻ ��� �ð��� ������

	float playTime = 0;							// ���� ����� �ð�

	unordered_map<string, KeyFrame*> keyFrames;	// �̸��� Ű�����ӵ�

	map<float, Event> events;					// ���ۿ� ���� �̺�Ʈ(���۰� ���ÿ� ������ �ڵ�)
	map<float, Event>::iterator eventIter;		// �̺�Ʈ�� �ݺ���
};

