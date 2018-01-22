// TaskDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "remote-control.h"
#include "TaskDlg.h"
#include "afxdialogex.h"


// CTaskDlg �Ի���

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


// CTaskDlg ��Ϣ�������


void CTaskDlg::OnShowWindow(BOOL bShow , UINT nStatus)
{
  CDialogEx::OnShowWindow(bShow , nStatus);

  // TODO:  �ڴ˴������Ϣ����������
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

  // TODO:  �ڴ���Ӷ���ĳ�ʼ��
  m_ListTask.ModifyStyle(0, LVS_REPORT);
  DWORD dwStye = m_ListTask.GetExtendedStyle();
  dwStye = dwStye | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
  m_ListTask.SetExtendedStyle(dwStye);
  m_ListTask.InsertColumn(0 , _T("PID"), 0, 100);
  m_ListTask.InsertColumn(1 , _T("Process"), 0, 400);

  return TRUE;  // return TRUE unless you set the focus to a control
  // �쳣:  OCX ����ҳӦ���� FALSE
}


void CTaskDlg::OnBnClickedButtonKillprocess()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  int nSel = m_ListTask.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);
  if(nSel < 0)
  {
    MessageBox(_T("��ѡ����̣�"));
    return ;
  }

  DWORD PID = m_ListTask.GetItemData(nSel);
  m_pInfo->csSend.Lock();
  SendPacket(m_sClient , emMessType::processkill, (char*)&PID , sizeof(DWORD));
  m_pInfo->csSend.Unlock();

}


void CTaskDlg::OnBnClickedButtonTaskrefresh()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
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
    m_StaticText.SetWindowTextW(_T("�����ɹ���"));
  }
  else if(nRet == emMessType::processkillfail)
  {
    m_StaticText.SetWindowTextW(_T("����ʧ�ܣ�"));
  }
}
