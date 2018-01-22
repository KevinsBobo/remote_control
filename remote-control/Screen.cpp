// Screen.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "remote-control.h"
#include "Screen.h"
#include "afxdialogex.h"
#include "zlib.h"
#include "common.h"
#include "myFile.h"


// CScreen �Ի���

IMPLEMENT_DYNAMIC(CScreen, CDialogEx)

CScreen::CScreen(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScreen::IDD, pParent)
  , m_isCapture(FALSE)
  , m_pInfo(NULL)
{

}

CScreen::~CScreen()
{
}

void CScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreen, CDialogEx)
  ON_WM_SHOWWINDOW()
  ON_WM_CLOSE()
END_MESSAGE_MAP()


// CScreen ��Ϣ�������


void CScreen::OnShowWindow(BOOL bShow , UINT nStatus)
{
  CDialogEx::OnShowWindow(bShow , nStatus);

  // TODO:  �ڴ˴������Ϣ����������
  m_isContinue = TRUE;
  //֪ͨ�ͻ��˿�ʼ����
  m_pInfo->csSend.Lock();
  SendPacket(m_sClient, emMessType::screenrecv, NULL, 0);
  m_pInfo->csSend.Unlock();
}


void CScreen::OnClose()
{
  // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
  m_isContinue = FALSE;

  CDialogEx::OnClose();
}


void CScreen::GetScreen(char* szBuff , DWORD nLen)
{
    //��ѹ��
  uLong nUnCompressLength = *(uLong*)szBuff;

  uLong nUnCompressLength2 = (uLong)(nUnCompressLength*2.2);

  char* pUncompressBuf = new char[nUnCompressLength2];

  if (pUncompressBuf == NULL)
  {
      TRACE("��ѹ������ռ�ʧ��");
      return;
  }

  DWORD nCompressedlen = nLen;
  char* pData = szBuff + sizeof(uLong);
  //��ʼ��ѹ���� 
  int nRet = uncompress(
      (Bytef*)pUncompressBuf,      //��ѹ������
      (uLongf*)&nUnCompressLength2,//��ѹ��Ĵ�С
      (Bytef*)(pData),         //Դ������
      (u_long)nCompressedlen); // ѹ����Ĵ�С

  if (nRet != Z_OK)
  {
      TRACE("��ѹʧ��");
      return;
  }
  else
  {
    //�����ݴ��ݸ�CapturScreen������ʾ
    ShowScreen(pUncompressBuf, nUnCompressLength);
  }


  if(m_isContinue)
  {
    //֪ͨ�ͻ��˷�����һ֡
    m_pInfo->csSend.Lock();
    SendPacket(m_sClient, emMessType::screenrecv, NULL, 0);
    m_pInfo->csSend.Unlock();
  }

  if (pUncompressBuf != NULL)
  {
      delete[] pUncompressBuf;
      pUncompressBuf = NULL;
  }
}


