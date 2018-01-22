
// local-clientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "local-client.h"
#include "local-clientDlg.h"
#include "afxdialogex.h"
#include "zlib.h"
#include <tlhelp32.h>
#include <Psapi.h>
#include "Buffer.h"

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


// CLocalClientDlg 对话框



CLocalClientDlg::CLocalClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLocalClientDlg::IDD, pParent)
  , m_sServer(INVALID_SOCKET)
  , m_hCmdReadPipe(NULL)
  , m_hCmdWritePipe(NULL)
  , m_hReadPipe(NULL)
  , m_hWritePipe(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLocalClientDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX , IDC_STATIC_STATE , m_StaticText);
}

BEGIN_MESSAGE_MAP(CLocalClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_WM_TIMER()
  ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLocalClientDlg 消息处理程序

BOOL CLocalClientDlg::OnInitDialog()
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
  InitSocket();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLocalClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLocalClientDlg::OnPaint()
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
HCURSOR CLocalClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// 接收消息线程回调
DWORD WINAPI RecMessageThreadProc(LPVOID lpParameter)
{
  CLocalClientDlg* obj = (CLocalClientDlg*)lpParameter;
  SOCKET sServer = obj->m_sServer;
  DWORD dwWritedBytes = 0;

  char* szBuff = NULL; //  = new char[ MAX_BUFFER_LEN ];
  char cRet = '\0';
  int nRecvedBytes = 0;
  DWORD nMessLen = 0;
  BYTE nMessType = -1;
  while(true)
  {
    szBuff = RecivPacket(sServer);

    if(szBuff != NULL)
    {
      nMessLen = *(DWORD*)szBuff;
      if(nMessLen > 0)
      {
        nMessType = *(BYTE*)(szBuff + sizeof(DWORD));
        switch(nMessType)
        {
          case emMessType::cmdstart:
            obj->CmdStart();
            break;
          case emMessType::cmdstext:
            obj->CmdInput(szBuff + sizeof(DWORD) + sizeof(BYTE) , nMessLen - sizeof(BYTE));
            break;
          case emMessType::screenrecv:
            obj->CaptureScreen();
            break;
          case emMessType::processrequest:
            obj->GetProcess();
            break;
          case emMessType::processkill:
            obj->KillProcess(*(DWORD*)(szBuff + sizeof(DWORD) + sizeof(BYTE)));
            break;
          case emMessType::filediskrequest:
            obj->OnDiskMsg();
            break;
          case emMessType::filepathrequest:
            obj->OnFileMsg(szBuff + sizeof(DWORD) + sizeof(BYTE));
            break;
          default:
            break;
        }
      }

      delete[] szBuff;
    }
    else
    {
      break;
    }
  }

  return 0;
}


// 发送消息线程回调
DWORD WINAPI SendCmdMessageThreadProc(LPVOID lpParameter)
{
  CLocalClientDlg* obj = (CLocalClientDlg*)lpParameter;
  SOCKET sClient = obj->m_sServer;
  int nSendedBytes = 0;

  DWORD dwReadedBytes = 0;
  DWORD dwBytesRead = 0;
  DWORD dwTotalBytesAvail = 0;
  DWORD dwBytesLeftThisMessage = 0;
  char szReadBuf[ 8192 ] = { 0 };
  TCHAR szReadBufW[ 8192 ] = { 0 };
  if(szReadBuf == NULL || szReadBufW == NULL)
  {
    return -1;
  }

  while(true)
  {
    Sleep(50);
    BOOL bRet = PeekNamedPipe(obj->m_hReadPipe,
                              NULL,
                              0,
                              &dwBytesRead,
                              &dwTotalBytesAvail,
                              &dwBytesLeftThisMessage);

    if(bRet == FALSE)
    {
      continue;
    }

    if (dwTotalBytesAvail > 0)
    {
      bRet = ReadFile(obj->m_hReadPipe,
                      szReadBuf,
                      dwTotalBytesAvail,
                      &dwReadedBytes,
                      NULL);

      // if(bRet == FALSE)
      // {
      //   continue;
      // }

      szReadBuf[ dwReadedBytes ] = '\0';

      int nLen = MultiByteToWideChar(CP_ACP, 0, szReadBuf, strlen(szReadBuf), NULL, 0);  

      MultiByteToWideChar(CP_ACP , 0 , szReadBuf , strlen(szReadBuf) , szReadBufW , nLen);

      // 发送消息
      obj->m_csSend.Lock();
      SendPacket(sClient ,
                 emMessType::cmdtext,
                 szReadBuf ,
                 strlen(szReadBuf) + 1
                );
      obj->m_csSend.Unlock();
    }
  }

  return 0;
}

void CLocalClientDlg::InitSocket()
{
  // 1. 创建一个套接字 socket
  m_sServer = socket(AF_INET , // 网络协议簇
                 SOCK_STREAM , // "流式" -- TCP, SOCK_DGRAM "数据报" -- UDP
                 IPPROTO_TCP);
  if(m_sServer == INVALID_SOCKET)
  {
    m_StaticText.SetWindowTextW(_T("创建 Socket 失败！"));
    return ;
  }

  // 2. 设置服务端 IP 和 端口 // 客户端不需要绑定
  // sockaddr addr; // 一个14字节大的结构体
  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDR); // IP地址
  m_addr.sin_port = htons(SERVER_PORT); // htons() 将主机字节序转为网络序，s代表sort

  // 3. 连接服务端 connect
  int nRet = connect(m_sServer , (sockaddr*)&m_addr , sizeof(sockaddr));
  if(nRet == SOCKET_ERROR)
  {
    m_StaticText.SetWindowTextW(_T("连接失败！"));
    return ;
  }

  m_StaticText.SetWindowTextW(_T("连接成功！"));

  // 4. 开启接收线程

  DWORD dwThreadId = 0;
  HANDLE hRecThread = CreateThread(NULL ,
                                0 ,
                                RecMessageThreadProc ,
                                (LPDWORD)this,
                                0 ,
                                &dwThreadId);


  // 5. 发送心跳包
  SendHeartBeat();

  // 6. 设置定时器发送心跳包
  SetTimer(emTimerType::tHeartbeat , HEARTEBEAT_TIME, NULL);
}


