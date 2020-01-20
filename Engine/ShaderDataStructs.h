#pragma once
#include "Math.h"
#include <vector>
#include <d3d11.h>
#include "Texture.h"
#include "Light.h"
#include "MeshDataStructs.h"


struct RenderFormat
{
	unsigned int stride = sizeof(Vert3D);
	unsigned int offset = 0u;
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};


struct Vec4Buffer
{
	SVec4 vec4;
};


struct WMBuffer
{
	SMatrix world;
};


struct ViewProjBuffer
{
	SMatrix lightView;
	SMatrix lightProjection;
};


struct ElapsedTimeBuffer
{
	float elapsed;
	SVec3 padding;
};


struct LightBuffer
{
	SVec3 alc;
	float ali;
	SVec3  dlc;
	float dli;
	SVec3 slc;
	float sli;
	SVec4 pos;
	SVec4 ePos;
};

struct LightBuffer2
{
	SVec3 alc;
	float ali;
	SVec3  dlc;
	float dli;
	SVec3 slc;
	float sli;
	SVec4 pos;
};


struct VolumShaderBuffer
{
	SMatrix rot;
	SVec4 ePos;
};


struct LightPosBuffer
{
	SVec3 lightPosition;
	float padding;
};