#pragma once
#include "Resource.h"
#include "common.h"


// CScreen �Ի���

class CScreen : public CDialogEx
{
	DECLARE_DYNAMIC(CScreen)

public:
	CScreen(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScreen();

// �Ի�������
	enum { IDD = IDD_DIALOG_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
  SOCKET m_sClient;
  afx_msg void OnShowWindow(BOOL bShow , UINT nStatus);
  afx_msg void OnClose();
  BOOL m_isContinue;
  BOOL m_isCapture;
  void GetScreen(char* szBuff , DWORD nLen);
  void ShowScreen(char* szBuff , DWORD nLen);
  pSocketInfo m_pInfo;
};
