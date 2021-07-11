#pragma once

#define NICKNAMELEN 64
enum PackageCommand {
	C2S_LOGIN,
	C2S_LOGOUT,
	C2S_PUBLIC,
	C2S_PRIVATE,
	C2S_HEART,

	S2C_CLIENTLOGIN,
	S2C_CLIENTOUT,

	S2C_PUBLIC,
	S2C_PRIVATE
};

typedef struct tagUserInfo {
	unsigned short	m_nDstPort;//端口号
	char		    m_szDstNickName[NICKNAMELEN];//昵称
}USERINFO, *PUSERINFO;

typedef struct tagPackageHeader {
	PackageCommand m_nCommand;//包的类型
}PACKAGEHEADER, *PPACKAGEHEADER;


typedef struct tagLongPkg {
	PackageCommand m_nConmmand;//包
	char m_szMyNickName[NICKNAMELEN];//我的昵称
}LONGPKG, *PLONGPKG;

//下线包
typedef struct tagLoginoutPkg {
	PackageCommand m_nConmmand;//包
}LOGINOUTPKG, *PLOGINOUTPKG;

//通知所客户端新用户的包的数据结构
typedef struct tagClientLoginPkg {
	PackageCommand m_nCommand;//包的类型
	int m_nUserCount;//用户的个数
	USERINFO m_users[1];//用户的数据
}CLIENTLOGINPKG, *PCLIENTLOGINPKG;


typedef struct tagPublicPkg {
	PackageCommand m_nCommand;//包类型
	char m_szMyNickName[NICKNAMELEN];
	int m_nMsgLen;
	char m_szMsg[1];
}PUBLICPKG, *PPUBLICPKG;

typedef struct tagS2CPublicPkg {
	PackageCommand m_nCommand;//包类型
	USERINFO m_userSendPublic;
	int m_nMsgLen;
	char m_szMsg[1];
}S2CPUBLICPKG, *PS2CPUBLICPKG;

typedef struct tagPrivatePkg {
	PackageCommand m_nCommand;
	char m_szMyNickName[NICKNAMELEN];
	USERINFO m_userDst;//对方昵称
	int m_nMsgLen;
	char m_szMsg[1];
}PRIVATPKG, *PPRIVATPKG;



typedef struct tagS2CPrivatePkg {
	PackageCommand m_nCommand;//包类型
	USERINFO m_userSendPublic;
	int m_nMsgLen;
	char m_szMsg[1];
}S2CPRIVATEPKG, *PS2CPRIVATEPKG;

//心跳包
typedef struct tagHeartPkg {
	PackageCommand m_nConmmand;//包
}HEARTPKG, *PHEARTPKG;
