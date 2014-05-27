#include "StdAfx.h"
#include <string.h>
#include "DIB.h"
#include "ScopeGuard.hpp"

// ����ͼ��ÿ��������ռ���ֽ���Ŀ 
#define BYTE_PER_LINE(width, bitCount) ((((width)*(bitCount)+31)/32)*4)

CDIB::CDIB(void)
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBitCount = 0;
	m_pbmih = NULL;
	m_lpPalette = NULL;
	m_PaletterSize = 0;
	m_lpBits = NULL;
}


CDIB::~CDIB(void)
{
	if (m_pbmih)
	{
		free(m_pbmih);
	}
	if (m_lpPalette)
	{
		free(m_lpPalette);
	}
	if (m_lpBits)
	{
		free(m_lpBits);
	}
}

int myFileSeek(HANDLE hf, int distance, DWORD MoveMethod)
{
	LARGE_INTEGER li;

	li.QuadPart = distance;

	li.LowPart = SetFilePointer (hf, 
		li.LowPart, 
		&li.HighPart, 
		MoveMethod);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() 
		!= NO_ERROR)
	{
		li.QuadPart = -1;
	}

	return (int)li.QuadPart;
}

BOOL CDIB::LoadFromFile( LPCTSTR filename )
{
	BOOL bSuccess = FALSE;
	DWORD dwFileSize = 0;
	DWORD dwHighSize = 0;
	DWORD dwBytesRead = 0;
	HANDLE hFile = NULL;
	int nWidth,nHeight,nBitCount,nByteWidth;

	//���Ѵ����ļ�
	hFile = CreateFile (filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	ON_SCOPE_EXIT([&]{CloseHandle(hFile);});
	//��ȡ�ļ���С
	dwFileSize = GetFileSize(hFile, &dwHighSize) ;
	if (dwHighSize)
	{
		return FALSE;
	}
	BITMAPFILEHEADER pbmfh;
	bSuccess = ReadFile (hFile, &pbmfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);

	if (!bSuccess || (dwBytesRead != sizeof(BITMAPFILEHEADER))         
		|| (pbmfh.bfType != * (WORD *) "BM") 
		|| (pbmfh.bfSize != dwFileSize))
	{
		return FALSE;
	}
	m_pbmih = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	bSuccess = ReadFile(hFile, m_pbmih, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
	if (!bSuccess || (dwBytesRead != sizeof(BITMAPINFOHEADER)))
	{
		return FALSE;
	}
	nWidth = m_pbmih->biWidth;
	nHeight = m_pbmih->biHeight;
	nBitCount = m_pbmih->biBitCount;
	nByteWidth = BYTE_PER_LINE(nWidth, nBitCount);

	// �ͷ���ǰ��λͼ���ݺ͵�ɫ������ռ�е��ڴ�
	if (m_lpBits) 
		free(m_lpBits);
	m_lpBits = NULL;
	if (m_lpPalette) 
		free(m_lpPalette);
	m_lpPalette = NULL;

	//�ƶ�ָ��
	myFileSeek(hFile, pbmfh.bfOffBits, FILE_BEGIN);
	// ����Ϊλͼ�������ݷ����ڴ�
	m_lpBits= (BYTE*)malloc(nByteWidth * nHeight);
	if (!m_lpBits)
	{
		return FALSE;
	}
	memset(m_lpBits, 0x00, nByteWidth * nHeight);
	bSuccess = ReadFile(hFile, m_lpBits, nByteWidth * nHeight, &dwBytesRead, NULL);

	if (!bSuccess || (dwBytesRead != (nByteWidth * nHeight)))
	{
		free(m_lpBits);
		m_lpBits = NULL;
		return FALSE;
	}

	// ���λͼ����ɫ��
	switch (nBitCount)
	{
		// �����24λ��ɫ��λͼ��û�е�ɫ����Ϣ
	case 24:
		m_PaletterSize = 0;
		break;
		// �����8λ����4λ��ɫ��λͼ���е�ɫ����Ϣ
	case 8:
	case 4:
		// �����ɫ����ʵ����ɫ����
		m_PaletterSize = (1 << m_pbmih->biBitCount);
		// ���pInfo->biClrUsed������0��
		// ʹ��pInfo->biClrUsedָ����λͼʵ��ʹ�õ���ɫ��
		if ((m_pbmih->biClrUsed != 0) && (m_pbmih->biClrUsed < m_PaletterSize))
		{
			m_PaletterSize = m_pbmih->biClrUsed;
		}
		// �ƶ�ָ�뵽��ɫ��λ��
		myFileSeek(hFile, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), FILE_BEGIN);

		// Ϊ�����ɫ����Ϣ���ݵ�m_lpPalette����ռ�
		m_lpPalette = (BYTE*)malloc(sizeof(RGBQUAD) * m_PaletterSize);
		bSuccess = ReadFile(hFile, m_lpPalette, sizeof(RGBQUAD) * m_PaletterSize, &dwBytesRead, NULL);
		if (!bSuccess || (dwBytesRead != (sizeof(RGBQUAD) * m_PaletterSize)))
		{
			free(m_lpPalette);
			m_lpPalette = NULL;
			free(m_lpBits);
			m_lpBits = NULL;
			return FALSE;
		}
		break;
		// ������������账��
	default:
		free(m_lpBits);
		m_lpBits = NULL;
		return FALSE;
	}

	// ��¼λͼ�Ŀ��
	m_nBitCount = nBitCount;
	// ��¼λͼ�ĸ߶�
	m_nWidth = nWidth;
	// ��¼λͼ��ʾ��ɫ���õ�λ��
	m_nHeight = nHeight;

	return TRUE;
}

// ��λͼ���ݱ��浽λͼ�ļ������ݴ򿪵��ļ�ָ�룩
BOOL CDIB::SaveToFile(LPCTSTR filename)
{
	DWORD  dwBytesWritten;

	// ���ͼ������Ϊ�գ�����
	if (m_lpBits == NULL) 
		return FALSE;

	HANDLE hFile = CreateFile (filename, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE ;
		}
	// ��ȡͼ���ļ���ÿ��ͼ����ռ�ֽ���
	int nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// ���λͼ�ļ�ͷ�ṹ��ָ���ļ������Ϣ
	BITMAPFILEHEADER bmfh;

	// ָ���ļ�����Ϊλͼ
	bmfh.bfType = 'M'*256+'B';
	// ָ��λͼ�ļ��Ĵ�С
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 
		m_PaletterSize * sizeof(RGBQUAD) + nByteWidth * m_nHeight;
	// �����ĽṹԪ�أ�����Ϊ0
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	// ������ļ�ͷ��ʼ��ʵ�ʵ�ͼ������֮���ƫ�������ֽ�����
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) + m_PaletterSize * sizeof(RGBQUAD);

	// ���ļ���д��λͼ�ļ�ͷ��Ϣ
	WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);

	// ���λͼ��Ϣͷ�ṹ��ָ��λͼ�Ĵ�С����ɫ��Ϣ
	BITMAPINFOHEADER bmi;
	// ָ��λͼ��Ϣͷ�ṹ�Ĵ�С
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	// ָ��λͼ�Ŀ��
	bmi.biWidth = m_nWidth;
	// ָ��λͼ�ĸ߶�
	bmi.biHeight = m_nHeight;
	// Ŀ���豸��λ��������ֵ����Ϊ1
	bmi.biPlanes = 1;
	// ָ����ʾ��ɫʱ�õ���λ����
	// ���õ�ֵΪ1(�ڰ׶�ɫͼ)��4(16ɫͼ)��8(256ɫͼ)��24(���ɫͼ)
	bmi.biBitCount = m_nBitCount;
	// ˵��û��ѹ��ͼ������
	bmi.biCompression = BI_RGB;
	// ָ��ʵ�ʵ�λͼ����ռ�õ��ֽ�����
	// ����BI_RGB��ʽʱ��������Ϊ0 
	bmi.biSizeImage = nByteWidth * m_nHeight;
	// ָ��Ŀ���豸��ˮƽ�ֱ��ʣ�������/�ױ�ʾ
	bmi.biXPelsPerMeter = 0;
	// ָ��Ŀ���豸�Ĵ�ֱ�ֱ��ʣ�������/�ױ�ʾ
	bmi.biYPelsPerMeter = 0;
	// ָ��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ���
	bmi.biClrUsed = 0;
	// ָ����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��
	bmi.biClrImportant = 0;
	// ��λͼ��Ϣͷд���ļ�
	WriteFile(hFile, &bmi,sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);

	// �������24λ���ɫλͼ������ɫ����Ϣд���ļ�
	if (m_nBitCount != 24)
		WriteFile(hFile, m_lpPalette, m_PaletterSize * sizeof(RGBQUAD), &dwBytesWritten, NULL);

	// ��λͼ����д���ļ�
	WriteFile(hFile, m_lpBits, nByteWidth*m_nHeight, &dwBytesWritten, NULL);

	CloseHandle(hFile);
	return TRUE;
}

