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

	texture->Release(); // ���� ���� �ؽ�ó
	srv->Release();		// �� Ŭ���� ���� srv
}

void ModelAnimator::Update()
{
	UpdateFrame(); // ������ ������Ʈ
	UpdateWorld(); // �����ӿ� ���� ���� ��ȭ ������Ʈ
}

void ModelAnimator::Render()
{
	// �ؽ�ó ������ ������ ��������
	if (texture == nullptr) CreateTexture();

	frameBuffer->SetVS(3); // ���ۿ� ���� ���� ����
	DC->VSSetShaderResources(0, 1, &srv); // ��� ��ġ�� 0������ 1�� ������ ��� ������ srv ���뿡 ���� �����ؼ� �ؽ�ó�� ���� �� �ֵ��� ����

	Model::Render();
}

void ModelAnimator::PostRender()
{
	// 2D�� UI�� �����Ƿ� ����
}

void ModelAnimator::GUIRender()
{
	int clip = frameBuffer->Get().cur.clip; // ���� �������� Ŭ��(�ε���)
	ImGui::SliderInt("Clip", &frameBuffer->Get().cur.clip, 0, clips.size() - 1); 
											// ��ϵ� Ŭ����ŭ ������ �� �ְ� �ۼ�
	ImGui::SliderInt("Frame", (int*)&frameBuffer->Get().cur.curFrame,
		0, clips[clip]->frameCount - 1);	// ��ϵ� �����Ӹ�ŭ ������ �� �ְ� �ۼ�
	ImGui::SliderFloat("Scale", &frameBuffer->Get().cur.scale, 0, 10.0f);
											// ����(0)���� 10���(10.0f)����

	Model::GUIRender(); // �� ���� Ȯ�� �����ϵ���
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

	FOR(boneCount) // ���� ���� ����ŭ ���� + ������ �б� ����
	{
		KeyFrame* keyFrame = new KeyFrame();
		keyFrame->boneName = reader->String();

		UINT size = reader->UInt(); // ������ + ���븦 �����ϴ� Ʈ������ �� �ޱ�
		if (size > 0)
		{
			keyFrame->transforms.resize(size);

			void* ptr = (void*)keyFrame->transforms.data();	// �����͸� ���� �޸� �ּҷ� �޾Ƽ�
			reader->Byte(&ptr, sizeof(KeyTransform) * size); // �ִ� �״�� �����ؼ� �ֱ�
		}
		clip->keyFrames[keyFrame->boneName] = keyFrame;
	}

	// ����� �� ���뿡 ���� Ʈ������ ����(� �ڼ� ���ΰ�) ���ΰ� 1�� ���´�
	// = ���� 1���� ������ ���´�
	clips.push_back(clip); // �ݺ����� ���� �� ������ ���� : ������ ���۸��� ���۸��� �־�� �Ǵϱ�

	delete reader;
}

void ModelAnimator::PlayClip(int clip, float scale, float takeTime)
{
	isPlay = true; // ��� ���� true�� ����

	frameBuffer->Get().next.clip = clip; // ���� ������ Ŭ�� �ε����� ���� ��� ����
	frameBuffer->Get().next.scale = scale;
	frameBuffer->Get().takeTime = takeTime;

	// �� �� �ٷ� (�������� ������Ʈ ���̶��) �� ������ �ٲ� ��
	// ���� �� ���ۿ� ���� ���� �̺�Ʈ�� ���� ��� �غ��Ű�� ��
	clips[clip]->Init(); // ������ �̺�Ʈ �ʱ�ȭ �Լ� ����
}

