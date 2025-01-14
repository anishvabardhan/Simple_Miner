#pragma once

#include <map>
#include <vector>
#include <deque>

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/RaycastUtils.hpp"

#include "Game/GameCommon.hpp"
#include "Game/BlockIterator.hpp"

#include <set>

class Game;
class Chunk;
class World;
class Block;
class Player;
class Shader;
class Texture;
class SpriteSheet;
class ConstantBuffer;

struct GameRaycastResult3D
{
	RaycastResult3D m_raycast;
	BlockIterator m_impactedBlock;
	IntVec3 m_blockFace = IntVec3(-1, -1, -1);
};

struct WorldConstants
{
	Vec4		m_camWorldPos;		
	Vec4		m_skyColor;			
	Vec4		m_outdoorLightColor;
	Vec4		m_indoorLightColor;	
	float		m_fogStartDist;		
	float		m_fogEndDist;		
	float		m_fogMaxAlpha;		
	float		m_time;				
};

class ChunkGenerateJob : public Job
{
	//World* m_chunkWorld = nullptr;
	//IntVec2 m_chunkCoord;
public:
	Chunk* m_chunkToGenerate = nullptr;

	ChunkGenerateJob() = default;
	ChunkGenerateJob(Chunk* chunk) { m_chunkToGenerate = chunk; };
	~ChunkGenerateJob();

	virtual void Execute() override;
};

class World
{
public:
	std::map<IntVec2, Chunk*>		m_activeChunks;
	GameRaycastResult3D				m_raycastVsBlocks;
	std::vector<Chunk*>				m_dirtyChunks;
	std::vector<IntVec2>			m_activeChunkCoords;
	std::set<IntVec2>				m_chunkBeingGeneratedOrLoaded;
	std::vector<ChunkGenerateJob*>	m_jobList;
	int								m_jobListIndex				= 0;
	std::deque<BlockIterator>		m_dirtyBlockIters;
	Game*							m_game						= nullptr;
	Texture*						m_chunkTexture				= nullptr;
	Shader*							m_worldShader				= nullptr;
	ConstantBuffer*					m_worldFogConstants			= nullptr;
	SpriteSheet*					m_spriteSheet				= nullptr;
	unsigned int					m_worldSeed					= 0;
	int								m_chunkActivationRange		= 0;
	int								m_maxChunksRadiusX			= 0;
	int								m_maxChunksRadiusY			= 0;
	int								m_maxChunks					= 0;
	int								m_numChunks					= 0;
	int								m_numOfVerts				= 0;
	Rgba8							m_skyColor					= Rgba8::BLACK;
	Rgba8							m_outdoorLightColor			= Rgba8::BLACK;
	float							m_worldDay					= 0.0f;
	float							m_worldTimeScale			= 200.0f;
	unsigned int					m_placedBlockID				= 0;
	IntVec2							m_currentGeneratedChunkCoord		= IntVec2::ZERO;
public:
									World(Game* owner);
									~World();

	void							Update(float deltaseconds);
	void							Render() const;

	void							DeleteWorld();
	void							ActivateChunk(IntVec2 ChunkCoord);
	void							ActivateChunk(Chunk* chunkToActivate);
	void							DeactivateChunk(IntVec2 ChunkCoord);
	void							UpdateChunks(float deltaseconds);
	bool							ActivateNearestMissingChunkInRange();
	void							DeactivateFurthestChunkOutOfRange();
	void							PlaceBlock(unsigned char blockID);
	void							Dig();
	void							SetWorldConstants() const;
	void							ProcessDirtyLighting();
	void							MarkDirtyLightingInChunk(IntVec2 chunkCoord);
	void							ProcessNextDirtyLightBlock(BlockIterator blockIter);
	void							MarkDirtyLighting(BlockIterator blockIter);
	void							UndirtyAllBlocksInChunk();
	GameRaycastResult3D				RaycastVsBlocks(Vec3 const& start, Vec3 const& direction, float distance);

	void							RenderRaycastBlock() const;
	void							RequestNewChunk(IntVec2 chunkCoords);
	bool							CheckCloserChunk(Chunk* a, Chunk* b);
	void							GetDirtyChunks(std::vector<Chunk*>& dirtyChunks);
	void							ActivateChunkLighting(IntVec2 chunkCoord);
};

//bool								operator<(IntVec2 const& first, IntVec2 const& seceond);