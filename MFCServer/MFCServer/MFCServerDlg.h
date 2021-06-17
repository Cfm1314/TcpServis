// MFCServerDlg.h : ͷ�ļ�
//

#pragma once
#include "TCPServerState.h"
#include "afxwin.h"

// CMFCServerDlg �Ի���
class CMFCServerDlg : public CDialog
{
// ����
public:
	CMFCServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CTCPServerState	m_ServerState;
	
	static  void	OnClientConnect(char* info, void* pContext);
	static  void	OnClientRcv(char* from, char* info, void* pContext);
	static	void	OnServerInit(char* info, void* pContext);

public:
	afx_msg void	OnBnClickedBtnopen();
private:
	void  RunServer();

	static unsigned int __stdcall threadClientSocket(void* pPara);
public:
	CListBox m_lstServerInfo;
	CListBox m_lstClient;
	CListBox m_lstRcvInfo;
};

