#include "stdafx.h"
#include "dib.h"

// ����ͼ��ÿ��������ռ���ֽ���Ŀ 
#define BYTE_PER_LINE(w, c) ((((w)*(c)+31)/32)*4)
// ���ݱ�ʾ��ɫ��λ����ȷ���ܱ�ʾ����ɫ��
// �����24λ��ɫ��Ϊ0�������8λ��ɫ��Ϊ256�������4λ��ɫ��Ϊ16
#define PALETTESIZE(b) (((b)==8)?256:(((b)==4)?16:0))

// ���캯������ʼ��CDIB���������
CDIB::CDIB()
{
	m_lpBits = NULL;
	m_lpPalette = NULL;
	m_nWidth = m_nHeight = 0;
	m_nBitCount = 0;
}

// ��������
CDIB::~CDIB()
{
	Clear();
}

// �����ǰ��ͼ�����ݣ����ͷ��ڴ�
void CDIB::Clear()
{
	// �ͷ�λͼ����
	if (m_lpBits) 
		delete[] m_lpBits;
	m_lpBits=NULL;

	// �Ƿ��ɫ������
	if (m_lpPalette) 
		delete[] m_lpPalette;
	m_lpPalette = NULL;

	// ���λͼ��Ϣ
	m_nWidth = m_nHeight = 0;
	m_nBitCount = 0;
}

// 	��λͼ���ݱ��浽λͼ�ļ��������ļ�����
BOOL CDIB::SaveToFile(LPCTSTR fn /*�ļ���*/)
{
	CFile f;
	
	// ������ܴ�ָ�����Ƶ��ļ�������
	if (!f.Open(fn, CFile::modeCreate|CFile::modeWrite)) 
		return FALSE;
	
	// ���򣬱���λͼ���ݵ��ļ�
	BOOL r = SaveToFile(&f);

	// �ر��ļ�
	f.Close();
	return r;
}

// ��λͼ�ļ��ж�ȡλͼ���ݣ������ļ�����
BOOL CDIB::LoadFromFile(LPCTSTR filename /*�ļ���*/)
{
	CFile f;

	// ������ܴ�ָ�����Ƶ��ļ�������
	if (!f.Open(filename, CFile::modeRead)) 
		return FALSE;
	
	// ���򣬱���λͼ���ݵ��ļ�
	BOOL r = LoadFromFile(&f);

	// �ر��ļ�
	f.Close();
	return r;
}

// ��λͼ���ݱ��浽λͼ�ļ������ݴ򿪵��ļ�ָ�룩
BOOL CDIB::SaveToFile(CFile *pf)
{
	// ���ͼ������Ϊ�գ�����
	if (m_lpBits == NULL) 
		return FALSE;

	// ��ȡͼ���ļ���ÿ��ͼ����ռ�ֽ���
	int nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// ���λͼ�ļ�ͷ�ṹ��ָ���ļ������Ϣ
	BITMAPFILEHEADER bm;
	
	// ָ���ļ�����Ϊλͼ
	bm.bfType = 'M'*256+'B';
	// ָ��λͼ�ļ��Ĵ�С
	bm.bfSize = nByteWidth*m_nHeight;
	// �����ĽṹԪ�أ�����Ϊ0
	bm.bfReserved1 = 0;
	bm.bfReserved2 = 0;
	// ������ļ�ͷ��ʼ��ʵ�ʵ�ͼ������֮���ƫ�������ֽ�����
	bm.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	// �������24λ���ɫλͼ�����ϵ�ɫ����Ϣ�ĳ���
	if (m_nBitCount != 24)
		bm.bfOffBits += PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD);

	// ���ļ���д��λͼ�ļ�ͷ��Ϣ
	pf->Write(&bm, sizeof(BITMAPFILEHEADER));

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
	bmi.biSizeImage = 0;
	// ָ��Ŀ���豸��ˮƽ�ֱ��ʣ�������/�ױ�ʾ
	bmi.biXPelsPerMeter = 0;
	// ָ��Ŀ���豸�Ĵ�ֱ�ֱ��ʣ�������/�ױ�ʾ
	bmi.biYPelsPerMeter = 0;
	// ָ��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ���
	bmi.biClrUsed = 0;
	// ָ����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��
	bmi.biClrImportant = 0;
	// ��λͼ��Ϣͷд���ļ�
	pf->Write(&bmi,sizeof BITMAPINFOHEADER);
	
	// �������24λ���ɫλͼ������ɫ����Ϣд���ļ�
	if (m_nBitCount!=24)
		pf->Write(m_lpPalette, PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD));
	
	// ��λͼ����д���ļ�
	pf->Write(m_lpBits, nByteWidth*m_nHeight);

	return TRUE;
}

