#include "Framework.h"

Spark::Spark(wstring imageFile, bool isAdditive)
{
	//material->SetShader(L"Basic/Texture.hlsl"); // 텍스처가 최우선일 때
	//material->SetShader(L"Effect/Particle.hlsl"); // 범용 셰이더
	material->SetShader(L"Geometry/Spark.hlsl"); // 지오메트리 셰이더 맞춤 스파크 셰이더

	material->SetDiffuseMap(imageFile);

	geometryShader = Shader::AddGS(L"Geometry/Spark.hlsl"); // 지오메트리 셰이더 설정

	buffer = new FloatValueBuffer();
	startColorBuffer = new ColorBuffer();
	endColorBuffer = new ColorBuffer();

	if (isAdditive) blendState[1]->Additive(); // 혼합 투명 설정일 경우 출력 상태에 반영
	
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

	buffer->Get()[0] += DELTA; // 소수 버퍼에 등록한 데이터 0번에 누적 경과 시간 더하기
	
	if (buffer->Get()[0] > buffer->Get()[1]) Stop(); // 소수 버퍼 0번과 1번을 비교해서 0번이 더 크면 이펙트 종료
													 // = 0번은 경과시간, 1번은 기준시간
}

void Spark::Render()
{
	// 각각 소수 버퍼로 설정된 데이터는 함수처럼 써서 진행

	buffer->SetVS(10);	// 각각 정해진 매개변수를 줘서 조건문을 셰이더 안에서 달성
	buffer->SetPS(10);	// * 왜 10을 매개변수로 전달하는가? : 정해진 이유는 없고, 해당 숫자가 무난하므로
						// 보통 0~2 정도는 화면 출력에서 쓰니까, 10 정도면 보통은 침범하지 않는다

	startColorBuffer->SetPS(11);
	endColorBuffer->SetPS(12);
						// 왜 해당 부분에선 11과 12를 전달하는가? -> 10으로 슬롯을 준 셰이더가 계산한 결과를 지금 쓰고 싶어서

	Particle::Render(); // 부모 함수 수행(=렌더)
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
