#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <map>
#include <vector>
#include "User.h"
#include "RoomManager.h"
#include "../../Common/PACKET_HEADER.h"
#include "../../Common/SAFE.h"
using namespace std;

class UserManager
{
	int UserCount;
	map<SOCKET, User *>m_mapUser;
	UserManager();
	~UserManager();
	static UserManager * s_this;

public:
	static UserManager * GetInstance()
	{
		if (s_this == NULL) 
			s_this = new UserManager();

		return s_this;
	}
	void RegisterSocket(SOCKET sock, User * user);
	void DeleteSocket(SOCKET sock);
	User * GetUser(SOCKET sock);
	void err_display(char* szMsg);
	void err_display(int errcode);
	
	//전송 메세지
	void SendAcceptSuccess(SOCKET sock, int roomname[],int len);
	void SendRoomAcceptSuccess(User * user , int roomindex);
	void SendRoomUpdate(User * user, int roomindex);
	void SendRoomDelete(SOCKET sock, int roomname[], int len,BOOL Close = FALSE);
	void SendRoomJoinFail(User * user);
	void SendRoomJoinSuccess(User * user,Room * room);
	void SendRoomUserUpdate(User * user, Room * room,BOOL makeCheck = TRUE);
	void SendRoomUserClose(User * user, Room * room);
	void SendRoomUserEhcoMsg(User * user, Room * room, const char * Msg, int MsgLen);
	void SendGameBlockInfoEhco(User * user, Room * room, PACKET_GAME_BLOCK_INFO paket);
	void SendRoomUserGameStartResult(User * user, Room * room, BOOL Ready);
	void SendRoomGameVictory(Room * room);
	void SendRoomUserGameOverEhco(Room * room);
};



