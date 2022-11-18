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
	closesocket(sock);									// ���� ����
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

	memset(&servAddr, 0, sizeof(servAddr)); // �ʱ�ȭ
	servAddr.sin_family = AF_INET;			// ���� �ּ�ü�� ���ϱ� AF_INET = (x,x,x,x)
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
	//int cnt = 0;	// ������� Ŭ���̾�Ʈ �� (-1)
	//PacketInfo p;

	while (true)
	{
		cClient* c = new cClient();				// Ŭ���̾�Ʈ ����
		c->AcceptClient(sock);					// ���� ��� (���ŷ)
		m_vecWaitClient.push_back(c);			// ��ٸ� ���Ϳ� �����

		//if (cnt == 1)	// ��� ���Ϳ� �ΰ��� ���ִٸ�
		if (m_vecWaitClient.size() == 2)	// ��� ���Ϳ� �ΰ��� ���ִٸ�
		{
			cMatchingRoom* matchingRoom = new cMatchingRoom();							// ��Ī�� ����
			matchingRoom->CreateMatchingRoom(m_vecWaitClient[0], m_vecWaitClient[1]);	// ��Ī�뿡 0,1�� Ŭ���̾�Ʈ �־��ֱ�
			m_pMatchingRoomMgr->PushMatchingRoom(matchingRoom);							// ��Ī�� �Ŵ������� ������ ��Ī�� ������� ��Ŵ
			m_vecWaitClient.clear();													// ��� ���� Ŭ����
			//cnt = 0;																	// ī��Ʈ 0���� �ʱ�ȭ
		}
		//else			// �ƴ϶��
		//{
		//	cnt++;		// ī��Ʈ�� ����
		//}												// ��ȣ ����
	}
}

void cGameServer::recvMsg()		// �޼��� �ޱ� (������� ����)
{
	char buff[PACKET_SIZE] = { 0, };	// �޼��� ����

	while (true)
	{
		recv(sock, buff, sizeof(PACKET_SIZE), 0);
		memcpy((char*)&recvInfo, buff, sizeof(PacketInfo));		// �޸� ī�� (recvInfo�� �����)
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