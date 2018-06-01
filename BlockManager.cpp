#include "BlockManager.h"


BlockManager::BlockManager()
{
	for (int i = 0; i < 4; i++)
	{
		USERPOSITION * userinfo = new USERPOSITION;

		switch (i)
		{
		case 0:
			userinfo->BlockSize = 25;
			userinfo->StrarPosition.x = 50;
			userinfo->StrarPosition.y = 55;
			break;
		case 1:
			userinfo->BlockSize = 15;
			userinfo->StrarPosition.x = 470;
			userinfo->StrarPosition.y = 160;
			break;
		case 2:
			userinfo->BlockSize = 15;
			userinfo->StrarPosition.x = 670;
			userinfo->StrarPosition.y = 160;
			break;
		case 3:
			userinfo->BlockSize = 15;
			userinfo->StrarPosition.x = 870;
			userinfo->StrarPosition.y = 160;
			break;
		}
		StratPosition[i] = userinfo;
	}
}


BlockManager::~BlockManager()
{
}

void BlockManager::Init(HWND hWnd, HINSTANCE hInstance, JEngine::User * user, int UserPosition)
{
	m_UserInfo.UserIdLen = user->GetUserStrIdLen();
	strncpy(m_UserInfo.UserId, user->GetUserStrId(), m_UserInfo.UserIdLen);

	m_bGameOver = JEngine::ResourceManager::GetInstance()->Getm_pGameImage(JEngine::GAME_OVER);

	m_iScore = 0;
	m_fCloseTime = 0.0f;

	EndCheck = FALSE;

	for (int i = 0; i < HEIGHT; i++)
	{
		int x = 0;
		int y = StratPosition[UserPosition]->BlockSize* i;
		for (int j = 0; j < WIDTH; j++)
		{
			BLOCKINFO * info = new BLOCKINFO;
			info->Block_color = BLOCK_COLOR_WHITE;
			info->Position.x = StratPosition[UserPosition]->StrarPosition.x + x;
			info->Position.y = StratPosition[UserPosition]->StrarPosition.y + y;
			info->CloseBlock = TRUE;
			info->RemoveBlock = FALSE;
			x += StratPosition[UserPosition]->BlockSize;

			BlockArr[i][j] = info;
		}
	}

	for (int i = (int)JEngine::GAME_BLOCK_BLUE; i <= (int)JEngine::GAME_BLOCK_WHITE; i++)
	{
		m_bBlockList[i - (int)JEngine::GAME_BLOCK_BLUE] = JEngine::ResourceManager::GetInstance()->Getm_pGameImage(i);
		int c = 0;
	}


	BLOCK * block;

	block = new BlockJ();
	block->Init();

	m_vecBlocck.push_back(block);

	block = new BlockI();
	block->Init();

	m_vecBlocck.push_back(block);

	block = new BlockS();
	block->Init();

	m_vecBlocck.push_back(block);

	block = new BlockO();
	block->Init();

	m_vecBlocck.push_back(block);

	block = new BlockT();
	block->Init();

	m_vecBlocck.push_back(block);

	block = new BlockZ();
	block->Init();

	m_vecBlocck.push_back(block);

	block = new BlockL();
	block->Init();

	m_vecBlocck.push_back(block);

	JEngine::FontManager::GetInstance()->ChangeFont(15, 0, 0, 0);

	if (BlockArr[0][0]->Position.x != 50)
		return;

	RegistBlock(rand() % 7);
}

bool BlockManager::Input(float fETime, int key)
{
	if (EndCheck)
		return FALSE;

	if (BlockArr[0][0]->Position.x != 50)
		return FALSE;

	BLOCK * block = m_vecBlockManager.at(0);

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (BlockArr[i][j]->CloseBlock)
				BlockArr[i][j]->Block_color = BLOCK_COLOR_WHITE;
		}
	}

	if (JEngine::InputManager::GetInstance()->isKeyPress(VK_UP))
	{
		if (block->Input(fETime, VK_UP, BlockArr))
		{
			PACKET_GAME_BLOCK_INFO paket = block->GetBlockInfo();
			JEngine::Buffer::GetInstance()->SendGeamBlockInfo(paket, FALSE);
		}
	}
	else  if (JEngine::InputManager::GetInstance()->isKeyPress(VK_RIGHT))
	{
		if (block->Input(fETime, VK_RIGHT, BlockArr))
		{
			PACKET_GAME_BLOCK_INFO paket = block->GetBlockInfo();
			JEngine::Buffer::GetInstance()->SendGeamBlockInfo(paket, FALSE);
		}
	}

	 else if (JEngine::InputManager::GetInstance()->isKeyPress(VK_DOWN))
	{
		if (block->Input(fETime, VK_DOWN, BlockArr))
		{
			PACKET_GAME_BLOCK_INFO paket = block->GetBlockInfo();
			JEngine::Buffer::GetInstance()->SendGeamBlockInfo(paket, FALSE);
		}
	}

	 else if (JEngine::InputManager::GetInstance()->isKeyPress(VK_LEFT))
	{
		if (block->Input(fETime, VK_LEFT, BlockArr))
		{
			PACKET_GAME_BLOCK_INFO paket = block->GetBlockInfo();
			JEngine::Buffer::GetInstance()->SendGeamBlockInfo(paket, FALSE);
		}
	}
	 else if (JEngine::InputManager::GetInstance()->isKeyPress(VK_SPACE))
	{
		 block->Input(fETime, VK_SPACE, BlockArr);
	}
	return false;
}

