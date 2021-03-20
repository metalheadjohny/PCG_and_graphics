#include "Engine.h"
#include "GUI.h"
#include <string>
#include <Mouse.h>

#include "ShaderGenerator.h"


Engine::Engine() :
	_scrWidth(GetSystemMetrics(SM_CXSCREEN)),
	_scrHeight(GetSystemMetrics(SM_CYSCREEN)),
	_threadPool(std::thread::hardware_concurrency() - 1)
{}


Engine::~Engine()
{
	Shutdown();
}


bool Engine::Initialize()
{
	// Testing grounds... move to tests project eventually
	//ShaderGenerator::preprocessAllPermutations(L"ShGen\\VS_proto.hlsl", "ShGen\\GeneratedVS\\vs_");
	
	_engineWindow.createWindow("Aeolian engine", this, _windowWidth, _windowHeight,
		Window<Engine>::CreationFlags::SHOW_WINDOW |
		Window<Engine>::CreationFlags::START_FOCUSED |
		Window<Engine>::CreationFlags::START_FOREGROUND);

	if (!_D3D.Initialize(_windowWidth, _windowHeight, false, _engineWindow.handle(), FULL_SCREEN))
	{
		OutputDebugStringA("Can't initialize D3D!");
		return false;
	}

	_device = _D3D.GetDevice();
	_deviceContext = _D3D.GetDeviceContext();

	_inputManager.initialize(_engineWindow.handle());
	_defController = Controller(&_inputManager);
	_inputManager.registerController(&_defController);

	if (!_renderer.initialize(_windowWidth, _windowHeight, _D3D))
	{
		OutputDebugStringA("Could not initialize the renderer!");
		return false;
	}

	_resMan.init(_device);

	_shaderCompiler.init(_device);
	_shaderCache.init(&_shaderCompiler);
	_matCache.init(&_shaderCache, &_resMan);

	_renderer._cam._controller = &_defController;

	GUI::initDxWin32(_engineWindow.handle(), _device, _deviceContext);

	// Loads the project configuration data into the project loader, as well as a list of levels associated to the project
	_project.loadFromConfig("../Tower Defense/Tower defense.json");
	
	if (!_project.getLevelReader().loadLevel(_project.getLevelList()[0]))
		assert(false && "Failed to load level list.");

	// Seems pointless but the project's ledger path will be in a file just not done yet.
	_project._ledgerPath = "../Tower Defense/Ledger.json";
	_resMan._assetLedger._ledgerFilePath = _project._ledgerPath;
	_resMan._assetLedger.load();
	_resMan._assetLedger.purge();

	_levelMan = new LevelManager(*this);

	return true;
}



void Engine::Run()
{
	_clock.Reset();

	MSG msg = {};
	bool done = false;

	// Loop until there is a quit message from the window or the user.
	while(!done)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			_clock.Tick();
			done = !Frame(_clock.DeltaTime());	// Otherwise do the frame processing.
		}
	}
}



bool Engine::Frame(float dTime)
{
	if (_inputManager.isKeyDown(VK_ESCAPE))
		return false;

	if (!_renderer.frame(dTime))
		return false;

	_levelMan->handleInput(*this, dTime);

	_levelMan->updateAndDrawCurrent(_renderer.rc);

	//_colEngine.update(); Old stuff, has some nice code in there though, need to pull it out and refactor it into something useful

	_inputManager.update();

	return true;
}



void Engine::OutputFPS(float dTime)
{
	std::ostringstream ss;
	ss << "Frame time: " << 1.0f / dTime << "\n";
	std::string s(ss.str());
	OutputDebugStringA(ss.str().c_str());
}



void Engine::Shutdown()
{
	ShowCursor(true);

	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);
}



extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::HandleWindowInput(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return true;

	switch (message)
	{
		case WM_KEYDOWN:
		{
			_inputManager.setKeyPressed((unsigned int)wparam);
			break;
		}
		case WM_KEYUP:
		{
			_inputManager.setKeyReleased((unsigned int)wparam);
			break;
		}
		case WM_INPUT:
		{
			UINT dwSize;

			GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			
			if (dwSize > 0)
			{
				LPBYTE lpb = new BYTE[dwSize];

				if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
					OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

				RAWINPUT* raw = (RAWINPUT*)lpb;

				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					_inputManager.setRelativeXY((short)(raw->data.mouse.lLastX), (short)(raw->data.mouse.lLastY));
				}

				delete[] lpb;
			}		
			break;
		}
		
		//mouse
		{
		case WM_ACTIVATEAPP:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			DirectX::Mouse::ProcessMessage(message, wparam, lparam);
			break;
		}
		default:
		{
			return DefWindowProc(hwnd, message, wparam, lparam);
		}
	}

	return 0;
}