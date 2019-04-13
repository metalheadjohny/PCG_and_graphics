#pragma once
#include <vector>

#include "ShaderDataStructs.h"
#include "ShaderBase.h"
#include "ShaderLight.h"
#include "ShaderDepth.h"
#include "ShaderShadow.h"
#include "ShaderHUD.h"
#include "ShaderWireframe.h"
#include "ShaderPT.h"
#include "ShaderCM.h"
#include "ShaderSkybox.h"
#include "ShaderStrife.h"
#include "ShaderWater.h"
#include "ShaderVolumetric.h"
#include "InstancedShader.h"
#include "TerrainShader.h"
#include "PerlinShader.h"
#include "ShaderMaze.h"
#include "ShaderClipper.h"


class ShaderManager
{
protected:
	ID3D11Device* _device;
	HWND hwnd;

	MatrixBuffer matrixBuffer;
	VariableBuffer variableBuffer;
	LightBuffer lightBuffer;

public:

	ShaderBase base;
	ShaderLight light;
	WireframeShader wireframe;
	ShaderHUD HUD, brightnessMasker, blurHor, blurVer, bloom;
	ShaderDepth depth;
	ShaderPT texProjector;
	ShaderShadow shadow;
	ShaderCM cubeMapShader;
	ShaderSkybox skyboxShader;
	ShaderStrife strife;
	ShaderWater water;
	InstancedShader instanced;
	PerlinShader perlin;
	TerrainShader terrainMultiTex;
	TerrainShader terrainNormals;
	TerrainShader treeShader;
	ShaderMaze dynamicHeightMaze;
	ShaderMaze normalMapper;
	ShaderClipper clipper;

	//wisps
	ShaderVolumetric shVolumEarth;
	ShaderVolumetric shVolumFire;
	ShaderVolumetric shVolumWater;
	ShaderVolumetric shVolumAir;
	ShaderVolumetric shVolumLava;
	ShaderVolumetric dragonCurve;

	ShaderManager();
	~ShaderManager();

	void init(ID3D11Device* _device, HWND hwnd);
};