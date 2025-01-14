#pragma once

#include <string>

#include "Engine/Math/IntVec2.hpp"

struct BlockDefinition
{
	unsigned char			m_ID = 0u;

	std::string				m_name;

	bool					m_isVisible			= false;
	bool					m_isSolid			= false;
	bool					m_isOpaque			= false;

	IntVec2					m_topUVs			= IntVec2::ZERO;
	IntVec2					m_sideUVs			= IntVec2::ZERO;
	IntVec2					m_bottomUVs			= IntVec2::ZERO;

	unsigned char			m_lightInfluence	= 0;

	static BlockDefinition	s_blockDefinitions[19];

							BlockDefinition()	= default;
							~BlockDefinition() {};

	static void				initializeBlockDefs();
	static void				CreateNewBlockDef(unsigned char id, std::string name, bool isVisible, bool isSolid, bool isOpaque, IntVec2 topUV, IntVec2 sideUV, IntVec2 bottomUV, unsigned char lightVal);
	static BlockDefinition	GetBlockDefByID(unsigned char blockDefID);
};