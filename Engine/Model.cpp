#include "Model.h"
#include "Direct3D.h"

struct ModelData {
	//FBX
	Fbx* pFbx_;
	//トランスフォーム
	Transform transform_;
	string filename_;	//ファイル名
	float nowFrame, animSpeed;
	int startFrame, endFrame;
	//▼初期化
	ModelData() :pFbx_(nullptr), nowFrame(0), startFrame(0), endFrame(0), animSpeed(0)
	{
	}

	void SetAnimFrame(int _start, int _end, float _speed)
	{
		nowFrame = (float)_start;
		startFrame = _start;
		endFrame = _end;
		animSpeed = _speed;
	}
};
//モデルのポインタをぶち込んでおくベクタ
vector<ModelData* >modelList;


int Model::Load(string _fileName)
{
	ModelData* pData;
	pData = new ModelData;
	pData->filename_ = _fileName;
	pData->pFbx_ = nullptr;

	//ファイルネームが同じだったら、読まん
	for (auto& e : modelList) {
		if (e->filename_ == _fileName) {
			pData->pFbx_ = e->pFbx_;
			break;
		}
	}


	if (pData->pFbx_ == nullptr) {
		pData->pFbx_ = new Fbx;
		pData->pFbx_->Load(_fileName);
	}

	modelList.push_back(pData);
	return(modelList.size() - 1);	//今呼んだモデルのモデル番号を返す

}

void Model::SetTransform(int _hModel, Transform _transform)
{
	//モデル番号は、modelListのインデックス
	modelList[_hModel]->transform_ = _transform;

}

void Model::Draw(int _hModel)
{
	if (_hModel < 0 || _hModel >= modelList.size() || modelList[_hModel] == nullptr)
	{
		return;
	}
	//▼アニメーションを進める
	modelList[_hModel]->nowFrame += modelList[_hModel]->animSpeed;

	//▼最後までアニメーションしたら戻す
	if (modelList[_hModel]->nowFrame > (float)modelList[_hModel]->endFrame)
	{
		modelList[_hModel]->nowFrame = (float)modelList[_hModel]->startFrame;
	}
	//モデル番号は、modelListのインデックス
	if (modelList[_hModel]->pFbx_)
	{
		modelList[_hModel]->pFbx_->Draw(modelList[_hModel]->transform_, (int)modelList[_hModel]->nowFrame);
	}
}

void Model::Release()
{
	bool isReffered = false;	//参照されてる?
	for (int i = 0; i < modelList.size(); i++) {
		for (int j = i + 1; j < modelList.size(); j++) {
			if (modelList[i]->pFbx_ == modelList[j]->pFbx_) {
				isReffered = true;
				break;
			}
		}
		if (isReffered == false) {
			SAFE_DELETE(modelList[i]->pFbx_);
		}
		SAFE_DELETE(modelList[i]);
	}
	modelList.clear();
	
}

void Model::RayCast(int _handle, RayCastData* _data)
{
	XMFLOAT3 target = Transform::Float3Add(_data->start, _data->dir);
	XMMATRIX matInv = XMMatrixInverse(nullptr, modelList[_handle]->transform_.GetWorldMatrix());
	XMVECTOR vecStart = XMVector3TransformCoord(XMLoadFloat3(&_data->start), matInv);
	XMVECTOR vecTarget = XMVector3TransformCoord(XMLoadFloat3(&target), matInv);
	XMVECTOR vecDir = vecTarget - vecStart;

	XMStoreFloat3(&_data->start, vecStart);
	XMStoreFloat3(&_data->dir, vecDir);

	modelList[_handle]->pFbx_->RayCast(_data);
}
