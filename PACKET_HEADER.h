#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <map>

using namespace std;

//word 로 하는이유 적은 바이트를 보내야하기떄문 Word -2바이트 DWord - 4바이트
#pragma pack(1)


enum PACKET_INDEX
{
	PACKET_INDEX_LOGIN_RET,			//접속 정보(ID정보 보냄)
	PACKET_INDEX_CLOSE,
	PACKET_INDEX_USER_UPDATE,
	PACKET_INDEX_ROOM_MAKE,
	PACKET_INDEX_ROOM_ACCEPT,
	PACKET_INDEX_ROOM_UPDATE,
	PACKET_INDEX_ROOM_DELETE,
	PACKET_INDEX_ROOM_CLOSE,  //클라 -> 서버
	PACKET_INDEX_ROOM_JOIN,
	PACKET_INDEX_ROOM_JOIN_FAIL,
	PACKET_INDEX_ROOM_JOIN_SUCCESS,
	PACKET_INDEX_ROOM_USER_CLOSE,  // 서버 -> 클라 (나간 사람)
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
	USER_READY_NON_PLAYER, //방장말고 사람이 없을떄
	USER_READY_NON_READY,  //다른유저가 시작을 누르지 않았을떄
	USER_READY_SUCCESS, //성공(일반유저 시작 성공 시)
	USER_READY_GAME_START, //다른유저가 다 시작을 누르고 방장이 시작버튼누르면
	USER_READY_READY_CANCLE, //유저가 ready 취소
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