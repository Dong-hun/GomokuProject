#pragma once
#include "stdafx.h"
#include "cMatchingRoomManager.h"
#include <queue>
#include <mutex>


//// 메세지 큐
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
//	// 교체 큐 생성
//	std::queue<T>* CreateQueue() { return new std::queue<T>(); }
//
//	// 큐에 넣기
//	void Enqueue(const T& data)
//	{
//		locker mt(_mutex);
//		_messageQueue->push(data);
//	}
//
//	// 큐 교체
//	void UpdateQueue(std::queue<T>*& queue) 
//	{
//		locker mt(_mutex);
//		std::queue<T>* currentQueue = _messageQueue;
//		_messageQueue = queue;
//		queue = currentQueue;
//	}
//};
struct PacketInfo			// 전달할 패킷
{
	int x, y;				// 바둑판 좌표
	int stoneColor;			// 돌 색깔
	ClientState curState;	// 현재 상태

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
	std::vector<cClient*> m_vecWaitClient;			// 상대를 기다리고있는 클라이언트

	PacketInfo sendInfo;	// 보내려는 패킷
	PacketInfo recvInfo;	// 받는 패킷

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