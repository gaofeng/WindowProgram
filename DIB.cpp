#include "stdafx.h"
#include "dib.h"

// 计算图像每行象素所占的字节数目 
#define BYTE_PER_LINE(w, c) ((((w)*(c)+31)/32)*4)
// 根据表示颜色的位数来确定能表示的颜色数
// 如果是24位颜色，为0；如果是8位颜色，为256；如果是4位颜色，为16
#define PALETTESIZE(b) (((b)==8)?256:(((b)==4)?16:0))

// 构造函数，初始化CDIB对象的数据
CDIB::CDIB()
{
	m_lpBits = NULL;
	m_lpPalette = NULL;
	m_nWidth = m_nHeight = 0;
	m_nBitCount = 0;
}

// 析构函数
CDIB::~CDIB()
{
	Clear();
}

// 清除以前的图像数据，并释放内存
void CDIB::Clear()
{
	// 释放位图数据
	if (m_lpBits) 
		delete[] m_lpBits;
	m_lpBits=NULL;

	// 是否调色板数据
	if (m_lpPalette) 
		delete[] m_lpPalette;
	m_lpPalette = NULL;

	// 清除位图信息
	m_nWidth = m_nHeight = 0;
	m_nBitCount = 0;
}

// 	将位图数据保存到位图文件（根据文件名）
BOOL CDIB::SaveToFile(LPCTSTR fn /*文件名*/)
{
	CFile f;
	
	// 如果不能打开指定名称的文件，返回
	if (!f.Open(fn, CFile::modeCreate|CFile::modeWrite)) 
		return FALSE;
	
	// 否则，保存位图数据到文件
	BOOL r = SaveToFile(&f);

	// 关闭文件
	f.Close();
	return r;
}

// 从位图文件中读取位图数据（根据文件名）
BOOL CDIB::LoadFromFile(LPCTSTR filename /*文件名*/)
{
	CFile f;

	// 如果不能打开指定名称的文件，返回
	if (!f.Open(filename, CFile::modeRead)) 
		return FALSE;
	
	// 否则，保存位图数据到文件
	BOOL r = LoadFromFile(&f);

	// 关闭文件
	f.Close();
	return r;
}

// 将位图数据保存到位图文件（根据打开的文件指针）
BOOL CDIB::SaveToFile(CFile *pf)
{
	// 如果图像数据为空，返回
	if (m_lpBits == NULL) 
		return FALSE;

	// 获取图像文件中每行图像所占字节数
	int nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);

	// 填充位图文件头结构，指定文件相关信息
	BITMAPFILEHEADER bm;
	
	// 指定文件类型为位图
	bm.bfType = 'M'*256+'B';
	// 指定位图文件的大小
	bm.bfSize = nByteWidth*m_nHeight;
	// 保留的结构元素，必须为0
	bm.bfReserved1 = 0;
	bm.bfReserved2 = 0;
	// 计算从文件头开始到实际的图象数据之间的偏移量（字节数）
	bm.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	// 如果不是24位真彩色位图，加上调色板信息的长度
	if (m_nBitCount != 24)
		bm.bfOffBits += PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD);

	// 在文件中写入位图文件头信息
	pf->Write(&bm, sizeof(BITMAPFILEHEADER));

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
	bmi.biSizeImage = 0;
	// 指定目标设备的水平分辨率，用象素/米表示
	bmi.biXPelsPerMeter = 0;
	// 指定目标设备的垂直分辨率，用象素/米表示
	bmi.biYPelsPerMeter = 0;
	// 指定位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
	bmi.biClrUsed = 0;
	// 指定对图像显示有重要影响的颜色索引的数目，如果是0，表示都重要。
	bmi.biClrImportant = 0;
	// 将位图信息头写入文件
	pf->Write(&bmi,sizeof BITMAPINFOHEADER);
	
	// 如果不是24位真彩色位图，将调色板信息写入文件
	if (m_nBitCount!=24)
		pf->Write(m_lpPalette, PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD));
	
	// 将位图数据写入文件
	pf->Write(m_lpBits, nByteWidth*m_nHeight);

	return TRUE;
}

// 从位图文件中读取位图数据（根据打开的文件指针）
BOOL CDIB::LoadFromFile(CFile *pf)
{
	BYTE *lpBitmap,*lpDib;
	int nLen;

	// 获取位图文件的大小
	nLen = pf->GetLength();

	// 分配保存位图数据的内存空间
	lpBitmap = new BYTE[nLen];

	// 从文件中读入位图数据
	pf->Read(lpBitmap, nLen);

	// 检查文件的类型
	// 如果不是位图文件，提示错误，释放内存
	if (lpBitmap[0]!='B' && lpBitmap[1]!='M')
	{
		AfxMessageBox("非位图文件");
		delete[] lpBitmap;
		return FALSE;
	}

	// 否则，找到位图信息头的起始位置
	lpDib = lpBitmap+sizeof(BITMAPFILEHEADER);
	
	// 将DIB位图数据填充到到CDib类的数据结构
	BOOL ret = LoadDib (lpDib);
	
	// 释放内存
	delete[] lpBitmap;
	
	return ret;
}

