// GomokuServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "stdafx.h"
#include "cGameServer.h"
#include "cClient.h"
#include "cMatchingRoomManager.h"

//std::vector<cClient*> vecSocket;		// 생성된 클라이언트 담아줄 벡터


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
	cGameServer* GameServer = new cGameServer();
	GameServer->CreateServer();
	std::thread(GameServer->AcceptClient).detach();	// 쓰레드로 클라이언트 연결 돌리기
	std::thread(GameServer->recvMsg).detach();

	while (true)
	{
		//GameServer->MessageProcess();
	}
	delete GameServer;
}

//void CharToWChar(const char* pstrSrc, wchar_t pwstrDest[])
//{
//	int nLen = (int)strlen(pstrSrc) + 1;
//	size_t s;
//	mbstowcs_s(&s, pwstrDest, nLen, pstrSrc, nLen);
//}

//void CreateServer()
//{
//	WSADATA wsaData;
//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//	{
//		printf("Failed WASStartup() \n");
//		return;
//	}
//
//	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (sock == INVALID_SOCKET)
//	{
//		WSACleanup();
//		printf("Failed Socket() \n");
//		return;
//	}
//	SOCKADDR_IN servAddr = { 0 };
//
//	// 서버 연결
//	// 연결 서버 주소 세팅
//	memset(&servAddr, 0, sizeof(servAddr)); // 초기화
//	servAddr.sin_family = AF_INET;			// 서버 주소체계 정하기 AF_INET = (x,x,x,x)
//	inet_pton(AF_INET, SERVER_IP, &servAddr.sin_addr);
//	//inet_pton(AF_INET, INADDR_ANY, &servAddr.sin_addr);
//	//servAddr.sin_port = htons(30002);
//	servAddr.sin_port = htons(PORT);
//
//	if (bind(sock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
//	{
//		printf("Binding Error \n");
//		closesocket(sock);
//		WSACleanup();
//		return;
//	}
//
//	if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
//	{
//		printf("listen Error \n");
//		closesocket(sock);
//		WSACleanup();
//		return;
//	}
//
//	printf("Server Start \n");
//
//	std::thread(acceptClient, std::ref(sock)).detach();	// 쓰레드로 클라이언트 연결 돌리기
//
//	while (true);										// 프로그램 꺼짐 방지
//
//	for (int i = 0; i < vecWaitClient.size(); i++)		// 소켓 갯수만큼 돌아서
//		closesocket(vecWaitClient[i]->GetMySocket());	// 닫아주기
//
//	for (int i = 0; i < vecMatchingRoom.size(); i++)
//		delete vecMatchingRoom[i];
//
//	closesocket(sock);									// 소켓 종료
//	WSACleanup();
//}

