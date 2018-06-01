#include "UserManager.h"
CRITICAL_SECTION cs;


UserManager * UserManager::s_this = NULL;

UserManager::UserManager()
{
	UserCount = 0;
	InitializeCriticalSection(&cs);
}


UserManager::~UserManager()
{
	DeleteCriticalSection(&cs);
}

void UserManager::RegisterSocket(SOCKET sock, User * user)
{
	EnterCriticalSection(&cs);
	UserCount++;
	m_mapUser.insert(make_pair(sock, user));
	LeaveCriticalSection(&cs);
}

void UserManager::DeleteSocket(SOCKET sock)
{
	m_mapUser.erase(sock);
}

User * UserManager::GetUser(SOCKET sock)
{
	User * user = NULL;

	EnterCriticalSection(&cs);
	user = m_mapUser[sock];
	LeaveCriticalSection(&cs);

	return user;
}

void UserManager::err_display(char * szMsg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s]%s\n", szMsg, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void UserManager::err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[오류]%s", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void UserManager::SendAcceptSuccess(SOCKET sock, int roomname[], int len)
{
	EnterCriticalSection(&cs);

	User * user = m_mapUser[sock];

	LeaveCriticalSection(&cs);

	int retval;
	DWORD sendbytes, flag;

	flag = 0;

	PACKET_LOGIN_RET paket;
	paket.paket.wIndex = PACKET_INDEX_LOGIN_RET;
	paket.paket.wLen = sizeof(PACKET_HEADER) + sizeof(WORD) + (sizeof(WORD)*len);
	paket.id = UserCount;

	for (int i = 0; i < len; i++)
	{
		paket.RoomName[i] = roomname[i];
	}

	user->SendInit();

	user->SendBuffer((char *)&paket, paket.paket.wLen);

	user->SendUp(10);

	user->ChangeCheckSend(TRUE);

	retval = WSASend(user->GetSocket(), &(user->GetWsaSendbuf()), 1, &sendbytes, flag, &user->GetOverlapped(SEND), NULL);

	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			err_display("WSASend()");
		}
	}

}

void UserManager::SendRoomAcceptSuccess(User * user, int roomindex)
{
	int retval;
	DWORD sendbytes, flag;

	flag = 0;

	PACKET_ROOM_ACCEPT paket;
	paket.paket.wIndex = PACKET_INDEX_ROOM_ACCEPT;
	paket.paket.wLen = sizeof(PACKET_ROOM_ACCEPT);
	paket.RoomNumber = roomindex;

	user->SendInit();

	user->SendBuffer((char *)&paket, paket.paket.wLen);

	user->SendUp(10);

	user->ChangeCheckSend(TRUE);

	retval = WSASend(user->GetSocket(), &(user->GetWsaSendbuf()), 1, &sendbytes, flag, &user->GetOverlapped(SEND), NULL);

	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			err_display("WSASend()");
		}
	}

}

void UserManager::SendRoomUpdate(User * user, int roomindex)
{
	int retval;

	PACKET_ROOM_UPDATE paket;
	paket.paket.wIndex = PACKET_INDEX_ROOM_UPDATE;
	paket.paket.wLen = sizeof(PACKET_ROOM_UPDATE);
	paket.RoomNumber = roomindex;


	for (auto iter = m_mapUser.begin(); iter != m_mapUser.end(); iter++)
	{
		DWORD sendbytes, flag;
		flag = 0;

		if (iter->second != user)
		{

			iter->second->SendInit();

			iter->second->SendBuffer((char *)&paket, paket.paket.wLen);

			iter->second->SendUp(10);

			iter->second->ChangeCheckSend(TRUE);

			retval = WSASend(iter->second->GetSocket(), &(iter->second->GetWsaSendbuf()), 1, &sendbytes, flag, &iter->second->GetOverlapped(SEND), NULL);

			if (retval == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					err_display("WSASend()");
				}
			}

		}
	}
}


