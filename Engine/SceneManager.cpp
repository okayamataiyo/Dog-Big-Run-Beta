#include "SceneManager.h"
#include "Model.h"
#include "../PlayScene.h"
#include "../GameOverScene.h"

SceneManager::SceneManager(GameObject* parent)
	:GameObject(parent, "SceneManager")
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize()
{
	currentSceneID_ = SCENE_ID_PLAY;
	nextSceneID_ = currentSceneID_;
	Instantiate<PlayScene>(this);
}

void SceneManager::Update()
{

	//�V�[�������ۂɐ؂�ւ���
	//���݂̃V�[���ƁA�l�N�X�g�V�[�����ʂ�������؂�ւ�
	if (currentSceneID_ != nextSceneID_) {

		//���̃V�[���̃I�u�W�F�N�g��S�폜
		KillAllChildren();

		//���[�h�����f�[�^��S�폜
		Model::Release();
		//��Еt���I��
		switch (nextSceneID_) {

		case SCENE_ID_PLAY:	Instantiate<PlayScene>(this);	break;
		case SCENE_ID_GAMEOVER: Instantiate<GameOverScene>(this); break;
		}
		currentSceneID_ = nextSceneID_;
	}
	//���݂̃V�[���̌�Еt��
	//�V�����V�[���̏���
	//currentSceneID_��nextSCENEID
	
}

void SceneManager::Draw()
{
}

void SceneManager::Release()
{
}

/// <summary>
/// ���Ɏw�肵���V�[����nextSceneID_�ɐݒ肷��(=����Update�ŃV�[�����ς��)
/// </summary>
/// <param name="_next">���̃V�[��</param>
void SceneManager::ChangeScene(SCENE_ID _next)
{

	nextSceneID_ = _next;
}
