#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "common.h"


// CCmdDlg 对话框

class CCmdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCmdDlg)

public:
	CCmdDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCmdDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CMD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
  SOCKET m_sClient;
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  CEdit m_EditCmd;
  void PostCmdInput();
  afx_msg void OnShowWindow(BOOL bShow , UINT nStatus);
  void SetCmdEditText(char* szBuff);
  pSocketInfo m_pInfo;
};
