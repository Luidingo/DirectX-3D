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

	Model::Render();
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
// 노드 인덱스를 주면 해당 노드의 트랜스폼 정보를 돌려받는 함수
Matrix ModelAnimator::GetTransformByNode(int nodeIndex)
{
	if (texture == nullptr) // 애니메이터 클래스의 특징 상 텍스처가 없으면 흰색을 만들어서라도 쓴다 (Model 클래스도 마찬가지)
							// 흰색조차 없다 = 노드를 잘못 불렀거나 fbx가 문제가 있어서 노드 정보가 없다
	{
		return XMMatrixIdentity(); // 단위 행렬 = 없는 것과 마찬가지
	}

	// 텍스처가 있다 = 어쨌든 이곳에는 노드가 있다

	Matrix curAnim; // 현재 애니메이션 정보를 담은 임시 변수

	// 위 변수를 사용해서 노드 찾기를 한다
	// -> 1. 현재 동작 자체에 대해서
	{
		Frame& curFrame = frameBuffer->Get().cur;

		Matrix cur = nodeTransforms[curFrame.clip].transform[curFrame.curFrame][nodeIndex];
		Matrix next = nodeTransforms[curFrame.clip].transform[curFrame.curFrame + 1][nodeIndex];

		// 현재 동작의, 현재 프레임과, 이 다음 프레임을 합쳐서 보간한 후에 전체 트랜스폼과의 관계를 다시 낸다
		curAnim = Lerp(cur, next, curFrame.time) * world; // 일단은 자기 자신이 곧 전체 트랜스폼이라 나머지 항 생략
	}

	// -> 2. 다음 동작도 미리 예측해서
	{
		Frame& nextFrame = frameBuffer->Get().next;

		// 다음 동작이 없다면 현재 동작만으로 트랜스폼 정보는 다 나왔으므로 그대로 반환
		if (nextFrame.clip == -1) return curAnim;

		// 다음 동작이 예약된 게 있다면 (동작이 바뀌는 중일 것이므로) 그에 따른 트랜스폼 변화까지 포함
		Matrix cur = nodeTransforms[nextFrame.clip].transform[nextFrame.curFrame][nodeIndex];
		Matrix next = nodeTransforms[nextFrame.clip].transform[nextFrame.curFrame + 1][nodeIndex];

		// 다음 동작에서의 트랜스폼 정보 내기
		Matrix nextAnim = Lerp(cur, next, nextFrame.time) * world;

		// 전환시간 재기
		float tweenTime = frameBuffer->Get().tweenTime; // 나중에 조건이 달라지면 바꾸기

		// 최종 보간 후 반환
		return Lerp(curAnim, nextAnim, tweenTime);
	}
}

void ModelAnimator::CreateTexture()
{
	// 있으면 있는 대로 없어도 만들어서 모델을 위한 텍스처 적용하기

	UINT clipCount = clips.size(); // 얼마나 텍스처를 입힐 지

	// 변수를 이용해서 (포인터로서의) 배열을 동적 할당
	// * 이 방법으로 본인만의 벡터를 구현해볼 수도 있음
	clipTransforms = new ClipTransform[clipCount];
	nodeTransforms = new ClipTransform[clipCount];

	FOR(clipCount)				// 동작 수만큼 반복문을 돌려서
		CreateClipTransform(i);	// 동작에 대한 부분 트랜스폼 생성

	// DX에서 쓰일 텍스처 설정 옵션 구조체 생성
	// 그림이 없어서 만드는 것이기 때문에 일반적인 그림의 픽셀 정보가 들어가는 게 아니라
	// 새로 씌우기 위한 "규칙"을 만든다 (텍스처의 크기 : 뼈대와 동작 수를 따르고 처리방식은 기본으로)
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = MAX_BONE * 4;
	desc.Height = MAX_FRAME;
	desc.MipLevels = 1;
	desc.ArraySize = clipCount;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// 필수는 아니지만 텍스처의 기본 옵션이기 때문에
													// * 이 옵션은 텍스처(그림)이 셰이더를 통과해서
													//   모니터에 출력되기 위한 옵션 (셰이더 리소스)

	// 그림이 없으므로 가상의 이미지 연산 자리를 만들어서 할당까지 한다
	UINT pitchSize = MAX_BONE * sizeof(Matrix);
	UINT pageSize = pitchSize * MAX_FRAME;
	// -> 할당
	void* p = VirtualAlloc(nullptr, pageSize * clipCount, MEM_RESERVE, PAGE_READWRITE);

	FOR(clipCount) // 동작마다
	{
		UINT start = i * pageSize; // 시작점을 위에서 찾은 페이지 크기 (프레임과 본 크기에 따른 총 데이터 용량)

		for (UINT y = 0; y < MAX_FRAME; ++y)
		{
			void* tmp = (BYTE*)p + pitchSize * y + start; // 시작점을 기준으로 프레임별, 뼈대별 메모리 주소
			
			VirtualAlloc(tmp, pitchSize, MEM_COMMIT, PAGE_READWRITE);
			// 클립에서 딴 트랜스폼의 프레임 정보를, 주어진 "텍스처 크기"만큼, tmp에 복사
			memcpy(tmp, clipTransforms[i].transform[y], pitchSize);
		}
	}

	// DX용 출력 처리 변수 호출
	D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[clipCount];

	FOR(clipCount)
	{
		void* tmp = (BYTE*)p + i * pageSize;
		
		// 모든 동작마다 출력 정보 할당하기
		subResource[i].pSysMem = tmp; // 구성 메모리 설정 (= 데이터)
		subResource[i].SysMemPitch = pitchSize;
		subResource[i].SysMemSlicePitch = pageSize;
	}

	// 여기까지 오면...
	// 원래 그림이 있었다면 -> 원본 그림에 대응하는 동작별 변형된 텍스처 정보(그림)가 도출
	// 원래 그림이 없었다면 -> 동작의 트랜스폼 정보에 점별로 대응하는 기본 화소가 생성

	// 어느 쪽이든, 해당 정보를 텍스처 = 진짜 그림으로 만들기
	DEVICE->CreateTexture2D(&desc, subResource, &texture);
			// 어떤 정보를, 어떤 설정으로, 어떤 변수에 만들 것인가

	// 텍스처가 나왔으니 출력 정보(subResource) 용도 종료
	delete[] subResource;
	VirtualFree(p, 0, MEM_RELEASE); // 아까 설정한 포인터를, 별도의 크기 설정 없이(처음 설정받은 만큼)
									// 메모레 해제로 (쓰기 가능으로) 메모리 동적 할당을 종료

	// 셰이더 리소스의 렌더 정보(옵션) 구조체를 만든다
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; // 2D 텍스처의 묶음으로 보이기
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = clipCount; // 배열의 (기본) 크기 : 동작 만큼

	// 텍스처를 셰이더에 통과시켜서 실제 화면으로 만들기 위한 데이터 생성
	DEVICE->CreateShaderResourceView(texture, &srvDesc, &srv);
			// 어떤 텍스처를, 어떤 설정으로, 어떤 변수에 만들 것인가
}

