#include "StdAfx.h"
#include <string.h>
#include "DIB.h"
#include "ScopeGuard.hpp"

// 计算图像每行象素所占的字节数目 
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

	//打开已存在文件
	hFile = CreateFile (filename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	ON_SCOPE_EXIT([&]{CloseHandle(hFile);});
	//获取文件大小
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

	// 释放以前的位图数据和调色板数据占有的内存
	if (m_lpBits) 
		free(m_lpBits);
	m_lpBits = NULL;
	if (m_lpPalette) 
		free(m_lpPalette);
	m_lpPalette = NULL;

	//移动指针
	myFileSeek(hFile, pbmfh.bfOffBits, FILE_BEGIN);
	// 重新为位图象素数据分配内存
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

	// 检查位图的颜色数
	switch (nBitCount)
	{
		// 如果是24位颜色的位图，没有调色板信息
	case 24:
		m_PaletterSize = 0;
		break;
		// 如果是8位或者4位颜色的位图，有调色板信息
	case 8:
	case 4:
		// 计算调色板中实际颜色数量
		m_PaletterSize = (1 << m_pbmih->biBitCount);
		// 如果pInfo->biClrUsed不等于0，
		// 使用pInfo->biClrUsed指定的位图实际使用的颜色数
		if ((m_pbmih->biClrUsed != 0) && (m_pbmih->biClrUsed < m_PaletterSize))
		{
			m_PaletterSize = m_pbmih->biClrUsed;
		}
		// 移动指针到调色板位置
		myFileSeek(hFile, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), FILE_BEGIN);

		// 为保存调色板信息数据的m_lpPalette分配空间
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
		// 其它情况，不予处理
	default:
		free(m_lpBits);
		m_lpBits = NULL;
		return FALSE;
	}

	// 记录位图的宽度
	m_nBitCount = nBitCount;
	// 记录位图的高度
	m_nWidth = nWidth;
	// 记录位图表示颜色所用的位数
	m_nHeight = nHeight;

	return TRUE;
}

// 将位图数据保存到位图文件（根据打开的文件指针）
BOOL CDIB::SaveToFile(LPCTSTR filename)
{
	DWORD  dwBytesWritten;

	// 如果图像数据为空，返回
	if (m_lpBits == NULL) 
		return FALSE;

	HANDLE hFile = CreateFile (filename, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE ;
		}
	// 获取图像文件中每行图像所占字节数
	int nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// 填充位图文件头结构，指定文件相关信息
	BITMAPFILEHEADER bmfh;

	// 指定文件类型为位图
	bmfh.bfType = 'M'*256+'B';
	// 指定位图文件的大小
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 
		m_PaletterSize * sizeof(RGBQUAD) + nByteWidth * m_nHeight;
	// 保留的结构元素，必须为0
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	// 计算从文件头开始到实际的图象数据之间的偏移量（字节数）
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER) + m_PaletterSize * sizeof(RGBQUAD);

	// 在文件中写入位图文件头信息
	WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);

	// 填充位图信息头结构，指定位图的大小和颜色信息
	BITMAPINFOHEADER bmi;
	// 指定位图信息头结构的大小
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	// 指定位图的宽度
	bmi.biWidth = m_nWidth;
	// 指定位图的高度
	bmi.biHeight = m_nHeight;
	// 目标设备的位面数，其值总设为1
	bmi.biPlanes = 1;
	// 指定表示颜色时用到的位数，
	// 常用的值为1(黑白二色图)、4(16色图)、8(256色图)、24(真彩色图)
	bmi.biBitCount = m_nBitCount;
	// 说明没有压缩图像数据
	bmi.biCompression = BI_RGB;
	// 指定实际的位图数据占用的字节数，
	// 当用BI_RGB格式时，可设置为0 
	bmi.biSizeImage = nByteWidth * m_nHeight;
	// 指定目标设备的水平分辨率，用象素/米表示
	bmi.biXPelsPerMeter = 0;
	// 指定目标设备的垂直分辨率，用象素/米表示
	bmi.biYPelsPerMeter = 0;
	// 指定位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
	bmi.biClrUsed = 0;
	// 指定对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要。
	bmi.biClrImportant = 0;
	// 将位图信息头写入文件
	WriteFile(hFile, &bmi,sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);

	// 如果不是24位真彩色位图，将调色板信息写入文件
	if (m_nBitCount != 24)
		WriteFile(hFile, m_lpPalette, m_PaletterSize * sizeof(RGBQUAD), &dwBytesWritten, NULL);

	// 将位图数据写入文件
	WriteFile(hFile, m_lpBits, nByteWidth*m_nHeight, &dwBytesWritten, NULL);

	CloseHandle(hFile);
	return TRUE;
}

