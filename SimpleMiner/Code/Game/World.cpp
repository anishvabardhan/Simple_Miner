#include "Game/World.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"

#include "ThirdParty/Squirrel/SmoothNoise.hpp"

#include "Game/Chunk.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/BlockDefinition.hpp"

#include <algorithm>

World::World(Game* owner)
{
	m_game = owner;

	BlockDefinition::initializeBlockDefs();
	BlockTemplate::InitializeBlockTemplates();

	m_chunkTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Textures/BasicSprites_64x64.png");
	m_spriteSheet = new SpriteSheet(*m_chunkTexture, IntVec2(64, 64));
	m_spriteSheet->CalculateUVsOfSpriteSheet();

	m_worldShader = g_theRenderer->CreateShader("World", VertexType::PCU);

	m_worldFogConstants = g_theRenderer->CreateConstantBuffer(sizeof(WorldConstants));

	m_chunkActivationRange = g_gameConfigBlackboard.GetValue("chunkActivationDistance", 0);
	m_worldSeed = g_gameConfigBlackboard.GetValue("worldSeed", 0);

	if (m_worldSeed == 0)
	{
		RandomNumberGenerator rng = RandomNumberGenerator();
		m_worldSeed = rng.RollRandomIntInRange(1, 50);
	}

	m_maxChunksRadiusX = 1 + int( m_chunkActivationRange ) / CHUNK_SIZE_X;
	m_maxChunksRadiusY = 1 + int( m_chunkActivationRange ) / CHUNK_SIZE_Y;
	m_maxChunks = (2 * m_maxChunksRadiusX) * (2 * m_maxChunksRadiusY); 
}

World::~World()
{
	DELETE_PTR(m_spriteSheet);
	DELETE_PTR(m_worldShader);

	for (size_t i = 0; i < m_activeChunkCoords.size(); i++)
	{
		DeactivateChunk(m_activeChunkCoords[i]);
	}

	m_activeChunks.clear();

	for (int i = 0; i < m_jobList.size(); i++)
	{
		g_theJobSystem->RetrieveJob(m_jobList[i]);

		DELETE_PTR(m_jobList[i]);
	}
}

void World::Update(float deltaseconds)
{
	m_worldTimeScale = 250.0f;

	m_raycastVsBlocks = RaycastVsBlocks(m_game->m_player->m_position, m_game->m_player->m_orientationDegrees.GetAsMatrix_XFwd_YLeft_ZUp().GetIBasis3D(), 8.0f);
	
	UpdateChunks(deltaseconds);
	
	ProcessDirtyLighting();

	if (g_theInputSystem->WasKeyJustPressed(KEYCODE_F8))
	{
		DeleteWorld();
	}

	if (g_theInputSystem->WasKeyJustPressed('1'))
	{
		m_placedBlockID = 5;
	}

	if (g_theInputSystem->WasKeyJustPressed('y'))
	{
		m_worldTimeScale *= 50.0f;
	}

	if (g_theInputSystem->WasKeyJustPressed('2'))
	{
		m_placedBlockID = 10;
	}

	if (g_theInputSystem->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE))
	{
		PlaceBlock((unsigned char)m_placedBlockID);
	}

	if (g_theInputSystem->WasKeyJustPressed(KEYCODE_LEFT_MOUSE))
	{
		Dig();
	}

	m_worldDay += (deltaseconds * m_worldTimeScale * 10.0f) / (60.f * 60.f * 24.f);

	int x = (int)m_worldDay;

	float time = m_worldDay - float(x);

	float lightningPerlin = Compute1dPerlinNoise(time, 0.1f, 9);
	float lightningStrength = RangeMapClamped(lightningPerlin, 0.6f, 0.9f, 0.0f, 1.0f);

	if ((time >= 0.75f && time <= 1.0f) || (time >= 0.0f && time <= 0.25f))
	{
		m_skyColor = Rgba8(20, 20, 40, 255);
		m_outdoorLightColor = Rgba8(54, 69, 79, 255);
	}
	else if (time >= 0.25f && time <= 0.5f)
	{
		m_skyColor = Interpolate(m_skyColor, Rgba8(200, 230, 255, 255), deltaseconds);
		m_outdoorLightColor = Interpolate(m_outdoorLightColor, Rgba8::WHITE, deltaseconds);
	}
	else if (time >= 0.5f && time <= 0.75f)
	{
		m_skyColor = Interpolate(m_skyColor, Rgba8(20, 20, 40, 255), deltaseconds);
		m_outdoorLightColor = Interpolate(m_outdoorLightColor, Rgba8(54, 69, 79, 255), deltaseconds);
	}

	m_skyColor = Interpolate(m_skyColor, Rgba8::WHITE, lightningStrength);
	m_outdoorLightColor = Interpolate(m_outdoorLightColor, Rgba8::WHITE, lightningStrength);
}

void World::Render() const
{
	SetWorldConstants();

	g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE, Mat44(), Rgba8::WHITE);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetDepthMode(DepthMode::ENABLED);
	g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);

	g_theRenderer->BindShader(m_worldShader);
	g_theRenderer->BindTexture(0, m_chunkTexture);

	for (auto i : m_activeChunks)
	{
		if (i.second && i.second->m_northNeighbour && i.second->m_southNeighbour && i.second->m_eastNeighbour && i.second->m_westNeighbour)
		{
			i.second->Render();
		}
	}
	
	RenderRaycastBlock();
}

void World::DeleteWorld()
{
	for (size_t i = 0; i < m_activeChunkCoords.size(); i++)
	{
		DeactivateChunk(m_activeChunkCoords[i]);
		DELETE_PTR(m_activeChunks[m_activeChunkCoords[i]]);
	}

	m_activeChunks.clear();

	m_worldSeed = g_gameConfigBlackboard.GetValue("worldSeed", 0);

	if (m_worldSeed == 0)
	{
		RandomNumberGenerator rng = RandomNumberGenerator();
		m_worldSeed = rng.RollRandomIntInRange(1, 50);
	}
}

