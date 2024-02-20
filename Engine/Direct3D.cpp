#include <d3dcompiler.h>
#include <cassert>
#include <vector>
#include "Direct3D.h"
#include "Camera.h"

//変数
namespace Direct3D
{
	ID3D11Device* pDevice_					   = nullptr;	//デバイス
	ID3D11DeviceContext* pContext_			   = nullptr;	//デバイスコンテキスト
	IDXGISwapChain* pSwapChain_				   = nullptr;	//スワップチェイン
	ID3D11RenderTargetView* pRenderTargetView_ = nullptr;	//レンダーターゲットビュー

	ID3D11Texture2D* pDepthStencil;							//深度ステンシル
	ID3D11DepthStencilView* pDepthStencilView;				//深度ステンシルビュー

	ID3D11VertexShader* pVertexShader_		   = nullptr;	//頂点シェーダー
	ID3D11PixelShader* pPixelShader_		   = nullptr;	//ピクセルシェーダー
	ID3D11InputLayout* pVertexLayout_		   = nullptr;	//頂点インプットレイアウト
	ID3D11RasterizerState* pRasterizerState_   = nullptr;	//ラスタライザー
	D3D11_VIEWPORT vp[2];									//分割ビューポート、これをモデルの描画前に設定する
	struct SHADER_BUNDLE
	{
		ID3D11VertexShader* pVertexShader_	   = nullptr;	//頂点シェーダー
		ID3D11PixelShader* pPixelShader_	   = nullptr;	//ピクセルシェーダー
		ID3D11InputLayout* pVertexLayout_	   = nullptr;	//頂点インプットレイアウト
		ID3D11RasterizerState* pRasterizerState_ = nullptr;	//ラスタライザー
	};

	bool isDrawCollision_ = true;							//コリジョンを表示するか
	int isChangeView_ = 0;
	float vPSize_[2] = { 2,2 };
	float vPMove_[2] = { 0,0 };
	float prevVP_ = 0;
	int widthHaif_ = 0;
	int width_ = 0;
	int height_ = 0;
	bool isFinishView_ = false;
	SHADER_BUNDLE shaderBundle[SHADER_MAX] = {};
}