/*得到的BITMAP对象需自行释放*/
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
	// 位图信息结构，包括位图信息头和调色板信息，
	// 保存位图的大小和颜色情况
	BITMAPINFO *pbmi;

	// 分配位图数据所占内存空间
	pbmi=(BITMAPINFO *)new BYTE[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*m_PaletterSize];

	// 将调色板信息复制到位图信息结构中
	memcpy (pbmi->bmiColors, m_lpPalette, sizeof(RGBQUAD)*m_PaletterSize);
	//  指定位图信息头结构的大小
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// 指定位图的宽度
	pbmi->bmiHeader.biWidth = m_nWidth;
	// 指定位图的高度
	pbmi->bmiHeader.biHeight = m_nHeight;
	// 目标设备的位面数，其值总设为1	
	pbmi->bmiHeader.biPlanes = 1;
	// 指定表示颜色时用到的位数
	pbmi->bmiHeader.biBitCount = m_nBitCount;
	// 说明没有压缩图像数据 
	pbmi->bmiHeader.biCompression = BI_RGB;
	// 指定实际的位图数据占用的字节数，
	// 当用BI_RGB格式时，可设置为0 
	pbmi->bmiHeader.biSizeImage = 0;
	// 指定目标设备的水平分辨率，用象素/米表示 
	pbmi->bmiHeader.biXPelsPerMeter = 0;
	// 指定目标设备的垂直分辨率，用象素/米表示
	pbmi->bmiHeader.biYPelsPerMeter = 0;
	// 指定位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
	pbmi->bmiHeader.biClrUsed=0;
	// 指定对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要 
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

	// 设置画位图的模式
	SetStretchBltMode(hDC, COLORONCOLOR);

	// 在指定的位置上按照指定的大小画出位图
	int ret = StretchDIBits(hDC, XDest, YDest, nDestWidth, nDestHeight,
		XSrc, YSrc, nSrcWidth, nSrcHeight, m_lpBits,
		pbmi, iUsage, dwRop);

	// 删除所分配的内存空间
	delete[] pbmi;

	return ret;
}

// 将CDib类的数据结构中的数据转换为DIB位图格式数据
BOOL CDIB::SaveToDib(BYTE *lpDib)
{
	// 如果没有位图数据，返回
	if (m_lpBits == NULL) 
		return FALSE;

	// 获取位图数据
	BYTE *p = lpDib;
	// 计算位图每行象素所占的字节数目
	int nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// 填充位图信息头结构
	BITMAPINFOHEADER bmi;
	// 指定位图信息头结构的大小
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	// 指定位图的宽度
	bmi.biWidth = m_nWidth;
	// 指定位图的高度
	bmi.biHeight = m_nHeight;
	// 目标设备的位面数，其值总设为1
	bmi.biPlanes = 1;
	// 指定表示颜色时用到的位数，
	// 常用的值为1(黑白二色图)、4(16色图)、8(256色图)、24(真彩色图)
	bmi.biBitCount = m_nBitCount;
	// 说明没有压缩图像数据
	bmi.biCompression = BI_RGB;
	// 指定实际的位图数据占用的字节数，
	// 当用BI_RGB格式时，可设置为0 
	bmi.biSizeImage = 0;
	// 指定目标设备的水平分辨率，用象素/米表示
	bmi.biXPelsPerMeter = 0;
	// 指定目标设备的垂直分辨率，用象素/米表示
	bmi.biYPelsPerMeter=0;
	// 指定位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
	bmi.biClrUsed=0;
	// 指定对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要 
	bmi.biClrImportant=0;
	// 将位图信息头结构的数据复制到位图数据中
	memcpy(p, &bmi, sizeof(BITMAPINFOHEADER));

	// 将指针移动到位图信息头结构之后
	p += sizeof(BITMAPINFOHEADER);
	// 如果不是24位真彩色位图
	if (m_nBitCount != 24)
	{
		// 将调色板信息数据复制到位图数据中
		memcpy(p, m_lpPalette, m_PaletterSize * sizeof(RGBQUAD));
		// 将指针移动到调色板信息数据之后
		p += m_PaletterSize * sizeof(RGBQUAD);
	}

	// 将像素数据复制到位图数据中
	memcpy(p, m_lpBits, nByteWidth*m_nHeight);

	return TRUE;
}

