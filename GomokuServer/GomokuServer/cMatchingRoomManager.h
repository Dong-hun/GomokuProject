#pragma once
#include "stdafx.h"
#include "cMatchingRoom.h"

// ¸ÅÄª·ë ¸Å´ÏÁ®
class cMatchingRoomManager
{
private:
	std::vector<cMatchingRoom*> m_pVecMatchingRoom;	// ¸ÅÄª·ë º¤ÅÍ·Î ´ã¾ÆÁÖ±â

public:
	cMatchingRoomManager();
	~cMatchingRoomManager();
	
	void PushMatchingRoom(cMatchingRoom* matchingRoom)	{ m_pVecMatchingRoom.push_back(matchingRoom); }
	std::vector<cMatchingRoom*> GetMatchingRoomList()	{ return m_pVecMatchingRoom; }

	cMatchingRoom FindMachingRoom(SOCKET s);
};