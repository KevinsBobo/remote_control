// CmdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "remote-control.h"
#include "CmdDlg.h"
#include "afxdialogex.h"
#include "common.h"

// CCmdDlg 对话框

IMPLEMENT_DYNAMIC(CCmdDlg, CDialogEx)

CCmdDlg::CCmdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCmdDlg::IDD, pParent)
  , m_sClient(INVALID_SOCKET)
  , m_pInfo(NULL)
{

}

CCmdDlg::~CCmdDlg()
{
}

void CCmdDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX , IDC_EDIT_CMD , m_EditCmd);
}


BEGIN_MESSAGE_MAP(CCmdDlg, CDialogEx)
  ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CCmdDlg 消息处理程序


BOOL CCmdDlg::PreTranslateMessage(MSG* pMsg)
{
  // TODO:  在此添加专用代码和/或调用基类
  if((pMsg->message == WM_CHAR) && (pMsg->wParam == VK_RETURN))
  {
    if(pMsg->hwnd = m_EditCmd.m_hWnd)
    {
      PostCmdInput();
    }
  }


  return CDialogEx::PreTranslateMessage(pMsg);
}


void CCmdDlg::PostCmdInput()
{
  if(m_sClient == INVALID_SOCKET || m_sClient == SOCKET_ERROR)
  {
    return ;
  }

  int nLineCnt = m_EditCmd.GetLineCount();
  TCHAR szBuff[ MAXBYTE ] = { 0 };
  TCHAR* pBuffer = szBuff;
  
  int nCopyCnt = m_EditCmd.GetLine(nLineCnt - 1, pBuffer, MAXBYTE);
  pBuffer[nCopyCnt] = _T('\0');
  int nSize = WideCharToMultiByte(CP_OEMCP, 0, pBuffer, nCopyCnt + 1, NULL, 0, NULL, FALSE);

  char* pMcbsBuffer = new char[nSize];
  WideCharToMultiByte(CP_OEMCP, 0, pBuffer, nCopyCnt + 1, pMcbsBuffer, nSize, NULL, FALSE);

  char  cmdBuf[MAXBYTE] = { 0 };
  char* szInput = NULL;
  DWORD dwMbLen = strlen(pMcbsBuffer);
  DWORD dwSize = 0;
  for (DWORD i = 0; i < dwMbLen; i++)
  {
    if (pMcbsBuffer[i] == _T('>'))
    {
      szInput = &pMcbsBuffer[i + 1];
      strcpy_s(cmdBuf, MAXBYTE, szInput);
      dwSize = dwMbLen - i;
      cmdBuf[dwSize - 1] = '\n';
      cmdBuf[dwSize] = '\0';
      break;
    }
  }

  m_pInfo->csSend.Lock();
  SendPacket(m_sClient , emMessType::cmdstext , cmdBuf , strlen(cmdBuf) + 1);
  m_pInfo->csSend.Unlock();

  m_EditCmd.Undo();
}



void CCmdDlg::OnShowWindow(BOOL bShow , UINT nStatus)
{
  CDialogEx::OnShowWindow(bShow , nStatus);

  // TODO:  在此处添加消息处理程序代码
  m_pInfo->csSend.Lock();
  SendPacket(m_sClient , emMessType::cmdstart , NULL , 0);
  m_pInfo->csSend.Unlock();
}


void CCmdDlg::SetCmdEditText(char* szBuff)
{
  if(szBuff == NULL)
  {
    return ;
  }

  CString str;
  str = szBuff;
  m_EditCmd.SetSel(-1);
  m_EditCmd.ReplaceSel(str.GetBuffer());
}
