#pragma once
#include "Systems.h"
#include "GameObject.h"

#define device		_sys._device
#define context		_sys._deviceContext
#define randy		_sys._renderer
#define shady		_sys._renderer._shMan
#define collision	_sys._colEngine
#define resources	_sys._resMan
#define doot		_sys._audio
#define inman		_sys._inputManager

class LevelManager;

class Level
{
protected:
	Systems& _sys;
	LevelManager* lvlMan;

	float sinceLastInput = 0.f;	//consequence of slightly changing rastertek input instead of completely redoing it... fucking hell

	std::vector<GameObject*> objects;
	std::vector<GraphicComponent*> lesRenderables;
	std::vector<Collider> _levelColliders;
	Camera camera;

	bool finished = false;

public:
	Level(Systems& sys, LevelManager* lMan);
	
	void updateCam(float dTime) { randy._cam.Update(randy.rc.dTime); }

	void ProcessSpecialInput(float dTime);

	virtual void init(Systems& sys) = 0;
	virtual void draw(const RenderContext& rc) = 0;
	virtual void demolish() = 0;
	void procGen() {};
};