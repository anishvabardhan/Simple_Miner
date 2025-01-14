#include "Game/Chunk.hpp"

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

#include "ThirdParty/Squirrel/SmoothNoise.hpp"
#include "ThirdParty/Squirrel/RawNoise.hpp"

#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Game/BlockDefinition.hpp"

Chunk::Chunk(World* owner, IntVec2 coord)
{
	m_chunkWorld = owner;
	m_coordinate = coord;

	//RandomNumberGenerator random = RandomNumberGenerator();

	//unsigned char air		= 0;
	//unsigned char grass		= 1;	
	//unsigned char dirt		= 2;	
	//unsigned char stone		= 3;	
	//unsigned char water		= 4;	
	//unsigned char coal		= 6;	
	//unsigned char iron		= 7;	
	//unsigned char gold		= 8;	
	//unsigned char diamond	= 9;
	//unsigned char ice		= 11;
	//unsigned char sand		= 12;
	//unsigned char snowyGrass = 18;

	//unsigned int humiditySeed = m_chunkWorld->m_worldSeed + 1;
	//unsigned int temperatureSeed = m_chunkWorld->m_worldSeed + 2;
	//unsigned int hillinessSeed = m_chunkWorld->m_worldSeed + 3;
	//unsigned int oceannessSeed = m_chunkWorld->m_worldSeed + 4;

	//for (int x = 0; x < CHUNK_SIZE_X; x++)
	//{
	//	for (int y = 0; y < CHUNK_SIZE_Y; y++)
	//	{
	//		int globalX = (x + (m_coordinate.x * CHUNK_SIZE_X));
	//		int globalY = (y + (m_coordinate.y * CHUNK_SIZE_Y));

	//		float humidity = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 8, 0.5f, 2.0f, true, humiditySeed);
	//		float temperature = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 7, 0.5f, 2.0f, true, temperatureSeed );
	//		float hilliness = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 8, 0.5f, 2.0f, true, hillinessSeed );
	//		float oceanness = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 8, 0.5f, 2.0f, true, oceannessSeed );

	//		hilliness = SmoothStep3(SmoothStep3(hilliness));
	//		oceanness = SmoothStep3(SmoothStep3(oceanness));

	//		int waterLevelZ = CHUNK_SIZE_Z / 2;
	//		int riverDepth = 5;
	//		int maxOceanDepth = 10;
	//		int riverBed = waterLevelZ - riverDepth;
	//		int maxHeightAboveWater = waterLevelZ + riverDepth;

	//		float terrainHeightNoise = fabsf(Compute2dPerlinNoise( float(globalX), float(globalY), 300.f, 6, 0.5f, 2.0f, 0 ));
	//		float theoreticalTerrainHeightAboveWater = terrainHeightNoise * maxHeightAboveWater;

	//		if (theoreticalTerrainHeightAboveWater >= riverDepth)
	//		{
	//			theoreticalTerrainHeightAboveWater *= hilliness;
	//		}

	//		int actualTerrainHeightZ = riverBed + RoundDownToInt(theoreticalTerrainHeightAboveWater);

	//		int oceanLoweringStrength = 0;

	//		if (oceanness > 0.75f)
	//		{
	//			oceanLoweringStrength = 1;
	//		}
	//		else if (oceanness < 0.5f)
	//		{
	//			oceanLoweringStrength = 0;
	//		}
	//		else
	//		{
	//			oceanLoweringStrength = (int)RangeMap(oceanness, 0.5f, 0.75f, 0.0f, 1.0f);
	//		}

	//		actualTerrainHeightZ -= (oceanLoweringStrength * maxOceanDepth);

	//		for (int z = 0; z < CHUNK_SIZE_Z; z++)
	//		{
	//			float grassHeight = random.RollRandomFloatInRange(3, 4);

	//			float chance = random.RollRandomFloatZeroToOne();

	//			int index = x | (y << CHUNK_BITSHIFT_Y) | (z << CHUNK_BITSHIFT_Z);

	//			if (z == actualTerrainHeightZ)
	//			{
	//				if (humidity < 0.4f)
	//				{
	//					if (temperature >= 0.4f)
	//					{
	//						if (z <= waterLevelZ + 1 && z >= waterLevelZ)
	//						{
	//							m_blocks[index].SetType(sand);
	//						}
	//						else
	//						{
	//							m_blocks[index].SetType(grass);
	//						}
	//					}
	//					else
	//					{
	//						m_blocks[index].SetType(snowyGrass);
	//					}
	//				}
	//				else
	//				{
	//					m_blocks[index].SetType(grass);
	//				}
	//			}
	//			else if (z > actualTerrainHeightZ)
	//			{
	//				if (z <= waterLevelZ)
	//				{
	//					if (temperature < 0.4f)
	//					{
	//						m_blocks[index].SetType(ice);
	//					}
	//					else
	//					{
	//						m_blocks[index].SetType(water);
	//					}
	//				}
	//				else
	//				{
	//					m_blocks[index].SetType(air);
	//				}
	//			}
	//			else if(z < actualTerrainHeightZ && z >= actualTerrainHeightZ - (int)grassHeight)
	//			{
	//				if (humidity < 0.4f)
	//				{
	//					if (z <= waterLevelZ + 3 && z >= waterLevelZ)
	//					{
	//						m_blocks[index].SetType(sand);
	//					}
	//					else
	//					{
	//						m_blocks[index].SetType(dirt);
	//					}
	//				}
	//				else
	//				{
	//					m_blocks[index].SetType(dirt);
	//				}
	//			}
	//			else
	//			{
	//				if (chance >= 0.045f && chance <= 0.055f)
	//				{
	//					m_blocks[index].SetType(coal);
	//				}
	//				else if (chance >= 0.015f && chance <= 0.025f)
	//				{
	//					m_blocks[index].SetType(iron);
	//				}
	//				else if (chance >= 0.0045f && chance <= 0.0055f)
	//				{
	//					m_blocks[index].SetType(gold);
	//				}
	//				else if (chance <= 0.0005f && chance <= 0.0015f)
	//				{
	//					m_blocks[index].SetType(diamond);
	//				}
	//				else
	//				{
	//					m_blocks[index].SetType(stone);
	//				}
	//			}
	//		}
	//	}
	//}

	//for (int x = 0; x < CHUNK_SIZE_X; x++)
	//{
	//	for (int y = 0; y < CHUNK_SIZE_Y; y++)
	//	{
	//		int globalX = (x + (m_coordinate.x * CHUNK_SIZE_X));
	//		int globalY = (y + (m_coordinate.y * CHUNK_SIZE_Y));
	//		
	//		float forestness = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 100.0f, 7, 0.5f, 2.0f, 0 );
	//		float temperature = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 7, 0.5f, 2.0f, true, temperatureSeed );
	//		float humidity = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 8, 0.5f, 2.0f, true, humiditySeed);

	//		FoliageSpawnAtThisPosition(IntVec2(globalX, globalY), forestness, temperature, humidity);
	//	}
	//}
}