void CLocalClientDlg::SendHeartBeat()
{
  m_csSend.Lock();
  SendPacket(m_sServer , emMessType::heartbeat , NULL , 0);
  m_csSend.Unlock();
}

void CLocalClientDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO:  在此添加消息处理程序代码和/或调用默认值
  switch(nIDEvent)
  {
    case emTimerType::tHeartbeat:
      SendHeartBeat();
      break;
    default:
      break;
  }

  CDialogEx::OnTimer(nIDEvent);
}


void CLocalClientDlg::CmdStart()
{
  static BOOL isStart = FALSE;
  if(isStart)
  {
    return;
  }
  isStart = TRUE;
  // 变量声明
  STARTUPINFO si = { 0 };
  si.cb = sizeof(si);
  // 创建管道

  memset(&m_pi , 0 , sizeof(PROCESS_INFORMATION));

  SECURITY_ATTRIBUTES sa = { 0 };
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;

  // 创建两条管道
  BOOL bRet = CreatePipe(&m_hCmdReadPipe ,
                         &m_hWritePipe ,
                         &sa ,
                         0);

  if(bRet == FALSE)
  {
    return ;
  }

  bRet = CreatePipe(&m_hReadPipe ,
                    &m_hCmdWritePipe ,
                    &sa ,
                    0);

  if(bRet == FALSE)
  {
    return ;
  }

  // 重定义输入输出到管道
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.hStdInput = m_hCmdReadPipe;
  si.hStdOutput = m_hCmdWritePipe;
  si.hStdError = m_hCmdWritePipe;

  // 打开目标进程
  TCHAR szProcess[ MAXBYTE ] = { 0 };
  wcscpy_s(szProcess , MAXBYTE , TEXT("cmd.exe"));
  bRet = CreateProcess(NULL ,
                       szProcess ,
                       NULL ,
                       NULL ,
                       TRUE ,
                       0 ,
                       NULL ,
                       NULL ,
                       &si ,
                       &m_pi);

  // 创建发送线程 
  DWORD dwThreadId = 0;
  HANDLE hRecThread = CreateThread(NULL ,
                                0 ,
                                SendCmdMessageThreadProc ,
                                (LPDWORD)this,
                                0 ,
                                &dwThreadId);


  if(bRet == FALSE)
  {
    return ;
  }


}


