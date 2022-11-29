#include "stdafx.h"
#include "cMatchingRoom.h"

cMatchingRoom::cMatchingRoom()
{
	m_pBlackStone = new cClient();
	m_pWhiteStone = new cClient();

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

void cMatchingRoom::ProcessMessageRoomClient(PacketInfo* p)	// �� ������Ʈ
{
	sendInfo = *p;

	if (CheckWin(sendInfo))						// ������ �̰�ٸ�
	{
		sendInfo.curState = eFinish;			// ���¸� ������ �ؼ� ����
	}
	else										// �ƴ϶��
	{
		if (sendInfo.curState == eBlack)		// ���� ���°� ���̶��
			sendInfo.curState = eWhite;			// ������ ����
		else if (sendInfo.curState == eWhite)	// ���̶��
			sendInfo.curState = eBlack;			// ������ ����
	}

	// �޼��� ������
	send(m_pBlackStone->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);
	send(m_pWhiteStone->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);
}

bool cMatchingRoom::CheckWin(PacketInfo p)	// ���� �˻�
{
	//std::cout << "���� �˻�" << std::endl;
	int x = p.x;
	int y = p.y;
	board[x][y] = p.stoneColor;	// ���� ���忡 ���� ��ǥ�� �� �÷� �ֱ� (x = ���� y = ����)

	if (CheckRow(x, y) == 5 || CheckColm(x, y) == 5 || CheckDiagonal(x, y) == 5)
		return true;
		//return p.stoneColor;

	return false;
}

int cMatchingRoom::CheckRow(int x, int y)		// ���� �˻�
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

int cMatchingRoom::CheckColm(int x, int y)		// ���� �˻�
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

int cMatchingRoom::CheckDiagonal(int x, int y)	// �밢�� �˻�
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