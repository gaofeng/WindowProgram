#include <Windows.h>
#include <tchar.h>

#include <CommCtrl.h>

//http://www.codeproject.com/Articles/227831/A-dialog-based-Win32-C-program

/* usually, the resource editor creates this file to us: */
#include "resource.h"

#pragma comment(linker, \
	"\"/manifestdependency:type='Win32' "\
	"name='Microsoft.Windows.Common-Controls' "\
	"version='6.0.0.0' "\
	"processorArchitecture='*' "\
	"publicKeyToken='6595b64144ccf1df' "\
	"language='*'\"")

#pragma comment(lib, "ComCtl32.lib")

HINSTANCE g_hinst;

BOOL CenterWindow(HWND hwndWindow)
{
	HWND hwndParent;
	RECT rectWindow, rectParent;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	GetWindowRect(hwndWindow, &rectWindow);

	int nWidth = rectWindow.right - rectWindow.left;
	int nHeight = rectWindow.bottom - rectWindow.top;

	int nX, nY;


	// make the window relative to its parent
	if ((hwndParent = GetParent(hwndWindow)) != NULL)
	{
		GetWindowRect(hwndParent, &rectParent);

		nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
		nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

		// make sure that the dialog box never moves outside of the screen
		if (nX < 0) nX = 0;
		if (nY < 0) nY = 0;
		if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
		if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;
	}
	else
	{
		nX = (nScreenWidth - nWidth) / 2;
		nY = (nScreenHeight - nHeight) / 2;
	}
	MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

	return TRUE;
}

INT_PTR CALLBACK DialogProc2(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CenterWindow(hDlg);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;
		case IDOK:
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND dialog;

	INT_PTR ret;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		CenterWindow(hDlg);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;
		case IDC_BTN1:
			ret = DialogBox(g_hinst, MAKEINTRESOURCE(IDD_DIALOG2), hDlg, DialogProc2);
			return TRUE;
		}
		break;

	case WM_CLOSE:
		if(MessageBox(hDlg, TEXT("Close the program?"), TEXT("Close"),
			MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			DestroyWindow(hDlg);
		}
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	}

	return FALSE;
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
	HWND hDlg;
	MSG msg;
	BOOL ret;

	g_hinst = hInst;

	InitCommonControls();
	hDlg = CreateDialogParam(g_hinst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc, 0);
	ShowWindow(hDlg, nCmdShow);

	while((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if(ret == -1)
			return -1;

		if(!IsDialogMessage(hDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}