void UserManager::SendRoomDelete(SOCKET sock, int roomname[], int len, BOOL Close)
{
	int retval;
	User * user = NULL;

	if (Close)
		user = m_mapUser[sock];

	PACKET_ROOM_DELETE paket;
	paket.paket.wIndex = PACKET_INDEX_ROOM_DELETE;
	paket.paket.wLen = sizeof(PACKET_HEADER) + (sizeof(WORD)*len);

	for (int i = 0; i < len; i++)
	{
		paket.RoomName[i] = roomname[i];
	}

	for (auto iter = m_mapUser.begin(); iter != m_mapUser.end(); iter++)
	{
		if (iter->second != user)
		{
			DWORD sendbytes, flag;
			flag = 0;

			iter->second->SendInit();

			iter->second->SendBuffer((char *)&paket, paket.paket.wLen);

			iter->second->SendUp(10);

			iter->second->ChangeCheckSend(TRUE);

			retval = WSASend(iter->second->GetSocket(), &(iter->second->GetWsaSendbuf()), 1, &sendbytes, flag, &iter->second->GetOverlapped(SEND), NULL);

			if (retval == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					err_display("WSASend()");
				}
			}

		}
	}
}

void UserManager::SendRoomJoinFail(User * user)
{
	int retval;

	DWORD sendbytes, flag;
	flag = 0;

	PACKET_ROOM_JOIN_FAIL paket;
	paket.paket.wIndex = PACKET_INDEX_ROOM_JOIN_FAIL;
	paket.paket.wLen = sizeof(PACKET_HEADER);

	user->SendInit();

	user->SendBuffer((char *)&paket, paket.paket.wLen);

	user->SendUp(10);

	user->ChangeCheckSend(TRUE);

	retval = WSASend(user->GetSocket(), &(user->GetWsaSendbuf()), 1, &sendbytes, flag, &user->GetOverlapped(SEND), NULL);

	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			err_display("WSASend()");
		}
	}
}

void UserManager::SendRoomJoinSuccess(User * user, Room * room)  
{
	int retval;

	PACKET_ROOM_JOIN_SUCCESS paket[4];
	
	int count;
	int totalLen = 0;

	for (count = 0; count < room->GetJoinUserCount(); count++)
	{
		paket[count].paket.wIndex = PACKET_INDEX_ROOM_JOIN_SUCCESS;

		paket[count].User.UserCharacter = room->GetUser(count)->GetUserCharacter();

		strcpy(paket[count].User.UserId, room->GetUser(count)->GetUserStrId());

		if (room->GetUser(count) == room->GetRoomMaker())
		{
			paket[count].User.RoomMaker = TRUE;
		}
		else
		{
			paket[count].User.RoomMaker = FALSE;
		}

		paket[count].User.UserReady = room->GetUser(count)->IsReadyCheck();

		for (int i = 0; i < sizeof(paket[count].User.UserId); i++)
		{
			if (paket[count].User.UserId[i] == 0)
			{
				paket[count].User.UserIdLen = i+1;
				break;
			}
		}

		paket[count].paket.wLen = sizeof(PACKET_HEADER) +  sizeof(WORD) + sizeof(WORD) + sizeof(BOOL) + sizeof(BOOL) + paket[count].User.UserIdLen;
	}

	for (int i = 0; i < room->GetJoinUserCount(); i++) //사람 수
	{
		for (int j = 0; j < count; j++) //paket 수
		{
			DWORD sendbytes, flag;
			flag = 0;

			room->GetUser(i)->SendInit();

			room->GetUser(i)->SendBuffer((char *)&paket[j], paket[j].paket.wLen);

			room->GetUser(i)->SendUp(10);

			room->GetUser(i)->ChangeCheckSend(TRUE);

			retval = WSASend(room->GetUser(i)->GetSocket(), &(room->GetUser(i)->GetWsaSendbuf()), 1, &sendbytes, flag, &room->GetUser(i)->GetOverlapped(SEND), NULL);

			if (retval == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					err_display("WSASend()");
				}
			}

		
		}
		
	}


}

