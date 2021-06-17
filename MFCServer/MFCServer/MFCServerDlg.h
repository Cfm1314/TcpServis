// MFCServerDlg.h : 头文件
//

#pragma once
#include "TCPServerState.h"
#include "afxwin.h"

// CMFCServerDlg 对话框
class CMFCServerDlg : public CDialog
{
// 构造
public:
	CMFCServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFCSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