// ���� �Լ� 3�� : �ִϸ��̼� ��� �� ���� ��� Ȥ�� ���� ����� ���� �Լ�
// ��� �ε����� �ָ� �ش� ����� Ʈ������ ������ �����޴� �Լ�
Matrix ModelAnimator::GetTransformByNode(int nodeIndex)
{
	if (texture == nullptr) // �ִϸ����� Ŭ������ Ư¡ �� �ؽ�ó�� ������ ����� ������ ���� (Model Ŭ������ ��������)
							// ������� ���� = ��带 �߸� �ҷ��ų� fbx�� ������ �־ ��� ������ ����
	{
		return XMMatrixIdentity(); // ���� ��� = ���� �Ͱ� ��������
	}

	// �ؽ�ó�� �ִ� = ��·�� �̰����� ��尡 �ִ�

	Matrix curAnim; // ���� �ִϸ��̼� ������ ���� �ӽ� ����

	// �� ������ ����ؼ� ��� ã�⸦ �Ѵ�
	// -> 1. ���� ���� ��ü�� ���ؼ�
	{
		Frame& curFrame = frameBuffer->Get().cur;

		Matrix cur = nodeTransforms[curFrame.clip].transform[curFrame.curFrame][nodeIndex];
		Matrix next = nodeTransforms[curFrame.clip].transform[curFrame.curFrame + 1][nodeIndex];

		// ���� ������, ���� �����Ӱ�, �� ���� �������� ���ļ� ������ �Ŀ� ��ü Ʈ���������� ���踦 �ٽ� ����
		curAnim = Lerp(cur, next, curFrame.time) * world; // �ϴ��� �ڱ� �ڽ��� �� ��ü Ʈ�������̶� ������ �� ����
	}

	// -> 2. ���� ���۵� �̸� �����ؼ�
	{
		Frame& nextFrame = frameBuffer->Get().next;

		// ���� ������ ���ٸ� ���� ���۸����� Ʈ������ ������ �� �������Ƿ� �״�� ��ȯ
		if (nextFrame.clip == -1) return curAnim;

		// ���� ������ ����� �� �ִٸ� (������ �ٲ�� ���� ���̹Ƿ�) �׿� ���� Ʈ������ ��ȭ���� ����
		Matrix cur = nodeTransforms[nextFrame.clip].transform[nextFrame.curFrame][nodeIndex];
		Matrix next = nodeTransforms[nextFrame.clip].transform[nextFrame.curFrame + 1][nodeIndex];

		// ���� ���ۿ����� Ʈ������ ���� ����
		Matrix nextAnim = Lerp(cur, next, nextFrame.time) * world;

		// ��ȯ�ð� ���
		float tweenTime = frameBuffer->Get().tweenTime; // ���߿� ������ �޶����� �ٲٱ�

		// ���� ���� �� ��ȯ
		return Lerp(curAnim, nextAnim, tweenTime);
	}
}