//初期化
HRESULT Direct3D::Initialize(int winW, int winH, HWND hWnd)
{
	widthHaif_ = winW / 2;
	width_ = winW;
	height_ = winH;
	prevVP_ = vp[1].Width;
	HRESULT hr;
	///////////////////////////いろいろ準備するための設定///////////////////////////////
	//いろいろな設定項目をまとめた構造体
	DXGI_SWAP_CHAIN_DESC scDesc;

	//とりあえず全部0
	ZeroMemory(&scDesc, sizeof(scDesc));

	//描画先のフォーマット
	scDesc.BufferDesc.Width = winW;		//画面幅
	scDesc.BufferDesc.Height = winH;	//画面高さ
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 何色使えるか

	//FPS（1/60秒に1回）
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;

	//その他
	scDesc.Windowed = TRUE;			//ウィンドウモードかフルスクリーンか
	scDesc.OutputWindow = hWnd;		//ウィンドウハンドル
	scDesc.BufferCount = 1;			//バックバッファの枚数
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//バックバッファの使い道＝画面に描画するために
	scDesc.SampleDesc.Count = 1;		//MSAA（アンチエイリアス）の設定
	scDesc.SampleDesc.Quality = 0;		//　〃
	////////////////上記設定をもとにデバイス、コンテキスト、スワップチェインを作成////////////////////////
	D3D_FEATURE_LEVEL level;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,						// どのビデオアダプタを使用するか？既定ならばnullptrで
		D3D_DRIVER_TYPE_HARDWARE,		// ドライバのタイプを渡す。ふつうはHARDWARE
		nullptr,						// 上記をD3D_DRIVER_TYPE_SOFTWAREに設定しないかぎりnullptr
		0,								// 何らかのフラグを指定する。（デバッグ時はD3D11_CREATE_DEVICE_DEBUG？）
		nullptr,						// デバイス、コンテキストのレベルを設定。nullptrにしとけばOK
		0,								// 上の引数でレベルを何個指定したか
		D3D11_SDK_VERSION,				// SDKのバージョン。必ずこの値
		&scDesc,						// 上でいろいろ設定した構造体
		&pSwapChain_,					// 無事完成したSwapChainのアドレスが返ってくる
		&pDevice_,						// 無事完成したDeviceアドレスが返ってくる
		&level,							// 無事完成したDevice、Contextのレベルが返ってくる
		&pContext_);					// 無事完成したContextのアドレスが返ってくる
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "デバイス、コンテキスト、スワップチェインの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}
	///////////////////////////レンダーターゲットビュー作成///////////////////////////////
	//スワップチェーンからバックバッファを取得（バックバッファ ＝ レンダーターゲット）
	ID3D11Texture2D* pBackBuffer;
	hr = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "バックバッファの取得に失敗しました", "エラー", MB_OK);
		return hr;
	}

	//レンダーターゲットビューを作成
	hr = pDevice_->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView_);
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "レンダーターゲットビューの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}
	//一時的にバックバッファを取得しただけなので解放
	SAFE_RELEASE(pBackBuffer);

	///////////////////////////ビューポート（描画範囲）設定///////////////////////////////
	
	//深度ステンシルビューの作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = winW;
	descDepth.Height = winH;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	pDevice_->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	pDevice_->CreateDepthStencilView(pDepthStencil, NULL, &pDepthStencilView);

	//データを画面に描画するための一通りの設定（パイプライン）
	pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		// データの入力種類を指定
	pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView);    // 描画先を

	isDrawCollision_ = GetPrivateProfileInt("DEBUG", "ViewCollider", 0, ".\\Assets/setup.ini") != 0;
	//シェーダー準備
	hr = InitShader();
	if (FAILED(hr))
	{
		//エラー処理
		return hr;
	}

	return S_OK;
}

//シェーダー準備
HRESULT Direct3D::InitShader()
{
	if (FAILED(InitShader3D()))
	{
		return E_FAIL;
	}

	if (FAILED(InitShader2D()))
	{
		return E_FAIL;
	}

	if (FAILED(InitShaderToon()))
	{
		return E_FAIL;
	}

	if (FAILED(InitShaderToonOutLine()))
	{
		return E_FAIL;
	}

	if (FAILED(InitShaderUNLIT()))
	{
		return E_FAIL;
	}
	Direct3D::SetShader(SHADER_3D);
	return S_OK;
}

