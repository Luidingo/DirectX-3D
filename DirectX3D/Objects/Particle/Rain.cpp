#include "Framework.h"

Rain::Rain()
{
	// 그림 세팅
	material->SetShader(L"Geometry/Rain.hlsl");
	material->SetDiffuseMap(L"Textures/Effect/Rain.png");

	// 셰이더 세팅
	geometryShader = Shader::AddGS(L"Geometry/Rain.hlsl");

	buffer = new WeatherBuffer(); // 클래스 안에 기본값이 있음

	Create();
}

Rain::~Rain()
{
	delete buffer;
}

void Rain::Update()
{
	buffer->Get().time += DELTA; // 경과시간만 누적
}

void Rain::Render()
{
	buffer->SetVS(10); // 날씨 조건 실행에 따른 옵션 조건 설정 + 셰이더 출력도 같이 설정
	Particle::Render(); // 파티클로서 렌더 실행 (= 빗방울 출력)
}

void Rain::GUIRender()
{
	// 날씨 옵션 설정
	ImGui::Text("Rain Effect Option");
	ImGui::DragFloat3("Velocity", (float*)&buffer->Get().velocity, 0.1f);
		// DragFloat3 : ImGui의 옵션 변경 함수. 매개변수는 데이터 위치, 드래그 한 틱당 변경 수치
	ImGui::DragFloat3("Origin", (float*)&buffer->Get().origin, 0.1f);
	ImGui::DragFloat3("Size", (float*)&buffer->Get().size, 0.1f);

	ImGui::SliderFloat("Distance", &buffer->Get().distance, 0.f, 500.f); // 어디까지 그려지는가

	ImGui::ColorEdit4("Color", (float*)&buffer->Get().color);
}

void Rain::Create()
{
	int count = 100000; // 원하는 개수 입력
	buffer->Get().velocity = { 0, -5, 0 }; // 속력이 필요하면 해당 코드에서 수정 (기본값 : -5만큼 아래로)

	particleCount = (count < MAX_COUNT) ? count : MAX_COUNT; // 부모(Particle)에서 정한 최대 범위는 넘지 못하도록 설정

	vertices.resize(particleCount);
	FOR(particleCount)
	{
		// 크기 설정
		Float2 size;
		size.x = Random(0.1f, 0.4f);
		size.y = Random(2.0f, 8.0f);
		
		// 위치 설정
		Float3 pos;
		pos.x = Random(-buffer->Get().size.x, buffer->Get().size.x); // 크기로 지정된 곳의 -+ 범위 내에서
		pos.y = Random(-buffer->Get().size.y, buffer->Get().size.y);
		pos.z = Random(-buffer->Get().size.z, buffer->Get().size.z);
						// 버퍼 내에서의 size = 파티클로서의 비가 가지는 크기
						// = 보는 사람 입장에서는 "비가 내리는 곳의 영역"
						// = 빗방울은 해당 영역 내의 어딘가에 있는 것

		// 도출된 수치를 정점 벡터에 반영
		vertices[i].pos = pos;
		vertices[i].uv = size; // UV라 적혀있지만, 실제 역할은 텍스처의 크기를 의미함
	}

	// 정점 버퍼 업데이트 (생성 중)
	vertexBuffer = new VertexBuffer(vertices.data(), sizeof(VertexUV), particleCount);
}
