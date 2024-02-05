#pragma once

class ParticleSystem
{
	// ��ƼŬ �ý��� (Ȥ�� ���� ��ƼŬ) Ŭ����
	// �׸� �����͸� ����� �޾Ƽ� �ɼǿ� ���� ����ϴ� ���� Ŭ����

	// -> ���� ��ƼŬ�� ���� ��������, ���ϴ� �ɼǿ� ���� ��ƼŬ�� �����ؼ� �� �� �ִ�

private:
	struct InstanceData // ��ƼŬ ������ ������ ���� ������ ������
	{
		Matrix transform = XMMatrixIdentity();
	};

	struct ParticleData // ��ƼŬ ��ü�� ������ = ��ü�� �ɼ�
	{
		// �Ϲ������� ��ƼŬ�� ����Ǵ� �پ��� �ɼ� ��, ���� ���������� ���̴� �ٽ� �κ��� �ۼ�
		// �߰��� �ʿ��� �ɼ��� ������ ���⼭ �ۼ�

		bool isLoop = false; // �ݺ��ϴ°�?
		bool isAdditive = true; // ȥ�� �����ΰ�?
		bool isBillboard = true; // ������ ����ΰ�(ī�޶� ������� ������ ���̴°�?)
		UINT count = 100; // ��ƼŬ�� ��� ���� Ȥ�� ��ƼŬ �� ���� �ν��Ͻ��� ���� (�⺻�� ����)
		float duration = 1.f; // ��ƼŬ(= ����Ʈ)�� ���� �ð�
		Vector3 minVelocity = { -1, -1, -1 }; // �ּ� �ӷ� ���� (Ȥ�� ���� ������ -��)
		Vector3 maxVelocity = { +1, +1, +1 }; // �ִ� �ӷ� ���� (Ȥ�� ���� ������ +��)
		Vector3 minAcceleration; // �ּ� ���ӷ� (Ȥ�� �ӵ��� ��ȭ��)
		Vector3 maxAcceleration; // �ִ� ���ӷ� (Ȥ�� �ӵ��� ��ȭ��)
		Vector3 minStartScale = { 1, 1, 1 }; // ��ƼŬ ���� �� �ּ� ũ��
		Vector3 maxStartScale = { 1, 1, 1 }; // ��ƼŬ ���� �� �ִ� ũ��
		Vector3 minEndScale = { 1, 1, 1 }; // ��ƼŬ ���� �� �ּ� ũ��
		Vector3 maxEndScale = { 1, 1, 1 }; // ��ƼŬ ���� �� �ִ� ũ��
		float minSpeed = 1; // �ּ� ���� �̵� �ӷ� (Ȥ�� �ӷ��� ��� ���)
		float maxSpeed = 3; // �ִ� ���� �̵� �ӷ� (Ȥ�� �ӷ��� ��� ���)
		float minAngularVelocity = -10.f; // �ּ� ���� ������
		float maxAngularVelocity = +10.f; // �ִ� ���� ������
		float minStartTime = 0.f; // �ּ� ���� ���� �ð�
		float maxStartTime = 0.f; // �ִ� ���� ���� �ð�
		Float4 startColor = { 1, 1, 1, 1 }; // ���� �� ����� ���� ����(Ȥ�� ���� ����)
		Float4 endColor = { 1, 1, 1, 1 }; // ���� �� ����� ���� ����(Ȥ�� ���� ����)

		// -> ���� ����, ���̴� ���� ���� ������� �ʰ� �ǵ� ������ �ּ����� �ɼ��� �ۼ�
		// -> �� �ɼ��� �������� �� ���ڰ� ������� �� �������� �ɼ��� ����
	};

	// �׷��� ������ ������� �� ������ (Ȥ�� ��ƼŬ ��ü �����)
	// ���� ���� ������ Ȯ���ϱ� ���� ����ü ����
	struct ParticleInfo
	{
		Transform transform; // �� ���� Ȥ�� ��ü �߽��� Ʈ������
		Vector3 velocity; // �� �����Ϳ��� �������� ���� �ӵ��� ����
		Vector3 accleration; // �� �����Ϳ��� �������� ���� ���ӷ�
		Vector3 startScale; // ���۽� ũ��
		Vector3 endScale; // ����� ũ��

		// �⺻�� �ִ� �Ҽ��� (���� ������ �ϸ� �⺻���� �����Ƿ�)
		float speed  = 1.f;
		float angularVelocity = 0.f;
		float startTime = 0.f;
		
		// -> ������� ���� �ɼ� Ȯ�� Ȥ�� (�ʿ��ϸ�) ���� � ���δ�
		// �� ���ڰ� ����̸� -> ���� �ϳ��ϳ��� �����ϴ� ����
		// ����� ����̸� -> ��ƼŬ ��ü�� �뺯�ϴ� ����

		// �켱�� �� ����ü�� �� ������ ������ Ȱ��
	};

public:
	ParticleSystem(string file);	// ���� �����̵�, �̹��� ���� �ε��̵�,
	ParticleSystem(wstring file);	// �̹��� ���� ��ƼŬ�� ������ ���� ����
	~ParticleSystem();

	void Update();
	void Render();
	void GUIRender();

	void Play(Vector3 pos, Vector3 rot = Vector3());	// ��ƼŬ�� ����� ��ġ�� ȸ�� ����
														// ȸ�� ������ �ʿ��� ���� = �����尡 �ƴ� ���� �־
														// �Ű����� �⺻���� {0,0,0}�� ���� : �����尡 �⺻�̶�

	void Stop(); // �ܵ� Ŭ������ ���ߴ� �͵� �Լ��� ���� �ʿ��ϴ�

	bool IsActive() { return quad->Active(); } // ������ Ȱ��ȭ ���ο��� ��ƼŬ�� Ȱ��ȭ �Ǵ�
private:
	void UpdatePhysical();	// ������ ��� = ���� ���� ������Ʈ
	void UpdateColor();		// ���� ������Ʈ
	void Init();			// ����� ����

	void LoadData(string file);	// C++�� ���� ���� �б� : �ؽ�Ʈ �б� Ŭ������ ���� ������ ���� �б�
	void LoadData(wstring file); // �����쿡 ���� ���ҽ� ���� : �̹��� ���ҽ��� ���� �б�

private:
	// ��ƼŬ�� �ʿ��� ��
	
	// ���� ���� : �̹��� �� ��ü
	Quad* quad; // �� �̹����� ���� �޴°�? -> �����尡 �ƴ� ���� �־ (���� ������ �ʿ�)
				// = ������Ʈ�� ���̴��� ���� �ʴ´�

	// ��ƼŬ �� �ν��Ͻ��� ���Ϳ� ����
	vector<InstanceData> instances; // �ν��Ͻ���
	vector<ParticleInfo> particleInfo; // ������� �� �ν��Ͻ��� ���� ��

	// �ν��Ͻ��� ��� ���� �������μ� �����ϱ� ���� ����
	VertexBuffer* instanceBuffer;

	ParticleData data;

	float lifeSpan = 0; // ���� �ֱ�
	UINT drawCount = 0; // ��ο� �� Ƚ��
	UINT particleCount = 100;

	BlendState* blendState[2];
	DepthStencilState* depthState[2];
};

