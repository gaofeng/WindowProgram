#pragma once
#include <windows.h>

class CDIB
{
private:
	int m_nWidth;		// 图象的宽度，单位是象素
	int m_nHeight;		// 图象的高度，单位是象素
	int m_nBitCount;	// 图像中表示每像素所用的位数
	BITMAPINFOHEADER* m_pbmih;
	BYTE *m_lpPalette;	// 调色板指针
	DWORD m_PaletterSize; // 调色板大小
	BYTE *m_lpBits;		// 图象数据指针
public:
	CDIB(void);
	~CDIB(void);

	// 从位图文件中读取位图数据（根据文件名）
	BOOL LoadFromFile(LPCTSTR filename);
	HBITMAP GetBitmapObject(HDC hdc);
	int Stretch(HDC hDC, int XDest,int YDest,int nDestWidth,int nDestHeight, int XSrc,int YSrc,int nSrcWidth,int nSrcHeight, UINT iUsage,DWORD dwRop);
	BOOL Copy();
	BOOL Grey();
	BOOL HFlip();
	BOOL VFlip();
	BOOL Rotate();
	BOOL SaveToDib(BYTE *lpDib);
	BOOL SaveToFile(LPCTSTR filename);
};

