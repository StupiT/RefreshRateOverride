// STUPID PROG, BASED ON DEFAULT VS2010 Express C++ Win32 Project
// ---------------------------------------------------------------
// I don't like this way of changing display settings,
// I don't like to "kick" monitor twice, but it's the price
// for using silly "afterkick" (WM_DISPLAYCHANGE).
// I made this program just as a primitive little "brother"
// of DirectRefresh (which is using .NET, that I don't like).
// ---------------------------------------------------------------
#include "RefreshRateOverrideTEST.h"

TCHAR Title[]=L"Refresh Rate Override by StupiT [2012-03-06-15] (Inspired by Direct Refresh)\0";

DWORD RROenable=8;
DWORD RROdefault=60;
DWORD updateregistry=0;
DWORD Skip_WM_DISPLAYCHANGE=0;
HMENU hmenu;
HINSTANCE hInst;
DWORD refreshrate;
DEVMODE devmode;

ATOM				MyRegisterClass();
BOOL				InitInstance();
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void _go()
{
	MSG msg;
	hInst = GetModuleHandle(0);
	if(!MyRegisterClass()) ExitProcess(-1);
	if(!InitInstance()) ExitProcess(-1);

	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	ExitProcess(msg.wParam);
}



ATOM MyRegisterClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= LoadIcon(0, IDI_EXCLAMATION);
	wcex.hIconSm		= wcex.hIcon;
	wcex.hCursor		= LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCWSTR)ID_MENU;
	wcex.lpszClassName	= Title;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance()
{
	HWND hWnd;

	hWnd = CreateWindowEx(0, Title, Title,
		(WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX),
		160, 320, 720, 80, 0, 0, hInst, 0);

	if (!hWnd) return 0;

	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);
	return 1;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_60:
		case IDM_70:
		case IDM_85:
		case IDM_100:
		case IDM_120:
		case IDM_150:
			if(wmId==RROdefault)
			{
				RROenable^=8;
				CheckMenuItem(hmenu, wmId, RROenable);
			}
			else
			{
				CheckMenuItem(hmenu, RROdefault, 0);
				RROdefault=wmId;
				RROenable=8;
				CheckMenuItem(hmenu, wmId, 8);
			}
			break;
		case IDM_UPDATEREGISTRY:
			updateregistry^=1;
			CheckMenuItem(hmenu, IDM_UPDATEREGISTRY, (updateregistry<<3));
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DISPLAYCHANGE:
		if(!RROenable || Skip_WM_DISPLAYCHANGE) break;

		HDC hDefaultDisplay;
		hDefaultDisplay = GetDC(0);
		if(!hDefaultDisplay) break;

		refreshrate = GetDeviceCaps(hDefaultDisplay, VREFRESH);
		ReleaseDC(0, hDefaultDisplay);
		if (refreshrate==RROdefault) break;

		devmode.dmSize = sizeof(devmode);
		devmode.dmPelsWidth = (lParam<<16>>16);
		devmode.dmPelsHeight = (lParam>>16);
		devmode.dmFields = (DM_DISPLAYFREQUENCY|DM_PELSWIDTH|DM_PELSHEIGHT);
		devmode.dmDisplayFrequency = RROdefault;

		/*
		TCHAR temp[256];
		wsprintf((LPWSTR)&temp,L"%ldx%ld",devmode.dmPelsWidth,devmode.dmPelsHeight);
		MessageBox(0,(LPWSTR)&temp,(LPWSTR)&temp,0);
		break;
		//*/

		// TEST will not help if there is no restriction on frequency (user turned it off, etc)!
		if((ChangeDisplaySettingsW(&devmode, CDS_TEST))==DISP_CHANGE_SUCCESSFUL)
		{
			Skip_WM_DISPLAYCHANGE=1; // I hope this works as I think...
			ChangeDisplaySettingsW(&devmode, updateregistry); // CDS_UPDATEREGISTRY = 1
			Skip_WM_DISPLAYCHANGE=0;
		}
		break;

	case WM_CREATE:
		hmenu=GetMenu(hWnd);
		if(hmenu)return 1;
		else return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