void World::ActivateChunk(IntVec2 ChunkCoord)
{
	/*Chunk* activeChunk = new Chunk(this, ChunkCoord);

	std::vector<unsigned char> savedChunkData;
	std::string fileName = Stringf("Saves/World_%u/Chunk(%i,%i).chunk",m_worldSeed, ChunkCoord.x, ChunkCoord.y);

	int error = -1;

	error = FileReadToBuffer(savedChunkData, fileName);

	if (error == 0)
	{
		int blockIndex = 0;

		for (size_t i = 8; i < savedChunkData.size(); i++)
		{
			unsigned char blockType = savedChunkData[i];
			int numOfBlocks = (int)savedChunkData[i + 1];

			for (int j = 0; j < numOfBlocks; j++)
			{
				activeChunk->m_blocks[blockIndex].m_blockID = blockType;
				blockIndex++;
			}

			i++;
		}
	}

	m_activeChunks[ChunkCoord] = activeChunk;

	m_activeChunkCoords.push_back(ChunkCoord);

	IntVec2 northCoord = ChunkCoord + IntVec2(0, 1);
	IntVec2 southCoord = ChunkCoord + IntVec2(0, -1);
	IntVec2 eastCoord = ChunkCoord + IntVec2(1, 0);
	IntVec2 westCoord = ChunkCoord + IntVec2(-1, 0);

	auto found = m_activeChunks.find(northCoord);

	if (found != m_activeChunks.end())
	{
		m_activeChunks[ChunkCoord]->m_northNeighbour = found->second;
	}

	found = m_activeChunks.find(southCoord);

	if (found != m_activeChunks.end())
	{
		m_activeChunks[ChunkCoord]->m_southNeighbour = found->second;
	}

	found = m_activeChunks.find(eastCoord);

	if (found != m_activeChunks.end())
	{
		m_activeChunks[ChunkCoord]->m_eastNeighbour = found->second;
	}

	found = m_activeChunks.find(westCoord);

	if (found != m_activeChunks.end())
	{
		m_activeChunks[ChunkCoord]->m_westNeighbour = found->second;
	}

	if(m_activeChunks[ChunkCoord]->m_northNeighbour) m_activeChunks[ChunkCoord]->m_northNeighbour->m_southNeighbour = m_activeChunks[ChunkCoord];
	if(m_activeChunks[ChunkCoord]->m_southNeighbour) m_activeChunks[ChunkCoord]->m_southNeighbour->m_northNeighbour = m_activeChunks[ChunkCoord];
	if(m_activeChunks[ChunkCoord]->m_eastNeighbour) m_activeChunks[ChunkCoord]->m_eastNeighbour->m_westNeighbour = m_activeChunks[ChunkCoord];
	if(m_activeChunks[ChunkCoord]->m_westNeighbour) m_activeChunks[ChunkCoord]->m_westNeighbour->m_eastNeighbour = m_activeChunks[ChunkCoord];*/

	/*for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
	{
		int x = i & CHUNK_MASK_X;
		int y = (i >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;

		if (x == 0 || x == CHUNK_SIZE_X - 1)
		{
			if (y == 0 || y == CHUNK_SIZE_Y - 1)
			{
				if (!BlockDefinition::s_blockDefinitions[m_activeChunks[ChunkCoord]->m_blocks[i].m_blockID].m_isOpaque)
				{
					BlockIterator* blockIter = new BlockIterator(m_activeChunks[ChunkCoord], i);

					MarkDirtyLighting(*blockIter);
				}
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--)
			{
				int index = m_activeChunks[ChunkCoord]->GetBlockIndex(IntVec3(x, y, z));

				if (BlockDefinition::s_blockDefinitions[m_activeChunks[ChunkCoord]->m_blocks[index].m_blockID].m_isOpaque)
				{
					break;
				}

				m_activeChunks[ChunkCoord]->m_blocks[index].SetIsBlockSky(true);
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--)
			{
				int index = m_activeChunks[ChunkCoord]->GetBlockIndex(IntVec3(x, y, z));

				if (BlockDefinition::s_blockDefinitions[m_activeChunks[ChunkCoord]->m_blocks[index].m_blockID].m_isOpaque)
				{
					break;
				}

				BlockIterator* blockIter = new BlockIterator(m_activeChunks[ChunkCoord], index);

				blockIter->GetBlock()->SetOutdoorLightInfluence(15);

				int nIndex = blockIter->GetNorthNeighbor().m_blockIndex;
				int sIndex = blockIter->GetSouthNeighbor().m_blockIndex;
				int eIndex = blockIter->GetEastNeighbor().m_blockIndex;
				int wIndex = blockIter->GetWestNeighbor().m_blockIndex;

				if (blockIter->GetNorthNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[ChunkCoord]->m_blocks[nIndex].m_blockID].m_isOpaque && !blockIter->GetNorthNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetNorthNeighbor());
					}
				}

				if (blockIter->GetSouthNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[ChunkCoord]->m_blocks[sIndex].m_blockID].m_isOpaque && !blockIter->GetSouthNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetSouthNeighbor());
					}
				}

				if (blockIter->GetEastNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[ChunkCoord]->m_blocks[eIndex].m_blockID].m_isOpaque && !blockIter->GetEastNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetEastNeighbor());
					}
				}

				if (blockIter->GetWestNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[ChunkCoord]->m_blocks[wIndex].m_blockID].m_isOpaque && !blockIter->GetWestNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetWestNeighbor());
					}
				}
			}
		}
	}

	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
	{
		if (m_activeChunks[ChunkCoord]->m_blocks[i].m_blockID == 10)
		{
			BlockIterator* iter = new BlockIterator(m_activeChunks[ChunkCoord], i);

			MarkDirtyLighting(*iter);
		}
	}*/
}

void World::ActivateChunk(Chunk* chunkToActivate)
{
	IntVec2 chunkCoords = chunkToActivate->m_coordinate;
	
	m_activeChunks[chunkCoords] = chunkToActivate;

	m_activeChunkCoords.push_back(chunkCoords);

	chunkToActivate->InitializeChunkNeighbors(this);

	ActivateChunkLighting(chunkCoords);
}

void World::DeactivateChunk(IntVec2 ChunkCoord)
{
	if (m_activeChunks[ChunkCoord])
	{
		delete m_activeChunks[ChunkCoord];
		m_activeChunks[ChunkCoord] = nullptr;
	}

	m_activeChunks.erase(ChunkCoord);

	int index = 0;

	for (int i = 0; i < m_activeChunkCoords.size(); i++)
	{
		if(m_activeChunkCoords[i] == ChunkCoord)
		{
			index = i;
			break;
		}
	}

	m_activeChunkCoords.erase(m_activeChunkCoords.begin() + index);

	m_numChunks--;
}

void World::UpdateChunks(float deltaseconds)
{
	bool didActivate = false;

	if (m_numChunks == m_maxChunks)
	{
		DeactivateFurthestChunkOutOfRange();
	}
	else if (m_numChunks < m_maxChunks)
	{
		didActivate = ActivateNearestMissingChunkInRange();
		
		if (didActivate)
		{
			m_numChunks++;
		}
		else if (!didActivate)
		{
			DeactivateFurthestChunkOutOfRange();
		}
	}

	m_dirtyChunks.clear();

	GetDirtyChunks(m_dirtyChunks);

	std::sort(m_dirtyChunks.begin(), m_dirtyChunks.end(), [this](Chunk* a, Chunk* b) 
		{

			AABB2 chunkBoundsA = AABB2(float(a->m_coordinate.x * CHUNK_SIZE_X), float(a->m_coordinate.y * CHUNK_SIZE_Y), float((a->m_coordinate.x + 1) * CHUNK_SIZE_X), float((a->m_coordinate.y + 1) * CHUNK_SIZE_Y));
			AABB2 chunkBoundsB = AABB2(float(b->m_coordinate.x * CHUNK_SIZE_X), float(b->m_coordinate.y * CHUNK_SIZE_Y), float((b->m_coordinate.x + 1) * CHUNK_SIZE_X), float((b->m_coordinate.y + 1) * CHUNK_SIZE_Y));

			float distanceToChunkSquaredA = GetDistanceSquared2D(chunkBoundsA.GetCenter(), Vec2(m_game->m_player->m_position.x, m_game->m_player->m_position.y));
			float distanceToChunkSquaredB = GetDistanceSquared2D(chunkBoundsB.GetCenter(), Vec2(m_game->m_player->m_position.x, m_game->m_player->m_position.y));

			return distanceToChunkSquaredA < distanceToChunkSquaredB;
		}
	);

	int rebuildIndex = 0;

	for (auto i : m_dirtyChunks)
	{
		if (rebuildIndex < 2)
		{
			i->Update(deltaseconds);
			rebuildIndex++;
		}
	}

	Job* retrieveJob = g_theJobSystem->RetrieveJob();

	ChunkGenerateJob* chunkGenerateJob = dynamic_cast<ChunkGenerateJob*>(retrieveJob);

	if (chunkGenerateJob)
	{
		chunkGenerateJob->m_chunkToGenerate->m_chunkState = ChunkState::ACTIVATING_GENERATE_COMPLETE;
		m_chunkBeingGeneratedOrLoaded.erase(chunkGenerateJob->m_chunkToGenerate->m_coordinate);
		ActivateChunk(chunkGenerateJob->m_chunkToGenerate);
	}
}

