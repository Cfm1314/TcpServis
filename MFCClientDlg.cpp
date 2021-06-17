// MFCClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCClient.h"
#include "MFCClientDlg.h"

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


// CMFCClientDlg �Ի���




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


// CMFCClientDlg ��Ϣ�������

BOOL CMFCClientDlg::OnInitDialog()
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
	m_State.RegCallBackFunc(OnClientInit, OnConnect, OnRcvData, this);
	m_State.Init();
	SetDlgItemText(IDC_EDTSERVER, CString(m_State.RemoteServer()));
	SetDlgItemInt(IDC_EDTPORT, m_State.RemotePort());

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCClientDlg::OnPaint()
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
HCURSOR CMFCClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCClientDlg::OnBnClickedBtnconnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	//���п��ַ�ת��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	wchar_t wSendInfo[256] = {0};
	GetDlgItemText(IDC_EDTSEND, wSendInfo, STR_LEN(wSendInfo));
	char sendInfo[256] = {0};
	WideCharToMultiByte(CP_OEMCP, 0, wSendInfo, -1, sendInfo, STR_LEN(sendInfo), NULL, 0);
	m_State.SendData(sendInfo, strlen(sendInfo));

}