void UserManager::SendRoomUserUpdate(User * user, Room * room, BOOL makeCheck)
{
	if (makeCheck)
	{
		if (user == room->GetRoomMaker())  //방장이 나간거임 여기서 방장 변경 해주기
		{
			room->SetRoomMaker(room->GetUser(0));
			room->GetRoomMaker()->ChangeReadyCheck(FALSE);
		}
	}

	int retval;

	PACKET_ROOM_JOIN_SUCCESS paket[4];

	int count;
	int totalLen = 0;

	for (count = 0; count < room->GetJoinUserCount(); count++)
	{
		paket[count].paket.wIndex = PACKET_INDEX_ROOM_JOIN_SUCCESS;

		paket[count].User.UserCharacter = room->GetUser(count)->GetUserCharacter();

		strcpy(paket[count].User.UserId, room->GetUser(count)->GetUserStrId());

		if (room->GetUser(count) == room->GetRoomMaker())
		{
			paket[count].User.RoomMaker = TRUE;
		}
		else
		{
			paket[count].User.RoomMaker = FALSE;
		}

		for (int i = 0; i < sizeof(paket[count].User.UserId); i++)
		{
			if (paket[count].User.UserId[i] == 0)
			{
				paket[count].User.UserIdLen = i + 1;
				break;
			}
		}

		paket[count].User.UserReady = room->GetUser(count)->IsReadyCheck();

		paket[count].paket.wLen = sizeof(PACKET_HEADER) + sizeof(WORD) + sizeof(WORD) + sizeof(BOOL) + sizeof(BOOL) + paket[count].User.UserIdLen;
	}

	
  	for (int i = 0; i < room->GetJoinUserCount(); i++) //사람 수
	{
		for (int j = 0; j < count; j++) //paket 수
		{
			if (room->GetUser(i) == user)
				continue;

			DWORD sendbytes, flag;
			flag = 0;

			room->GetUser(i)->SendInit();

			room->GetUser(i)->SendBuffer((char *)&paket[j], paket[j].paket.wLen);

			room->GetUser(i)->SendUp(10);

			room->GetUser(i)->ChangeCheckSend(TRUE);

			retval = WSASend(room->GetUser(i)->GetSocket(), &(room->GetUser(i)->GetWsaSendbuf()), 1, &sendbytes, flag, &room->GetUser(i)->GetOverlapped(SEND), NULL);
		
			if (retval == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					err_display("WSASend()");
				}
			}

		}

	}

}

void UserManager::SendRoomUserClose(User * user, Room * room)
{
	int retval;
	PACKET_ROOM_USER_CLOSE paket;

	paket.paket.wIndex = PACKET_INDEX_ROOM_USER_CLOSE; 
	paket.paket.wLen = sizeof(PACKET_ROOM_USER_CLOSE);

	for (int i = 0; i < room->GetJoinUserCount(); i++)
	{
		if (user->GetSocket() == room->GetUser(i)->GetSocket())
			continue;

		DWORD sendbytes, flag;
		flag = 0;

		room->GetUser(i)->SendInit();

		room->GetUser(i)->SendBuffer((char *)&paket, paket.paket.wLen);

		room->GetUser(i)->SendUp(10);

		room->GetUser(i)->ChangeCheckSend(TRUE);

		retval = WSASend(room->GetUser(i)->GetSocket(), &(room->GetUser(i)->GetWsaSendbuf()), 1, &sendbytes, flag, &room->GetUser(i)->GetOverlapped(SEND), NULL);

		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				err_display("WSASend()");
			}
		}

	}
}

void UserManager::SendRoomUserEhcoMsg(User * user, Room * room, const char * Msg , int MsgLen)
{
	int retval;
	PACKET_ROOM_USER_MSG paket;

	paket.paket.wIndex = PACKET_INDEX_ROOM_USER_MSG;
	paket.paket.wLen = MsgLen;
	paket.RoomIndex = 0;

	sprintf(paket.Msg, "%s", Msg);

	for (int i = 0; i < room->GetJoinUserCount(); i++)
	{
		DWORD sendbytes, flag;
		flag = 0;

		room->GetUser(i)->SendInit();

		room->GetUser(i)->SendBuffer((char *)&paket, paket.paket.wLen);

		room->GetUser(i)->SendUp(10);

		room->GetUser(i)->ChangeCheckSend(TRUE);

		retval = WSASend(room->GetUser(i)->GetSocket(), &(room->GetUser(i)->GetWsaSendbuf()), 1, &sendbytes, flag, &room->GetUser(i)->GetOverlapped(SEND), NULL);

		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				err_display("WSASend()");
			}
		}
	}

}

void UserManager::SendGameBlockInfoEhco(User * user, Room * room, PACKET_GAME_BLOCK_INFO paket)
{
	int retval;

	for (int i = 0; i < room->GetJoinUserCount(); i++)
	{
		if (user == room->GetUser(i))
			continue;

		DWORD sendbytes, flag;
		flag = 0;

		room->GetUser(i)->SendInit();

		room->GetUser(i)->SendBuffer((char *)&paket, paket.paket.wLen);

		room->GetUser(i)->SendUp(10);

		room->GetUser(i)->ChangeCheckSend(TRUE);

		retval = WSASend(room->GetUser(i)->GetSocket(), &(room->GetUser(i)->GetWsaSendbuf()), 1, &sendbytes, flag, &room->GetUser(i)->GetOverlapped(SEND), NULL);

		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				err_display("WSASend()");
			}
		}
	}
}

