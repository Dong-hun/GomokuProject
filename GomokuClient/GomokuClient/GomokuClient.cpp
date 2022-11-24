// GomokuClient.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "stdafx.h"

using namespace std;

enum ClientState		// 턴 상태
{
	eNone = 0,		// 없음
	eJoin_Room,		// 방 입장
	eGame_Start,	// 게임 시작
	eBlack,			// 흑돌 턴
	eWhite,			// 백돌 턴
	eFinish,		// 끝
};

// 패킷 전달
struct PacketInfo
{
	//SOCKET		s;				// 어느 소켓에서 왔는지?
	int			x, y;			// 바둑판 좌표
	int			stoneColor;		// 돌 색깔
	ClientState curState;		// 현재 상태

	PacketInfo()
	{
		//s = { 0 };
		x = -1;
		y = -1;
		stoneColor = 0;
		curState = eNone;
	}

	void SetPacket(PacketInfo p)
	{
		//s = { 0 };
		x = p.x;
		y = p.y;
		stoneColor = p.stoneColor;
		curState = p.curState;
	}
};

PacketInfo sendInfo;            // 보내려는 패킷
PacketInfo recvInfo;            // 받는 패킷
atomic<bool>broken(false);		// 서버가 닫히면 클라이언트도 그걸감지하고 자동종료하기위한 변수

int board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // 판 그리기
int myColor = 0;                                // 내 색깔
int posX;                                       // 현재 x좌표
int posY;                                       // 현재 y좌표
ClientState state;
SOCKET sock;

void JoinServer();
//void sendMsg(SOCKET& s);
void recvMsg();

void InputKey();
void gotoxy(int x, int y);
//void gotoTurnInput(ClientState turn);
void InitDrawBoard();
void DrawStone(PacketInfo info);
void DrawInfo();
void ChangState(PacketInfo pRecvMsg);
void StateProcess();
void DrawResult(PacketInfo p);
void ShowRecvMsg(PacketInfo p);
void ChangeState(ClientState s);        // 상태 변경
string GetStateToString(ClientState s);

int main()
{
	JoinServer();
	while (true);
	return 0;
}

void JoinServer()       // 서버 접속
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed WASStartup() \n");
		return;
	}
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		WSACleanup();
		printf("Failed Socket() \n");
		return;
	}
	SOCKADDR_IN servAddr = { 0 };

	memset(&servAddr, 0, sizeof(servAddr)); // 초기화
	servAddr.sin_family = AF_INET;			// 서버 주소체계 정하기 AF_INET = (x,x,x,x)
	inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr);
	//inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
	//servAddr.sin_port = htons(30002);
	servAddr.sin_port = htons(PORT);

	//while (connect(sock, (SOCKADDR*)&servAddr, sizeof servAddr));	// 접속 하기
	connect(sock, (SOCKADDR*)&servAddr, sizeof servAddr);   // 접속
	InitDrawBoard();                                        // 보드 그리기
	state = eNone;
	ChangeState(eJoin_Room);

	//sendInfo.s = sock;
	thread(recvMsg).detach();

	while (true)
	{
		StateProcess();
	}
}

void recvMsg()         // 메세지 받기
{
	char buff[sizeof(PacketInfo)] = { 0 };                  // 메세지 받을 버퍼
	while (true)
	{
		recv(sock, buff, sizeof(PacketInfo), 0);               // 메세지 받기
		memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// 메모리 카피
		//ShowRecvMsg(recvInfo);
		if (WSAGetLastError())									// 서버 종료 감지
		{
			broken.store(true, memory_order_release);			// 종료
			return;
		}
		ChangState(recvInfo);									// 상태 변경
	}
}

void ShowRecvMsg(PacketInfo p)
{
	gotoxy(GOMOKU_SIZE + 4, 2);
	std::cout << "내 소켓 : " << sock << std::endl;
	gotoxy(GOMOKU_SIZE + 4, 3);
	//std::cout << "상태 : " << p.curState << std::endl;
	std::cout << "상태 : " << GetStateToString(p.curState) << std::endl;
	gotoxy(GOMOKU_SIZE + 4, 4);
	std::cout << "x좌표 : " << p.x << std::endl;
	gotoxy(GOMOKU_SIZE + 4, 5);
	std::cout << "y좌표: " << p.y << std::endl;
	gotoxy(GOMOKU_SIZE + 4, 6);
	std::cout << "돌 색깔 : " << p.stoneColor<< std::endl;
}

// 키 입력 받기
void InputKey()
{
	int input = _getch();

	if (input == UP)
	{
		if (posY > 0)
			posY--;

		gotoxy(posX, posY);
	}
	else if (input == DOWN)
	{
		if (posY < GOMOKU_SIZE - 1)
			posY++;

		gotoxy(posX, posY);
	}
	else if (input == LEFT)
	{
		if (posX > 0)
			posX--;

		gotoxy(posX, posY);
	}
	else if (input == RIGHT)
	{
		if (posX < GOMOKU_SIZE - 1)
			posX++;

		gotoxy(posX, posY);
	}
	else if (input == SPACE)
	{
		if (board[posY][posX] == 0)
		{
			//sendInfo.s = sock;
			sendInfo.x = posX;
			sendInfo.y = posY;
			sendInfo.stoneColor = myColor;
			sendInfo.curState = state;

			send(sock, (char*)&sendInfo, sizeof(PacketInfo), 0);	//송신
		}
	}
}

