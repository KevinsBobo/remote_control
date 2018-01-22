#pragma once
#include "Resource.h"
#include "common.h"
#include "afxcmn.h"
#include "afxwin.h"


// CTaskDlg 对话框

class CTaskDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskDlg)

public:
	CTaskDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_TASK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
