#include "Framework.h"

ParticleSystem::ParticleSystem(string file)
{
}

// �ؽ�ó ���Ϸκ��� ���� ��ƼŬ �����ϱ�
ParticleSystem::ParticleSystem(wstring file)
{
	LoadData(file); // ���Ϸκ��� ��ƼŬ�� �����͸� ����

	// �ν��Ͻ� ���� ���� �غ�
	instanceBuffer = new VertexBuffer(instances.data(), sizeof(InstanceData), data.count);

	// ��� �غ�
	FOR(2) blendState[i] = new BlendState();
	FOR(2) depthState[i] = new DepthStencilState();

	blendState[1]->Alpha(true); // ���� �ϴ� ���� (ȥ�� ������ �ɼǿ� ���� ���� ����)
	depthState[1]->DepthWriteMask(D3D11_DEPTH_WRITE_MASK_ZERO); // �� ������
	//depthState[1]->DepthWriteMask(D3D11_DEPTH_WRITE_MASK_ALL);

	quad->SetActive(false); // �̹��� �ϴ��� ����
}

ParticleSystem::~ParticleSystem()
{
	delete quad;
	delete instanceBuffer;
	FOR(2) delete blendState[i];
	FOR(2) delete depthState[i];
}

void ParticleSystem::Update()
{
	// ��ƼŬ�� ������Ʈ�Ǵ� ����

	if (!quad->Active()) return; // �׸��� ��Ȱ��ȭ��� ����

	lifeSpan += DELTA; // ���� �ֱ� ����

	UpdatePhysical(); // ������ ��� = ���� ������Ʈ
	UpdateColor(); // ���� ������Ʈ
	quad->UpdateWorld(); // ������Ʈ ����� �̹����� �ݿ�

	if (lifeSpan >= data.duration) // ����� ���� �ֱⰡ ��ƼŬ���� ������ ���ӽð��� �ٴٸ���
	{
		if (data.isLoop) Init(); // �ݺ� �ɼ��� ���� ��� �����
		else Stop(); // �ݺ� �ɼ��� ���� ��� ���߱�
	}
}

void ParticleSystem::Render()
{
}

void ParticleSystem::GUIRender()
{
}

void ParticleSystem::Play(Vector3 pos, Vector3 rot)
{
}

void ParticleSystem::Stop()
{
}

void ParticleSystem::UpdatePhysical()
{
	// ���� ���� ������Ʈ

	drawCount = 0; // ��ο� �� Ƚ�� �ʱ�ȭ (= ��ƼŬ���ٴ� ������ Ȯ�ο�)

	// ��¥�� �Ʒ� �ݺ���
	FOR(data.count) // �� ���ڿ� �ڵ� �ݿ�
	{
		if (lifeSpan < particleInfo[i].startTime) continue;
				// ���� ���� �ֱⰡ ������ ������ ���� ���� �� = ������ ���� �� �� ��

		// �ֱ� ���� ���� ��ƼŬ�� ����
		particleInfo[i].velocity += particleInfo[i].accleration * DELTA; 
							// �ӷ� ���ؿ� ���ӷ°� �߰� ������ ���ؼ� �ӵ�(���� + ����)�� �����
		particleInfo[i].transform.Pos() += particleInfo[i].velocity
			* particleInfo[i].speed * DELTA; // �ӵ���, �ӷ°�, ����ð��� ��� �ݿ�, ��ġ ����

		particleInfo[i].transform.Rot().z += particleInfo[i].angularVelocity * DELTA;
								// �ð�/�ݽð� ȸ�� ���� ������ �ݿ�

		// ������ �ɼ��� ���� �־��� ���, �׸��� ������ ����� �ǵ���
		if (data.isBillboard)
		{
			particleInfo[i].transform.Rot().x = CAM->Rot().x; // ī�޶� ����� ����ȭ
			particleInfo[i].transform.Rot().y = CAM->Rot().y;
		}
		// * �� ���Ͱ� �ƴ� ī�޶� �����ΰ� : ���͸� ���� ī�޶�� ������ ���߱⺸��
		//   �̹����� ī�޶� �ٶ󺸰� �ȴ� -> ���� ���忡�� �� ���� 100%��� ������ ���

		// ��� �ð��� ���� ����
		float t = (lifeSpan - particleInfo[i].startTime) // ����� �ֱ�
			/ (data.duration - particleInfo[i].startTime); // ���� �ð�

		// ������ ���� ũ�Ⱑ Ȥ�� �޶����� �ϸ� ����
		particleInfo[i].transform.Scale() =
			Lerp(particleInfo[i].startScale, particleInfo[i].endScale, t);

		particleInfo[i].transform.UpdateWorld(); // ���ݱ��� ���� ��ġ, ȸ��, ũ�� ������Ʈ

		// -> ������� �����ϸ� �� �ν��Ͻ��� ���� �ִ� �ɼǿ� ����,
		//	  �ش� �ɼ��� �ð��� ���� �ݿ��� Ʈ������ ����� ���� ����ü �ȿ� ����

		// -> �� ����ü�� ������ �ν��Ͻ��� �ִ´�
		instances[drawCount++].transform = // �� ȣ���� ������ ��ο� �� Ƚ���� +1
			XMMatrixTranspose(particleInfo[i].transform.GetWorld()); // ������ ��Ÿ���� ��� �����͸� ����
	
		// * �ν��Ͻ��� ���� ����ϰ� ���� ����ü�� �ִ� �͵� �����ϰ�.
		//	 ��������δ� �� ���� �������̰�����, �������� ����� �� �صΰ�
		//	 ���� �ν��Ͻ��� �� ���� ��ġ�� �� �� ������ �������̴�
	}

	// ������� ���� ��ƼŬ�� ���� ������ �ν��Ͻ� ������ ������ ���ŵȴ�
	// ���� ���� �ν��Ͻ� ������ ���ۿ��� �ݿ�
	instanceBuffer->Update(instances.data(), drawCount);
											// ������Ʈ Ƚ���� ��ο� �� Ƚ����ŭ��
}

