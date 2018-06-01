#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#include <iostream>
#include "UserManager.h"
#include "RoomManager.h"
#include "..\..\Common\SAFE.h"

using namespace std;

unsigned int WINAPI WorkerThread(void* arg);
bool ProcessPacket(User * user, char * pBuf, DWORD &  len);

int main(int argc, char* argv[])
{
	srand(GetTickCount());

	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	//입출력 완료 포트 생성
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;

	//cpu 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 3; i++)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, hcp, 0, NULL);
		if (hThread == NULL)
			return 1;
		CloseHandle(hThread);
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		cout << "err on socket" << endl;
		return -1;
	}

	//bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		cout << "err on bind" << endl;
		return -1;
	}

//listen
retval = listen(listen_sock, SOMAXCONN);
if (retval == SOCKET_ERROR)
{
	cout << "err on listen" << endl;
	return -1;
}

SOCKET client_sock;
SOCKADDR_IN Clientaddr;

while (1)
{
	int addrlen;
	DWORD recvbytes, flags;

	flags = 0;

	addrlen = sizeof(Clientaddr);
	client_sock = accept(listen_sock, (SOCKADDR *)&Clientaddr, &addrlen);

	if (client_sock == INVALID_SOCKET)
	{
		continue;
	}

	BOOL opt_val = TRUE;

	setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&opt_val, sizeof(opt_val));

	::printf("[TCP 서버] 클라이언트 접속 : IP 주소 = %s , 포트번호 = %d\n", inet_ntoa(Clientaddr.sin_addr), ntohs(Clientaddr.sin_port));

	CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);

	User * user = new User(client_sock);

	UserManager::GetInstance()->RegisterSocket(client_sock, user);

	user->RecvUp(10);
	user->ChangeCheckRecv(TRUE);
	user->RecvInit();

	retval = WSARecv(user->GetSocket(), &user->GetWsaRecvbuf(), 1, &recvbytes, &flags, &user->GetOverlapped(RECV), NULL);

	int roomname[100];

	int len = RoomManager::GetInstance()->GetRoomName(roomname);

	UserManager::GetInstance()->SendAcceptSuccess(client_sock, roomname, len);
}

WSACleanup();

return 0;
}

unsigned int WINAPI WorkerThread(void* arg)
{
	int retval;
	HANDLE hcp = (HANDLE)arg;

	DWORD cbTransferred;
	DWORD Len = 0;
	SOCKET client_sock;
	over * overlapped;
	DWORD recvbytes, flags;
	User * user = NULL;

	while (1)
	{
	STRART:

		retval = GetQueuedCompletionStatus(hcp, &cbTransferred, (LPDWORD)&client_sock, (LPOVERLAPPED *)&overlapped, INFINITE);

		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				break;
			}
			else
				continue;
		}

		Len = cbTransferred;

		user = UserManager::GetInstance()->GetUser(client_sock);

		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(user->GetSocket(), (SOCKADDR *)&clientaddr, &addrlen);

		if (retval == 0)   //강제 종료
		{
			DWORD temp1, temp2;
			WSAGetOverlappedResult(user->GetSocket(), &user->GetOverlapped(RECV), &temp1, FALSE, &temp2);

			int index = RoomManager::GetInstance()->findRoom(user->GetSocket());

			if (index != 0)
			{
				if (RoomManager::GetInstance()->DeleteRoom(user->GetSocket(), index)) //방이 없어지는거 
				{
					int roomname[100];

					int len = RoomManager::GetInstance()->GetRoomName(roomname);

					UserManager::GetInstance()->SendRoomDelete(user->GetSocket(), roomname, len, TRUE);
				}
				else //방이 없어지지않고 사람만 나간거   
				{
					int roomname[100];

					int len = RoomManager::GetInstance()->GetRoomName(roomname);

					Room * room = RoomManager::GetInstance()->GetRoom(index);

					UserManager::GetInstance()->SendRoomDelete(user->GetSocket(), roomname, len, TRUE);

					Sleep(100);

					UserManager::GetInstance()->SendRoomUserClose(user, room);

					Sleep(100);

					if (!user->IsReadyCheck())
					{
						UserManager::GetInstance()->SendRoomUserUpdate(user, room);
					}
				}
			}

			Sleep(100);

			UserManager::GetInstance()->DeleteSocket(user->GetSocket());
			closesocket(user->GetSocket());
			SAFE_DELETE(user);

			::printf("[TCP 서버] 클라이언트 퇴장 : IP 주소 = %s , 포트번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			continue;
		}

		int RecvCount = user->GetRecvCount();
		int SendCount = user->GetSendCount();

		switch (overlapped->type)
		{
		case SEND:
			user->SendDown(10);
			user->ChangeCheckSend(FALSE);
			goto STRART;
		break;

		case RECV:
			user->SetTransfrred(cbTransferred);
			user->RecvDown(10);
			user->ChangeCheckRecv(FALSE);
		}

		DWORD len = user->GetTransfrred();

		while (true)
		{
			if (!ProcessPacket(user, user->m_wsBuf, len))
			{
				if (!user->IsRecv())
				{
					flags = 0;
					user->RecvUp(10);
					user->ChangeCheckRecv(TRUE);
					user->RecvInit();

					retval = WSARecv(user->GetSocket(), &user->GetWsaRecvbuf(), 1, &recvbytes, &flags, &user->GetOverlapped(RECV), NULL);
				}

				break;
			}
			else
			{
				if (user->m_iLen <= sizeof(PACKET_HEADER))
				{
					if (user->IsDelete())  //정상적인 종료
					{
						UserManager::GetInstance()->DeleteSocket(user->GetSocket());
						closesocket(user->GetSocket());
						SAFE_DELETE(user);

						break;
					}

					if (!user->IsRecv() ) //다읽었으니 다시 대기(보낸 대이터가 없을 경우 )
					{
					re1:

						flags = 0;
						user->RecvUp(10);
						user->ChangeCheckRecv(TRUE);
						user->RecvInit();
						retval = WSARecv(user->GetSocket(), &user->GetWsaRecvbuf(), 1, &recvbytes, &flags, &user->GetOverlapped(RECV), NULL);
					}

					break;
				}
				else
				{							
					continue;				
				}
			}
		}


	}

	return 0;
}


