#pragma once
#include "stdafx.h"

// 클라이언트 캡슐화
class cClient
{
private:
	SOCKADDR_IN servAddr;

public:
	SOCKET		sock;
	int			servAddrSize;
	cClient();
	~cClient();

	//프로퍼티
	//SOCKET		GetMySocket()			{ return sock; }
	SOCKADDR_IN GetSocketAddress()		{ return servAddr; }
	int			GetSocketAddressSize()	{ return servAddrSize; }

	void SetServAddress(SOCKADDR_IN* addr)	{ servAddr = *addr; }
	void SetSocketAddressSize(int size)		{ servAddrSize = size; }
	void SetMySocket(SOCKET s)				{ sock = s; }
};