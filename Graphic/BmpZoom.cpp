#include "StdAfx.h"
#include "BmpZoom.h"

BmpZoom::BmpZoom(void)
{
}

BmpZoom::~BmpZoom(void)
{
}



void ScaleBitmap(CBitmap *pSrcBitmap, CBitmap *pDestBitmap, int destWidth, int destHeight)   
  {   
  BITMAP bitmapInfo;   
  pSrcBitmap->GetBitmap(&bitmapInfo);   
  int srcWidth = bitmapInfo.bmWidth;   
  int srcHeight = bitmapInfo.bmHeight;   
  
  HDC hScreenDC = ::GetDC(NULL);  
  CDC* pScreenDC = CDC::FromHandle(hScreenDC);  
  CDC* pSrcDC = new CDC();  
  pSrcDC->CreateCompatibleDC(pScreenDC);  
  CBitmap* pSrcOldBitmap = pSrcDC->SelectObject(pSrcBitmap);  
 
 pDestBitmap->CreateCompatibleBitmap(pSrcDC,destWidth,destHeight);  
 
 CDC* pDestDC = new CDC();  
 pDestDC->CreateCompatibleDC(pSrcDC);  
 CBitmap* pDestOldBitmap = pDestDC->SelectObject(pDestBitmap);  
 
 pDestDC->SetStretchBltMode(HALFTONE);  
 pDestDC->StretchBlt(0,0,destWidth,destHeight,pSrcDC,0,0,srcWidth,srcHeight,SRCCOPY);  
 pDestDC->SelectObject(pDestOldBitmap);  
 pSrcDC->SelectObject(pSrcOldBitmap);  
 
 delete pDestDC;  
 delete pSrcDC;  
 
 ::ReleaseDC(NULL,hScreenDC);  
 }  


void BmpZoom::DoScaleBitMap(HBITMAP hBitmap,char * outBmpPath)
{
	CBitmap* pSrcBitmap = new CBitmap();  
	pSrcBitmap->Attach(hBitmap);  

	CBitmap* pDestBitmap = new CBitmap();  
	ScaleBitmap(pSrcBitmap,pDestBitmap,328,356);  


	CDdb* pDdb = new CDdb(pDestBitmap);
	pDdb->SaveAs256(outBmpPath, 0, 0, 328, 356);
	pDdb->ClearMemory();

	pSrcBitmap->Detach();  
	delete pSrcBitmap;  
	delete pDestBitmap;
	
}
