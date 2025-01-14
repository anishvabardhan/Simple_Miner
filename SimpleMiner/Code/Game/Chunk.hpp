#pragma once

#include <vector>
#include <mutex>

#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

#include "Game/Block.hpp"

class VertexBuffer;
class Texture;
class SpriteSheet;
class World;

constexpr int CHUNK_SIZE_X							= 16;
constexpr int CHUNK_SIZE_Y							= 16;
constexpr int CHUNK_SIZE_Z							= 128;

constexpr int CHUNK_BITS_X							= 4;
constexpr int CHUNK_BITS_Y							= 4;
constexpr int CHUNK_BITS_Z							= 7;

constexpr unsigned int CHUNK_LAYER_SIZE				= CHUNK_SIZE_X * CHUNK_SIZE_Y;
constexpr unsigned int CHUNK_TOTAL_SIZE				= CHUNK_LAYER_SIZE * CHUNK_SIZE_Z;

constexpr unsigned int CHUNK_MASK_X					= CHUNK_SIZE_X - 1;
constexpr unsigned int CHUNK_MASK_Y					= CHUNK_SIZE_Y - 1;
constexpr unsigned int CHUNK_MASK_Z					= CHUNK_SIZE_Z - 1;

constexpr unsigned int CHUNK_BITSHIFT_X				= 0;
constexpr unsigned int CHUNK_BITSHIFT_Y				= 4;
constexpr unsigned int CHUNK_BITSHIFT_Z				= 4 + 4;

enum ChunkState
{
	MISSING,
	ON_DISK,
	CONSTRUCTING,

	ACTIVATING_QUEUED_LOAD,
	ACTIVATING_LOADING,
	ACTIVATING_LOAD_COMPLETE,

	ACTIVATING_QUEUED_GENERATE,
	ACTIVATING_GENERATING,
	ACTIVATING_GENERATE_COMPLETE,

	ACTIVE,

	DEACTIVATING_QUEUED_SAVE,
	DEACTIVATING_SAVING,
	DEACTIVATING_SAVE_COMPLETE,
	DECONSTRUCTING,

	NUM_CHUNK_STATES
};

class Chunk
{
	AABB3						m_worldBounds		= AABB3(0.0f, 0.0f, 0.0f, CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);
	std::vector<Vertex_PCU>		m_cpuMesh;
	VertexBuffer*				m_gpuMesh			= nullptr;
	int							m_chunkMeshSize		= 0;
	bool						m_isDebugMode		= false;
	bool						m_isChunkActive		= true;
	bool						m_needsSaving		= false;
public:
	bool						m_isMeshDirty		= true;
	std::atomic<ChunkState>		m_chunkState		= ChunkState::MISSING;
	World*						m_chunkWorld		= nullptr;	
	Block						m_blocks[CHUNK_TOTAL_SIZE];
	IntVec2						m_coordinate		= IntVec2::ZERO;
	Chunk*						m_northNeighbour	= nullptr;
	Chunk*						m_southNeighbour	= nullptr;
	Chunk*						m_eastNeighbour		= nullptr;
	Chunk*						m_westNeighbour		= nullptr;
public:
								Chunk()				= default;
	explicit					Chunk(World* owner, IntVec2 coord);
								~Chunk();

	void						Update(float deltaseconds);
	void						Render() const;

	bool						IsDirty() const;
	void						SaveChunk();

	void						FoliageSpawnAtThisPosition(IntVec2 globalPos, float forestBiomeFactor, float temperatureBiomeFactor, float humidityBiomeFactor);

	int							GetBlockIndex(IntVec3 blockCoord);

	void						InitializeChunkNeighbors(World* world);
	void						GenerateBlocks();
	void						BuildMesh();
	void						AddVertsForCactus();
	void						AddVertsForOakTree();
	void						AddVertsForBlock(std::vector<Vertex_PCU>& verts, IntVec3 worldPosition, Rgba8 color, AABB2 topUVs, AABB2 sideUVs, AABB2 bottomUVs);
};