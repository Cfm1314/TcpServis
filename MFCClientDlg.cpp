// MFCClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCClient.h"
#include "MFCClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMFCClientDlg 对话框




CMFCClientDlg::CMFCClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LSTINIT, m_lstInit);
	DDX_Control(pDX, IDC_LSTRCVINFO, m_lstRcvInfo);
}

BEGIN_MESSAGE_MAP(CMFCClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTNCONNECT, &CMFCClientDlg::OnBnClickedBtnconnect)
	ON_BN_CLICKED(IDC_BTNSEND, &CMFCClientDlg::OnBnClickedBtnsend)
END_MESSAGE_MAP()


// CMFCClientDlg 消息处理程序

BOOL CMFCClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_State.RegCallBackFunc(OnClientInit, OnConnect, OnRcvData, this);
	m_State.Init();
	SetDlgItemText(IDC_EDTSERVER, CString(m_State.RemoteServer()));
	SetDlgItemInt(IDC_EDTPORT, m_State.RemotePort());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCClientDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCClientDlg::OnBnClickedBtnconnect()
{
	// TODO: 在此添加控件通知处理程序代码
	wchar_t wRemoteServer[32] = {0};
	GetDlgItemText(IDC_EDTSERVER, wRemoteServer, STR_LEN(wRemoteServer));
	short  remotePort = GetDlgItemInt(IDC_EDTPORT);
	char remoteServer[32] = {0};
	WideCharToMultiByte(CP_OEMCP, 0, wRemoteServer, -1, remoteServer, STR_LEN(remoteServer), NULL, 0);
	m_State.Connect(remoteServer, remotePort);
}

void CMFCClientDlg::OnClientInit(char* info, void* pContext)
{
	CMFCClientDlg* pDlg = (CMFCClientDlg*)pContext;
	pDlg->m_lstInit.AddString(CString(info));
}

void CMFCClientDlg::OnConnect(bool connectOK, char* info, void* pContext)
{
	CMFCClientDlg* pDlg = (CMFCClientDlg*)pContext;
	pDlg->SetDlgItemText(IDC_EDTCONNECT, CString(info));

}

void CMFCClientDlg::OnRcvData(char* from, char* info, void* pContext)
{
	CMFCClientDlg* pDlg = (CMFCClientDlg*)pContext;
	//进行宽字符转换
	wchar_t wFrom[128] = {0};
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, from, strlen(from),
		                wFrom,STR_LEN(wFrom));
	wchar_t wInfo[256] = {0};
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, info, strlen(info),
		                wInfo,STR_LEN(wInfo));

	CString  rcvInfo;
	rcvInfo.Format(L"[%s]:%s", wFrom, wInfo);
	pDlg->m_lstRcvInfo.AddString(rcvInfo);



}

void CMFCClientDlg::OnBnClickedBtnsend()
{
	// TODO: 在此添加控件通知处理程序代码

	wchar_t wSendInfo[256] = {0};
	GetDlgItemText(IDC_EDTSEND, wSendInfo, STR_LEN(wSendInfo));
	char sendInfo[256] = {0};
	WideCharToMultiByte(CP_OEMCP, 0, wSendInfo, -1, sendInfo, STR_LEN(sendInfo), NULL, 0);
	m_State.SendData(sendInfo, strlen(sendInfo));

}
