#include "stdafx.h"
#include "cMatchingRoom.h"

cMatchingRoom::cMatchingRoom()
{
	m_pBlackStone = new cClient();
	m_pWhiteStone = new cClient();

	board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // 판 그리기
}

cMatchingRoom::~cMatchingRoom()
{
	board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // 판 그리기

	if (m_pWhiteStone != nullptr)
		delete m_pWhiteStone;
	if (m_pBlackStone != nullptr)
		delete m_pBlackStone;
}

// 방 생성
void cMatchingRoom::CreateMatchingRoom(cClient * black, cClient* white)
{
	//sendInfo.curState	= ClientState::eGame_Start;
	sendInfo.x			= -1;
	sendInfo.y			= -1;
	sendInfo.stoneColor	= BLACK;
	sendInfo.curState	= ClientState::eGame_Start;
	m_pBlackStone		= black;
	send(m_pBlackStone->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);

	sendInfo.stoneColor	= WHITE;
	m_pWhiteStone		= white;
	send(m_pWhiteStone->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);

	sendInfo.curState = ClientState::eBlack;
	send(m_pBlackStone->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);
	send(m_pWhiteStone->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);
}

void cMatchingRoom::GameStart()
{

}

void cMatchingRoom::ProcessMessageRoomClient(PacketInfo* p)	// 방 업데이트
{
	sendInfo = *p;

	if (CheckWin(sendInfo))						// 누군가 이겼다면
	{
		sendInfo.curState = eFinish;			// 상태만 끝으로 해서 변경
	}
	else										// 아니라면
	{
		if (sendInfo.curState == eBlack)		// 현재 상태가 흑이라면
			sendInfo.curState = eWhite;			// 백으로 변경
		else if (sendInfo.curState == eWhite)	// 백이라면
			sendInfo.curState = eBlack;			// 흑으로 변경
	}

	// 메세지 보내기
	send(m_pBlackStone->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);
	send(m_pWhiteStone->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);
}

bool cMatchingRoom::CheckWin(PacketInfo p)	// 승패 검사
{
	//std::cout << "승패 검사" << std::endl;
	int x = p.x;
	int y = p.y;
	board[x][y] = p.stoneColor;	// 서버 보드에 받은 좌표의 돌 컬러 넣기 (x = 세로 y = 가로)

	if (CheckRow(x, y) == 5 || CheckColm(x, y) == 5 || CheckDiagonal(x, y) == 5)
		return true;
		//return p.stoneColor;

	return false;
}

int cMatchingRoom::CheckRow(int x, int y)		// 가로 검사
{
	int color = board[x][y];	// 색깔
	int victoryCnt = 5;			// 승리에 필요한 카운트
	int curCnt = 1;				// 현재 카운트

	// 왼쪽부터 검사 (놓인 돌 위치 다음돌부터)
	for (int i = 1; i < victoryCnt; i++)
	{
		// color가 같으면curCnt에 1더해줌
		if (board[x][y - i] == color)
			curCnt++;
		else if (board[x][y - i] != color || y - i < 0)
			break;
	}

	for (int i = 1; i < victoryCnt; ++i)
	{
		// color가 같으면curCnt에 1더해줌
		if (board[x][y + i] == color)
			curCnt++;
		else if (board[x][y - i] != color || y + i > GOMOKU_SIZE - 1)
			break;
	}

	return curCnt;
	//if (curCnt == 5)
	//	return color;
	//else
	//	return 0;
}

int cMatchingRoom::CheckColm(int x, int y)		// 세로 검사
{
	int color = board[x][y];	// 색깔
	int victoryCnt = 5;			// 승리에 필요한 카운트
	int curCnt = 1;				// 현재 카운트

	// 위쪽부터 검사 (놓인 돌 위치 다음돌부터)
	for (int i = 1; i < victoryCnt; i++)
	{
		// color가 같으면curCnt에 1더해줌
		if (board[x - i][y] == color)
			curCnt++;
		else if (board[x - i][y] != color || x - i < 0)
			break;
	}

	for (int i = 1; i < victoryCnt; ++i)
	{
		// color가 같으면curCnt에 1더해줌
		if (board[x + i][y] == color)
			curCnt++;
		else if (board[x + i][y] != color || x + i > GOMOKU_SIZE - 1)
			break;
	}

	return curCnt;
}

int cMatchingRoom::CheckDiagonal(int x, int y)	// 대각선 검사
{
	int color = board[x][y];		// 색깔
	int victoryCnt = 5;				// 승리에 필요한 카운트
	int curCnt = 1;					// 현재 카운트

	// 왼쪽 대각선 부터 검사 (놓인 돌 위치 다음돌부터)
	for (int i = 1; i < victoryCnt; i++)
	{
		// color가 같으면curCnt에 1더해줌
		if (board[x - i][y - i] == color)
			curCnt++;
		else if (board[x - i][y - i] != color || x - i < 0 || y - i < 0)
			break;
	}

	for (int i = 1; i < victoryCnt; ++i)
	{
		if (board[x + i][y + i] == color)
			curCnt++;
		else if (board[x + i][y + i] != color || x + i > GOMOKU_SIZE - 1 || y + i > GOMOKU_SIZE - 1)
			break;
	}

	return curCnt;
}