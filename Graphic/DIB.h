#pragma once
#include <windows.h>

class CDIB
{
private:
	int m_nWidth;		// ͼ��Ŀ�ȣ���λ������
	int m_nHeight;		// ͼ��ĸ߶ȣ���λ������
	int m_nBitCount;	// ͼ���б�ʾÿ�������õ�λ��
	BITMAPINFOHEADER* m_pbmih;
	BYTE *m_lpPalette;	// ��ɫ��ָ��
	DWORD m_PaletterSize; // ��ɫ���С
	BYTE *m_lpBits;		// ͼ������ָ��
public:
	CDIB(void);
	~CDIB(void);

	// ��λͼ�ļ��ж�ȡλͼ���ݣ������ļ�����
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