bool World::ActivateNearestMissingChunkInRange()
{
	bool foundMissingChunk = false;

	IntVec2 playerCoord = IntVec2(RoundDownToInt(m_game->m_player->m_position.x) / CHUNK_SIZE_X, RoundDownToInt(m_game->m_player->m_position.y) / CHUNK_SIZE_Y);
	IntVec2 closestChunkCoord;

	float closestDistance = FLT_MAX;

	for (int x = playerCoord.x - m_maxChunksRadiusX; x < playerCoord.x + m_maxChunksRadiusX; x++)
	{
		for (int y = playerCoord.y - m_maxChunksRadiusY; y < playerCoord.y + m_maxChunksRadiusY; y++)
		{
			AABB2 chunkBounds = AABB2(float(x * CHUNK_SIZE_X), float(y * CHUNK_SIZE_Y), float((x + 1) * CHUNK_SIZE_X), float((y + 1) * CHUNK_SIZE_Y));

			float distanceToChunkSquared = GetDistanceSquared2D(chunkBounds.GetCenter(), Vec2(m_game->m_player->m_position.x, m_game->m_player->m_position.y));

			if (distanceToChunkSquared < m_chunkActivationRange * m_chunkActivationRange && distanceToChunkSquared < closestDistance)
			{
				auto found = m_chunkBeingGeneratedOrLoaded.find(IntVec2(x, y));

				if (found != m_chunkBeingGeneratedOrLoaded.end()) continue;

				if (m_activeChunks.find(IntVec2(x, y)) == m_activeChunks.end())
				{
					foundMissingChunk = true;
					closestChunkCoord = IntVec2(x, y);
					closestDistance = distanceToChunkSquared;
				}
			}
		}
	}

	if (foundMissingChunk)
	{
		//ActivateChunk(closestChunkCoord);
		RequestNewChunk(closestChunkCoord);
		m_currentGeneratedChunkCoord = closestChunkCoord;
		return true;
	}

	return false;
}

void World::DeactivateFurthestChunkOutOfRange()
{
	bool farthestChunkFound = false;

	float farthestDistance = float(m_chunkActivationRange + CHUNK_SIZE_X + CHUNK_SIZE_Y);

	IntVec2 playerCoord = IntVec2(RoundDownToInt(m_game->m_player->m_position.x) / CHUNK_SIZE_X, RoundDownToInt(m_game->m_player->m_position.y) / CHUNK_SIZE_Y);
	IntVec2 farthestChunkCoord;

	for (auto i : m_activeChunks)
	{
		AABB2 chunkBounds = AABB2((float)(i.second->m_coordinate.x * CHUNK_SIZE_X), (float)(i.second->m_coordinate.y * CHUNK_SIZE_Y), float((i.second->m_coordinate.x + 1) * CHUNK_SIZE_X), float((i.second->m_coordinate.y + 1) * CHUNK_SIZE_Y));

		float distanceToChunkSquared = GetDistanceSquared2D(chunkBounds.GetCenter(), Vec2(m_game->m_player->m_position.x, m_game->m_player->m_position.y));

		if (distanceToChunkSquared >= farthestDistance * farthestDistance)
		{
			farthestDistance = distanceToChunkSquared;
			farthestChunkCoord = i.second->m_coordinate;
			farthestChunkFound = true;
		}
	}

	if (farthestChunkFound)
	{
		DeactivateChunk(farthestChunkCoord);
	}
}

void World::PlaceBlock(unsigned char blockID)
{
	if (m_raycastVsBlocks.m_raycast.m_didImpact)
	{
		Chunk* newBlockChunk = m_raycastVsBlocks.m_impactedBlock.m_chunk;

		BlockIterator blockIter = m_raycastVsBlocks.m_impactedBlock;

		if (m_raycastVsBlocks.m_blockFace == IntVec3::NORTH)
		{
			int index = blockIter.GetNorthNeighbor().m_blockIndex;

			newBlockChunk->m_blocks[index].SetType(blockID);

			if (!newBlockChunk->IsDirty())
			{
				newBlockChunk->m_isMeshDirty = true;
				BlockIterator* iter = new BlockIterator(newBlockChunk, index);
				MarkDirtyLighting(*iter);
				newBlockChunk->SaveChunk();
			}
		}

		if (m_raycastVsBlocks.m_blockFace == IntVec3::SOUTH)
		{
			int index = blockIter.GetSouthNeighbor().m_blockIndex;

			newBlockChunk->m_blocks[index].SetType(blockID);

			if (!newBlockChunk->IsDirty())
			{
				newBlockChunk->m_isMeshDirty = true;
				BlockIterator* iter = new BlockIterator(newBlockChunk, index);
				MarkDirtyLighting(*iter);
				newBlockChunk->SaveChunk();
			}
		}

		if (m_raycastVsBlocks.m_blockFace == IntVec3::EAST)
		{
			int index = blockIter.GetEastNeighbor().m_blockIndex;

			newBlockChunk->m_blocks[index].SetType(blockID);

			if (!newBlockChunk->IsDirty())
			{
				newBlockChunk->m_isMeshDirty = true;
				BlockIterator* iter = new BlockIterator(newBlockChunk, index);
				MarkDirtyLighting(*iter);
				newBlockChunk->SaveChunk();
			}
		}

		if (m_raycastVsBlocks.m_blockFace == IntVec3::WEST)
		{
			int index = blockIter.GetWestNeighbor().m_blockIndex;

			newBlockChunk->m_blocks[index].SetType(blockID);

			if (!newBlockChunk->IsDirty())
			{
				newBlockChunk->m_isMeshDirty = true;
				BlockIterator* iter = new BlockIterator(newBlockChunk, index);
				MarkDirtyLighting(*iter);
				newBlockChunk->SaveChunk();
			}
		}

		if (m_raycastVsBlocks.m_blockFace == IntVec3::UP)
		{
			int index = blockIter.GetUpNeighbor().m_blockIndex;

			newBlockChunk->m_blocks[index].SetType(blockID);

			if (!newBlockChunk->IsDirty())
			{
				newBlockChunk->m_isMeshDirty = true;
				BlockIterator* iter = new BlockIterator(newBlockChunk, index);
				MarkDirtyLighting(*iter);
				newBlockChunk->SaveChunk();
			}
		}

		if (m_raycastVsBlocks.m_blockFace == IntVec3::DOWN)
		{
			int index = blockIter.GetDownNeighbor().m_blockIndex;

			newBlockChunk->m_blocks[index].SetType(blockID);

			if (!newBlockChunk->IsDirty())
			{
				newBlockChunk->m_isMeshDirty = true;
				BlockIterator* iter = new BlockIterator(newBlockChunk, index);
				MarkDirtyLighting(*iter);
				newBlockChunk->SaveChunk();
			}
		}
	}
}

