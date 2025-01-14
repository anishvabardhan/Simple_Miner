#include "Game/Block.hpp"

BlockTemplate BlockTemplate::s_blockTemplates[3];

BlockTemplateEntry::BlockTemplateEntry(unsigned char blockID, IntVec3 offset)
{
	m_blockID = blockID;
	m_offset = offset;
}

void BlockTemplate::InitializeBlockTemplates()
{
	// OAK TREE
	s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(13, IntVec3::ZERO));
	s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(13, IntVec3(0, 0, 1)));
	s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(13, IntVec3(0, 0, 2)));
	s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(13, IntVec3(0, 0, 3)));
	s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(13, IntVec3(0, 0, 4)));

	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1, -1, 2)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1,  0, 2)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1,  1, 2)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 0, -1, 2)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1, -1, 2)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1,  0, 2)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1,  1, 2)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 0,  1, 2)));
	//																  
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1, -1, 3)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1,  0, 3)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1,  1, 3)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 0, -1, 3)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1, -1, 3)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1,  0, 3)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1,  1, 3)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 0,  1, 3)));
	//																  
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1, -1, 4)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1,  0, 4)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3(-1,  1, 4)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 0, -1, 4)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1, -1, 4)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1,  0, 4)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 1,  1, 4)));
	//s_blockTemplates[0].m_blockTemplate.push_back(BlockTemplateEntry(14, IntVec3( 0,  1, 4)));

	// SPRUCE TREE
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(15, IntVec3::ZERO));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(15, IntVec3(0, 0, 1)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(15, IntVec3(0, 0, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(15, IntVec3(0, 0, 3)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(15, IntVec3(0, 0, 4)));
					 /*
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1, -1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1,  0, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1,  1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 0, -1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1, -1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1,  0, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1,  1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 0,  1, 2)));
					 												  
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 2, -1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 2,  0, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 2,  1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-2, -1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-2,  0, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-2,  1, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1,  2, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 0,  2, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1,  2, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1, -2, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 0, -2, 2)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1, -2, 2)));
					 												  
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1, -1, 3)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1,  0, 3)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1,  1, 3)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 0, -1, 3)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1, -1, 3)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1,  0, 3)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1,  1, 3)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 0,  1, 3)));
					 												  
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1, -1, 4)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1,  0, 4)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3(-1,  1, 4)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 0, -1, 4)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1, -1, 4)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1,  0, 4)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 1,  1, 4)));
	s_blockTemplates[1].m_blockTemplate.push_back(BlockTemplateEntry(16, IntVec3( 0,  1, 4)));*/

	// CACTUS
	s_blockTemplates[2].m_blockTemplate.push_back(BlockTemplateEntry(17, IntVec3::ZERO));
	s_blockTemplates[2].m_blockTemplate.push_back(BlockTemplateEntry(17, IntVec3(0, 0, 1)));
	s_blockTemplates[2].m_blockTemplate.push_back(BlockTemplateEntry(17, IntVec3(0, 0, 2)));
	s_blockTemplates[2].m_blockTemplate.push_back(BlockTemplateEntry(17, IntVec3(0, 0, 3)));
}

Block::Block()
{
}

Block::~Block()
{
}

void Block::SetType(unsigned char blockID)
{
	m_blockID = blockID;
}

unsigned char Block::GetIndoorLightInfluence() const
{
	return (m_lightInfluenceData & BLOCK_INDOOR_MASK);
}

unsigned char Block::GetOutdoorLightInfluence() const
{
	return (m_lightInfluenceData >> BLOCK_OUTDOOR_BITSHIFT) & BLOCK_OUTDOOR_MASK;
}

void Block::SetIndoorLightInfluence(unsigned char influnceValue)
{
	m_lightInfluenceData &= ~BLOCK_INDOOR_MASK;

	m_lightInfluenceData |= (influnceValue << BLOCK_INDOOR_BITSHIFT);
}

void Block::SetOutdoorLightInfluence(unsigned char influnceValue)
{
	m_lightInfluenceData &= ~(BLOCK_OUTDOOR_MASK << BLOCK_OUTDOOR_BITSHIFT);

	m_lightInfluenceData |= ((influnceValue << BLOCK_OUTDOOR_BITSHIFT) & (BLOCK_OUTDOOR_MASK << BLOCK_OUTDOOR_BITSHIFT));
}

bool Block::IsBlockSky() const
{
	return (m_blockBitFlag & BLOCK_BIT_IS_SKY) == BLOCK_BIT_IS_SKY;
}

bool Block::IsBlockLightDirty() const
{
	return (m_blockBitFlag & BLOCK_BIT_IS_LIGHT_DIRTY) == BLOCK_BIT_IS_LIGHT_DIRTY;
}

bool Block::IsBlockOpaque() const
{
	return false;
}

void Block::SetIsBlockSky(bool isSky)
{
	if (isSky)
	{
		m_blockBitFlag |= BLOCK_BIT_IS_SKY;
	}
	else
	{
		m_blockBitFlag &= ~BLOCK_BIT_IS_SKY;
	}
}

void Block::SetIsBlockLightDirty(bool isLightDirty)
{
	if (isLightDirty)
	{
		m_blockBitFlag |= BLOCK_BIT_IS_LIGHT_DIRTY;
	}
	else
	{
		m_blockBitFlag &= ~BLOCK_BIT_IS_LIGHT_DIRTY;
	}
}
