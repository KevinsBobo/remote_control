
// remote-controlDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "FileDlg.h"
#include "TaskDlg.h"
#include "Screen.h"
#include "CmdDlg.h"
#include "IOCPModel.h"
#include "afxwin.h"
#include "common.h"

enum emListClient
{
  nSocket = 0,
  address,
  port
};

enum emDlgType
{
  nFileDlg = 0,
  nTaskDlg,
  nScreenDlg,
  nScreenDlgCapture,
  nCmdDlg
};


// CremoteControlDlg �Ի���
class CremoteControlDlg : public CDialogEx
{
// ����
public:
	CremoteControlDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REMOTECONTROL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  CListCtrl m_ListClient;
  afx_msg void OnNMRClickListClient(NMHDR *pNMHDR , LRESULT *pResult);
  afx_msg void OnClose();
  afx_msg void OnBnClickedOk();
  afx_msg void OnMenuFilemanager();
  afx_msg void OnMenuTaskmanager();
  afx_msg void OnMenuScreenCapture();
  afx_msg void OnMenuScreenMonitor();
  afx_msg void OnMuctrlclientCmd();
  CFileDlg m_dlgFile;
  CTaskDlg m_dlgTask;
  CScreen  m_dlgScreen;
  CCmdDlg  m_dlgCmd;

	// ��ǰ�ͻ���������Ϣ������ʱ��������������ʾ�µ�������Ϣ(����CIOCPModel�е���)
	// Ϊ�˼��ٽ�������Ч�ʵ�Ӱ�죬�˴�ʹ��������
	inline void AddInformation(const CString& strInfo)
	{
    // OutputDebugString(strInfo);
		TRACE( strInfo+_T("\n") );
	}

	inline void AddInformation(const CStringA& strInfo)
	{
    // OutputDebugString(strInfo);
		TRACE( strInfo+"\n" );
	}

	CIOCPModel m_IOCP;                         // ��Ҫ������ɶ˿�ģ��
  void InitListClient();
  void InitIOCP();
  CButton m_btnStart;
  CButton m_btnStop;
  afx_msg void OnBnClickedButtonStop();
  CMap<SOCKET , SOCKET , pSocketInfo , pSocketInfo> m_mapSocket;
  void SetNewClient(SOCKET sClient, IN_ADDR addr, u_short nPort, PER_SOCKET_CONTEXT* pPer);
  CCriticalSection m_csSetNewClient;
  void CreateSocketDlg(SOCKET sClient , int nDlg);
  void ReleaseSocketInfo();
  void ReleaseSocketInfo(int nItem);
  void ReleaseSocketInfo(SOCKET sDelClient);
  void CheckHeartBeat();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  void GetHeartBeat(SOCKET sClient);
  pSocketInfo GetSocketInfo(SOCKET sClient);
  void GetCmdText(SOCKET sClient , char* szBuff);
  void GetScreenBmp(SOCKET sClient , char* szBuff, DWORD nLen);
  void GetProcessInfo(SOCKET sClient , char* szBuff);
  void GetProcessKill(SOCKET sClient , DWORD nRet);
  void GetFileDisk(SOCKET sClient , char* szBuff);
  void GetFileList(SOCKET sClient , char* szBuff);
};
