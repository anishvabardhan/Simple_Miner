#pragma once

#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

#include "Game/App.hpp"

#define UNUSED(x) (void)x
#define DELETE_PTR(x) if(x) { delete x; x = nullptr; }

extern Renderer* g_theRenderer;
extern InputSystem* g_theInputSystem;
extern AudioSystem* g_theAudio;
extern DevConsole* g_theConsole;
extern JobSystem* g_theJobSystem;
extern App* g_theApp;

struct Vec2;
struct Rgba8;

constexpr float			SCREEN_SIZE_X						= 1600.0f;
constexpr float			SCREEN_SIZE_Y						= 800.0f;
constexpr float			WORLD_SIZE_X						= 200.0f;
constexpr float			WORLD_SIZE_Y						= 100.0f;
constexpr float			WORLD_CENTER_X						= WORLD_SIZE_X / 2.0f;
constexpr float			WORLD_CENTER_Y						= WORLD_SIZE_Y / 2.0f;

void					DrawDebugRing(Vec2 const& center, float const& radius, float const& orientation, float const& thickness, Rgba8 const& color);
void					DrawDebugLine(Vec2 const& startPos, Vec2 const& endPos, float const& thickness, Rgba8 const& color);