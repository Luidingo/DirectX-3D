#pragma once

class ModelAnimator : public Model
{
protected: // ���߿� �ִϸ����͸� ��ӹ޴� �� �ٸ� Ŭ������ ���� ��

	struct Frame
	{
		int clip = 0;			// � ������
		UINT curFrame = 0;		// � ����(�ڼ�)��
		float time = 0;		// �󸶳�
		float scale = 1;		// ��� �ӵ���
		// ����� ���ΰ��� ��Ÿ���� ����ü
	};

	class FrameBuffer : public ConstBuffer
	{
	public:
		struct Data
		{
			float takeTime = 0.2f;	// ���۰� ����ð�
			float tweenTime = 0;	// ���۰� ��ȯ�ð� (�����Ͱ� A->B�� �ٲ�� �ð�)
			float runningTime = 0;	// ������ ����ð�
			float padding = 0;		// ������ �����ð�

			Frame cur;				// ���� ������
			Frame next;				// ���� ������
		};

	private:
		Data data;
		
	public:
		FrameBuffer() : ConstBuffer(&data, sizeof(Data)) 
		{
			data.next.clip = -1;	// "���� ������"�� �ϴ� ���� ������ ����
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

	// ���� �ܺ� ���ٿ� (�ε����� ����)
	ModelClip* GetClip(UINT index) { return clips[index]; }

protected:
	void CreateTexture();					// �ؽ�ó �����ϱ�
	void CreateClipTransform(UINT index);	// ���ۿ� ���� Ʈ������ �����ϱ�

	void UpdateFrame();						// ������ ������Ʈ

protected:
	// ���� ��� ����

	bool isPlay = true; // ������ ������ ��� ������ �⺻

	// ���۰� ���� ó���� ��ü��
	vector<ModelClip*> clips;

	FrameBuffer* frameBuffer;

	// ���ۿ��� ���� �� �ִ� Ʈ������ ���� �̸� �غ�
	ClipTransform* clipTransforms;
	ClipTransform* nodeTransforms;

	// �ؽ�ó�� �ؽ�ó�� ���̴��� �����ϱ� ���� ���� Ŭ����
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* srv;
};

