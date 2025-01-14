#include "Game/BlockDefinition.hpp"

BlockDefinition BlockDefinition::s_blockDefinitions[19];

void BlockDefinition::initializeBlockDefs()
{
	CreateNewBlockDef(0,  "air",			false,	false,	false,	IntVec2(0, 0),		IntVec2(0, 0),		IntVec2(0, 0),   0);
	CreateNewBlockDef(1,  "grass",			true,	true,	true,	IntVec2(32, 33),	IntVec2(33, 33),	IntVec2(32, 34), 0);
	CreateNewBlockDef(2,  "dirt",			true,	true,	true,	IntVec2(32, 34),	IntVec2(32, 34),	IntVec2(32, 34), 0);
	CreateNewBlockDef(3,  "stone",			true,	true,	true,	IntVec2(33, 32),	IntVec2(33, 32),	IntVec2(33, 32), 0);
	CreateNewBlockDef(4,  "water",			true,	false,	true,	IntVec2(32, 44),	IntVec2(32, 44),	IntVec2(32, 44), 0);
	CreateNewBlockDef(5,  "cobblestone",	true,	true,	true,	IntVec2(34, 32),	IntVec2(34, 32),	IntVec2(34, 32), 0);
	CreateNewBlockDef(6,  "coal",			true,	true,	true,	IntVec2(63, 34),	IntVec2(63, 34),	IntVec2(63, 34), 0);
	CreateNewBlockDef(7,  "iron",			true,	true,	true,	IntVec2(63, 35),	IntVec2(63, 35),	IntVec2(63, 35), 0);
	CreateNewBlockDef(8,  "gold",			true,	true,	true,	IntVec2(63, 36),	IntVec2(63, 36),	IntVec2(63, 36), 0);
	CreateNewBlockDef(9,  "diamond",		true,	true,	true,	IntVec2(63, 37),	IntVec2(63, 37),	IntVec2(63, 37), 0);
	CreateNewBlockDef(10, "glowstone",		true,	true,	true,	IntVec2(46, 34),	IntVec2(46, 34),	IntVec2(46, 34), 15);
	CreateNewBlockDef(11, "ice",			true,	true,	true,	IntVec2(36, 35),	IntVec2(36, 35),	IntVec2(36, 35), 0);
	CreateNewBlockDef(12, "sand",			true,	true,	true,	IntVec2(34, 34),	IntVec2(34, 34),	IntVec2(34, 34), 0);
	CreateNewBlockDef(13, "oakLog",			true,	true,	true,	IntVec2(38, 33),	IntVec2(36, 33),	IntVec2(38, 33), 0);
	CreateNewBlockDef(14, "oakLeaf",		true,	true,	true,	IntVec2(32, 35),	IntVec2(32, 35),	IntVec2(32, 35), 0);
	CreateNewBlockDef(15, "spruceLog",		true,	true,	true,	IntVec2(38, 33),	IntVec2(35, 33),	IntVec2(38, 33), 0);
	CreateNewBlockDef(16, "spruceLeaf",		true,	true,	true,	IntVec2(34, 35),	IntVec2(34, 35),	IntVec2(34, 35), 0);
	CreateNewBlockDef(17, "cactus",			true,	true,	true,	IntVec2(38, 36),	IntVec2(37, 36),	IntVec2(39, 36), 0);
	CreateNewBlockDef(18, "snowyGrass",		true,	true,	true,	IntVec2(36, 35),	IntVec2(33, 35),	IntVec2(32, 34), 0);
}

void BlockDefinition::CreateNewBlockDef(unsigned char id, std::string name, bool isVisible, bool isSolid, bool isOpaque, IntVec2 topUV, IntVec2 sideUV, IntVec2 bottomUV, unsigned char lightVal)
{
	static int index = 0;

	s_blockDefinitions[index].m_ID			= id;
	s_blockDefinitions[index].m_name		= name;
	s_blockDefinitions[index].m_isVisible	= isVisible;
	s_blockDefinitions[index].m_isSolid		= isSolid;
	s_blockDefinitions[index].m_isOpaque	= isOpaque;
	s_blockDefinitions[index].m_topUVs		= topUV;
	s_blockDefinitions[index].m_sideUVs		= sideUV;
	s_blockDefinitions[index].m_bottomUVs	= bottomUV;
	s_blockDefinitions[index].m_lightInfluence = lightVal;

	index++;
}

BlockDefinition BlockDefinition::GetBlockDefByID(unsigned char blockDefID)
{
	switch (blockDefID)
	{
	case 0:
		return s_blockDefinitions[0];
	case 1:
		return s_blockDefinitions[1];
	case 2:
		return s_blockDefinitions[2];
	case 3:
		return s_blockDefinitions[3];
	case 4:
		return s_blockDefinitions[4];
	case 5:
		return s_blockDefinitions[5];
	case 6:
		return s_blockDefinitions[6];
	case 7:
		return s_blockDefinitions[7];
	case 8:
		return s_blockDefinitions[8];
	case 9:
		return s_blockDefinitions[9];
	case 10:
		return s_blockDefinitions[10];
	case 11:
		return s_blockDefinitions[11];
	case 12:
		return s_blockDefinitions[12];
	case 13:
		return s_blockDefinitions[13];
	case 14:
		return s_blockDefinitions[14];
	case 15:
		return s_blockDefinitions[15];
	case 16:
		return s_blockDefinitions[16];
	case 17:
		return s_blockDefinitions[17];
	case 18:
		return s_blockDefinitions[18];
	default:
		break;
	}
	return BlockDefinition();
}
