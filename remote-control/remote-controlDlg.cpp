
// remote-controlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "remote-control.h"
#include "remote-controlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()


// CremoteControlDlg �Ի���



CremoteControlDlg::CremoteControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CremoteControlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CremoteControlDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX , IDC_LIST_CLIENT , m_ListClient);
  DDX_Control(pDX , IDOK , m_btnStart);
  DDX_Control(pDX , IDC_BUTTON_STOP , m_btnStop);
}

BEGIN_MESSAGE_MAP(CremoteControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_NOTIFY(NM_RCLICK , IDC_LIST_CLIENT , &CremoteControlDlg::OnNMRClickListClient)
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDOK , &CremoteControlDlg::OnBnClickedOk)
  ON_COMMAND(ID_ADMIN_FILEMANAGER , &CremoteControlDlg::OnMenuFilemanager)
  ON_COMMAND(ID_ADMIN_TASKMANAGER , &CremoteControlDlg::OnMenuTaskmanager)
  ON_COMMAND(ID_SCREEN_CAPTURE , &CremoteControlDlg::OnMenuScreenCapture)
  ON_COMMAND(ID_SCREEN_MONITOR , &CremoteControlDlg::OnMenuScreenMonitor)
  ON_COMMAND(ID_MUCTRLCLIENT_CMD , &CremoteControlDlg::OnMuctrlclientCmd)
  ON_BN_CLICKED(IDC_BUTTON_STOP , &CremoteControlDlg::OnBnClickedButtonStop)
  ON_WM_TIMER()
END_MESSAGE_MAP()


// CremoteControlDlg ��Ϣ�������

BOOL CremoteControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
  InitIOCP();
  InitListClient();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CremoteControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CremoteControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CremoteControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CremoteControlDlg::OnNMRClickListClient(NMHDR *pNMHDR , LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  *pResult = 0;

  int nSel = m_ListClient.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);

  // TRACE("%d\n" , nSel);

  if(nSel >= 0)
  {
    CMenu   menu;   //��������Ҫ�õ���cmenu����
    menu.LoadMenu(IDR_MENU_MAIN); //װ���Զ�����Ҽ��˵� 
    CMenu   *pContextMenu=menu.GetSubMenu(0); //��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵� 
    CPoint point1;//����һ������ȷ�����λ�õ�λ��  
    GetCursorPos(&point1);//��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����

    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point1.x,point1.y, this); //��ָ��λ����ʾ�����˵�
  }


}


void CremoteControlDlg::InitListClient()
{
  m_ListClient.ModifyStyle(0, LVS_REPORT);
  DWORD dwStye = m_ListClient.GetExtendedStyle();
  dwStye = dwStye | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
  m_ListClient.SetExtendedStyle(dwStye);
  m_ListClient.InsertColumn(emListClient::nSocket , _T("Socket"), 0, 100);
  m_ListClient.InsertColumn(emListClient::address , _T("Address"), 0, 200);
  m_ListClient.InsertColumn(emListClient::port , _T("Port"), 0, 100);
}


void CremoteControlDlg::InitIOCP()
{
	// ��������ָ��(Ϊ�˷����ڽ�������ʾ��Ϣ )
	m_IOCP.SetMainDlg(this);
}


void CremoteControlDlg::OnBnClickedOk()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  // CDialogEx::OnOK();
	if( false==m_IOCP.Start() )
	{
		AfxMessageBox(_T("����������ʧ�ܣ�"));
		return;	
	}
  m_btnStart.EnableWindow(FALSE);
  m_btnStop.EnableWindow(TRUE);
  // ������ʱ������
  SetTimer(emTimerType::tsHeartBeat , HEARTEBEAT_TIME * 2 , NULL);
}


void CremoteControlDlg::OnBnClickedButtonStop()
{
  // TODO:  �ڴ���ӿؼ�֪ͨ����������
  KillTimer(emTimerType::tsHeartBeat);
	m_IOCP.Stop();
  ReleaseSocketInfo();
  m_btnStart.EnableWindow(TRUE);
  m_btnStop.EnableWindow(FALSE);
}


void CremoteControlDlg::OnClose()
{
  // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
  OnBnClickedButtonStop();

  CDialogEx::OnClose();
}


