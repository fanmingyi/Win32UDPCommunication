
// ChatClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "Proto.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include<WS2tcpip.h>


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatClientDlg 对话框



CChatClientDlg::CChatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
	, m_bPrivate(FALSE)
	, m_strMsg(_T(""))
	, m_strNick(_T(""))
	, m_strShowMsg(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT4, m_lstbUsers);
	DDX_Control(pDX, LIST_LOGIN, m_lstbUsers);
	DDX_Check(pDX, IDC_CHECK1, m_bPrivate);
	DDX_Text(pDX, IDC_EDIT5, m_strMsg);
	DDX_Text(pDX, EDT_NAME, m_strNick);
	DDX_Text(pDX, IDC_EDIT3, m_strShowMsg);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CChatClientDlg::OnBnClickedLogin)
	ON_MESSAGE(WM_NEWUSER, &CChatClientDlg::OnNewuser)
	ON_BN_CLICKED(IDC_BUTTON4, &CChatClientDlg::OnBnClickedSendMsg)
	ON_MESSAGE(WM_UPDATEMSG, &CChatClientDlg::OnUpdatemsg)
	ON_BN_CLICKED(IDC_BUTTON3, &CChatClientDlg::OnBnClickedButton3)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CChatClientDlg 消息处理程序

BOOL CChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
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

	srand(time(NULL));

	CString strFmt;
	strFmt.Format("%d", rand() % 10000);
	m_strNick = strFmt;
	UpdateData(FALSE);

	m_siServer.sin_family = AF_INET;
	m_siServer.sin_port = htons(0x5566);


	char sendBuf[20] = { '\0' };


	struct sockaddr_in dst;
	InetPton(AF_INET, _T("127.0.0.1"), &m_siServer.sin_addr.s_addr);
	//m_siServer.sin_addr.S_un.S_addr = d;
	//inet_addr("127.0.0.1");


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChatClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



DWORD __stdcall CChatClientDlg::RecvThreadFunc(LPVOID lpParameter)
{
	CChatClientDlg* pThis = (CChatClientDlg*)lpParameter;
	//设置超时
	/*struct timeval tv;
	tv.tv_sec = 10000;
	tv.tv_usec = 0;
	if (setsockopt(pThis->m_sockClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0) {
		perror("Error");
	}*/
	while (pThis->m_bRunRecv)
	{
		sockaddr_in  siRecv;
		int nNameLen = sizeof(siRecv);
		int nSize = 0;


		int nRet = recvfrom(pThis->m_sockClient, (char*)&nSize, sizeof(nSize), 0, (sockaddr*)&siRecv, &nNameLen);
		//关闭连接
		if (nRet == 0)
		{
			break;
		}
		if (nRet == SOCKET_ERROR)
		{
			break;
		}
		char *pBuff = new char[nSize];
		nRet = recvfrom(pThis->m_sockClient, pBuff, nSize, 0, (sockaddr*)&siRecv, &nNameLen);

		pThis->OnParseCommand(pBuff);
		delete[]pBuff;
	}

	return 0;
}

void CChatClientDlg::OnParseCommand(char * pBuff)
{
	switch (*(int *)pBuff)
	{
	case S2C_PRIVATE: {

		PS2CPRIVATEPKG pS2cPrivate = (PS2CPRIVATEPKG)pBuff;

		sockaddr_in siSelf;
		int nSiLen = sizeof(siSelf);
		getsockname(m_sockClient, (sockaddr*)&siSelf, &nSiLen);

		CString strMsg;

		//如果私聊发送者等于自己
		if (pS2cPrivate->m_userSendPublic.m_nDstPort == siSelf.sin_port) {

			strMsg.Format("你对 %s:%d 说 %s \r\n",
				pS2cPrivate->m_userSendPublic.m_szDstNickName,
				ntohs(pS2cPrivate->m_userSendPublic.m_nDstPort),
				pS2cPrivate->m_szMsg
			);

		}
		else {
			strMsg.Format("%s:%d \t 对你说 %s \r\n",
				pS2cPrivate->m_userSendPublic.m_szDstNickName,
				ntohs(pS2cPrivate->m_userSendPublic.m_nDstPort),
				pS2cPrivate->m_szMsg
			);
		}




		m_strShowMsg += strMsg;

		PostMessage(WM_UPDATEMSG, NULL, NULL);


	}break;
	case S2C_CLIENTLOGIN://有新用户登录
	{



		PCLIENTLOGINPKG pClpkg = (PCLIENTLOGINPKG)pBuff;


		m_lstUserinfos.RemoveAll();
		for (size_t i = 0; i < pClpkg->m_nUserCount; i++)
		{
			m_lstUserinfos.AddTail(pClpkg->m_users[i]);
		}

		SendMessage(WM_NEWUSER, NULL, NULL);

	}
	break;
	case S2C_PUBLIC://群消息
	{

		//添加信息到界面
		PS2CPUBLICPKG pS2cPublic = (PS2CPUBLICPKG)pBuff;



		CString strMsg;

		strMsg.Format("%s:%d 说 %s \r\n",
			pS2cPublic->m_userSendPublic.m_szDstNickName,
			ntohs(pS2cPublic->m_userSendPublic.m_nDstPort),
			pS2cPublic->m_szMsg
		);

		m_strShowMsg += strMsg;

		PostMessage(WM_UPDATEMSG, NULL, NULL);
	}
	break;
	default:
		break;
	}

}

