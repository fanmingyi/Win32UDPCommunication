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
	unsigned short	m_nDstPort;//�˿ں�
	char		    m_szDstNickName[NICKNAMELEN];//�ǳ�
}USERINFO, *PUSERINFO;

typedef struct tagPackageHeader {
	PackageCommand m_nCommand;//��������
}PACKAGEHEADER, *PPACKAGEHEADER;


typedef struct tagLongPkg {
	PackageCommand m_nConmmand;//��
	char m_szMyNickName[NICKNAMELEN];//�ҵ��ǳ�
}LONGPKG, *PLONGPKG;

//���߰�
typedef struct tagLoginoutPkg {
	PackageCommand m_nConmmand;//��
}LOGINOUTPKG, *PLOGINOUTPKG;

//֪ͨ���ͻ������û��İ������ݽṹ
typedef struct tagClientLoginPkg {
	PackageCommand m_nCommand;//��������
	int m_nUserCount;//�û��ĸ���
	USERINFO m_users[1];//�û�������
}CLIENTLOGINPKG, *PCLIENTLOGINPKG;


typedef struct tagPublicPkg {
	PackageCommand m_nCommand;//������
	char m_szMyNickName[NICKNAMELEN];
	int m_nMsgLen;
	char m_szMsg[1];
}PUBLICPKG, *PPUBLICPKG;

typedef struct tagS2CPublicPkg {
	PackageCommand m_nCommand;//������
	USERINFO m_userSendPublic;
	int m_nMsgLen;
	char m_szMsg[1];
}S2CPUBLICPKG, *PS2CPUBLICPKG;

typedef struct tagPrivatePkg {
	PackageCommand m_nCommand;
	char m_szMyNickName[NICKNAMELEN];
	USERINFO m_userDst;//�Է��ǳ�
	int m_nMsgLen;
	char m_szMsg[1];
}PRIVATPKG, *PPRIVATPKG;



typedef struct tagS2CPrivatePkg {
	PackageCommand m_nCommand;//������
	USERINFO m_userSendPublic;
	int m_nMsgLen;
	char m_szMsg[1];
}S2CPRIVATEPKG, *PS2CPRIVATEPKG;

//������
typedef struct tagHeartPkg {
	PackageCommand m_nConmmand;//��
}HEARTPKG, *PHEARTPKG;
