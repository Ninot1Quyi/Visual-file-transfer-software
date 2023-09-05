
// 可视化文件传输3Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "可视化文件传输3.h"
#include "可视化文件传输3Dlg.h"
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
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	 
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	 
END_MESSAGE_MAP()


// C可视化文件传输3Dlg 对话框



C可视化文件传输3Dlg::C可视化文件传输3Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MY3_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void C可视化文件传输3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITIP, str_IP);
	DDX_Text(pDX, IDC_EDITFILEPATH, str_filePath);
	DDX_Text(pDX, IDC_EDITPORT, str_Port);
	//str_connectState
	DDX_Text(pDX, IDC_STATICConnect, str_connectState);
}

BEGIN_MESSAGE_MAP(C可视化文件传输3Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONCONNECT, &C可视化文件传输3Dlg::OnBnClickedButtonconnect)
	ON_BN_CLICKED(IDC_BUTTONBROWSE, &C可视化文件传输3Dlg::OnBnClickedButtonbrowse)
	ON_BN_CLICKED(IDC_BUTTONSEND, &C可视化文件传输3Dlg::OnBnClickedButtonsend)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &C可视化文件传输3Dlg::OnNMCustomdrawProgress1)
END_MESSAGE_MAP()


// C可视化文件传输3Dlg 消息处理程序

BOOL C可视化文件传输3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 在此添加额外的初始化代码
	 

	//str_IP = CString("127.0.0.1");
	str_IP = CString("2408:8445:b570:570f:821a:a613:78c0:4c41");
	str_Port = CString("6666");
	UpdateData(FALSE);

	fileTransfer.initialize();//初始化文件传输器
	fileTransfer.pStaticText = (CStatic*)GetDlgItem(IDC_EDITONLYREAD);
	fileTransfer.pStaticConnectText = (CStatic*)GetDlgItem(IDC_STATICConnect);
	// 获取进度控件的句柄
	CWnd* progressControlWnd;
	progressControlWnd = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	
	// 设置进度控件的范围和初始值
	progressControlWnd->SendMessage(PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // 设置范围为0到100
	progressControlWnd->SendMessage(PBM_SETPOS, 0, 0); // 设置初始值为0
	fileTransfer.pProgressCtrl = reinterpret_cast<CProgressCtrl*>(progressControlWnd);
	/*fileTransfer.pProgressCtrl->OffsetPos(50);
	fileTransfer.pProgressCtrl->OffsetPos(25);
	fileTransfer.pProgressCtrl->OffsetPos(25);*/
	writeToLogFile("\n\n=====================================================");
	writeToLogFile("启动服务器");//写入日志文件
	threadObjListen = std::thread(&FileTransfer::Listen, &fileTransfer);
	// 等待线程结束
	//threadObjListen.join();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void C可视化文件传输3Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void C可视化文件传输3Dlg::OnPaint()
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
HCURSOR C可视化文件传输3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//开始连接
void C可视化文件传输3Dlg::OnBnClickedButtonconnect()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	
	//MessageBox(str_IP);
	//MessageBox(str_Port);
	// 创建线程，并调用类的方法
	//字符串类型转换unicode转ASCII
	// IP 地址转换
	LPCWSTR lpwstr_IP = str_IP.GetString();

	int bufferSizeIP = WideCharToMultiByte(CP_UTF8, 0, lpwstr_IP, -1, NULL, 0, NULL, NULL);
	char* ipAddress = new char[bufferSizeIP];
	WideCharToMultiByte(CP_UTF8, 0, lpwstr_IP, -1, ipAddress, bufferSizeIP, NULL, NULL);

	// Port 转换
	CStringA strA_Port(str_Port);  // CStringA 是 ANSI 字符串类型
	const char* port = strA_Port;

	// 使用 ipAddress 和 port 进行操作
	// 创建线程，并调用类的方法
	for (int i = 0; i < bufferSizeIP; i++) {
		fileTransfer.ipAddress[i] = ipAddress[i];
	}

	 
	writeToLogFile(" 输入框中的ipAddress：" + std::string(ipAddress));
	fileTransfer.port = port;
	int iResult = fileTransfer.connectToServer(ipAddress, port);
	if (iResult == SOCKET_ERROR)
	{
		str_connectState = "未连接";
		writeToLogFile("连接失败");
	}
	else {
		fileTransfer.iconnect = 1;
		str_connectState = "已连接";
		//writeToLogFile("\n\n===========================================================");
		writeToLogFile("连接成功");
	}
	 
	CStatic* pStaticText = (CStatic*)GetDlgItem(IDC_STATICConnect);
	if (pStaticText != nullptr)
	{
		// 设置新的文本内容
		pStaticText->SetWindowText(str_connectState);
	}

	// 释放资源

	delete[] ipAddress;
	str_IP.ReleaseBuffer();
	
}


void C可视化文件传输3Dlg::OnBnClickedButtonbrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	cFileWind.DoModal();
	str_filePath = cFileWind.strFilePath;
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDITFILEPATH);
	if (pEdit != nullptr)
	{
		pEdit->SetWindowText(str_filePath);
	}
}



void C可视化文件传输3Dlg::OnBnClickedButtonsend()
{
	// TODO: 在此添加控件通知处理程序代码
 
	LPCWSTR lpwstr_filePath = str_filePath.GetString();

	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, lpwstr_filePath, -1, NULL, 0, NULL, NULL);
	char* filePath = new char[bufferSize];
	WideCharToMultiByte(CP_UTF8, 0, lpwstr_filePath, -1, filePath, bufferSize, NULL, NULL);

	CStatic* pStaticText = (CStatic*)GetDlgItem(IDC_STATICDETAIL);
	if (pStaticText != nullptr)
	{
		// 设置新的文本内容
		pStaticText->SetWindowText(CString("正在发送文件：")+CString(filePath));
	}
	writeToLogFile("正在发送文件：" + std::string(filePath));
	//std::thread threadObjSendData(&FileTransfer::sendData, &fileTransfer, filePath);
	//测试
	std::thread threadObjSendData(&FileTransfer::muliThreadTrans, &fileTransfer, filePath);
	threadObjSendData.join();
	writeToLogFile("文件发送完毕");
	// 设置新的文本内容
	pStaticText->SetWindowText(CString("已发送：") + CString(filePath));
	//MessageBox(str_filePath);
}


void C可视化文件传输3Dlg::OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
