#pragma once
#include "Resource.h"
#include "common.h"
#include "afxcmn.h"
#include <vector>
using namespace std;

#define COMPUTER_IMG  (2)
#define FOLOR_IMG  (1)

// CFileDlg 对话框

class CFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileDlg)

public:
	CFileDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
  SOCKET m_sClient;
  pSocketInfo m_pInfo;
  TVITEM m_tvItem;
  HTREEITEM m_RootItem;
  CString m_currentPath;

  virtual BOOL OnInitDialog();
  CTreeCtrl m_TreeFile;
  CListCtrl m_ListFile;
  afx_msg void OnShowWindow(BOOL bShow , UINT nStatus);
  void GetFileDisk(char* szBuff);
  void SplitCString(vector <CString>& vecString , CString strSource , CString ch);
  void ResizeListColumns();
  void SetTreeItem();
  afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR , LRESULT *pResult);
  void OpenDir(CString& dirPath);
  void OpenRoot();
  void OpenDirectory(CString& path);
  void GetFileList(char* szBuff);
};
