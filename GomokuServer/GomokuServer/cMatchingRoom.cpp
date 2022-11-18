#include "stdafx.h"
#include "cMatchingRoom.h"

cMatchingRoom::cMatchingRoom()
{
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
	PacketInfo packet;
	packet.curState = ClientState::eGame_Start;
	packet.stoneColor = BLACK;
	m_pBlackStone = black;
	send(m_pBlackStone->GetMySocket(), (char*)&packet, sizeof(PacketInfo), 0);

	packet.stoneColor = WHITE;
	m_pWhiteStone = white;
	send(m_pBlackStone->GetMySocket(), (char*)&packet, sizeof(PacketInfo), 0);

//	std::thread(RecvMsg, std::ref(m_pBlackStone->GetMySocket)).detach();	// 쓰레드로 클라이언트 연결 돌리기

}

void cMatchingRoom::GameStart()
{
}

int cMatchingRoom::CheckWin(PacketInfo p)
{
	std::cout << "승패 검사" << std::endl;
	int x = p.x;
	int y = p.y;
	board[x][y] = p.stoneColor;	// 서버 보드에 받은 좌표의 돌 컬러 넣기 (x = 세로 y = 가로)

	if (CheckRow(x, y) == 5 || CheckColm(x, y) == 5 || CheckDiagonal(x, y) == 5)
		return p.stoneColor;

	return 0;
}

int cMatchingRoom::CheckRow(int x, int y)
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

int cMatchingRoom::CheckColm(int x, int y)
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

int cMatchingRoom::CheckDiagonal(int x, int y)
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

//void cMatchingRoom::RecvMsg(SOCKET s)
//{
//	char buf[PACKET_SIZE] = { 0, };		// 메세지 받을 버퍼
//
//	// 반복문 돌기
//	while (true)
//	{
//		if (WSAGetLastError())									// 비정상 종료 되었다면
//		{
//			// 나갔을 때는 stoneColor로 체크
//			if (recvInfo.stoneColor == BLACK)
//				std::cout << "Black Exit" << std::endl;
//			else if (recvInfo.stoneColor == WHITE)
//				std::cout << "White Exit" << std::endl;
//
//			RemoveClient(s);									// 해당 클라이언트 벡터에서 삭제
//			return;
//		}
//		else
//		{
//			recv(s, buff, sizeof(PacketInfo), 0);					// 메세지 받기
//			memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// 메모리 카피 (recvInfo에 담아줌)
//			sendInfo = recvInfo;									// 보낼 메세지에 받은 메세지 넣어줌
//
//			if (CheckWin(recvInfo) != 0)									// 이겼는지 체크해서 이긴사람이 있다면
//			{
//				if (recvInfo.stoneColor == BLACK)
//				{
//					std::cout << "흑돌 승리" << std::endl;
//				}
//				else if (recvInfo.stoneColor == WHITE)
//				{
//					std::cout << "백돌 승리" << std::endl;
//				}
//				sendInfo.curState = eFINISH;						// FINISH로 상태 변경
//				for (int i = 0; i < vecSocket.size() - 1; ++i)				// 담긴 클라이언트 만큼 돌아서
//				{
//					send(vecSocket[i]->GetMySocket(), (char*)&sendInfo, sizeof(PacketInfo), 0);	// 다르다면 해당 클라이언트의 메세지를 다른 클라이언트에 띄워줌
//				}
//			}
//			else													// 아니라면
//			{
//				if (recvInfo.stoneColor == BLACK)					// 돌 색깔 변경
//				{
//					sendInfo.curState = eWHITE;
//					std::cout << "백돌 차례" << std::endl;
//				}
//				else if (recvInfo.stoneColor == WHITE)
//				{
//					sendInfo.curState = eBLACK;
//					std::cout << "흑돌 차례" << std::endl;
//				}
//				for (int i = 0; i < vecSocket.size() - 1; ++i)				// 담긴 클라이언트 만큼 돌아서
//				{
//					send(vecSocket[i]->GetMySocket(), (char*)&sendInfo, sizeof(PacketInfo), 0);	// 다르다면 해당 클라이언트의 메세지를 다른 클라이언트에 띄워줌
//				}
//			}
//
//
//		}
//
//	}
//}