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

	// ������� �����ϸ� Fox.mesh, Fox.mats ���� ����
	// -> �� ���� ������ �ش� ������ �ٽ� ������ �� ���

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
