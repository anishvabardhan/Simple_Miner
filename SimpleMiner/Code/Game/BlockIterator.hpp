#pragma once

#include "Engine/Math/Vec3.hpp"

class Chunk;
class Block;

struct BlockIterator
{
	Chunk*							m_chunk												= nullptr;
	int								m_blockIndex										= -1;
	
									BlockIterator()										= default;
	explicit 						BlockIterator(Chunk* ownerChunk, int blockIndex);
									~BlockIterator();

	Block*							GetBlock()											const;
	Vec3							GetWorldCenter()									const;

	BlockIterator					GetNorthNeighbor()									const;
	BlockIterator					GetSouthNeighbor()									const;
	BlockIterator					GetEastNeighbor()									const;
	BlockIterator					GetWestNeighbor()									const;
	BlockIterator					GetUpNeighbor()										const;
	BlockIterator					GetDownNeighbor()									const;
};