// 将Dib位图数据填充到到CDib类的数据结构
BOOL CDIB::LoadDib(BYTE *lpDib)
{
	// 清除以前的图像数据，并释放相关内存
	Clear();
	
	BYTE *lpBits;
	BITMAPINFOHEADER *pInfo;
	int nWidth,nHeight,nBitCount,nByteWidth;
	RGBQUAD *pPalette;
	unsigned int PaletteSize;

	// 获取位图信息头的指针
	pInfo = (BITMAPINFOHEADER *)lpDib;
	// 获取位图数据的指针
	lpBits = lpDib+sizeof(BITMAPINFOHEADER);

	// 获取位图的宽度
	nWidth = pInfo->biWidth;
	// 获取位图的高度
	nHeight = pInfo->biHeight;
	// 获取位图表示颜色所用的位数
	nBitCount = pInfo->biBitCount;
	// 计算位图每行象素所占的字节数目 
	nByteWidth = BYTE_PER_LINE(nWidth, nBitCount);

	// 检查位图的颜色数
	switch (nBitCount)
	{
		// 如果是24位颜色的位图，没有调色板信息
		case 24:
			// 释放以前的位图数据和调色板数据占有的内存
			if (m_lpBits) 
				delete[] m_lpBits;
			if (m_lpPalette) 
				delete[] m_lpPalette;
			m_lpPalette = NULL;

			// 重新为位图象素数据分配内存
			m_lpBits= new BYTE[nByteWidth*nHeight];
			// 将位图象素数据复制到m_lpBits
			memcpy(m_lpBits, lpBits, nByteWidth*nHeight);
			break;

		// 如果是8位或者4位颜色的位图，有调色板信息
		case 8:
		case 4:
			// 释放以前的位图数据和调色板数据占有的内存
			if (m_lpBits) 
				delete[] m_lpBits;
			if (m_lpPalette) 
				delete[] m_lpPalette;

			// 计算调色板中实际颜色数量
			PaletteSize = (1<<pInfo->biBitCount);
			// 如果pInfo->biClrUsed不等于0，
			// 使用pInfo->biClrUsed指定的位图实际使用的颜色数
			if (pInfo->biClrUsed!=0 && pInfo->biClrUsed<PaletteSize) 
				PaletteSize = pInfo->biClrUsed;
			
			// 获取调色板信息数据
			pPalette = (RGBQUAD *)lpBits;
			// 将指针移动到象素数据的开始处
			lpBits += sizeof(RGBQUAD)*PaletteSize;

			// 为保存调色板信息数据的m_lpPalette分配空间
			m_lpPalette = new BYTE[sizeof(RGBQUAD)*PaletteSize];
			// 将调色板信息数据复制到m_lpPalette
			memcpy(m_lpPalette, pPalette, sizeof(RGBQUAD)*PaletteSize);

			// 为保存位图象素数据的m_lpBits分配空间
			m_lpBits = new BYTE[nByteWidth*nHeight];
			// 将位图象素复制到m_lpBits
			memcpy(m_lpBits, lpBits, nByteWidth*nHeight);
			break;

		// 其它情况，不予处理
		default:
			return FALSE;
	}

	// 记录位图的宽度
	m_nBitCount=nBitCount;
	// 记录位图的高度
	m_nWidth=nWidth;
	// 记录位图表示颜色所用的位数
	m_nHeight=nHeight;

	return TRUE;
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
		memcpy(p, m_lpPalette, PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD));
		// 将指针移动到调色板信息数据之后
		p += PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD);
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
		nLen += PALETTESIZE(m_nBitCount)*sizeof(RGBQUAD);

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

// 复制剪贴板中的数据
BOOL CDIB::Paste()
{
	HGLOBAL hMem;
	BYTE *lpDib;

	// 如果无法打开剪贴板，返回
	if (!OpenClipboard(NULL)) 
		return FALSE;

	// 如果不能从剪贴板上得到位图格式的数据，返回
	if (!(hMem=GetClipboardData(CF_DIB))) 
		return FALSE;

	// 锁定保存数据的内存空间
	lpDib = (BYTE *)GlobalLock(hMem);
	// 将DIB位图数据填充到到CDib类的数据结构
	LoadDib(lpDib);

	// 关闭剪贴板
	CloseClipboard();
	return TRUE;
}

