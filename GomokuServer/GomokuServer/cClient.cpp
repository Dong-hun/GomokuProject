#include "stdafx.h"
#include "cClient.h"

cClient::cClient()
{
	sock = { 0 };
	servAddr = { 0 };
	servAddrSize = sizeof(servAddr);
}

cClient::~cClient()
{
	sock = { 0 };
	servAddr = { 0 };
	servAddrSize = sizeof(servAddr);
}

void cClient::AcceptClient(SOCKET s)
{
	sock = accept(s, (SOCKADDR*)&servAddr, &servAddrSize);
}

