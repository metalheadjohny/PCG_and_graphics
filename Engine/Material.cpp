#include "pch.h"
#include "Material.h"



Material::Material() 
	: _vertexShader(nullptr), _pixelShader(nullptr) {}



Material::Material(VertexShader* vs, PixelShader* ps, bool opaque) 
	: _vertexShader(vs), _pixelShader(ps), _opaque(opaque) {}



void Material::bindTextures(ID3D11DeviceContext* context) const
{
	//for (int i = 0; i < _materialTextures.size(); ++i)
	for(auto& [metaData, tex] : _materialTextures)
	{
		if (tex->_dxID.Get())
		{
			context->PSSetShaderResources(metaData._role, 1, tex->_arraySrv.GetAddressOf());
		}
	}
}


// What is this even doing 
std::vector<D3D11_SAMPLER_DESC> Material::createSamplerDescs() const
{
	static const std::map<TextureMapMode, D3D11_TEXTURE_ADDRESS_MODE> ADDR_MODE_MAP
	{
		{CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP},
		{WRAP, D3D11_TEXTURE_ADDRESS_WRAP},
		{MIRROR, D3D11_TEXTURE_ADDRESS_MIRROR},
		{MIRROR_ONCE, D3D11_TEXTURE_ADDRESS_MIRROR_ONCE},
		{BORDER, D3D11_TEXTURE_ADDRESS_BORDER}
	};

	std::vector<D3D11_SAMPLER_DESC> result;
	result.reserve(_materialTextures.size());

	// Uses default for now, but make it a setting eventually
	D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	for (UINT i = 0; i < _materialTextures.size(); ++i)
	{
		D3D11_TEXTURE_ADDRESS_MODE tam[3];

		for (UINT j = 0; j < 3; ++j)
			tam[j] = ADDR_MODE_MAP.at(_materialTextures[i]._metaData._mapMode[j]);

		result.push_back(Sampler::createSamplerDesc(filter, D3D11_COMPARISON_ALWAYS, 0.,
			D3D11_FLOAT32_MAX, tam[0], tam[1], tam[2]));	// goes the drum...
	}

	return result;
}