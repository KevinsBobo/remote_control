
// remote-controlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "remote-control.h"
#include "remote-controlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CremoteControlDlg 对话框



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


// CremoteControlDlg 消息处理程序

BOOL CremoteControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
  InitIOCP();
  InitListClient();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CremoteControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CremoteControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CremoteControlDlg::OnNMRClickListClient(NMHDR *pNMHDR , LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  // TODO:  在此添加控件通知处理程序代码
  *pResult = 0;

  int nSel = m_ListClient.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);

  // TRACE("%d\n" , nSel);

  if(nSel >= 0)
  {
    CMenu   menu;   //定义下面要用到的cmenu对象
    menu.LoadMenu(IDR_MENU_MAIN); //装载自定义的右键菜单 
    CMenu   *pContextMenu=menu.GetSubMenu(0); //获取第一个弹出菜单，所以第一个菜单必须有子菜单 
    CPoint point1;//定义一个用于确定光标位置的位置  
    GetCursorPos(&point1);//获取当前光标的位置，以便使得菜单可以跟随光标

    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point1.x,point1.y, this); //在指定位置显示弹出菜单
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
	// 绑定主界面指针(为了方便在界面中显示信息 )
	m_IOCP.SetMainDlg(this);
}


void CremoteControlDlg::OnBnClickedOk()
{
  // TODO:  在此添加控件通知处理程序代码
  // CDialogEx::OnOK();
	if( false==m_IOCP.Start() )
	{
		AfxMessageBox(_T("服务器启动失败！"));
		return;	
	}
  m_btnStart.EnableWindow(FALSE);
  m_btnStop.EnableWindow(TRUE);
  // 心跳包时间设置
  SetTimer(emTimerType::tsHeartBeat , HEARTEBEAT_TIME * 2 , NULL);
}


void CremoteControlDlg::OnBnClickedButtonStop()
{
  // TODO:  在此添加控件通知处理程序代码
  KillTimer(emTimerType::tsHeartBeat);
	m_IOCP.Stop();
  ReleaseSocketInfo();
  m_btnStart.EnableWindow(TRUE);
  m_btnStop.EnableWindow(FALSE);
}


void CremoteControlDlg::OnClose()
{
  // TODO:  在此添加消息处理程序代码和/或调用默认值
  OnBnClickedButtonStop();

  CDialogEx::OnClose();
}


void CremoteControlDlg::OnMenuFilemanager()
{
  // TODO:  在此添加命令处理程序代码
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
  // TODO:  在此添加命令处理程序代码
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
  // TODO:  在此添加命令处理程序代码
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
  // TODO:  在此添加命令处理程序代码
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
  // TODO:  在此添加命令处理程序代码
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
      //通知客户端发送截图
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
        // 移除客户端
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
  // TODO:  在此添加消息处理程序代码和/或调用默认值
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
