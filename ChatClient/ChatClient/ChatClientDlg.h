
// ChatClientDlg.h: 头文件
//

#pragma once



enum MYMSG {
	WM_NEWUSER= WM_USER + 1,
	WM_UPDATEMSG
};

#include"Proto.h"
// CChatClientDlg 对话框
class CChatClientDlg : public CDialogEx
{
	// 构造
public:
	CChatClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATCLIENT_DIALOG };
#endif

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
	SOCKET m_sockClient;//客户端
	sockaddr_in m_siServer;//服务器的IP和端口
	static DWORD WINAPI RecvThreadFunc(_In_ LPVOID lpParameter);
	HANDLE m_tThread;
	BOOL m_bRunRecv;
	void OnParseCommand(char *pBuff);
private:
	CList<USERINFO> m_lstUserinfos;

public:
	afx_msg void OnBnClickedLogin();
protected:
	afx_msg LRESULT OnNewuser(WPARAM wParam, LPARAM lParam);
public:
	CListBox m_lstbUsers;
	afx_msg void OnBnClickedSendMsg();
	BOOL m_bPrivate;
	CString m_strMsg;
	CString m_strNick;
	CString m_strShowMsg;
	int TIMER_ID=0x225;
private:
	bool SendPackge(char *pBuff,int nLen);
protected:
	afx_msg LRESULT OnUpdatemsg(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
