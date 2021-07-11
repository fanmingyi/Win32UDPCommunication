// UDPService.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include <Winsock2.h>
//导入动态库
#pragma comment(lib,"Ws2_32.lib")
#include "Proto.h"
#include<utility>
#include <list>
#include<time.h>
using namespace std;

list <pair<unsigned short, string>> g_lstUserInfo;
list <pair<unsigned short, clock_t>> g_lstUserHeart;//心跳

bool SendPackage(SOCKET sockServer, char* pBuff, int nLen, sockaddr_in siClient) {
	//发送包的长度
	int nSize = nLen;

	int nRet = sendto(sockServer, (char *)&nSize, sizeof(nSize), 0, (sockaddr *)(&siClient), sizeof(siClient));

	if (nRet == SOCKET_ERROR)
	{
		printf("通知登录失败");
		return false;

	}

	nRet = sendto(sockServer, (char*)pBuff, nSize, 0, (sockaddr *)(&siClient), sizeof(siClient));

	if (nRet == SOCKET_ERROR)
	{
		printf("通知登录失败");
		return false;
	}
}

bool SendClientInfos(SOCKET sockServer) {

	//向所有的登录的客户端发送信息，有新的客户端登录了
	char * pPkgBuff = new char[sizeof(CLIENTLOGINPKG) + sizeof(USERINFO)*g_lstUserInfo.size()];
	PCLIENTLOGINPKG pClpkg = (PCLIENTLOGINPKG)pPkgBuff;
	pClpkg->m_nCommand = S2C_CLIENTLOGIN;
	pClpkg->m_nUserCount = g_lstUserInfo.size();
	int nIdx = 0;
	for (auto &user : g_lstUserInfo)
	{
		//端口号
		pClpkg->m_users[nIdx].m_nDstPort = user.first;
		//名字
		strcpy(pClpkg->m_users[nIdx++].m_szDstNickName, user.second.c_str());
	}

	for (auto& user : g_lstUserInfo)
	{
		sockaddr_in si;
		si.sin_family = AF_INET;
		si.sin_port = user.first;
		si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		int nSize = sizeof(CLIENTLOGINPKG) + sizeof(USERINFO)*g_lstUserInfo.size();
		int nRet = sendto(sockServer, (char *)&nSize, sizeof(nSize), 0, (sockaddr*)&si, sizeof(si));
		if (nRet == SOCKET_ERROR)
		{
			printf("通知所有用户失败");
			return false;
		}

		nRet = sendto(sockServer, pPkgBuff, nSize, 0, (sockaddr*)&si, sizeof(si));
		if (nRet == SOCKET_ERROR)
		{
			printf("通知所有用户失败");
			return false;
		}

	}


	delete[]pPkgBuff;
	return true;
}

bool g_bRunCheckHeart = false; //标志 是否检测心跳

DWORD WINAPI HeartThreadFunc(LPVOID lpParam) {


	SOCKET sockServer = (SOCKET)lpParam;

	while (g_bRunCheckHeart)
	{
		//当前时间
		clock_t clockCurrent = clock();

		for (auto itr = g_lstUserHeart.begin(); itr != g_lstUserHeart.end(); ++itr)
		{
			//超过三秒视为下线
			if ((clockCurrent - itr->second) / CLOCKS_PER_SEC > 3)
			{
				g_lstUserHeart.erase(itr);
				for (auto itrUser = g_lstUserInfo.begin(); itrUser != g_lstUserInfo.end(); ++itrUser) {
					if (itrUser->first == itrUser->first)
					{
						g_lstUserInfo.erase(itrUser);
						break;
					}
				}

			}

		}
		SendClientInfos(sockServer );
		Sleep(1000);
	}
	return 0;
}
void OnParseCommand(SOCKET sockServer, char* pBuff, sockaddr_in  siClient) {
	switch (*(int*)pBuff)
	{
	case C2S_PRIVATE: {


		PPRIVATPKG pPrivatePkg = (PPRIVATPKG)pBuff;


		char * pS2cPrivatePkgBuf = new char[sizeof(S2CPRIVATEPKG) + pPrivatePkg->m_nMsgLen];
		PS2CPUBLICPKG pS2cPrivate = (PS2CPUBLICPKG)pS2cPrivatePkgBuf;
		pS2cPrivate->m_nCommand = S2C_PRIVATE;
		pS2cPrivate->m_userSendPublic.m_nDstPort = siClient.sin_port;

		strcpy(pS2cPrivate->m_userSendPublic.m_szDstNickName, pPrivatePkg->m_szMyNickName);
		pS2cPrivate->m_nMsgLen = pPrivatePkg->m_nMsgLen;
		strcpy(pS2cPrivate->m_szMsg, pPrivatePkg->m_szMsg);

		//发包
		sockaddr_in si;
		si.sin_family = AF_INET;
		si.sin_port = pPrivatePkg->m_userDst.m_nDstPort;
		si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

		//将私聊信息发送给目标
		if (!SendPackage(sockServer, pS2cPrivatePkgBuf, sizeof(S2CPRIVATEPKG) + pPrivatePkg->m_nMsgLen, si))
		{
			printf("发送私聊信息失败 \r\n");
		}
		//将私聊信息发送会原发送者
		si.sin_port = siClient.sin_port;
		if (!SendPackage(sockServer, pS2cPrivatePkgBuf, sizeof(S2CPRIVATEPKG) + pPrivatePkg->m_nMsgLen, si))
		{
			printf("发送私聊信息失败 \r\n");
		}

		break;
	}

	case C2S_LOGIN://登录包
	{
		PLONGPKG pLoginPkg = (PLONGPKG)pBuff;
		g_lstUserInfo.push_back(pair<unsigned short, string>(siClient.sin_port, pLoginPkg->m_szMyNickName));


		SendClientInfos(sockServer);

		break;
	}

	case C2S_PUBLIC: {
		//公聊
		PPUBLICPKG pPublicPkg = (PPUBLICPKG)pBuff;


		//组包
		char * pS2cPublicPkgBuf = new char[sizeof(S2CPUBLICPKG) + pPublicPkg->m_nMsgLen];
		PS2CPUBLICPKG pS2cPublic = (PS2CPUBLICPKG)pS2cPublicPkgBuf;
		pS2cPublic->m_nCommand = S2C_PUBLIC;
		pS2cPublic->m_userSendPublic.m_nDstPort = siClient.sin_port;
		strcpy(pS2cPublic->m_userSendPublic.m_szDstNickName, pPublicPkg->m_szMyNickName);

		pS2cPublic->m_nMsgLen = pPublicPkg->m_nMsgLen;
		strcpy(pS2cPublic->m_szMsg, pPublicPkg->m_szMsg);


		for (auto& user : g_lstUserInfo)
		{
			sockaddr_in si;
			si.sin_family = AF_INET;
			si.sin_port = user.first;
			si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			int nSize = sizeof(CLIENTLOGINPKG) + sizeof(USERINFO)*g_lstUserInfo.size();

			if (!SendPackage(sockServer, pS2cPublicPkgBuf, nSize, si)) {

				printf("发送公共聊天信息失败 \r\n");
				continue;
			}

		}

		delete[] pS2cPublicPkgBuf;

		break;
	}


					 //下线包
	case C2S_LOGOUT:
	{
		PLOGINOUTPKG pLogoutPkg = (PLOGINOUTPKG)pBuff;
		//删除下线的客户端
		for (auto itr = g_lstUserInfo.begin(); itr != g_lstUserInfo.end(); itr++)
		{
			if (itr->first == siClient.sin_port)
			{
				g_lstUserInfo.erase(itr);
				break;
			}
		}
		//重新发送
		SendClientInfos(sockServer);
	}
	case C2S_HEART:
	{
		//心跳包
		break;
	}
	break;
	default:
		break;
	}
}

