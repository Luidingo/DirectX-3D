#include "Framework.h"
#include "ModelExportScene.h"

ModelExportScene::ModelExportScene()
{
	string modelName = "Fox"; // 모델의 이름
	string filePath = "Models/FBX/" + modelName + ".fbx";

	ModelExporter* exporter = new ModelExporter(modelName, filePath);
	exporter->ExportMaterial(); // FBX의 실제 픽셀 정보를 모조리 읽어오기 때문에, 원본 안에 그림이 없으면 흰 픽셀이 나옴
								// 따라서 별도의 텍스처 파일이 존재하는 모델이라면, 매티리얼 추출은 하지 않는 편이 좋음
								// 반대로 텍스처가 따로 없고 모든 데이터가 있다면 추출을 할 것
								// 결국 원형에 대한 사전 조사(뷰어 열기)와 파일 백업이 필요하다
	exporter->ExportMesh();
	delete exporter;

	// 여기까지 실행하면 Fox.mesh, Fox.mats 파일 저장
	// -> 모델 렌더 씬에서 해당 파일을 다시 읽으면 모델 출력


	// 샘플 코드

	string clipName;
	clipName = "Fox_Run";
	string clipPath;
	clipPath = "Models/Animations/" + modelName + "/" + clipName + ".fbx";

	exporter = new ModelExporter(modelName, clipPath);
	exporter->ExportClip(clipName);
	delete exporter;
}

ModelExportScene::~ModelExportScene()
{
}

void ModelExportScene::Update()
{
}

void ModelExportScene::PreRender()
{
}

void ModelExportScene::Render()
{
}

void ModelExportScene::PostRender()
{
}

void ModelExportScene::GUIRender()
{
}
