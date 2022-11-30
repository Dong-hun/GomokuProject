#pragma once
#include "stdafx.h"
//#include "PacketInfo.h"
#include "cMatchingRoom.h"

// ¸ÅÄª·ë ¸Å´ÏÁ®
class cMatchingRoomManager
{
private:
	std::vector<cMatchingRoom*> m_pVecMatchingRoom;	// ¸ÅÄª·ë º¤ÅÍ·Î ´ã¾ÆÁÖ±â

public:
	cMatchingRoomManager();
	~cMatchingRoomManager();
	
	std::vector<cMatchingRoom*> GetMatchingRoomList()	{ return m_pVecMatchingRoom; }

	cMatchingRoom* GetMachingRoom(SOCKET s);
	void CreateMatchingRoom(cClient * black, cClient* white);
	bool UpdateMatchingRoom(SOCKET s, PacketInfo* p);
	//void PushMatchingRoom(cMatchingRoom* matchingRoom)	{ m_pVecMatchingRoom.push_back(matchingRoom); }
	//void UpdateMatchingRoom(int x, int y, int color);
};