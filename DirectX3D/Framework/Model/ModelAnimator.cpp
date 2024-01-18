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