/*�õ���BITMAP�����������ͷ�*/
HBITMAP CDIB::GetBitmapObject(HDC hdc)
{
	HBITMAP hBitmap = NULL;
	hBitmap = CreateCompatibleBitmap(hdc, m_nWidth, m_nHeight);
	BITMAPINFO* pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * m_PaletterSize);
	pbmi->bmiHeader = *m_pbmih;
	memcpy(pbmi->bmiColors, m_lpPalette, sizeof(RGBQUAD) * m_PaletterSize);
	SetDIBits(hdc, hBitmap, 0, m_nHeight, m_lpBits, pbmi, DIB_RGB_COLORS);
	free(pbmi);
	return hBitmap;
}

int CDIB::Stretch(HDC hDC,
				  int XDest,int YDest,int nDestWidth,int nDestHeight,
				  int XSrc,int YSrc,int nSrcWidth,int nSrcHeight,
				  UINT iUsage,DWORD dwRop)
{
	// λͼ��Ϣ�ṹ������λͼ��Ϣͷ�͵�ɫ����Ϣ��
	// ����λͼ�Ĵ�С����ɫ���
	BITMAPINFO *pbmi;

	// ����λͼ������ռ�ڴ�ռ�
	pbmi=(BITMAPINFO *)new BYTE[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*m_PaletterSize];

	// ����ɫ����Ϣ���Ƶ�λͼ��Ϣ�ṹ��
	memcpy (pbmi->bmiColors, m_lpPalette, sizeof(RGBQUAD)*m_PaletterSize);
	//  ָ��λͼ��Ϣͷ�ṹ�Ĵ�С
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// ָ��λͼ�Ŀ��
	pbmi->bmiHeader.biWidth = m_nWidth;
	// ָ��λͼ�ĸ߶�
	pbmi->bmiHeader.biHeight = m_nHeight;
	// Ŀ���豸��λ��������ֵ����Ϊ1	
	pbmi->bmiHeader.biPlanes = 1;
	// ָ����ʾ��ɫʱ�õ���λ��
	pbmi->bmiHeader.biBitCount = m_nBitCount;
	// ˵��û��ѹ��ͼ������ 
	pbmi->bmiHeader.biCompression = BI_RGB;
	// ָ��ʵ�ʵ�λͼ����ռ�õ��ֽ�����
	// ����BI_RGB��ʽʱ��������Ϊ0 
	pbmi->bmiHeader.biSizeImage = 0;
	// ָ��Ŀ���豸��ˮƽ�ֱ��ʣ�������/�ױ�ʾ 
	pbmi->bmiHeader.biXPelsPerMeter = 0;
	// ָ��Ŀ���豸�Ĵ�ֱ�ֱ��ʣ�������/�ױ�ʾ
	pbmi->bmiHeader.biYPelsPerMeter = 0;
	// ָ��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ���
	pbmi->bmiHeader.biClrUsed=0;
	// ָ����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ 
	pbmi->bmiHeader.biClrImportant=0;

	if (nSrcWidth == 0)
	{
		nSrcWidth = m_nWidth;
	}
	if (nSrcHeight == 0)
	{
		nSrcHeight = m_nHeight;
	}
	if (nDestWidth == 0)
	{
		nDestWidth = m_nWidth;
	}
	if (nDestHeight == 0)
	{
		nDestHeight = m_nHeight;
	}

	// ���û�λͼ��ģʽ
	SetStretchBltMode(hDC, COLORONCOLOR);

	// ��ָ����λ���ϰ���ָ���Ĵ�С����λͼ
	int ret = StretchDIBits(hDC, XDest, YDest, nDestWidth, nDestHeight,
		XSrc, YSrc, nSrcWidth, nSrcHeight, m_lpBits,
		pbmi, iUsage, dwRop);

	// ɾ����������ڴ�ռ�
	delete[] pbmi;

	return ret;
}

