//インクルード
#include <Windows.h>
#include <stdlib.h>
#include "Engine/ImGui/imgui.h"
#include "Engine/ImGui/imgui_impl_dx11.h"
#include "Engine/ImGui/imgui_impl_win32.h"

#include "Engine/Direct3D.h"
#include "Engine/Camera.h"
#include "Engine/Input.h"
#include "Engine/Rootjob.h"
#include "Engine/Model.h"
#include "Player.h"

//リンカ
#pragma comment(lib, "winmm.lib")

//定数宣言
const char* WIN_CLASS_NAME = "Dog Big Run";  //ウィンドウクラス名

Rootjob* pRootjob = nullptr;
Camera* pCamera = new Camera;


//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//エントリーポイント
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr;

	//初期化ファイル(setup.ini)から必要な情報を取得
	int screenWidth = GetPrivateProfileInt("SCREEN", "Width", 800, ".\\Assets\\setup.ini");
	int screenHeight = GetPrivateProfileInt("SCREEN", "Height", 600, ".\\Assets\\setup.ini");
	int fpsLimit = GetPrivateProfileInt("GAME", "Fps", 60, ".\\Assets\\setup.ini");
	int isDrawFps = GetPrivateProfileInt("DEBUG", "ViewFps", 0, ".\\Assets\\setup.ini");
	char caption[64];
	GetPrivateProfileString("SCREEN", "Caption", "***", caption, 64, ".\\Assets\\setup.ini");

	//ウィンドウクラス（設計図）を作成
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);             //この構造体のサイズ
	wc.hInstance = hInstance;                   //インスタンスハンドル
	wc.lpszClassName = WIN_CLASS_NAME;          //ウィンドウクラス名
	wc.lpfnWndProc = WndProc;                   //ウィンドウプロシージャ
	wc.style = CS_VREDRAW | CS_HREDRAW;         //スタイル（デフォルト）
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //アイコン
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);   //小さいアイコン
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);   //マウスカーソル
	wc.lpszMenuName = NULL;                     //メニュー（なし）
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //背景（白）
	RegisterClassEx(&wc); //クラスを登録

	//ウィンドウサイズの計算
	RECT winRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	int winW = winRect.right - winRect.left;     //ウィンドウ幅
	int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ

	//ウィンドウを作成
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,         //ウィンドウクラス名
		caption,     //タイトルバーに表示する内容
		WS_OVERLAPPEDWINDOW, //スタイル（普通のウィンドウ）
		CW_USEDEFAULT,       //表示位置左（おまかせ）
		CW_USEDEFAULT,       //表示位置上（おまかせ）
		winW,               //ウィンドウ幅
		winH,               //ウィンドウ高さ
		NULL,                //親ウインドウ（なし）
		NULL,                //メニュー（なし）
		hInstance,           //インスタンス
		NULL                 //パラメータ（なし）
	);

	//ウィンドウを表示
	ShowWindow(hWnd, nCmdShow);

	//Direct3D初期化
	hr = Direct3D::Initialize(winW, winH, hWnd);
	if (FAILED(hr))
	{
		MessageBox(NULL, "Direct3Dの初期化に失敗", "エラー", MB_OK);
		PostQuitMessage(0); //エラー起きたら強制終了
		return hr;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(3.0f); // すべてのサイズをスケーリング
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(Direct3D::pDevice_, Direct3D::pContext_);

	//カメラの初期化
	pCamera->Initialize();

	//DirectInputの初期化
	Input::Initialize(hWnd);

	pRootjob = new Rootjob();
	pRootjob->Initialize();

	bool isPause = false;

	//メッセージループ（何か起きるのを待つ）
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//メッセージあり
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		//メッセージなし
		else
		{
			//時間測定
			timeBeginPeriod(1);							//時間計測の制度をあげる
			static DWORD countFps = 0;
			static DWORD startTime = timeGetTime();
			DWORD nowTime = timeGetTime();
			static DWORD lastUpdateTime = nowTime;
			
			//キャプションに現在のFPS表示
			if (nowTime - startTime >= 1000)
			{
				char str[16];
				wsprintf(str, "%u", countFps);
				SetWindowText(hWnd, str);

				countFps = 0;
				startTime = nowTime;
			}

			if ((nowTime - lastUpdateTime) * 60 <= 1000)
			{
				continue;
			}
			lastUpdateTime = nowTime;

			countFps++;

			timeEndPeriod(1);

			if (Input::IsKeyDown(DIK_P))
			{
				isPause = !isPause;
			}

			//▼ゲームの処理
			//入力の処理
			Input::Update();

			if (!isPause)
			{
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				pRootjob->UpdateSub();

				//▼描画

				Direct3D::Update();

				Direct3D::BeginDraw();
			}
			constexpr uint8_t SIZE_VP = 2;
			for (auto i = 0u; i < SIZE_VP; i++) {
				Direct3D::SetViewPort(i);
				pCamera->Update(i);
				//カメラの処理
				pRootjob->DrawSub();	//ルートジョブから、すべてのオブジェクトのドローを呼ぶ

			}
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			Direct3D::EndDraw();

			if (Input::IsKeyUp(DIK_ESCAPE))
			{
				PostQuitMessage(0);
			}
		}
	}

	Model::AllRelease();
	pRootjob->ReleaseSub();
	SAFE_DELETE(pRootjob);

	Input::Release();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
	Direct3D::Release();

	return S_OK;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);


//ウィンドウプロシージャ（何かあった時によばれる関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);  //プログラム終了
		return 0;
	}
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	return DefWindowProc(hWnd, msg, wParam, lParam);
}