// 将图像数据复制到剪贴板
BOOL CDIB::Copy()
{
	// 如果没有位图象素数据，返回
	if (m_lpBits == 0) 
		return FALSE;

	// 如果无法打开剪贴板，返回
	if (!OpenClipboard(NULL)) 
		return FALSE;

	// 清空剪贴板，释放剪贴板上数据的句柄，
	// 使当前程序获得对剪贴板的控制
	EmptyClipboard();

	HGLOBAL hMem;
	BYTE *lpDib;
	// 计算位图数据所占有的字节数（位图信息头结构＋象素数据）
	int nLen = sizeof(BITMAPINFOHEADER)
		+ BYTE_PER_LINE(m_nWidth, m_nBitCount)*m_nHeight;
	// 如果不是24位真彩色位图，还要加上调色板信息数据的长度
	if (m_nBitCount != 24)
		nLen += m_PaletterSize * sizeof(RGBQUAD);

	// 为位图数据分配内存空间
	hMem = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, nLen);
	// 锁定所分配的内存空间
	lpDib = (BYTE *)GlobalLock(hMem);
	// 将CDib类的数据结构中的数据转换为DIB位图格式数据
	SaveToDib(lpDib);
	// 解开对内存空间的锁定
	GlobalUnlock(hMem);
	// 将数据按照DIB位图格式拷贝到剪贴板
	SetClipboardData(CF_DIB, hMem);
	// 关闭剪贴板
	CloseClipboard();

	return TRUE;
}

