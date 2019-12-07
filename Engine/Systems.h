#pragma once
#define WIN32_LEAN_AND_MEAN

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_win32.h"
#include "IMGUI/imgui_impl_dx11.h"

#include <windows.h>
#include "InputManager.h"
#include "Renderer.h"
#include "GameClock.h"
#include "CollisionEngine.h"
#include "Audio.h"
#include "LevelManager.h"
#include "ResourceManager.h"



//centralized, high level "glue" class that contains engine subsystems and exposes them to the game, outlives levels
class Systems
{
private:
	bool Frame(float dTime);
	void InitializeWindows(int& w, int& h);
	void OutputFPS(float dTime);

	LPCWSTR _applicationName;
	HINSTANCE _hinstance;
	HWND _hwnd;
	POINT _midWindow;

	int screenWidth, screenHeight, windowWidth, windowHeight;

public:
	Systems();
	~Systems();

	bool Initialize();
	void Run();
	void Shutdown();
	
	//core systems
	InputManager _inputManager;
	ResourceManager _resMan;
	Renderer _renderer;
	LevelManager* _levelMan;
	CollisionEngine _colEngine;
	Audio _audio;
	GameClock _clock;
	
	Controller _controller;

	//extra rendering data - this should end up in the renderer and loaders ONLY @TODO
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	D3D _D3D;

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	UINT getScrW() { return screenWidth;  }
	UINT getScrH() { return screenHeight; }
	UINT getWinW() { return windowWidth;  }
	UINT getWinH() { return windowHeight; }
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static Systems* ApplicationHandle = 0;