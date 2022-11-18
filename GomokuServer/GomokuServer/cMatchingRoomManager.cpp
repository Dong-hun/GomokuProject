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

cMatchingRoom cMatchingRoomManager::FindMachingRoom(SOCKET s)
{
	for (int i = 0; i < m_pVecMatchingRoom.size(); i++)
	{

	}

	return cMatchingRoom();
}
