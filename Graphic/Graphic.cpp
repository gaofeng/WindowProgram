// Graphic.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Graphic.h"
#include "dibfile.h"
#include "DIB.h"
#include <windows.h>
#include <string>

using namespace std;

#define WND_WIDTH 800
#define WND_HEIGHT 600

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

HBITMAP CreateDibSectionFromDibFile (PTSTR szFileName)
{
	BITMAPFILEHEADER bmfh ;
	BITMAPINFO     * pbmi ;
	BYTE           * pBits ;
	BOOL             bSuccess ;
	DWORD            dwInfoSize, dwBytesRead ;
	HANDLE           hFile ;
	HBITMAP          hBitmap ;
	// Open the file: read access, prohibit write access

	hFile = CreateFile (szFileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
		return NULL ;

	// Read in the BITMAPFILEHEADER

	bSuccess = ReadFile (hFile, &bmfh, sizeof (BITMAPFILEHEADER), 
		&dwBytesRead, NULL) ;

	if (!bSuccess || (dwBytesRead != sizeof (BITMAPFILEHEADER))         
		|| (bmfh.bfType != * (WORD *) "BM"))
	{
		CloseHandle (hFile) ;
		return NULL ;
	}

	// Allocate memory for the BITMAPINFO structure & read it in

	dwInfoSize = bmfh.bfOffBits - sizeof (BITMAPFILEHEADER) ;

	pbmi = (BITMAPINFO*)malloc (dwInfoSize) ;

	bSuccess = ReadFile (hFile, pbmi, dwInfoSize, &dwBytesRead, NULL) ;

	if (!bSuccess || (dwBytesRead != dwInfoSize))
	{
		free (pbmi) ;
		CloseHandle (hFile) ;
		return NULL ;
	}
	// Create the DIB Section

	hBitmap = CreateDIBSection (NULL, pbmi, DIB_RGB_COLORS, (VOID**)&pBits, NULL, 0) ;

	if (hBitmap == NULL)
	{
		free (pbmi) ;
		CloseHandle (hFile) ;
		return NULL ;
	}

	// Read in the bitmap bits

	ReadFile (hFile, pBits, bmfh.bfSize - bmfh.bfOffBits, &dwBytesRead, NULL) ;

	free (pbmi) ;
	CloseHandle (hFile) ;

	return hBitmap ;
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

HWND hStatic;

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

	BITMAP bg_bitmap;

	HBITMAP hBGOrig;
	HBITMAP hBitmap = LoadBitmap (hInstance, TEXT ("Bricks")) ;
	hBGOrig = (HBITMAP)LoadImage(NULL, "D:\\htc-desire.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(hBGOrig, sizeof(bg_bitmap), &bg_bitmap);
	sizeBG.cx = bg_bitmap.bmWidth;
	sizeBG.cy = bg_bitmap.bmHeight;
	//SelectObject(hdcBG, (HGDIOBJ)hBGOrig);
	
	static BITMAP bitmap    = { 0, 20, 5, 4, 1, 1 } ;
	static BYTE  bits [] = { 0x51, 0x77, 0x10, 0x00,
		0x57, 0x77, 0x50, 0x00,
		0x13, 0x77, 0x50, 0x00,
		0x57, 0x77, 0x50, 0x00,
		0x51, 0x11, 0x10, 0x00 } ;
	bitmap.bmBits = (PSTR) bits ;
	hBitmap = CreateBitmapIndirect (&bitmap) ;
	SelectObject(hdcBG, (HGDIOBJ)hBitmap);
	ReleaseDC (hWnd, hdc) ;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

TCHAR szAppName[] = TEXT ("ShowDib1") ;

CDIB dib_obj;

BOOL WriteBitmapToBmpFile(string dest_path, BYTE* bits_buf, int buf_len, 
	int width, int height, int bitCount )
{
	DWORD dwPaletteSize = 0;

	switch (bitCount)
	{
	case 1:
		dwPaletteSize = 2;
		break;
	case 8:
		dwPaletteSize = 256;
		break;
	case 24:
		break;
	default:
		//Not going to work!
		return FALSE;
	}

	BITMAPINFOHEADER bmih;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1;
	bmih.biBitCount = (unsigned short)bitCount;
	bmih.biCompression = BI_RGB;
	bmih.biSizeImage = buf_len;
	bmih.biXPelsPerMeter = 0;
	bmih.biYPelsPerMeter = 0;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;

	BITMAPFILEHEADER bmfh = { 0 };
	bmfh.bfType = ((WORD)('M' << 8) | 'B');
	bmfh.bfSize = buf_len + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + (sizeof(RGBQUAD)*dwPaletteSize);
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*dwPaletteSize);

	HANDLE hFile;

	hFile = CreateFile(dest_path.c_str(), GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwBytesWritten;
	BOOL   bSuccess;
	bSuccess = WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	bSuccess = WriteFile(hFile, &bmih, bmih.biSize, &dwBytesWritten, NULL);

	BYTE (*PaletteArray)[sizeof(RGBQUAD)] = new BYTE[dwPaletteSize][sizeof(RGBQUAD)];
	for (DWORD i = 0; i < dwPaletteSize; i++)
	{
		memset(PaletteArray[i], i, 3);
		PaletteArray[i][3] = 0x00;
	}
	bSuccess = WriteFile(hFile, PaletteArray, sizeof(RGBQUAD)*dwPaletteSize, &dwBytesWritten, NULL);
// 	BitmapFlipVertical(bits_buf, width, height, bitCount);

	bSuccess = WriteFile(hFile, bits_buf, buf_len, &dwBytesWritten, NULL);
	CloseHandle(hFile);
	return TRUE;
}

void HBITMAPToFile(HDC hdc, HBITMAP hBitmap)
{
	BITMAPINFO* bif = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	memset(bif, 0x00, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	for (int i = 0; i < 256; i++)
	{
		bif->bmiColors[i].rgbBlue = i;
		bif->bmiColors[i].rgbGreen = i;
		bif->bmiColors[i].rgbRed = i;
		bif->bmiColors[i].rgbReserved = 0x00;
	}
	BITMAP bitmap;
	::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	bif->bmiHeader.biSize = sizeof(bif->bmiHeader);
	::GetDIBits(hdc, hBitmap, 0, bitmap.bmHeight, NULL, bif, DIB_RGB_COLORS);
	bif->bmiHeader.biBitCount = (WORD)8;
	bif->bmiHeader.biCompression = BI_RGB;
	bif->bmiHeader.biClrUsed = 256;
	bif->bmiHeader.biClrImportant = 256;
	BYTE* dst_data = (BYTE*)malloc(1000000);
	int scan_lines = ::GetDIBits(hdc, hBitmap, 0, bitmap.bmHeight, dst_data, bif, DIB_RGB_COLORS);
	WriteBitmapToBmpFile("D:\\small_test.bmp", dst_data, bif->bmiHeader.biSizeImage, bif->bmiHeader.biWidth, bif->bmiHeader.biHeight, 8);
	free(bif);
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
// 	COLORREF transcolor,WhiteColor;
	TEXTMETRIC tm ;
	int cx_screen, cy_screen;
	static HBITMAP hBitmap ;
	BITMAP bitmap ;
	HDC hdcMem ;
	static BITMAPFILEHEADER * pbmfh ;
	static BITMAPINFO       * pbmi ;
	static BYTE             * pBits ;
	static int                cxClient, cyClient, cxDib, cyDib ;
	static TCHAR              szFileName [MAX_PATH], szTitleName [MAX_PATH] ;
	static HDC hdcMem2;
	int dest_width = 328;
	int dest_height = 356;
	static HBITMAP hBitmap2;

	switch (message)
	{
	case WM_CREATE:
		hdc = GetDC(hWnd) ;
		GetTextMetrics (hdc, &tm) ;
		cxChar = tm.tmAveCharWidth ;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		cx_screen = GetSystemMetrics(SM_CXSCREEN);
		cy_screen = GetSystemMetrics(SM_CYSCREEN);

		dib_obj.LoadFromFile("D:\\cat.bmp");
		hBitmap = dib_obj.GetBitmapObject(hdc);

// 		hStatic = CreateWindow(L"STATIC", NULL, WS_BORDER | WS_VISIBLE | WS_CHILD | SS_BITMAP,
// 			100, 100, 200, 200, hWnd, (HMENU)10000, hInst, NULL);
// 		SendMessage(hStatic, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

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
		case ID_FILE_OPEN:
			// Show the File Open dialog box
			if (!DibFileOpenDlg (hWnd, szFileName, szTitleName))
				return 0 ;

			SetCursor (LoadCursor (NULL, IDC_WAIT)) ;
			ShowCursor (TRUE) ;

			dib_obj.LoadFromFile(szFileName);
			ShowCursor (FALSE) ;
			SetCursor (LoadCursor (NULL, IDC_ARROW));

			// Invalidate the client area for later update
			InvalidateRect (hWnd, NULL, TRUE) ;

			return 0 ;
			break;
		case ID_FILE_SAVE:
// 			HBITMAPToFile(hdcMem2, hBitmap2);
// 			if (!DibFileOpenDlg (hWnd, szFileName, szTitleName))
// 				return 0 ;
			strcpy(szFileName, "D:\\Graphic.bmp");
			if (dib_obj.SaveToFile(szFileName) == TRUE)
			{
				MessageBox(hWnd, "保存成功", "恭喜", MB_OK);
			}
			break;
		case ID_IMAGE_HFLIP:
			dib_obj.HFlip();
			InvalidateRect (hWnd, NULL, TRUE) ;
			break;
		case ID_IMAGE_VFLIP:
			dib_obj.VFlip();
			InvalidateRect (hWnd, NULL, TRUE) ;
			break;
		case ID_IMAGE_ROTATE:
			dib_obj.Rotate();
			InvalidateRect (hWnd, NULL, TRUE) ;
			break;
		case ID_IMAGE_COPY:
			dib_obj.Copy();
		case ID_IMAGE_GRAY:
			dib_obj.Grey();
			InvalidateRect (hWnd, NULL, TRUE) ;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//显示最终结果
// 		BitBlt(hdc, 0, 0, 20, 20, hdcBG, 0, 0, SRCCOPY);
// 		StretchBlt(hdc, 0, 0, 200, 50, hdcBG, 0, 0, 20, 5, SRCCOPY);
		//BitBlt(hdc, 0, 0, sizePhoto.cx, sizePhoto.cy, hdcPhoto, 0, 0, SRCCOPY);
// 		TextOut (hdc, 0, 0, L"Hello World", 11) ;
// 		TextOut (hdc, 0, cyChar, L"My name is Jim", 14) ;

// 		if (pbmfh)
// 		{
// 			SetDIBitsToDevice (hdc, 
// 			0,         // xDst
// 			0,         // yDst
// 			cxDib,     // cxSrc
// 			cyDib,     // cySrc
// 			0,         // xSrc
// 			0,         // ySrc
// 			0,         // first scan line
// 			cyDib,     // number of scan lines
// 			pBits, 
// 			pbmi, 
// 			DIB_RGB_COLORS);
// 		}
		hBitmap = dib_obj.GetBitmapObject(hdc) ;
		if (hBitmap)
		{
			GetObject(hBitmap, sizeof (BITMAP), &bitmap);

			hdcMem = CreateCompatibleDC(hdc);
			SelectObject (hdcMem, hBitmap);

			hdcMem2 = ::CreateCompatibleDC(hdc);

			hBitmap2 = ::CreateCompatibleBitmap(hdc, dest_width, dest_height);
			::SelectObject(hdcMem2, hBitmap2);
			::SetStretchBltMode(hdcMem2, HALFTONE);//HALFTONE,COLORONCOLOR
			SetBrushOrgEx(hdcMem2, 0, 0, NULL);
			ret = ::StretchBlt(hdcMem2, 0, 0, dest_width, dest_height, 
				hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

			BitBlt(hdc, 0, 0, dest_width, dest_height, 
				hdcMem2, 0, 0, SRCCOPY);
			BitBlt(hdc, 300, 0, bitmap.bmWidth, bitmap.bmHeight, 
				hdcMem, 0, 0, SRCCOPY);

			DeleteDC(hdcMem);
		}
// 		dib_obj.Stretch(hdc, 100, 100, 0, 0, 0, 0, 0, 0, DIB_RGB_COLORS, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		if (pbmfh)
			free (pbmfh);
		if (hBitmap)
			DeleteObject (hBitmap);
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