BOOL BlockManager::Update(float fETime)
{
	if (EndCheck)
	{
		if (BlockArr[0][0]->Position.x == 50)
		{
			if (JEngine::Buffer::GetInstance()->IsUpdate2() && JEngine::Buffer::GetInstance()->IsUpdate() && JEngine::Buffer::GetInstance()->GetUdateCount() == 0)
			{
				JEngine::Buffer::GetInstance()->ChangeUpdateCheck2(FALSE);
				JEngine::Buffer::GetInstance()->ChangeUpdateCheck(FALSE);
				return TRUE;
			}
			else if (JEngine::Buffer::GetInstance()->IsUpdate2() && JEngine::Buffer::GetInstance()->IsUpdate() && JEngine::Buffer::GetInstance()->GetUdateCount() != 0)
			{
				JEngine::Buffer::GetInstance()->SubUdateCount();

				JEngine::Buffer::GetInstance()->ChangeUpdateCheck2(FALSE);
				JEngine::Buffer::GetInstance()->ChangeUpdateCheck(FALSE);

				JEngine::Buffer::GetInstance()->SendGameVictory(m_iScore);

				EndCheck = TRUE;
				return FALSE;
			}
		}
		return FALSE;
	}

	BLOCK * block = NULL;
	BOOL blockcopy = FALSE;

	if (JEngine::Buffer::GetInstance()->GetUdateCount() > 0)
	{
		if (BlockArr[0][0]->Position.x == 50)
		{
 			if (JEngine::Buffer::GetInstance()->IsUpdate2() && JEngine::Buffer::GetInstance()->IsUpdate())
			{
				JEngine::Buffer::GetInstance()->SubUdateCount();

				JEngine::Buffer::GetInstance()->ChangeUpdateCheck2(FALSE);
				JEngine::Buffer::GetInstance()->ChangeUpdateCheck(FALSE);

				JEngine::Buffer::GetInstance()->SendGameVictory(m_iScore);

				EndCheck = TRUE;
				return FALSE;
			}

			goto ELSE;
		}
		PACKET_GAME_BLOCK_INFO blockinfo = JEngine::Buffer::GetInstance()->GetBlockInfo(m_UserInfo.UserId);

		if (blockinfo.paket.wIndex == 0 && blockinfo.paket.wLen == 0)
			return FALSE;

		if (strcmp(blockinfo.UserId, m_UserInfo.UserId) != 0)
			goto ELSE;

		JEngine::Buffer::GetInstance()->BlockInfoPoP(m_UserInfo.UserId);

		JEngine::Buffer::GetInstance()->SubUdateCount();

		PACKET_GAME_BLOCK_INFO nextblockinfo = JEngine::Buffer::GetInstance()->GetBlockInfo(m_UserInfo.UserId);

		if (blockinfo.blockinfo.newBlock && m_vecBlockManager.size() == 1)
		{

			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					if (BlockArr[i][j]->CloseBlock)
						BlockArr[i][j]->Block_color = BLOCK_COLOR_WHITE;
				}
			}

			m_vecBlockManager.at(0)->CopyBlock(BlockArr, FALSE);
			blockcopy = TRUE;
			RegistBlock(blockinfo, blockcopy);

			goto BACK;
		}


		if (nextblockinfo.blockinfo.newBlock)
		{
			blockcopy = TRUE;
			RegistBlock(blockinfo, blockcopy);

			for (int i = 0; i < HEIGHT; i++)
			{
				for (int j = 0; j < WIDTH; j++)
				{
					if (BlockArr[i][j]->CloseBlock)
						BlockArr[i][j]->Block_color = BLOCK_COLOR_WHITE;
				}
			}

			m_vecBlockManager.at(0)->CopyBlock(BlockArr, FALSE);
			m_vecBlockManager.clear();

			goto BACK;
		}

		RegistBlock(blockinfo, blockcopy);
	}

	
