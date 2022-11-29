#include "stdafx.h"
#include "cMatchingRoomManager.h"

cMatchingRoomManager::cMatchingRoomManager()
{
	m_pVecMatchingRoom.clear();
}

cMatchingRoomManager::~cMatchingRoomManager()
{
	for (int i = 0; i < m_pVecMatchingRoom.size(); i++)
		delete m_pVecMatchingRoom[i];

	m_pVecMatchingRoom.clear();
}

cMatchingRoom* cMatchingRoomManager::GetMachingRoom(SOCKET s)
{
	for (int i = 0; i < m_pVecMatchingRoom.size(); i++)
	{
		cMatchingRoom* room = m_pVecMatchingRoom[i];
		if (room->GetBlackStone()->sock == s || room->GetWhiteStone()->sock == s)
			return room;
	}

	return nullptr;
}

void cMatchingRoomManager::CreateMatchingRoom(cClient * black, cClient* white)
{
	cMatchingRoom* room = new cMatchingRoom();
	room->CreateMatchingRoom(black, white);
	m_pVecMatchingRoom.push_back(room);
}

bool cMatchingRoomManager::UpdateMatchingRoom(SOCKET s, PacketInfo * p)
{
	cMatchingRoom* pRoom = GetMachingRoom(s);

	if (pRoom != nullptr)
	{
		pRoom->ProcessMessageRoomClient(p);
		return true;
	}

	return false;
}
