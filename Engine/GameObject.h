#pragma once
#include "Math.h"


class Model;
class ShaderBase;
class Camera;
class Collider;
class Controller;


class GameObject
{
public:
	GameObject() {};
	~GameObject() {};
};



class GraphicComponent
{
public:
	GraphicComponent() {};
	GraphicComponent(Model* m, ShaderBase* s);

	Model* model;
	ShaderBase* shader;
};



class Player
{
public:
	Player();
	~Player() {};

	SMatrix transform;
	Collider* collider = nullptr;

	GraphicComponent gfx_comp;
	Controller* con;
	Camera* cam;
};