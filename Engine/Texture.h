#pragma once
#include<d3d11.h>
#include<string>
#include <wincodec.h>
#include<wrl.h>
#include<DirectXMath.h>

#pragma comment( lib, "WindowsCodecs.lib" )

using namespace DirectX;
using namespace Microsoft::WRL;
using std::string;

class Texture
{
	ID3D11SamplerState* pSampler_;
	ID3D11ShaderResourceView* pSRV_;
	XMFLOAT2 imgSize_;
public:
	Texture();
	~Texture();
	HRESULT Load(string _fileName);
	void Release();
	ID3D11SamplerState* GetSampler() { return pSampler_; }
	ID3D11ShaderResourceView* GetSRV() { return pSRV_; }
	XMFLOAT2 GetSize() { return imgSize_; }				//‰æ‘œƒTƒCƒY‚ÌŽæ“¾
};

