/*----------------------------------------
   DIBFILE.H -- Header File for DIBFILE.C

  ----------------------------------------*/

BOOL DibFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName) ;
BOOL DibFileSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName) ;

BITMAPFILEHEADER * DibLoadImage (PTSTR pstrFileName) ;

BOOL DibSaveImage (PTSTR pstrFileName, BITMAPFILEHEADER *) ; 