void World::Dig()
{
	if (m_raycastVsBlocks.m_raycast.m_didImpact)
	{
		Chunk* newBlockChunk = m_raycastVsBlocks.m_impactedBlock.m_chunk;

		newBlockChunk->m_blocks[m_raycastVsBlocks.m_impactedBlock.m_blockIndex].SetType(0);

		if (!newBlockChunk->IsDirty())
		{
			newBlockChunk->m_isMeshDirty = true;
			BlockIterator* iter = new BlockIterator(newBlockChunk, m_raycastVsBlocks.m_impactedBlock.m_blockIndex);
			MarkDirtyLighting(*iter);
			newBlockChunk->SaveChunk();
		}
	}
}

void World::SetWorldConstants() const
{
	int x = (int)m_worldDay;

	float time = m_worldDay - float(x);

	float skyColor[4];
	//m_skyColor = Rgba8(200, 230, 255, 255);
	m_skyColor.GetAsFloats(skyColor);

	float outdoorColor[4];
	m_outdoorLightColor.GetAsFloats(outdoorColor);

	float glowPerlin = Compute1dPerlinNoise(time, 0.01f);
	float glowStrength = RangeMapClamped(glowPerlin, -1.0f, 1.0f, 0.8f, 1.0f);

	WorldConstants world;
	world.m_camWorldPos = m_game->m_player->GetModelMatrix().GetTranslation4D();
	world.m_skyColor = Vec4(skyColor[0], skyColor[1], skyColor[2], skyColor[3])/*Vec4::ONE*/;
	world.m_outdoorLightColor = Vec4(outdoorColor[0], outdoorColor[1], outdoorColor[2], outdoorColor[3])/*Vec4::ONE*/;
	world.m_indoorLightColor = Vec4(1.0f * glowStrength, 0.9f * glowStrength, 0.8f * glowStrength, 1.0f)/*Vec4::ONE*/;
	world.m_fogStartDist = float(m_chunkActivationRange - 16) * 0.5f;
	world.m_fogEndDist = float(m_chunkActivationRange - 16);
	world.m_fogMaxAlpha = 1.0f;
	world.m_time = 1.0f;

	g_theRenderer->CopyCPUToGPU(&world, m_worldFogConstants->m_size, m_worldFogConstants);
	g_theRenderer->BindConstantBuffer(8, m_worldFogConstants, RootSig::DEFAULT_PIPELINE);
}

void World::ProcessDirtyLighting()
{
	while (!m_dirtyBlockIters.empty())
	{
		BlockIterator iter = m_dirtyBlockIters.front();
		m_dirtyBlockIters.pop_front();
		ProcessNextDirtyLightBlock(iter);
	}
}

void World::MarkDirtyLightingInChunk(IntVec2 chunkCoord)
{
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
	{
		int x = i & CHUNK_MASK_X;
		int y = (i >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;

		if (x == 0 || x == CHUNK_SIZE_X - 1)
		{
			if (y == 0 || y == CHUNK_SIZE_Y - 1)
			{
				if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[i].m_blockID].m_isOpaque)
				{
					BlockIterator* blockIter = new BlockIterator(m_activeChunks[chunkCoord], i);

					MarkDirtyLighting(*blockIter);
				}
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--)
			{
				int index = m_activeChunks[chunkCoord]->GetBlockIndex(IntVec3(x, y, z));

				if (BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[index].m_blockID].m_isOpaque)
				{
					break;
				}

				m_activeChunks[chunkCoord]->m_blocks[index].SetIsBlockSky(true);
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--)
			{
				int index = m_activeChunks[chunkCoord]->GetBlockIndex(IntVec3(x, y, z));

				if (BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[index].m_blockID].m_isOpaque)
				{
					break;
				}

				BlockIterator* blockIter = new BlockIterator(m_activeChunks[chunkCoord], index);

				blockIter->GetBlock()->SetOutdoorLightInfluence(15);

				int nIndex = blockIter->GetNorthNeighbor().m_blockIndex;
				int sIndex = blockIter->GetSouthNeighbor().m_blockIndex;
				int eIndex = blockIter->GetEastNeighbor().m_blockIndex;
				int wIndex = blockIter->GetWestNeighbor().m_blockIndex;

				if (blockIter->GetNorthNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[nIndex].m_blockID].m_isOpaque && !blockIter->GetNorthNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetNorthNeighbor());
					}
				}

				if (blockIter->GetSouthNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[sIndex].m_blockID].m_isOpaque && !blockIter->GetSouthNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetSouthNeighbor());
					}
				}

				if (blockIter->GetEastNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[eIndex].m_blockID].m_isOpaque && !blockIter->GetEastNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetEastNeighbor());
					}
				}

				if (blockIter->GetWestNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[wIndex].m_blockID].m_isOpaque && !blockIter->GetWestNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetWestNeighbor());
					}
				}
			}
		}
	}

	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
	{
		if (m_activeChunks[chunkCoord]->m_blocks[i].m_blockID == 10)
		{
			BlockIterator* iter = new BlockIterator(m_activeChunks[chunkCoord], i);

			MarkDirtyLighting(*iter);
		}
	}
}

