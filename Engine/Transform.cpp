#include "Transform.h"

Transform::Transform(): pParent_(nullptr),
    matTranslate_(XMMatrixIdentity()),
    matRotate_(XMMatrixIdentity()),
    matScale_(XMMatrixIdentity()),
    position_(XMFLOAT3(0, 0, 0)),
    rotate_(XMFLOAT3(0, 0, 0)),
    scale_(XMFLOAT3(1, 1, 1))
{
}

Transform::~Transform()
{
}

void Transform::Calclation()
{

    //移動行列
    matTranslate_ = XMMatrixTranslation(position_.x, position_.y, position_.z);

    //回転行列作成
    XMMATRIX rotateX, rotateY, rotateZ;
    rotateX = XMMatrixRotationX(XMConvertToRadians(rotate_.x));
    rotateY = XMMatrixRotationY(XMConvertToRadians(rotate_.y));
    rotateZ = XMMatrixRotationZ(XMConvertToRadians(rotate_.z));
    //rotateZが最初だったのを最後にした。
    matRotate_ = rotateY * rotateX * rotateZ;

    //拡大行列作成
    matScale_ = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
}

XMMATRIX Transform::GetWorldMatrix()
{
    Calclation();
    if (pParent_ != nullptr) {

        //親があったら親のWorldMatrixをかける
        return  matScale_ * matRotate_ * matTranslate_ * pParent_->GetWorldMatrix();
    }
    else {  

        //親がnullptrの時は、子の変換だけ
        return matScale_ * matRotate_ * matTranslate_;
    }

}

XMMATRIX Transform::GetNormalMatrix()
{
    return matRotate_ * XMMatrixInverse(nullptr, matScale_);
}