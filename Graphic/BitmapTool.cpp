#include "BitmapTool.h"
#include "ScopeGuard.hpp"

//从BMP格式的图片文件中获得像素数据。
BYTE* GetBitsDataFromBMP(std::string path, int& width, int& height, int& bitCount)
{
	BOOL               bSuccess ;
	DWORD              dwFileSize, dwHighSize, dwBytesRead ;
	HANDLE             hFile ;

	hFile = CreateFile (path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL ;
	}
	ON_SCOPE_EXIT([&]{CloseHandle(hFile);});
	dwFileSize = GetFileSize (hFile, &dwHighSize);
	if (dwHighSize)
	{
		CloseHandle (hFile) ;
		return NULL ;
	}
	//获取文件头
	BITMAPFILEHEADER bmfh;
	bSuccess = ReadFile (hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
	if (!bSuccess || 
		(dwBytesRead != sizeof(BITMAPFILEHEADER)) ||     
		(bmfh.bfType != * (WORD *) "BM") ||
		(bmfh.bfSize != dwFileSize))
	{
		return NULL ;
	}
	//读信息头
	BITMAPINFOHEADER bmih;
	bSuccess = ReadFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);
	if (!bSuccess ||
		(dwBytesRead != sizeof(BITMAPINFOHEADER)))
	{
		return NULL;
	}
	//读位图数据
	DWORD dwPtr = SetFilePointer(hFile, bmfh.bfOffBits, NULL, FILE_BEGIN);
	if (dwPtr == INVALID_SET_FILE_POINTER)
	{
		return NULL;
	}
	if (bmih.biSizeImage == 0)
	{
		bmih.biSizeImage = bmih.biHeight * bmih.biWidth * (bmih.biBitCount / 8);
	}
	BYTE* bits = new BYTE[bmih.biSizeImage];
	bSuccess = ReadFile(hFile, bits, bmih.biSizeImage, &dwBytesRead, NULL);
	if (!bSuccess ||
		(dwBytesRead != bmih.biSizeImage))
	{
		delete [] bits;
		return NULL;
	}

	width = bmih.biWidth;
	height = bmih.biHeight;
	bitCount = bmih.biBitCount;

	//上下翻转
	BitmapFlipVertical(bits, width, height, bitCount);

	return bits;
}

void BitmapFlipVertical(BYTE* bits, int width, int height, int bitCount)
{
	int line_bytes = (width * bitCount + 31) / 32 * 4;
	BYTE* line_buf = new BYTE[line_bytes];
	for (int i = 0; i < (height / 2); i++)
	{
		memcpy(line_buf, bits + line_bytes * i, line_bytes);
		memcpy(bits + line_bytes * i, bits + line_bytes * (height - i - 1), line_bytes);
		memcpy(bits + line_bytes * (height - i - 1), line_buf, line_bytes);
	}
	delete [] line_buf;
}

void BitmapFlipHorizontal(BYTE* bits, int width, int height)
{
	BYTE pixel = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width / 2; j++)
		{
			pixel = bits[i * width + j];
			bits[i * width + j] = bits[(i + 1) * width - 1 - j];
			bits[(i + 1) * width - 1 - j] = pixel;
		}
	}
}

void BitmapToGray(BYTE* gray_bits, BYTE* color_bits, int width, int height)
{
	int nByteWidth = 0;
	int x = 0;
	int y = 0;
	BYTE *p = NULL;
	
	//转换为灰阶图像
	// 计算位图每行象素所占的字节数目
	nByteWidth = BYTE_PER_LINE(width, 24);
	// 对于位图中的每一行
	for (y = 0; y < height; y++)
	{
		// 对于每一行中的每一个象素
		for (x = 0; x < width; x++)
		{
			// 找到该象素在象素数据数组中的位置
			// 24位真彩色位图中，每个象素占3个字节
			p = color_bits + nByteWidth * y + x * 3;
			// 修改象素的颜色，使其灰度化
			gray_bits[y * width + x] = (BYTE)(0.299*p[2]+0.587*p[1]+0.114*p[0]);
		}
	}
}

bool BitmapExtend(BYTE* dst_bits, int x, int y, int dst_width, int dst_height, 
	BYTE* src_bits, int src_width, int src_height)
{
	if ((src_width > dst_width) ||
		(src_height > dst_height))
	{
		return false;
	}
	memset(dst_bits, 0xFF, dst_width * dst_height);
	for (int src_row_index = 0; src_row_index < src_height; ++src_row_index)
	{
		memcpy(dst_bits + (src_row_index + y) * dst_width + x, 
			src_bits + src_row_index * src_width, 
			src_width);
	}
	return true;
}

void BitmapCrop(BYTE* dst_bits, int dst_width, int dst_height, 
	BYTE* src_bits, int x, int y, int src_width, int src_height)
{
	memset(dst_bits, 0xFF, dst_width * dst_height);
	if (dst_width > (src_width - x))
	{
		dst_width = src_width - x;
	}
	if (dst_height > (src_height - y))
	{
		dst_height = src_width - y;
	}
	for (int src_row_index = y; src_row_index < dst_height; src_row_index++)
	{
		memcpy(dst_bits + src_row_index * dst_width, 
			src_bits + (y + src_row_index) * src_width + x, 
			dst_width);
	}
}
