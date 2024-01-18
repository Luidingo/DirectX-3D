#pragma once

class ModelAnimator : public Model
{
protected: // 나중에 애니메이터를 상속받는 또 다른 클래스가 있을 것

	struct Frame
	{
		int clip = 0;			// 어떤 동작을
		UINT curFrame = 0;		// 어떤 순간(자세)을
		float time = 0;		// 얼마나
		float scale = 1;		// 어느 속도로
		// 재생할 것인가를 나타내는 구조체
	};

	class FrameBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			float takeTime = 0.2f;	// 버퍼간 재생시간
			float tweenTime = 0;	// 버퍼간 전환시간 (데이터가 A->B로 바뀌는 시간)
			float runningTime = 0;	// 버퍼의 재생시간
			float padding = 0;		// 버퍼의 여유시간

			Frame cur;				// 현재 프레임
			Frame next;				// 다음 프레임
		};

	private:
		Data data;
		
	public:
		FrameBuffer() : ConstBuffer(&data, sizeof(Data)) 
		{
			data.next.clip = -1;	// "다음 프레임"은 일단 없는 것으로 설정
		}
		Data& Get() { return data; }
	};

public:
	ModelAnimator(string name);
	~ModelAnimator();

	void Update();
	void Render();
	void PostRender();
	void GUIRender();

	void ReadClip(string clipName, UINT clipNum = 0);
	void PlayClip(int clip, float scale = 1, float takeTime = 0.2f);

	Matrix GetTransformByNode(int nodeIndex);

	// 동작 외부 접근용 (인덱스에 의해)
	ModelClip* GetClip(UINT index) { return clips[index]; }

protected:
	void CreateTexture();					// 텍스처 생성하기
	void CreateClipTransform(UINT index);	// 동작에 의한 트랜스폼 생성하기

	void UpdateFrame();						// 프레임 업데이트

protected:
	// 내부 멤버 변수

	bool isPlay = true; // 동작이 있으면 재생 시작이 기본

	// 동작과 동작 처리용 객체들
	vector<ModelClip*> clips;

	FrameBuffer* frameBuffer;

	// 동작에서 따올 수 있는 트랜스폼 정보 미리 준비
	ClipTransform* clipTransforms;
	ClipTransform* nodeTransforms;

	// 텍스처와 텍스처가 셰이더에 접근하기 위한 연산 클래스
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv;
};