// ��λͼ�ļ��ж�ȡλͼ���ݣ����ݴ򿪵��ļ�ָ�룩
BOOL CDIB::LoadFromFile(CFile *pf)
{
	BYTE *lpBitmap,*lpDib;
	int nLen;

	// ��ȡλͼ�ļ��Ĵ�С
	nLen = pf->GetLength();

	// ���䱣��λͼ���ݵ��ڴ�ռ�
	lpBitmap = new BYTE[nLen];

	// ���ļ��ж���λͼ����
	pf->Read(lpBitmap, nLen);

	// ����ļ�������
	// �������λͼ�ļ�����ʾ�����ͷ��ڴ�
	if (lpBitmap[0]!='B' && lpBitmap[1]!='M')
	{
		AfxMessageBox("��λͼ�ļ�");
		delete[] lpBitmap;
		return FALSE;
	}

	// �����ҵ�λͼ��Ϣͷ����ʼλ��
	lpDib = lpBitmap+sizeof(BITMAPFILEHEADER);
	
	// ��DIBλͼ������䵽��CDib������ݽṹ
	BOOL ret = LoadDib (lpDib);
	
	// �ͷ��ڴ�
	delete[] lpBitmap;
	
	return ret;
}

// ��Dibλͼ������䵽��CDib������ݽṹ
BOOL CDIB::LoadDib(BYTE *lpDib)
{
	// �����ǰ��ͼ�����ݣ����ͷ�����ڴ�
	Clear();
	
	BYTE *lpBits;
	BITMAPINFOHEADER *pInfo;
	int nWidth,nHeight,nBitCount,nByteWidth;
	RGBQUAD *pPalette;
	unsigned int PaletteSize;

	// ��ȡλͼ��Ϣͷ��ָ��
	pInfo = (BITMAPINFOHEADER *)lpDib;
	// ��ȡλͼ���ݵ�ָ��
	lpBits = lpDib+sizeof(BITMAPINFOHEADER);

	// ��ȡλͼ�Ŀ��
	nWidth = pInfo->biWidth;
	// ��ȡλͼ�ĸ߶�
	nHeight = pInfo->biHeight;
	// ��ȡλͼ��ʾ��ɫ���õ�λ��
	nBitCount = pInfo->biBitCount;
	// ����λͼÿ��������ռ���ֽ���Ŀ 
	nByteWidth = BYTE_PER_LINE(nWidth, nBitCount);

	// ���λͼ����ɫ��
	switch (nBitCount)
	{
		// �����24λ��ɫ��λͼ��û�е�ɫ����Ϣ
		case 24:
			// �ͷ���ǰ��λͼ���ݺ͵�ɫ������ռ�е��ڴ�
			if (m_lpBits) 
				delete[] m_lpBits;
			if (m_lpPalette) 
				delete[] m_lpPalette;
			m_lpPalette = NULL;

			// ����Ϊλͼ�������ݷ����ڴ�
			m_lpBits= new BYTE[nByteWidth*nHeight];
			// ��λͼ�������ݸ��Ƶ�m_lpBits
			memcpy(m_lpBits, lpBits, nByteWidth*nHeight);
			break;

		// �����8λ����4λ��ɫ��λͼ���е�ɫ����Ϣ
		case 8:
		case 4:
			// �ͷ���ǰ��λͼ���ݺ͵�ɫ������ռ�е��ڴ�
			if (m_lpBits) 
				delete[] m_lpBits;
			if (m_lpPalette) 
				delete[] m_lpPalette;

			// �����ɫ����ʵ����ɫ����
			PaletteSize = (1<<pInfo->biBitCount);
			// ���pInfo->biClrUsed������0��
			// ʹ��pInfo->biClrUsedָ����λͼʵ��ʹ�õ���ɫ��
			if (pInfo->biClrUsed!=0 && pInfo->biClrUsed<PaletteSize) 
				PaletteSize = pInfo->biClrUsed;
			
			// ��ȡ��ɫ����Ϣ����
			pPalette = (RGBQUAD *)lpBits;
			// ��ָ���ƶ����������ݵĿ�ʼ��
			lpBits += sizeof(RGBQUAD)*PaletteSize;

			// Ϊ�����ɫ����Ϣ���ݵ�m_lpPalette����ռ�
			m_lpPalette = new BYTE[sizeof(RGBQUAD)*PaletteSize];
			// ����ɫ����Ϣ���ݸ��Ƶ�m_lpPalette
			memcpy(m_lpPalette, pPalette, sizeof(RGBQUAD)*PaletteSize);

			// Ϊ����λͼ�������ݵ�m_lpBits����ռ�
			m_lpBits = new BYTE[nByteWidth*nHeight];
			// ��λͼ���ظ��Ƶ�m_lpBits
			memcpy(m_lpBits, lpBits, nByteWidth*nHeight);
			break;

		// ������������账��
		default:
			return FALSE;
	}

	// ��¼λͼ�Ŀ��
	m_nBitCount=nBitCount;
	// ��¼λͼ�ĸ߶�
	m_nWidth=nWidth;
	// ��¼λͼ��ʾ��ɫ���õ�λ��
	m_nHeight=nHeight;

	return TRUE;
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
		memcpy(p, m_lpPalette, PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD));
		// ��ָ���ƶ�����ɫ����Ϣ����֮��
		p += PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD);
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
		nLen += PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD);

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