void CremoteControlDlg::OnMenuFilemanager()
{
  // TODO:  �ڴ���������������
  int nSel = m_ListClient.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);
  if(nSel < 0)
  {
    return;
  }
  
  SOCKET sClient = m_ListClient.GetItemData(nSel);
  CreateSocketDlg(sClient , emDlgType::nFileDlg);
}


void CremoteControlDlg::OnMenuTaskmanager()
{
  // TODO:  �ڴ���������������
  int nSel = m_ListClient.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);
  if(nSel < 0)
  {
    return;
  }
  
  SOCKET sClient = m_ListClient.GetItemData(nSel);
  CreateSocketDlg(sClient , emDlgType::nTaskDlg);
}


void CremoteControlDlg::OnMenuScreenCapture()
{
  // TODO:  �ڴ���������������
  int nSel = m_ListClient.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);
  if(nSel < 0)
  {
    return;
  }
  
  SOCKET sClient = m_ListClient.GetItemData(nSel);
  CreateSocketDlg(sClient , emDlgType::nScreenDlgCapture);
}


void CremoteControlDlg::OnMenuScreenMonitor()
{
  // TODO:  �ڴ���������������
  int nSel = m_ListClient.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);
  if(nSel < 0)
  {
    return;
  }
  
  SOCKET sClient = m_ListClient.GetItemData(nSel);
  CreateSocketDlg(sClient , emDlgType::nScreenDlg);
}


void CremoteControlDlg::OnMuctrlclientCmd()
{
  // TODO:  �ڴ���������������
  int nSel = m_ListClient.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);
  if(nSel < 0)
  {
    return;
  }
  
  SOCKET sClient = m_ListClient.GetItemData(nSel);
  CreateSocketDlg(sClient , emDlgType::nCmdDlg);
}


void CremoteControlDlg::SetNewClient(SOCKET sClient, IN_ADDR addr, u_short nPort, PER_SOCKET_CONTEXT* pPer)
{
  m_csSetNewClient.Lock();
  SocketInfo* pInfo = new SocketInfo;
  pInfo->isHeart = TRUE;
  m_mapSocket.SetAt(sClient , pInfo);
  CString strInfo;
  strInfo.Format(_T("%u") , sClient);
  m_ListClient.InsertItem(0 , strInfo);
  m_ListClient.SetItemData(0 , sClient);
  strInfo = inet_ntoa(addr);
  m_ListClient.SetItemText(0 , emListClient::address , strInfo);
  strInfo.Format(_T("%hd") , nPort);
  m_ListClient.SetItemText(0 , emListClient::port , strInfo);
  m_csSetNewClient.Unlock();
}


void CremoteControlDlg::CreateSocketDlg(SOCKET sClient , int nDlg)
{
  pSocketInfo pInfo = NULL;
  BOOL nRet = m_mapSocket.Lookup(sClient , pInfo);
  if(!nRet || pInfo == NULL)
  {
    return;
  }

  switch(nDlg)
  {
    case emDlgType::nFileDlg:
    {
      if(pInfo->pFileWnd == NULL)
      {
        pInfo->pFileWnd = new CFileDlg;
        pInfo->pFileWnd->Create(IDD_DIALOG_FILE , this);
      }
      pInfo->pFileWnd->m_sClient = sClient;
      pInfo->pFileWnd->m_pInfo = pInfo;
      pInfo->pFileWnd->ShowWindow(SW_SHOW);
    }
      break;
    case emDlgType::nTaskDlg:
    {
      if(pInfo->pTaskWnd == NULL)
      {
        pInfo->pTaskWnd = new CTaskDlg;
        pInfo->pTaskWnd->Create(IDD_DIALOG_TASK , this);
      }
      pInfo->pTaskWnd->m_sClient = sClient;
      pInfo->pTaskWnd->m_pInfo = pInfo;
      pInfo->pTaskWnd->ShowWindow(SW_SHOW);
    }
      break;
    case emDlgType::nScreenDlg:
    {
      if(pInfo->pScreenWnd == NULL)
      {
        pInfo->pScreenWnd = new CScreen;
        pInfo->pScreenWnd->Create(IDD_DIALOG_SCREEN , this);
      }
      pInfo->pScreenWnd->m_sClient = sClient;
      pInfo->pScreenWnd->m_pInfo = pInfo;
      pInfo->pScreenWnd->ShowWindow(SW_SHOW);
    }
      break;
    case emDlgType::nScreenDlgCapture:
    {
      if(pInfo->pScreenWnd == NULL)
      {
        pInfo->pScreenWnd = new CScreen;
        pInfo->pScreenWnd->Create(IDD_DIALOG_SCREEN , this);
      }
      pInfo->pScreenWnd->m_sClient = sClient;
      pInfo->pScreenWnd->m_pInfo = pInfo;
      pInfo->pScreenWnd->m_isCapture = TRUE;
      //֪ͨ�ͻ��˷��ͽ�ͼ
      pInfo->csSend.Lock();
      SendPacket(sClient, emMessType::screenrecv, NULL, 0);
      pInfo->csSend.Unlock();
    }
      break;
    case emDlgType::nCmdDlg:
    {
      if(pInfo->pCmdWnd == NULL)
      {
        pInfo->pCmdWnd = new CCmdDlg;
        pInfo->pCmdWnd->Create(IDD_DIALOG_CMD , this);
      }
      pInfo->pCmdWnd->m_sClient = sClient;
      pInfo->pCmdWnd->m_pInfo = pInfo;
      pInfo->pCmdWnd->ShowWindow(SW_SHOW);
    }
      break;
    default:
      break;
  }
}

