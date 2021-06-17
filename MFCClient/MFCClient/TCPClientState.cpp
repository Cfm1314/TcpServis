#include "TCPClientState.h"
#include <stdio.h>


#pragma comment(lib,  "Iphlpapi.lib")
#pragma comment(lib,  "Ws2_32.lib")

//-----------------------------------------------
//���캯��
TCPClientState::TCPClientState(void)
{
	m_ConnectOK = false;
	m_InitOK = false;

	sprintf(m_RemoteServer, "%s", "127.0.0.1");
	m_RemotePort = 2000;
}
//-----------------------------------------------
//��������
TCPClientState::~TCPClientState(void)
{
}

//-----------------------------------------------
//ע�������¼��ص�����
void  TCPClientState::RegCallBackFunc(POnClientInitDef pInit, POnConnectDef pConnect, POnRcvDef pOnRcv, void* pContext)
{
	m_pOnClientInit = pInit;
	m_pOnConnect = pConnect;
	m_pOnRcv = pOnRcv;
	m_pContext = pContext;
}

//-----------------------------------------------
//��ʼ��SOCKET����
bool TCPClientState::Init()
{
	if(0 != ::WSAStartup(MAKEWORD(2,2), &m_wsaData))
	{
		if(m_pOnClientInit) m_pOnClientInit("��ʼ�������ʧ��!", m_pContext);
		return false;
	}	
	if(m_pOnClientInit) m_pOnClientInit("��ʼ�������ɹ�!", m_pContext);
	m_TCP = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if(INVALID_SOCKET == m_TCP)
	{
		if(m_pOnClientInit) m_pOnClientInit("����SOCKETʧ��!", m_pContext);
		return false;
	}
	if(m_pOnClientInit) m_pOnClientInit("�ɹ�����SOCKET!", m_pContext);
	m_InitOK = true;
	
	if(m_pOnClientInit) m_pOnClientInit("�ͻ��˳���ɹ���ɳ�ʼ��!", m_pContext);
	return true;
}
//-----------------------------------------------
//ɾ�������
bool  TCPClientState::Destroy()
{
	m_InitOK = false;
	::closesocket(m_TCP);	
	if(SOCKET_ERROR == ::WSACleanup())
	{
		if(m_pOnClientInit) m_pOnClientInit("���������ʧ��!", m_pContext);
		return false;
	}
	if(m_pOnClientInit) m_pOnClientInit("���������ɹ�!", m_pContext);
	return true;

}
//-----------------------------------------------
//���пͻ��˹���
bool  TCPClientState::Connect(char* remote, short port)
{
	//�����Ǳ�����ַ127.0.0.1����ͨ��

	sprintf(m_RemoteServer, "%s", remote);
	m_RemotePort = port;

	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;	
	remoteAddr.sin_addr.S_un.S_addr =   ::inet_addr(m_RemoteServer);;
	remoteAddr.sin_port = ::htons(m_RemotePort);
	int addrLen = sizeof(remoteAddr);

	//�������ڷ������ݵĻ�����
	char buffer[1024] = {0};
	int  bufferLen = sizeof(buffer);
	if(connect(m_TCP, (sockaddr*)&remoteAddr, addrLen) == SOCKET_ERROR)
	{
		if(m_pOnConnect) m_pOnConnect(false,"���ӷ�����ʧ��!", m_pContext);
		return false;
	}
	if(m_pOnConnect) m_pOnConnect(true, "���ӷ������ɹ�!", m_pContext);
	m_ConnectOK = true;
	
	//������ȡSOCKET�̺߳���
	unsigned int iThreadID;
	uintptr_t rt = 0;
	rt = _beginthreadex(NULL, 0, threadRcvData,  this, 0, &iThreadID);
	
	return true;
}

//-----------------------------------------------
//
bool TCPClientState::SendData(char* info, int size)
{
	if(false == m_ConnectOK)	return false;	
	if(send(m_TCP, info, size, 0) > 0) return true;
	return false;
}

//-----------------------------------------------
//
void	TCPClientState::RcvData()
{
	while(m_InitOK)
	{
		char rcvInfo[1024] = {0};
		int rcvLen = sizeof(rcvInfo);
		int ret = ::recv(m_TCP, rcvInfo, rcvLen, 0);
		char fromInfo[128] = {0};
		sprintf(fromInfo, "���Է����%s", m_RemoteServer);
		if(ret > 0)
		{
			if(m_pOnRcv) m_pOnRcv(fromInfo, rcvInfo, m_pContext);
			
		}
	}
}
//-----------------------------------------------
//
unsigned int __stdcall TCPClientState::threadRcvData(void* pPara)
{
	TCPClientState* pState = (TCPClientState*)pPara;
	pState->RcvData();

	return 0;
}