// 将图像转换为灰度图像
BOOL CDIB::Grey()
{
	// 如果没有位图数据，返回
	if (!m_lpBits)
	{
		return FALSE;
	}
	int y, x, nByteWidth, grey;
	int i;
	BYTE *p = NULL;
	BYTE* gray_bits = NULL;

	// 如果是24位真彩色的位图
	if (m_nBitCount==24)
	{
		gray_bits = (BYTE*)malloc(m_nWidth * m_nHeight);
		//转换为灰阶图像
		// 计算位图每行象素所占的字节数目
		nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);
		// 对于位图中的每一行
		for (y = 0; y < m_nHeight; y++)
		{
			// 对于每一行中的每一个象素
			for (x = 0; x < m_nWidth; x++)
			{
				// 找到该象素在象素数据数组中的位置
				// 24位真彩色位图中，每个象素占3个字节
				p = m_lpBits + nByteWidth * y + x * 3;
				// 修改象素的颜色，使其灰度化
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
	// 如果不是24位真彩色位图，也不是8位灰阶图像，修改调色板中的颜色
	else if (m_nBitCount != 8)
	{
		// 对于调色板中的每一种颜色
		for (x = 0; x < m_PaletterSize; x++)
		{
			// 获得每一种颜色数据所在的位置
			// 调色板种每种颜色占4个字节
			p = m_lpPalette+x*4;

			// 将颜色灰度化
			grey = (BYTE)(0.299*p[2]+0.587*p[1]+0.114*p[0]);
			p[0]=grey;
			p[1]=grey;
			p[2]=grey;
		}
	}
	return TRUE;
}

// 横向翻转图像
BOOL CDIB::HFlip()
{
	// 如果没有位图数据，返回
	if (!m_lpBits) 
		return FALSE;

	int y, x, nByteWidth, temp;
	BYTE *p1, *p2;

	// 计算位图每行象素所占的字节数目
	nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// 检查位图的颜色位数，并交换第i列和倒数第i列的象素
	switch (m_nBitCount)
	{
		// 如果是24位真彩色位图，每个象素占3个字节
	case 24:
		for (y = 0; y < m_nHeight; y++)
		{
			for (x = 0; x < m_nWidth / 2; x++)
			{
				// 第y行的第x个象素的数据指针
				p1 = m_lpBits + nByteWidth * y + x * 3;
				// 第y行的倒数第x个象素的数据指针
				p2 = m_lpBits+nByteWidth*y+(m_nWidth-x-1)*3;
				// 利用temp，将p1的第1个字节与p2的第1个字节交换
				temp = p1[0];
				p1[0] = p2[0];
				p2[0] = temp;
				// 利用temp，将p1的第2个字节与p2的第2个字节交换
				temp = p1[1];
				p1[1] = p2[1];
				p2[1] = temp;
				// 利用temp，将p1的第3个字节与p2的第3个字节交换
				temp = p1[2];
				p1[2] = p2[2];
				p2[2] = temp;
			}
		}
		break;
		// 如果是8位彩色位图，每个象素占一个字节
	case 8:
		for (y = 0; y < m_nHeight; y++)
		{
			for (x = 0; x < m_nWidth / 2; x++)
			{
				// 第y行的第x个象素的数据指针
				p1 = m_lpBits+nByteWidth*y+x;
				// 第y行的倒数第x个象素的数据指针
				p2 = m_lpBits+nByteWidth*y+(m_nWidth-x-1);
				// 利用temp，交换这两个象素的数据
				temp = *p1;
				*p1 = *p2;
				*p2 = temp;
			}
		}
		break;
		// 如果是4位彩色位图，返回
	case 4:
		return FALSE;
	}
	return TRUE;
}

// 纵向翻转图像
BOOL CDIB::VFlip()
{
	// 如果没有位图数据，返回
	if (!m_lpBits) 
		return FALSE;

	int y, nByteWidth;
	BYTE *p1, *p2, *pm;

	// 计算位图每行象素所占的字节数目
	nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// 分配能存储一行象素的内存空间
	pm = new BYTE[nByteWidth];

	// 将第y行象素与倒数y行象素进行交换
	for (y = 0; y < m_nHeight / 2; y++)
	{
		// 取得第y行象素的数据
		p1 = m_lpBits + y * nByteWidth;
		// 取得倒数第y行象素的数据
		p2 = m_lpBits + (m_nHeight - y - 1) * nByteWidth;
		// 利用pm，将将第y行象素与倒数y行象素进行交换
		memcpy(pm, p1, nByteWidth);
		memcpy(p1, p2, nByteWidth);
		memcpy(p2, pm, nByteWidth);
	}

	// 删除分配的内存空间
	delete[] pm;

	return TRUE;
}

// 将图像顺时针旋转90度
BOOL CDIB::Rotate()
{
	// 如果没有位图数据，返回
	if (m_lpBits == NULL) 
		return FALSE;

	BYTE *pbits, *p1, *p2, *pa, *pb, *pa1, *pb1;
	int w2, h2, bw2, bw1, x, y;

	// 获取旋转前位图的高度
	w2 = m_nHeight;	
	// 获取旋转前位图的宽度
	h2 = m_nWidth;

	// 计算旋转前位图每行象素所占的字节数目
	bw1 = BYTE_PER_LINE(m_nWidth, m_nBitCount);
	// 计算旋转后位图每行象素所占的字节数目
	bw2 = BYTE_PER_LINE(w2, m_nBitCount);

	// 分配内存空间以保存旋转后的位图
	pbits = new BYTE[bw2*h2];

	// 检查位图的颜色位数
	switch (m_nBitCount)
	{
		// 如果是24位真彩色位图，每个象素占三个字节
	case 24:
		for (y=0; y<m_nHeight; y++)
			for (x=0; x<m_nWidth; x++)
			{
				// 取第y行的第x个象素
				p1 = m_lpBits+bw1 * y + x * 3;
				// 取得旋转后所对应的象素
				p2 = pbits + bw2 * (h2 - x - 1) + y * 3;
				// 将旋转前的象素数据复制给旋转后的象素
				p2[0] = p1[0];
				p2[1] = p1[1];
				p2[2] = p1[2];
			}
			break;

			// 如果是8位颜色的位图，每个象素占一个字节
	case 8:
		for (y=0; y<m_nHeight; y++)
			for (x=0; x<m_nWidth; x++)
			{
				// 取第y行的第x个象素
				p1 = m_lpBits+bw1*y+x;
				// 取旋转后的所对应的象素
				p2 = pbits+bw2*(h2-x-1)+y;
				// 将旋转前的象素数据复制给旋转后的象素
				p2[0] = p1[0];
			}
			break;

			//	如果是4位颜色的位图，每个象素占半个字节
	case 4:
		for (y=0; y<m_nHeight; y+=2)
			for (x=0; x<m_nWidth; x+=2)
			{
				// 取旋转前的象素
				pa = m_lpBits+bw1*y+x/2;
				pb = pa+bw1;
				// 取旋转后的所对应的象素
				pb1 = pbits+bw2*(h2-x-1)+y/2;
				pa1 = pb1-bw2;
				// 检查pa1是否越界，
				// 并将旋转前的象素数据复制给旋转后的象素
				if (pa1 >= pbits)
					*pa1 = ((*pa&0x0f)<<4)|(*pb&0x0f);
				*pb1 = (*pa&0xf0)|((*pb&0xf0)>>4);
			}
			break;
	}

	// 删除旋转前的位图象素数据
	delete[] m_lpBits;

	// 将旋转后的位图数据赋值给m_lpBits
	m_lpBits = pbits;
	// 将m_nWidth设置为旋转后的位图宽度
	m_nWidth = w2;
	// 将m_nHeight设置为旋转后的位图高度
	m_nHeight = h2;
	//更新位图头结构体
	m_pbmih->biWidth = m_nWidth;
	m_pbmih->biHeight = m_nHeight;

	return TRUE;
}