void ParticleSystem::UpdateColor()
{
	float t = lifeSpan / data.duration; // ������ �ϰ� ����, ���� �ð��� ������ �ϰ� �����
										// �� �Լ����� ������, ���⼭�� ����

	// ���� t�� Ȱ���ؼ� �� ���� ���� ����
	Float4 color;
	color.x = Lerp(data.startColor.x, data.endColor.x, t); // R
	color.y = Lerp(data.startColor.y, data.endColor.y, t); // G
	color.z = Lerp(data.startColor.z, data.endColor.z, t); // B
	color.w = Lerp(data.startColor.w, data.endColor.w, t); // A

	// ������ ������ ���忡 ����
	quad->GetMaterial()->GetData().diffuse = color;
						// GetData() : �̹��� �����Ϳ� �����ϱ� ���� �Լ�
						// diffuse : �̹��� �����Ϳ��� ����
}

void ParticleSystem::Init()
{
	// ��ƼŬ �����ϱ�

	// �ɼ� �߿� ȥ�� ������ ���� �־��� ���, ��� ���¿� �ݿ� �ʿ�
	if (data.isAdditive) blendState[1]->Additive(); // ȥ�� ���� ȣ��
	else blendState[1]->Alpha(true); // ���� ����ȭ (�߰��� �ɼ��� �ٲ� ���� �־ ���⼭ Ȯ��)

	// ��� ���� ����

	lifeSpan = 0; // ���� �ֱ�(�߿��� ������ ����� �ð�) : �Ҹ� ������ duration����
	drawCount = 0; // ��ο� �� Ƚ�� (0 �ʱ�ȭ)
	data.count = particleCount; // Ŭ�������� ��ƼŬ ���� �ɼ� �ٲ� ��� �޾ƿ���

	// data.count�� particleCount�� �޶��ٸ� ���⼭ �ʱ�ȭ
	instances.resize(data.count);
	particleInfo.resize(data.count);

	// ���� �ν��Ͻ��� �ɼ��� ���⼭ ���� (���� ��Ȳ)
	// * Ȥ�� �ٸ� ������ �ɼ��� �ٲ��� �� ���� �����Ƿ� �׷� ���� ������ ����

	for (ParticleInfo& info : particleInfo) // ���� �ν��Ͻ� ���� �������� ȣ���ؼ� ����
	{
		info.transform.Pos() = {}; // �ϴ� ���
		
		// �� ��ƼŬ�� ��ü �ɼ� ����
		info.velocity = Random(data.minVelocity, data.maxVelocity);
		info.accleration = Random(data.minAcceleration, data.maxAcceleration);
		info.angularVelocity = Random(data.minAngularVelocity, data.maxAngularVelocity);
		info.speed = Random(data.minSpeed, data.maxSpeed);
		info.startTime = Random(data.minStartTime, data.maxStartTime);
		info.startScale = Random(data.minStartScale, data.maxStartScale);
		info.endScale = Random(data.minEndScale, data.maxEndScale);

		// �ɼ� : �ӷ�(������)�� ���� �ӵ�(����1 + ����)�� �����Ѵٸ� �Ʒ� �ڵ����
		info.velocity.Normalize(); // = ����ȭ
	}
	// -> �ݺ����� ������ ��ƼŬ �ɼǿ� ���� �� �̹����� ���� ���� ����
}

void ParticleSystem::LoadData(string file)
{
}

void ParticleSystem::LoadData(wstring file)
{
	// �׸� ���Ͽ��� ���� �����
	quad = new Quad(Vector2(1, 1)); // xy ǥ�� ũ��� �簢������ �����

	// ���� ���忡 ��Ƽ������ ���� (�׸��� �ٷ� �� ���� ���� : �ٷ� ���� ���� ũ�Ⱑ �׸��� ����
	quad->GetMaterial()->SetDiffuseMap(file);
	quad->GetMaterial()->SetShader(L"Effect/Particle.hlsl"); // �⺻ ��ƼŬ ���̴�
															// (�� �⺻���� ������ �ؽ�ó ���̴� ���)

	// �ɼ����� �����Ǿ��� ������ ���߾ �ν��Ͻ��� ����(= ���� ���� �� ������� ����)
	//instances.resize(data.count); // ������ ���� �ɼ��� ����� ���
	instances.resize(particleCount);	// Ŭ������ ��� ������ ����� ���
										// ���� �Լ������� �� �� ��� data.count�� �� �� (��ƼŬ�μ��� �ɼ��� �Ἥ)

	// �ν��Ͻ��� ���� ���͵� �ʱ�ȭ
	//particleInfo.resize(data.count);
	particleInfo.resize(particleCount);

	// -> �׸� ���Ͽ��� ���� ���� + �ν��Ͻ� ���� + ��ƼŬ�� ����Ʈ �ɼ� �ε� �Ϸ�

}