void CLocalClientDlg::OnClose()
{
  // TODO:  在此添加消息处理程序代码和/或调用默认值
  if(m_pi.hProcess != INVALID_HANDLE_VALUE && m_pi.hProcess != NULL)
  {
    TerminateProcess(m_pi.hProcess , 0);
    CloseHandle(m_pi.hProcess);
  }

  CDialogEx::OnClose();
}


void CLocalClientDlg::CmdInput(char* szBuff , DWORD nBuffLen)
{
  if(szBuff != NULL && nBuffLen > 0)
  {
    DWORD dwWritedBytes = 0;
    WriteFile(m_hWritePipe,
              szBuff,
              strlen(szBuff),
              &dwWritedBytes,
              NULL);
  }
}


// 发送屏幕线程回调
DWORD WINAPI SendScreenThreadProc(LPVOID lpParameter)
{
  CLocalClientDlg* obj = (CLocalClientDlg*)lpParameter;

  //创建内存dc
  CDC memDc;
  //内存dc兼容桌面dc
  CDC* pDcDesktop = obj->GetDesktopWindow()->GetDC();
  memDc.CreateCompatibleDC(pDcDesktop);
  //拿到桌面宽高
  int nWidth = ::GetSystemMetrics(SM_CXSCREEN);
  int nHeigth = ::GetSystemMetrics(SM_CYSCREEN);

  CBitmap bmpSend;
  bmpSend.CreateCompatibleBitmap(pDcDesktop, nWidth, nHeigth);
  memDc.SelectObject(bmpSend);

  BOOL bRet =
      memDc.BitBlt(0, 0, nWidth, nHeigth, pDcDesktop, 0, 0, SRCCOPY);

  if (bRet == FALSE)
  {
      TRACE("bitBlt错误");
      return -1;
  }

  DWORD dwScreenSize = 4 * nWidth * nHeigth;
  char* szBmpBuf = new char[dwScreenSize + 8];
  if (szBmpBuf == NULL)
  {
      TRACE("szBmpBuf申请错误");
      return -1;
  }

  *(int*)szBmpBuf = nWidth;
  *(int*)(szBmpBuf + 4) = nHeigth;
  //得到位图
  DWORD dwRet = bmpSend.GetBitmapBits(dwScreenSize, szBmpBuf + 8);

  if (dwRet == 0)
  {
      TRACE("GetBitmapBits错误");
      return -1;
  }

  //使用zlib压缩，首先计算压缩后的大小
  uLongf nCompressedlen = compressBound(dwScreenSize+8); /* 压缩后的长度是不会超过nCompressedlen的 */
  //数据包内的数据大小是压缩后的大小加上屏幕数据
  
  uLongf nDataSize = nCompressedlen + sizeof(uLongf);
  char* pBuf = new char[nDataSize];
  //开始压缩数据 
  if (compress((Bytef*)(pBuf + sizeof(uLongf)),
      (uLongf*)&nCompressedlen,
      (Bytef*)szBmpBuf,
      (uLongf)nDataSize) != Z_OK)
  {
      //说明压缩失败
      TRACE("压缩失败");
      return -1;
  }
  //把压缩前的大小加到数据包的头部
  *(DWORD*)pBuf = dwScreenSize + 8;
  //uLongf nDataSize = nCompressedlen + sizeof(uLongf);

  // 开始发送数据

  obj->m_csSend.Lock();
  int nRet = SendPacket(obj->m_sServer, emMessType::screenbmp, pBuf, nCompressedlen+sizeof(DWORD));
  obj->m_csSend.Unlock();
  if (nRet == SOCKET_ERROR)
  {
      TRACE("发送数据失败");
      return -1;
  }
  //释放资源
  if (szBmpBuf != NULL)
  {
      delete[] szBmpBuf;
      szBmpBuf = NULL;
  }  
  if (pBuf != NULL)
  {
      delete[] pBuf;
      pBuf = NULL;
  }

  return 0;
}


