// FileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "remote-control.h"
#include "FileDlg.h"
#include "afxdialogex.h"


// CFileDlg 对话框

IMPLEMENT_DYNAMIC(CFileDlg, CDialogEx)

CFileDlg::CFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileDlg::IDD, pParent)
  , m_pInfo(NULL)
{

}

CFileDlg::~CFileDlg()
{
}

void CFileDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX , IDC_TREE_FILE , m_TreeFile);
  DDX_Control(pDX , IDC_LIST_FILE , m_ListFile);
}


BEGIN_MESSAGE_MAP(CFileDlg, CDialogEx)
  ON_WM_SHOWWINDOW()
  ON_NOTIFY(NM_DBLCLK , IDC_LIST_FILE , &CFileDlg::OnNMDblclkListFile)
END_MESSAGE_MAP()


// CFileDlg 消息处理程序


BOOL CFileDlg::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  在此添加额外的初始化
  m_ListFile.ModifyStyle(0, LVS_REPORT);
  DWORD dwStye = m_ListFile.GetExtendedStyle();
  dwStye = dwStye | LVS_EX_FULLROWSELECT;
  m_ListFile.SetExtendedStyle(dwStye);
  // m_ListFile.InsertColumn(0 , _T("文件名"), 0, 300);
  m_ListFile.InsertColumn(0, _T("名称"));
  m_ListFile.InsertColumn(1, _T("修改日期"));
  m_ListFile.InsertColumn(2, _T("类型"));
  m_ListFile.InsertColumn(3, _T("大小"));


  SetTreeItem();

  return TRUE;  // return TRUE unless you set the focus to a control
  // 异常:  OCX 属性页应返回 FALSE
}


void CFileDlg::SetTreeItem()
{
  SHFILEINFO sfi;
  HIMAGELIST imglist = (HIMAGELIST)SHGetFileInfo(_T(""), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
  m_TreeFile.SetImageList(CImageList::FromHandle(imglist), TVSIL_NORMAL);
  m_tvItem.mask = TVIF_CHILDREN | TVIF_HANDLE;
  m_tvItem.cChildren = 1;
  m_RootItem = m_TreeFile.InsertItem(_T("计算机"), COMPUTER_IMG, COMPUTER_IMG);
}


void CFileDlg::OnShowWindow(BOOL bShow , UINT nStatus)
{
  CDialogEx::OnShowWindow(bShow , nStatus);

  // TODO:  在此处添加消息处理程序代码
  m_ListFile.DeleteAllItems();
  m_TreeFile.DeleteAllItems();
  SetTreeItem();
  m_pInfo->csSend.Lock();
  SendPacket(m_sClient , emMessType::filediskrequest, NULL , 0);
  m_pInfo->csSend.Unlock();
}


void CFileDlg::GetFileDisk(char* szBuff)
{
    CString strDisk;
    strDisk = (TCHAR*)szBuff;
    // strDisk.Format("%s", szRealData);
    CString strSplite = _T("#");
    vector<CString> vcInfo;
    SplitCString(vcInfo, strDisk, strSplite);
    CStringA strTemp;

    for (u_int i = 0; i < vcInfo.size() ; i++)
    {
        //盘符
        int nIndex = i / 4;
        if (i % 4 == 0)
        {
            HTREEITEM item = m_TreeFile.InsertItem(vcInfo[i], FOLOR_IMG, FOLOR_IMG, m_RootItem);
            m_tvItem.hItem = item;
            m_TreeFile.SetItem(&m_tvItem);

            
            strTemp = vcInfo[ i + 1 ];
            int nIcon = atoi(strTemp.GetBuffer());
            m_ListFile.InsertItem(nIndex, vcInfo[i], nIcon);
            m_ListFile.SetItemData(nIndex, 1);
            m_ListFile.SetItemText(nIndex, 1, vcInfo[i+2]);
            m_ListFile.SetItemText(nIndex, 2, vcInfo[i + 3]);
        }
    }
    ResizeListColumns();
    m_TreeFile.Expand(m_RootItem, TVE_EXPAND);
}


void CFileDlg::SplitCString(vector <CString>& vecString, CString strSource, CString ch)
{
    //vector <CString> vecString;
    int iPos = 0;
    CString strTmp;
    strTmp = strSource.Tokenize(ch, iPos);
    while (strTmp.Trim() != _T(""))
    {
        vecString.push_back(strTmp);
        strTmp = strSource.Tokenize(ch, iPos);
    }
}


void CFileDlg::ResizeListColumns()
{
    for (int i = 0; i < m_ListFile.GetHeaderCtrl()->GetItemCount(); i++)
    {
        m_ListFile.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
    }
}



void CFileDlg::OnNMDblclkListFile(NMHDR *pNMHDR , LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  // TODO:  在此添加控件通知处理程序代码
  int itemIndex = m_ListFile.GetSelectionMark();
  if (itemIndex == -1)
  {
      return;
  }
  if (m_ListFile.GetItemData(itemIndex) == 0)
  {
      return;
  }
  CString nameStr = m_ListFile.GetItemText(itemIndex, 0);

  CString filePath = m_currentPath == _T("") ? _T("") : (m_currentPath + _T("\\"));

  filePath += nameStr + _T("\\");

  filePath.Replace(_T("\\\\"), _T("\\"));

  OpenDir(filePath);

  *pResult = 0;
}


void CFileDlg::OpenDir(CString& dirPath)
{
    if (m_ListFile.GetItemCount() > 0)
    {
        m_ListFile.DeleteAllItems();
    }
    if (dirPath == "")
    {
        OpenRoot();
    }
    else
    {
        OpenDirectory(dirPath);
    }   
    ResizeListColumns();
    m_currentPath = dirPath;
}


void CFileDlg::OpenRoot()
{
    if (m_sClient == INVALID_SOCKET)
    {
        TRACE("文件客户端socket 无效");
        return ;
    }
    m_pInfo->csSend.Lock();
    SendPacket(m_sClient , emMessType::filerootrequest, NULL , 0);
    m_pInfo->csSend.Unlock();
}


void CFileDlg::OpenDirectory(CString& path)
{   
    m_pInfo->csSend.Lock();
    SendPacket(m_sClient , emMessType::filepathrequest, (char*)(path.GetBuffer()), (path.GetLength()+1) * sizeof(short));
    m_pInfo->csSend.Unlock();
}


void CFileDlg::GetFileList(char* szBuff)
{
    CString strFile;
    strFile = (TCHAR*)szBuff;
    CString strSplite = _T("#");
    vector<CString> vcInfo;
    SplitCString(vcInfo, strFile, strSplite);
    CStringA strTemp;

    for (u_int i = 0; i < vcInfo.size(); i++)
    {
        //文件名
        int nIndex = i / 6;
        if (i % 6 == 0)
        {
          strTemp = vcInfo[ i + 1 ];
            int nIcon = atoi(strTemp.GetBuffer());
            m_ListFile.InsertItem(nIndex, vcInfo[i], nIcon);
          strTemp = vcInfo[ i + 2 ];
            int nIsDir = atoi(strTemp.GetBuffer());
            m_ListFile.SetItemData(nIndex, nIsDir);
            m_ListFile.SetItemText(nIndex, 1, vcInfo[i + 3]);
            m_ListFile.SetItemText(nIndex, 2, vcInfo[i + 4]);
            m_ListFile.SetItemText(nIndex, 3, vcInfo[i + 5]);
        }
    }
    ResizeListColumns();
}