void ModelAnimator::CreateTexture()
{
	// ������ �ִ� ��� ��� ���� ���� ���� �ؽ�ó �����ϱ�

	UINT clipCount = clips.size(); // �󸶳� �ؽ�ó�� ���� ��

	// ������ �̿��ؼ� (�����ͷμ���) �迭�� ���� �Ҵ�
	// * �� ������� ���θ��� ���͸� �����غ� ���� ����
	clipTransforms = new ClipTransform[clipCount];
	nodeTransforms = new ClipTransform[clipCount];

	FOR(clipCount)				// ���� ����ŭ �ݺ����� ������
		CreateClipTransform(i);	// ���ۿ� ���� �κ� Ʈ������ ����

	// DX���� ���� �ؽ�ó ���� �ɼ� ����ü ����
	// �׸��� ��� ����� ���̱� ������ �Ϲ����� �׸��� �ȼ� ������ ���� �� �ƴ϶�
	// ���� ����� ���� "��Ģ"�� ����� (�ؽ�ó�� ũ�� : ����� ���� ���� ������ ó������� �⺻����)
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = MAX_BONE * 4;
	desc.Height = MAX_FRAME;
	desc.MipLevels = 1;
	desc.ArraySize = clipCount;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// �ʼ��� �ƴ����� �ؽ�ó�� �⺻ �ɼ��̱� ������
													// * �� �ɼ��� �ؽ�ó(�׸�)�� ���̴��� ����ؼ�
													//   ����Ϳ� ��µǱ� ���� �ɼ� (���̴� ���ҽ�)

	// �׸��� �����Ƿ� ������ �̹��� ���� �ڸ��� ���� �Ҵ���� �Ѵ�
	UINT pitchSize = MAX_BONE * sizeof(Matrix);
	UINT pageSize = pitchSize * MAX_FRAME;
	// -> �Ҵ�
	void* p = VirtualAlloc(nullptr, pageSize * clipCount, MEM_RESERVE, PAGE_READWRITE);

	FOR(clipCount) // ���۸���
	{
		UINT start = i * pageSize; // �������� ������ ã�� ������ ũ�� (�����Ӱ� �� ũ�⿡ ���� �� ������ �뷮)

		for (UINT y = 0; y < MAX_FRAME; ++y)
		{
			void* tmp = (BYTE*)p + pitchSize * y + start; // �������� �������� �����Ӻ�, ���뺰 �޸� �ּ�
			
			VirtualAlloc(tmp, pitchSize, MEM_COMMIT, PAGE_READWRITE);
			// Ŭ������ �� Ʈ�������� ������ ������, �־��� "�ؽ�ó ũ��"��ŭ, tmp�� ����
			memcpy(tmp, clipTransforms[i].transform[y], pitchSize);
		}
	}

	// DX�� ��� ó�� ���� ȣ��
	D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[clipCount];

	FOR(clipCount)
	{
		void* tmp = (BYTE*)p + i * pageSize;
		
		// ��� ���۸��� ��� ���� �Ҵ��ϱ�
		subResource[i].pSysMem = tmp; // ���� �޸� ���� (= ������)
		subResource[i].SysMemPitch = pitchSize;
		subResource[i].SysMemSlicePitch = pageSize;
	}

	// ������� ����...
	// ���� �׸��� �־��ٸ� -> ���� �׸��� �����ϴ� ���ۺ� ������ �ؽ�ó ����(�׸�)�� ����
	// ���� �׸��� �����ٸ� -> ������ Ʈ������ ������ ������ �����ϴ� �⺻ ȭ�Ұ� ����

	// ��� ���̵�, �ش� ������ �ؽ�ó = ��¥ �׸����� �����
	DEVICE->CreateTexture2D(&desc, subResource, &texture);
			// � ������, � ��������, � ������ ���� ���ΰ�

	// �ؽ�ó�� �������� ��� ����(subResource) �뵵 ����
	delete[] subResource;
	VirtualFree(p, 0, MEM_RELEASE); // �Ʊ� ������ �����͸�, ������ ũ�� ���� ����(ó�� �������� ��ŭ)
									// �޸� ������ (���� ��������) �޸� ���� �Ҵ��� ����

	// ���̴� ���ҽ��� ���� ����(�ɼ�) ����ü�� �����
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; // 2D �ؽ�ó�� �������� ���̱�
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = clipCount; // �迭�� (�⺻) ũ�� : ���� ��ŭ

	// �ؽ�ó�� ���̴��� ������Ѽ� ���� ȭ������ ����� ���� ������ ����
	DEVICE->CreateShaderResourceView(texture, &srvDesc, &srv);
			// � �ؽ�ó��, � ��������, � ������ ���� ���ΰ�
}

void ModelAnimator::CreateClipTransform(UINT index)
{
	// �ִϸ��̼� ��� ��, �� Ʈ���������κ��� ���� ������ ����� ���� �Լ�
	ModelClip* clip = clips[index]; // ���� ���� �ε����� ���� Ŭ��(������) ���� �����
	
	FOR(clip->frameCount) // ������ �� ��ŭ �ݺ�
	{
		UINT nodeIndex = 0;

		for (NodeData node : nodes)
		{
			Matrix animation;
			KeyFrame* frame = clip->GetKeyFrame(node.name);

			if (frame != nullptr) // �������� �ִٸ�
			{
				KeyTransform& transform = frame->transforms[i]; // �����ӿ� �ִ� Ʈ������ ���� �ޱ�

				// DX ������ ��Ʈ������ ���� ��, �Ű������� ������ ���ļ�, animation�� ����
				animation = XMMatrixTransformation( // ��Ʈ���� ���� �� ���� �Լ�
					XMVectorZero(),					// ���� �� ũ�� : �ʱⰪ = 0
					XMQuaternionIdentity(),			// ũ�⿡ ���� ���� : ����
					Vector3(transform.scale),		// ��¥ ũ��
					XMVectorZero(),					// ȸ�� �⺻ �� : ȸ�� �� ��
					XMLoadFloat4(&transform.rot),	// ��¥ ȸ�� ����
					Vector3(transform.pos)			// ����� ���� ���� = ��ġ
				);
			}
			else // ���� ������ �ִµ� �� ���� �ο����� ������ ������ ���ٸ�
			{
				animation = XMMatrixIdentity();
			}

			Matrix parent;
			int parentIndex = node.parent;

			if (parentIndex < 0)
				parent = XMMatrixIdentity();
			else
				parent = nodeTransforms[index].transform[i][parentIndex];

			// ��� ��ü�� ���� ���� �ִϸ��̼ǰ�, �θ��� (������) Ʈ������ ������ ���ļ�
			// ���� ���� �����

			nodeTransforms[index].transform[i][nodeIndex] = animation * parent;

			// ����� ���ϱ�
			int boneIndex = -1;
			if (boneMap.count(node.name)) // ��忡 �´� ���밡 ������
				boneIndex = boneMap[node.name]; // ����� �̸��� ���� ������ �ε��� ���ϱ�

			if (boneIndex >= 0) // �� �ε����� ������ �����
			{
				Matrix offset = bones[boneIndex].offset; // ������ ������(����ġ)�� �ޱ�
				offset *= nodeTransforms[index].transform[i][nodeIndex]; // ���� ��� Ʈ�������� ��ġ��

				// ��� ����� �����ӿ� Ʈ������ �����Ϳ� ����
				clipTransforms[index].transform[i][boneIndex] = offset;
			}

			// ������� ���� : (���� ���) �ִϸ��̼� ���� ��, �θ� ���� ��, 
			//				  �ִϸ��̼ǰ� �θ� ��ü ��, ���뿡 ���� ���� ��

			// �� �������Ƿ� ���� ������ ���ؼ� ����� �ε����� +1
			nodeIndex++;
		}
	}
}