bool SaveBitmapToFile(CBitmap* bitmap, LPCWSTR lpFileName)  
{              
    HBITMAP hBitmap;    // Ϊ�ղŵ���Ļλͼ���  
    HDC hDC; //�豸������    
    int iBits; //��ǰ��ʾ�ֱ�����ÿ��������ռ�ֽ���    
    WORD wBitCount; //λͼ��ÿ��������ռ�ֽ���    
    DWORD dwPaletteSize = 0, //�����ɫ���С  
        dwBmBitsSize,  //λͼ�������ֽڴ�С     
        dwDIBSize,   //λͼ�ļ���С  
        dwWritten;  //д���ļ��ֽ���  
    BITMAP Bitmap; //λͼ���Խṹ    
    BITMAPFILEHEADER bmfHdr; //λͼ�ļ�ͷ�ṹ    
    BITMAPINFOHEADER bi; //λͼ��Ϣͷ�ṹ      
    LPBITMAPINFOHEADER lpbi; //ָ��λͼ��Ϣͷ�ṹ    
    HANDLE fh,   //�����ļ�  
        hDib,    //�����ڴ���  
        hPal,   //��ɫ����  
        hOldPal = NULL;  
      
    //����λͼ�ļ�ÿ��������ռ�ֽ���    
    hBitmap = (HBITMAP)*bitmap;    
    hDC = CreateDC(_T("DISPLAY"),NULL,NULL,NULL);    
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);    
    DeleteDC(hDC);  
      
    if (iBits <= 1)    
        wBitCount = 1;    
    else if (iBits <= 4)    
        wBitCount = 4;    
    else if (iBits <= 8)    
        wBitCount = 8;    
    else if (iBits <= 24)    
        wBitCount = 24;    
    else if (iBits <= 32)  
        wBitCount = 32;  
      
    //�����ɫ���С    
    if (wBitCount <= 8)    
        dwPaletteSize = (1 << wBitCount) * sizeof (RGBQUAD);  
      
    //����λͼ��Ϣͷ�ṹ    
    GetObject(hBitmap, sizeof (BITMAP), (LPSTR)&Bitmap);  
    bi.biSize = sizeof (BITMAPINFOHEADER);    
    bi.biWidth = Bitmap.bmWidth;    
    bi.biHeight = Bitmap.bmHeight;    
    bi.biPlanes = 1;      
    bi.biBitCount = wBitCount;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;    
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;    
      
    dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31) / 32) * 4 * Bitmap.bmHeight;    
      
    //Ϊλͼ���ݷ����ڴ�    
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof (BITMAPINFOHEADER));    
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);    
    *lpbi = bi;  
      
    // �����ɫ��         
    hPal = GetStockObject(DEFAULT_PALETTE);    
    if (hPal)    
    {    
        hDC = ::GetDC(NULL);      
        hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);    
        RealizePalette(hDC);  
    }    
      
    //   ��ȡ�õ�ɫ�����µ�����ֵ    
    GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight,    
        (LPSTR)lpbi + sizeof (BITMAPINFOHEADER) + dwPaletteSize,  
        (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);    
      
    //�ָ���ɫ��       
    if (hOldPal)      
    {    
        SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);    
        RealizePalette(hDC);    
        ::ReleaseDC(NULL, hDC);  
    }    
      
    //����λͼ�ļ�              
    fh = CreateFile(lpFileName, GENERIC_WRITE,  
        0, NULL, CREATE_ALWAYS,  
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);    
      
    if (fh == INVALID_HANDLE_VALUE)  
        return FALSE;    
      
    //   ����λͼ�ļ�ͷ    
    bmfHdr.bfType = 0x4D42;     //   "BM"    
    dwDIBSize = sizeof (BITMAPFILEHEADER)       
        + sizeof (BITMAPINFOHEADER)    
        + dwPaletteSize + dwBmBitsSize;      
    bmfHdr.bfSize = dwDIBSize;    
    bmfHdr.bfReserved1 = 0;    
    bmfHdr.bfReserved2 = 0;    
    bmfHdr.bfOffBits = (DWORD)sizeof (BITMAPFILEHEADER)     
        + (DWORD)sizeof (BITMAPINFOHEADER)    
        + dwPaletteSize;      
      
    //   д��λͼ�ļ�ͷ    
    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);    
      
    //   д��λͼ�ļ���������    
    WriteFile(fh, (LPSTR)lpbi, dwDIBSize,  
        &dwWritten, NULL);  
      
    //���            
    GlobalUnlock(hDib);    
    GlobalFree(hDib);    
    CloseHandle(fh);    
  
    return TRUE;      
}  


void CScreen::ShowScreen(char* szBuff , DWORD nLen)
{
  //������Ļ����Ϣ,����ʾ
  CDC memDc;
  CDC* pDC = GetDC();
  memDc.CreateCompatibleDC(pDC);
  int nWidth = *(int*)(szBuff);
  int nHeigth = *(int*)(szBuff + 4);
  CBitmap bmpReciv;
  bmpReciv.CreateCompatibleBitmap(pDC, nWidth, nHeigth);
  int nDataLength = nLen;
  bmpReciv.SetBitmapBits(nDataLength, szBuff);

  if(m_isContinue)
  {
    memDc.SelectObject(&bmpReciv);
  }

  if(m_isCapture) // ����λͼ
  {
    static int nPic = 0;
    ++nPic;
    CString strFileName;
    strFileName.Format(_T("save_%d.bmp") , nPic);

    SaveBitmapToFile(&bmpReciv , strFileName);

    m_isCapture = FALSE;
  }
  BOOL bRet = pDC->BitBlt(0, 0, nWidth, nHeigth, &memDc, 0, 0, SRCCOPY);
}
