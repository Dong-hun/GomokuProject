#pragma once
#include "stdafx.h"
#include "cMatchingRoom.h"

// ��Ī�� �Ŵ���
class cMatchingRoomManager
{
private:
	std::vector<cMatchingRoom*> m_pVecMatchingRoom;	// ��Ī�� ���ͷ� ����ֱ�

public:
	cMatchingRoomManager();
	~cMatchingRoomManager();
	
	void PushMatchingRoom(cMatchingRoom* matchingRoom)	{ m_pVecMatchingRoom.push_back(matchingRoom); }
	std::vector<cMatchingRoom*> GetMatchingRoomList()	{ return m_pVecMatchingRoom; }

	cMatchingRoom FindMachingRoom(SOCKET s);
};