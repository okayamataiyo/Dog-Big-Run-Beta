#include "Model.h"
#include "Direct3D.h"

struct ModelData {
	//FBX
	Fbx* pfbx_;
	//トランスフォーム
	Transform transform_;
	std::string filename_;	//ファイル名
};
	//モデルのポインタをぶち込んでおくベクタ
	std::vector<ModelData* >modelList;


int Model::Load(std::string _fileName)
{
	ModelData* pData;
	pData = new ModelData;
	pData->filename_ = _fileName;
	pData->pfbx_ = nullptr;

	//ファイルネームが同じだったら、読まん
	for (auto& e : modelList) {
		if (e->filename_ == _fileName) {
			pData->pfbx_ = e->pfbx_;
			break;
		}
	}


	if (pData->pfbx_ == nullptr) {
		pData->pfbx_ = new Fbx;
		pData->pfbx_->Load(_fileName);
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
	//モデル番号は、modelListのインデックス
	modelList[_hModel]->pfbx_->Draw(modelList[_hModel]->transform_);
//	Transform& trf = modelList
}

void Model::Release()
{
	bool isReffered = false;	//参照されてる?
	for (int i = 0; i < modelList.size(); i++) {
		for (int j = i + 1; j < modelList.size(); j++) {
			if (modelList[i]->pfbx_ == modelList[j]->pfbx_) {
				isReffered = true;
				break;
			}
		}
		if (isReffered == false) {
			SAFE_DELETE(modelList[i]->pfbx_);
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

	modelList[_handle]->pfbx_->RayCast(_data);
}
