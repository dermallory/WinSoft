// WinSoft.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "winsoft.h"

#include <iostream>
#include <random>
#include <ctime>
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hWindow;									// window
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

bool isRunning = true;

RECT clientRect;

POINT clickPos = {0, 0};
float objPosX = 0, objPosY = 0;
float velX = 0, velY = 0;

const float TARGET_RADIUS = 25.0f;
const float MAX_SPEED = 100.0f;
const float ACCELERATION = 1.25f;

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
	srand((unsigned int)time(NULL));

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

	GetClientRect(hWindow, &clientRect);

    MSG msg;
	
    // Main message loop:
    while (isRunning)
    {
        PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
			TranslateMessage(&msg);
            DispatchMessage(&msg);			
        }

		// calculate new position of rectangle
		{
			const float dx = clickPos.x - objPosX;
			const float dy = clickPos.y - objPosY;

			const float distance = sqrt((dx*dx)+(dy*dy));

			if (distance > TARGET_RADIUS)
			{
				int dirX = 0;
				if (dx > 0)
					dirX = 1;
				else if (dx < 0)
					dirX = -1;

				int dirY = 0;
				if (dy > 0)
					dirY = -1;
				else if (dy < 0)
					dirY = 1;

				velX += cos(ACCELERATION)*dirX;
				velY += -cos(ACCELERATION)*dirY;		

				velX = abs(velX) < cos(MAX_SPEED) ? velX : cos(MAX_SPEED)*dirX;
				velY = abs(velY) < -cos(MAX_SPEED) ? velY : -cos(MAX_SPEED)*dirY;

				objPosX += velX;// * WinSoft::FRAMES_PER_SECOND;
				objPosY += velY;// * WinSoft::FRAMES_PER_SECOND;
			}
			else
			{
				objPosX = clickPos.x;
				objPosY = clickPos.y;
			}
		}		

		// RENDER THE SCENE
		HDC hdc = GetDC(hWindow);

		// clear client area		
		InvalidateRect(hWindow, &clientRect, true);

		Rectangle(hdc, objPosX+0, objPosY+0, objPosX+50, objPosY+50);

		ReleaseDC(hWindow, hdc);
		
		// block the thread for framerate
		Sleep(WinSoft::FRAMES_PER_SECOND);
    }
	
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
    case WM_PAINT:
        {
   //         PAINTSTRUCT ps;
   //         HDC hdc = BeginPaint(hWnd, &ps);
   //         // TODO: Add any drawing code that uses hdc here...	
			///*RECT rect;
			//GetWindowRect(hWnd, &rect);*/

			///*for (int y = 0; y < 250; ++y)
			//{
			//	for (int x = 0; x < 250; ++x)
			//	{
			//		SetPixel(hdc, x, y, rand() % (256*256*256));
			//	}
			//}*/
			//POINT points[7];
			//points[0] = { curvePos.x + 50,  curvePos.y + 50 };
			//points[1] = { curvePos.x + 250, curvePos.y + 250};
			//points[2] = { curvePos.x + 50,  curvePos.y + 250 };
			//points[3] = { curvePos.x + 75,  curvePos.y + 250 };
			//points[4] = { curvePos.x + 275, curvePos.y + 50 };
			//points[5] = { curvePos.x + 95,  curvePos.y + 45};
			//points[6] = { curvePos.x + 83,  curvePos.y + 23 };

			//PolyBezier(hdc, points, 7);

   //         EndPaint(hWnd, &ps);
        }
        break;
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		clickPos.x = x;
		clickPos.y = y;
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