Chunk::~Chunk()
{
	if (m_needsSaving)
	{
		std::vector<unsigned char> chunkData;

		chunkData.push_back('G');
		chunkData.push_back('C');
		chunkData.push_back('H');
		chunkData.push_back('K');
		chunkData.push_back(1);
		chunkData.push_back(CHUNK_BITS_X);
		chunkData.push_back(CHUNK_BITS_Y);
		chunkData.push_back(CHUNK_BITS_Z);

		int totalNumOfTypeBlocks = 0;
		unsigned char previousBlockType = m_blocks[0].m_blockID;

		for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
		{
			unsigned char currentBlockType = m_blocks[i].m_blockID;

			if (currentBlockType != previousBlockType || totalNumOfTypeBlocks == 255)
			{
				chunkData.push_back(previousBlockType);
				chunkData.push_back((unsigned char)totalNumOfTypeBlocks);

				totalNumOfTypeBlocks = 0;

				previousBlockType = currentBlockType;
			}

			totalNumOfTypeBlocks++;

			if (i == CHUNK_TOTAL_SIZE - 1)
			{
				chunkData.push_back(currentBlockType);
				chunkData.push_back((unsigned char)totalNumOfTypeBlocks);
			}
		}

		CreateFolder(Stringf("Saves/World_%u",m_chunkWorld->m_worldSeed));

		std::string filePath = Stringf("Saves/World_%u/Chunk(%i,%i).chunk",m_chunkWorld->m_worldSeed, m_coordinate.x, m_coordinate.y);

		WriteBufferToFile(chunkData, filePath);
	}

	m_northNeighbour = nullptr;
	m_southNeighbour = nullptr;
	m_eastNeighbour = nullptr;
	m_westNeighbour = nullptr;

	m_chunkWorld->m_numOfVerts -= 24;

	DELETE_PTR(m_gpuMesh);
}

void Chunk::Update(float deltaseconds)
{
	UNUSED(deltaseconds);

	BuildMesh();
}

void Chunk::Render() const
{
	if (m_gpuMesh)
	{
		g_theRenderer->DrawVertexBuffer(m_gpuMesh, m_chunkMeshSize, sizeof(Vertex_PCU));
	}
}

bool Chunk::IsDirty() const
{
	return m_isMeshDirty;
}

void Chunk::SaveChunk()
{
	m_needsSaving = true;
}