void UserManager::SendRoomUserGameStartResult(User * user, Room * room, BOOL Ready)
{
	int retval;
	PACKET_ROOM_USER_READY paket;
	int idLen = 0;
	char  userId[100] = { 0, };

	paket.paket.wIndex = PACKET_INDEX_ROOM_USER_READY;

	memcpy(userId, user->GetUserStrId(), sizeof(userId));

	for (idLen = 0; idLen >= 0; idLen++)
	{
		if (userId[idLen] == 0)
		{
			idLen++;
			break;
		}
	}
	
	paket.RoomIndex = room->GetRoomIndex();

	if (Ready)
		paket.ErrCode = room->GetErrCode(user);
	else
		paket.ErrCode = USER_READY_READY_CANCLE;

	strncpy(paket.UserId, userId , idLen);
	
	paket.paket.wLen = sizeof(PACKET_HEADER) + sizeof(WORD) + sizeof(WORD) + idLen;

	for (int i = 0; i < room->GetJoinUserCount(); i++)
	{
		DWORD sendbytes, flag;
		flag = 0;

		room->GetUser(i)->SendInit();

		room->GetUser(i)->SendBuffer((char *)&paket, paket.paket.wLen);

		room->GetUser(i)->SendUp(10);

		room->GetUser(i)->ChangeCheckSend(TRUE);

		retval = WSASend(room->GetUser(i)->GetSocket(), &(room->GetUser(i)->GetWsaSendbuf()), 1, &sendbytes, flag, &room->GetUser(i)->GetOverlapped(SEND), NULL);

		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				err_display("WSASend()");
			}
		}
	}

}

void UserManager::SendRoomGameVictory(Room * room)
{
	int retval;
	PACKET_GAME_END_EHCO paket;

	User * user = room->GetUserGameVictory();

	if (user == NULL)
		return;

	paket.paket.wIndex = PACKET_INDEX_GAME_VICORY;
	paket.paket.wLen = sizeof(PACKET_HEADER);

	DWORD sendbytes, flag;
	flag = 0;

	user->SendInit();

	user->SendBuffer((char *)&paket, paket.paket.wLen);

	user->SendUp(10);

	user->ChangeCheckSend(TRUE);

	retval = WSASend(user->GetSocket(), &(user->GetWsaSendbuf()), 1, &sendbytes, flag, &user->GetOverlapped(SEND), NULL);

	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			err_display("WSASend()");
		}
	}

}

void UserManager::SendRoomUserGameOverEhco(Room * room)
{
	int retval;

	PACKET_GAME_END_EHCO paket[4];
	int idLen = 0;
	char  userId[100] = { 0, };

	for (int i = 0; i < room->GetJoinUserCount(); i++)
	{
		memcpy(userId, room->GetUser(i)->GetUserStrId(), sizeof(userId));

		for (idLen = 0; idLen >= 0; idLen++)
		{
			if (userId[idLen] == 0)
			{
				idLen++;
				break;
			}
		}

		paket[i].paket.wIndex = PACKET_INDEX_GAME_END_EHCO;
		paket[i].paket.wLen = sizeof(PACKET_HEADER) + sizeof(WORD) + sizeof(WORD) + idLen;
		paket[i].Ranking = room->GetUserRanking(room->GetUser(i));
		paket[i].Score = room->GetUser(i)->GetGameScore();
		strncpy(paket[i].UserId, userId, idLen);
	}

	for (int i = 0; i < room->GetJoinUserCount(); i++) //사람 수
	{
		for (int j = 0; j < room->GetJoinUserCount(); j++) //paket 수
		{
			DWORD sendbytes, flag;
			flag = 0;

			room->GetUser(j)->SendInit();

			room->GetUser(j)->SendBuffer((char *)&paket[i], paket[i].paket.wLen);

			room->GetUser(j)->SendUp(10);

			room->GetUser(j)->ChangeCheckSend(TRUE);

			retval = WSASend(room->GetUser(j)->GetSocket(), &(room->GetUser(j)->GetWsaSendbuf()), 1, &sendbytes, flag, &room->GetUser(j)->GetOverlapped(SEND), NULL);

			if (retval == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					err_display("WSASend()");
				}
			}

		}

	}

}

