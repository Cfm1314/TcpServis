// MFCClientDlg.h : ͷ�ļ�
//

#pragma once

#include "TCPClientState.h"
#include "afxwin.h"
// CMFCClientDlg �Ի���
class CMFCClientDlg : public CDialog
{
// ����
public:
	CMFCClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCCLIENT_DIALOG };

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
