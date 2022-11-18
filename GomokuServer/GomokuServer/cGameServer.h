#pragma once
#include "stdafx.h"
#include "cMatchingRoomManager.h"
#include <queue>
#include <mutex>


//// �޼��� ť
//typedef std::lock_guard<std::mutex> locker;
//
//template<typename T>
//class MessageQueue
//{
//private:
//	std::queue<T>* _messageQueue;
//	std::mutex _mutex;
//
//public:
//	typedef std::queue<T>* QueueType;
//	
//	MessageQueue()	{ _messageQueue = new std::queue<T>(); }
//	~MessageQueue() { delete _messageQueue; }
//
//	// ��ü ť ����
//	std::queue<T>* CreateQueue() { return new std::queue<T>(); }
//
//	// ť�� �ֱ�
//	void Enqueue(const T& data)
//	{
//		locker mt(_mutex);
//		_messageQueue->push(data);
//	}
//
//	// ť ��ü
//	void UpdateQueue(std::queue<T>*& queue) 
//	{
//		locker mt(_mutex);
//		std::queue<T>* currentQueue = _messageQueue;
//		_messageQueue = queue;
//		queue = currentQueue;
//	}
//};
struct PacketInfo			// ������ ��Ŷ
{
	int x, y;				// �ٵ��� ��ǥ
	int stoneColor;			// �� ����
	ClientState curState;	// ���� ����

	PacketInfo()
	{
		x = -1;
		y = -1;
		stoneColor = 0;
		curState = eNone;
	}
};

class cGameServer
{
private:
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN servAddr;

	cMatchingRoomManager* m_pMatchingRoomMgr;
	std::vector<cClient*> m_vecWaitClient;			// ��븦 ��ٸ����ִ� Ŭ���̾�Ʈ

	PacketInfo sendInfo;	// �������� ��Ŷ
	PacketInfo recvInfo;	// �޴� ��Ŷ

	//std::deque<PacketInfo*> m_pMessageQueue;
	std::queue<PacketInfo*> m_pMessageQueue;

public:
	cGameServer();
	~cGameServer();

	void CreateServer();
	void AcceptClient();
	void recvMsg();
	void MessageProcess();

	//void PushWaitClient(cClient* client) { vecWaitClient.push_back(client); }
	//std::vector<cClient*> GetWaitVector() { return vecWaitClient; }
};