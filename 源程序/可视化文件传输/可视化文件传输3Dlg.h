
// 可视化文件传输3Dlg.h: 头文件
//

#include "CFILEWIND.h"
#include "FileTransfer.h"

#pragma once


// C可视化文件传输3Dlg 对话框
class C可视化文件传输3Dlg : public CDialogEx
{
// 构造
public:
	C可视化文件传输3Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY3_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	CString str_IP;
	CString str_Port;
	CString str_filePath;
	CString str_connectState;
	CFILEWIND cFileWind;//文件预览窗口
	FileTransfer fileTransfer;//文件传输器
	std::thread threadObjListen; // 成员变量，用于存储多线程对象监听连接
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonconnect();
	afx_msg void OnBnClickedButtonbrowse();
	afx_msg void OnBnClickedButtonsend();
	afx_msg void OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult);
};
