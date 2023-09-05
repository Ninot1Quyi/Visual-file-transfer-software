// CFILEWIND.cpp: 实现文件
//

#include "pch.h"
#include "可视化文件传输3.h"
#include "afxdialogex.h"
#include "CFILEWIND.h"


// CFILEWIND 对话框

IMPLEMENT_DYNAMIC(CFILEWIND, CDialogEx)

CFILEWIND::CFILEWIND(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILEWIND, pParent)
{

}

CFILEWIND::~CFILEWIND()
{
}

void CFILEWIND::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCSHELLTREE1, my_shellTree);
	DDX_Control(pDX, IDC_MFCSHELLLIST1, my_shellList);
	DDX_Text(pDX, IDC_EDITFILEPATHSHOW, strFilePath);
}


BEGIN_MESSAGE_MAP(CFILEWIND, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_MFCSHELLTREE1, &CFILEWIND::OnTvnSelchangedMfcshelltree1)
	ON_NOTIFY(NM_CLICK, IDC_MFCSHELLLIST1, &CFILEWIND::OnNMClickMfcshelllist1)
	 
	ON_BN_CLICKED(IDOK, &CFILEWIND::OnBnClickedOk)
END_MESSAGE_MAP()


// CFILEWIND 消息处理程序


void CFILEWIND::OnTvnSelchangedMfcshelltree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CString str_filePath;
	my_shellTree.GetItemPath(str_filePath, pNMTreeView->itemNew.hItem);
	my_shellList.DisplayFolder(str_filePath);
	*pResult = 0;
}


//点击了shell list中的文件
void CFILEWIND::OnNMClickMfcshelllist1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	
	int nSelectedIndex = my_shellList.GetNextItem(-1, LVNI_SELECTED);
	CString strSelectedFilePath;
	if (nSelectedIndex != -1)
	{
		// 获取选中项的数据（文件路径存储在数据中）
		//strSelectedFilePath = my_shellList.GetItemText(nSelectedIndex, 0);
		my_shellList.GetItemPath(strSelectedFilePath, nSelectedIndex);
		strFilePath = strSelectedFilePath;
		UpdateData(FALSE);
		//MessageBox(strSelectedFilePath);
	}

	*pResult = 0;
}


 

void CFILEWIND::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
