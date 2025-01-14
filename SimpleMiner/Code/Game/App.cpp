#include "Game/App.hpp"

#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Window/Window.hpp"

#include "Game/Game.hpp"
#include "Game/World.hpp"

App* g_theApp = nullptr;
Renderer* g_theRenderer = nullptr;
AudioSystem* g_theAudio = nullptr;
Window* g_theWindow = nullptr;
Game* g_theGame = nullptr;
JobSystem* g_theJobSystem = nullptr;

App::App()
{
}

App::~App()
{
}

void App::StartUp()
{
	InitializeGameConfigurations("Data/GameConfig.xml");

	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);

	DevConsoleConfig consoleConfig;
	consoleConfig.m_fontFilePath = "Data/Fonts/SquirrelFixedFont.png";
	g_theConsole = new DevConsole(consoleConfig);

	JobSystemConfig jobSystemConfig;
	jobSystemConfig.m_numOfWorkerThreads = std::thread::hardware_concurrency() - 1;
	g_theJobSystem = new JobSystem(jobSystemConfig);

	InputConfig inputConfig;
	g_theInputSystem = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_inputSystem = g_theInputSystem;
	windowConfig.m_windowTitle = "SimpleMiner";
	windowConfig.m_clientAspect = 2.0f;
	g_theWindow = new Window(windowConfig);

	RenderConfig renderConfig;
	renderConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(renderConfig);

	g_theAudio = new AudioSystem();
	g_theGame = new Game();

	g_theJobSystem->StartUp();
	g_theEventSystem->StartUp();
	g_theConsole->StartUp();
	g_theInputSystem->StartUp();
	g_theWindow->StartUp();
	g_theRenderer->StartUp();
	
	DebugRenderConfig debugConfig;
	debugConfig.m_renderer = g_theRenderer;
	
	DebugRenderSystemStartup(debugConfig);
	
	g_theAudio->Startup();
	g_theGame->StartUp();

	SubscribeEventCallbackFunction("QUIT", App::QuitApp);
}

void App::Run()
{
	while (!IsQuitting())
	{
		RunFrame();
	}
}

void App::ShutDown()
{
	g_theGame->Shutdown();
	g_theAudio->Shutdown();
	g_theRenderer->ShutDown();
	g_theWindow->ShutDown();
	g_theInputSystem->ShutDown();
	g_theConsole->ShutDown();
	g_theEventSystem->ShutDown();
	g_theJobSystem->ShutDown();

	DELETE_PTR(g_theGame);
	DELETE_PTR(g_theConsole);
	DELETE_PTR(g_theEventSystem);
	DELETE_PTR(g_theRenderer);
	DELETE_PTR(g_theAudio);
	DELETE_PTR(g_theWindow);
	DELETE_PTR(g_theInputSystem);
}

void App::RunFrame()
{
	Clock::GetSystemClock().TickSystemClock();

	BeginFrame();
	Update(Clock::GetSystemClock().GetDeltaSeconds());
	Render();
	EndFrame();
}

void App::BeginFrame()
{
	g_theJobSystem->BeginFrame();
	g_theEventSystem->BeginFrame();
	g_theConsole->BeginFrame();
	g_theWindow->BeginFrame();
	g_theInputSystem->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();

	DebugRenderBeginFrame();
}

void App::Update(float deltaseconds)
{
	if (g_theConsole->IsOpen()/* && g_theGame->m_isAttractMode*/)
	{
		g_theInputSystem->SetCursorMode(false, false);
	}
	else
	{
		g_theInputSystem->SetCursorMode(true, true);
	}

	if (g_theInputSystem->WasKeyJustPressed('P'))
	{
		m_isPaused = !m_isPaused;
	}

	if (g_theInputSystem->WasKeyJustPressed('O'))
	{
		g_theGame->Update(deltaseconds);
		m_isPaused = true;
	}

	if (g_theInputSystem->WasKeyJustPressed('T'))
	{
		m_isSlowMo = true;
	}
	if (g_theInputSystem->WasKeyJustReleased('T'))
	{
		m_isSlowMo = false;
	}

	if (g_theInputSystem->WasKeyJustPressed('Y'))
	{
		m_isFastMo = true;
	}
	if (g_theInputSystem->WasKeyJustReleased('Y'))
	{
		m_isFastMo = false;
	}

	if (g_theInputSystem->WasKeyJustPressed(KEYCODE_TILDE))
	{
		g_theConsole->ToggleOpen();
	}

	if (m_isPaused && !m_isSlowMo && !m_isFastMo)
	{
		g_theGame->m_gameClock->SetTimeScale(0.0f);
	}
	else if (!m_isPaused && m_isSlowMo && !m_isFastMo)
	{
		g_theGame->m_gameClock->SetTimeScale(0.1f);
	}
	else if (!m_isPaused && !m_isSlowMo && m_isFastMo)
	{
		g_theGame->m_gameClock->SetTimeScale(4.0f);
	}
	else if (!m_isPaused && !m_isSlowMo && !m_isFastMo)
	{
		g_theGame->m_gameClock->SetTimeScale(1.0f);
	}

	g_theGame->Update(g_theGame->m_gameClock->GetDeltaSeconds());
}

void App::Render() const
{
	g_theRenderer->ClearScreen(g_theGame->m_gameWorld->m_skyColor/*Rgba8(200, 230, 255, 255)*/);

	g_theGame->Render();

	/*if (g_theConsole->IsOpen())
	{
		g_theRenderer->BeginCamera(*g_theGame->m_screenCamera);
	
		g_theConsole->Render(AABB2(g_theGame->m_screenCamera->GetOrthoBottomLeft(), g_theGame->m_screenCamera->GetOrthoTopRight()), g_theRenderer);
	
		g_theRenderer->EndCamera(*g_theGame->m_screenCamera);
	}*/
}

void App::EndFrame()
{
	g_theAudio->EndFrame();
	g_theRenderer->EndFrame();
	g_theWindow->EndFrame();
	g_theInputSystem->EndFrame();
	g_theConsole->EndFrame();
	g_theEventSystem->EndFrame();
	g_theJobSystem->EndFrame();

	DebugRenderEndFrame();
}

void App::HandleKeyPressed(unsigned char keyCode)
{
	g_theInputSystem->HandleKeyPressed(keyCode);
}

void App::HandleKeyReleased(unsigned char keyCode)
{
	g_theInputSystem->HandleKeyReleased(keyCode);
}

void App::HandleQuitRequested()
{
	m_isQuitting = true;
}

float App::GetDeltaTime() const
{
	return m_deltaTime;
}

void App::SetDeltaTime(float newDeltaTime)
{
	m_deltaTime = newDeltaTime;
}

void App::InitializeGameConfigurations(char const* dataFilePath)
{
	XmlDocument gameConfigFile;

	XmlError result = gameConfigFile.LoadFile(dataFilePath);

	if (result != tinyxml2::XML_SUCCESS)
		GUARANTEE_OR_DIE(false, "COULD NOT LOAD XML");

	XmlElement* gameConfig = gameConfigFile.FirstChildElement();

	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*gameConfig, false);
}

bool App::QuitApp(EventArgs& args)
{
	UNUSED(args);

	g_theApp->HandleQuitRequested();

	return true;
}