void Chunk::FoliageSpawnAtThisPosition(IntVec2 globalPos, float forestBiomeFactor, float temperatureBiomeFactor, float humidityBiomeFactor)
{
	float localMaxima = Get2dNoiseZeroToOne(globalPos.x, globalPos.y);

	float northNeighborMaxima			= Get2dNoiseZeroToOne(globalPos.x,     globalPos.y + 1);
	float southNeighborMaxima			= Get2dNoiseZeroToOne(globalPos.x,     globalPos.y - 1);
	float eastNeighborMaxima			= Get2dNoiseZeroToOne(globalPos.x + 1, globalPos.y    );
	float westNeighborMaxima			= Get2dNoiseZeroToOne(globalPos.x - 1, globalPos.y    );
	float northEastNeighborMaxima		= Get2dNoiseZeroToOne(globalPos.x + 1, globalPos.y + 1);
	float northWestNeighborMaxima		= Get2dNoiseZeroToOne(globalPos.x - 1, globalPos.y + 1);
	float southEastNeighborMaxima		= Get2dNoiseZeroToOne(globalPos.x + 1, globalPos.y - 1);
	float southWestNeighborMaxima		= Get2dNoiseZeroToOne(globalPos.x - 1, globalPos.y - 1);

	bool localVNorth					= localMaxima > northNeighborMaxima;
	bool localVSouth					= localMaxima > southNeighborMaxima;
	bool localVEast						= localMaxima > eastNeighborMaxima;
	bool localVWest						= localMaxima > westNeighborMaxima;
	bool localVNorthEast				= localMaxima > northEastNeighborMaxima;
	bool localVNorthWest				= localMaxima > northWestNeighborMaxima;
	bool localVSouthEast				= localMaxima > southEastNeighborMaxima;
	bool localVSouthWest				= localMaxima > southWestNeighborMaxima;

	if (localVNorth && localVSouth && localVEast && localVWest && localVNorthEast && localVNorthWest && localVSouthEast && localVSouthWest)
	{
		float treeMinimumThreshold = RangeMap(forestBiomeFactor, 0.6f, 1.0f, 1.0f, 0.85f);

		if (localMaxima > treeMinimumThreshold)
		{
			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--)
			{
				int localX = globalPos.x - (m_coordinate.x * CHUNK_SIZE_X);
				int localY = globalPos.y - (m_coordinate.y * CHUNK_SIZE_Y);

				int blockIndex = GetBlockIndex(IntVec3(localX, localY, z));

				BlockIterator blockIter = BlockIterator(this, blockIndex);

				if (BlockDefinition::s_blockDefinitions[blockIter.GetBlock()->m_blockID].m_isSolid && blockIter.GetBlock()->m_blockID == 12)
				{
					if (blockIter.GetUpNeighbor().GetBlock() && blockIter.GetUpNeighbor().GetBlock()->m_blockID == 0)
					{
						if (humidityBiomeFactor < 0.4f)
						{
							m_blocks[blockIter.GetUpNeighbor().m_blockIndex].SetType(17);

							for (int i = 1; i < (int)BlockTemplate::s_blockTemplates[2].m_blockTemplate.size(); i++)
							{
								BlockDefinition tempBlockDef = BlockDefinition::GetBlockDefByID(BlockTemplate::s_blockTemplates[2].m_blockTemplate[i].m_blockID);

								IntVec3 blockOffset = BlockTemplate::s_blockTemplates[2].m_blockTemplate[i].m_offset;

								int offsetBlockIndex = GetBlockIndex(IntVec3(localX + blockOffset.x, localY + blockOffset.y, z + blockOffset.z));
								
								if (offsetBlockIndex >= 0 && offsetBlockIndex < CHUNK_TOTAL_SIZE)
								{
									BlockIterator offsetBlockIter = BlockIterator(this, offsetBlockIndex);

									if (offsetBlockIter.GetBlock()->m_blockID == 0)
									{
										m_blocks[offsetBlockIter.m_blockIndex].SetType(17);
									}
								}
							}
						}
					}
				}

				if (BlockDefinition::s_blockDefinitions[blockIter.GetBlock()->m_blockID].m_isSolid && blockIter.GetBlock()->m_blockID == 1)
				{
					if (blockIter.GetUpNeighbor().GetBlock() && blockIter.GetUpNeighbor().GetBlock()->m_blockID == 0)
					{
						if (temperatureBiomeFactor > 0.3f)
						{
							m_blocks[blockIter.GetUpNeighbor().m_blockIndex].SetType(13);

							for (int i = 1; i < (int)BlockTemplate::s_blockTemplates[0].m_blockTemplate.size(); i++)
							{
								BlockDefinition tempBlockDef = BlockDefinition::GetBlockDefByID(BlockTemplate::s_blockTemplates[0].m_blockTemplate[i].m_blockID);

								IntVec3 blockOffset = BlockTemplate::s_blockTemplates[0].m_blockTemplate[i].m_offset;

								int offsetBlockIndex = GetBlockIndex(IntVec3(localX + blockOffset.x, localY + blockOffset.y, z + blockOffset.z));

								if (offsetBlockIndex >= 0 && offsetBlockIndex < CHUNK_TOTAL_SIZE)
								{
									BlockIterator offsetBlockIter = BlockIterator(this, offsetBlockIndex);

									if (offsetBlockIter.GetBlock()->m_blockID == 0)
									{
										m_blocks[offsetBlockIter.m_blockIndex].SetType(BlockTemplate::s_blockTemplates[0].m_blockTemplate[i].m_blockID);
									}
								}
							}
						}
						else
						{
							m_blocks[blockIter.GetUpNeighbor().m_blockIndex].SetType(15);

							for (int i = 1; i < (int)BlockTemplate::s_blockTemplates[1].m_blockTemplate.size(); i++)
							{
								BlockDefinition tempBlockDef = BlockDefinition::GetBlockDefByID(BlockTemplate::s_blockTemplates[1].m_blockTemplate[i].m_blockID);

								IntVec3 blockOffset = BlockTemplate::s_blockTemplates[1].m_blockTemplate[i].m_offset;

								int offsetBlockIndex = GetBlockIndex(IntVec3(localX + blockOffset.x, localY + blockOffset.y, z + blockOffset.z));

								if (offsetBlockIndex >= 0 && offsetBlockIndex < CHUNK_TOTAL_SIZE)
								{
									BlockIterator offsetBlockIter = BlockIterator(this, offsetBlockIndex);

									if (offsetBlockIter.GetBlock()->m_blockID == 0)
									{
										m_blocks[offsetBlockIter.m_blockIndex].SetType(BlockTemplate::s_blockTemplates[1].m_blockTemplate[i].m_blockID);
									}
								}
							}
						}
					}
				}

				if (BlockDefinition::s_blockDefinitions[blockIter.GetBlock()->m_blockID].m_isSolid && blockIter.GetBlock()->m_blockID == 18)
				{
					if (blockIter.GetUpNeighbor().GetBlock() && blockIter.GetUpNeighbor().GetBlock()->m_blockID == 0)
					{
						m_blocks[blockIter.GetUpNeighbor().m_blockIndex].SetType(15);

						for (int i = 1; i < (int)BlockTemplate::s_blockTemplates[1].m_blockTemplate.size(); i++)
						{
							BlockDefinition tempBlockDef = BlockDefinition::GetBlockDefByID(BlockTemplate::s_blockTemplates[1].m_blockTemplate[i].m_blockID);

							IntVec3 blockOffset = BlockTemplate::s_blockTemplates[1].m_blockTemplate[i].m_offset;

							int offsetBlockIndex = GetBlockIndex(IntVec3(localX + blockOffset.x, localY + blockOffset.y, z + blockOffset.z));

							if (offsetBlockIndex >= 0 && offsetBlockIndex < CHUNK_TOTAL_SIZE)
							{
								BlockIterator offsetBlockIter = BlockIterator(this, offsetBlockIndex);

								if (offsetBlockIter.GetBlock()->m_blockID == 0)
								{
									m_blocks[offsetBlockIter.m_blockIndex].SetType(BlockTemplate::s_blockTemplates[1].m_blockTemplate[i].m_blockID);
								}
							}
						}
					}
				}
			}
		}
	}
}

