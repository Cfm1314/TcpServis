// MFCServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCServer.h"
#include "MFCServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMFCServerDlg �Ի���



CMFCServerDlg::CMFCServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTSERVER, m_lstServerInfo);
	DDX_Control(pDX, IDC_LISTCLIENT, m_lstClient);
	DDX_Control(pDX, IDC_LISTRCVINFO, m_lstRcvInfo);
}

BEGIN_MESSAGE_MAP(CMFCServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTNOPEN, &CMFCServerDlg::OnBnClickedBtnopen)
END_MESSAGE_MAP()


// CMFCServerDlg ��Ϣ�������

BOOL CMFCServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	m_ServerState.RegOnClientConnect(OnServerInit, OnClientConnect, OnClientRcv, this);
	char erroInfo[256] = {0};
	if(false == m_ServerState.Init(erroInfo))
	{
		CString errNotice(erroInfo);
		AfxMessageBox(errNotice);
		return FALSE;
	}

	SetDlgItemText(IDC_EDT_SERVERIP, CString(m_ServerState.ServerIP()));
	SetDlgItemInt(IDC_EDT_PORT, m_ServerState.ServerPort());

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCServerDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void	CMFCServerDlg::OnClientConnect(char* info, void* pContext)
{
	CMFCServerDlg* pDlg = (CMFCServerDlg*)pContext;
	pDlg->m_lstClient.AddString(CString(info));
}

void	CMFCServerDlg::OnClientRcv(char* from, char* info, void* pContext)
{
	CMFCServerDlg* pDlg = (CMFCServerDlg*)pContext;
	char rcvInfo[256] = {0};
	sprintf(rcvInfo, "[%s]:%s", from, info);
	pDlg->m_lstRcvInfo.AddString(CString(rcvInfo));	
}

void	CMFCServerDlg::OnServerInit(char* info, void* pContext)
{
	CMFCServerDlg* pDlg = (CMFCServerDlg*)pContext;
	pDlg->m_lstServerInfo.AddString(CString(info));

}



void CMFCServerDlg::OnBnClickedBtnopen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	unsigned int iThreadID;
	uintptr_t rt = 0;
	rt = _beginthreadex(NULL, 0, threadClientSocket,  this, 0, &iThreadID);
}

void  CMFCServerDlg::RunServer()
{
	wchar_t		wServerIP[32] = {0};
	char		serverIP[32] = {0};
	short		serverPort;

	GetDlgItemText(IDC_EDT_SERVERIP, wServerIP, STR_LEN(wServerIP));
	serverPort = GetDlgItemInt(IDC_EDT_PORT);
	WideCharToMultiByte(CP_OEMCP, 0, wServerIP, -1, serverIP, STR_LEN(serverIP), NULL, 0);

	char errorInfo[256] = {0};
	if(m_ServerState.Run(serverIP, serverPort, errorInfo) == false)
	{
		AfxMessageBox(CString(errorInfo));
	}
}

unsigned int __stdcall CMFCServerDlg::threadClientSocket(void* pPara)
{
	CMFCServerDlg* pDlg = (CMFCServerDlg*)pPara;
	pDlg->RunServer();
	return 0;
}