// MFCClientDlg.h : 头文件
//

#pragma once

#include "TCPClientState.h"
#include "afxwin.h"
// CMFCClientDlg 对话框
class CMFCClientDlg : public CDialog
{
// 构造
public:
	CMFCClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFCCLIENT_DIALOG };

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
public:
	afx_msg void OnBnClickedBtnconnect();

private:
	TCPClientState	m_State;

	static  void OnClientInit(char* info, void* pContext);
	static  void OnConnect(bool connectOK, char* info, void* pContext);
	static  void OnRcvData(char* from, char* info, void* pContext);
public:
	CListBox m_lstInit;
	CListBox m_lstRcvInfo;
	afx_msg void OnBnClickedBtnsend();
};
