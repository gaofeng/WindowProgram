#ifndef _BITMAP_TOOL_H
#define _BITMAP_TOOL_H

#include "windows.h"
#include <string>

using namespace std;

// ����ͼ��ÿ��������ռ���ֽ���Ŀ 
#define BYTE_PER_LINE(w, c) ((((w)*(c)+31)/32)*4)

//��BMP��ʽ��ͼƬ�ļ��л���������ݡ�
BYTE* GetBitsDataFromBMP(std::string path, int& width, int& height, int& bitCount);

//λͼ�������·�ת
void BitmapFlipVertical(BYTE* bits, int width, int height, int bitCount);

//λͼ�������ҷ�ת��λ��Ϊ8
void BitmapFlipHorizontal(BYTE* bits, int width, int height);

//��24bits��λͼת��Ϊ8bits�Ļҽ�ͼ��
void BitmapToGray(BYTE* gray_bits, BYTE* color_bits, int width, int height);

//��BPP=8��ͼ�������չ
bool BitmapExtend(BYTE* dst_bits, int x, int y, int dst_width, int dst_height, 
	BYTE* src_bits, int src_width, int src_height);

void BitmapCrop(BYTE* dst_bits, int dst_width, int dst_height, 
	BYTE* src_bits, int x, int y, int src_width, int src_height);

#endif //_BITMAP_TOOL_H