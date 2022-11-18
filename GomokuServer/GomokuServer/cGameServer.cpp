#include "stdafx.h"
#include "cGameServer.h"

cGameServer::cGameServer()
{
	m_pMatchingRoomMgr = new cMatchingRoomManager();
	servAddr = { 0 };
}

cGameServer::~cGameServer()
{
	delete m_pMatchingRoomMgr;
	servAddr = { 0 };
	closesocket(sock);									// 소켓 종료
	WSACleanup();
}

void cGameServer::CreateServer()
{
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

	printf("Success Create Server\n");
}

void cGameServer::AcceptClient()
{
	//int cnt = 0;	// 대기중인 클라이언트 수 (-1)
	//PacketInfo p;

	while (true)
	{
		cClient* c = new cClient();				// 클라이언트 생성
		c->AcceptClient(sock);					// 연결 대기 (블로킹)
		m_vecWaitClient.push_back(c);			// 됬다면 벡터에 담아줌

		//if (cnt == 1)	// 대기 벡터에 두개가 차있다면
		if (m_vecWaitClient.size() == 2)	// 대기 벡터에 두개가 차있다면
		{
			cMatchingRoom* matchingRoom = new cMatchingRoom();							// 매칭룸 생성
			matchingRoom->CreateMatchingRoom(m_vecWaitClient[0], m_vecWaitClient[1]);	// 매칭룸에 0,1번 클라이언트 넣어주기
			m_pMatchingRoomMgr->PushMatchingRoom(matchingRoom);							// 매칭룸 매니저에게 생성된 매칭룸 담으라고 시킴
			m_vecWaitClient.clear();													// 대기 벡터 클리어
			//cnt = 0;																	// 카운트 0으로 초기화
		}
		//else			// 아니라면
		//{
		//	cnt++;		// 카운트만 증가
		//}												// 번호 증가
	}
}

void cGameServer::recvMsg()		// 메세지 받기 (쓰레드로 돌림)
{
	char buff[PACKET_SIZE] = { 0, };	// 메세지 버퍼

	while (true)
	{
		recv(sock, buff, sizeof(PACKET_SIZE), 0);
		memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// 메모리 카피 (recvInfo에 담아줌)
		m_pMatchingRoomMgr->FindMachingRoom();
	}
}

//void cGameServer::MessageProcess()
//{
//	while (true)
//	{
//		Sleep(1000);
//		if (!m_pMessageQueue.empty())
//		{
//			PacketInfo* p = m_pMessageQueue.front();
//			m_pMessageQueue.pop();
//		}
//	}
//}