#pragma once
#include "InputManager.h"
#include "ResourceManager.h"
#include "BitMap.h"
#include "FontManager.h"
#include "Point.h"
#include "UserManager.h"
#include <vector> 
#include "Buffer.h"
#include "BLOCK.h"
#include "BlockT.h"
#include "BlockJ.h"
#include "BlockL.h"
#include "BlockZ.h"
#include "BlockS.h"
#include "BlockI.h"
#include "BlockO.h"
#include "..\..\Common\PACKET_HEADER.h"

#define HEIGHT 20
#define WIDTH 10

struct USERPOSITION
{
	JEngine::POINT StrarPosition;
	int BlockSize;
};

struct USERINFO
{
	char UserId[100];
	int UserIdLen;
};

class BlockManager
{
	BLOCKINFO * BlockArr[HEIGHT][WIDTH];
	USERPOSITION * StratPosition[4];
	JEngine::BitMap * m_bBlockList[8];
	JEngine::BitMap * m_bGameOver;
	USERINFO m_UserInfo;
	vector <BLOCK *> m_vecBlockManager;
	vector <BLOCK *> m_vecBlocck;
	int m_iScore;
	float m_fCloseTime;
	BOOL EndCheck;

public:
	BlockManager();
	~BlockManager();
	void Init(HWND hWnd, HINSTANCE hInstance,JEngine::User * user,int UserPosition);
	bool Input(float fETime,int key = 0);
	BOOL Update(float fETime);
	void Draw(HDC hdc, float fETime);
	void Release();
	void RegistBlock(int BlockNum);
	void RegistBlock(PACKET_GAME_BLOCK_INFO Blockinfo,BOOL copy);
	BOOL CloseBlock();
	void ClierBlock();
	void NextBlock();
};

