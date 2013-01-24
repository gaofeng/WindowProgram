// Graphic.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Graphic.h"

#define WND_WIDTH 1100
#define WND_HEIGHT 800


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

static SIZE sizeBG;

static TCHAR Name[32];//Max 30
static TCHAR Gender[8];
static TCHAR Nationality[16];
static TCHAR Year[8];
static TCHAR Month[8];
static TCHAR Day[8];
static TCHAR Address[72];//MAX 70
static TCHAR IDNum[20]; //18

/*照片位置*/
static SIZE sizePhoto;
static POINT posPhoto;
static POINT posName;
static POINT posGender;
static POINT posNationality;
static POINT posBirthdayYear;
static POINT posBirthdayMonth;
static POINT posBirthdayDay;
static POINT posAddress;
static POINT posIDNum;

static int FontHeight;
static int AddressMaxLen;

static HDC hdcPhoto;
static HDC maskDC;
static HDC InvertMaskDC;
static HDC hdcBG;

void CalcElementsPos(SIZE bg)
{
	sizePhoto.cx = (LONG)(bg.cx * 26 / 85.6);
	sizePhoto.cy = bg.cy * 32 / 54;
	posPhoto.x = (LONG)(bg.cx * 0.63);
	posPhoto.y = (LONG)(bg.cy * 0.115);

	//字体大小
	FontHeight = (LONG)(sizeBG.cy * 0.07);
	//姓名位置
	posName.x = (LONG)(sizeBG.cx * 0.2);
	posName.y = (LONG)(sizeBG.cy * 0.145);
	//性别位置
	posGender.x = (LONG)(sizeBG.cx * 0.2);
	posGender.y = (LONG)(sizeBG.cy * 0.265);
	//民族位置
	posNationality.x = (LONG)(sizeBG.cx * 0.4);
	posNationality.y = (LONG)(sizeBG.cy * 0.265);

	//生日位置
	posBirthdayYear.x = (LONG)(sizeBG.cx * 0.2);
	posBirthdayYear.y = (LONG)(sizeBG.cy * 0.39);
	posBirthdayMonth.x = (LONG)(sizeBG.cx * 0.345);
	posBirthdayMonth.y = (LONG)(sizeBG.cy * 0.39);
	posBirthdayDay.x = (LONG)(sizeBG.cx * 0.425);
	posBirthdayDay.y = (LONG)(sizeBG.cy * 0.39);

	//地址位置
	posAddress.x = (LONG)(sizeBG.cx * 0.2);
	posAddress.y = (LONG)(sizeBG.cy * 0.518);
	AddressMaxLen = (LONG)(sizeBG.cx * 0.42);

	//身份证号码地址
	posIDNum.x = (LONG)(sizeBG.cx * 0.345);
	posIDNum.y = (LONG)(sizeBG.cy * 0.825);
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

	BITMAP bitmap;

	//读取背景图片
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

	//计算尺寸
	CalcElementsPos(sizeBG);

	HBITMAP hPhoto;
	hPhoto = (HBITMAP)LoadImage(NULL, L"D:\\370102198502152134.bmp", IMAGE_BITMAP, sizePhoto.cx, sizePhoto.cy, LR_LOADFROMFILE);
	//获取长和高

	SelectObject(hdcPhoto, (HGDIOBJ)hPhoto);

	//创建蒙板
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
	BitBlt(hdcBG, posPhoto.x, posPhoto.y, sizePhoto.cx, sizePhoto.cy, InvertMaskDC, 1, 0, SRCPAINT);
	//将处理后的头像与背景图进行AND操作，达到背景图透明的效果。
	BitBlt(hdcBG, posPhoto.x, posPhoto.y, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, SRCAND);

	DeleteObject(hPhoto);
	DeleteObject(hMaskBmp);
	DeleteObject(hMaskBmp2);
	DeleteObject(hGB);

	LOGFONT lf;
	HFONT largeFont;
	HFONT smallFont;
	SetBkMode (hdcBG, TRANSPARENT);
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfCharSet = GB2312_CHARSET;
	wcscpy_s(lf.lfFaceName, L"黑体");
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_BLACK;
	lf.lfHeight = -FontHeight;
	largeFont = CreateFontIndirect(&lf);
	lf.lfHeight = -FontHeight + 7;
	smallFont = CreateFontIndirect(&lf);

	wcscpy_s(Name, L"高峰");
	wcscpy_s(Gender, L"男");
	wcscpy_s(Nationality, L"汉");
	wcscpy_s(Year, L"1985");
	wcscpy_s(Month, L"2");
	wcscpy_s(Day, L"15");
	wcscpy_s(Address, L"济南市天桥区板桥庄93-1号");
	wcscpy_s(IDNum, L"370102198502152134");

	//绘制文本
	SelectObject(hdcBG, largeFont);
	TextOut (hdcBG, posName.x, posName.y, Name, wcslen(Name));
	SelectObject(hdcBG, smallFont);
	TextOut (hdcBG, posGender.x, posGender.y, Gender, wcslen(Gender)) ;
	TextOut (hdcBG, posNationality.x, posNationality.y, Nationality, wcslen(Nationality)) ;
	TextOut (hdcBG, posBirthdayYear.x, posBirthdayYear.y, Year, wcslen(Year)) ;
	TextOut (hdcBG, posBirthdayMonth.x, posBirthdayMonth.y, Month, wcslen(Month)) ;
	TextOut (hdcBG, posBirthdayDay.x, posBirthdayDay.y, Day, wcslen(Day)) ;
	//判断换回
	int offset = 0;
	int address_len = wcslen(Address);
	TCHAR* pAddr = Address;
	SIZE str_size;
	while (offset < address_len)
	{
		GetTextExtentPoint32(hdcBG, pAddr, offset, &str_size);
		if (str_size.cx > AddressMaxLen)
		{
			TextOut (hdcBG, posAddress.x, posAddress.y, pAddr, offset) ;
			posAddress.y += (LONG)(FontHeight * 1.18);
			pAddr += offset;
			address_len -= offset;
			offset = 0;
		}
		offset++;
	}
	TextOut (hdcBG, posAddress.x, posAddress.y, pAddr, address_len) ;


	SelectObject(hdcBG, largeFont);
	TextOut (hdcBG, posIDNum.x, posIDNum.y, IDNum, wcslen(IDNum)) ;

	DeleteObject(smallFont);
	DeleteObject(largeFont);

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

		//显示最终结果
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
