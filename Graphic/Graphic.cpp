// Graphic.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Graphic.h"

#define WND_WIDTH 1100
#define WND_HEIGHT 800

static SIZE sizeBG;

/*��Ƭλ��*/
static SIZE sizePhoto;
static POINT posPhoto;
static POINT posName;
static POINT posGender;
static POINT posNationality;

static int FontHeight;

static HDC hdcPhoto;
static HDC maskDC;
static HDC InvertMaskDC;
static HDC hdcBG;

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

void CalcElementsPos(SIZE bg)
{
	sizePhoto.cx = (LONG)(bg.cx * 26 / 85.6);
	sizePhoto.cy = bg.cy * 32 / 54;
	posPhoto.x = (LONG)(bg.cx * 0.63);
	posPhoto.y = (LONG)(bg.cy * 0.115);

	FontHeight = (LONG)(sizeBG.cy * 0.07);
	posName.x = (LONG)(sizeBG.cx * 0.2);
	posName.y = (LONG)(sizeBG.cy * 0.145);
}

void DrawIDPicture(HWND hWnd)
{
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

	BITMAP bitmap;

	//��ȡ����ͼƬ
	HBITMAP hBGOrig;
	hBGOrig = (HBITMAP)LoadImage(NULL, L"D:\\A.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hBGOrig, sizeof(bitmap), &bitmap);
	sizeBG.cx = bitmap.bmWidth;
	sizeBG.cy = bitmap.bmHeight;
	SelectObject(hdc_origBG, (HGDIOBJ)hBGOrig);
	HBITMAP hGB = CreateCompatibleBitmap(hdc, sizeBG.cx, sizeBG.cy);
	SelectObject(hdcBG, (HGDIOBJ)hGB);
	StretchBlt(hdcBG, 0, 0, sizeBG.cx, sizeBG.cy, hdc_origBG, 0, 0, sizeBG.cx, sizeBG.cy, MERGECOPY);

	DeleteObject(hBGOrig);

	//����ߴ�
	CalcElementsPos(sizeBG);

	HBITMAP hPhoto;
	hPhoto = (HBITMAP)LoadImage(NULL, L"D:\\370102198502152134.bmp", IMAGE_BITMAP, sizePhoto.cx, sizePhoto.cy, LR_LOADFROMFILE);
	//��ȡ���͸�

	SelectObject(hdcPhoto, (HGDIOBJ)hPhoto);

	//�����ɰ�
	maskDC = CreateCompatibleDC(NULL);
	InvertMaskDC = CreateCompatibleDC(NULL);
	HBITMAP hMaskBmp;
	hMaskBmp = CreateBitmap(sizePhoto.cx, sizePhoto.cy, 1, 1, 0);
	SelectObject(maskDC, hMaskBmp);
	HBITMAP hMaskBmp2;
	hMaskBmp2 = CreateBitmap(sizePhoto.cx, sizePhoto.cy, 1, 1, 0);
	SelectObject(InvertMaskDC, hMaskBmp2);

	COLORREF transcolor,WhiteColor;
	BOOL ret;
	//ʹ��ͼƬ�����Ͻ����ص���ɫ��Ϊ����ɫ
	transcolor= GetPixel(hdcPhoto,1,1);
	SetBkColor(hdcPhoto, transcolor);
	//ת��Ϊ�ڰ��ɰ�ͼʱ������ɫת��Ϊ��ɫ��������ɫת��Ϊ��ɫ
	ret = BitBlt(maskDC, 0, 0, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, SRCCOPY);
	//����ɫΪ��ɫ��������ɫΪ��ɫ������һ��ͼƬ�෴
	ret = BitBlt(InvertMaskDC, 0, 0, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, NOTSRCCOPY);

	//���ñ���ɫΪ��ɫ
	WhiteColor = RGB(255, 255, 255);
	SetBkColor(hdcPhoto, WhiteColor);
	//���ڰ��ɰ�İ�ɫλ������ȫ������Ϊ֮ǰ���õı���ɫ��������ɫ����OR������
	//������Ƭ����ɫ��Ϊ��ɫ��
	BitBlt(hdcPhoto, 0, 0, sizePhoto.cx, sizePhoto.cy, maskDC, 0, 0, SRCPAINT);

	//����ת�ڰ��ɰ��뱳��ͼ����OR��������ǰ��λ����������Ϊ��ɫ������������ɫ���䡣
	//��ͷ��λ��������Ϊ��ɫ
	BitBlt(hdcBG, posPhoto.x, posPhoto.y, sizePhoto.cx, sizePhoto.cy, InvertMaskDC, 1, 0, SRCPAINT);
	//��������ͷ���뱳��ͼ����AND�������ﵽ����ͼ͸����Ч����
	BitBlt(hdcBG, posPhoto.x, posPhoto.y, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, SRCAND);

	DeleteObject(hPhoto);
	DeleteObject(hMaskBmp);
	DeleteObject(hMaskBmp2);
	DeleteObject(hGB);

	LOGFONT lf;
	HFONT my_font;
	SetBkMode (hdcBG, TRANSPARENT);
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfCharSet = GB2312_CHARSET;
	wcscpy_s(lf.lfFaceName, L"����");
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_MEDIUM;
	lf.lfHeight = -FontHeight;
	my_font = CreateFontIndirect(&lf);
	SelectObject(hdcBG, my_font);
	//�����ı�
	TextOut (hdcBG, posName.x, posName.y, L"�߷�", 2) ;

	DeleteObject(my_font);

	ReleaseDC (hWnd, hdc) ;
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

	DrawIDPicture(hWnd);

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

		//��ʾ���ս��
 		BitBlt(hdc, 0, 0, sizeBG.cx, sizeBG.cy, hdcBG, 0, 0, SRCCOPY);
		//BitBlt(hdc, 0, 0, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, SRCCOPY);

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
