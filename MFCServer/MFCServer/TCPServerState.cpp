#include "TCPServerState.h"
#include <windows.h>

#include <stdio.h>

#pragma comment(lib,  "Iphlpapi.lib")
#pragma comment(lib,  "Ws2_32.lib")


//-----------------------------------------------
//构造函数
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
//析构函数
CTCPServerState::~CTCPServerState(void)
{
}

//-----------------------------------------------
//注册客户端相关回调函数
void  CTCPServerState::RegOnClientConnect(POnServerInitDef pOnInit, POnClientConnectDef pOnConect, POnClientRcvDef pOnRcv, void* pContext)
{
	m_OnServerInit = pOnInit;
	m_OnClientConnect = pOnConect;
	m_OnClientRcv = pOnRcv;
	m_pContext = pContext;


}

//-----------------------------------------------
//初始化SOCKET函数
bool CTCPServerState::Init(char* errorInfo)
{
	if(0 != ::WSAStartup(MAKEWORD(2,2), &m_wsaData))
	{
		sprintf(errorInfo, "%s", "初始化网络库失败!");
		return false;
	}	
	m_TCP = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if(INVALID_SOCKET == m_TCP)
	{
		sprintf(errorInfo, "%s", "创建SOCKET失败!");
		return false;
	}
	m_InitOK = true;
	m_HasRun = false;
	
	return true;
}
//-----------------------------------------------
//删除网络库
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
//运行服务端功能
bool  CTCPServerState::Run(char* serverIP, short port, char* errorInfo)
{
	//已经处于运行中，直接返回
	if(m_HasRun) return false;
	m_Port = port;
	sprintf(m_ServerIP, "%s", serverIP);

	//这里是本机地址127.0.0.1进行通信
	sockaddr_in LocalAddr;
	LocalAddr.sin_family = AF_INET;	
	LocalAddr.sin_addr.S_un.S_addr =   ::inet_addr(m_ServerIP);;
	LocalAddr.sin_port = ::htons(m_Port);

	//进行SOCKET绑定
	if(::bind(m_TCP, (LPSOCKADDR)&LocalAddr, sizeof(LocalAddr)) == SOCKET_ERROR)
	{
		sprintf(errorInfo, "绑定到%s:%d失败!", m_ServerIP, m_Port);
		::closesocket(m_TCP);
		if(m_OnServerInit) m_OnServerInit(errorInfo, m_pContext);
		return false;
	}
	char info[256] = {0};
	sprintf(info, "成功绑定到%s:%d!", m_ServerIP, m_Port);
	if(m_OnServerInit) m_OnServerInit(info, m_pContext);

	//开始进行监听连接请求
	if( SOCKET_ERROR  == ::listen(m_TCP, 8))
	{
		sprintf(errorInfo, "%s", "监听SOCKET失败!");
		if(m_OnServerInit) m_OnServerInit(errorInfo, m_pContext);
		return false;
	}
	sprintf(info, "%s",  "成功监听SOCKET!");
	if(m_OnServerInit) m_OnServerInit(info, m_pContext);

	//定义用于接收数据的缓冲区
	char buffer[1024] = {0};
	int bufferLen = sizeof(buffer);
	//定义地址对象，用于记录发送数据方的地址信息。
	sockaddr_in  fromAddr;
	int addrLen = sizeof(fromAddr);
	m_HasRun = true;
	while(m_InitOK)
	{
		
		//等待客户端的连接
		sprintf(info, "%s",  "等待客户机的连接!");
		if(m_OnServerInit) m_OnServerInit(info, m_pContext);

		SOCKET AcceptSocket = accept(m_TCP, (sockaddr*)&fromAddr, &addrLen);
		if(INVALID_SOCKET  != AcceptSocket)
		{
			char* fromIP = inet_ntoa(fromAddr.sin_addr);
			char clientInfo[128] = {0};
			sprintf(clientInfo, "接收到%s的客户端连接", fromIP);			
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
//客户端SOCKET读取数据线程
unsigned int __stdcall  CTCPServerState::threadClientSocket(void* pPara)
{
	PClientContext pContext = (PClientContext)pPara;
	CTCPServerState* pState = (CTCPServerState*)pContext->pParent;
	char buffer[1024] = {0};
	int bufferLen = sizeof(buffer);
	//cout<<"成功创建客户机"<< inet_ntoa(pContext->fromAddr.sin_addr)<<"的读取数据线程！"<<endl;
	while(false == pContext->bExit)
	{
		memset(buffer, 0, bufferLen);
		//使用阻塞式方式来读取数据
		int size = ::recv(pContext->client, buffer, bufferLen, 0);
		if(size > 0)
		{
			char* from = inet_ntoa(pContext->fromAddr.sin_addr);
			if(pState->m_OnClientRcv) pState->m_OnClientRcv(from, buffer, pState->m_pContext);
			char sendBuffer[1024] = {0};
			sprintf(sendBuffer, "ACK:%s", buffer);
			int sendSize = strlen(sendBuffer);
			//回复发送者发送的字符。
			send(pContext->client, sendBuffer, sendSize, 0);
		}else //socket被关闭或发生异常
		{
			//cout<<"SOCKET被关闭或异常!"<<endl;
			break;
		}	
	}
	delete pContext;
	return 0;
}