// ���� �Լ� : ������ ������Ʈ (�ִϸ��̼� ���)

void ModelAnimator::UpdateFrame()
{
	if (!isPlay) return;

	FrameBuffer::Data& frameData = frameBuffer->Get();

	// ���� ������ ������ ����
	{
		ModelClip* clip = clips[frameData.cur.clip]; // �����ʹ� ���������� ���� �̸��� {} �ȿ����� ��ȿ

		// ������ ��� �ð��� �� ������ ���� ��� ����� ��� ����
		clip->playTime += frameData.cur.scale * DELTA;
		frameData.cur.time += clip->tickPerSecond * frameData.cur.scale * DELTA;

		if (frameData.cur.time >= 1) // �������� ����� �ð��� ������ ���� �ð��� ������
		{
			// ������ ���� +1 �ϰ�, ��ü ������ ������ ���缭 �������ش� 
			// (������ �ʰ����� �ʰ� �ݺ��ϵ���)
			frameData.cur.curFrame = (frameData.cur.curFrame + 1) % (clip->frameCount - 1);
			frameData.cur.time -= 1.0f; // ������ �����ϴٴ� ���� �Ͽ� ������ ������ 1�ʷ� ����
			//frameData.cur.time = 0;	// ���� �߻��� �����ϴٴ� ���� �Ͽ� ���� ���� ������ ����
		}

		// ������ ��� �ø��� ���� �̺�Ʈ�� �ִ��� Ȯ���ϰ�, ������ ������ ����
		clip->Execute();
	}

	// ���� ������ ������ ���� (���� ������ ���� ��쿡��)
	{
		if (frameData.next.clip < 0) return; // ���� ������ ������ �Ʒ� ����� �ʿ� ����

		ModelClip* clip = clips[frameData.next.clip]; // clip�̶� �̸��� �� �Ἥ, ���� ���� ������ �ޱ�

		frameData.tweenTime += DELTA / frameData.takeTime; // ��ȯ �ð��� ���� �ð� ������ �°� ���� Ȯ��

		if (frameData.tweenTime >= 1)
		{
			frameData.cur = frameData.next; // ������ ��ȯ

			// ��ȯ �� ������ �ʱ�ȭ
			frameData.tweenTime = 0;
			frameData.next.clip = -1; // ���� ��ȯ �������� ���� ���� ����
			frameData.next.curFrame = 0;
			frameData.next.time = 0;
			return; // ��ȯ ����
		}

		// ��� �ð� ���� (�����Ű��)
		frameData.next.time += clip->tickPerSecond * frameData.next.scale * DELTA;

		// ���� ���ۿ� ����� ������ �����͵� �̸�(Ȥ�� ���ÿ�) ��� �����ϱ�
		if (frameData.next.time >= 1)
		{
			// ������ ��ȯ �� �ð� ����
			frameData.next.curFrame = (frameData.next.curFrame + 1) % (clip->frameCount - 1);
			frameData.next.time -= 1.0f;
		}
	}
}
