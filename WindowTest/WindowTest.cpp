// WindowTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WindowTest.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

//显示模式，0表示空，1表示显示键盘按键信息
int DisplayMode = 0;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINDOWTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWTEST));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWTEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINDOWTEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


#define BUTTON_ID 1
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int   cxClientMax, cyClientMax, cxClient, cyClient, cxChar, cyChar ;
	static int   cLinesMax, cLines ;
	static PMSG  pmsg ;
	static int* state_array;
	static RECT  rectScroll ;
	static TCHAR szTop[] = TEXT ("Message        Key       Char     ")
		TEXT ("Repeat Scan Ext ALT Prev Tran Shift Ctrl Alt") ;
	static TCHAR szUnd[] = TEXT ("_______        ___       ____     ")
		TEXT ("______ ____ ___ ___ ____ ____ _____ ____ ___") ;

	static TCHAR * szFormat[2] = { 
		TEXT ("%-13s %3d %-15s%c%6u %4d %3s %3s %4s %4s %5s %4s %3s"),
		TEXT ("%-13s            0x%04X%1s%c %6u %4d %3s %3s %4s %4s %5s %4s %3s") 
	};
	static TCHAR * szYes  = TEXT ("Yes") ;
	static TCHAR * szNo   = TEXT ("No") ;
	static TCHAR * szDown = TEXT ("Down") ;
	static TCHAR * szUp   = TEXT ("Up") ;

	static TCHAR * szMessage [] = { 
		TEXT ("WM_KEYDOWN"),    TEXT ("WM_KEYUP"), 
		TEXT ("WM_CHAR"),       TEXT ("WM_DEADCHAR"), 
		TEXT ("WM_SYSKEYDOWN"), TEXT ("WM_SYSKEYUP"), 
		TEXT ("WM_SYSCHAR"),    TEXT ("WM_SYSDEADCHAR") 
	};
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	int i, iType ;
	HDC hdc;
	static HWND hwndButton;
	TCHAR        szBuffer[128], szKeyName [32] ;
	TEXTMETRIC   tm ;
	int mouse_x, mouse_y;
	HMENU      hMenu ;


	switch (message)
	{
	case WM_CREATE:
		OutputDebugString(L"WM_CREATE\n");
// 		hwndButton = CreateWindow(TEXT("button"), TEXT("TEST"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
// 			5, 5, 40, 20, hWnd, (HMENU)BUTTON_ID, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
// 		break;
		hMenu = GetMenu(hWnd);
		CheckMenuItem(hMenu, ID_FUNCTION_NORMAL, MF_CHECKED);


		//the display resolution has changed
	case WM_DISPLAYCHANGE:
		// Get maximum size of client area
		cxClientMax = GetSystemMetrics (SM_CXMAXIMIZED) ;
		cyClientMax = GetSystemMetrics (SM_CYMAXIMIZED) ;

		// Get character size for fixed-pitch font
		hdc = GetDC (hWnd) ;
		SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
		GetTextMetrics (hdc, &tm) ;
		cxChar = tm.tmAveCharWidth ;
		cyChar = tm.tmHeight ;
		ReleaseDC (hWnd, hdc) ;

		// Allocate memory for display lines
		if (pmsg)
			free (pmsg) ;
		if (state_array)
		{
			free(state_array);
		}

		//计算可显示的最大行数
		cLinesMax = cyClientMax / cyChar ;
		pmsg = (MSG*)malloc (cLinesMax * sizeof (MSG)) ;
		state_array = (int*)malloc(cLinesMax * sizeof(int));
		cLines = 0 ;
		// fall through
	case WM_SIZE:
		if (message == WM_SIZE)
		{
			cxClient = LOWORD (lParam) ;
			cyClient = HIWORD (lParam) ;
		}
		// Calculate scrolling rectangle
		rectScroll.left   = 0 ;
		rectScroll.right  = cxClient ;
		rectScroll.top    = cyChar ;
		rectScroll.bottom = cyChar * (cyClient / cyChar) ;

		InvalidateRect (hWnd, NULL, TRUE) ;
		return 0 ;

	case WM_SHOWWINDOW:
		OutputDebugString(L"WM_SHOWWINDOW\n");
		break;
	case WM_COMMAND:
		hMenu = GetMenu(hWnd);
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case BUTTON_ID:
			ShowWindow(hWnd, FALSE);
			break;
		case ID_FUNCTION_KEYBOARD:
			CheckMenuItem (hMenu, wmId, MF_CHECKED);
			CheckMenuItem(hMenu, ID_FUNCTION_NORMAL, MF_UNCHECKED);
			DisplayMode = 1;
			//强制界面重绘
			InvalidateRect (hWnd, NULL, TRUE) ;
			break;
		case ID_FUNCTION_NORMAL:
			CheckMenuItem (hMenu, wmId, MF_CHECKED);
			CheckMenuItem(hMenu, ID_FUNCTION_KEYBOARD, MF_UNCHECKED);
			DisplayMode = 0;
			//强制界面重绘
			InvalidateRect (hWnd, NULL, TRUE) ;
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SYSCOMMAND:
		mouse_x = HIWORD(lParam);
		mouse_y = LOWORD(lParam);
		wsprintf(szBuffer, L"Mouse Pos: X:%d, Y:%d\n", mouse_x, mouse_y);
		OutputDebugString(szBuffer);
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR: 
		if (DisplayMode == 1)
		{
			// Rearrange storage array
			for (i = cLinesMax - 1 ; i > 0 ; i--)
			{
				pmsg[i] = pmsg[i - 1] ;
				state_array[i] = state_array[i - 1];
			}
			// Store new message
			pmsg[0].hwnd = hWnd ;
			pmsg[0].message = message ;
			pmsg[0].wParam = wParam ;
			pmsg[0].lParam = lParam ;

			state_array[0] = 0;
			// 检查按键状态
			if (GetKeyState(VK_SHIFT) & 0x8000)
			{
				state_array[0] |= 1;
			}
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
				state_array[0] |= 2;
			}
			if (GetKeyState(VK_MENU) & 0x8000)
			{
				state_array[0] |= 4;
			}

			cLines = min(cLines + 1, cLinesMax) ;

			// Scroll up the display
			ScrollWindow (hWnd, 0, -cyChar, &rectScroll, &rectScroll) ;
			OutputDebugString(L"WM_KEY&SCROLL\n");
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break ;        // i.e., call DefWindowProc so Sys messages work

	case WM_PAINT:
		hdc = BeginPaint (hWnd, &ps) ;
		if (DisplayMode == 1)
		{
			SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
			SetBkMode (hdc, TRANSPARENT) ;
			TextOut (hdc, 0, 0, szTop, lstrlen (szTop)) ;
			TextOut (hdc, 0, 0, szUnd, lstrlen (szUnd)) ;

			for (i = 0 ; i < min (cLines, cyClient / cyChar - 1) ; i++)
			{
				iType = pmsg[i].message == WM_CHAR || pmsg[i].message == WM_SYSCHAR ||
					pmsg[i].message == WM_DEADCHAR || pmsg[i].message == WM_SYSDEADCHAR;

				GetKeyNameText(pmsg[i].lParam, szKeyName, sizeof(szKeyName) / sizeof(TCHAR));

				TextOut (hdc, 0, (cyClient / cyChar - 1 - i) * cyChar, szBuffer,
					wsprintf (szBuffer, szFormat [iType],
					szMessage [pmsg[i].message - WM_KEYFIRST],
					pmsg[i].wParam,
					(PTSTR) (iType ? TEXT (" ") : szKeyName),	//KEYSTROKE
					(TCHAR) (iType ? pmsg[i].wParam : ' '),		//CHAR
					LOWORD (pmsg[i].lParam),		//16-bit repeat count
					HIWORD (pmsg[i].lParam) & 0xFF,	//8-bit OEM scan code
					0x01000000 & pmsg[i].lParam ? szYes  : szNo,
					0x20000000 & pmsg[i].lParam ? szYes  : szNo,
					0x40000000 & pmsg[i].lParam ? szDown : szUp,
					0x80000000 & pmsg[i].lParam ? szUp   : szDown,
					0x00000001 & state_array[i] ? szDown : szUp,
					0x00000002 & state_array[i] ? szDown : szUp,
					0x00000004 & state_array[i] ? szDown : szUp
					)) ;
			}
		}

		EndPaint (hWnd, &ps) ;
		OutputDebugString(L"WM_PAINT\n");
		return 0 ;
	case WM_CLOSE:
		OutputDebugString(L"WM_CLOSE\n");
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		OutputDebugString(L"WM_DESTROY\n");
		PostQuitMessage(0);
		break;
	case WM_SETFOCUS:
		OutputDebugString(L"WM_SETFOCUS\n");
		break;
	case WM_KILLFOCUS:
		OutputDebugString(L"WM_KILLFOCUS\n");
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
