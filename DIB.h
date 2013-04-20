/*/////////////////////////////////////////////////////////////
����һ�� DIBλͼ�����࣬�����漸���ڲ����������� DIBλͼ��Ϣ��
	BYTE *m_lpBits; ---------- ����
	BYTE *m_lpPalette; ------- ��ɫ�壻����24λλͼ����ָ����NULL
	int m_nWidth; ------------ λͼ���
	int m_nHeight; ----------- λͼ�߶�
	int m_nBitCount; --------- ÿ�������õ�λ��

����λͼ�ĺ����У�
	BOOL LoadFromFile(LPCTSTR filename);
	        ------ �� BMP�ļ��ж���ͼ��
	BOOL ImportPcx(char *fn);
	        ------ �� PCX�ļ��ж���ͼ�񣻼��� pcx��ʽ����

��DIB λͼ����ĺ����У�
	BOOL Grey();			//ת���ɻҶ�ͼ��
	BOOL HFlip();			//ͼ�����ת
	BOOL VFlip();			//ͼ������ת
	BOOL Rotate();			//˳ʱ����ת90��
	BOOL Negative();		//ת�ɸ���
/*////////////////////////////////////////////////////////////*/

#ifndef __DIB_H
#define __DIB_H
#include <windows.h>

class CDIB
{
private:
	BYTE *m_lpBits;		// ͼ������ָ��
	int m_nWidth;		// ͼ��Ŀ�ȣ���λ������
	int m_nHeight;		// ͼ��ĸ߶ȣ���λ������
	int m_nBitCount;	// ͼ���б�ʾÿ�������õ�λ��
	BYTE *m_lpPalette;	// ��ɫ��ָ��

	// ��Dibλͼ������䵽��CDib������ݽṹ
	BOOL LoadDib(BYTE* lpDib);
	// ��CDib������ݽṹ�е�����ת��ΪDibλͼ��ʽ����
	BOOL SaveToDib(BYTE *lpDib);

public:
	CDIB();		// CDib��Ĺ��캯��
	~CDIB();	// CDib�����������

	// ���CDib�����й�λͼ������
	void Clear();
	// ���λͼ�����Ƿ�գ���ͼ��
	BOOL IsEmpty()	{return !m_lpBits;}
	// ��ȡͼ����
	int GetWidth()	{return m_nWidth;}
	// ��ȡͼ��߶�
	int GetHeight()	{return m_nHeight;}
	// ��ȡͼ���б�ʾÿ�������õ�λ��
	int GetBitCount()	{return m_nBitCount;}
	// ����λͼ���ݻ���λͼ
	int Stretch(HDC,int,int,int,int,int,int,int,int,UINT,DWORD);

	// ��λͼ�ļ��ж�ȡλͼ���ݣ������ļ�����
	BOOL LoadFromFile(LPCTSTR filename);
	// ��λͼ���ݱ��浽λͼ�ļ������ݴ򿪵��ļ�ָ�룩
	BOOL SaveToFile(CFile *pf);
	// ��λͼ���ݱ��浽λͼ�ļ��������ļ�����
	BOOL SaveToFile(LPCTSTR fn);
	// ����ָ���ļ�������PCX��ʽ��ͼ���ļ�������
	BOOL ImportPcx(char *fn);

	// ��ͼ�����ݸ��Ƶ�������
	BOOL Copy();
	// ���Ƽ������е�����
	BOOL Paste();

	// ��ͼ��ת��Ϊ�Ҷ�ͼ��
	BOOL Grey();
	// ����תͼ��
	BOOL HFlip();
	// ����תͼ��	
	BOOL VFlip();
	// ��ͼ��˳ʱ����ת90��
	BOOL Rotate();
	// ��ͼ��ת��Ϊ����
	BOOL Negative();
};

#endif //dib.h