int Chunk::GetBlockIndex(IntVec3 blockCoord)
{
	return blockCoord.x | (blockCoord.y << CHUNK_BITSHIFT_Y) | (blockCoord.z << CHUNK_BITSHIFT_Z);
}

void Chunk::InitializeChunkNeighbors(World* world)
{
	IntVec2 northNeighborCoords = m_coordinate + IntVec2(0, 1);
	IntVec2 southNeighborCoords = m_coordinate + IntVec2(0, -1);
	IntVec2 eastNeighborCoords = m_coordinate + IntVec2(1, 0);
	IntVec2 westNeighborCoords = m_coordinate + IntVec2(-1, 0);

	auto northNeighborIt = world->m_activeChunks.find(northNeighborCoords);
	if (northNeighborIt != world->m_activeChunks.end())
	{
		m_northNeighbour = northNeighborIt->second;
		m_northNeighbour->m_southNeighbour = this;
	}

	auto southNeighborIt = world->m_activeChunks.find(southNeighborCoords);
	if (southNeighborIt != world->m_activeChunks.end())
	{
		m_southNeighbour = southNeighborIt->second;
		m_southNeighbour->m_northNeighbour = this;
	}

	auto eastNeighborIt = world->m_activeChunks.find(eastNeighborCoords);
	if (eastNeighborIt != world->m_activeChunks.end())
	{
		m_eastNeighbour = eastNeighborIt->second;
		m_eastNeighbour->m_westNeighbour = this;
	}

	auto westNeighborIt = world->m_activeChunks.find(westNeighborCoords);
	if (westNeighborIt != world->m_activeChunks.end())
	{
		m_westNeighbour = westNeighborIt->second;
		m_westNeighbour->m_eastNeighbour = this;
	}
}

