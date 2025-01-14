#pragma once

#include "Engine/Math/IntVec3.hpp"

#include <vector>

constexpr unsigned int BLOCK_INDOOR_BITSHIFT = 0;
constexpr unsigned int BLOCK_OUTDOOR_BITSHIFT = 4;

constexpr unsigned int BLOCK_INDOOR_MASK = 15;
constexpr unsigned int BLOCK_OUTDOOR_MASK = 15;
		   
constexpr unsigned int BLOCK_BIT_IS_SKY = 1;
constexpr unsigned int BLOCK_BIT_IS_LIGHT_DIRTY = 2;
constexpr unsigned int BLOCK_BIT_IS_OPAQUE = 4;

struct BlockTemplateEntry
{
	unsigned char m_blockID = 0u;
	IntVec3 m_offset;

	BlockTemplateEntry(unsigned char blockID, IntVec3 offset);
};

struct BlockTemplate
{
	std::vector<BlockTemplateEntry> m_blockTemplate;

	static BlockTemplate s_blockTemplates[3];

	static void InitializeBlockTemplates();
};

class Block
{
public:
	unsigned char m_blockID = 0u;
	unsigned char m_lightInfluenceData = 0;
	unsigned char m_blockBitFlag = 0u;
public:
	Block();
	~Block();

	void SetType(unsigned char blockID);

	unsigned char GetIndoorLightInfluence() const;
	unsigned char GetOutdoorLightInfluence() const;

	void SetIndoorLightInfluence(unsigned char influnceValue);
	void SetOutdoorLightInfluence(unsigned char influnceValue);

	bool IsBlockSky() const;
	bool IsBlockLightDirty() const;
	bool IsBlockOpaque() const;

	void SetIsBlockSky(bool isSky);
	void SetIsBlockLightDirty(bool isLightDirty);
};