// ��CDib������ݽṹ�е�����ת��ΪDIBλͼ��ʽ����
BOOL CDIB::SaveToDib(BYTE *lpDib)
{
	// ���û��λͼ���ݣ�����
	if (m_lpBits == NULL) 
		return FALSE;

	// ��ȡλͼ����
	BYTE *p = lpDib;
	// ����λͼÿ��������ռ���ֽ���Ŀ
	int nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// ���λͼ��Ϣͷ�ṹ
	BITMAPINFOHEADER bmi;
	// ָ��λͼ��Ϣͷ�ṹ�Ĵ�С
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	// ָ��λͼ�Ŀ��
	bmi.biWidth = m_nWidth;
	// ָ��λͼ�ĸ߶�
	bmi.biHeight = m_nHeight;
	// Ŀ���豸��λ��������ֵ����Ϊ1
	bmi.biPlanes = 1;
	// ָ����ʾ��ɫʱ�õ���λ����
	// ���õ�ֵΪ1(�ڰ׶�ɫͼ)��4(16ɫͼ)��8(256ɫͼ)��24(���ɫͼ)
	bmi.biBitCount = m_nBitCount;
	// ˵��û��ѹ��ͼ������
	bmi.biCompression = BI_RGB;
	// ָ��ʵ�ʵ�λͼ����ռ�õ��ֽ�����
	// ����BI_RGB��ʽʱ��������Ϊ0 
	bmi.biSizeImage = 0;
	// ָ��Ŀ���豸��ˮƽ�ֱ��ʣ�������/�ױ�ʾ
	bmi.biXPelsPerMeter = 0;
	// ָ��Ŀ���豸�Ĵ�ֱ�ֱ��ʣ�������/�ױ�ʾ
	bmi.biYPelsPerMeter=0;
	// ָ��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ���
	bmi.biClrUsed=0;
	// ָ����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ 
	bmi.biClrImportant=0;
	// ��λͼ��Ϣͷ�ṹ�����ݸ��Ƶ�λͼ������
	memcpy(p, &bmi, sizeof(BITMAPINFOHEADER));

	// ��ָ���ƶ���λͼ��Ϣͷ�ṹ֮��
	p += sizeof(BITMAPINFOHEADER);
	// �������24λ���ɫλͼ
	if (m_nBitCount != 24)
	{
		// ����ɫ����Ϣ���ݸ��Ƶ�λͼ������
		memcpy(p, m_lpPalette, m_PaletterSize * sizeof(RGBQUAD));
		// ��ָ���ƶ�����ɫ����Ϣ����֮��
		p += m_PaletterSize * sizeof(RGBQUAD);
	}

	// ���������ݸ��Ƶ�λͼ������
	memcpy(p, m_lpBits, nByteWidth*m_nHeight);

	return TRUE;
}