// 메세지 받기
void recvMsg(SOCKET s)
{
	//char buff[sizeof(PacketInfo)] = { 0, };		// 메세지 받을 버퍼

	//if (sendInfo.stoneColor == BLACK)
	//	std::cout << "Black 입장" << std::endl;
	//else if (sendInfo.stoneColor == WHITE)
	//	std::cout << "White 입장" << std::endl;

	//while (true)
	//{
	//	if (vecSocket.size() > 2)
	//	{
	//		sendInfo.curState = eBLACK;
	//		sendInfo.stoneColor = eBLACK;
	//		sendInfo.x = sendInfo.y = -1;
	//		for (int i = 0; i < vecSocket.size() - 1; i++)
	//			send(vecSocket[i]->GetMySocket(), (char*)&sendInfo, sizeof(PacketInfo), 0);	// 다르다면 해당 클라이언트의 메세지를 다른 클라이언트에 띄워줌
	//		break;
	//	}
	//}

	//// 반복문 돌기
	//while (true)
	//{
	//	if (WSAGetLastError())									// 비정상 종료 되었다면
	//	{
	//		// 나갔을 때는 stoneColor로 체크
	//		if (recvInfo.stoneColor == BLACK)
	//			std::cout << "Black Exit" << std::endl;
	//		else if (recvInfo.stoneColor == WHITE)
	//			std::cout << "White Exit" << std::endl;

	//		RemoveClient(s);									// 해당 클라이언트 벡터에서 삭제
	//		return;
	//	}
	//	else
	//	{
	//		recv(s, buff, sizeof(PacketInfo), 0);					// 메세지 받기
	//		memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// 메모리 카피 (recvInfo에 담아줌)
	//		sendInfo = recvInfo;									// 보낼 메세지에 받은 메세지 넣어줌

	//		if (CheckWin(recvInfo) != 0)									// 이겼는지 체크해서 이긴사람이 있다면
	//		{
	//			if (recvInfo.stoneColor == BLACK)
	//			{
	//				std::cout << "흑돌 승리" << std::endl;
	//			}
	//			else if (recvInfo.stoneColor == WHITE)
	//			{
	//				std::cout << "백돌 승리" << std::endl;
	//			}
	//			sendInfo.curState = eFINISH;						// FINISH로 상태 변경
	//			for (int i = 0; i < vecSocket.size() - 1; ++i)				// 담긴 클라이언트 만큼 돌아서
	//			{
	//				send(vecSocket[i]->GetMySocket(), (char*)&sendInfo, sizeof(PacketInfo), 0);	// 다르다면 해당 클라이언트의 메세지를 다른 클라이언트에 띄워줌
	//			}
	//		}
	//		else													// 아니라면
	//		{
	//			if (recvInfo.stoneColor == BLACK)					// 돌 색깔 변경
	//			{
	//				sendInfo.curState = eWHITE;
	//				std::cout << "백돌 차례" << std::endl;
	//			}
	//			else if (recvInfo.stoneColor == WHITE)
	//			{
	//				sendInfo.curState = eBLACK;
	//				std::cout << "흑돌 차례" << std::endl;
	//			}
	//			for (int i = 0; i < vecSocket.size() - 1; ++i)				// 담긴 클라이언트 만큼 돌아서
	//			{
	//				send(vecSocket[i]->GetMySocket(), (char*)&sendInfo, sizeof(PacketInfo), 0);	// 다르다면 해당 클라이언트의 메세지를 다른 클라이언트에 띄워줌
	//			}
	//		}


	//	}

	//}
}

// 클라이언트 연결
void acceptClient(SOCKET& s)
{
	int cnt = 0;	// 대기중인 클라이언트 수 (-1)
	PacketInfo p;

	while (true)
	{
		cClient* c = new cClient();				// 클라이언트 생성
		vecWaitClient.push_back(c);				// 대기 벡터에 넣기
		vecWaitClient[cnt]->AcceptClient(s);	// 소켓 연결
		
		p.curState = eJoin_Room;
		send(vecWaitClient[cnt]->GetMySocket(), (char*)&p, sizeof(PacketInfo), 0);

		if (cnt == 1)	// 대기 벡터에 두개가 차있다면
		{
			cMatchingRoom* matchingRoom = new cMatchingRoom();						// 매칭룸 생성
			matchingRoom->CreateMatchingRoom(vecWaitClient[0], vecWaitClient[1]);	// 매칭룸에 0,1번 클라이언트 넣어주기
			vecWaitClient.clear();													// 대기 벡터 클리어
			cnt = 0;																// 카운트 0으로 초기화
		}
		else			// 아니라면
		{
			cnt++;		// 카운트만 증가
		}
		//sendInfo.curState = eNONE;
		//send(vecSocket[cnt]->GetMySocket(), (char*)&sendInfo, sizeof(PacketInfo), 0);									// 색깔 보내기
		//std::thread(recvMsg, (vecSocket[cnt]->GetMySocket())).detach();													// 쓰레드로 돌리기
		//cnt++;																									// 번호 증가
	}
}

//void RemoveClient(SOCKET s)	// 클라이언트 제거
//{
//	for (int i = 0; i < vecSocket.size(); i++)
//	{
//		if (vecSocket[i]->GetMySocket() == s)
//			vecSocket.erase(vecSocket.begin() + i);
//	}
//	--cnt;
//}

