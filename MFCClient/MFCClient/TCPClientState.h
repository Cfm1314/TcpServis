#pragma once
#include "Ws2tcpip.h"
#include <process.h>


#define  STR_LEN(x) (sizeof(x) / sizeof(x[0]))

//定义客户端回调函数
typedef void (*POnClientInitDef)(char* info, void* pContext);
typedef void (*POnConnectDef)(bool connectOK, char* info, void* pContext);
typedef void (*POnRcvDef)(char* from, char* info, void* pContext);


class TCPClientState
{
public:
	bool	Init();
	bool	Destroy();
	bool	Connect(char* remote, short port);
	bool	SendData(char* info, int size);
	void	RegCallBackFunc(POnClientInitDef pInit, POnConnectDef pConnect, POnRcvDef pOnRcv, void* pContext);
	char*	RemoteServer() {return m_RemoteServer;}
	short	RemotePort() {return m_RemotePort;}
public:
	TCPClientState(void);
	~TCPClientState(void);
private:
	SOCKET				m_TCP;
	WSADATA				m_wsaData;
	bool				m_InitOK;
	bool				m_ConnectOK;
	char				m_RemoteServer[32];
	short				m_RemotePort;

	POnClientInitDef	m_pOnClientInit;
	POnConnectDef		m_pOnConnect;
	POnRcvDef			m_pOnRcv;
	void*				m_pContext;

	void				RcvData();


	static unsigned int __stdcall threadRcvData(void* pPara);
};