void World::ProcessNextDirtyLightBlock(BlockIterator blockIter)
{
	blockIter.GetBlock()->SetIsBlockLightDirty(false);
	
	Block* currentBlock = blockIter.GetBlock();

	bool isLightInfluenceChanged = false;

	unsigned char highestIndoorLightInfluence = 0u;
	unsigned char highestOutdoorLightInfluence = 0u;

	unsigned char currentIndoorLightInfluence = currentBlock->GetIndoorLightInfluence();
	unsigned char currentOutdoorLightInfluence = currentBlock->GetOutdoorLightInfluence();

	if (!BlockDefinition::s_blockDefinitions[currentBlock->m_blockID].m_isOpaque)
	{
		if (blockIter.GetNorthNeighbor().m_chunk && blockIter.GetNorthNeighbor().GetBlock())
		{
			if (blockIter.GetNorthNeighbor().GetBlock()->GetIndoorLightInfluence() > highestIndoorLightInfluence)
			{
				highestIndoorLightInfluence = blockIter.GetNorthNeighbor().GetBlock()->GetIndoorLightInfluence();
			}
			if (blockIter.GetNorthNeighbor().GetBlock()->GetOutdoorLightInfluence() > highestOutdoorLightInfluence)
			{
				highestOutdoorLightInfluence = blockIter.GetNorthNeighbor().GetBlock()->GetOutdoorLightInfluence();
			}
		}

		if (blockIter.GetSouthNeighbor().m_chunk && blockIter.GetSouthNeighbor().GetBlock())
		{
			if (blockIter.GetSouthNeighbor().GetBlock()->GetIndoorLightInfluence() > highestIndoorLightInfluence)
			{
				highestIndoorLightInfluence = blockIter.GetSouthNeighbor().GetBlock()->GetIndoorLightInfluence();
			}
			if (blockIter.GetSouthNeighbor().GetBlock()->GetOutdoorLightInfluence() > highestOutdoorLightInfluence)
			{
				highestOutdoorLightInfluence = blockIter.GetSouthNeighbor().GetBlock()->GetOutdoorLightInfluence();
			}
		}

		if (blockIter.GetEastNeighbor().m_chunk && blockIter.GetEastNeighbor().GetBlock())
		{
			if (blockIter.GetEastNeighbor().GetBlock()->GetIndoorLightInfluence() > highestIndoorLightInfluence)
			{
				highestIndoorLightInfluence = blockIter.GetEastNeighbor().GetBlock()->GetIndoorLightInfluence();
			}
			if (blockIter.GetEastNeighbor().GetBlock()->GetOutdoorLightInfluence() > highestOutdoorLightInfluence)
			{
				highestOutdoorLightInfluence = blockIter.GetEastNeighbor().GetBlock()->GetOutdoorLightInfluence();
			}
		}

		if (blockIter.GetWestNeighbor().m_chunk && blockIter.GetWestNeighbor().GetBlock())
		{
			if (blockIter.GetWestNeighbor().GetBlock()->GetIndoorLightInfluence() > highestIndoorLightInfluence)
			{
				highestIndoorLightInfluence = blockIter.GetWestNeighbor().GetBlock()->GetIndoorLightInfluence();
			}
			if (blockIter.GetWestNeighbor().GetBlock()->GetOutdoorLightInfluence() > highestOutdoorLightInfluence)
			{
				highestOutdoorLightInfluence = blockIter.GetWestNeighbor().GetBlock()->GetOutdoorLightInfluence();
			}
		}

		if (blockIter.GetUpNeighbor().m_chunk && blockIter.GetUpNeighbor().GetBlock())
		{
			if (blockIter.GetUpNeighbor().GetBlock()->GetIndoorLightInfluence() > highestIndoorLightInfluence)
			{
				highestIndoorLightInfluence = blockIter.GetUpNeighbor().GetBlock()->GetIndoorLightInfluence();
			}
			if (blockIter.GetUpNeighbor().GetBlock()->GetOutdoorLightInfluence() > highestOutdoorLightInfluence)
			{
				highestOutdoorLightInfluence = blockIter.GetUpNeighbor().GetBlock()->GetOutdoorLightInfluence();
			}
		}

		if (blockIter.GetDownNeighbor().m_chunk && blockIter.GetDownNeighbor().GetBlock())
		{
			if (blockIter.GetDownNeighbor().GetBlock()->GetIndoorLightInfluence() > highestIndoorLightInfluence)
			{
				highestIndoorLightInfluence = blockIter.GetDownNeighbor().GetBlock()->GetIndoorLightInfluence();
			}
			if (blockIter.GetDownNeighbor().GetBlock()->GetOutdoorLightInfluence() > highestOutdoorLightInfluence)
			{
				highestOutdoorLightInfluence = blockIter.GetDownNeighbor().GetBlock()->GetOutdoorLightInfluence();
			}
		}
	}

	unsigned char theoreticalIndoorLightInfluence = 0u;
	unsigned char theoreticalOutdoorLightInfluence = 0u;

	if (!BlockDefinition::s_blockDefinitions[currentBlock->m_blockID].m_isOpaque)
	{
		if (highestIndoorLightInfluence > 0)
		{
			theoreticalIndoorLightInfluence = highestIndoorLightInfluence - 1;
		}

		if (highestOutdoorLightInfluence > 0)
		{
			theoreticalOutdoorLightInfluence = highestOutdoorLightInfluence - 1;
		}
	}
	else
	{
		if (BlockDefinition::s_blockDefinitions[currentBlock->m_blockID].m_lightInfluence == 0)
		{
			isLightInfluenceChanged = true;
		}
	}

	if (currentBlock->IsBlockSky())
	{
		theoreticalOutdoorLightInfluence = 15u;
	}

	if (currentBlock->m_blockID == 10)
	{
		theoreticalIndoorLightInfluence = 15u;
	}

	if (theoreticalIndoorLightInfluence != currentIndoorLightInfluence)
	{
		if (BlockDefinition::s_blockDefinitions[currentBlock->m_blockID].m_lightInfluence > theoreticalIndoorLightInfluence)
		{
			theoreticalIndoorLightInfluence = BlockDefinition::s_blockDefinitions[currentBlock->m_blockID].m_lightInfluence;
		}
		
		blockIter.GetBlock()->SetIndoorLightInfluence(theoreticalIndoorLightInfluence);
		isLightInfluenceChanged = true;
	}

	if (isLightInfluenceChanged)
	{
		if (blockIter.GetNorthNeighbor().m_chunk)
		{
			blockIter.GetNorthNeighbor().m_chunk->m_isMeshDirty = true;
			if (!BlockDefinition::s_blockDefinitions[blockIter.GetNorthNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetNorthNeighbor());
			}
		}

		if (blockIter.GetSouthNeighbor().m_chunk)
		{
			blockIter.GetSouthNeighbor().m_chunk->m_isMeshDirty = true;
			if (!BlockDefinition::s_blockDefinitions[blockIter.GetSouthNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetSouthNeighbor());
			}
		}

		if (blockIter.GetEastNeighbor().m_chunk)
		{
			blockIter.GetEastNeighbor().m_chunk->m_isMeshDirty = true;
			if (!BlockDefinition::s_blockDefinitions[blockIter.GetEastNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetEastNeighbor());
			}
		}

		if (blockIter.GetWestNeighbor().m_chunk)
		{
			blockIter.GetWestNeighbor().m_chunk->m_isMeshDirty = true;
			if (!BlockDefinition::s_blockDefinitions[blockIter.GetWestNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetWestNeighbor());
			}
		}

		if (blockIter.GetUpNeighbor().m_chunk)
		{
			blockIter.GetUpNeighbor().m_chunk->m_isMeshDirty = true;
			if (!BlockDefinition::s_blockDefinitions[blockIter.GetUpNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetUpNeighbor());
			}
		}

		if (blockIter.GetDownNeighbor().m_chunk)
		{
			blockIter.GetDownNeighbor().m_chunk->m_isMeshDirty = true;
			if (!BlockDefinition::s_blockDefinitions[blockIter.GetDownNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetDownNeighbor());
			}
		}
	}

	if (theoreticalOutdoorLightInfluence != currentOutdoorLightInfluence)
	{
		blockIter.GetBlock()->SetOutdoorLightInfluence(theoreticalOutdoorLightInfluence);

		if (blockIter.GetNorthNeighbor().m_chunk)
		{
			blockIter.GetNorthNeighbor().m_chunk->m_isMeshDirty = true;

			if (!BlockDefinition::s_blockDefinitions[blockIter.GetNorthNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetNorthNeighbor());
			}
		}

		if (blockIter.GetSouthNeighbor().m_chunk)
		{
			blockIter.GetSouthNeighbor().m_chunk->m_isMeshDirty = true;

			if (!BlockDefinition::s_blockDefinitions[blockIter.GetSouthNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetSouthNeighbor());
			}
		}

		if (blockIter.GetEastNeighbor().m_chunk)
		{
			blockIter.GetEastNeighbor().m_chunk->m_isMeshDirty = true;

			if (!BlockDefinition::s_blockDefinitions[blockIter.GetEastNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetEastNeighbor());
			}
		}

		if (blockIter.GetWestNeighbor().m_chunk)
		{
			blockIter.GetWestNeighbor().m_chunk->m_isMeshDirty = true;

			if (!BlockDefinition::s_blockDefinitions[blockIter.GetWestNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetWestNeighbor());
			}
		}

		if (blockIter.GetUpNeighbor().m_chunk)
		{
			blockIter.GetUpNeighbor().m_chunk->m_isMeshDirty = true;

			if (!BlockDefinition::s_blockDefinitions[blockIter.GetUpNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetUpNeighbor());
			}
		}

		if (blockIter.GetDownNeighbor().m_chunk)
		{
			blockIter.GetDownNeighbor().m_chunk->m_isMeshDirty = true;

			if (!BlockDefinition::s_blockDefinitions[blockIter.GetDownNeighbor().GetBlock()->m_blockID].m_isOpaque)
			{
				MarkDirtyLighting(blockIter.GetDownNeighbor());
			}
		}
	}
}

