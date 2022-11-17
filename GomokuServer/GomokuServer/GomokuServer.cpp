// GomokuServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>
#include <string>
#include <vector>
#include <utility>	//pair
#include <thread>
#include <queue>
#include <string>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PACKET_SIZE 1024
#define	PORT 4578
#define SERVER_IP "172.30.85.182"

#define GOMOKU_SIZE 15
#define BLACK 1
#define WHITE 2

//using namespace std;


class CLIENT		// 클라이언트 캡슐화
{
public:
	SOCKET sock;
	SOCKET battlePlayer;
	SOCKADDR_IN servAddr = { 0 };
	int servAddrSize = sizeof(servAddr);
	//int color;
	//int number;
	//char name[PACKET_SIZE] = { 0, };

	CLIENT()
	{
		sock = { 0 };
		battlePlayer = { 0 };
		servAddr = { 0 };
		servAddrSize = sizeof(servAddr);
	}
	~CLIENT()
	{
		sock = { 0 };
		battlePlayer = { 0 };
		servAddr = { 0 };
		servAddrSize = sizeof(servAddr);
		//color = -1;
	}
};

enum TurnState		// 턴 상태
{
	eNONE = 0,		// 없음
	eBLACK,			// 흑돌 턴
	eWHITE,			// 백돌 턴
	eFINISH,		// 끝
};

struct PacketInfo			// 전달할 패킷
{
	int x, y;				// 바둑판 좌표
	int stoneColor;			// 돌 색깔
	TurnState curState;		// 현재 상태

	PacketInfo()
	{
		x = -1;
		y = -1;
		stoneColor = 0;
		curState = eNONE;
	}
};

PacketInfo sendInfo;			// 보내려는 패킷
PacketInfo recvInfo;			// 받는 패킷
std::vector<CLIENT*> vecSocket;		// 생성된 클라이언트 담아줄 벡터
int cnt = 0;					// 클라이언트 번호

int board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // 판 그리기

void CreateServer();
void recvMsg(SOCKET s);
void acceptClient(SOCKET& s);
void RemoveClient(SOCKET s);
int CheckWin(PacketInfo p);
int CheckRow(int x, int y);
int CheckColm(int x, int y);
int CheckDiagonal(int x, int y);
void CharToWChar(const char* pstrSrc, wchar_t pwstrDest[]);

int main()
{
	//HANDLE Mutex;
	//const char ProgMutex[] = "Sysdocu";
	//wchar_t lps[] = { 0, };
	//CharToWChar(ProgMutex, lps);

	//if ((Mutex = OpenMutex(MUTEX_ALL_ACCESS, false, lps)) == NULL)
	//{
	//	Mutex = CreateMutex(NULL, true, lps);
	//}
	//else
	//{
	//	return 0;
	//}


	CreateServer();
}

void CharToWChar(const char* pstrSrc, wchar_t pwstrDest[])
{
	int nLen = (int)strlen(pstrSrc) + 1;
	size_t s;
	mbstowcs_s(&s, pwstrDest, nLen, pstrSrc, nLen);
}

void CreateServer()
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

	// 서버 연결
	// 연결 서버 주소 세팅
	memset(&servAddr, 0, sizeof(servAddr)); // 초기화
	servAddr.sin_family = AF_INET;			// 서버 주소체계 정하기 AF_INET = (x,x,x,x)
	inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr);
	//inet_pton(AF_INET, INADDR_ANY, &servAddr.sin_addr);
	//servAddr.sin_port = htons(30002);
	servAddr.sin_port = htons(PORT);

	if (bind(sock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("Binding Error \n");
		closesocket(sock);
		WSACleanup();
		return;
	}

	if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("listen Error \n");
		closesocket(sock);
		WSACleanup();
		return;
	}

	printf("Server Start \n");
	vecSocket.clear();

	std::thread(acceptClient, std::ref(sock)).detach();	// 쓰레드로 클라이언트 연결 돌리기

	while (true);										// 프로그램 꺼짐 방지

	for (int i = 0; i < vecSocket.size(); i++)			// 소켓 갯수만큼 돌아서
	{
		closesocket(vecSocket[i]->sock);				// 닫아주기
		delete vecSocket[i];
	}

	closesocket(sock);									// 소켓 종료
	WSACleanup();
}

