// WinSoft.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "winsoft.h"

#include <cmath>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hWindow;									// window
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

bool isRunning = true;
bool mouseDown = false;
int mouseX = -1;
int mouseY = -1;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void CreateConsole();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	CreateConsole();

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINSOFT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);	
	
    
	// Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINSOFT));
	
	// create the bitmap backbuffer for rendering (hCompatBuffer)
	// "memory/compatible device contexts"
	HDC hMemDC = NULL;
	HDC hClient = NULL;
	HBITMAP map = NULL;
	RECT clientRect;

	hClient = GetDC(hWindow);

	GetClientRect(hWindow, &clientRect);

	const DWORD WIDTH = clientRect.right+1;
	const DWORD HEIGHT = clientRect.bottom+1;

	hMemDC = CreateCompatibleDC(hClient);

	BITMAPINFOHEADER header;
	ZeroMemory(&header, sizeof(BITMAPINFOHEADER));
	header.biBitCount = 32;
	header.biCompression = BI_RGB;
	header.biWidth = WIDTH;
	header.biHeight = HEIGHT;
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biPlanes = 1;
	BITMAPINFO info;
	ZeroMemory(&info, sizeof(BITMAPINFO));
	info.bmiHeader = header;

	PVOID surfaceData = NULL;

	HBITMAP hDibSurface = CreateDIBSection(hMemDC, &info, DIB_RGB_COLORS, &surfaceData, NULL, 0);
	SelectObject(hMemDC, hDibSurface);

	WinSoft::Surface surface {};
	surface._data = surfaceData;
	surface._rect = WinSoft::Rect{ WinSoft::Point{ 0, 0 }, WinSoft::Point{ (float)WIDTH, (float)HEIGHT } };

    // Main message loop:
	MSG msg;

	WinSoft::Startup();

	WinSoft::Draw3DSettings drawSettings{};
	drawSettings._drawMode = WinSoft::Draw3DMode::WIREFRAME;

	WinSoft::Vertex vertices[6] = {
		WinSoft::Vertex{ 20.f, 20.f, 1.0f, 0, 0, 1.0f },
		WinSoft::Vertex{ 100.f, 100.f, 1.0f, 0, 0, 1.0f },
		WinSoft::Vertex{ 180.f, 20.f, 1.0f, 0, 0, 1.0f },
		WinSoft::Vertex{ 100.f, 150.f, 1.0f, 0, 0, 1.0f },
		WinSoft::Vertex{ 125.f, 170.f, 1.0f, 1.0f, 0, 1.0f },
		WinSoft::Vertex{ 0.f, 170.f, 1.0f, 1.0f, 0, 1.0f }
	};
	WinSoft::Vertex vertices2[4] = {
		WinSoft::Vertex{ 320.f, 320.f, 1.0f, 0.25f, 0, 1.0f },
		WinSoft::Vertex{ 400.f, 100.f, 1.0f, 0, 0.289f, 1.0f },
		WinSoft::Vertex{ 480.f, 220.f, 1.0f, 0, 0.5f, 1.0f },
		WinSoft::Vertex{ 500.f, 150.f, 1.0f, 0.25f, 0, 1.0f }
	};
	unsigned int indices[6] = {
		5, 1, 0, 4, 2, 1
	};
	unsigned int indices2[4] = {
		1, 3, 0, 2
	};

	int vbo = WinSoft::CreateVBO(vertices, 6);
	int vbo2 = WinSoft::CreateVBO(vertices2, 4);
	int ibo = WinSoft::CreateIBO(indices, 6);
	int ibo2 = WinSoft::CreateIBO(indices2, 4);
	int object1 = WinSoft::CreateObject(vbo, ibo, WinSoft::Primitive::TRIANGLE_STRIP);
	int object2 = WinSoft::CreateObject(vbo2, ibo2, WinSoft::Primitive::TRIANGLE_STRIP);
	
    while (isRunning)
    {
        PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
			TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

		WinSoft::RefreshSurface(surface, WinSoft::FColor32 { 0, 0, 0, 0 });
		
		if (mouseDown)
			WinSoft::DrawObject(object1);
		else
			WinSoft::DrawObject(object2);
		WinSoft::Draw3D(drawSettings, surface);

		// render
		SetDIBits(hMemDC, hDibSurface, 0, WIDTH*HEIGHT, NULL, &info, DIB_RGB_COLORS);
		BitBlt(hClient, 0, 0, WIDTH, HEIGHT, hMemDC, 0, 0, SRCCOPY);

		// block the thread for framerate
		Sleep(10);
    }

	WinSoft::Shutdown();

	DeleteObject(hDibSurface);
	DeleteDC(hMemDC);
	ReleaseDC(hWindow, hClient);

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINSOFT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINSOFT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = hWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
    
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
	}
		break;
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouseDown = true;
	}
		break;
	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		mouseX = x;
		mouseY = y;
	}
		break;
	case WM_LBUTTONUP:
	{
		mouseDown = false;
	}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);	
		isRunning = false;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void CreateConsole()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
}
