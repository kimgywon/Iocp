#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <map>
#include <vector>
#include "Room.h"
#include "../../Common/PACKET_HEADER.h"

class RoomManager
{
	int m_RoomIndex;
	map<int, Room *> m_mapRoomList;
	RoomManager();
	~RoomManager();
	static RoomManager * s_this;
public:
	static RoomManager * GetInstance()
	{
		if (s_this == NULL)
			s_this = new RoomManager();

		return s_this;
	}
	void RegisterRoom(Room * room);
	BOOL DeleteRoom(SOCKET socket);
	BOOL DeleteRoom(SOCKET socket,int RoomIndex);
	int findRoom(SOCKET socket);
	int GetRoomName(int roomname[]);
	inline Room * GetRoom(int  roomindex) { return m_mapRoomList[roomindex]; }
	inline int GetRoomIndex() { return m_RoomIndex; }
};