HRESULT Direct3D::InitShader3D()
{
	HRESULT hr;
	using namespace Direct3D;
	// 頂点シェーダの作成（コンパイル）
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Assets/Shader/Simple3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr); //ここはassertionで処理

	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(),
		NULL, &(shaderBundle[SHADER_3D].pVertexShader_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "頂点シェーダーの作成に失敗しました", "エラー", MB_OK);
		//解放処理
		return hr;
	}
	//ID3DBlob* pCompileVS = nullptr;
	//D3DCompileFromFile(L"Simple3Dr.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	//assert(pCompileVS != nullptr);
	////pCompileVS = nullptr;
	//hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &pVertexShader_);
	//if (FAILED(hr))
	//{
	//	//エラー処理
	//	return hr;
	//}

	//頂点インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//法線
	};
	hr = pDevice_->CreateInputLayout(layout, 3, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(),
		&(shaderBundle[SHADER_3D].pVertexLayout_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		//解放処理
		SAFE_RELEASE(pCompileVS);
		return hr;
	}
	SAFE_RELEASE(pCompileVS);

	//D3D11_INPUT_ELEMENT_DESC layout[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
	//	{ "TEXCOORDE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 },//法線
	//};
	//

	//hr = pDevice_->CreateInputLayout(layout, 3, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &pVertexLayout_);
	//if (FAILED(hr))
	//{
	//	//エラー処理
	//	return hr;
	//}

	//SAFE_RELEASE(pCompileVS);

	//ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Assets/Shader/Simple3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);

	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shaderBundle[SHADER_3D].pPixelShader_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		SAFE_RELEASE(pCompilePS);
		return hr;
	}

	SAFE_RELEASE(pCompilePS);
	//ID3DBlob* pCompilePS = nullptr;
	//D3DCompileFromFile(L"Simple3Dr.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	//assert(pCompilePS != nullptr);
	//hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &pPixelShader_);
	//if (FAILED(hr))
	//{
	//	//エラー処理
	//	return hr;
	//}

	//SAFE_RELEASE(pCompilePS);

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK;
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = FALSE;
	rdc.ScissorEnable = false;
	rdc.MultisampleEnable = false;
	hr = pDevice_->CreateRasterizerState(&rdc, &(shaderBundle[SHADER_3D].pRasterizerState_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	return S_OK;
}

HRESULT Direct3D::InitShader2D()
{
	HRESULT hr;

	//こっから2Dのやつ
	// 頂点シェーダの作成（コンパイル）
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Assets/Shader/Simple2D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr);
	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &(shaderBundle[SHADER_2D].pVertexShader_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "頂点シェーダーの作成に失敗しました", "エラー", MB_OK);
		SAFE_RELEASE(pCompileVS);
		//解放処理
		return hr;
	}

	//頂点インプットレイアウト
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
	};
	hr = pDevice_->CreateInputLayout(layout.data(), layout.size(),
		pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &(shaderBundle[SHADER_2D].pVertexLayout_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		//解放処理
		SAFE_RELEASE(pCompileVS);
		return hr;
	}
	SAFE_RELEASE(pCompileVS);

	// ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Assets/Shader/Simple2D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);

	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shaderBundle[SHADER_2D].pPixelShader_));
	SAFE_RELEASE(pCompilePS);
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		//SAFE_RELEASE(pCompilePS);
		return hr;
	}
	//SAFE_RELEASE(pCompilePS);
	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK;
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = FALSE;
	hr = pDevice_->CreateRasterizerState(&rdc, &(shaderBundle[SHADER_2D].pRasterizerState_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	return S_OK;
}

HRESULT Direct3D::InitShaderToon()
{
	HRESULT hr;
	using namespace Direct3D;
	// 頂点シェーダの作成（コンパイル）
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Assets/Shader/Toon.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr); //ここはassertionで処理

	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(),
		NULL, &(shaderBundle[SHADER_TOON].pVertexShader_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "頂点シェーダーの作成に失敗しました", "エラー", MB_OK);
		//解放処理
		return hr;
	}

	//頂点インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//法線
	};
	hr = pDevice_->CreateInputLayout(layout, 3, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(),
		&(shaderBundle[SHADER_TOON].pVertexLayout_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		//解放処理
		SAFE_RELEASE(pCompileVS);
		return hr;
	}
	SAFE_RELEASE(pCompileVS);

	//ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Assets/Shader/Toon.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);

	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shaderBundle[SHADER_TOON].pPixelShader_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		SAFE_RELEASE(pCompilePS);
		return hr;
	}
	SAFE_RELEASE(pCompilePS);

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK;
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = FALSE;
	rdc.ScissorEnable = false;
	rdc.MultisampleEnable = false;
	hr = pDevice_->CreateRasterizerState(&rdc, &(shaderBundle[SHADER_TOON].pRasterizerState_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}

	return S_OK;
}

