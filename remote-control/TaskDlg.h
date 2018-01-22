#pragma once
#include "Resource.h"
#include "common.h"
#include "afxcmn.h"
#include "afxwin.h"


// CTaskDlg �Ի���

class CTaskDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskDlg)

public:
	CTaskDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTaskDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_TASK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
  SOCKET m_sClient;
  pSocketInfo m_pInfo;
  afx_msg void OnShowWindow(BOOL bShow , UINT nStatus);
  void GetProcessInfo(char* szBuff);
  virtual BOOL OnInitDialog();
  CListCtrl m_ListTask;
  CCriticalSection m_csItem;
  afx_msg void OnBnClickedButtonKillprocess();
  afx_msg void OnBnClickedButtonTaskrefresh();
  CStatic m_StaticText;
  void GetProcessKill(DWORD nRet);
};
