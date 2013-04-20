/*/////////////////////////////////////////////////////////////
这是一个 DIB位图操作类，用下面几个内部数据来保存 DIB位图信息：
	BYTE *m_lpBits; ---------- 像素
	BYTE *m_lpPalette; ------- 调色板；对于24位位图，此指针是NULL
	int m_nWidth; ------------ 位图宽度
	int m_nHeight; ----------- 位图高度
	int m_nBitCount; --------- 每像素所用的位数

读入位图的函数有：
	BOOL LoadFromFile(LPCTSTR filename);
	        ------ 从 BMP文件中读入图像
	BOOL ImportPcx(char *fn);
	        ------ 从 PCX文件中读入图像；即对 pcx格式解码

对DIB 位图处理的函数有：
	BOOL Grey();			//转换成灰度图像
	BOOL HFlip();			//图像横向翻转
	BOOL VFlip();			//图像纵向翻转
	BOOL Rotate();			//顺时针旋转90度
	BOOL Negative();		//转成负像
/*////////////////////////////////////////////////////////////*/

#ifndef __DIB_H
#define __DIB_H
#include <windows.h>

class CDIB
{
private:
	BYTE *m_lpBits;		// 图象数据指针
	int m_nWidth;		// 图象的宽度，单位是象素
	int m_nHeight;		// 图象的高度，单位是象素
	int m_nBitCount;	// 图像中表示每像素所用的位数
	BYTE *m_lpPalette;	// 调色板指针

	// 将Dib位图数据填充到到CDib类的数据结构
	BOOL LoadDib(BYTE* lpDib);
	// 将CDib类的数据结构中的数据转换为Dib位图格式数据
	BOOL SaveToDib(BYTE *lpDib);

public:
	CDIB();		// CDib类的构造函数
	~CDIB();	// CDib类的析构函数

	// 清除CDib类中有关位图的数据
	void Clear();
	// 检测位图数据是否空（空图像）
	BOOL IsEmpty()	{return !m_lpBits;}
	// 获取图像宽度
	int GetWidth()	{return m_nWidth;}
	// 获取图像高度
	int GetHeight()	{return m_nHeight;}
	// 获取图像中表示每像素所用的位数
	int GetBitCount()	{return m_nBitCount;}
	// 根据位图数据画出位图
	int Stretch(HDC,int,int,int,int,int,int,int,int,UINT,DWORD);

	// 从位图文件中读取位图数据（根据文件名）
	BOOL LoadFromFile(LPCTSTR filename);
	// 将位图数据保存到位图文件（根据打开的文件指针）
	BOOL SaveToFile(CFile *pf);
	// 将位图数据保存到位图文件（根据文件名）
	BOOL SaveToFile(LPCTSTR fn);
	// 按照指定文件名读入PCX格式的图像文件的数据
	BOOL ImportPcx(char *fn);

	// 将图像数据复制到剪贴板
	BOOL Copy();
	// 复制剪贴板中的数据
	BOOL Paste();

	// 将图像转换为灰度图像
	BOOL Grey();
	// 横向翻转图像
	BOOL HFlip();
	// 纵向翻转图像	
	BOOL VFlip();
	// 将图像顺时针旋转90度
	BOOL Rotate();
	// 将图像转换为负像
	BOOL Negative();
};

#endif //dib.h
