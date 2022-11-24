#pragma once
#include "stdafx.h"
enum ClientState		// 턴 상태
{
	eNone = 0,		// 없음
	eJoin_Room,		// 방 입장
	eGame_Start,	// 게임 시작
	eBlack,			// 흑돌 턴
	eWhite,			// 백돌 턴
	eFinish,		// 끝
};

struct PacketInfo				// 전달할 패킷
{
	SOCKET		s;				// 어느 소켓에서 왔는지?
	int			x, y;			// 바둑판 좌표
	int			stoneColor;		// 돌 색깔
	ClientState curState;		// 현재 상태

	PacketInfo()
	{
		s			= { 0 };
		x			= -1;
		y			= -1;
		stoneColor	= 0;
		curState	= eNone;
	}

	void SetPacket(PacketInfo p)
	{
		s			= { 0 };
		x			= p.x;
		y			= p.y;
		stoneColor	= p.stoneColor;
		curState	= p.curState;
	}
};