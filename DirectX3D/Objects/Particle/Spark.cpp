#include "Framework.h"

Spark::Spark(wstring imageFile, bool isAdditive)
{
	//material->SetShader(L"Basic/Texture.hlsl"); // �ؽ�ó�� �ֿ켱�� ��
	//material->SetShader(L"Effect/Particle.hlsl"); // ���� ���̴�
	material->SetShader(L"Geometry/Spark.hlsl"); // ������Ʈ�� ���̴� ���� ����ũ ���̴�

	material->SetDiffuseMap(imageFile);

	geometryShader = Shader::AddGS(L"Geometry/Spark.hlsl"); // ������Ʈ�� ���̴� ����

	buffer = new FloatValueBuffer();
	startColorBuffer = new ColorBuffer();
	endColorBuffer = new ColorBuffer();

	if (isAdditive) blendState[1]->Additive(); // ȥ�� ���� ������ ��� ��� ���¿� �ݿ�
	
	Create();
}

Spark::~Spark()
{
	delete buffer;
	delete startColorBuffer;
	delete endColorBuffer;
}

void Spark::Update()
{
	if (!isActive) return;

	buffer->Get()[0] += DELTA; // �Ҽ� ���ۿ� ����� ������ 0���� ���� ��� �ð� ���ϱ�
	
	if (buffer->Get()[0] > buffer->Get()[1]) Stop(); // �Ҽ� ���� 0���� 1���� ���ؼ� 0���� �� ũ�� ����Ʈ ����
													 // = 0���� ����ð�, 1���� ���ؽð�
}

void Spark::Render()
{
	// ���� �Ҽ� ���۷� ������ �����ʹ� �Լ�ó�� �Ἥ ����

	buffer->SetVS(10);	// ���� ������ �Ű������� �༭ ���ǹ��� ���̴� �ȿ��� �޼�
	buffer->SetPS(10);	// * �� 10�� �Ű������� �����ϴ°�? : ������ ������ ����, �ش� ���ڰ� �����ϹǷ�
						// ���� 0~2 ������ ȭ�� ��¿��� ���ϱ�, 10 ������ ������ ħ������ �ʴ´�

	startColorBuffer->SetPS(11);
	endColorBuffer->SetPS(12);
						// �� �ش� �κп��� 11�� 12�� �����ϴ°�? -> 10���� ������ �� ���̴��� ����� ����� ���� ���� �;

	Particle::Render(); // �θ� �Լ� ����(=����)
}

void Spark::GUIRender()
{
}

void Spark::Play(Vector3 pos)
{
}

void Spark::Create()
{
}

void Spark::UpdateParticle()
{
}