void ReleaseSubWnd(pSocketInfo pInfo)
{
  if(pInfo == NULL)
  {
    return;
  }
  if(pInfo->pFileWnd != NULL)
  {
    pInfo->pFileWnd->ShowWindow(SW_HIDE);
    // delete pInfo->pFileWnd;
  }
  if(pInfo->pTaskWnd != NULL)
  {
    pInfo->pTaskWnd->ShowWindow(SW_HIDE);
    // delete pInfo->pTaskWnd;
  }
  if(pInfo->pScreenWnd != NULL)
  {
    pInfo->pScreenWnd->ShowWindow(SW_HIDE);
    // delete pInfo->pScreenWnd;
  }
  if(pInfo->pCmdWnd != NULL)
  {
    pInfo->pCmdWnd->ShowWindow(SW_HIDE);
    // delete pInfo->pCmdWnd;
  }
}

void CremoteControlDlg::ReleaseSocketInfo()
{
  m_csSetNewClient.Lock();
  int nClient = m_ListClient.GetItemCount();
  pSocketInfo pInfo = NULL;
  SOCKET sClient;
  BOOL nRet = FALSE;
  for(int i = 0; i < nClient; ++i)
  {
    sClient = m_ListClient.GetItemData(i);
    nRet = m_mapSocket.Lookup(sClient , pInfo);
    if(nRet && pInfo != NULL)
    {
      ReleaseSubWnd(pInfo);
      delete pInfo;
      pInfo = NULL;
      nRet = FALSE;
    }
    closesocket(sClient);
  }
  
  m_mapSocket.RemoveAll();
  m_ListClient.DeleteAllItems();
  m_csSetNewClient.Unlock();
}


void CremoteControlDlg::ReleaseSocketInfo(SOCKET sDelClien)
{
  m_csSetNewClient.Lock();
  int nClient = m_ListClient.GetItemCount();
  pSocketInfo pInfo = NULL;
  SOCKET sClient;
  BOOL nRet = FALSE;
  for(int i = 0; i < nClient; ++i)
  {
    sClient = m_ListClient.GetItemData(i);
    if(sClient != sDelClien)
    {
      continue;
    }
    nRet = m_mapSocket.Lookup(sClient , pInfo);
    if(nRet && pInfo != NULL)
    {
      ReleaseSubWnd(pInfo);
      delete pInfo;
      pInfo = NULL;
      nRet = FALSE;
    }
    m_mapSocket.RemoveKey(sClient);
    m_ListClient.DeleteItem(i);
    break;
  }
  
  m_csSetNewClient.Unlock();
}