void Chunk::GenerateBlocks()
{
	RandomNumberGenerator random = RandomNumberGenerator();

	unsigned char air		= 0;
	unsigned char grass		= 1;	
	unsigned char dirt		= 2;	
	unsigned char stone		= 3;	
	unsigned char water		= 4;	
	unsigned char coal		= 6;	
	unsigned char iron		= 7;	
	unsigned char gold		= 8;	
	unsigned char diamond	= 9;
	unsigned char ice		= 11;
	unsigned char sand		= 12;
	unsigned char snowyGrass = 18;

	unsigned int humiditySeed = m_chunkWorld->m_worldSeed + 1;
	unsigned int temperatureSeed = m_chunkWorld->m_worldSeed + 2;
	unsigned int hillinessSeed = m_chunkWorld->m_worldSeed + 3;
	unsigned int oceannessSeed = m_chunkWorld->m_worldSeed + 4;

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			int globalX = (x + (m_coordinate.x * CHUNK_SIZE_X));
			int globalY = (y + (m_coordinate.y * CHUNK_SIZE_Y));

			float humidity = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 8, 0.5f, 2.0f, true, humiditySeed);
			float temperature = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 7, 0.5f, 2.0f, true, temperatureSeed );
			float hilliness = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 8, 0.5f, 2.0f, true, hillinessSeed );
			float oceanness = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 8, 0.5f, 2.0f, true, oceannessSeed );

			hilliness = SmoothStep3(SmoothStep3(hilliness));
			oceanness = SmoothStep3(SmoothStep3(oceanness));

			int waterLevelZ = CHUNK_SIZE_Z / 2;
			int riverDepth = 5;
			int maxOceanDepth = 10;
			int riverBed = waterLevelZ - riverDepth;
			int maxHeightAboveWater = waterLevelZ + riverDepth;

			float terrainHeightNoise = fabsf(Compute2dPerlinNoise( float(globalX), float(globalY), 300.f, 6, 0.5f, 2.0f, 0 ));
			float theoreticalTerrainHeightAboveWater = terrainHeightNoise * maxHeightAboveWater;

			if (theoreticalTerrainHeightAboveWater >= riverDepth)
			{
				theoreticalTerrainHeightAboveWater *= hilliness;
			}

			int actualTerrainHeightZ = riverBed + RoundDownToInt(theoreticalTerrainHeightAboveWater);

			int oceanLoweringStrength = 0;

			if (oceanness > 0.75f)
			{
				oceanLoweringStrength = 1;
			}
			else if (oceanness < 0.5f)
			{
				oceanLoweringStrength = 0;
			}
			else
			{
				oceanLoweringStrength = (int)RangeMap(oceanness, 0.5f, 0.75f, 0.0f, 1.0f);
			}

			actualTerrainHeightZ -= (oceanLoweringStrength * maxOceanDepth);

			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				float grassHeight = random.RollRandomFloatInRange(3, 4);

				float chance = random.RollRandomFloatZeroToOne();

				int index = x | (y << CHUNK_BITSHIFT_Y) | (z << CHUNK_BITSHIFT_Z);

				if (z == actualTerrainHeightZ)
				{
					if (humidity < 0.4f)
					{
						if (temperature >= 0.4f)
						{
							if (z <= waterLevelZ + 1 && z >= waterLevelZ)
							{
								m_blocks[index].SetType(sand);
							}
							else
							{
								m_blocks[index].SetType(grass);
							}
						}
						else
						{
							m_blocks[index].SetType(snowyGrass);
						}
					}
					else
					{
						m_blocks[index].SetType(grass);
					}
				}
				else if (z > actualTerrainHeightZ)
				{
					if (z <= waterLevelZ)
					{
						if (temperature < 0.4f)
						{
							m_blocks[index].SetType(ice);
						}
						else
						{
							m_blocks[index].SetType(water);
						}
					}
					else
					{
						m_blocks[index].SetType(air);
					}
				}
				else if(z < actualTerrainHeightZ && z >= actualTerrainHeightZ - (int)grassHeight)
				{
					if (humidity < 0.4f)
					{
						if (z <= waterLevelZ + 3 && z >= waterLevelZ)
						{
							m_blocks[index].SetType(sand);
						}
						else
						{
							m_blocks[index].SetType(dirt);
						}
					}
					else
					{
						m_blocks[index].SetType(dirt);
					}
				}
				else
				{
					if (chance >= 0.045f && chance <= 0.055f)
					{
						m_blocks[index].SetType(coal);
					}
					else if (chance >= 0.015f && chance <= 0.025f)
					{
						m_blocks[index].SetType(iron);
					}
					else if (chance >= 0.0045f && chance <= 0.0055f)
					{
						m_blocks[index].SetType(gold);
					}
					else if (chance <= 0.0005f && chance <= 0.0015f)
					{
						m_blocks[index].SetType(diamond);
					}
					else
					{
						m_blocks[index].SetType(stone);
					}
				}
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			int globalX = (x + (m_coordinate.x * CHUNK_SIZE_X));
			int globalY = (y + (m_coordinate.y * CHUNK_SIZE_Y));

			float forestness = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 100.0f, 7, 0.5f, 2.0f, 0 );
			float temperature = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 7, 0.5f, 2.0f, true, temperatureSeed );
			float humidity = 0.5f + 0.5f * Compute2dPerlinNoise(float(globalX), float(globalY), 2000.0f, 8, 0.5f, 2.0f, true, humiditySeed);

			FoliageSpawnAtThisPosition(IntVec2(globalX, globalY), forestness, temperature, humidity);
		}
	}
}

