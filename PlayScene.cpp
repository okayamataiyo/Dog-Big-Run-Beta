#include "Engine/Input.h"
#include "Engine/SceneManager.h"
#include "PlayScene.h"
#include "Stage.h"
#include "Item.h"

PlayScene::PlayScene(GameObject* _pParent)
	:GameObject(_pParent, "PlayScene")
{
	
}

void PlayScene::Initialize()
{
	Instantiate<Item>(this);
	Instantiate<Stage>(this);
	for (int i = 0u; i <= 1; i++)
	{
		pPlayer_[i] = Instantiate<Player>(this);
		camVec_[i].x = 0;
		camVec_[i].y = 5;
		camVec_[i].z = -10;
	}
	pPlayer_[0]->SetObjectName("PlayerFirst");
	pPlayer_[1]->SetObjectName("PlayerSeconds");
	pSky_ = Instantiate<Sky>(this);
	pSky_->SetObjectName("SkyFirst");
	pCamera_ = new Camera;
	XMFLOAT3 FPlayerPos = { 3,0,0 };
	XMFLOAT3 SPlayerPos = { -3,0,0 };
	XMFLOAT3 skyRot = { 0,150,90 };
	XMFLOAT3 skyPos = { 0, -100, 0 };
	pSky_->SetRotate(skyRot);
	pSky_->SetPosition(skyPos);
	pPlayer_[0]->SetPosition(FPlayerPos);
	pPlayer_[1]->SetPosition(SPlayerPos);

}

void PlayScene::Update()
{
	for (int i = 0u; i <= 1; i++)
	{
		XMMATRIX mRotY[2];
		mRotY[i] = XMMatrixRotationY(XMConvertToRadians(pPlayer_[i]->GetRotate().y));
		XMMATRIX mRotX[2];
		mRotX[i] = XMMatrixRotationX(XMConvertToRadians(pPlayer_[i]->GetRotate().x));
		XMVECTOR vPos[2];
		vPos[i] = pPlayer_[i]->GetVecPos();
		XMFLOAT3 mouse;
		mouse = Input::GetMouseMove();

		const float sensitivity = 400;
		static float RotationX[2] = {};
		static float RotationY[2] = {};
		static float vecLength[2] = {};
		static float prevLen[2] = {};
		static float prevRotX[2] = {};

		RotationY[i] = mouse.y;
		RotationX[i] = mouse.x;
		vecLength[i] -= (mouse.z) / 50;

		XMFLOAT3 rDir = { 0,0,1 };
		XMVECTOR Dir = XMLoadFloat3(&rDir);

		//Dir = Dir * (pPlayer_[i]->GetRotate().x + RotationX[i]) * (pPlayer_[i]->GetRotate().y + RotationY[i]);
		//Dir = Dir + (vecLength * 2);
		camVec_[i].y += RotationX[i] / sensitivity;
		camVec_[i].x += RotationY[i] / sensitivity;
		//camVec_[i].z += vecLength[i]  + 10;

		float sigmaRotY = camVec_[i].y;// +pPlayer_[i]->GetRotate().y;
		float sigmaRotX = -camVec_[i].x;// + EasingX[i]; +pPlayer_[i]->GetRotate().x;
		//vecLength[i] = camVec_[i].z;

		if (sigmaRotX > 0*(3.14/180))
		{
			sigmaRotX = 0;
			camVec_[i].x -= RotationY[i] / sensitivity;
		}
		if (sigmaRotX < -88*(3.14/180))
		{
			sigmaRotX = -87.9 * (3.14 / 180);
			camVec_[i].x -= RotationY[i] / sensitivity;
		}

		prevRotX[i] = sigmaRotX;

		XMMATRIX mxRotX = XMMatrixRotationX(sigmaRotX);
		XMMATRIX mxRotY = XMMatrixRotationY(sigmaRotY);

		XMMATRIX rot = mxRotX * mxRotY;
		XMFLOAT3 playerPos = pPlayer_[i]->GetPosition();
		
		Dir = XMVector3Transform(Dir, rot);
		Dir = XMVector3Normalize(Dir);
		Dir = Dir * (vecLength[i] + 10);
		Dir += XMLoadFloat3(&playerPos);
		//camVec_ = pPlayer_[j]->GetPosition();
		//camVec_.y += 5;
		//camVec_.z += -10;
		//auto a = pPlayer_[j]->GetPosition();

		pCamera_->SetPosition(Dir, i);
		pCamera_->SetTarget(pPlayer_[i]->GetPosition(), i);

		prevLen[i] = vecLength[i];
	}
}

void PlayScene::Draw()
{

}

void PlayScene::Release()
{
}
