#include "TCPServerState.h"
#include <windows.h>

#include <stdio.h>

#pragma comment(lib,  "Iphlpapi.lib")
#pragma comment(lib,  "Ws2_32.lib")


//-----------------------------------------------
//���캯��
CTCPServerState::CTCPServerState(void)
{
	m_Port = 1000;
	m_InitOK = false;

	m_OnClientConnect = NULL;
	m_OnClientRcv = NULL;
	m_pContext = NULL;
	m_OnServerInit = NULL;

	sprintf(m_ServerIP, "%s", "127.0.0.1");
	m_Port = 2000;
}
//-----------------------------------------------
//��������
CTCPServerState::~CTCPServerState(void)
{
}

//-----------------------------------------------
//ע��ͻ�����ػص�����
void  CTCPServerState::RegOnClientConnect(POnServerInitDef pOnInit, POnClientConnectDef pOnConect, POnClientRcvDef pOnRcv, void* pContext)
{
	m_OnServerInit = pOnInit;
	m_OnClientConnect = pOnConect;
	m_OnClientRcv = pOnRcv;
	m_pContext = pContext;


}

//-----------------------------------------------
//��ʼ��SOCKET����
bool CTCPServerState::Init(char* errorInfo)
{
	if(0 != ::WSAStartup(MAKEWORD(2,2), &m_wsaData))
	{
		sprintf(errorInfo, "%s", "��ʼ�������ʧ��!");
		return false;
	}	
	m_TCP = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if(INVALID_SOCKET == m_TCP)
	{
		sprintf(errorInfo, "%s", "����SOCKETʧ��!");
		return false;
	}
	m_InitOK = true;
	m_HasRun = false;
	
	return true;
}
//-----------------------------------------------
//ɾ�������
bool  CTCPServerState::Destroy()
{
	m_InitOK = false;
	::closesocket(m_TCP);	
	if(SOCKET_ERROR == ::WSACleanup())
	{
		return false;
	}
	return true;

}
//-----------------------------------------------
//���з���˹���
bool  CTCPServerState::Run(char* serverIP, short port, char* errorInfo)
{
	//�Ѿ����������У�ֱ�ӷ���
	if(m_HasRun) return false;
	m_Port = port;
	sprintf(m_ServerIP, "%s", serverIP);

	//�����Ǳ�����ַ127.0.0.1����ͨ��
	sockaddr_in LocalAddr;
	LocalAddr.sin_family = AF_INET;	
	LocalAddr.sin_addr.S_un.S_addr =   ::inet_addr(m_ServerIP);;
	LocalAddr.sin_port = ::htons(m_Port);

	//����SOCKET��
	if(::bind(m_TCP, (LPSOCKADDR)&LocalAddr, sizeof(LocalAddr)) == SOCKET_ERROR)
	{
		sprintf(errorInfo, "�󶨵�%s:%dʧ��!", m_ServerIP, m_Port);
		::closesocket(m_TCP);
		if(m_OnServerInit) m_OnServerInit(errorInfo, m_pContext);
		return false;
	}
	char info[256] = {0};
	sprintf(info, "�ɹ��󶨵�%s:%d!", m_ServerIP, m_Port);
	if(m_OnServerInit) m_OnServerInit(info, m_pContext);

	//��ʼ���м�����������
	if( SOCKET_ERROR  == ::listen(m_TCP, 8))
	{
		sprintf(errorInfo, "%s", "����SOCKETʧ��!");
		if(m_OnServerInit) m_OnServerInit(errorInfo, m_pContext);
		return false;
	}
	sprintf(info, "%s",  "�ɹ�����SOCKET!");
	if(m_OnServerInit) m_OnServerInit(info, m_pContext);

	//�������ڽ������ݵĻ�����
	char buffer[1024] = {0};
	int bufferLen = sizeof(buffer);
	//�����ַ�������ڼ�¼�������ݷ��ĵ�ַ��Ϣ��
	sockaddr_in  fromAddr;
	int addrLen = sizeof(fromAddr);
	m_HasRun = true;
	while(m_InitOK)
	{
		
		//�ȴ��ͻ��˵�����
		sprintf(info, "%s",  "�ȴ��ͻ���������!");
		if(m_OnServerInit) m_OnServerInit(info, m_pContext);

		SOCKET AcceptSocket = accept(m_TCP, (sockaddr*)&fromAddr, &addrLen);
		if(INVALID_SOCKET  != AcceptSocket)
		{
			char* fromIP = inet_ntoa(fromAddr.sin_addr);
			char clientInfo[128] = {0};
			sprintf(clientInfo, "���յ�%s�Ŀͻ�������", fromIP);			
			if(m_OnClientConnect) m_OnClientConnect(clientInfo, m_pContext);
			PClientContext pContext  = new ClientContext();
			pContext->client = AcceptSocket;
			memcpy(&pContext->fromAddr, &fromAddr, sizeof(fromAddr));
			pContext->pParent = this;
			pContext->bExit = false;

			unsigned int iThreadID;
			uintptr_t rt = 0;
			rt = _beginthreadex(NULL, 0, threadClientSocket,  pContext, 0, &iThreadID);
		}

		
	}
	m_HasRun = false;
	return true;
}

//-----------------------------------------------
//�ͻ���SOCKET��ȡ�����߳�
unsigned int __stdcall  CTCPServerState::threadClientSocket(void* pPara)
{
	PClientContext pContext = (PClientContext)pPara;
	CTCPServerState* pState = (CTCPServerState*)pContext->pParent;
	char buffer[1024] = {0};
	int bufferLen = sizeof(buffer);
	//cout<<"�ɹ������ͻ���"<< inet_ntoa(pContext->fromAddr.sin_addr)<<"�Ķ�ȡ�����̣߳�"<<endl;
	while(false == pContext->bExit)
	{
		memset(buffer, 0, bufferLen);
		//ʹ������ʽ��ʽ����ȡ����
		int size = ::recv(pContext->client, buffer, bufferLen, 0);
		if(size > 0)
		{
			char* from = inet_ntoa(pContext->fromAddr.sin_addr);
			if(pState->m_OnClientRcv) pState->m_OnClientRcv(from, buffer, pState->m_pContext);
			char sendBuffer[1024] = {0};
			sprintf(sendBuffer, "ACK:%s", buffer);
			int sendSize = strlen(sendBuffer);
			//�ظ������߷��͵��ַ���
			send(pContext->client, sendBuffer, sendSize, 0);
		}else //socket���رջ����쳣
		{
			//cout<<"SOCKET���رջ��쳣!"<<endl;
			break;
		}	
	}
	delete pContext;
	return 0;
}