void CLocalClientDlg::CaptureScreen()
{
  // 创建发送屏幕线程 
  DWORD dwThreadId = 0;
  HANDLE hRecThread = CreateThread(NULL ,
                                0 ,
                                SendScreenThreadProc ,
                                (LPDWORD)this,
                                0 ,
                                &dwThreadId);
  // SendScreenThreadProc((LPVOID)this);

}


// 发送进程线程回调
DWORD WINAPI SendProcessThreadProc(LPVOID lpParameter)
{
  CLocalClientDlg* obj = (CLocalClientDlg*)lpParameter;

  HANDLE         hProcessSnap = NULL;
  DWORD          bRet = FALSE;
  PROCESSENTRY32 pe32 = { 0 };
  TCHAR szExeName[MAX_PATH] = { 0 };
  DWORD nExeFileLen = 0;

  //  Take a snapshot of all processes in the system. 

  hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (hProcessSnap == INVALID_HANDLE_VALUE || hProcessSnap == NULL)
    return (FALSE);

  //  Fill in the size of the structure before using it. 

  pe32.dwSize = sizeof(PROCESSENTRY32);

  //  Walk the snapshot of the processes, and for each process, 
  //  display information. 

  if (Process32First(hProcessSnap, &pe32))
  {
    do
    {
        *(DWORD*)szExeName = pe32.th32ProcessID;
        nExeFileLen = wcslen(pe32.szExeFile);
        memcpy_s((char*)(szExeName + sizeof(short)) , MAX_PATH - sizeof(DWORD), pe32.szExeFile, (nExeFileLen + 1) * 2), 
        // wcscpy_s(szExeName + sizeof(short) , MAX_PATH , pe32.szExeFile);

        obj->m_csSend.Lock();
        SendPacket(obj->m_sServer , emMessType::processtext , (char*)szExeName ,
                   (wcslen(szExeName + sizeof(short)) + sizeof(short)) * sizeof(short) + sizeof(short));
        obj->m_csSend.Unlock();
        Sleep(1); // 奇怪的Bug，不睡不正常

    } while (Process32Next(hProcessSnap, &pe32));

    bRet = TRUE;
  }
  else
  {
    bRet = FALSE;    // could not walk the list of processes 
  }

  // Do not forget to clean up the snapshot object. 

  CloseHandle(hProcessSnap);
  return (bRet);
}


void CLocalClientDlg::GetProcess()
{
  // 创建发送屏幕线程 
  DWORD dwThreadId = 0;
  HANDLE hRecThread = CreateThread(NULL ,
                                0 ,
                                SendProcessThreadProc ,
                                (LPDWORD)this,
                                0 ,
                                &dwThreadId);
}


void CLocalClientDlg::KillProcess(DWORD PID)
{
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
  BOOL nRet = TerminateProcess(hProcess , 0);
  CloseHandle(hProcess);

  if(nRet)
  {
     m_csSend.Lock();
     SendPacket(m_sServer , emMessType::processkillsecc , NULL, 0 );
     m_csSend.Unlock();
  }
  else
  {
     m_csSend.Lock();
     SendPacket(m_sServer , emMessType::processkillfail , NULL, 0 );
     m_csSend.Unlock();
  }
}


//获取磁盘信息
void CLocalClientDlg::OnDiskMsg()
{
    TCHAR buf[MAXBYTE] = { 0 };
    GetLogicalDriveStrings(MAXBYTE, buf);
    TCHAR* pBuf = buf;
    // CBuffer myBuff;
    CString strSend = _T("");
    while (*pBuf)
    {
        if (GetDriveType(pBuf) == DRIVE_FIXED)
        {
            ULARGE_INTEGER freeToCaller = { 0 };
            ULARGE_INTEGER totalBytes = { 0 };
            ULARGE_INTEGER freeBytes = { 0 };
            GetDiskFreeSpaceEx(pBuf, &freeToCaller, &totalBytes, &freeBytes);
            SHFILEINFO sfi = { 0 };

            DWORD attr = GetFileAttributes(pBuf);

            (HIMAGELIST)SHGetFileInfo(pBuf,
                attr,
                &sfi,
                sizeof(sfi),
                SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME | SHGFI_DISPLAYNAME);
            TCHAR* szDiskName = pBuf;
            int nIcon = sfi.iIcon;
            TCHAR* szTypeName = sfi.szTypeName;
            ULONGLONG uFreeBytes = freeBytes.QuadPart;

            PackageDiskInfo(strSend, szDiskName, nIcon, szTypeName, uFreeBytes);
            // myBuff.Write(strSend);
        }
        pBuf += _tcslen(pBuf) + 1;
    }

    m_csSend.Lock();
    SendPacket(m_sServer, emMessType::filedisk, (char*)(strSend.GetBuffer()), (strSend.GetLength() + 1) * 2);
    m_csSend.Unlock();

}