void CChatClientDlg::OnBnClickedLogin()
{
	m_sockClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_sockClient == INVALID_SOCKET)
	{
		AfxMessageBox("启动错误");
	}


	//向服务器发送登录包
	CString strNickName;
	GetDlgItemText(EDT_NAME, strNickName);

	LONGPKG pkg;
	pkg.m_nConmmand = C2S_LOGIN;
	strcpy(pkg.m_szMyNickName, strNickName.GetBuffer(0));



	//发送包的长度
	int nSize = sizeof(pkg);
	//发送数据包长度包
	int nRet = sendto(m_sockClient, (char*)&nSize, sizeof(nSize), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));
	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox("登录失败");
	}


	nRet = sendto(m_sockClient, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));

	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox("登录失败");
	}

	m_tThread = CreateThread(NULL, 0, RecvThreadFunc, this, 0, NULL);

	if (m_tThread == NULL)
	{
		AfxMessageBox("登陆失败");
	}

	//设置定时器消息
	SetTimer(TIMER_ID, 1000, NULL);

	m_bPrivate = TRUE;

}


afx_msg LRESULT CChatClientDlg::OnNewuser(WPARAM wParam, LPARAM lParam)
{
	m_lstbUsers.ResetContent();

	POSITION pos = m_lstUserinfos.GetHeadPosition();

	while (pos)
	{
		auto& user = m_lstUserinfos.GetNext(pos);
		int nIdx = m_lstbUsers.AddString(user.m_szDstNickName);

		m_lstbUsers.SetItemData(nIdx, user.m_nDstPort);
	}

	return 0;
}


void CChatClientDlg::OnBnClickedSendMsg()
{
	UpdateData(TRUE);
	//私聊
	if (m_bPrivate)
	{
		//
		int nIdx = m_lstbUsers.GetCurSel();
		if (nIdx == -1)
		{
			AfxMessageBox("请选择私聊的用户");
			return;
		}

		CString strDstNickName;

		m_lstbUsers.GetText(nIdx, strDstNickName);
		unsigned short nPort = m_lstbUsers.GetItemData(nIdx);

		//组包
		char * pPublicPkgBuff = new char[sizeof(PRIVATPKG) + m_strMsg.GetLength() + 1];
		PPRIVATPKG pPrivatePkg = (PPRIVATPKG)pPublicPkgBuff;
		pPrivatePkg->m_nCommand = C2S_PRIVATE;
		strcpy(pPrivatePkg->m_szMyNickName, m_strNick.GetBuffer());

		pPrivatePkg->m_userDst.m_nDstPort = nPort;
		strcpy(pPrivatePkg->m_userDst.m_szDstNickName, strDstNickName.GetBuffer());
		pPrivatePkg->m_nMsgLen = m_strMsg.GetLength();
		strcpy(pPrivatePkg->m_szMsg, m_strMsg.GetBuffer());

		//发包
		if (!SendPackge(pPublicPkgBuff, sizeof(PRIVATPKG) + m_strMsg.GetLength() + 1))
		{
			AfxMessageBox("发送私聊包失败");
		}

		//清理
		delete[] pPublicPkgBuff;


	}
	else {
		//群聊
		char *pPublicPkgBuff = new char[sizeof(PUBLICPKG) + m_strMsg.GetLength() + 1];
		PPUBLICPKG pPublicPkg = (PPUBLICPKG)pPublicPkgBuff;
		pPublicPkg->m_nCommand = C2S_PUBLIC;
		strcpy(pPublicPkg->m_szMyNickName, m_strNick.GetBuffer());
		pPublicPkg->m_nMsgLen = m_strMsg.GetLength() + 1;
		strcpy(pPublicPkg->m_szMsg, m_strMsg.GetBuffer());

		//长度
		int nSize = sizeof(PUBLICPKG) + m_strMsg.GetLength() + 1;

		//发包
		if (!SendPackge(pPublicPkgBuff, nSize))
		{
			AfxMessageBox("发送群聊包失败");
		}

		//清理
		delete[] pPublicPkgBuff;

	}
}


afx_msg bool CChatClientDlg::SendPackge(char * pBuff, int nLen)
{
	//长度
	int nSize = nLen;

	//发送数据包长度包
	int nRet = sendto(m_sockClient, (char*)&nSize, sizeof(nSize), 0, (sockaddr*)&m_siServer, sizeof(m_siServer));

	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox("登录失败");
		return FALSE;
	}

	nRet = sendto(m_sockClient, pBuff, nSize, 0, (sockaddr*)&m_siServer, sizeof(m_siServer));

	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox("登录失败");
		return FALSE;
	}

	return TRUE;
}

LRESULT CChatClientDlg::OnUpdatemsg(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);

	return 0;
}


//点击下线按钮
void CChatClientDlg::OnBnClickedButton3()
{
	//发送下线包

	//组包
	LOGINOUTPKG logoutPkg;
	logoutPkg.m_nConmmand = C2S_LOGOUT;
	//发包
	SendPackge((char *)&logoutPkg, sizeof logoutPkg);

	//关闭socket
	closesocket(m_sockClient);

	//线程结束标志
	m_bRunRecv = FALSE;


	DWORD dwRet = WaitForSingleObject(m_tThread, 1000);

	if (dwRet == WAIT_TIMEOUT)
	{
		::TerminateThread(m_tThread, -1);
	}
	CloseHandle(m_tThread);
	//清空列表
	m_lstbUsers.ResetContent();

	//关闭定时任务
	KillTimer(TIMER_ID);
}


void CChatClientDlg::OnTimer(UINT_PTR nIDEvent)
{

	CDialogEx::OnTimer(nIDEvent);
	//发送心跳包
	if (m_bPrivate) {
		//群聊
		HEARTPKG hearPkg;

		//长度
		int nSize = sizeof(HEARTPKG);
		//发包
		if (!SendPackge((char*)&hearPkg, nSize))
		{
			AfxMessageBox("发送心跳包失败");
		}

	}
}
