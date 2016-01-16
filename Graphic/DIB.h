#pragma once
#include <windows.h>
#include <string>

using namespace std;

class CDIB
{
private:
	int m_nWidth;		// ͼ��Ŀ�ȣ���λ������
	int m_nHeight;		// ͼ��ĸ߶ȣ���λ������
	int m_nBitCount;	// ͼ���б�ʾÿ�������õ�λ��
	BITMAPINFOHEADER* m_pbmih;
	BYTE* m_lpPalette;	// ��ɫ��ָ��
	DWORD m_PaletterSize; // ��ɫ���С
	BYTE* m_lpBits;		// ͼ������ָ��
public:
	CDIB(void);
	~CDIB(void);

	// ��λͼ�ļ��ж�ȡλͼ���ݣ������ļ�����
	BOOL LoadFromFile(LPCTSTR filename);
	BOOL LoadFromMem(BYTE* data_buf, int data_len);
	HBITMAP GetBitmapObject(HDC hdc);
	int GetWidth();
	int GetBytesPerLine();
	int GetHeight();
	int GetBitCount();
	bool GetBits(BYTE* data_buf, int data_len);
	int Stretch(HDC hDC, int XDest,int YDest,int nDestWidth,int nDestHeight, int XSrc,int YSrc,int nSrcWidth,int nSrcHeight, UINT iUsage,DWORD dwRop);
	BOOL Copy();
	BOOL Gray();
	BOOL HFlip();
	BOOL VFlip();
	BOOL Rotate();
	BOOL SaveToDib(BYTE *lpDib);
	BOOL SaveToFile(LPCTSTR filename);

	static int ReadWholeFileContent(const string file_path, BYTE** buf_ptr);
};

