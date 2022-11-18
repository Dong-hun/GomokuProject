#include "stdafx.h"
#include "cMatchingRoom.h"

cMatchingRoom::cMatchingRoom()
{
	board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // �� �׸���
}

cMatchingRoom::~cMatchingRoom()
{
	board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // �� �׸���

	if (m_pWhiteStone != nullptr)
		delete m_pWhiteStone;
	if (m_pBlackStone != nullptr)
		delete m_pBlackStone;
}

// �� ����
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

//	std::thread(RecvMsg, std::ref(m_pBlackStone->GetMySocket)).detach();	// ������� Ŭ���̾�Ʈ ���� ������

}

void cMatchingRoom::GameStart()
{
}

int cMatchingRoom::CheckWin(PacketInfo p)
{
	std::cout << "���� �˻�" << std::endl;
	int x = p.x;
	int y = p.y;
	board[x][y] = p.stoneColor;	// ���� ���忡 ���� ��ǥ�� �� �÷� �ֱ� (x = ���� y = ����)

	if (CheckRow(x, y) == 5 || CheckColm(x, y) == 5 || CheckDiagonal(x, y) == 5)
		return p.stoneColor;

	return 0;
}

int cMatchingRoom::CheckRow(int x, int y)
{
	int color = board[x][y];	// ����
	int victoryCnt = 5;			// �¸��� �ʿ��� ī��Ʈ
	int curCnt = 1;				// ���� ī��Ʈ

	// ���ʺ��� �˻� (���� �� ��ġ ����������)
	for (int i = 1; i < victoryCnt; i++)
	{
		// color�� ������curCnt�� 1������
		if (board[x][y - i] == color)
			curCnt++;
		else if (board[x][y - i] != color || y - i < 0)
			break;
	}

	for (int i = 1; i < victoryCnt; ++i)
	{
		// color�� ������curCnt�� 1������
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
	int color = board[x][y];	// ����
	int victoryCnt = 5;			// �¸��� �ʿ��� ī��Ʈ
	int curCnt = 1;				// ���� ī��Ʈ

	// ���ʺ��� �˻� (���� �� ��ġ ����������)
	for (int i = 1; i < victoryCnt; i++)
	{
		// color�� ������curCnt�� 1������
		if (board[x - i][y] == color)
			curCnt++;
		else if (board[x - i][y] != color || x - i < 0)
			break;
	}

	for (int i = 1; i < victoryCnt; ++i)
	{
		// color�� ������curCnt�� 1������
		if (board[x + i][y] == color)
			curCnt++;
		else if (board[x + i][y] != color || x + i > GOMOKU_SIZE - 1)
			break;
	}

	return curCnt;
}

int cMatchingRoom::CheckDiagonal(int x, int y)
{
	int color = board[x][y];		// ����
	int victoryCnt = 5;				// �¸��� �ʿ��� ī��Ʈ
	int curCnt = 1;					// ���� ī��Ʈ

	// ���� �밢�� ���� �˻� (���� �� ��ġ ����������)
	for (int i = 1; i < victoryCnt; i++)
	{
		// color�� ������curCnt�� 1������
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
//	char buf[PACKET_SIZE] = { 0, };		// �޼��� ���� ����
//
//	// �ݺ��� ����
//	while (true)
//	{
//		if (WSAGetLastError())									// ������ ���� �Ǿ��ٸ�
//		{
//			// ������ ���� stoneColor�� üũ
//			if (recvInfo.stoneColor == BLACK)
//				std::cout << "Black Exit" << std::endl;
//			else if (recvInfo.stoneColor == WHITE)
//				std::cout << "White Exit" << std::endl;
//
//			RemoveClient(s);									// �ش� Ŭ���̾�Ʈ ���Ϳ��� ����
//			return;
//		}
//		else
//		{
//			recv(s, buff, sizeof(PacketInfo), 0);					// �޼��� �ޱ�
//			memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// �޸� ī�� (recvInfo�� �����)
//			sendInfo = recvInfo;									// ���� �޼����� ���� �޼��� �־���
//
//			if (CheckWin(recvInfo) != 0)									// �̰���� üũ�ؼ� �̱����� �ִٸ�
//			{
//				if (recvInfo.stoneColor == BLACK)
//				{
//					std::cout << "�浹 �¸�" << std::endl;
//				}
//				else if (recvInfo.stoneColor == WHITE)
//				{
//					std::cout << "�鵹 �¸�" << std::endl;
//				}
//				sendInfo.curState = eFINISH;						// FINISH�� ���� ����
//				for (int i = 0; i < vecSocket.size() - 1; ++i)				// ��� Ŭ���̾�Ʈ ��ŭ ���Ƽ�
//				{
//					send(vecSocket[i]->GetMySocket(), (char*)&sendInfo, sizeof(PacketInfo), 0);	// �ٸ��ٸ� �ش� Ŭ���̾�Ʈ�� �޼����� �ٸ� Ŭ���̾�Ʈ�� �����
//				}
//			}
//			else													// �ƴ϶��
//			{
//				if (recvInfo.stoneColor == BLACK)					// �� ���� ����
//				{
//					sendInfo.curState = eWHITE;
//					std::cout << "�鵹 ����" << std::endl;
//				}
//				else if (recvInfo.stoneColor == WHITE)
//				{
//					sendInfo.curState = eBLACK;
//					std::cout << "�浹 ����" << std::endl;
//				}
//				for (int i = 0; i < vecSocket.size() - 1; ++i)				// ��� Ŭ���̾�Ʈ ��ŭ ���Ƽ�
//				{
//					send(vecSocket[i]->GetMySocket(), (char*)&sendInfo, sizeof(PacketInfo), 0);	// �ٸ��ٸ� �ش� Ŭ���̾�Ʈ�� �޼����� �ٸ� Ŭ���̾�Ʈ�� �����
//				}
//			}
//
//
//		}
//
//	}
//}