void Chunk::BuildMesh()
{
	Vec2 uvMins, uvMaxs;

	AABB2 topUVs;
	AABB2 sideUVs;
	AABB2 bottomUVs;

	BlockDefinition blockDef;
	
	for (int index = 0; index < CHUNK_TOTAL_SIZE; index++)
	{
		int x = index & CHUNK_MASK_X;
		int y = (index >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
		int z = (index >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

		blockDef = BlockDefinition::GetBlockDefByID(m_blocks[index].m_blockID);

		if (blockDef.m_isVisible)
		{
			if (m_blocks[index].m_blockID == 17)
			{
				for (int i = 1; i < (int)BlockTemplate::s_blockTemplates[2].m_blockTemplate.size(); i++)
				{
					BlockDefinition tempBlockDef = BlockDefinition::GetBlockDefByID(BlockTemplate::s_blockTemplates[2].m_blockTemplate[i].m_blockID);

					IntVec3 blockOffset = BlockTemplate::s_blockTemplates[2].m_blockTemplate[i].m_offset;

					int uvTopIndex = tempBlockDef.m_topUVs.x + (tempBlockDef.m_topUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);
					int uvSideIndex = tempBlockDef.m_sideUVs.x + (tempBlockDef.m_sideUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);
					int uvBottomIndex = tempBlockDef.m_bottomUVs.x + (tempBlockDef.m_bottomUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvTopIndex);
					topUVs = AABB2(uvMins, uvMaxs);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvSideIndex);
					sideUVs = AABB2(uvMins, uvMaxs);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvBottomIndex);
					bottomUVs = AABB2(uvMins, uvMaxs);

					IntVec3 worldPos = IntVec3((x + blockOffset.x) + (m_coordinate.x * CHUNK_SIZE_X), (y + blockOffset.y) + (m_coordinate.y * CHUNK_SIZE_Y), (z + blockOffset.z));

					AddVertsForBlock(m_cpuMesh, worldPos, Rgba8::WHITE, topUVs, sideUVs, bottomUVs); 
				}
			}
			
			if (m_blocks[index].m_blockID == 13)
			{
				for (int i = 1; i < (int)BlockTemplate::s_blockTemplates[0].m_blockTemplate.size(); i++)
				{
					BlockDefinition tempBlockDef = BlockDefinition::GetBlockDefByID(BlockTemplate::s_blockTemplates[0].m_blockTemplate[i].m_blockID);

					IntVec3 blockOffset = BlockTemplate::s_blockTemplates[0].m_blockTemplate[i].m_offset;

					int uvTopIndex = tempBlockDef.m_topUVs.x + (tempBlockDef.m_topUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);
					int uvSideIndex = tempBlockDef.m_sideUVs.x + (tempBlockDef.m_sideUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);
					int uvBottomIndex = tempBlockDef.m_bottomUVs.x + (tempBlockDef.m_bottomUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvTopIndex);
					topUVs = AABB2(uvMins, uvMaxs);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvSideIndex);
					sideUVs = AABB2(uvMins, uvMaxs);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvBottomIndex);
					bottomUVs = AABB2(uvMins, uvMaxs);

					IntVec3 worldPos = IntVec3((x + blockOffset.x) + (m_coordinate.x * CHUNK_SIZE_X), (y + blockOffset.y) + (m_coordinate.y * CHUNK_SIZE_Y), (z + blockOffset.z));

					AddVertsForBlock(m_cpuMesh, worldPos, Rgba8::WHITE, topUVs, sideUVs, bottomUVs); 
				}
			}

			if (m_blocks[index].m_blockID == 15)
			{
				for (int i = 1; i < (int)BlockTemplate::s_blockTemplates[1].m_blockTemplate.size(); i++)
				{
					BlockDefinition tempBlockDef = BlockDefinition::GetBlockDefByID(BlockTemplate::s_blockTemplates[1].m_blockTemplate[i].m_blockID);

					IntVec3 blockOffset = BlockTemplate::s_blockTemplates[1].m_blockTemplate[i].m_offset;

					int uvTopIndex = tempBlockDef.m_topUVs.x + (tempBlockDef.m_topUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);
					int uvSideIndex = tempBlockDef.m_sideUVs.x + (tempBlockDef.m_sideUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);
					int uvBottomIndex = tempBlockDef.m_bottomUVs.x + (tempBlockDef.m_bottomUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvTopIndex);
					topUVs = AABB2(uvMins, uvMaxs);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvSideIndex);
					sideUVs = AABB2(uvMins, uvMaxs);

					m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvBottomIndex);
					bottomUVs = AABB2(uvMins, uvMaxs);

					IntVec3 worldPos = IntVec3((x + blockOffset.x) + (m_coordinate.x * CHUNK_SIZE_X), (y + blockOffset.y) + (m_coordinate.y * CHUNK_SIZE_Y), (z + blockOffset.z));

					AddVertsForBlock(m_cpuMesh, worldPos, Rgba8::WHITE, topUVs, sideUVs, bottomUVs); 
				}
			}

			{
				int uvTopIndex = blockDef.m_topUVs.x + (blockDef.m_topUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);
				int uvSideIndex = blockDef.m_sideUVs.x + (blockDef.m_sideUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);
				int uvBottomIndex = blockDef.m_bottomUVs.x + (blockDef.m_bottomUVs.y * m_chunkWorld->m_spriteSheet->GetSheetDimensions().x);

				m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvTopIndex);
				topUVs = AABB2(uvMins, uvMaxs);

				m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvSideIndex);
				sideUVs = AABB2(uvMins, uvMaxs);

				m_chunkWorld->m_spriteSheet->GetSpriteUVs(uvMins, uvMaxs, uvBottomIndex);
				bottomUVs = AABB2(uvMins, uvMaxs);

				IntVec3 worldPos = IntVec3(x + (m_coordinate.x * CHUNK_SIZE_X), y + (m_coordinate.y * CHUNK_SIZE_Y), z);

				AddVertsForBlock(m_cpuMesh, worldPos, Rgba8::WHITE, topUVs, sideUVs, bottomUVs); 
			}
		}
	}

	m_gpuMesh = g_theRenderer->CreateVertexBuffer(m_cpuMesh.size() * sizeof(Vertex_PCU));
	g_theRenderer->CopyCPUToGPU(m_cpuMesh.data(), m_cpuMesh.size() * sizeof(Vertex_PCU), m_gpuMesh);

	m_chunkMeshSize = (int)m_cpuMesh.size();
	m_cpuMesh.clear();
	m_isMeshDirty = false;
}

