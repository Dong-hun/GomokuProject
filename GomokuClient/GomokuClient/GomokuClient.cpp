// GomokuClient.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <thread>
#include <atomic>
#include <string>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PACKET_SIZE 1024
#define	PORT 4578
#define SERVER_IP "172.30.85.182"

#define UP 72
#define DOWN 80
#define RIGHT 77
#define LEFT 75
#define SPACE 32

#define DRAW_BLACK printf("○");
#define DRAW_WHITE printf("●")

#define GOMOKU_SIZE 15
#define BLACK 1
#define WHITE 2

using namespace std;

enum TurnState		// 턴 상태
{
	eNONE,    	    // 없음
	eBLACK,   	    // 흑돌 턴
	eWHITE,   	    // 백돌 턴
	eFINISH,  	    // 끝
};

// 패킷 전달
struct PacketInfo
{
	int x, y;           // 바둑판 좌표
	int stoneColor;     // 돌 색깔
	TurnState curState; // 현재 상태

	PacketInfo()
	{
		x = -1;
		y = -1;
		stoneColor = 0;
		curState = eNONE;
	}
};

PacketInfo sendInfo;            // 보내려는 패킷
PacketInfo recvInfo;            // 받는 패킷
atomic<bool>broken(false);		// 서버가 닫히면 클라이언트도 그걸감지하고 자동종료하기위한 변수

int board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // 판 그리기
int myColor = 0;                                // 내 색깔
int posX;                                       // 현재 x좌표
int posY;                                       // 현재 y좌표
TurnState state;

void JoinServer();
void sendMsg(SOCKET& s);
void recvMsg(SOCKET& s);

void InputKey(SOCKET& s);
void gotoxy(int x, int y);
void gotoTurnInput(TurnState turn);
void InitDrawBoard();
void DrawStone(PacketInfo info);
void DrawInfo();
void ChangState(PacketInfo pRecvMsg);
void StateProcess(SOCKET s);
void DrawResult(PacketInfo p);

int main()
{
	//const char ProgMutex[] = "RUNNING";
	//HANDLE Mutex = ::CreateMutex(FALSE, 0, (LPCWSTR)ProgMutex);
	//if (::GetLastError() == ERROR_ALREADY_EXISTS)
	//{
	//	cout << "Error! Task already started!" << endl;;
	//	exit(0);
	//}
	//else
	//{
	//	JoinServer();
	//	while (true);
	//}

	//::ReleaseMutex(Mutex);

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
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
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

	char buff[sizeof(PacketInfo)] = { 0 };                  // 메세지 받을 버퍼
	recv(sock, buff, sizeof(PacketInfo), 0);                // 메세지 받아서 버퍼에 넣기
	memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// 메모리 카피 (recvInfo에 담아줌)
	myColor = recvInfo.stoneColor;                          // 내 색깔 지정

	DrawInfo();                                             // 내 정보 그려주기

	posX = posY = GOMOKU_SIZE / 2;
	gotoxy(posX, posY);

	thread(recvMsg, ref(sock)).detach();

	while (true)
	{
		StateProcess(sock);
	}
}

void recvMsg(SOCKET& s)         // 메세지 받기
{
	char buff[sizeof(PacketInfo)] = { 0 };                  // 메세지 받을 버퍼
	while (true)
	{
		recv(s, buff, sizeof(PacketInfo), 0);               // 메세지 받기
		memcpy((char*)&recvInfo, buff, sizeof(PacketInfo)); // 메모리 카피
		if (WSAGetLastError())	                            // 서버 종료 감지
		{
			broken.store(true, memory_order_release);       // 종료
			return;
		}
		ChangState(recvInfo);                               // 상태 변경
	}
}

// 키 입력 받기
void InputKey(SOCKET& s)
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
			char buf[PACKET_SIZE] = { 0, };
			sendInfo.stoneColor = myColor;
			sendInfo.x = posX;
			sendInfo.y = posY;
			sendInfo.curState = state;

			send(s, (char*)&sendInfo, sizeof(PacketInfo), 0);	//송신
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

// 턴 표시
void gotoTurnInput(TurnState turn)
{
	gotoxy(1, GOMOKU_SIZE + 2);

	switch (turn)
	{
	case eNONE:
		cout << "대기중" << endl;
		break;
	case eBLACK:
		cout << "흑돌 차례" << endl;
		break;
	case eWHITE:
		cout << "백돌 차례" << endl;
		break;
	case eFINISH:
		break;
	default:
		break;
	}

	gotoxy(posX, posY);
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

	gotoxy(1, GOMOKU_SIZE + 2);
	cout << "대기중" << endl;
}

void ChangState(PacketInfo pRecvMsg)        // 상태 변경
{
	if (state == pRecvMsg.curState)         // 변경하려는 상태가 기존 상태랑 같다면
		return;                             // 리턴

	state = pRecvMsg.curState;              // 바꾸려는 상태 적용
	DrawStone(pRecvMsg);                    // 서버에서 받아온 정보로 돌 그리기

	switch (state)
	{
	case eNONE:
		break;
	case eBLACK:
		gotoTurnInput(eBLACK);              // 흑돌 턴이라고 써주기
		break;
	case eWHITE:
		gotoTurnInput(eWHITE);              // 백돌 턴이라고 써주기
		break;
	case eFINISH:
		DrawResult(pRecvMsg);               // 결과 출력하기
		break;
	default:
		break;
	}
}

void StateProcess(SOCKET s)
{
	switch (state)
	{
	case eNONE:
		break;
	case eBLACK:
		if (myColor == BLACK)
		{
			InputKey(s);
		}
		break;
	case eWHITE:
		if (myColor == WHITE)
		{
			InputKey(s);
		}
		break;
	case eFINISH:
		break;
	default:
		break;
	}
}