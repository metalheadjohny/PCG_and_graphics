#pragma once
#include "Math.h"
#include "Texture.h"
#include "Light.h"

namespace Strife
{

	class CloudscapeDefinition
	{
	public:
		PointLight celestial;

		SVec2 heightMask;

		SMatrix planeMat;
		SMatrix lightViewMat;
		SVec4 repeat = SVec4(4096.f);

		Texture coverage_broad;
		Texture coverage_frequent;
		Texture blue_noise;

		//3d texture resorce views
		ID3D11ShaderResourceView* baseShape;
		ID3D11ShaderResourceView* fineShape;
		Texture curlNoise;

		SVec3 rgb_sig_scattering;
		SVec3 rgb_sig_absorption;

		float eccentricity = 0.f;
		float scrQuadOffset = 15.f;
		float globalCoverage = .5f;
	};

}