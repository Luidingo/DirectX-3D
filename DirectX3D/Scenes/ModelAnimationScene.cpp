#include "Framework.h"
#include "ModelAnimationScene.h"

ModelAnimationScene::ModelAnimationScene()
{
	//model = new ModelAnimator("Fox");
	//model->ReadClip("Fox_Run");

	//model = new ModelAnimator("Human"); 
	//model->ReadClip("Idle");
	//model->ReadClip("Run");
	//model->ReadClip("Jump");

	// 위 코드로 읽으면 모델이 움직이지 않음
	// 이유 : 인간 .fbx 파일에 동작이 0번이 아닌 1번에 들어가기 때문에
	//		  (동작이 이중 목록으로 들어가서 0-1, 1-1...등으로 구성돼 있다)
	//		  이런 식으로 동작이 복잡한 모델을 읽을 때에는 (뷰어에서 확인 필요) 뒤에 동작 수를
	//		  매개변수로 더해주어야 함
	// 
	//model->ReadClip("Idle", 1); // 동작이 0 뒤에 1이 있음
	//model->ReadClip("Run", 1);
	//model->ReadClip("Jump", 1);

	//------------------------------------------------------------

	model = new ModelAnimator("Naruto");
	model->ReadClip("Idle");
	model->ReadClip("Run");
	model->ReadClip("Throw");
}

ModelAnimationScene::~ModelAnimationScene()
{
	delete model;
}

void ModelAnimationScene::Update()
{
	model->Update();
}

void ModelAnimationScene::PreRender()
{
}

void ModelAnimationScene::Render()
{
	model->Render();
}

void ModelAnimationScene::PostRender()
{
}

void ModelAnimationScene::GUIRender()
{
	model->GUIRender();
}
