#include "Framework.h"
#include "ModelExportScene.h"

ModelExportScene::ModelExportScene()
{
	string modelName = "Fox"; // ���� �̸�
	string filePath = "Models/FBX/" + modelName + ".fbx";

	ModelExporter* exporter = new ModelExporter(modelName, filePath);
	exporter->ExportMaterial(); // FBX�� ���� �ȼ� ������ ������ �о���� ������, ���� �ȿ� �׸��� ������ �� �ȼ��� ����
								// ���� ������ �ؽ�ó ������ �����ϴ� ���̶��, ��Ƽ���� ������ ���� �ʴ� ���� ����
								// �ݴ�� �ؽ�ó�� ���� ���� ��� �����Ͱ� �ִٸ� ������ �� ��
								// �ᱹ ������ ���� ���� ����(��� ����)�� ���� ����� �ʿ��ϴ�
	exporter->ExportMesh();
	delete exporter;

	// ������� �����ϸ� Fox.mesh, Fox.mats ���� ����
	// -> �� ���� ������ �ش� ������ �ٽ� ������ �� ���


	// ���� �ڵ�

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
