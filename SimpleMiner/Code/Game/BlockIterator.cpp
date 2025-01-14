#include "Game/BlockIterator.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/IntVec2.hpp"

#include "Game/Chunk.hpp"
#include "Game/GameCommon.hpp"

BlockIterator::BlockIterator(Chunk* ownerChunk, int blockIndex)
{
	m_chunk = ownerChunk;
	m_blockIndex = blockIndex;
}

BlockIterator::~BlockIterator()
{
}

Block* BlockIterator::GetBlock() const
{
	if(!m_chunk)
		return nullptr;

	GUARANTEE_OR_DIE(m_blockIndex >= 0 && m_blockIndex <= CHUNK_TOTAL_SIZE - 1, "Block out of bounds!");

	return &m_chunk->m_blocks[m_blockIndex];
}

Vec3 BlockIterator::GetWorldCenter() const
{
	int x = m_blockIndex & CHUNK_MASK_X;
	int y = (m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
	int z = (m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

	IntVec2 chunkWorldPos;
	chunkWorldPos.x = m_chunk->m_coordinate.x * CHUNK_SIZE_X;
	chunkWorldPos.y = m_chunk->m_coordinate.y * CHUNK_SIZE_Y;

	IntVec3 blockWorldCoord;
	blockWorldCoord.x = chunkWorldPos.x + x;
	blockWorldCoord.y = chunkWorldPos.y + y;
	blockWorldCoord.z = z;

	Vec3 blockWorldCenter = Vec3((float)blockWorldCoord.x + 0.5f, (float)blockWorldCoord.y + 0.5f, (float)blockWorldCoord.z + 0.5f);

	return blockWorldCenter;
}

BlockIterator BlockIterator::GetNorthNeighbor() const
{
	BlockIterator blockItr;
	
	int x = m_blockIndex & CHUNK_MASK_X;
	int y = (m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
	int z = (m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

	if (y == CHUNK_SIZE_Y - 1)
	{
		int neighborBlockIndex = x | (0 << CHUNK_BITSHIFT_Y) | (z << CHUNK_BITSHIFT_Z);

		blockItr.m_chunk = m_chunk->m_northNeighbour;
		blockItr.m_blockIndex = neighborBlockIndex;
	}
	else
	{
		blockItr.m_chunk = m_chunk;
		blockItr.m_blockIndex = m_blockIndex + CHUNK_SIZE_X;
	}

	return blockItr;
}

BlockIterator BlockIterator::GetSouthNeighbor() const
{
	BlockIterator blockItr;

	int x = m_blockIndex & CHUNK_MASK_X;
	int y = (m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
	int z = (m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

	if (y == 0)
	{
		int neighborBlockIndex = x | ((CHUNK_SIZE_Y - 1) << CHUNK_BITSHIFT_Y) | (z << CHUNK_BITSHIFT_Z);

		blockItr.m_chunk = m_chunk->m_southNeighbour;
		blockItr.m_blockIndex = neighborBlockIndex;
	}
	else
	{
		blockItr.m_chunk = m_chunk;
		blockItr.m_blockIndex = m_blockIndex - CHUNK_SIZE_X;
	}

	return blockItr;
}

BlockIterator BlockIterator::GetEastNeighbor() const
{
	BlockIterator blockItr;

	int x = m_blockIndex & CHUNK_MASK_X;
	int y = (m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
	int z = (m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

	if (x == CHUNK_SIZE_X - 1)
	{
		int neighborBlockIndex = 0 | (y << CHUNK_BITSHIFT_Y) | (z << CHUNK_BITSHIFT_Z);

		blockItr.m_chunk = m_chunk->m_eastNeighbour;
		blockItr.m_blockIndex = neighborBlockIndex;
	}
	else
	{
		blockItr.m_chunk = m_chunk;
		blockItr.m_blockIndex = m_blockIndex + 1;
	}

	return blockItr;
}

BlockIterator BlockIterator::GetWestNeighbor() const
{
	BlockIterator blockItr;

	int x = m_blockIndex & CHUNK_MASK_X;
	int y = (m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
	int z = (m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

	if (x == 0)
	{
		int neighborBlockIndex = (CHUNK_SIZE_X - 1) | (y << CHUNK_BITSHIFT_Y) | (z << CHUNK_BITSHIFT_Z);

		blockItr.m_chunk = m_chunk->m_westNeighbour;
		blockItr.m_blockIndex = neighborBlockIndex;
	}
	else
	{
		blockItr.m_chunk = m_chunk;
		blockItr.m_blockIndex = m_blockIndex - 1;
	}

	return blockItr;
}

BlockIterator BlockIterator::GetUpNeighbor() const
{
	BlockIterator blockItr;

	int z = (m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

	if (z == CHUNK_SIZE_Z - 1)
	{
		blockItr.m_chunk = nullptr;
		blockItr.m_blockIndex = -1;
	}
	else
	{
		blockItr.m_chunk = m_chunk;
		blockItr.m_blockIndex = m_blockIndex + CHUNK_LAYER_SIZE;
	}

	return blockItr;
}

BlockIterator BlockIterator::GetDownNeighbor() const
{
	BlockIterator blockItr;

	//int x = m_blockIndex & CHUNK_MASK_X;
	//int y = (m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
	int z = (m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

	if (z == 0)
	{
		blockItr.m_chunk = nullptr;
		blockItr.m_blockIndex = -1;
	}
	else
	{
		blockItr.m_chunk = m_chunk;
		blockItr.m_blockIndex = m_blockIndex - CHUNK_LAYER_SIZE;
	}

	return blockItr;
}