// ���Ƽ������е�����
BOOL CDIB::Paste()
{
	HGLOBAL hMem;
	BYTE *lpDib;

	// ����޷��򿪼����壬����
	if (!OpenClipboard(NULL)) 
		return FALSE;

	// ������ܴӼ������ϵõ�λͼ��ʽ�����ݣ�����
	if (!(hMem=GetClipboardData(CF_DIB))) 
		return FALSE;

	// �����������ݵ��ڴ�ռ�
	lpDib = (BYTE *)GlobalLock(hMem);
	// ��DIBλͼ������䵽��CDib������ݽṹ
	LoadDib(lpDib);

	// �رռ�����
	CloseClipboard();
	return TRUE;
}

// ����λͼ���ݻ���λͼ
int CDIB::Stretch(HDC hDC,
			int XDest,int YDest,int nDestWidth,int nDestHeight,
			int XSrc,int YSrc,int nSrcWidth,int nSrcHeight,
			UINT iUsage,DWORD dwRop)
{
	// λͼ��Ϣ�ṹ������λͼ��Ϣͷ�͵�ɫ����Ϣ��
	// ����λͼ�Ĵ�С����ɫ���
	BITMAPINFO *pbmi;
	int palsize;

	// ��ȡ��ɫ�����ɫ��
	palsize = PALETTESIZE(m_nBitCount);
	// ����λͼ������ռ�ڴ�ռ�
	pbmi=(BITMAPINFO *)new BYTE[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*palsize];

	// ����ɫ����Ϣ���Ƶ�λͼ��Ϣ�ṹ��
	memcpy (pbmi->bmiColors, m_lpPalette, sizeof(RGBQUAD)*palsize);
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

// ��ͼ��ת��Ϊ�Ҷ�ͼ��
BOOL CDIB::Grey()
{
	// ���û��λͼ���ݣ�����
	if (!m_lpBits) 
		return FALSE;

	int y, x, nByteWidth, palsize, grey;
	BYTE *p;
	
	// �����24λ���ɫ��λͼ
	if (m_nBitCount==24)
	{
		// ����λͼÿ��������ռ���ֽ���Ŀ
		nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);
		// ����λͼ�е�ÿһ��
		for (y=0; y<m_nHeight; y++)
			// ����ÿһ���е�ÿһ������
			for (x=0; x<m_nWidth; x++)
			{
				// �ҵ����������������������е�λ��
				// 24λ���ɫλͼ�У�ÿ������ռ3���ֽ�
				p = m_lpBits+nByteWidth*y+x*3;
				
				// �޸����ص���ɫ��ʹ��ҶȻ�
				grey = (BYTE)(0.299*p[2]+0.587*p[1]+0.114*p[0]);
				p[0] = grey;
				p[1] = grey;
				p[2] = grey;
			}
	}
	// �������24λ���ɫλͼ���޸ĵ�ɫ���е���ɫ
	else
	{
		// ��õ�ɫ�����ɫ��
		palsize = PALETTESIZE(m_nBitCount);
		// ���ڵ�ɫ���е�ÿһ����ɫ
		for (x=0; x<palsize; x ++)
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
			for (y=0; y<m_nHeight; y++)
				for (x=0; x<m_nWidth/2; x++)
				{
					// ��y�еĵ�x�����ص�����ָ��
					p1 = m_lpBits+nByteWidth*y+x*3;
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
			break;
	
		// �����8λ��ɫλͼ��ÿ������ռһ���ֽ�
		case 8:
			for (y=0; y<m_nHeight; y++)
				for (x=0; x<m_nWidth/2; x++)
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
	for (y=0; y<m_nHeight/2; y++)
	{
		// ȡ�õ�y�����ص�����
		p1 = m_lpBits+y*nByteWidth;
		// ȡ�õ�����y�����ص�����
		p2 = m_lpBits+(m_nHeight-y-1)*nByteWidth;
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
					p1 = m_lpBits+bw1*y+x*3;
					// ȡ����ת������Ӧ������
					p2 = pbits+bw2*(h2-x-1)+y*3;
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
	
	return TRUE;
}

// ��ͼ��ת��Ϊ����
BOOL CDIB::Negative()
{
	// ���û��λͼ���ݣ�����
	if (!m_lpBits) 
		return FALSE;

	int y, x, nByteWidth, palsize;
	BYTE *p;

	// �����24λ���ɫλͼ��ÿ������ռ�����ֽ�
	if (m_nBitCount==24)
	{
		// ����λͼÿ��������ռ���ֽ���Ŀ
		nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);
		
		for (y=0; y<m_nHeight; y++)
			for (x=0; x<m_nWidth; x++)
			{
				// ȡ�õ�y�еĵ�x����
				p = m_lpBits+nByteWidth*y+x*3;
				// ��ÿһ�����ص�����ȡ��
				p[0] = ~p[0];
				p[1] = ~p[1];
				p[2] = ~p[2];
			}
	}
	else
	{
		// ��õ�ɫ�����ɫ��
		palsize = PALETTESIZE(m_nBitCount);
		// ���ڵ�ɫ���е�ÿһ����ɫ
		for (x=0; x<palsize; x++)
		{
			// ���ÿһ����ɫ�������ڵ�λ��
			// ��ɫ����ÿ����ɫռ4���ֽ�			
			p = m_lpPalette+x*4;
			// ��ÿ����ɫ������ȡ��
			p[0] = ~p[0];
			p[1] = ~p[1];
			p[2] = ~p[2];
		}
	}
	
	return TRUE;
}

// ����ָ���ļ�������PCX��ʽ��ͼ���ļ�������
BOOL CDIB::ImportPcx(char *fn)
{
	BOOL ret=FALSE;
	CFile File;
	PCXFILEHEADER pcxh;
	RGBQUAD *pal;
	BYTE *pBits = NULL, *pPal = NULL, *pline;
	int w, h, bit, bytepl, x, y, count;

	// ������ܴ�PCX�ļ�������
	if (!File.Open(fn, CFile::modeRead)) 
		return FALSE;

	// ��ȡPCX�ļ�ͷ�ṹ������
	File.Read(&pcxh, sizeof(PCXFILEHEADER));
	
	// ����ļ���ʽ��
	// �������PCX�ļ���ʽ���ر��ļ�������ʾ����
	if (pcxh.manufacturer != 0x0a)
	{
		File.Close();
		AfxMessageBox("�� PCX ��ʽ�ļ�!");
		return FALSE;
	}

	// ��ȡPCXͼ��Ŀ��
	w = pcxh.xmax-pcxh.xmin+1;
	// ��ȡPCXͼ��ĸ߶�
	h = pcxh.ymax-pcxh.ymin+1;
	// ����ÿ��������ռ�õ�λ��
	bit = pcxh.bits_per_pixel*pcxh.color_planes;
	// ����λͼÿ��������ռ��λ��
	bytepl = BYTE_PER_LINE(w, bit);

	BYTE *pPcx,*pData;
	int len;
	unsigned char ch;
	
	// ���ͼ�����ݵĳ���
	len = File.GetLength()-sizeof(PCXFILEHEADER);
	// ���䱣��ͼ�����ݵ��ڴ�ռ�
	pPcx = new BYTE[len];
	// ��ͼ�����ݶ��뵽�ڴ�
	File.Read(pPcx, len);
	// ��pDataָ��ָ�򱣴����ݵ��ڴ�
	pData = pPcx;

	// ����ÿ��������ռ�õ�λ������ѹ����
	switch (bit)
	{
		case 24: 
		case 4:	
		case 8:		
			// ���䱣���ѹ�Ժ�λͼ���ݵ��ڴ�
			pBits = new BYTE[bytepl*h];
			
			//��ѹһ��ͼ��
			for (y=0; y<h; y++)
			{		
				// ָ��λͼ���ݵ�һ�е�ָ��
				pline = pBits+(h-y-1)*bytepl;
				for (x=0; x<(w*bit+7)/8;)
				{
					// ȡPCXͼ�������е�һ���ֽ�
					ch = *pData++;
					// �ж��Ƿ�ѹ��
					if ((ch&0xc0)==0xc0)
					{
						// �����ظ�
						count=ch&0x3f;
						// ȡ��һ���ֽ�
						ch = *pData++;
						// �����ݰ��ظ��Ĵ�����ӵ�λͼ������
						while ((count--) && (x<(w*bit+7)/8)) 
							pline[x++]=ch;
					}
					// ���򣬽�����ֱ�Ӽ��뵽λͼ������
					else
						pline[x++]=ch;
				}
			}
			
			// ����ɫ�����Ϣ
			// �����256ɫ��ɫ��
			if (bit == 8)		
			{
				// ��ָ��ָ��PCX��ɫ�����ݵ�λ��
				pData = pPcx+len-768;
				// ���䱣���ɫ�����ݵ��ڴ�ռ�
				pPal = new BYTE[sizeof(RGBQUAD)*256];
				// ѭ�������ν���ɫ������ӵ���ɫ��
				for (x=0; x<256; x++)
				{
					// ��õ�ɫ�����ݵ�ָ��
					pal = (RGBQUAD*)(pPal+sizeof(RGBQUAD)*x);
					// д�����ɫ�ĺ�ɫ����
					pal->rgbRed = *pData++;
					// д�����ɫ����ɫ����
					pal->rgbGreen = *pData++;
					// д�����ɫ����ɫ����
					pal->rgbBlue = *pData++;
					// д�뱣�����ֵ
					pal->rgbReserved= 0;
				}
			}
			// �����16ɫ��ɫ��
			else if (bit == 4)
			{
				// ���䱣���ɫ�����ݵ��ڴ�ռ�
				pPal = new BYTE[sizeof(RGBQUAD)*16];
				for (x=0; x<16; x++)
				{
					// ��õ�ɫ�����ݵ�ָ��
					pal = (RGBQUAD *)(pPal+sizeof(RGBQUAD )*x);
					// д�����ɫ�ĺ�ɫ����
					pal->rgbRed = pcxh.palette[3*x];
					// д�����ɫ����ɫ����
					pal->rgbGreen = pcxh.palette[3*x+1];
					// д�����ɫ����ɫ����
					pal->rgbBlue = pcxh.palette[3*x+2];
					// д�뱣�����ֵ
					pal->rgbReserved = 0;
				}
			}
			// ���򣬲���Ҫ��ɫ��
			else
			{
				pPal = NULL;
				
				//ɫƽ�洦��
				BYTE *linebuf = new BYTE[bytepl];
				for (y=0; y<h; y++)
				{
					pline = pBits+y*bytepl;
					memcpy(linebuf, pline, bytepl);
					for (x=0; x<w; x++)
					{
						pline[x*3+2] = linebuf[x];
						pline[x*3+1] = linebuf[w+x];
						pline[x*3] = linebuf[w*2+x];
					}
				}
				delete[] linebuf;
			}
			ret = TRUE;
			break;

		// ����������ʾ������
		default:
			AfxMessageBox("����ʶ����ļ���ʽ");
			ret = FALSE;
			break;
	}
	
	// �ر��ļ�
	File.Close();
	// ����ļ�����ɹ�
	if (ret)
	{
		// ���ԭ����λͼ���ݺ���Ӧ��Ϣ
		Clear();
		// ����ѹ������ݸ��Ƶ�m_lpBits
		m_lpBits = pBits;
		// ����ɫ�����Ϣ���Ƶ�m_lpPalette
		m_lpPalette = pPal;
		// ����λͼ�Ŀ�ȡ��߶Ⱥ�ÿ������ռ�õ�λ��
		m_nWidth = w;
		m_nHeight = h;
		m_nBitCount = bit;
	}
	// ����ļ�����ʧ�ܣ�ɾ�����ɵ�λͼ���ݺ͵�ɫ������
	else
	{
		if (pBits) 
			delete[] pBits;
		if (pPal) 
			delete[] pPal;
	}
	
	// ɾ�������PCX����
	delete[] pPcx;
	
	return ret;
}