// 메세지 받기
void recvMsg(SOCKET s)
{
	char buff[sizeof(PacketInfo)] = { 0, };		// 메세지 받을 버퍼

	if (sendInfo.stoneColor == BLACK)
		std::cout << "Black 입장" << std::endl;
	else if (sendInfo.stoneColor == WHITE)
		std::cout << "White 입장" << std::endl;

	while (true)
	{
		if (vecSocket.size() > 2)
		{
			sendInfo.curState = eBLACK;
			sendInfo.stoneColor = eBLACK;
			sendInfo.x = sendInfo.y = -1;
			for (int i = 0; i < vecSocket.size() - 1; i++)
				send(vecSocket[i]->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);	// 다르다면 해당 클라이언트의 메세지를 다른 클라이언트에 띄워줌
			break;
		}
	}

	// 반복문 돌기
	while (true)
	{
		if (WSAGetLastError())									// 비정상 종료 되었다면
		{
			// 나갔을 때는 stoneColor로 체크
			if (recvInfo.stoneColor == BLACK)
				std::cout << "Black Exit" << std::endl;
			else if (recvInfo.stoneColor == WHITE)
				std::cout << "White Exit" << std::endl;

			RemoveClient(s);									// 해당 클라이언트 벡터에서 삭제
			return;
		}
		else
		{
			recv(s, buff, sizeof(PacketInfo), 0);					// 메세지 받기
			memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// 메모리 카피 (recvInfo에 담아줌)
			sendInfo = recvInfo;									// 보낼 메세지에 받은 메세지 넣어줌

			if (CheckWin(recvInfo) != 0)									// 이겼는지 체크해서 이긴사람이 있다면
			{
				if (recvInfo.stoneColor == BLACK)
				{
					std::cout << "흑돌 승리" << std::endl;
				}
				else if (recvInfo.stoneColor == WHITE)
				{
					std::cout << "백돌 승리" << std::endl;
				}
				sendInfo.curState = eFINISH;						// FINISH로 상태 변경
				for (int i = 0; i < vecSocket.size() - 1; ++i)				// 담긴 클라이언트 만큼 돌아서
				{
					send(vecSocket[i]->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);	// 다르다면 해당 클라이언트의 메세지를 다른 클라이언트에 띄워줌
				}
			}
			else													// 아니라면
			{
				if (recvInfo.stoneColor == BLACK)					// 돌 색깔 변경
				{
					sendInfo.curState = eWHITE;
					std::cout << "백돌 차례" << std::endl;
				}
				else if (recvInfo.stoneColor == WHITE)
				{
					sendInfo.curState = eBLACK;
					std::cout << "흑돌 차례" << std::endl;
				}
				for (int i = 0; i < vecSocket.size() - 1; ++i)				// 담긴 클라이언트 만큼 돌아서
				{
					send(vecSocket[i]->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);	// 다르다면 해당 클라이언트의 메세지를 다른 클라이언트에 띄워줌
				}
			}


		}

	}
}

// 클라이언트 연결
void acceptClient(SOCKET& s)
{
	char id[PACKET_SIZE] = { 0 };	// 번호 보내기
	char buff[sizeof(PacketInfo)] = { 0 };	// 이름 받기

	while (true)
	{
		CLIENT* c = new CLIENT();																				// 클라이언트 생성
		vecSocket.push_back(c);																					// 벡터에 넣기
		vecSocket[cnt]->sock = accept(s, (SOCKADDR*)&vecSocket[cnt]->servAddr, &vecSocket[cnt]->servAddrSize);	// 생성한 클라이언트 소켓 연결하기 (연결 될때까지 블로킹)
		sendInfo.stoneColor = (cnt % 2) + 1;
		sendInfo.curState = eNONE;
		send(vecSocket[cnt]->sock, (char*)&sendInfo, sizeof(PacketInfo), 0);									// 색깔 보내기
		std::thread(recvMsg, std::ref(vecSocket[cnt]->sock)).detach();													// 쓰레드로 돌리기
		cnt++;																									// 번호 증가
	}
}

void RemoveClient(SOCKET s)	// 클라이언트 제거
{
	for (int i = 0; i < vecSocket.size(); i++)
	{
		if (vecSocket[i]->sock == s)
			vecSocket.erase(vecSocket.begin() + i);
	}
	--cnt;
}

int CheckWin(PacketInfo p)	// 이겼는지 검사
{
	std::cout << "승패 검사" << std::endl;
	int x = p.x;
	int y = p.y;
	board[x][y] = p.stoneColor;	// 서버 보드에 받은 좌표의 돌 컬러 넣기 (x = 세로 y = 가로)

	if (CheckRow(x, y) == 5 || CheckColm(x, y) == 5 || CheckDiagonal(x, y) == 5)
		return p.stoneColor;

	return 0;
}

int CheckRow(int x, int y)
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

int CheckColm(int x, int y)		// 세로 검사
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

int CheckDiagonal(int x, int y)		// 대각선 검사
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