void CLocalClientDlg::PackageDiskInfo(CString& strSend,
                                      TCHAR* szDiskName,
                                      int nIcon,
                                      TCHAR* szTypeName,
                                      ULONGLONG uReeBytes)
{
    strSend += szDiskName;
    strSend += _T("#");

    CString strIcon;
    strIcon.Format(_T("%d"), nIcon);
    strSend += strIcon;
    strSend += _T("#");

    strSend += szTypeName;
    strSend += _T("#");

    CString struReeBytes;
    struReeBytes.Format(_T("%d"), uReeBytes);
    strSend += struReeBytes;
    strSend += _T("#");
}


void CLocalClientDlg::OnFileMsg(char* szDirA)
{
  TCHAR* szDir = NULL;
  szDir = (TCHAR*)szDirA;
    //获取子目录
    WIN32_FIND_DATA findData;
    CString strPath;
    strPath.Format(_T("%s"), szDir);
    CString findStr = strPath + _T("*.*");
    HANDLE hFind = FindFirstFile(findStr.GetBuffer(0), &findData);
    CString strSend = _T("");
    
    if (hFind != INVALID_HANDLE_VALUE)
    {
        while (true)
        {
            CString filePath = strPath + findData.cFileName;
            SHFILEINFO sfi = { 0 };
            //      Tools::GetFileInfo(filePath.GetBuffer(0),sfi);
            //获取文件信息 
            DWORD attr = GetFileAttributes(filePath.GetBuffer(0));

            (HIMAGELIST)SHGetFileInfo(filePath.GetBuffer(0),
                attr,
                &sfi,
                sizeof(sfi),
                SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME | SHGFI_DISPLAYNAME);

            //文件名
            CString fileName = findData.cFileName;
            if (fileName == _T(".") || fileName == _T(".."))
            {
                if (!FindNextFile(hFind, &findData))
                {
                    break;
                }
                continue;;
            }

            //图标
            int nIcon = sfi.iIcon;

            //是不是文件夹
            int nIsDir = 0;
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                nIsDir = 1;
            }
            else
            {
                nIsDir = 0;
            }

            FILETIME ft;
            FileTimeToLocalFileTime(&findData.ftLastWriteTime, &ft);
            CTime time(ft);
            //修改时间
            CString strTime = time.Format("%Y/%m/%d %H:%M");
            //类型
            TCHAR* szType = sfi.szTypeName;
            //大小
            DWORD dwSize = findData.nFileSizeLow;

            PackageFileInfo(strSend, fileName, nIcon, nIsDir, strTime, szType, dwSize);

            if (!FindNextFile(hFind, &findData))
            {
                break;
            }
        }
        FindClose(hFind);
    }
    m_csSend.Lock();
    SendPacket(m_sServer, emMessType::filelist, (char*)(strSend.GetBuffer()), (strSend.GetLength() + 1) * 2);
    m_csSend.Unlock();
}


void CLocalClientDlg::PackageFileInfo
(CString& strSend, CString& fileName, int nIcon, int nIsDir, CString&strTime, TCHAR* szType, DWORD dwSize)
{
    strSend += fileName;
    strSend += _T("#");

    CString strIcon;
    strIcon.Format(_T("%d"), nIcon);
    strSend += strIcon;
    strSend += _T("#");

    CString strIsDir;
    strIsDir.Format(_T("%d"), nIsDir);
    strSend += strIsDir;
    strSend += _T("#");

    strSend += strTime;
    strSend += _T("#");

    strSend += szType;
    strSend += _T("#");

    CString strSize;
    strSize.Format(_T("%d"), dwSize);
    strSend += strSize;
    strSend += _T("#");
}
