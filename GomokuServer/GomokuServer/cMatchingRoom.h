#pragma once
#include "stdafx.h"
#include "PacketInfo.h"
//#include "cMatchingRoomManager.h"
#include "cClient.h"


// 매칭룸
class cMatchingRoom
{
private:
	cClient* m_pBlackStone;	// 흑돌
	cClient* m_pWhiteStone;	// 백돌

	PacketInfo sendInfo;					// 보내려는 패킷
	int board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // 판 그리기

public:
	cMatchingRoom();
	~cMatchingRoom();

	cClient* GetBlackStone() { return m_pBlackStone; }
	cClient* GetWhiteStone() { return m_pWhiteStone; }

	void CreateMatchingRoom(cClient* black, cClient* white);
	void GameStart();
	void UpdateRoom(PacketInfo* p);

	//int CheckWin(PacketInfo p);			// 이겼는지 검사
	bool CheckWin(PacketInfo p);			// 이겼는지 검사
	int CheckRow(int x, int y);			// 가로 검사
	int CheckColm(int x, int y);		// 세로 검사
	int CheckDiagonal(int x, int y);	// 대각선 검사
};