void World::MarkDirtyLighting(BlockIterator blockIter)
{
	Block* block = blockIter.GetBlock();

	if(!block)
		return;

	if (!block->IsBlockLightDirty())
	{
		blockIter.GetBlock()->SetIsBlockLightDirty(true);
		m_dirtyBlockIters.push_back(blockIter);
	}
}

void World::UndirtyAllBlocksInChunk()
{
	while (!m_dirtyBlockIters.empty())
	{
		BlockIterator iter = m_dirtyBlockIters.front();
		m_dirtyBlockIters.pop_front();
		iter.GetBlock()->SetIsBlockLightDirty(false);
	}
}

GameRaycastResult3D World::RaycastVsBlocks(Vec3 const& start, Vec3 const& direction, float distance)
{
	GameRaycastResult3D ray;

	ray.m_raycast.m_rayStartPos = start;
	ray.m_raycast.m_rayFwdNormal = direction;
	ray.m_raycast.m_rayMaxLength = distance;
	ray.m_raycast.m_impactPos = start + (distance * direction);

	if(start.z > CHUNK_SIZE_Z)
		return ray;

	IntVec2 chunkLocalSpacePos;

	int blockX = RoundDownToInt(start.x);
	int blockY = RoundDownToInt(start.y);
	int blockZ = RoundDownToInt(start.z);

	int xSign = (blockX < 0 ? -1 : 0);
	int ySign = (blockY < 0 ? -1 : 0);

	chunkLocalSpacePos.x = blockX / CHUNK_SIZE_X;
	chunkLocalSpacePos.y = blockY / CHUNK_SIZE_Y;

	if (xSign < 0)
	{
		chunkLocalSpacePos.x -= 1;
	}

	if (ySign < 0)
	{
		chunkLocalSpacePos.y -= 1;
	}

	auto found = m_activeChunks.find(chunkLocalSpacePos);

	if (found == m_activeChunks.end())
	{
		return ray;
	}

	Chunk* rayChunk = m_activeChunks[chunkLocalSpacePos];

	IntVec2 chunkWorldSpacePos;

	chunkWorldSpacePos.x = chunkLocalSpacePos.x * CHUNK_SIZE_X;
	chunkWorldSpacePos.y = chunkLocalSpacePos.y * CHUNK_SIZE_Y;

	IntVec3 startBlockPos;

	startBlockPos.x = blockX - chunkWorldSpacePos.x;
	startBlockPos.y = blockY - chunkWorldSpacePos.y;
	startBlockPos.z = blockZ;

	int startBlockIndex = rayChunk->GetBlockIndex(startBlockPos);

	BlockIterator blockItr = BlockIterator(rayChunk, startBlockIndex);

	if(BlockDefinition::s_blockDefinitions[rayChunk->m_blocks[startBlockIndex].m_blockID].m_isSolid)
		return ray;

	float fwdDistPerXCrossing = 1.0f / fabsf(direction.x);
	float fwdDistPerYCrossing = 1.0f / fabsf(direction.y);
	float fwdDistPerZCrossing = 1.0f / fabsf(direction.z);

	int blockStepDirectionX = 0;
	int blockStepDirectionY = 0;
	int blockStepDirectionZ = 0;

	if (direction.x < 0)
		blockStepDirectionX = -1;
	else
		blockStepDirectionX = 1;

	if (direction.y < 0)
		blockStepDirectionY = -1;
	else
		blockStepDirectionY = 1;

	if (direction.z < 0)
		blockStepDirectionZ = -1;
	else
		blockStepDirectionZ = 1;

	float xAtFirstXCrossing = float(blockX + ((blockStepDirectionX + 1) / 2));
	float yAtFirstYCrossing = float(blockY + ((blockStepDirectionY + 1) / 2));
	float zAtFirstZCrossing = float(blockZ + ((blockStepDirectionZ + 1) / 2));

	float xDistToFirstXCrossing = xAtFirstXCrossing - start.x;
	float yDistToFirstYCrossing = yAtFirstYCrossing - start.y;
	float zDistToFirstZCrossing = zAtFirstZCrossing - start.z;

	float fwdDistAtNextXCrossing = fabsf(xDistToFirstXCrossing) * fwdDistPerXCrossing;
	float fwdDistAtNextYCrossing = fabsf(yDistToFirstYCrossing) * fwdDistPerYCrossing;
	float fwdDistAtNextZCrossing = fabsf(zDistToFirstZCrossing) * fwdDistPerZCrossing;

	while (true)
	{
		if (fwdDistAtNextZCrossing < fwdDistAtNextXCrossing && fwdDistAtNextZCrossing < fwdDistAtNextYCrossing)
		{
			if (fwdDistAtNextZCrossing > distance)
				return ray;

			if (blockStepDirectionZ > 0)
			{
				blockItr = blockItr.GetUpNeighbor();
			}
			else
			{
				blockItr = blockItr.GetDownNeighbor();
			}

			if (!blockItr.m_chunk)
			{
				ray.m_raycast.m_didImpact = false;
				return ray;
			}

			if (BlockDefinition::s_blockDefinitions[blockItr.GetBlock()->m_blockID].m_isSolid)
			{
				ray.m_raycast.m_didImpact = true;
				ray.m_raycast.m_impactDist = fwdDistAtNextZCrossing;
				ray.m_raycast.m_impactPos = start + (direction * fwdDistAtNextZCrossing);
				ray.m_raycast.m_impactNormal = -(float)blockStepDirectionZ * Vec3(0.0f, 0.0f, 1.0f);

				ray.m_impactedBlock = blockItr;

				//int x = blockItr.m_blockIndex & CHUNK_MASK_X;
				//int y = (blockItr.m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
				int z = (blockItr.m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

				if (ray.m_raycast.m_impactPos.z == (float)z)
				{
					ray.m_blockFace = IntVec3::DOWN;
				}
				else if (ray.m_raycast.m_impactPos.z == (float)(z + 1))
				{
					ray.m_blockFace = IntVec3::UP;
				}

				return ray;
			}
			else
			{
				fwdDistAtNextZCrossing += fwdDistPerZCrossing;
			}
		}
		else
		{
			if (fwdDistAtNextXCrossing < fwdDistAtNextYCrossing)
			{
				if (fwdDistAtNextXCrossing > distance)
					return ray;

				if (blockStepDirectionX > 0)
				{
					blockItr = blockItr.GetEastNeighbor();
				}
				else
				{
					blockItr = blockItr.GetWestNeighbor();
				}

				if (!blockItr.m_chunk)
				{
					ray.m_raycast.m_didImpact = false;
					return ray;
				}

				if (BlockDefinition::s_blockDefinitions[blockItr.GetBlock()->m_blockID].m_isSolid)
				{
					ray.m_raycast.m_didImpact = true;
					ray.m_raycast.m_impactDist = fwdDistAtNextXCrossing;
					ray.m_raycast.m_impactPos = start + (direction * fwdDistAtNextXCrossing);
					ray.m_raycast.m_impactNormal = -(float)blockStepDirectionX * Vec3(1.0f, 0.0f, 0.0f);

					ray.m_impactedBlock = blockItr;

					int x = blockItr.m_blockIndex & CHUNK_MASK_X;
					int y = (blockItr.m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
					//int z = (blockItr.m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

					IntVec2 chunkWorldPos;
					chunkWorldPos.x = blockItr.m_chunk->m_coordinate.x * CHUNK_SIZE_X;
					chunkWorldPos.y = blockItr.m_chunk->m_coordinate.y * CHUNK_SIZE_Y;

					x += chunkWorldPos.x;
					y += chunkWorldPos.y;

					if (ray.m_raycast.m_impactPos.x == (float)x)
					{
						ray.m_blockFace = IntVec3::WEST;
					}
					else if (ray.m_raycast.m_impactPos.x == (float)(x + 1))
					{
						ray.m_blockFace = IntVec3::EAST;
					}

					return ray;
				}
				else
				{
					fwdDistAtNextXCrossing += fwdDistPerXCrossing;
				}
			}
			else
			{
				if (fwdDistAtNextYCrossing > distance)
					return ray;

				if (blockStepDirectionY > 0)
				{
					blockItr = blockItr.GetNorthNeighbor();
				}
				else
				{
					blockItr = blockItr.GetSouthNeighbor();
				}

				if (!blockItr.m_chunk)
				{
					ray.m_raycast.m_didImpact = false;
					return ray;
				}

				if (BlockDefinition::s_blockDefinitions[blockItr.GetBlock()->m_blockID].m_isSolid)
				{
					ray.m_raycast.m_didImpact = true;
					ray.m_raycast.m_impactDist = fwdDistAtNextYCrossing;
					ray.m_raycast.m_impactPos = start + (direction * fwdDistAtNextYCrossing);
					ray.m_raycast.m_impactNormal = -(float)blockStepDirectionY * Vec3(0.0f, 1.0f, 0.0f);

					ray.m_impactedBlock = blockItr;

					int x = blockItr.m_blockIndex & CHUNK_MASK_X;
					int y = (blockItr.m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;
					//int z = (blockItr.m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z;

					IntVec2 chunkWorldPos;
					chunkWorldPos.x = blockItr.m_chunk->m_coordinate.x * CHUNK_SIZE_X;
					chunkWorldPos.y = blockItr.m_chunk->m_coordinate.y * CHUNK_SIZE_Y;

					x += chunkWorldPos.x;
					y += chunkWorldPos.y;

					if (ray.m_raycast.m_impactPos.y == (float)y)
					{
						ray.m_blockFace = IntVec3::SOUTH;
					}
					else if (ray.m_raycast.m_impactPos.y == (float)(y + 1))
					{
						ray.m_blockFace = IntVec3::NORTH;
					}

					return ray;
				}
				else
				{
					fwdDistAtNextYCrossing += fwdDistPerYCrossing;
				}
			}
		}
	}

	return GameRaycastResult3D();
}

void World::RenderRaycastBlock() const
{
	if (m_raycastVsBlocks.m_raycast.m_didImpact)
	{
		std::vector<Vertex_PCU> verts;

		float x = float(m_raycastVsBlocks.m_impactedBlock.m_blockIndex & CHUNK_MASK_X);
		float y = float((m_raycastVsBlocks.m_impactedBlock.m_blockIndex >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y);
		float z = float((m_raycastVsBlocks.m_impactedBlock.m_blockIndex >> CHUNK_BITSHIFT_Z) & CHUNK_MASK_Z);

		x += (m_raycastVsBlocks.m_impactedBlock.m_chunk->m_coordinate.x * CHUNK_SIZE_X);
		y += (m_raycastVsBlocks.m_impactedBlock.m_chunk->m_coordinate.y * CHUNK_SIZE_Y);

		Vec3 bl;
		Vec3 br;
		Vec3 tr;
		Vec3 tl;

		if (m_raycastVsBlocks.m_blockFace == IntVec3::UP)
		{
			AABB3 bounds = AABB3(x, y, z + 1.05f, x + 1, y + 1, z + 1.05f);

			bl = bounds.m_mins;
			br = Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
			tr = bounds.m_maxs;
			tl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
		}
		if (m_raycastVsBlocks.m_blockFace == IntVec3::DOWN)
		{
			AABB3 bounds = AABB3(x, y + 1, z - 0.05f, x + 1, y, z - 0.05f);

			bl = bounds.m_mins;
			br = Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
			tr = bounds.m_maxs;
			tl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
		}
		if (m_raycastVsBlocks.m_blockFace == IntVec3::NORTH)
		{
			AABB3 bounds = AABB3(x + 1, y + 1.05f, z, x, y + 1.05f, z + 1);

			bl = bounds.m_mins;
			br = Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
			tr = bounds.m_maxs;
			tl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
		}
		if (m_raycastVsBlocks.m_blockFace == IntVec3::SOUTH)
		{
			AABB3 bounds = AABB3(x, y - 0.05f, z, x + 1, y - 0.05f, z + 1);

			bl = bounds.m_mins;
			br = Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
			tr = bounds.m_maxs;
			tl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
		}
		if (m_raycastVsBlocks.m_blockFace == IntVec3::EAST)
		{
			AABB3 bounds = AABB3(x + 1.05f, y, z, x + 1.05f, y + 1, z + 1);

			bl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
			br = Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
			tr = Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
			tl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
		}
		if (m_raycastVsBlocks.m_blockFace == IntVec3::WEST)
		{
			AABB3 bounds = AABB3(x - 0.05f, y + 1, z, x - 0.05f, y, z + 1);

			bl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
			br = Vec3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
			tr = Vec3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
			tl = Vec3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
		}

		AddVertsForCylinder3D(verts, bl, br, 0.01f, Rgba8::MAGENTA, Rgba8::MAGENTA);
		AddVertsForCylinder3D(verts, br, tr, 0.01f, Rgba8::MAGENTA, Rgba8::MAGENTA);
		AddVertsForCylinder3D(verts, tr, tl, 0.01f, Rgba8::MAGENTA, Rgba8::MAGENTA);
		AddVertsForCylinder3D(verts, tl, bl, 0.01f, Rgba8::MAGENTA, Rgba8::MAGENTA);

		g_theRenderer->SetModelConstants(RootSig::DEFAULT_PIPELINE);
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);
		g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
		g_theRenderer->SetSamplerMode(SamplerMode::POINT_CLAMP);
		g_theRenderer->SetDepthMode(DepthMode::DISABLED);

		g_theRenderer->BindShader();
		g_theRenderer->BindTexture();

		g_theRenderer->DrawVertexArray((int)verts.size(), verts.data());
	}
}

void World::RequestNewChunk(IntVec2 chunkCoords)
{
	if (m_activeChunks.find(chunkCoords) == m_activeChunks.end())
	{
		Chunk* activeChunk = new Chunk(this, chunkCoords);

		std::vector<unsigned char> savedChunkData;
		std::string fileName = Stringf("Saves/World_%u/Chunk(%i,%i).chunk",m_worldSeed, chunkCoords.x, chunkCoords.y);

		int error = -1;

		error = FileReadToBuffer(savedChunkData, fileName);

		if (error == 0)
		{
			int blockIndex = 0;

			for (size_t i = 8; i < savedChunkData.size(); i++)
			{
				unsigned char blockType = savedChunkData[i];
				int numOfBlocks = (int)savedChunkData[i + 1];

				for (int j = 0; j < numOfBlocks; j++)
				{
					activeChunk->m_blocks[blockIndex].m_blockID = blockType;
					blockIndex++;
				}

				i++;
			}

			ActivateChunk(activeChunk);

			/*m_activeChunks[chunkCoords] = activeChunk;

			m_activeChunkCoords.push_back(chunkCoords);

			IntVec2 northCoord = chunkCoords + IntVec2(0, 1);
			IntVec2 southCoord = chunkCoords + IntVec2(0, -1);
			IntVec2 eastCoord = chunkCoords + IntVec2(1, 0);
			IntVec2 westCoord = chunkCoords + IntVec2(-1, 0);

			auto found = m_activeChunks.find(northCoord);

			if (found != m_activeChunks.end())
			{
				m_activeChunks[chunkCoords]->m_northNeighbour = found->second;
			}

			found = m_activeChunks.find(southCoord);

			if (found != m_activeChunks.end())
			{
				m_activeChunks[chunkCoords]->m_southNeighbour = found->second;
			}

			found = m_activeChunks.find(eastCoord);

			if (found != m_activeChunks.end())
			{
				m_activeChunks[chunkCoords]->m_eastNeighbour = found->second;
			}

			found = m_activeChunks.find(westCoord);

			if (found != m_activeChunks.end())
			{
				m_activeChunks[chunkCoords]->m_westNeighbour = found->second;
			}

			if(m_activeChunks[chunkCoords]->m_northNeighbour) m_activeChunks[chunkCoords]->m_northNeighbour->m_southNeighbour = m_activeChunks[chunkCoords];
			if(m_activeChunks[chunkCoords]->m_southNeighbour) m_activeChunks[chunkCoords]->m_southNeighbour->m_northNeighbour = m_activeChunks[chunkCoords];
			if(m_activeChunks[chunkCoords]->m_eastNeighbour) m_activeChunks[chunkCoords]->m_eastNeighbour->m_westNeighbour = m_activeChunks[chunkCoords];
			if(m_activeChunks[chunkCoords]->m_westNeighbour) m_activeChunks[chunkCoords]->m_westNeighbour->m_eastNeighbour = m_activeChunks[chunkCoords];
		*/
			return;
		}

		if (g_theJobSystem->GetNumOfQueuedJobs() < 4)
		{
			ChunkGenerateJob* job = new ChunkGenerateJob(activeChunk);
			g_theJobSystem->AddJob(job);
			m_chunkBeingGeneratedOrLoaded.insert(chunkCoords);

			m_activeChunks[chunkCoords] = activeChunk;
		}
		else
		{
			delete activeChunk;
		}
	}
}

bool World::CheckCloserChunk(Chunk* a, Chunk* b)
{	
	AABB2 chunkBoundsA = AABB2(float(a->m_coordinate.x * CHUNK_SIZE_X), float(a->m_coordinate.y * CHUNK_SIZE_Y), float((a->m_coordinate.x + 1) * CHUNK_SIZE_X), float((a->m_coordinate.y + 1) * CHUNK_SIZE_Y));
	AABB2 chunkBoundsB = AABB2(float(b->m_coordinate.x * CHUNK_SIZE_X), float(b->m_coordinate.y * CHUNK_SIZE_Y), float((b->m_coordinate.x + 1) * CHUNK_SIZE_X), float((b->m_coordinate.y + 1) * CHUNK_SIZE_Y));

	float distanceToChunkSquaredA = GetDistanceSquared2D(chunkBoundsA.GetCenter(), Vec2(m_game->m_player->m_position.x, m_game->m_player->m_position.y));
	float distanceToChunkSquaredB = GetDistanceSquared2D(chunkBoundsB.GetCenter(), Vec2(m_game->m_player->m_position.x, m_game->m_player->m_position.y));

	return distanceToChunkSquaredA < distanceToChunkSquaredB;
}

void World::GetDirtyChunks(std::vector<Chunk*>& dirtyChunks)
{
	for (auto i : m_activeChunks)
	{
		if (i.second->m_isMeshDirty && i.second->m_northNeighbour && i.second->m_southNeighbour && i.second->m_eastNeighbour && i.second->m_westNeighbour)
		{
			dirtyChunks.push_back(i.second);
		}
	}
}

void World::ActivateChunkLighting(IntVec2 chunkCoord)
{
	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
	{
		int x = i & CHUNK_MASK_X;
		int y = (i >> CHUNK_BITSHIFT_Y) & CHUNK_MASK_Y;

		if (x == 0 || x == CHUNK_SIZE_X - 1)
		{
			if (y == 0 || y == CHUNK_SIZE_Y - 1)
			{
				if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[i].m_blockID].m_isOpaque)
				{
					BlockIterator* blockIter = new BlockIterator(m_activeChunks[chunkCoord], i);

					MarkDirtyLighting(*blockIter);
				}
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--)
			{
				int index = m_activeChunks[chunkCoord]->GetBlockIndex(IntVec3(x, y, z));

				if (BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[index].m_blockID].m_isOpaque)
				{
					break;
				}

				m_activeChunks[chunkCoord]->m_blocks[index].SetIsBlockSky(true);
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = CHUNK_SIZE_Z - 1; z >= 0; z--)
			{
				int index = m_activeChunks[chunkCoord]->GetBlockIndex(IntVec3(x, y, z));

				if (BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[index].m_blockID].m_isOpaque)
				{
					break;
				}

				BlockIterator* blockIter = new BlockIterator(m_activeChunks[chunkCoord], index);

				blockIter->GetBlock()->SetOutdoorLightInfluence(15);

				int nIndex = blockIter->GetNorthNeighbor().m_blockIndex;
				int sIndex = blockIter->GetSouthNeighbor().m_blockIndex;
				int eIndex = blockIter->GetEastNeighbor().m_blockIndex;
				int wIndex = blockIter->GetWestNeighbor().m_blockIndex;

				if (blockIter->GetNorthNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[nIndex].m_blockID].m_isOpaque && !blockIter->GetNorthNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetNorthNeighbor());
					}
				}

				if (blockIter->GetSouthNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[sIndex].m_blockID].m_isOpaque && !blockIter->GetSouthNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetSouthNeighbor());
					}
				}

				if (blockIter->GetEastNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[eIndex].m_blockID].m_isOpaque && !blockIter->GetEastNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetEastNeighbor());
					}
				}

				if (blockIter->GetWestNeighbor().GetBlock())
				{
					if (!BlockDefinition::s_blockDefinitions[m_activeChunks[chunkCoord]->m_blocks[wIndex].m_blockID].m_isOpaque && !blockIter->GetWestNeighbor().GetBlock()->IsBlockSky())
					{
						MarkDirtyLighting(blockIter->GetWestNeighbor());
					}
				}
			}
		}
	}

	for (int i = 0; i < CHUNK_TOTAL_SIZE; i++)
	{
		if (m_activeChunks[chunkCoord]->m_blocks[i].m_blockID == 10)
		{
			BlockIterator* iter = new BlockIterator(m_activeChunks[chunkCoord], i);

			MarkDirtyLighting(*iter);
		}
	}
}
//
//void World::QueueNewJob(IntVec2 chunkCoord)
//{
//	ChunkGenerateJob* job = new ChunkGenerateJob(this, chunkCoord);
//	m_jobList.push_back(job);
//	g_theJobSystem->AddJob(job);
//}

//bool operator<(IntVec2 const& first, IntVec2 const& seceond)
//{
//	if(first.y < seceond.y)
//		return true;
//	else if(first.y > seceond.y)
//		return false;
//	
//	return (first.x < seceond.x);
//}
//
//ChunkGenerateJob::ChunkGenerateJob(World* chunkWorld, IntVec2 chunkCoord)
//{
//}

ChunkGenerateJob::~ChunkGenerateJob()
{
}

void ChunkGenerateJob::Execute()
{
	if (m_chunkToGenerate)
	{
		m_chunkToGenerate->GenerateBlocks();
		m_chunkToGenerate->m_chunkState = ChunkState::ACTIVATING_GENERATE_COMPLETE;
		m_status = JobStatus::COMPLETED;
	}
}
