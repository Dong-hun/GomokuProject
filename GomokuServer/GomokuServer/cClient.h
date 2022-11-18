#pragma once
#include "stdafx.h"

enum ClientState		// 턴 상태
{
#ifdef MATCHING_ROOM
	eNone = 0,		// 없음
	eJoin_Room,		// 방 입장
	eGame_Start,	// 게임 시작
	eBlack,			// 흑돌 턴
	eWhite,			// 백돌 턴
	eFinish,		// 끝
#else
	eNONE = 0,		// 없음
	eBLACK,			// 흑돌 턴
	eWHITE,			// 백돌 턴
	eFINISH,		// 끝
#endif // MATCHING_ROOM
};

// 클라이언트 캡슐화
class cClient
{
private:
	SOCKET		sock;
	SOCKADDR_IN servAddr;
	int			servAddrSize;

public:
	cClient();
	~cClient();

	//프로퍼티
	SOCKET		GetMySocket()			{ return sock; }
	SOCKADDR_IN GetSocketAddress()		{ return servAddr; }
	int			GetSocketAddressSize()	{ return servAddrSize; }

	void SetSocketAddressSize(int size) { servAddrSize = size; }
	void SetMySocket(SOCKET s)			{ sock = s; }

	void AcceptClient(SOCKET s);		// 연결
};