// Graphic.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Graphic.h"

#define WND_WIDTH 1100
#define WND_HEIGHT 800

#define PHOTO_X 600
#define PHOTO_Y 100
#define PHOTO_CX 204 
#define PHOTO_CY 252

static HDC hdcPhoto;
static HDC maskDC;
static HDC InvertMaskDC;
static HDC hdcBG;
static SIZE sizePhoto;
static SIZE sizeBG;

static int cxChar;
static int cyChar;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

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
	LoadString(hInstance, IDC_GRAPHIC, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHIC));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHIC));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GRAPHIC);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BITMAPFILEHEADER * DibLoadImage (PTSTR pstrFileName)
{
	BOOL               bSuccess ;
	DWORD              dwFileSize, dwHighSize, dwBytesRead ;
	HANDLE             hFile ;
	BITMAPFILEHEADER * pbmfh ;

	hFile = CreateFile (pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
		return NULL ;

	dwFileSize = GetFileSize (hFile, &dwHighSize) ;

	if (dwHighSize)
	{
		CloseHandle (hFile) ;
		return NULL ;
	}

	pbmfh = (BITMAPFILEHEADER*)malloc(dwFileSize);
	if (!pbmfh)
	{
		CloseHandle (hFile) ;
		return NULL ;
	}

	bSuccess = ReadFile (hFile, pbmfh, dwFileSize, &dwBytesRead, NULL) ;
	CloseHandle (hFile) ;
	if (!bSuccess || (dwBytesRead != dwFileSize)         
		|| (pbmfh->bfType != * (WORD *) "BM") 
		|| (pbmfh->bfSize != dwFileSize))
	{
		free (pbmfh) ;
		return NULL ;
	}
	return pbmfh ;
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

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, WND_WIDTH, WND_HEIGHT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	HDC hdc;
	//retrieves the device context (DC) for the entire window
	hdc = GetDC(hWnd);
	// creates a memory device context (DC) compatible with the specified device
	hdcPhoto = CreateCompatibleDC(hdc);
	hdcBG = CreateCompatibleDC(hdc);
	HDC hdc_origBG;
	hdc_origBG = CreateCompatibleDC(hdc);
	//    hBmp = (HBITMAP)LoadImage(NULL, L"D:\\cat.bmp", IMAGE_BITMAP, 0,
	// 	   0, LR_LOADFROMFILE);
	//    BITMAPINFO bmpInfo = {0};
	//    bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
	//    GetDIBits(hBgd, hBmp, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);
	// 
	HBITMAP hPhoto;
	hPhoto = (HBITMAP)LoadImage(NULL, L"D:\\zp_green.bmp", IMAGE_BITMAP, PHOTO_CX, PHOTO_CY, LR_LOADFROMFILE);
	//获取长和高
	BITMAP bitmap;
	GetObject(hPhoto, sizeof(bitmap), &bitmap);
	sizePhoto.cx = bitmap.bmWidth;
	sizePhoto.cy = bitmap.bmHeight;
	SelectObject(hdcPhoto, (HGDIOBJ)hPhoto);


	HBITMAP hBGOrig;
	hBGOrig = (HBITMAP)LoadImage(NULL, L"D:\\A.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hBGOrig, sizeof(bitmap), &bitmap);
	sizeBG.cx = bitmap.bmWidth;
	sizeBG.cy = bitmap.bmHeight;
	SelectObject(hdc_origBG, (HGDIOBJ)hBGOrig);
	HBITMAP hGB = CreateCompatibleBitmap(hdc, sizeBG.cx, sizeBG.cy);
	SelectObject(hdcBG, (HGDIOBJ)hGB);
	StretchBlt(hdcBG, 0, 0, sizeBG.cx, sizeBG.cy, hdc_origBG, 0, 0, sizeBG.cx, sizeBG.cy, MERGECOPY);

	//创建蒙板
	maskDC = CreateCompatibleDC(NULL);
	InvertMaskDC = CreateCompatibleDC(NULL);
	HBITMAP hMaskBmp;
	hMaskBmp = CreateBitmap(sizePhoto.cx, sizePhoto.cy, 1, 1, 0);
	SelectObject(maskDC, hMaskBmp);
	HBITMAP hMaskBmp2;
	hMaskBmp2 = CreateBitmap(sizePhoto.cx, sizePhoto.cy, 1, 1, 0);
	SelectObject(InvertMaskDC, hMaskBmp2);

	ReleaseDC (hWnd, hdc) ;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

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
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	BOOL ret;
	COLORREF transcolor,WhiteColor;
	TEXTMETRIC tm ;
	int cx_screen, cy_screen;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd) ;
		GetTextMetrics (hdc, &tm) ;
		cxChar = tm.tmAveCharWidth ;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		cx_screen = GetSystemMetrics(SM_CXSCREEN);
		cy_screen = GetSystemMetrics(SM_CYSCREEN);
		ReleaseDC (hWnd, hdc) ;
		return 0 ;


	case WM_COMMAND:
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
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//使用图片的左上角像素的颜色作为背景色
		transcolor= GetPixel(hdcPhoto,1,1);
		SetBkColor(hdcPhoto, transcolor);
		//转换为黑白蒙板图时，背景色转换为白色，其他颜色转换为黑色
		ret = BitBlt(maskDC, 0, 0, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, SRCCOPY);
		//背景色为黑色，其他颜色为白色，与上一个图片相反
		ret = BitBlt(InvertMaskDC, 0, 0, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, NOTSRCCOPY);

		//设置背景色为白色
		WhiteColor = RGB(255, 255, 255);
 		SetBkColor(hdcPhoto, WhiteColor);
		//将黑白蒙板的白色位置像素全部设置为之前设置的背景色，其他颜色进行OR操作。
		//即将相片背景色改为白色。
		BitBlt(hdcPhoto, 0, 0, sizePhoto.cx, sizePhoto.cy, maskDC, 0, 0, SRCPAINT);

		//将反转黑白蒙板与背景图进行OR操作，即前景位置像素设置为白色，背景像素颜色不变。
 		//即头像位置像素设为白色
		BitBlt(hdcBG, PHOTO_X, PHOTO_Y, sizePhoto.cx, sizePhoto.cy, InvertMaskDC, 0, 0, SRCPAINT);
		//将处理后的头像与背景图进行AND操作，达到背景图透明的效果。
   		BitBlt(hdcBG, PHOTO_X, PHOTO_Y, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, SRCAND);
		//显示最终结果
 		BitBlt(hdc, 0, 0, sizeBG.cx, sizeBG.cy, hdcBG, 0, 0, SRCCOPY);
		//BitBlt(hdc, 0, 0, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, SRCCOPY);
		TextOut (hdc, 0, 0, L"Hello World", 11) ;
		TextOut (hdc, 0, cyChar, L"My name is Jim", 14) ;

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
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
