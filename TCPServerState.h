#pragma once
#include "Ws2tcpip.h"
#include <process.h>

#define  STR_LEN(x) (sizeof(x) / sizeof(x[0]))

//�ͻ��������Ϣ
typedef struct tagClientContext
{
	sockaddr_in  fromAddr;
	char		rcvBuffer[256];
	SOCKET		client;
	void*		pParent;
	bool		bExit;
}ClientContext,*PClientContext;

//����ͻ��˻ص�����
typedef void (*POnServerInitDef)(char* info, void* pContext);
typedef void (*POnClientConnectDef)(char* info, void* pContext);
typedef void (*POnClientRcvDef)(char* from, char* info, void* pContext);

//������
class CTCPServerState
{
public:
	bool  Init(char* errorInfo);
	bool  Run(char* serverIP, short port, char* errorInfo);
	bool  Destroy();
	char* ServerIP()  {return m_ServerIP;}
	short ServerPort() {return  m_Port;}
	void  RegOnClientConnect(POnServerInitDef pOnInit, POnClientConnectDef pOnConect, POnClientRcvDef pOnRcv, void* pContext);
public:
	CTCPServerState(void);
	~CTCPServerState(void);
private:
	SOCKET		m_TCP;
	WSADATA		m_wsaData;
	bool		m_InitOK;
	bool		m_HasRun;
	short       m_Port;
	char		m_ServerIP[32];

	POnServerInitDef	m_OnServerInit;
	POnClientConnectDef	m_OnClientConnect;
	POnClientRcvDef		m_OnClientRcv;
	void*				m_pContext;

	static unsigned int __stdcall threadClientSocket(void* pPara);
};