// ��ͼ�����ݸ��Ƶ�������
BOOL CDIB::Copy()
{
	// ���û��λͼ�������ݣ�����
	if (m_lpBits == 0) 
		return FALSE;

	// ����޷��򿪼����壬����
	if (!OpenClipboard(NULL)) 
		return FALSE;

	// ��ռ����壬�ͷż����������ݵľ����
	// ʹ��ǰ�����öԼ�����Ŀ���
	EmptyClipboard();

	HGLOBAL hMem;
	BYTE *lpDib;
	// ����λͼ������ռ�е��ֽ�����λͼ��Ϣͷ�ṹ���������ݣ�
	int nLen = sizeof(BITMAPINFOHEADER)
		+ BYTE_PER_LINE(m_nWidth, m_nBitCount)*m_nHeight;
	// �������24λ���ɫλͼ����Ҫ���ϵ�ɫ����Ϣ���ݵĳ���
	if (m_nBitCount != 24)
		nLen += m_PaletterSize * sizeof(RGBQUAD);

	// Ϊλͼ���ݷ����ڴ�ռ�
	hMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, nLen);
	// ������������ڴ�ռ�
	lpDib = (BYTE *)GlobalLock(hMem);
	// ��CDib������ݽṹ�е�����ת��ΪDIBλͼ��ʽ����
	SaveToDib(lpDib);
	// �⿪���ڴ�ռ������
	GlobalUnlock(hMem);
	// �����ݰ���DIBλͼ��ʽ������������
	SetClipboardData(CF_DIB, hMem);
	// �رռ�����
	CloseClipboard();

	return TRUE;
}

