#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "common.h"


// CCmdDlg �Ի���

class CCmdDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCmdDlg)

public:
	CCmdDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCmdDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_CMD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
