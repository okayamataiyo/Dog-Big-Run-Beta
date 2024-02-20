#pragma once
//�C���N���[�h
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <vector>
#include "Texture.h"
#include "Transform.h"

using namespace DirectX;
using std::vector;

/// <summary>
/// 2D�摜��\�����邽�߂̃N���X
/// </summary>
class Sprite
{
private:
	//�R���X�^���g�o�b�t�@�[
	struct CONSTANT_BUFFER
	{
		XMMATRIX world;
		XMMATRIX uvTrans;
		XMFLOAT4 color;
	};
	//1�̒��_�����i�[����\����
	struct VERTEX
	{
		XMFLOAT3 position;
		XMFLOAT3 uv;
	};
protected:
	UINT64 vertexNum_;				//���_��
	ID3D11Buffer* pVertexBuffer_;

	UINT64 indexNum;
	vector<int>index_;
	ID3D11Buffer* pIndexBuffer_;
	ID3D11Buffer* pConstantBuffer_;
	Texture* pTexture_;
public:
	Sprite();
	~Sprite();

	HRESULT Initialize();

	void Release();

	XMFLOAT2 GetTextureSize() { return pTexture_->GetSize(); }

private:
	virtual void InitVertexData();
	HRESULT CreateVertexBuffer();         //���_�o�b�t�@���쐬

	virtual void InitIndexData();        //�C���f�b�N�X��������
	HRESULT CreateIndexBuffer();         //�C���f�b�N�X�o�b�t�@���쐬

	HRESULT CreateConstantBuffer();      //�R���X�^���g�o�b�t�@�쐬

	HRESULT LoadTexture(string fileName);               //�e�N�X�`�������[�h
};

