#ifndef _BITMAP_TOOL_H
#define _BITMAP_TOOL_H

#include "windows.h"
#include <string>

using namespace std;

// 计算图像每行象素所占的字节数目 
#define BYTE_PER_LINE(w, c) ((((w)*(c)+31)/32)*4)

//从BMP格式的图片文件中获得像素数据。
BYTE* GetBitsDataFromBMP(std::string path, int& width, int& height, int& bitCount);

//位图数据上下翻转
void BitmapFlipVertical(BYTE* bits, int width, int height, int bitCount);

//位图数据左右翻转，位宽为8
void BitmapFlipHorizontal(BYTE* bits, int width, int height);

//将24bits的位图转换为8bits的灰阶图像
void BitmapToGray(BYTE* gray_bits, BYTE* color_bits, int width, int height);

//对BPP=8的图像进行扩展
bool BitmapExtend(BYTE* dst_bits, int x, int y, int dst_width, int dst_height, 
	BYTE* src_bits, int src_width, int src_height);

void BitmapCrop(BYTE* dst_bits, int dst_width, int dst_height, 
	BYTE* src_bits, int x, int y, int src_width, int src_height);

#endif //_BITMAP_TOOL_H