int main()
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return 1;
	}
	else
		printf("The Winsock 2.2 dll was found okay\n");


	//（2）创建套接字对象，用于指定协议类型
	//https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket
	SOCKET sockServer = socket(
		AF_INET,//指定协议是IPV4 还是IPV6.AF_INET是IPV4，AF_INET6是IPV6
		SOCK_DGRAM,//指定是TCP（SOCK_STREAM）或者UDP（SOCK_DGRAM）
		IPPROTO_UDP//指定具体的协议类型，如ICMP,IPPROTO_TCP或IPPROTO_UDP 
	);

	if (sockServer == INVALID_SOCKET)
	{
		printf("创建socket失败 \r\n");
		return 0;
	}

	//（3）绑定套接字所指定的端口
	sockaddr_in si;
	si.sin_family = AF_INET;
	//htons函数用于转化大小端，网络传输字节流统一采用大端
	//但在x86使用小端编码，所以利用htons屏蔽大小端
	si.sin_port = htons(0x5566);
	//inet_addr同上
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//绑定端口
	int nRet = bind(
		sockServer,//指定的协议
		(sockaddr *)&si,//指定的端口信息
		sizeof(si)
	);

	if (nRet == SOCKET_ERROR)
	{
		printf("绑定端口失败\r\n");
		WSACleanup();
		return 0;
	}


	//接收数据
	sockaddr_in siRecf;

	char aryBuf[MAXBYTE] = { 0 };
	int nNameLen = sizeof(siRecf);
	while (true)
	{

		int nSize = 0;
		nRet = recvfrom(sockServer, (char *)&nSize, sizeof(nSize), 0, (sockaddr *)&siRecf, &nNameLen);
		if (nRet == SOCKET_ERROR)
		{
			printf("发送错误\r\n");
			WSACleanup();
			return 0;
		}

		char* pBuff = new char[nSize];
		nRet = recvfrom(sockServer, pBuff, nSize, 0, (sockaddr *)&siRecf, &nNameLen);
		if (nRet == SOCKET_ERROR)
		{
			printf("接收信息错误\r\n");
			WSACleanup();
			return 0;
		}

		OnParseCommand(sockServer, pBuff, siRecf);


		/*	std::cout << "收到客户端信息:" << aryBuf << std::endl;

			char aryBuf[MAXBYTE] = { "hello Server" };

			printf("请输入要发送的数据\r\n");

			std::cin >> aryBuf;
			nRet = sendto(sockServer, aryBuf, sizeof(aryBuf), 0, (sockaddr *)&siRecf, sizeof(siRecf));
			if (nRet == SOCKET_ERROR)
			{
				printf("发送数据错误 \r\n");
				WSACleanup();
				return 0;
			}*/

	}




	//关闭资源


	closesocket(sockServer);

	/* The Winsock DLL is acceptable. Proceed to use it. */

	/* Add network programming using Winsock here */

	/* then call WSACleanup when done using the Winsock dll */

	WSACleanup();



	return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