ELSE:

	if (m_vecBlockManager.size() == 0)
		return FALSE;

	block = m_vecBlockManager.at(0);

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (BlockArr[i][j]->CloseBlock)
				BlockArr[i][j]->Block_color = BLOCK_COLOR_WHITE;
		}
	}

	if (block->Update(fETime, BlockArr))
	{
		if (BlockArr[0][0]->Position.x == 50)
		{
			block->CopyBlock(BlockArr, FALSE);

			NextBlock();

BACK:
			if (CloseBlock())
				m_fCloseTime = 0.5;

		}
	}
	else
	{
		EndCheck = block->IsEndCheck();

		if (BlockArr[0][0]->Position.x == 50)
		{
			if (EndCheck)
  				JEngine::Buffer::GetInstance()->SendGameOver(m_iScore);
		}
	}
	

	return FALSE;
}

BOOL BlockManager::CloseBlock()
{
	int count = 0;
	int score = 0;

	for (int i = 0; i < HEIGHT; i++)
	{
		BOOL Check = TRUE;
		for (int j = 0; j < WIDTH; j++)
		{
			if (BlockArr[i][j]->CloseBlock)
			{
				Check = FALSE;
				break;
			}
		}

		if (Check)
		{
			for (int c = 0; c < WIDTH; c++)
			{
				BlockArr[i][c]->RemoveBlock = TRUE;
			}
			count++;
			score += 10;
		}
	}

	if (count != 0)
	{
		m_iScore += score*count;
		return TRUE;
	}

	return FALSE;
}

void BlockManager::ClierBlock()
{
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			if (BlockArr[i][j]->RemoveBlock)
			{
				for (int c = i; c > 0; c--)
				{
					BlockArr[c][j]->RemoveBlock = BlockArr[c - 1][j]->RemoveBlock;
					BlockArr[c][j]->CloseBlock = BlockArr[c - 1][j]->CloseBlock;
					BlockArr[c][j]->Block_color = BlockArr[c - 1][j]->Block_color;

				}
				BlockArr[0][j]->RemoveBlock = FALSE;
				BlockArr[0][j]->CloseBlock = TRUE;
				BlockArr[0][j]->Block_color = BLOCK_COLOR_WHITE;
			}
		}
	}
}

void BlockManager::NextBlock()
{

	PACKET_GAME_BLOCK_INFO paket = (*m_vecBlockManager.begin())->GetBlockInfo();

	if (BlockArr[0][0]->Position.x == 50)
		JEngine::Buffer::GetInstance()->SendGeamBlockInfo(paket, FALSE);

	//Sleep(100);

	m_vecBlockManager.erase(m_vecBlockManager.begin());

	(*m_vecBlockManager.begin())->Init();

	paket = (*m_vecBlockManager.begin())->GetBlockInfo();

	if (BlockArr[0][0]->Position.x == 50)
		JEngine::Buffer::GetInstance()->SendGeamBlockInfo(paket, TRUE);

	int num = rand() % 7;
	m_vecBlockManager.push_back(m_vecBlocck.at(num));

}


