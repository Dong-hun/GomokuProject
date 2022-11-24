﻿// GomokuServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.

#include "stdafx.h"
#include "PacketInfo.h"
#include "cMatchingRoomManager.h"

//enum ClientState		// 턴 상태
//{
//	eNone = 0,		// 없음
//	eJoin_Room,		// 방 입장
//	eGame_Start,	// 게임 시작
//	eBlack,			// 흑돌 턴
//	eWhite,			// 백돌 턴
//	eFinish,		// 끝
//};
//
//struct PacketInfo				// 전달할 패킷
//{
//	SOCKET s;					// 어느 소켓에서 왔는지?
//	int x, y;					// 바둑판 좌표
//	int stoneColor;				// 돌 색깔
//	ClientState curState;		// 현재 상태
//
//	PacketInfo()
//	{
//		s			= { 0 };
//		x			= -1;
//		y			= -1;
//		stoneColor	= 0;
//		curState	= eNone;
//	}
//
//	void SetPacket(PacketInfo* p)
//	{
//		s			= { 0 };
//		x			= p->x;
//		y			= p->y;
//		stoneColor	= p->stoneColor;
//		curState	= p->curState;
//	}
//};

std::vector<cClient*> waitClientList;	// 생성된 클라이언트 담아줄 벡터
cMatchingRoomManager* matchingRoomMgr;	// 매칭룸 매니저
PacketInfo recvInfo;					// 받는 패킷

//typedef std::queue<std::pair<SOCKET, PacketInfo*>> MessageQueue;	// 메세지 큐
//MessageQueue messageQueue;

std::queue<PacketInfo*> _messageQueue;

SOCKET sock;

//int board[GOMOKU_SIZE][GOMOKU_SIZE] = { 0, };   // 판 그리기

void CreateServer();
void recvMsg(SOCKET s);
void acceptClient(SOCKET& s);
void ProcessMessage();

int main()
{
	CreateServer();
	//ProcessMessage();
	while (true);

	for (int i = 0; i < waitClientList.size(); i++)			// 소켓 갯수만큼 돌아서
	{
		closesocket(waitClientList[i]->sock);				// 닫아주기
		delete waitClientList[i];
	}

	closesocket(sock);									// 소켓 종료
	WSACleanup();
}

void ProcessMessage()
{
	while (true)
	{
		if (!_messageQueue.empty())
		{
			PacketInfo* p = _messageQueue.front();
			cMatchingRoom* room = matchingRoomMgr->GetMachingRoom(p->s);
			if (room != nullptr)
			{
				room->UpdateRoom(p);
				_messageQueue.pop();
			}
		}
		//else
		//	std::cout << "메세지 큐에 메세지 없음" << std::endl;
	}
}

void CreateServer()
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

	waitClientList.clear();
	matchingRoomMgr = new cMatchingRoomManager();
	_messageQueue = std::queue<PacketInfo*>();

	std::thread(acceptClient, std::ref(sock)).detach();	// 쓰레드로 클라이언트 연결 돌리기
	std::thread(ProcessMessage).detach();	// 쓰레드로 클라이언트 연결 돌리기
}

// 메세지 받기
void recvMsg(SOCKET s)
{
	char buff[sizeof(PacketInfo)] = { 0, };		// 메세지 받을 버퍼

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

			// 대기중이라면 벡터에서만 지워주고
			// 게임중이라면 방 파괴

			return;
		}
		else
		{
			recv(s, buff, sizeof(PacketInfo), 0);					// 메세지 받기
			memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// 메모리 카피 (recvInfo에 담아줌)
			//PacketInfo* p = new PacketInfo();
			//p = &recvInfo;
			PacketInfo* p = &recvInfo;
			p->s = s;
			_messageQueue.push(p);
		}

	}
}

// 클라이언트 연결
void acceptClient(SOCKET& s)
{
	int cnt = 0;
	while (true)
	{
		cClient* c = new cClient();																				// 클라이언트 생성
		c->sock = accept(s, (SOCKADDR*)&c->GetSocketAddress(), &c->servAddrSize);		// 생성한 클라이언트 소켓 연결하기 (연결 될때까지 블로킹)

		waitClientList.push_back(c);

		if (waitClientList.size() == 2)	// 대기 벡터에 두개가 차있다면
		{
			std::cout << "클라이언트 2명 생성. 방 생성" << std::endl;
			//cMatchingRoom* matchingRoom = new cMatchingRoom();							// 매칭룸 생성
			//matchingRoom->CreateMatchingRoom(waitClientList[0], waitClientList[1]);		// 매칭룸에 0,1번 클라이언트 넣어주기
			//matchingRoomMgr->PushMatchingRoom(matchingRoom);							
			matchingRoomMgr->CreateMatchingRoom(waitClientList[0], waitClientList[1]);	// 매칭룸 생성
			std::thread(recvMsg, std::ref(waitClientList[0]->sock)).detach();	// 쓰레드로 클라이언트 연결 돌리기
			std::thread(recvMsg, std::ref(waitClientList[1]->sock)).detach();	// 쓰레드로 클라이언트 연결 돌리기
			waitClientList.clear();														// 대기 벡터 클리어
		}
	}
}