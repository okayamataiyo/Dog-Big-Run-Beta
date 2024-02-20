//�C���N���[�h
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

//�����J
#pragma comment(lib, "winmm.lib")

//�萔�錾
const char* WIN_CLASS_NAME = "Dog Big Run";  //�E�B���h�E�N���X��

Rootjob* pRootjob = nullptr;
Camera* pCamera = new Camera;


//�v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//�G���g���[�|�C���g
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	HRESULT hr;

	//�������t�@�C��(setup.ini)����K�v�ȏ����擾
	int screenWidth = GetPrivateProfileInt("SCREEN", "Width", 800, ".\\Assets\\setup.ini");
	int screenHeight = GetPrivateProfileInt("SCREEN", "Height", 600, ".\\Assets\\setup.ini");
	int fpsLimit = GetPrivateProfileInt("GAME", "Fps", 60, ".\\Assets\\setup.ini");
	int isDrawFps = GetPrivateProfileInt("DEBUG", "ViewFps", 0, ".\\Assets\\setup.ini");
	char caption[64];
	GetPrivateProfileString("SCREEN", "Caption", "***", caption, 64, ".\\Assets\\setup.ini");

	//�E�B���h�E�N���X�i�݌v�}�j���쐬
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);             //���̍\���̂̃T�C�Y
	wc.hInstance = hInstance;                   //�C���X�^���X�n���h��
	wc.lpszClassName = WIN_CLASS_NAME;          //�E�B���h�E�N���X��
	wc.lpfnWndProc = WndProc;                   //�E�B���h�E�v���V�[�W��
	wc.style = CS_VREDRAW | CS_HREDRAW;         //�X�^�C���i�f�t�H���g�j
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //�A�C�R��
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);   //�������A�C�R��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);   //�}�E�X�J�[�\��
	wc.lpszMenuName = NULL;                     //���j���[�i�Ȃ��j
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //�w�i�i���j
	RegisterClassEx(&wc); //�N���X��o�^

	//�E�B���h�E�T�C�Y�̌v�Z
	RECT winRect = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	int winW = winRect.right - winRect.left;     //�E�B���h�E��
	int winH = winRect.bottom - winRect.top;     //�E�B���h�E����

	//�E�B���h�E���쐬
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,         //�E�B���h�E�N���X��
		caption,     //�^�C�g���o�[�ɕ\��������e
		WS_OVERLAPPEDWINDOW, //�X�^�C���i���ʂ̃E�B���h�E�j
		CW_USEDEFAULT,       //�\���ʒu���i���܂����j
		CW_USEDEFAULT,       //�\���ʒu��i���܂����j
		winW,               //�E�B���h�E��
		winH,               //�E�B���h�E����
		NULL,                //�e�E�C���h�E�i�Ȃ��j
		NULL,                //���j���[�i�Ȃ��j
		hInstance,           //�C���X�^���X
		NULL                 //�p�����[�^�i�Ȃ��j
	);

	//�E�B���h�E��\��
	ShowWindow(hWnd, nCmdShow);

	//Direct3D������
	hr = Direct3D::Initialize(winW, winH, hWnd);
	if (FAILED(hr))
	{
		MessageBox(NULL, "Direct3D�̏������Ɏ��s", "�G���[", MB_OK);
		PostQuitMessage(0); //�G���[�N�����狭���I��
		return hr;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(3.0f); // ���ׂẴT�C�Y���X�P�[�����O
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(Direct3D::pDevice_, Direct3D::pContext_);

	//�J�����̏�����
	pCamera->Initialize();

	//DirectInput�̏�����
	Input::Initialize(hWnd);

	pRootjob = new Rootjob();
	pRootjob->Initialize();

	bool isPause = false;

	//���b�Z�[�W���[�v�i�����N����̂�҂j
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		//���b�Z�[�W����
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		//���b�Z�[�W�Ȃ�
		else
		{
			//���ԑ���
			timeBeginPeriod(1);							//���Ԍv���̐��x��������
			static DWORD countFps = 0;
			static DWORD startTime = timeGetTime();
			DWORD nowTime = timeGetTime();
			static DWORD lastUpdateTime = nowTime;
			
			//�L���v�V�����Ɍ��݂�FPS�\��
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

			//���Q�[���̏���
			//���͂̏���
			Input::Update();

			if (!isPause)
			{
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplWin32_NewFrame();
				ImGui::NewFrame();
				pRootjob->UpdateSub();

				//���`��

				Direct3D::Update();

				Direct3D::BeginDraw();
			}
			constexpr uint8_t SIZE_VP = 2;
			for (auto i = 0u; i < SIZE_VP; i++) {
				Direct3D::SetViewPort(i);
				pCamera->Update(i);
				//�J�����̏���
				pRootjob->DrawSub();	//���[�g�W���u����A���ׂẴI�u�W�F�N�g�̃h���[���Ă�

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

	Model::Release();
	pRootjob->ReleaseSub();
	SAFE_DELETE(pRootjob);

	Input::Release();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
	Direct3D::Release();

	return S_OK;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);


//�E�B���h�E�v���V�[�W���i�������������ɂ�΂��֐��j
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);  //�v���O�����I��
		return 0;
	}
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	return DefWindowProc(hWnd, msg, wParam, lParam);
}