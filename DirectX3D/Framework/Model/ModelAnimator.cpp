#include "Framework.h"

ModelAnimator::ModelAnimator(string name) : Model(name)
{
	SetShader(L"Model/ModelAnimation.hlsl");

	frameBuffer = new FrameBuffer();

	worldBuffer->SetType(1);
}

ModelAnimator::~ModelAnimator()
{
	for (ModelClip* clip : clips)
		delete clip;

	delete frameBuffer;

	delete[] clipTransforms;
	delete[] nodeTransforms;

	texture->Release(); // 따로 만든 텍스처
	srv->Release();		// 이 클래스 내의 srv
}

void ModelAnimator::Update()
{
	UpdateFrame(); // 프레임 업데이트
	UpdateWorld(); // 프레임에 따른 정점 변화 업데이트
}

void ModelAnimator::Render()
{
	// 텍스처 정보가 없으면 만들기부터
	if (texture == nullptr) CreateTexture();

	frameBuffer->SetVS(3); // 버퍼에 연산 슬롯 배정
	DC->VSSetShaderResources(0, 1, &srv); // 출력 장치에 0번부터 1개 단위의 출력 슬롯을 srv 내용에 따라 배정해서 텍스처가 나올 수 있도록 설정
}

void ModelAnimator::PostRender()
{
	// 2D용 UI가 없으므로 보류
}

void ModelAnimator::GUIRender()
{
	int clip = frameBuffer->Get().cur.clip; // 현재 프레임의 클립(인덱스)
	ImGui::SliderInt("Clip", &frameBuffer->Get().cur.clip, 0, clips.size() - 1); 
											// 등록된 클립만큼 변경할 수 있게 작성
	ImGui::SliderInt("Frame", (int*)&frameBuffer->Get().cur.curFrame,
		0, clips[clip]->frameCount - 1);	// 등록된 프레임만큼 변경할 수 있게 작성
	ImGui::SliderFloat("Scale", &frameBuffer->Get().cur.scale, 0, 10.0f);
											// 정지(0)부터 10배속(10.0f)까지

	Model::GUIRender(); // 모델 정보 확인 가능하도록
}

void ModelAnimator::ReadClip(string clipName, UINT clipNum)
{
	string path = "Models/Clips/" + name + "/" + clipName + to_string(clipNum) + ".clip";

	BinaryReader* reader = new BinaryReader(path);

	ModelClip* clip = new ModelClip();
	clip->name = reader->String();
	clip->frameCount = reader->UInt();
	clip->tickPerSecond = reader->Float();

	UINT boneCount = reader->UInt();

	FOR(boneCount) // 읽은 뼈대 수만큼 뼈대 + 프레임 읽기 진행
	{
		KeyFrame* keyFrame = new KeyFrame();
		keyFrame->boneName = reader->String();

		UINT size = reader->UInt(); // 프레임 + 뼈대를 구성하는 트랜스폼 수 받기
		if (size > 0)
		{
			keyFrame->transforms.resize(size);

			void* ptr = (void*)keyFrame->transforms.data();	// 데이터를 순수 메모리 주소로 받아서
			reader->Byte(&ptr, sizeof(KeyTransform) * size); // 있는 그대로 복제해서 넣기
		}
		clip->keyFrames[keyFrame->boneName] = keyFrame;
	}

	// 뼈대와 그 뼈대에 대한 트랜스폼 정보(어떤 자세 중인가) 여부가 1개 나온다
	// = 동작 1개의 정보가 나온다
	clips.push_back(clip); // 반복문을 따로 안 돌리는 이유 : 동작은 동작마다 동작명이 있어야 되니까

	delete reader;
}

void ModelAnimator::PlayClip(int clip, float scale, float takeTime)
{
	isPlay = true; // 재생 여부 true로 변경

	frameBuffer->Get().next.clip = clip; // 다음 동작의 클립 인덱스를 받은 대로 세팅
	frameBuffer->Get().next.scale = scale;
	frameBuffer->Get().takeTime = takeTime;

	// 위 세 줄로 (프레임이 업데이트 중이라면) 곧 동작이 바뀔 것
	// 남은 건 동작에 대항 동시 이벤트가 있을 경우 준비시키는 것
	clips[clip]->Init(); // 동작의 이벤트 초기화 함수 실행
}

// 이하 함수 3개 : 애니메이션 재생 중 정보 얻기 혹은 정보 부재시 생성 함수

Matrix ModelAnimator::GetTransformByNode(int nodeIndex)
{
	return Matrix();
}

void ModelAnimator::CreateTexture()
{
}

void ModelAnimator::CreateClipTransform(UINT index)
{
}

// 이하 함수 : 프레임 업데이트 (애니메이션 재생)

void ModelAnimator::UpdateFrame()
{
	if (!isPlay) return;

	FrameBuffer::Data& frameData = frameBuffer->Get();

	// 현재 동작의 프레임 연산
	{
		ModelClip* clip = clips[frameData.cur.clip]; // 데이터는 유지되지만 변수 이름은 {} 안에서만 유효

		// 동작의 재생 시간과 각 프레임 내의 재생 경과를 모두 측정
		clip->playTime += frameData.cur.scale * DELTA;
		frameData.cur.time += clip->tickPerSecond * frameData.cur.scale * DELTA;

		if (frameData.cur.time >= 1) // 프레임이 진행된 시간이 정해진 기준 시간을 넘으면
		{
			// 프레임 수를 +1 하고, 전체 프레임 개수에 맞춰서 리셋해준다 
			// (범위를 초과하지 않고 반복하도록)
			frameData.cur.curFrame = (frameData.cur.curFrame + 1) % (clip->frameCount - 1);
			frameData.cur.time -= 1.0f; // 오차가 일정하다는 전제 하에 프레임 간격을 1초로 유지
			//frameData.cur.time = 0;	// 오차 발생이 일정하다는 전제 하에 연산 수행 시점을 유지
		}

		// 프레임 재생 시마다 동시 이벤트가 있는지 확인하고, 조건이 맞으면 실행
		clip->Execute();
	}

	// 다음 동작의 프레임 연산 (다음 동작이 있을 경우에만)
	{
		if (frameData.next.clip < 0) return; // 다음 동작이 없으면 아래 계산은 필요 없음

		ModelClip* clip = clips[frameData.next.clip]; // clip이란 이름을 또 써서, 다음 동작 데이터 받기

		frameData.tweenTime += DELTA / frameData.takeTime; // 전환 시간을 지연 시간 비율에 맞게 누적 확인

		if (frameData.tweenTime >= 1)
		{
			frameData.cur = frameData.next; // 동작을 전환

			// 전환 후 데이터 초기화
			frameData.tweenTime = 0;
			frameData.next.clip = -1; // 동작 전환 끝났으니 다음 동작 없음
			frameData.next.curFrame = 0;
			frameData.next.time = 0;
			return; // 전환 종료
		}

		// 재생 시간 보기 (진행시키기)
		frameData.next.time += clip->tickPerSecond * frameData.next.scale * DELTA;

		// 다음 동작에 대비한 프레임 데이터도 미리(혹은 동시에) 재생 진행하기
		if (frameData.next.time >= 1)
		{
			// 프레임 순환 후 시간 빼기
			frameData.next.curFrame = (frameData.next.curFrame + 1) % (clip->frameCount - 1);
			frameData.next.time -= 1.0f;
		}
	}
}
