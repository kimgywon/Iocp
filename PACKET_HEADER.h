#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <map>

using namespace std;

//word �� �ϴ����� ���� ����Ʈ�� �������ϱ⋚�� Word -2����Ʈ DWord - 4����Ʈ
#pragma pack(1)


enum PACKET_INDEX
{
	PACKET_INDEX_LOGIN_RET,			//���� ����(ID���� ����)
	PACKET_INDEX_CLOSE,
	PACKET_INDEX_USER_UPDATE,
	PACKET_INDEX_ROOM_MAKE,
	PACKET_INDEX_ROOM_ACCEPT,
	PACKET_INDEX_ROOM_UPDATE,
	PACKET_INDEX_ROOM_DELETE,
	PACKET_INDEX_ROOM_CLOSE,  //Ŭ�� -> ����
	PACKET_INDEX_ROOM_JOIN,
	PACKET_INDEX_ROOM_JOIN_FAIL,
	PACKET_INDEX_ROOM_JOIN_SUCCESS,
	PACKET_INDEX_ROOM_USER_CLOSE,  // ���� -> Ŭ�� (���� ���)
	PACKET_INDEX_ROOM_USER_MSG,
	PACKET_INDEX_ROOM_USER_READY,
	PACKET_INDEX_GAME_BLOCK_INFO,
	PACKET_INDEX_GAME_OVER,
	PACKET_INDEX_GAME_VICORY,
	PACKET_INDEX_GAME_END_EHCO,
};

enum USER_CHARACTER
{
	USER_CHARACTER_DOG,
	USER_CHARACTER_COW,
	USER_CHARACTER_MONCY,
	USER_CHARACTER_CHICKEN,
	USER_CHARACTER_NON,
};

enum USER_READY_ERRCODE
{
	USER_READY_NON_PLAYER, //���帻�� ����� ������
	USER_READY_NON_READY,  //�ٸ������� ������ ������ �ʾ�����
	USER_READY_SUCCESS, //����(�Ϲ����� ���� ���� ��)
	USER_READY_GAME_START, //�ٸ������� �� ������ ������ ������ ���۹�ư������
	USER_READY_READY_CANCLE, //������ ready ���
	USER_READY_NON,
};

enum BLOCK_COLOR
{
	BLOCK_COLOR_BLUE,
	BLOCK_COLOR_SKY,
	BLOCK_COLOR_GREEN,
	BLOCK_COLOR_YELLOW,
	BLOCK_COLOR_PURPLE,
	BLOCK_COLOR_RED,
	BLOCK_COLOR_ORANGE,
	BLOCK_COLOR_WHITE,
};

struct PACKET_HEADER
{
	WORD wIndex;
	WORD wLen;
};

struct USER_INFO
{
	WORD UserCharacter;
	WORD UserIdLen;
	BOOL RoomMaker;
	BOOL UserReady;
	char UserId[100];
};

struct BLOCKINFO
{
	BLOCK_COLOR Block_color;
	POINT Position;
	BOOL CloseBlock;
	BOOL RemoveBlock;
};

struct SENDBLOCKINFO
{
	WORD Block_color;
	POINT Position;
	WORD BlockType;
	WORD newBlock;
};

enum IOTYPE
{
	RECV,
	SEND,
};

struct over :  public OVERLAPPED
{
	IOTYPE type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct	PACKET_LOGIN_RET
{
	PACKET_HEADER paket;
	WORD id;
	WORD RoomName[100];
};

struct PACKET_CLOSE
{
	PACKET_HEADER paket;
	WORD RoonIndex;
	BOOL RoomDelete;
};

struct PACKET_ROOM_MAKE
{
	PACKET_HEADER paket;
};

struct PACKET_ROOM_ACCEPT
{
	PACKET_HEADER paket;
	WORD RoomNumber;
};

struct PACKET_ROOM_UPDATE
{
	PACKET_HEADER paket;
	WORD RoomNumber;
};

struct PACKET_ROOM_DELETE
{
	PACKET_HEADER paket;
	WORD RoomName[100];
};

struct PACKET_USER_UPDATE
{
	PACKET_HEADER paket;
	WORD UserCharacter;
	char UserId[100];
};

struct PACKET_ROOM_CLOSE
{
	PACKET_HEADER paket;
	WORD RoomIndex;
};

struct PACKET_ROOM_JOIN
{
	PACKET_HEADER paket;
	WORD RoomIndex;
};

struct PACKET_ROOM_JOIN_FAIL
{
	PACKET_HEADER paket;
};

struct PACKET_ROOM_JOIN_SUCCESS
{
	PACKET_HEADER paket;
	USER_INFO User;
};

struct PACKET_ROOM_USER_CLOSE
{
	PACKET_HEADER paket;
};

struct 	PACKET_ROOM_USER_MSG
{
	PACKET_HEADER paket;
	WORD RoomIndex;
	char Msg[1000];
};

struct PACKET_ROOM_USER_READY
{
	PACKET_HEADER paket;
	WORD RoomIndex;
	WORD ErrCode;
	char UserId[100];
};

struct PACKET_GAME_BLOCK_INFO
{
	PACKET_HEADER paket;
	WORD RoomIndex;
	SENDBLOCKINFO blockinfo;
	char UserId[100];
};

struct PACKET_GAME_OVER
{
	PACKET_HEADER paket;
	WORD RoomIndex;
	WORD Score;
};

struct PACKET_GAME_VICORY
{
	PACKET_HEADER paket;
	WORD RoomIndex;
	WORD Score;
};

struct PACKET_GAME_END_EHCO
{
	PACKET_HEADER paket;
	WORD Score;
	WORD Ranking;
	char UserId[100];
};

#pragma pack()