#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"

#include "Game/GameCommon.hpp"

class Clock;
class BitmapFont;

class World;
class Player;

class Game
{
public:
	Camera*				m_screenCamera					= nullptr;
	Clock*				m_gameClock						= nullptr;
	BitmapFont*			m_bitmapFont					= nullptr;
	Player*				m_player						= nullptr;
	World*				m_gameWorld						= nullptr;
	float				m_timerValue					= 0.0f;
public:
						Game();
						~Game();

	void				StartUp();
	void				Shutdown();

	void				Update(float deltaseconds);
	void				Render() const ;

	void				HandleInput();
	void				UpdateFromController(float deltaseconds);
};