#pragma once
#include "stdafx.h"

// Ŭ���̾�Ʈ ĸ��ȭ
class cClient
{
private:
	SOCKADDR_IN servAddr;

public:
	SOCKET		sock;
	int			servAddrSize;
	cClient();
	~cClient();

	//������Ƽ
	//SOCKET		GetMySocket()			{ return sock; }
	SOCKADDR_IN GetSocketAddress()		{ return servAddr; }
	int			GetSocketAddressSize()	{ return servAddrSize; }

	void SetServAddress(SOCKADDR_IN* addr)	{ servAddr = *addr; }
	void SetSocketAddressSize(int size)		{ servAddrSize = size; }
	void SetMySocket(SOCKET s)				{ sock = s; }
};