void Chunk::AddVertsForCactus()
{
}

void Chunk::AddVertsForOakTree()
{
}

void Chunk::AddVertsForBlock(std::vector<Vertex_PCU>& verts, IntVec3 worldPosition, Rgba8 color, AABB2 topUVs, AABB2 sideUVs, AABB2 bottomUVs)
{
	Vec3 worldUnitPos;
	worldUnitPos.x = (float)worldPosition.x;
	worldUnitPos.y = (float)worldPosition.y;
	worldUnitPos.z = (float)worldPosition.z;

	Vec3 bottomLeftNear		= Vec3(worldUnitPos.x,			worldUnitPos.y,			worldUnitPos.z);
	Vec3 bottomRightNear	= Vec3(worldUnitPos.x + 1.0f,	worldUnitPos.y,			worldUnitPos.z);
	Vec3 topLeftNear		= Vec3(worldUnitPos.x,			worldUnitPos.y,			worldUnitPos.z + 1.0f);
	Vec3 topRightNear		= Vec3(worldUnitPos.x + 1.0f,	worldUnitPos.y,			worldUnitPos.z + 1.0f);
	Vec3 bottomLeftFar		= Vec3(worldUnitPos.x,			worldUnitPos.y + 1.0f,	worldUnitPos.z);
	Vec3 bottomRightFar		= Vec3(worldUnitPos.x + 1.0f,	worldUnitPos.y + 1.0f,	worldUnitPos.z);
	Vec3 topLeftFar			= Vec3(worldUnitPos.x,			worldUnitPos.y + 1.0f,	worldUnitPos.z + 1.0f);
	Vec3 topRightFar		= Vec3(worldUnitPos.x + 1.0f,	worldUnitPos.y + 1.0f,	worldUnitPos.z + 1.0f);

	IntVec3 localUnitPos = IntVec3(worldPosition.x - (m_coordinate.x * CHUNK_SIZE_X), worldPosition.y - (m_coordinate.y * CHUNK_SIZE_Y), worldPosition.z);

	int blockIndex = GetBlockIndex(localUnitPos);

	if (blockIndex >= 0)
	{
		BlockIterator blockIter = BlockIterator(this, blockIndex);

		BlockIterator nIter = blockIter.GetNorthNeighbor();
		BlockIterator sIter = blockIter.GetSouthNeighbor();
		BlockIterator eIter = blockIter.GetEastNeighbor();
		BlockIterator wIter = blockIter.GetWestNeighbor();
		BlockIterator uIter = blockIter.GetUpNeighbor();
		BlockIterator dIter = blockIter.GetDownNeighbor();

		Rgba8 colour;

		if (blockIter.GetBlock()->m_blockID != 0)
		{
			if (nIter.GetBlock() && nIter.GetBlock()->m_blockID == 0) 
			{
				colour = Rgba8(unsigned char(255 * float((float)nIter.GetBlock()->GetOutdoorLightInfluence() / 15.0f)), unsigned char(255 * float((float)nIter.GetBlock()->GetIndoorLightInfluence() / 15.0f)), 127, 255);

				AddVertsForQuad3D(verts, bottomRightFar, bottomLeftFar, topLeftFar, topRightFar, colour, sideUVs);
				m_chunkWorld->m_numOfVerts += 4;
			}

			if (sIter.GetBlock() && sIter.GetBlock()->m_blockID == 0) 
			{
				colour = Rgba8(unsigned char(255 * float((float)sIter.GetBlock()->GetOutdoorLightInfluence() / 15.0f)), unsigned char(255 * float((float)sIter.GetBlock()->GetIndoorLightInfluence() / 15.0f)), 127, 255);

				AddVertsForQuad3D(verts, bottomLeftNear, bottomRightNear, topRightNear, topLeftNear, colour, sideUVs);
				m_chunkWorld->m_numOfVerts += 4;
			}

			if (eIter.GetBlock() && eIter.GetBlock()->m_blockID == 0) 
			{			
				colour = Rgba8(unsigned char(255 * float((float)eIter.GetBlock()->GetOutdoorLightInfluence() / 15.0f)), unsigned char(255 * float((float)eIter.GetBlock()->GetIndoorLightInfluence() / 15.0f)), 127, 255);

				AddVertsForQuad3D(verts, bottomRightNear, bottomRightFar, topRightFar, topRightNear, colour, sideUVs);
				m_chunkWorld->m_numOfVerts += 4;
			}

			if (wIter.GetBlock() && wIter.GetBlock()->m_blockID == 0) 
			{
				colour = Rgba8(unsigned char(255 * float((float)wIter.GetBlock()->GetOutdoorLightInfluence() / 15.0f)), unsigned char(255 * float((float)wIter.GetBlock()->GetIndoorLightInfluence() / 15.0f)), 127, 255);

				AddVertsForQuad3D(verts, bottomLeftFar, bottomLeftNear, topLeftNear, topLeftFar, colour, sideUVs);
				m_chunkWorld->m_numOfVerts += 4;
			}

			if (uIter.GetBlock() && uIter.GetBlock()->m_blockID == 0)
			{
				colour = Rgba8(unsigned char(255 * float((float)uIter.GetBlock()->GetOutdoorLightInfluence() / 15.0f)), unsigned char(255 * float((float)uIter.GetBlock()->GetIndoorLightInfluence() / 15.0f)), 127, 255);

				AddVertsForQuad3D(verts, topLeftNear, topRightNear, topRightFar, topLeftFar, colour, topUVs);
				m_chunkWorld->m_numOfVerts += 4;
			}

			if (dIter.GetBlock() && dIter.GetBlock()->m_blockID == 0)
			{
				colour = Rgba8(unsigned char(255 * float((float)dIter.GetBlock()->GetOutdoorLightInfluence() / 15.0f)), unsigned char(255 * float((float)dIter.GetBlock()->GetIndoorLightInfluence() / 15.0f)), 127, 255);

				AddVertsForQuad3D(verts, bottomLeftFar, bottomRightFar, bottomRightNear, bottomLeftNear, colour, sideUVs);
				m_chunkWorld->m_numOfVerts += 4;
			}
		}
	}

}
