#pragma once
#include "stdafx.h"

enum ClientState		// �� ����
{
#ifdef MATCHING_ROOM
	eNone = 0,		// ����
	eJoin_Room,		// �� ����
	eGame_Start,	// ���� ����
	eBlack,			// �浹 ��
	eWhite,			// �鵹 ��
	eFinish,		// ��
#else
	eNONE = 0,		// ����
	eBLACK,			// �浹 ��
	eWHITE,			// �鵹 ��
	eFINISH,		// ��
#endif // MATCHING_ROOM
};

// Ŭ���̾�Ʈ ĸ��ȭ
class cClient
{
private:
	SOCKET		sock;
	SOCKADDR_IN servAddr;
	int			servAddrSize;

public:
	cClient();
	~cClient();

	//������Ƽ
	SOCKET		GetMySocket()			{ return sock; }
	SOCKADDR_IN GetSocketAddress()		{ return servAddr; }
	int			GetSocketAddressSize()	{ return servAddrSize; }

	void SetSocketAddressSize(int size) { servAddrSize = size; }
	void SetMySocket(SOCKET s)			{ sock = s; }

	void AcceptClient(SOCKET s);		// ����
};