void CremoteControlDlg::ReleaseSocketInfo(int nItem)
{
  int nClient = m_ListClient.GetItemCount();
  if(nItem >= nClient)
  {
    return ;
  }
  pSocketInfo pInfo = NULL;
  SOCKET sClient;
  BOOL nRet = FALSE;
  sClient = m_ListClient.GetItemData(nItem);
  nRet = m_mapSocket.Lookup(sClient , pInfo);
  if(nRet && pInfo != NULL)
  {
    ReleaseSubWnd(pInfo);
    delete pInfo;
    pInfo = NULL;
    nRet = FALSE;
  }
  
  m_mapSocket.RemoveKey(sClient);
  m_ListClient.DeleteItem(nItem);
}



void CremoteControlDlg::CheckHeartBeat()
{
  m_csSetNewClient.Lock();
  int nClient = m_ListClient.GetItemCount();
  pSocketInfo pInfo = NULL;
  SOCKET sClient;
  BOOL nRet = FALSE;
  for(int i = 0; i < nClient; ++i)
  {
    sClient = m_ListClient.GetItemData(i);
    nRet = m_mapSocket.Lookup(sClient , pInfo);
    if(nRet && pInfo != NULL)
    {
      if(pInfo->isHeart == TRUE)
      {
        pInfo->isHeart = FALSE;
      }
      else
      {
        // �Ƴ��ͻ���
        ReleaseSocketInfo(i);
        closesocket(sClient);
        --i;
        nClient = m_ListClient.GetItemCount();
      }
    }
  }
  
  m_csSetNewClient.Unlock();

}


void CremoteControlDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
  switch(nIDEvent)
  {
    case emTimerType::tsHeartBeat:
      CheckHeartBeat();
      break;
    default:
      break;
  }

  CDialogEx::OnTimer(nIDEvent);
}


void CremoteControlDlg::GetHeartBeat(SOCKET sClient)
{
  pSocketInfo pInfo = NULL;
  BOOL nRet = m_mapSocket.Lookup(sClient , pInfo);
  if(nRet && pInfo != NULL)
  {
    pInfo->isHeart = TRUE;
  }
}


pSocketInfo CremoteControlDlg::GetSocketInfo(SOCKET sClient)
{
  pSocketInfo pInfo = NULL;
  m_mapSocket.Lookup(sClient , pInfo);

  return pInfo;
}


void CremoteControlDlg::GetCmdText(SOCKET sClient , char* szBuff)
{
  pSocketInfo pInfo = NULL;
  pInfo = GetSocketInfo(sClient);
  if(pInfo->pCmdWnd != NULL)
  {
    pInfo->pCmdWnd->SetCmdEditText(szBuff);
  }
}


void CremoteControlDlg::GetScreenBmp(SOCKET sClient , char* szBuff , DWORD nLen)
{
  pSocketInfo pInfo = NULL;
  pInfo = GetSocketInfo(sClient);
  if(pInfo->pScreenWnd != NULL)
  {
    pInfo->pScreenWnd->GetScreen(szBuff, nLen);
  }
}


void CremoteControlDlg::GetProcessInfo(SOCKET sClient , char* szBuff)
{
  pSocketInfo pInfo = NULL;
  pInfo = GetSocketInfo(sClient);
  if(pInfo->pTaskWnd != NULL)
  {
    pInfo->pTaskWnd->GetProcessInfo(szBuff);
  }
}


void CremoteControlDlg::GetProcessKill(SOCKET sClient , DWORD nRet)
{
  pSocketInfo pInfo = NULL;
  pInfo = GetSocketInfo(sClient);
  if(pInfo->pTaskWnd != NULL)
  {
    pInfo->pTaskWnd->GetProcessKill(nRet);
  }
}


void CremoteControlDlg::GetFileDisk(SOCKET sClient , char* szBuff)
{
  pSocketInfo pInfo = NULL;
  pInfo = GetSocketInfo(sClient);
  if(pInfo->pFileWnd != NULL)
  {
    pInfo->pFileWnd->GetFileDisk(szBuff);
  }
}


void CremoteControlDlg::GetFileList(SOCKET sClient , char* szBuff)
{
  pSocketInfo pInfo = NULL;
  pInfo = GetSocketInfo(sClient);
  if(pInfo->pFileWnd != NULL)
  {
    pInfo->pFileWnd->GetFileList(szBuff);
  }
}