HRESULT Direct3D::InitShaderToonOutLine()
{
	HRESULT hr;
	using namespace Direct3D;
	// 頂点シェーダの作成（コンパイル）
	ID3DBlob* pCompileVS = nullptr;
	D3DCompileFromFile(L"Assets/Shader/ToonOutLine.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	assert(pCompileVS != nullptr); //ここはassertionで処理

	hr = pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(),
		NULL, &(shaderBundle[SHADER_TOONOUTLINE].pVertexShader_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "頂点シェーダーの作成に失敗しました", "エラー", MB_OK);
		//解放処理
		return hr;
	}

	//頂点インプットレイアウト
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMVECTOR) , D3D11_INPUT_PER_VERTEX_DATA, 0 },//UV座標
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },//法線
	};
	hr = pDevice_->CreateInputLayout(layout, 3, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(),
		&(shaderBundle[SHADER_TOONOUTLINE].pVertexLayout_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
		//解放処理
		SAFE_RELEASE(pCompileVS);
		return hr;
	}
	SAFE_RELEASE(pCompileVS);

	//ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = nullptr;
	D3DCompileFromFile(L"Assets/Shader/ToonOutLine.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	assert(pCompilePS != nullptr);

	hr = pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &(shaderBundle[SHADER_TOONOUTLINE].pPixelShader_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
		SAFE_RELEASE(pCompilePS);
		return hr;
	}
	SAFE_RELEASE(pCompilePS);

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_FRONT;
	rdc.FillMode = D3D11_FILL_SOLID;
	rdc.FrontCounterClockwise = FALSE;
	rdc.ScissorEnable = false;
	rdc.MultisampleEnable = false;
	hr = pDevice_->CreateRasterizerState(&rdc, &(shaderBundle[SHADER_TOONOUTLINE].pRasterizerState_));
	if (FAILED(hr))
	{
		//エラー処理
		MessageBox(NULL, "ラスタライザの作成に失敗しました", "エラー", MB_OK);
		return hr;
	}
	return S_OK;
}

HRESULT Direct3D::InitShaderUNLIT()
{
	// 頂点シェーダの作成（コンパイル）
	ID3DBlob* pCompileVS = NULL;
	D3DCompileFromFile(L"Assets/Shader/Debug3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
	pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle[SHADER_UNLIT].pVertexShader_);

	// ピクセルシェーダの作成（コンパイル）
	ID3DBlob* pCompilePS = NULL;
	D3DCompileFromFile(L"Assets/Shader/Debug3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
	pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle[SHADER_UNLIT].pPixelShader_);

	// 頂点レイアウトの作成（1頂点の情報が何のデータをどんな順番で持っているか）
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	pDevice_->CreateInputLayout(layout, 1, pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle[SHADER_UNLIT].pVertexLayout_);

	//シェーダーが無事作成できたので、コンパイルしたやつはいらない
	pCompileVS->Release();
	pCompilePS->Release();

	//ラスタライザ作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_NONE;
	rdc.FillMode = D3D11_FILL_WIREFRAME;
	rdc.FrontCounterClockwise = TRUE;
	pDevice_->CreateRasterizerState(&rdc, &shaderBundle[SHADER_UNLIT].pRasterizerState_);

	return S_OK;
}

void Direct3D::SetShader(SHADER_TYPE type)
{
	//それぞれをデバイスコンテキストにセット
	pContext_->VSSetShader(shaderBundle[type].pVertexShader_, NULL, 0);	//頂点シェーダー
	pContext_->PSSetShader(shaderBundle[type].pPixelShader_, NULL, 0);	//ピクセルシェーダー
	pContext_->IASetInputLayout(shaderBundle[type].pVertexLayout_);	//頂点インプットレイアウト
	pContext_->RSSetState(shaderBundle[type].pRasterizerState_);		//ラスタライザー
}

void Direct3D::Update()
{
	//レンダリング結果を表示する範囲
	vp[0].Width = widthHaif_ + vPSize_[0];
	vp[0].Height = height_;
	vp[0].TopLeftX = 0;			 //画面左上のx座標
	vp[0].TopLeftY = 0;			 //画面左上のy座標
	vp[0].MinDepth = 0.0f;		 //深度値の最小値
	vp[0].MaxDepth = 1.0f;		 //深度値の最大値
	vp[1].Width = widthHaif_;
	vp[1].Height = height_;
	vp[1].TopLeftX = widthHaif_ + vPSize_[1];	 //画面左上のx座標
	vp[1].TopLeftY = 0;			 //画面左上のy座標
	vp[1].MinDepth = 0.0f;		 //深度値の最小値
	vp[1].MaxDepth = 1.0f;		 //深度値の最大値

	switch (isChangeView_)
	{
	case 0:
		prevVP_ = vp[1].Width;
		break;
	case 1:
		if (vPSize_[0] <= widthHaif_)
		{
			vPSize_[0] += 10;
			vPSize_[1] += 10;
		}
		break;
	case 2:
		if(vPSize_[0] >= 10)
		{
			vPSize_[0] -= 10;
			vPSize_[1] -= 10;
		}
		break;
	default:
		break;
	}
}

//描画開始
void Direct3D::BeginDraw()
{
	//背景の色
	float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };//R,G,B,A
	//画面をクリア
	pContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);
	//深度バッファクリア
	pContext_->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

