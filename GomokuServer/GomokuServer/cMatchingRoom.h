#pragma once
#include "stdafx.h"
#include "cMatchingRoomManager.h"
#include "cClient.h"

#ifdef MATCHING_ROOM

// ��Ī��
class cMatchingRoom : public cMatchingRoomManager
{
private:
	cClient* m_pBlackStone;	// �浹
	cClient* m_pWhiteStone;	// �鵹

	int board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // �� �׸���

public:
	cMatchingRoom();
	~cMatchingRoom();

	cClient* GetBlackStone() { return m_pBlackStone; }
	cClient* GetWhiteStone() { return m_pWhiteStone; }

	void CreateMatchingRoom(cClient* black, cClient* white);
	void GameStart();

	int CheckWin(PacketInfo p);			// �̰���� �˻�
	int CheckRow(int x, int y);			// ���� �˻�
	int CheckColm(int x, int y);		// ���� �˻�
	int CheckDiagonal(int x, int y);	// �밢�� �˻�
};

#endif // MATCHING_ROOM

