#pragma once
#include <vector>

#include "SkelAnimShader.h"
#include "ShaderDataStructs.h"
#include "Phong.h"
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
#include "ShaderMaze.h"
#include "ShaderClipper.h"
#include "VolumetricScreen.h"



class ShaderManager
{
protected:
	ID3D11Device* _device;
	HWND hwnd;

	WMBuffer matrixBuffer;
	ElapsedTimeBuffer variableBuffer;
	LightBuffer lightBuffer;

public:

	ShaderSkelAnim animator;
	Phong light;
	WireframeShader wireframe;
	ShaderHUD HUD, brightMask, blurHor, blurVer, bloom;
	ShaderDepth depth;
	ShaderPT texProjector;
	ShaderShadow shadow;
	ShaderCM cubeMapShader;
	ShaderSkybox skyboxShader;
	ShaderStrife strife;
	ShaderWater water;
	InstancedShader instanced, dragon;
	TerrainShader terrainMultiTex, terrainNormals, treeShader;
	ShaderMaze mazeMat, mazeFloorMat;
	ShaderClipper clipper;

	//wisps
	ShaderVolumetric shVolumEarth, shVolumFire, shVolumWater, shVolumAir, shVolumLava, shVolumTornado;
	VolumetricScreen shVolumScreen;

	ShaderManager();
	~ShaderManager();

	void init(ID3D11Device* _device, HWND hwnd);
};