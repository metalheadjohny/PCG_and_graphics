#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Sampler.h"
#include "MeshDataStructs.h"

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include <memory>



struct TextureMetaData
{
	std::shared_ptr<Texture> _tex;
	TextureRole _role;
	std::array<TextureMapMode, 3> _mapMode = { TextureMapMode::WRAP, TextureMapMode::WRAP, TextureMapMode::WRAP };
	uint8_t _uvIndex = 0u;
	uint8_t _regIndex = 0u;	// Decouple from role later


	template <typename Archive>
	void save(Archive& ar)
	{
		ar(_tex->_fileName(), _role, _mapMode, _uvIndex, _regIndex);
	}

	template <typename Archive>
	void load(Archive& ar)
	{
		std::string texFileName;
		ar(texFileName, _role, _mapMode, _uvIndex, _regIndex);
	}
};



class Material
{
protected:

	// Most important sorting criteria
	VertexShader* _vertexShader{};
	PixelShader* _pixelShader{};

public:

	// Second most important sorting criteria
	std::vector<TextureMetaData> _texMetaData;
	
	// Determines whether it goes to the transparent or opaque queue
	bool _opaque{false};

	// This could also belong in the vertex buffer... like stride and offset do
	D3D11_PRIMITIVE_TOPOLOGY _primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	Material();

	Material(VertexShader* vs, PixelShader* ps, bool opaque);

	void bind(ID3D11DeviceContext* context)
	{
		_vertexShader->bind(context);
		_pixelShader->bind(context);
		bindTextures(context);
		context->IASetPrimitiveTopology(_primitiveTopology);
	}

	void bindTextures(ID3D11DeviceContext* context) const;
	
	std::vector<D3D11_SAMPLER_DESC> createSamplerDescs() const;

	inline VertexShader* getVS() const { return _vertexShader; }

	inline PixelShader* getPS() const { return _pixelShader; }

	inline void setVS(VertexShader* vs) { _vertexShader = vs; }

	inline void setPS(PixelShader* ps) { _pixelShader = ps; }

	inline void addMaterialTexture(Texture* t, TextureRole role, std::array<TextureMapMode, 3> tmm, uint8_t uvIndex = 0u)
	{
		_texMetaData.push_back({std::shared_ptr<Texture>(t), role, tmm, uvIndex, 0u});
	}

	template <typename Archive>
	void save(Archive& ar, const std::vector<UINT>& texIDs) const
	{
		ar(getVS()->_id, get_PS()->_id, _texMetaData, _primitiveTopology, _opaque);
	}

	template <typename Archive>
	void load(Archive& ar, const std::vector<UINT>& texIDs) const
	{
		ar(getVS()->_id, get_PS()->_id, _texMetaData, _primitiveTopology, _opaque);
	}
};



struct MaterialFileFormat
{
	uint64_t _shaderKey{};
	std::vector<TextureMetaData> _texMetaData;
	D3D11_PRIMITIVE_TOPOLOGY _primitiveTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	bool _opaque{true};

	template <typename Archive>
	void serialize(Archive& ar, const std::vector<UINT>& texIDs) const
	{
		ar(_shaderKey, _texMetaData, _primitiveTopology, _opaque);
	}

	MaterialFileFormat(const Material& mat) 
		: _texMetaData (mat._texMetaData), _primitiveTopology(mat._primitiveTopology), _opaque(mat._opaque)
	{
		if (mat.getVS())	// Id is currently same for both vs and ps, which isnt optimal but ok
			mat.getVS()->_id;
		else
			_shaderKey = 0u;	// Lowest common denominator, only uses position
	}
};