// ��ͼ��ת��Ϊ�Ҷ�ͼ��
BOOL CDIB::Grey()
{
	// ���û��λͼ���ݣ�����
	if (!m_lpBits)
	{
		return FALSE;
	}
	int y, x, nByteWidth, grey;
	int i;
	BYTE *p = NULL;
	BYTE* gray_bits = NULL;

	// �����24λ���ɫ��λͼ
	if (m_nBitCount==24)
	{
		gray_bits = (BYTE*)malloc(m_nWidth * m_nHeight);
		//ת��Ϊ�ҽ�ͼ��
		// ����λͼÿ��������ռ���ֽ���Ŀ
		nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);
		// ����λͼ�е�ÿһ��
		for (y = 0; y < m_nHeight; y++)
		{
			// ����ÿһ���е�ÿһ������
			for (x = 0; x < m_nWidth; x++)
			{
				// �ҵ����������������������е�λ��
				// 24λ���ɫλͼ�У�ÿ������ռ3���ֽ�
				p = m_lpBits + nByteWidth * y + x * 3;
				// �޸����ص���ɫ��ʹ��ҶȻ�
				gray_bits[y * m_nWidth + x] = (BYTE)(0.299*p[2]+0.587*p[1]+0.114*p[0]);
			}
		}
		free(m_lpBits);
		m_lpBits = gray_bits;
		m_nBitCount = 8;
		m_pbmih->biBitCount = 8;
		m_PaletterSize = 256;
		m_lpPalette = (BYTE*)malloc(sizeof(RGBQUAD) * m_PaletterSize);
		for (i = 0; i < m_PaletterSize; i++)
		{
			m_lpPalette[i * 4 + 0] = i;
			m_lpPalette[i * 4 + 1] = i;
			m_lpPalette[i * 4 + 2] = i;
			m_lpPalette[i * 4 + 3] = 0;
		}
	}
	// �������24λ���ɫλͼ��Ҳ����8λ�ҽ�ͼ���޸ĵ�ɫ���е���ɫ
	else if (m_nBitCount != 8)
	{
		// ���ڵ�ɫ���е�ÿһ����ɫ
		for (x = 0; x < m_PaletterSize; x++)
		{
			// ���ÿһ����ɫ�������ڵ�λ��
			// ��ɫ����ÿ����ɫռ4���ֽ�
			p = m_lpPalette+x*4;

			// ����ɫ�ҶȻ�
			grey = (BYTE)(0.299*p[2]+0.587*p[1]+0.114*p[0]);
			p[0]=grey;
			p[1]=grey;
			p[2]=grey;
		}
	}
	return TRUE;
}

// ����תͼ��
BOOL CDIB::HFlip()
{
	// ���û��λͼ���ݣ�����
	if (!m_lpBits) 
		return FALSE;

	int y, x, nByteWidth, temp;
	BYTE *p1, *p2;

	// ����λͼÿ��������ռ���ֽ���Ŀ
	nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// ���λͼ����ɫλ������������i�к͵�����i�е�����
	switch (m_nBitCount)
	{
		// �����24λ���ɫλͼ��ÿ������ռ3���ֽ�
	case 24:
		for (y = 0; y < m_nHeight; y++)
		{
			for (x = 0; x < m_nWidth / 2; x++)
			{
				// ��y�еĵ�x�����ص�����ָ��
				p1 = m_lpBits + nByteWidth * y + x * 3;
				// ��y�еĵ�����x�����ص�����ָ��
				p2 = m_lpBits+nByteWidth*y+(m_nWidth-x-1)*3;
				// ����temp����p1�ĵ�1���ֽ���p2�ĵ�1���ֽڽ���
				temp = p1[0];
				p1[0] = p2[0];
				p2[0] = temp;
				// ����temp����p1�ĵ�2���ֽ���p2�ĵ�2���ֽڽ���
				temp = p1[1];
				p1[1] = p2[1];
				p2[1] = temp;
				// ����temp����p1�ĵ�3���ֽ���p2�ĵ�3���ֽڽ���
				temp = p1[2];
				p1[2] = p2[2];
				p2[2] = temp;
			}
		}
		break;
		// �����8λ��ɫλͼ��ÿ������ռһ���ֽ�
	case 8:
		for (y = 0; y < m_nHeight; y++)
		{
			for (x = 0; x < m_nWidth / 2; x++)
			{
				// ��y�еĵ�x�����ص�����ָ��
				p1 = m_lpBits+nByteWidth*y+x;
				// ��y�еĵ�����x�����ص�����ָ��
				p2 = m_lpBits+nByteWidth*y+(m_nWidth-x-1);
				// ����temp���������������ص�����
				temp = *p1;
				*p1 = *p2;
				*p2 = temp;
			}
		}
		break;
		// �����4λ��ɫλͼ������
	case 4:
		return FALSE;
	}
	return TRUE;
}