bool ProcessPacket(User * user, char * pBuf, DWORD &  len)
{
	if (len != 0)
	{
		memcpy(&user->m_szBuf[user->m_iLen], pBuf, len);

		PACKET_HEADER header;
		memcpy(&header, user->m_szBuf, sizeof(PACKET_HEADER));

		user->m_iLen += len;

		len = 0;
		user->SetTransfrred(0);
	}

	if (user->m_iLen < sizeof(PACKET_HEADER))
	{
		return false;
	}

	PACKET_HEADER header;
	memcpy(&header, user->m_szBuf, sizeof(PACKET_HEADER));

	if (user->m_iLen < header.wLen)
	{
		return false;
	}

	switch (header.wIndex)
	{
	case PACKET_INDEX_CLOSE:
	{
		PACKET_CLOSE paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		if (paket.RoomDelete)
		{
			if (RoomManager::GetInstance()->DeleteRoom(user->GetSocket(), paket.RoonIndex))
			{
				int roomname[100];

				int len = RoomManager::GetInstance()->GetRoomName(roomname);

				UserManager::GetInstance()->SendRoomDelete(user->GetSocket(), roomname, len, TRUE);
			}
			else //room 정보 넘겨주기 
			{
				int roomname[100];

				int len = RoomManager::GetInstance()->GetRoomName(roomname);

				Room * room = RoomManager::GetInstance()->GetRoom(paket.RoonIndex);

				UserManager::GetInstance()->SendRoomDelete(user->GetSocket(), roomname, len, TRUE);

				Sleep(100);

				UserManager::GetInstance()->SendRoomUserClose(user, room);

				Sleep(100);

				UserManager::GetInstance()->SendRoomUserUpdate(user, room);
			}
		}
		else
		{
			int c = 0;

		}

		Sleep(100);

		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(user->GetSocket(), (SOCKADDR *)&clientaddr, &addrlen);

		::printf("[TCP 서버] 클라이언트 퇴장 : IP 주소 = %s , 포트번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		user->ChangeCheckDelete();
	}
	break;

	case PACKET_INDEX_ROOM_MAKE:
	{
		PACKET_ROOM_MAKE paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		Room * room = new Room();
		room->SetRoomMaker(user);
		room->SetRoomIndex(RoomManager::GetInstance()->GetRoomIndex());
		room->JoinUser(user);

		UserManager::GetInstance()->SendRoomAcceptSuccess(user, RoomManager::GetInstance()->GetRoomIndex());

		Sleep(100);

		UserManager::GetInstance()->SendRoomUpdate(user, RoomManager::GetInstance()->GetRoomIndex());

		RoomManager::GetInstance()->RegisterRoom(room);
	}
	break;

	case PACKET_INDEX_USER_UPDATE:
	{
		PACKET_USER_UPDATE paket;

		memcpy(&paket, user->m_szBuf, header.wLen);
		user->SetUserInfo((char *)paket.UserId, (USER_CHARACTER)paket.UserCharacter);

	}
	break;

	case PACKET_INDEX_ROOM_CLOSE:
	{
		PACKET_ROOM_CLOSE paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		user->ChangeReadyCheck(FALSE);

		if (RoomManager::GetInstance()->DeleteRoom(user->GetSocket(), paket.RoomIndex))
		{
			int roomname[100];

			int len = RoomManager::GetInstance()->GetRoomName(roomname);

			UserManager::GetInstance()->SendRoomDelete(user->GetSocket(), roomname, len);
		}
		else //room 정보 넘겨주기 
		{
			int roomname[100];

			int len = RoomManager::GetInstance()->GetRoomName(roomname);

			Room * room = RoomManager::GetInstance()->GetRoom(paket.RoomIndex);

			UserManager::GetInstance()->SendRoomDelete(user->GetSocket(), roomname, len);

			Sleep(100);

			UserManager::GetInstance()->SendRoomUserClose(user, room);

			Sleep(100);

			UserManager::GetInstance()->SendRoomUserUpdate(user, room);
		}

	}
	break;

	case PACKET_INDEX_ROOM_USER_MSG:
	{
		PACKET_ROOM_USER_MSG paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		Room * room = RoomManager::GetInstance()->GetRoom(paket.RoomIndex);

		UserManager::GetInstance()->SendRoomUserEhcoMsg(user, room, paket.Msg, paket.paket.wLen);
	}
	break;

	case PACKET_INDEX_ROOM_JOIN:
	{
		PACKET_ROOM_JOIN paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		Room * room = RoomManager::GetInstance()->GetRoom(paket.RoomIndex);

		if (room->JoinUser(user)) //접속 성공
		{
			UserManager::GetInstance()->SendRoomJoinSuccess(user, room); //user -> 방에접속할려는 사람 
		}
		else //방에 접속 못했음(사람이 4명다찰경우)
		{
			//접속 실패 메세지 보내기 
			UserManager::GetInstance()->SendRoomJoinFail(user);
		}

	}
	break;

	case PACKET_INDEX_GAME_BLOCK_INFO:
	{
		PACKET_GAME_BLOCK_INFO paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		Room * room = RoomManager::GetInstance()->GetRoom(paket.RoomIndex);

		UserManager::GetInstance()->SendGameBlockInfoEhco(user, room, paket);

	}
	break;

	case PACKET_INDEX_ROOM_USER_READY:
	{
		PACKET_ROOM_USER_READY paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		Room * room = RoomManager::GetInstance()->GetRoom(paket.RoomIndex);

		if (room->GetRoomMaker() != user)
			user->ChangeReadyCheck(paket.ErrCode);

		UserManager::GetInstance()->SendRoomUserGameStartResult(user, room, paket.ErrCode);
	}
	break;

	case PACKET_INDEX_GAME_VICORY:
	{
		PACKET_GAME_VICORY paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		Room * room = RoomManager::GetInstance()->GetRoom(paket.RoomIndex);

		user->SetGameScore(paket.Score);

		if (room->ResisterGameVictoryUser(user))
		{
			UserManager::GetInstance()->SendRoomUserGameOverEhco(room);

			room->Reset();
		}
	}
	break;

	case PACKET_INDEX_GAME_OVER:
	{
		PACKET_GAME_OVER paket;

		memcpy(&paket, user->m_szBuf, header.wLen);

		Room * room = RoomManager::GetInstance()->GetRoom(paket.RoomIndex);
	
		user->SetGameScore(paket.Score);

		if (room->ResisterGameOverUser(user,room))
		{
			UserManager::GetInstance()->SendRoomGameVictory(room);
		}
	}
	break;

	}

	if (user->m_iLen - header.wLen > 0)
	{
		memcpy(user->m_szBuf, &user->m_szBuf[header.wLen], user->m_iLen - header.wLen);
	}

		user->m_iLen -= header.wLen;

	return true;
}