// Screen.cpp : 实现文件
//

#include "stdafx.h"
#include "remote-control.h"
#include "Screen.h"
#include "afxdialogex.h"
#include "zlib.h"
#include "common.h"
#include "myFile.h"


// CScreen 对话框

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


// CScreen 消息处理程序


void CScreen::OnShowWindow(BOOL bShow , UINT nStatus)
{
  CDialogEx::OnShowWindow(bShow , nStatus);

  // TODO:  在此处添加消息处理程序代码
  m_isContinue = TRUE;
  //通知客户端开始发送
  m_pInfo->csSend.Lock();
  SendPacket(m_sClient, emMessType::screenrecv, NULL, 0);
  m_pInfo->csSend.Unlock();
}


void CScreen::OnClose()
{
  // TODO:  在此添加消息处理程序代码和/或调用默认值
  m_isContinue = FALSE;

  CDialogEx::OnClose();
}


void CScreen::GetScreen(char* szBuff , DWORD nLen)
{
    //解压缩
  uLong nUnCompressLength = *(uLong*)szBuff;

  uLong nUnCompressLength2 = (uLong)(nUnCompressLength*2.2);

  char* pUncompressBuf = new char[nUnCompressLength2];

  if (pUncompressBuf == NULL)
  {
      TRACE("解压缩申请空间失败");
      return;
  }

  DWORD nCompressedlen = nLen;
  char* pData = szBuff + sizeof(uLong);
  //开始解压数据 
  int nRet = uncompress(
      (Bytef*)pUncompressBuf,      //解压缓冲区
      (uLongf*)&nUnCompressLength2,//解压后的大小
      (Bytef*)(pData),         //源数据区
      (u_long)nCompressedlen); // 压缩后的大小

  if (nRet != Z_OK)
  {
      TRACE("解压失败");
      return;
  }
  else
  {
    //把数据传递给CapturScreen窗口显示
    ShowScreen(pUncompressBuf, nUnCompressLength);
  }


  if(m_isContinue)
  {
    //通知客户端发送下一帧
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
    HBITMAP hBitmap;    // 为刚才的屏幕位图句柄  
    HDC hDC; //设备描述表    
    int iBits; //当前显示分辨率下每个像素所占字节数    
    WORD wBitCount; //位图中每个像素所占字节数    
    DWORD dwPaletteSize = 0, //定义调色板大小  
        dwBmBitsSize,  //位图中像素字节大小     
        dwDIBSize,   //位图文件大小  
        dwWritten;  //写入文件字节数  
    BITMAP Bitmap; //位图属性结构    
    BITMAPFILEHEADER bmfHdr; //位图文件头结构    
    BITMAPINFOHEADER bi; //位图信息头结构      
    LPBITMAPINFOHEADER lpbi; //指向位图信息头结构    
    HANDLE fh,   //定义文件  
        hDib,    //分配内存句柄  
        hPal,   //调色板句柄  
        hOldPal = NULL;  
      
    //计算位图文件每个像素所占字节数    
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
      
    //计算调色板大小    
    if (wBitCount <= 8)    
        dwPaletteSize = (1 << wBitCount) * sizeof (RGBQUAD);  
      
    //设置位图信息头结构    
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
      
    //为位图内容分配内存    
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof (BITMAPINFOHEADER));    
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);    
    *lpbi = bi;  
      
    // 处理调色板         
    hPal = GetStockObject(DEFAULT_PALETTE);    
    if (hPal)    
    {    
        hDC = ::GetDC(NULL);      
        hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);    
        RealizePalette(hDC);  
    }    
      
    //   获取该调色板下新的像素值    
    GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight,    
        (LPSTR)lpbi + sizeof (BITMAPINFOHEADER) + dwPaletteSize,  
        (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);    
      
    //恢复调色板       
    if (hOldPal)      
    {    
        SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);    
        RealizePalette(hDC);    
        ::ReleaseDC(NULL, hDC);  
    }    
      
    //创建位图文件              
    fh = CreateFile(lpFileName, GENERIC_WRITE,  
        0, NULL, CREATE_ALWAYS,  
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);    
      
    if (fh == INVALID_HANDLE_VALUE)  
        return FALSE;    
      
    //   设置位图文件头    
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
      
    //   写入位图文件头    
    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);    
      
    //   写入位图文件其余内容    
    WriteFile(fh, (LPSTR)lpbi, dwDIBSize,  
        &dwWritten, NULL);  
      
    //清除            
    GlobalUnlock(hDib);    
    GlobalFree(hDib);    
    CloseHandle(fh);    
  
    return TRUE;      
}  


void CScreen::ShowScreen(char* szBuff , DWORD nLen)
{
  //接收屏幕的信息,并显示
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

  if(m_isCapture) // 保存位图
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