// ����תͼ��
BOOL CDIB::VFlip()
{
	// ���û��λͼ���ݣ�����
	if (!m_lpBits) 
		return FALSE;

	int y, nByteWidth;
	BYTE *p1, *p2, *pm;

	// ����λͼÿ��������ռ���ֽ���Ŀ
	nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// �����ܴ洢һ�����ص��ڴ�ռ�
	pm = new BYTE[nByteWidth];

	// ����y�������뵹��y�����ؽ��н���
	for (y = 0; y < m_nHeight / 2; y++)
	{
		// ȡ�õ�y�����ص�����
		p1 = m_lpBits + y * nByteWidth;
		// ȡ�õ�����y�����ص�����
		p2 = m_lpBits + (m_nHeight - y - 1) * nByteWidth;
		// ����pm��������y�������뵹��y�����ؽ��н���
		memcpy(pm, p1, nByteWidth);
		memcpy(p1, p2, nByteWidth);
		memcpy(p2, pm, nByteWidth);
	}

	// ɾ��������ڴ�ռ�
	delete[] pm;

	return TRUE;
}

// ��ͼ��˳ʱ����ת90��
BOOL CDIB::Rotate()
{
	// ���û��λͼ���ݣ�����
	if (m_lpBits == NULL) 
		return FALSE;

	BYTE *pbits, *p1, *p2, *pa, *pb, *pa1, *pb1;
	int w2, h2, bw2, bw1, x, y;

	// ��ȡ��תǰλͼ�ĸ߶�
	w2 = m_nHeight;	
	// ��ȡ��תǰλͼ�Ŀ��
	h2 = m_nWidth;

	// ������תǰλͼÿ��������ռ���ֽ���Ŀ
	bw1 = BYTE_PER_LINE(m_nWidth, m_nBitCount);
	// ������ת��λͼÿ��������ռ���ֽ���Ŀ
	bw2 = BYTE_PER_LINE(w2, m_nBitCount);

	// �����ڴ�ռ��Ա�����ת���λͼ
	pbits = new BYTE[bw2*h2];

	// ���λͼ����ɫλ��
	switch (m_nBitCount)
	{
		// �����24λ���ɫλͼ��ÿ������ռ�����ֽ�
	case 24:
		for (y=0; y<m_nHeight; y++)
			for (x=0; x<m_nWidth; x++)
			{
				// ȡ��y�еĵ�x������
				p1 = m_lpBits+bw1 * y + x * 3;
				// ȡ����ת������Ӧ������
				p2 = pbits + bw2 * (h2 - x - 1) + y * 3;
				// ����תǰ���������ݸ��Ƹ���ת�������
				p2[0] = p1[0];
				p2[1] = p1[1];
				p2[2] = p1[2];
			}
			break;

			// �����8λ��ɫ��λͼ��ÿ������ռһ���ֽ�
	case 8:
		for (y=0; y<m_nHeight; y++)
			for (x=0; x<m_nWidth; x++)
			{
				// ȡ��y�еĵ�x������
				p1 = m_lpBits+bw1*y+x;
				// ȡ��ת�������Ӧ������
				p2 = pbits+bw2*(h2-x-1)+y;
				// ����תǰ���������ݸ��Ƹ���ת�������
				p2[0] = p1[0];
			}
			break;

			//	�����4λ��ɫ��λͼ��ÿ������ռ����ֽ�
	case 4:
		for (y=0; y<m_nHeight; y+=2)
			for (x=0; x<m_nWidth; x+=2)
			{
				// ȡ��תǰ������
				pa = m_lpBits+bw1*y+x/2;
				pb = pa+bw1;
				// ȡ��ת�������Ӧ������
				pb1 = pbits+bw2*(h2-x-1)+y/2;
				pa1 = pb1-bw2;
				// ���pa1�Ƿ�Խ�磬
				// ������תǰ���������ݸ��Ƹ���ת�������
				if (pa1 >= pbits)
					*pa1 = ((*pa&0x0f)<<4)|(*pb&0x0f);
				*pb1 = (*pa&0xf0)|((*pb&0xf0)>>4);
			}
			break;
	}

	// ɾ����תǰ��λͼ��������
	delete[] m_lpBits;

	// ����ת���λͼ���ݸ�ֵ��m_lpBits
	m_lpBits = pbits;
	// ��m_nWidth����Ϊ��ת���λͼ���
	m_nWidth = w2;
	// ��m_nHeight����Ϊ��ת���λͼ�߶�
	m_nHeight = h2;
	//����λͼͷ�ṹ��
	m_pbmih->biWidth = m_nWidth;
	m_pbmih->biHeight = m_nHeight;

	return TRUE;
}