//描画終了
void Direct3D::EndDraw()
{
	//スワップ（バックバッファを表に表示する）
	pSwapChain_->Present(0, 0);
}

//解放処理
void Direct3D::Release()
{
	//解放処理
	for (int i = 0; i < SHADER_MAX; i++)
	{
		SAFE_RELEASE(shaderBundle[i].pRasterizerState_);
		SAFE_RELEASE(shaderBundle[i].pVertexLayout_);
		SAFE_RELEASE(shaderBundle[i].pPixelShader_);
		SAFE_RELEASE(shaderBundle[i].pVertexShader_);
	}
	SAFE_RELEASE(pRenderTargetView_);
	SAFE_RELEASE(pSwapChain_);
	SAFE_RELEASE(pContext_);
	SAFE_RELEASE(pDevice_);
}

bool Direct3D::Intersect(XMFLOAT3& _start, XMFLOAT3& _direction, XMFLOAT3& _v0, XMFLOAT3& _v1, XMFLOAT3& _v2, float* _distance)
{
	//微小な定数([M?ller97]での値)
	constexpr float kEpsilon = 1e-6f;

	//三角形の２辺
	XMVECTOR edge1 = XMVectorSet(_v1.x - _v0.x, _v1.y - _v0.y, _v1.z - _v0.z, 0.0f);
	XMVECTOR edge2 = XMVectorSet(_v2.x - _v0.x, _v2.y - _v0.y, _v2.z - _v0.z, 0.0f);

	XMVECTOR alpha = XMVector3Cross(XMLoadFloat3(&_direction), edge2);
	float det = XMVector3Dot(edge1, alpha).m128_f32[0];

	// 三角形に対して、レイが平行に入射するような場合 det = 0 となる。
	// det が小さすぎると 1/det が大きくなりすぎて数値的に不安定になるので
	// det ? 0 の場合は交差しないこととする。
	if (-kEpsilon < det && det < kEpsilon)
	{
		return false;
	}

	float invDet = 1.0f / det;
	XMFLOAT3 r = XMFLOAT3(_start.x - _v0.x, _start.y - _v0.y, _start.z - _v0.z);

	// u が 0 <= u <= 1 を満たしているかを調べる。
	float u = XMVector3Dot(alpha, XMLoadFloat3(&r)).m128_f32[0] * invDet;
	if (u < 0.0f || u > 1.0f)
	{
		return false;
	}

	XMVECTOR beta = XMVector3Cross(XMLoadFloat3(&r), edge1);

	// v が 0 <= v <= 1 かつ u + v <= 1 を満たすことを調べる。
	// すなわち、v が 0 <= v <= 1 - u をみたしているかを調べればOK。
	float v = XMVector3Dot(XMLoadFloat3(&_direction), beta).m128_f32[0] * invDet;
	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

	// t が 0 <= t を満たすことを調べる。
	float t = XMVector3Dot(edge2, beta).m128_f32[0] * invDet;
	if (t < 0.0f)
	{
		return false;
	}

	*_distance = t;
	return true;
}

void Direct3D::SetViewPort(int VpType)
{
	uint8_t cnt = sizeof(vp) / sizeof(vp[0]);

	pContext_->RSSetViewports(1, &vp[VpType]);
}

void Direct3D::SetIsChangeView(int _IsChangeView)
{
	isChangeView_ = _IsChangeView;
}
