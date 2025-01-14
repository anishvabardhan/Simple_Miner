#include "Game/Game.hpp"

#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/World.hpp"
#include "Game/Chunk.hpp"

#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/SimpleTriangleFont.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

#include <time.h>

Game::Game()
{
}

Game::~Game()
{
}

void Game::StartUp()
{
	srand((unsigned int)time(NULL));

	m_gameClock = new Clock(Clock::GetSystemClock());

	m_screenCamera = new Camera();

	m_bitmapFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont.png");
	
	m_player = new Player(this, Vec3(-5.0f, 2.0f, 100.0f));

	m_gameWorld = new World(this);

	DevConsoleLine line = DevConsoleLine("----------------------------------------", DevConsole::INFO_MAJOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("CONTROLS", DevConsole::COMMAND_ECHO);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS Q/LEFT TRIGGER          ----> ROLL CAMERA LEFT", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS E/RIGHT TRIGGER         ----> ROLL CAMERA RIGHT", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS H/START BUTTON          ----> RESET CAMERA POSITION AND ORIENTATION TO ZERO", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("HOLD SHIFT/A BUTTON           ----> INCREASE CAMERA SPEED BY A FACTOR OF 10 WHILE HELD", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS Z/LEFT SHOULDER         ----> MOVE UP RELATIVE TO WORLD", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS C/RIGHT SHOULDER        ----> MOVE DOWN RELATIVE TO WORLD", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS W/LEFT STICK Y-AXIS     ----> MOVE CAMERA FORWARD", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS S/LEFT STICK Y-AXIS     ----> MOVE CAMERA BACKWARD", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);
	
	line = DevConsoleLine("PRESS D/LEFT STICK X-AXIS     ----> MOVE CAMERA RIGHT", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);
	
	line = DevConsoleLine("PRESS A/LEFT STICK X-AXIS     ----> MOVE CAMERA LEFT", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);
	
	line = DevConsoleLine("PRESS UP/MOUSE                ----> PITCH CAMERA UP", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);
	
	line = DevConsoleLine("PRESS DOWN/MOUSE              ----> PITCH CAMERA DOWN", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);
	
	line = DevConsoleLine("PRESS RIGHT/MOUSE             ----> YAW CAMERA RIGHT", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS LEFT/MOUSE              ----> YAW CAMERA LEFT", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS 1                       ----> Spawn a line from the player along their forward direction 20 units in length. Duration 10 seconds, draw in x-ray mode, color yellow.", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS 2                       ----> Spawn a sphere directly below the player position on the xy-plane. Duration 60 seconds, draw with depth, color RGB values 150, 75, 0.", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS 3                       ----> Spawn a wireframe sphere 2 units in front of player camera. Radius 1, duration 5 seconds, draw with depth, color transitions from green to red.", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS 4                       ----> Spawn a basis using the player current model matrix. Duration 20 seconds, draw with depth, color white.", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS 5                       ----> Spawn billboarded text showing the player position and orientation. Duration 10 seconds, draw with depth, color transitions from white to red.", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS 6                       ----> Spawn a wireframe cylinder at the player position. Duration 10 seconds, draw with depth, color transitions from white to red.", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("PRESS 7                       ----> Spawn a wireframe cylinder at the player position. Duration 10 seconds, draw with depth, color transitions from white to red.", DevConsole::INFO_MINOR);
	g_theConsole->m_lines.push_back(line);

	line = DevConsoleLine("----------------------------------------", DevConsole::INFO_MAJOR);
	g_theConsole->m_lines.push_back(line);
}

void Game::Shutdown()
{
	DELETE_PTR(m_screenCamera);
	DELETE_PTR(m_gameWorld);
	DELETE_PTR(m_player);
}

void Game::Update(float deltaseconds)
{
	m_timerValue += deltaseconds;
	
	if (m_timerValue >= 0.5f)
	{
		m_timerValue = 0.0f;
	}

	m_screenCamera->SetOrthoView(Vec2(0.0f, 0.0f), Vec2(SCREEN_SIZE_X, SCREEN_SIZE_Y));

	m_player->Update(deltaseconds);
	m_gameWorld->Update(deltaseconds);	

	if (!g_theConsole->IsOpen())
	{
		HandleInput();
		UpdateFromController(deltaseconds);
	}
}

void Game::Render() const
{
	g_theRenderer->BeginCamera(*m_player->m_worldCamera, RootSig::DEFAULT_PIPELINE);

	m_gameWorld->Render();

	g_theRenderer->EndCamera(*m_player->m_worldCamera);

	//DebugRenderWorld(*m_player->m_worldCamera);

	//static float fps = 0.0f;

	//if (m_timerValue == 0.0f)
	//{
	//	fps = 1.0f / Clock::GetSystemClock().GetDeltaSeconds();
	//}

	//Mat44 screenTextModelMatrix1;
	//screenTextModelMatrix1.SetTranslation3D(Vec3(650.0f, 80.0f, 0.0f));
	//DebugAddScreenText("WASD: Fly Horizontal, QE: Fly Vertical (SPACE: Fast), F1: Chunk Bounds, F8: Regenerate", screenTextModelMatrix1, 15.0f, Vec2(0.5f, 0.5f), 0.0f, Rgba8::YELLOW, Rgba8::YELLOW);

	//Mat44 screenTextModelMatrix2;
	//screenTextModelMatrix2.SetTranslation3D(Vec3(670.0f, 50.0f, 0.0f));
	//DebugAddScreenText(Stringf("NumChunks: %d/%d, NumBlocks: %d, NumVerts: %d, (FPS: %.0f)", m_gameWorld->m_activeChunks.size(), m_gameWorld->m_maxChunks, int((float)m_gameWorld->m_numOfVerts * 0.125f), m_gameWorld->m_numOfVerts, fps), screenTextModelMatrix2, 15.0f, Vec2(1.0f, 0.5f), 0.0f, Rgba8::CYAN, Rgba8::CYAN);

	//DebugAddMessage(Stringf("NumChunks In Map: %d", m_gameWorld->m_activeChunks.size()), Vec3(0.0f, 600.0f, 0.0f), 15.0f, Vec2(0.5f, 0.5f), 0.0f, Rgba8::BLACK, Rgba8::BLACK);
	//DebugAddMessage(Stringf("NumChunks: %d", m_gameWorld->m_numChunks), Vec3(0.0f, 600.0f, 0.0f), 15.0f, Vec2(0.5f, 0.5f), 0.0f, Rgba8::BLACK, Rgba8::BLACK);
	//DebugAddMessage(Stringf("NumChunkCoords: %d", m_gameWorld->m_activeChunkCoords.size()), Vec3(0.0f, 600.0f, 0.0f), 15.0f, Vec2(0.5f, 0.5f), 0.0f, Rgba8::BLACK, Rgba8::BLACK);

	//DebugRenderScreen(*m_screenCamera);
}

void Game::HandleInput()
{
	if (g_theInputSystem->WasKeyJustPressed(KEYCODE_ESC))
	{
		g_theApp->HandleQuitRequested();
	}
}

void Game::UpdateFromController(float deltaseconds)
{
	UNUSED(deltaseconds);

	XboxController const& controller = g_theInputSystem->GetController(0);

	if (controller.WasButtonJustPressed(XboxButtonID::BUTTON_B))
	{
		g_theApp->HandleQuitRequested();
	}
}