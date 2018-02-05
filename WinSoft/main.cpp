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

	const float PI = 3.14159265359;	
	const float MAX_RADIUS = 200.0f;
	const float ROTATION = 360.f/60.f;

	float y = 0;
	float x = 0;
	float radius = MAX_RADIUS;

	int degrees = 0;

    while (isRunning)
    {
        PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
			TranslateMessage(&msg);
            DispatchMessage(&msg);
        }	

		float radians = degrees *PI/180;
		x = std::cos(radians)*radius + (WIDTH/4-1);
		y = std::sin(radians)*radius + (HEIGHT/2-1);

		WinSoft::RefreshSurface(surface, WinSoft::Color32 { 0, 0, 0, 0 });
		WinSoft::DrawLine(WinSoft::Point{(float)WIDTH/4-1, (float)HEIGHT/2-1}, WinSoft::Point{(float)x, (float)y}, WinSoft::Color32{1, 0, 0, 1}, surface);

		degrees -= ROTATION;

		// render
		SetDIBits(hMemDC, hDibSurface, 0, WIDTH*HEIGHT, NULL, &info, DIB_RGB_COLORS);
		BitBlt(hClient, 0, 0, WIDTH, HEIGHT, hMemDC, 0, 0, SRCCOPY);

		// block the thread for framerate
		Sleep(10);
    }

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
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
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
}
