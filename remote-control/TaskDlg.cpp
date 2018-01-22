// TaskDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "remote-control.h"
#include "TaskDlg.h"
#include "afxdialogex.h"


// CTaskDlg 对话框

IMPLEMENT_DYNAMIC(CTaskDlg, CDialogEx)

CTaskDlg::CTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTaskDlg::IDD, pParent)
  , m_pInfo(NULL)
{

}

CTaskDlg::~CTaskDlg()
{
}

void CTaskDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX , IDC_LIST_TASK , m_ListTask);
  DDX_Control(pDX , IDC_STATIC_TEXT_BOTTOM , m_StaticText);
}


BEGIN_MESSAGE_MAP(CTaskDlg, CDialogEx)
  ON_WM_SHOWWINDOW()
  ON_BN_CLICKED(IDC_BUTTON_KILLPROCESS , &CTaskDlg::OnBnClickedButtonKillprocess)
  ON_BN_CLICKED(IDC_BUTTON_TASKREFRESH , &CTaskDlg::OnBnClickedButtonTaskrefresh)
END_MESSAGE_MAP()


// CTaskDlg 消息处理程序


void CTaskDlg::OnShowWindow(BOOL bShow , UINT nStatus)
{
  CDialogEx::OnShowWindow(bShow , nStatus);

  // TODO:  在此处添加消息处理程序代码
  m_StaticText.SetWindowTextW(_T(""));
  m_ListTask.DeleteAllItems();
  m_pInfo->csSend.Lock();
  SendPacket(m_sClient , emMessType::processrequest, NULL , 0);
  m_pInfo->csSend.Unlock();
}


void CTaskDlg::GetProcessInfo(char* szBuff)
{
  DWORD PID = *(DWORD*)szBuff;
  TCHAR* strInfo = (TCHAR*)(szBuff + sizeof(DWORD));
  CString strPID;
  strPID.Format(_T("%u") , PID);
  m_csItem.Lock();
  m_ListTask.InsertItem(0 , strPID.GetBuffer());
  m_ListTask.SetItemData(0 , PID);
  m_ListTask.SetItemText(0 , 1 , strInfo);
  m_csItem.Unlock();
}


BOOL CTaskDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  在此添加额外的初始化
  m_ListTask.ModifyStyle(0, LVS_REPORT);
  DWORD dwStye = m_ListTask.GetExtendedStyle();
  dwStye = dwStye | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
  m_ListTask.SetExtendedStyle(dwStye);
  m_ListTask.InsertColumn(0 , _T("PID"), 0, 100);
  m_ListTask.InsertColumn(1 , _T("Process"), 0, 400);

  return TRUE;  // return TRUE unless you set the focus to a control
  // 异常:  OCX 属性页应返回 FALSE
}


void CTaskDlg::OnBnClickedButtonKillprocess()
{
  // TODO:  在此添加控件通知处理程序代码
  int nSel = m_ListTask.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);
  if(nSel < 0)
  {
    MessageBox(_T("请选择进程！"));
    return ;
  }

  DWORD PID = m_ListTask.GetItemData(nSel);
  m_pInfo->csSend.Lock();
  SendPacket(m_sClient , emMessType::processkill, (char*)&PID , sizeof(DWORD));
  m_pInfo->csSend.Unlock();

}


void CTaskDlg::OnBnClickedButtonTaskrefresh()
{
  // TODO:  在此添加控件通知处理程序代码
  m_StaticText.SetWindowTextW(_T(""));
  m_ListTask.DeleteAllItems();
  m_pInfo->csSend.Lock();
  SendPacket(m_sClient , emMessType::processrequest, NULL , 0);
  m_pInfo->csSend.Unlock();
}


void CTaskDlg::GetProcessKill(DWORD nRet)
{
  if(nRet == emMessType::processkillsecc)
  {
    m_StaticText.SetWindowTextW(_T("结束成功！"));
  }
  else if(nRet == emMessType::processkillfail)
  {
    m_StaticText.SetWindowTextW(_T("结束失败！"));
  }
}