void ModelAnimator::CreateClipTransform(UINT index)
{
	// 애니메이션 재생 시, 각 트랜스폼으로부터 동작 정보를 만들어 내는 함수
	ModelClip* clip = clips[index]; // 받은 동작 인덱스에 따른 클립(움직임) 정보 만들기
	
	FOR(clip->frameCount) // 프레임 수 만큼 반복
	{
		UINT nodeIndex = 0;

		for (NodeData node : nodes)
		{
			Matrix animation;
			KeyFrame* frame = clip->GetKeyFrame(node.name);

			if (frame != nullptr) // 프레임이 있다면
			{
				KeyTransform& transform = frame->transforms[i]; // 프레임에 있던 트랜스폼 정보 받기

				// DX 식으로 매트릭스를 만든 후, 매개변수로 설정을 거쳐서, animation에 대입
				animation = XMMatrixTransformation( // 매트릭스 생성 후 변형 함수
					XMVectorZero(),					// 시작 시 크기 : 초기값 = 0
					XMQuaternionIdentity(),			// 크기에 대한 보정 : 없음
					Vector3(transform.scale),		// 진짜 크기
					XMVectorZero(),					// 회전 기본 값 : 회전 안 함
					XMLoadFloat4(&transform.rot),	// 진짜 회전 정보
					Vector3(transform.pos)			// 행렬의 전이 정보 = 위치
				);
			}
			else // 모델은 동작이 있는데 이 노드는 부여받은 움직임 정보가 없다면
			{
				animation = XMMatrixIdentity();
			}

			Matrix parent;
			int parentIndex = node.parent;

			if (parentIndex < 0)
				parent = XMMatrixIdentity();
			else
				parent = nodeTransforms[index].transform[i][parentIndex];

			// 노드 자체가 가진 고유 애니메이션과, 부모의 (기준의) 트랜스폼 정보를 합쳐서
			// 최종 형태 만들기

			nodeTransforms[index].transform[i][nodeIndex] = animation * parent;

			// 뼈대와 비교하기
			int boneIndex = -1;
			if (boneMap.count(node.name)) // 노드에 맞는 뼈대가 있으면
				boneIndex = boneMap[node.name]; // 노드의 이름에 따른 뼈대의 인덱스 구하기

			if (boneIndex >= 0) // 본 인덱스가 적절한 노드라면
			{
				Matrix offset = bones[boneIndex].offset; // 뼈대의 움직임(조정치)을 받기
				offset *= nodeTransforms[index].transform[i][nodeIndex]; // 원래 노드 트랜스폼과 합치기

				// 계산 결과를 움직임용 트랜스폼 데이터에 대입
				clipTransforms[index].transform[i][boneIndex] = offset;
			}

			// 여기까지 오면 : (현재 노드) 애니메이션 연산 끝, 부모 연산 끝, 
			//				  애니메이션과 부모 합체 끝, 뼈대에 대한 보정 끝

			// 다 끝났으므로 다음 연산을 위해서 노드의 인덱스를 +1
			nodeIndex++;
		}
	}
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
