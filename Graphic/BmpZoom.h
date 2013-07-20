#pragma once
#include <atlimage.h>
#include "FormatImg.h"


class BmpZoom
{
public:
	BmpZoom(void);
	~BmpZoom(void);
	void DoScaleBitMap(HBITMAP hBitmap,char * outBmpPath);
};