// 根据位图数据画出位图
int CDIB::Stretch(HDC hDC,
			int XDest,int YDest,int nDestWidth,int nDestHeight,
			int XSrc,int YSrc,int nSrcWidth,int nSrcHeight,
			UINT iUsage,DWORD dwRop)
{
	// 位图信息结构，包括位图信息头和调色板信息，
	// 保存位图的大小和颜色情况
	BITMAPINFO *pbmi;
	int palsize;

	// 获取调色板的颜色数
	palsize = PALETTESIZE(m_nBitCount);
	// 分配位图数据所占内存空间
	pbmi=(BITMAPINFO *)new BYTE[sizeof(BITMAPINFO)+sizeof(RGBQUAD)*palsize];

	// 将调色板信息复制到位图信息结构中
	memcpy (pbmi->bmiColors, m_lpPalette, sizeof(RGBQUAD)*palsize);
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

// 将图像转换为灰度图像
BOOL CDIB::Grey()
{
	// 如果没有位图数据，返回
	if (!m_lpBits) 
		return FALSE;

	int y, x, nByteWidth, palsize, grey;
	BYTE *p;
	
	// 如果是24位真彩色的位图
	if (m_nBitCount==24)
	{
		// 计算位图每行象素所占的字节数目
		nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);
		// 对于位图中的每一行
		for (y=0; y<m_nHeight; y++)
			// 对于每一行中的每一个象素
			for (x=0; x<m_nWidth; x++)
			{
				// 找到该象素在象素数据数组中的位置
				// 24位真彩色位图中，每个象素占3个字节
				p = m_lpBits+nByteWidth*y+x*3;
				
				// 修改象素的颜色，使其灰度化
				grey = (BYTE)(0.299*p[2]+0.587*p[1]+0.114*p[0]);
				p[0] = grey;
				p[1] = grey;
				p[2] = grey;
			}
	}
	// 如果不是24位真彩色位图，修改调色板中的颜色
	else
	{
		// 获得调色板的颜色数
		palsize = PALETTESIZE(m_nBitCount);
		// 对于调色板中的每一种颜色
		for (x=0; x<palsize; x ++)
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
			for (y=0; y<m_nHeight; y++)
				for (x=0; x<m_nWidth/2; x++)
				{
					// 第y行的第x个象素的数据指针
					p1 = m_lpBits+nByteWidth*y+x*3;
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
			break;
	
		// 如果是8位彩色位图，每个象素占一个字节
		case 8:
			for (y=0; y<m_nHeight; y++)
				for (x=0; x<m_nWidth/2; x++)
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
	for (y=0; y<m_nHeight/2; y++)
	{
		// 取得第y行象素的数据
		p1 = m_lpBits+y*nByteWidth;
		// 取得倒数第y行象素的数据
		p2 = m_lpBits+(m_nHeight-y-1)*nByteWidth;
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
					p1 = m_lpBits+bw1*y+x*3;
					// 取得旋转后所对应的象素
					p2 = pbits+bw2*(h2-x-1)+y*3;
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
	
	return TRUE;
}

// 将图像转换为负像
BOOL CDIB::Negative()
{
	// 如果没有位图数据，返回
	if (!m_lpBits) 
		return FALSE;

	int y, x, nByteWidth, palsize;
	BYTE *p;

	// 如果是24位真彩色位图，每个象素占三个字节
	if (m_nBitCount==24)
	{
		// 计算位图每行象素所占的字节数目
		nByteWidth = BYTE_PER_LINE(m_nWidth, m_nBitCount);
		
		for (y=0; y<m_nHeight; y++)
			for (x=0; x<m_nWidth; x++)
			{
				// 取得第y行的第x象素
				p = m_lpBits+nByteWidth*y+x*3;
				// 将每一个象素的数据取反
				p[0] = ~p[0];
				p[1] = ~p[1];
				p[2] = ~p[2];
			}
	}
	else
	{
		// 获得调色板的颜色数
		palsize = PALETTESIZE(m_nBitCount);
		// 对于调色板中的每一种颜色
		for (x=0; x<palsize; x++)
		{
			// 获得每一种颜色数据所在的位置
			// 调色板种每种颜色占4个字节			
			p = m_lpPalette+x*4;
			// 将每种颜色的数据取反
			p[0] = ~p[0];
			p[1] = ~p[1];
			p[2] = ~p[2];
		}
	}
	
	return TRUE;
}

// 按照指定文件名读入PCX格式的图像文件的数据
BOOL CDIB::ImportPcx(char *fn)
{
	BOOL ret=FALSE;
	CFile File;
	PCXFILEHEADER pcxh;
	RGBQUAD *pal;
	BYTE *pBits = NULL, *pPal = NULL, *pline;
	int w, h, bit, bytepl, x, y, count;

	// 如果不能打开PCX文件，返回
	if (!File.Open(fn, CFile::modeRead)) 
		return FALSE;

	// 读取PCX文件头结构的数据
	File.Read(&pcxh, sizeof(PCXFILEHEADER));
	
	// 检查文件格式，
	// 如果不是PCX文件格式，关闭文件，并提示错误
	if (pcxh.manufacturer != 0x0a)
	{
		File.Close();
		AfxMessageBox("非 PCX 格式文件!");
		return FALSE;
	}

	// 获取PCX图像的宽度
	w = pcxh.xmax-pcxh.xmin+1;
	// 获取PCX图像的高度
	h = pcxh.ymax-pcxh.ymin+1;
	// 计算每个象素所占用的位数
	bit = pcxh.bits_per_pixel*pcxh.color_planes;
	// 计算位图每行象素所占的位数
	bytepl = BYTE_PER_LINE(w, bit);

	BYTE *pPcx,*pData;
	int len;
	unsigned char ch;
	
	// 获得图像数据的长度
	len = File.GetLength()-sizeof(PCXFILEHEADER);
	// 分配保存图像数据的内存空间
	pPcx = new BYTE[len];
	// 将图像数据读入到内存
	File.Read(pPcx, len);
	// 将pData指针指向保存数据的内存
	pData = pPcx;

	// 根据每个象素所占用的位数，解压像素
	switch (bit)
	{
		case 24: 
		case 4:	
		case 8:		
			// 分配保存解压以后位图数据的内存
			pBits = new BYTE[bytepl*h];
			
			//解压一行图像
			for (y=0; y<h; y++)
			{		
				// 指向位图数据第一行的指针
				pline = pBits+(h-y-1)*bytepl;
				for (x=0; x<(w*bit+7)/8;)
				{
					// 取PCX图像数据中的一个字节
					ch = *pData++;
					// 判断是否压缩
					if ((ch&0xc0)==0xc0)
					{
						// 计算重复
						count=ch&0x3f;
						// 取下一个字节
						ch = *pData++;
						// 将数据按重复的次数添加到位图数据中
						while ((count--) && (x<(w*bit+7)/8)) 
							pline[x++]=ch;
					}
					// 否则，将数据直接加入到位图数据中
					else
						pline[x++]=ch;
				}
			}
			
			// 读调色板的信息
			// 如果是256色调色板
			if (bit == 8)		
			{
				// 将指针指向PCX调色板数据的位置
				pData = pPcx+len-768;
				// 分配保存调色板数据的内存空间
				pPal = new BYTE[sizeof(RGBQUAD)*256];
				// 循环，依次将颜色数据添加到调色板
				for (x=0; x<256; x++)
				{
					// 获得调色板数据的指针
					pal = (RGBQUAD*)(pPal+sizeof(RGBQUAD)*x);
					// 写入该颜色的红色分量
					pal->rgbRed = *pData++;
					// 写入该颜色的绿色分量
					pal->rgbGreen = *pData++;
					// 写入该颜色的蓝色分量
					pal->rgbBlue = *pData++;
					// 写入保留项的值
					pal->rgbReserved= 0;
				}
			}
			// 如果是16色调色板
			else if (bit == 4)
			{
				// 分配保存调色板数据的内存空间
				pPal = new BYTE[sizeof(RGBQUAD)*16];
				for (x=0; x<16; x++)
				{
					// 获得调色板数据的指针
					pal = (RGBQUAD *)(pPal+sizeof(RGBQUAD )*x);
					// 写入该颜色的红色分量
					pal->rgbRed = pcxh.palette[3*x];
					// 写入该颜色的绿色分量
					pal->rgbGreen = pcxh.palette[3*x+1];
					// 写入该颜色的蓝色分量
					pal->rgbBlue = pcxh.palette[3*x+2];
					// 写入保留项的值
					pal->rgbReserved = 0;
				}
			}
			// 否则，不需要调色板
			else
			{
				pPal = NULL;
				
				//色平面处理
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

		// 发生错误，提示并返回
		default:
			AfxMessageBox("不能识别的文件格式");
			ret = FALSE;
			break;
	}
	
	// 关闭文件
	File.Close();
	// 如果文件导入成功
	if (ret)
	{
		// 清除原来的位图数据和相应信息
		Clear();
		// 将解压后的数据复制到m_lpBits
		m_lpBits = pBits;
		// 将调色板的信息复制到m_lpPalette
		m_lpPalette = pPal;
		// 设置位图的宽度、高度和每象素所占用的位数
		m_nWidth = w;
		m_nHeight = h;
		m_nBitCount = bit;
	}
	// 如果文件导入失败，删除生成的位图数据和调色板数据
	else
	{
		if (pBits) 
			delete[] pBits;
		if (pPal) 
			delete[] pPal;
	}
	
	// 删除读入的PCX数据
	delete[] pPcx;
	
	return ret;
}


