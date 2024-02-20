#include "Sprite.h"
#include "Direct3D.h"
#include "../Global.h"

Sprite::Sprite() :
	pTexture_(nullptr)
{

}

Sprite::~Sprite()
{
	Release();
}

HRESULT Sprite::Initialize()
{
	//頂点情報
	InitVertexData();
	if (FAILED(CreateVertexBuffer()))
	{
		return E_FAIL;
	}
	InitIndexData();
	if (FAILED(CreateIndexBuffer()))
	{
		return E_FAIL;
	}
	if (FAILED(CreateConstantBuffer()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void Sprite::Release()
{
	SAFE_RELEASE(pVertexBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
}

void Sprite::InitVertexData()
{
}

HRESULT Sprite::CreateVertexBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * vertexNum_;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	//data_vertex.pSysMem = vertices_.data();
	hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}
	return S_OK;
}

void Sprite::InitIndexData()
{
	index_ = { 0,2,3, 0,1,2 };
	//インデックス数
	indexNum = index_.size();
	//メンバ変数へコピー
	//index_ = new int[indexNum];
	//memcpy(index_, index, sizeof(index));
}

HRESULT Sprite::CreateIndexBuffer()
{
	D3D11_BUFFER_DESC   bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * indexNum;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = index_.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &pIndexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}
	return S_OK;
}

HRESULT Sprite::CreateConstantBuffer()
{
	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	HRESULT hr;

	hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	return S_OK;
}

HRESULT Sprite::LoadTexture(string fileName)
{
	pTexture_ = new Texture();

	HRESULT hr;
	pTexture_->Load(fileName);

	return S_OK;
}