// 돌 그리기
void DrawStone(PacketInfo info)
{
	if (info.x < 0 || info.y < 0)
		return;

	int x = info.x;
	int y = info.y;
	int color = info.stoneColor;

	gotoxy(x, y);

	if (color == BLACK)
	{
		DRAW_BLACK;
		board[y][x] = BLACK;
	}
	else if (color == WHITE)
	{
		DRAW_WHITE;
		board[y][x] = WHITE;
	}
}

// 좌표 이동
void gotoxy(int x, int y)
{
	COORD pos = { x * 2, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 결과 표시
void DrawResult(PacketInfo p)
{
	gotoxy(1, GOMOKU_SIZE + 2);

	switch (p.stoneColor)
	{
	case BLACK:
		cout << "흑돌 승리" << endl;
		break;
	case WHITE:
		cout << "백돌 승리" << endl;
		break;
	default:
		break;
	}
}

// 초기 보드판 그리기
void InitDrawBoard()
{
	gotoxy(0, 0);

	for (int i = 0; i < GOMOKU_SIZE; i++)           // 세로
	{
		for (int j = 0; j < GOMOKU_SIZE; j++)       // 가로
		{
			gotoxy(j, i);
			if (i == 0) {
				if (j == 0)
					printf("┌");
				else if (j == GOMOKU_SIZE - 1)
					printf("┐");
				else
					printf("┬");
			}
			else if (j == 0) {
				if (i == GOMOKU_SIZE - 1)
					printf("└");
				else
					printf("├");
			}
			else if (j == GOMOKU_SIZE - 1) {
				if (i == GOMOKU_SIZE - 1)
					printf("┘");
				else
					printf("┤");
			}
			else if (i == GOMOKU_SIZE - 1) {
				printf("┴");
			}
			else
				printf("┼");
		}
		cout << endl;
	}

	for (int i = 0; i < GOMOKU_SIZE; i++)
	{
		gotoxy(GOMOKU_SIZE, i);
		printf("%d", i + 1);
		gotoxy(i, GOMOKU_SIZE);
		printf("%c", i + 'A');
	}
}


// 내 색깔 뭔지 적어주기
void DrawInfo()
{
	gotoxy(1, GOMOKU_SIZE + 4);

	switch (myColor)
	{
	case BLACK:
		cout << "내 돌 : 흑돌" << endl;
		break;
	case WHITE:
		cout << "내 돌 : 백돌" << endl;
		break;
	default:
		break;
	}
}

void ChangState(PacketInfo pRecvMsg)        // 상태 변경
{
	if (state == pRecvMsg.curState)         // 변경하려는 상태가 기존 상태랑 같다면
		return;                             // 리턴

	state = pRecvMsg.curState;              // 바꾸려는 상태 적용
	DrawStone(pRecvMsg);                    // 서버에서 받아온 정보로 돌 그리기

	switch (state)
	{
	case eNone:
		break;
	case eJoin_Room:
		gotoxy(1, GOMOKU_SIZE + 4);
		cout << "매칭 대기중" << endl;
		break;
	case eGame_Start:
		myColor = pRecvMsg.stoneColor;
		DrawInfo();
		//send(sock, (char*)&sendInfo, PACKET_SIZE, 0);
		break;
	case eBlack:
		gotoxy(1, GOMOKU_SIZE + 2);
		cout << "흑돌 차례" << endl;
		gotoxy(GOMOKU_SIZE / 2, GOMOKU_SIZE / 2);
		break;
	case eWhite:
		gotoxy(1, GOMOKU_SIZE + 2);
		cout << "백돌 차례" << endl;
		gotoxy(GOMOKU_SIZE / 2, GOMOKU_SIZE / 2);
		break;
	case eFinish:
		DrawResult(pRecvMsg);               // 결과 출력하기
		break;
	}
}

void ChangeState(ClientState s)        // 상태 변경
{
	if (state == s)		// 변경하려는 상태가 기존 상태랑 같다면
		return;         // 리턴

	state = s;          // 바꾸려는 상태 적용
	
	switch (state)
	{
	case eJoin_Room:
		gotoxy(1, GOMOKU_SIZE + 4);
		cout << "매칭 대기중" << endl;
		break;
	}
}

string GetStateToString(ClientState s)
{
	string str = "";
	switch (state)
	{
	case eNone:
		str = "내 상태 : None";
		break;
	case eJoin_Room:
		str = "내 상태 : Join Room";
		break;
	case eBlack:
		str = "내 상태 : Black";
		break;
	case eWhite:
		str = "내 상태 : White";
		break;
	case eFinish:
		str = "내 상태 : Finish";
		break;
	}

	return str;
}

void StateProcess()
{
	switch (state)
	{
	case eJoin_Room:
		break;
	case eGame_Start:
		break;
	case eBlack:
		if (myColor == BLACK)
		{
			InputKey();
		}
		break;
	case eWhite:
		if (myColor == WHITE)
		{
			InputKey();
		}
		break;
	case eFinish:
		break;
	}
}