void BlockManager::Draw(HDC hdc, float fETime)
{
	BOOL SizeUpCheck;

	if (m_fCloseTime >= 0)
	{
		m_fCloseTime -= fETime;

		if (m_fCloseTime < 0)
		{
			ClierBlock();
			m_fCloseTime = 0;
		}
	}
	if (BlockArr[0][0]->Position.x == 50)
	{
		JEngine::FontManager::GetInstance()->ChangeFont(25, 255, 0, 0);
		SizeUpCheck = TRUE;
	}
	else
	{
		JEngine::FontManager::GetInstance()->ChangeFont(25, 0, 0, 0);
		SizeUpCheck = FALSE;
	}

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			int f = m_fCloseTime * 10;

			switch (BlockArr[i][j]->Block_color)
			{
			case BLOCK_COLOR_BLUE:
				if (SizeUpCheck)
					m_bBlockList[0]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
				else
					m_bBlockList[0]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				break;
			case BLOCK_COLOR_SKY:
				if (SizeUpCheck)
					m_bBlockList[1]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
				else
					m_bBlockList[1]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				break;
			case BLOCK_COLOR_GREEN:
				if (SizeUpCheck)
					m_bBlockList[2]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
				else
					m_bBlockList[2]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				break;
			case BLOCK_COLOR_YELLOW:
				if (SizeUpCheck)
					m_bBlockList[3]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
				else
					m_bBlockList[3]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				break;
			case BLOCK_COLOR_PURPLE:
				if (SizeUpCheck)
					m_bBlockList[4]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
				else
					m_bBlockList[4]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				break;
			case BLOCK_COLOR_RED:
				if (SizeUpCheck)
					m_bBlockList[5]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
				else
					m_bBlockList[5]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				break;
			case BLOCK_COLOR_ORANGE:
				if (SizeUpCheck)
					m_bBlockList[6]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
				else
					m_bBlockList[6]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				break;
			case BLOCK_COLOR_WHITE:
				if (SizeUpCheck)
					m_bBlockList[7]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
				else
					m_bBlockList[7]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				break;
			}

			if (f % 2 == 1)
			{
				if (BlockArr[i][j]->RemoveBlock)
				{
					if (SizeUpCheck)
						m_bBlockList[7]->DrawImageSizeUpMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y, 10, 10);
					else
						m_bBlockList[7]->DrawMem(BlockArr[i][j]->Position.x, BlockArr[i][j]->Position.y);
				}
			}
		}
	}

	TextOut(m_bBlockList[0]->GetMemDC(0), BlockArr[HEIGHT - 1][0]->Position.x, BlockArr[HEIGHT - 1][0]->Position.y + 30
		, m_UserInfo.UserId, m_UserInfo.UserIdLen);

	if (EndCheck)
	{
		if(BlockArr[0][0]->Position.x == 50)
			m_bGameOver->DrawMem(BlockArr[0][0]->Position.x, BlockArr[0][0]->Position.y + 40);
		else
			m_bGameOver->DrawMem(BlockArr[0][0]->Position.x-50, BlockArr[0][0]->Position.y + 20);
	}
	
	if (BlockArr[0][0]->Position.x == 50)
	{
		if (m_iScore != 0)
		{
			char Score[100] = { 0, };

			sprintf(Score, "%d", m_iScore);

			TextOut(JEngine::ResourceManager::GetInstance()->Getm_pBackImage()->GetMemDC(0), 350, 320, Score, sizeof(Score));

		}

		BLOCK * block = NULL;
		block = m_vecBlockManager.at(0);

		block = m_vecBlockManager.at(1);

		block->Draw(fETime);
	}
}

void BlockManager::Release()
{
	for (int i = 0; i < 4; i++)
	{
		SAFE_DELETE(StratPosition[i]);
	}

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			SAFE_DELETE(BlockArr[i][j]);
		}
	}

	for (auto iter = m_vecBlocck.begin(); iter < m_vecBlocck.end(); iter++)
	{
		SAFE_DELETE((*iter));
	}
}

void BlockManager::RegistBlock(int BlockNum)
{
	m_vecBlockManager.push_back(m_vecBlocck.at(BlockNum));
	PACKET_GAME_BLOCK_INFO paket = m_vecBlocck.at(BlockNum)->GetBlockInfo();

	if (BlockArr[0][0]->Position.x == 50)
		JEngine::Buffer::GetInstance()->SendGeamBlockInfo(paket, TRUE);

	int num = rand() % 7;
	m_vecBlockManager.push_back(m_vecBlocck.at(num));
}

void BlockManager::RegistBlock(PACKET_GAME_BLOCK_INFO Blockinfo, BOOL copy)
{
	if (Blockinfo.blockinfo.newBlock == FALSE)
	{
		if (m_vecBlockManager.size() == 1)
		{
			POINT point;

			point.x = Blockinfo.blockinfo.Position.x;
			point.y = Blockinfo.blockinfo.Position.y;

			if (!copy)
			{
				if (m_vecBlockManager.at(0)->GetBlockInfo().blockinfo.Position.y >= Blockinfo.blockinfo.Position.y)
				{
					point.y = m_vecBlockManager.at(0)->GetBlockInfo().blockinfo.Position.y;
				}
				m_vecBlockManager.at(0)->Init();
				m_vecBlockManager.at(0)->SetBlockPosition(point, Blockinfo.blockinfo.BlockType);
			}
			else
			{
				m_vecBlockManager.clear();
				m_vecBlocck.at(Blockinfo.blockinfo.Block_color)->Init();
				m_vecBlockManager.push_back(m_vecBlocck.at(Blockinfo.blockinfo.Block_color));
				m_vecBlockManager.at(0)->SetBlockPosition(point, Blockinfo.blockinfo.BlockType);
			}
		}
	}
	else
	{
			m_vecBlockManager.clear();
			m_vecBlocck.at(Blockinfo.blockinfo.Block_color)->Init();
			m_vecBlockManager.push_back(m_vecBlocck.at(Blockinfo.blockinfo.Block_color));
	}
}
