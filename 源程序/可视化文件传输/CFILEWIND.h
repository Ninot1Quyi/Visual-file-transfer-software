#pragma once
#include "afxdialogex.h"


// CFILEWIND 对话框

class CFILEWIND : public CDialogEx
{
	DECLARE_DYNAMIC(CFILEWIND)

public:
	CFILEWIND(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFILEWIND();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEWIND };
#endif
public:
	CString strFilePath;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMFCShellTreeCtrl my_shellTree;
	CMFCShellListCtrl my_shellList;
	afx_msg void OnTvnSelchangedMfcshelltree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClickMfcshelllist1(NMHDR* pNMHDR, LRESULT* pResult);
	 
	afx_msg void OnBnClickedOk();
};
