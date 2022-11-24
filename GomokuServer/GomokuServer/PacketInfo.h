#pragma once
#include "stdafx.h"
enum ClientState		// �� ����
{
	eNone = 0,		// ����
	eJoin_Room,		// �� ����
	eGame_Start,	// ���� ����
	eBlack,			// �浹 ��
	eWhite,			// �鵹 ��
	eFinish,		// ��
};

struct PacketInfo				// ������ ��Ŷ
{
	SOCKET		s;				// ��� ���Ͽ��� �Դ���?
	int			x, y;			// �ٵ��� ��ǥ
	int			stoneColor;		// �� ����
	ClientState curState;		// ���� ����

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