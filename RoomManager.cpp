#include "RoomManager.h"


RoomManager * RoomManager::s_this = NULL;

RoomManager::RoomManager()
{
	m_RoomIndex = 1;
}


RoomManager::~RoomManager()
{

}

void RoomManager::RegisterRoom(Room * room)
{
	m_mapRoomList.insert(make_pair(m_RoomIndex++, room));
}

BOOL RoomManager::DeleteRoom(SOCKET socket)
{
	BOOL RoomDelete = FALSE;
	int RoomCloseNum = -1;
	for (auto iter = m_mapRoomList.begin(); iter != m_mapRoomList.end(); iter++)
	{
		if (iter->second->UserCheck(socket))
		{
			iter->second->OutUser(socket);
			if (iter->second->GetJoinUserCount() == 0)
				RoomCloseNum = iter->second->GetRoomIndex();
			break;
		}
	}

	if (RoomCloseNum != -1)
	{
		m_mapRoomList.erase(RoomCloseNum);
		RoomDelete = TRUE;
	}
	return RoomDelete;
}

BOOL RoomManager::DeleteRoom(SOCKET socket, int RoomIndex)
{
	BOOL RoomDelete = FALSE;

	Room* room = m_mapRoomList[RoomIndex];

	if (room->UserCheck(socket) == FALSE)
		return RoomDelete;

	if (m_mapRoomList.size() != 0)
	{
		//Room* room = m_mapRoomList[RoomIndex];

		room->OutUser(socket);

		if (room->GetJoinUserCount() == 0)
		{
			m_mapRoomList.erase(RoomIndex);
			RoomDelete = TRUE;
		}
	}

	return RoomDelete;
}

int RoomManager::findRoom(SOCKET socket)
{
	int index = 0;

	for (auto iter = m_mapRoomList.begin(); iter != m_mapRoomList.end(); iter++)
	{
		if (iter->second->UserCheck(socket))
		{
			index = iter->first;
			break;
		}
	}
	return index;
}

int RoomManager::GetRoomName(int roomname[])
{
	int  i = 0;
	for (auto iter = m_mapRoomList.begin(); iter != m_mapRoomList.end(); iter++,i++)
	{
		roomname[i] = iter->first;
	}
	return i;
}

