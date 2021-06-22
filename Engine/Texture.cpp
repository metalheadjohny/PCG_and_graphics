#include "pch.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STBI_MSC_SECURE_CRT
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "WICTextureLoader.h"
#include "Texture.h"
#include "Perlin.h"


Texture::Texture() : _dxID(nullptr), _arraySrv(nullptr) {}



Texture::Texture(ID3D11Device* device, const std::string& fileName) : _fileName(fileName), _dxID(nullptr), _arraySrv(nullptr)
{
	if (!loadFromFile(fileName.c_str()))
	{
		OutputDebugStringA("Texture not in file, checking memory... \n");
		return;
	}

	setUpAsResource(device);
}



Texture::Texture(const std::string& fileName) : _fileName(fileName), _dxID(nullptr), _arraySrv(nullptr)
{
	if (!loadFromFile(fileName.c_str()))
	{
		OutputDebugStringA("Texture not in file, checking memory... \n");
	}
}



int Texture::GetFormatFromFile(const char* filename)
{
	int w, h, n;
	stbi_info(filename, &w, &h, &n);	//_nc is our format but holds this temporarily
	return n == 3 ? 4 : n;
}



int Texture::GetFormatFromMemory(const unsigned char* data, size_t size)
{
	int w, h, n;
	stbi_info_from_memory(data, size, &w, &h, &n);
	return n == 3 ? 4 : n;
}



bool Texture::loadFromFile(const char* filename)
{
	int fileFormat, desiredFormat, w, h;
	desiredFormat = GetFormatFromFile(filename);
	_mdata = std::shared_ptr<unsigned char[]>(stbi_load(filename, &w, &h, &fileFormat, desiredFormat));

	if (!_mdata)
	{
		return false;
	}

	_w = w;
	_h = h;
	_nc = desiredFormat;

	_fileName = filename;

	return true;
}


std::vector<float> Texture::LoadAsFloatVec(const std::string& path)
{
	float* temp;
	try
	{
		int tw, th, tn;

		// Staying as it is to avoid reworking strife level but should remove the copy.
		temp = stbi_loadf(path.c_str(), &tw, &th, &tn, 4);
		std::vector<float> result(temp, temp + tw * th * tn);

		delete temp;

		return result;
	}
	catch (...)
	{
		if (temp)
			delete temp;

		OutputDebugStringA(("Error loading texture '" + path + "' \n").c_str());
		return std::vector<float>();
	}
}


bool Texture::loadFromMemory(const unsigned char* data, size_t size)
{
	try
	{
		int fileFormat, desiredFormat, w, h;
		desiredFormat = GetFormatFromMemory(data, size);
		unsigned char* wat = stbi_load_from_memory(data, size, &w, &h, &fileFormat, desiredFormat);
		_mdata = std::shared_ptr<unsigned char[]>(wat);
		_w = w;
		_h = h;
		_nc = desiredFormat;

		return (_mdata.get() != nullptr);
	}
	catch (...)
	{
		OutputDebugStringA("Error loading texture from memory. \n");
		return false;
	}
}


bool Texture::loadFromPerlin(ID3D11Device* device, Procedural::Perlin& perlin)
{
	_w = perlin._w;
	_h = perlin._h;
	_nc = 1;
	
	_mdata = std::shared_ptr<unsigned char[]>(perlin.getUCharVector().data());

	return setUpAsResource(device);
}


bool Texture::loadWithMipLevels(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& path)
{
	std::wstring temp(path.begin(), path.end());
	const wchar_t* widecstr = temp.c_str();

	HRESULT result = DirectX::CreateWICTextureFromFile(device, context, widecstr, nullptr, &_arraySrv, 0);

	if (FAILED(result))
	{
		OutputDebugStringA("Can't create texture2d with mip levels (WIC). \n");
		exit(4201);
		return false;
	}
	return true;
}



bool Texture::setUpAsResource(ID3D11Device* device, bool deleteData)
{
	DXGI_FORMAT inferredFormat = N_TO_FORMAT_DX11[_nc - 1];

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = _w;
	desc.Height = _h;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = inferredFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// For now always uses 1 byte per channel textures, @todo add byte width flag as well
	UINT pixelWidth = _nc;

	D3D11_SUBRESOURCE_DATA texData;
	texData.pSysMem = (void *)(_mdata.get());
	texData.SysMemPitch = desc.Width * pixelWidth;
	texData.SysMemSlicePitch = 0;

	createGPUResource(device, &desc, &texData);
	createSRV(device, desc);

	if (deleteData)
		freeMemory();

	return true;
}



bool Texture::createGPUResource(ID3D11Device* device, D3D11_TEXTURE2D_DESC* desc, D3D11_SUBRESOURCE_DATA* data)
{
	if (FAILED(device->CreateTexture2D(desc, data, &_dxID)))
	{
		OutputDebugStringA("Can't create texture2d. \n");
		exit(42);
	}

	return true;
}


bool Texture::createSRV(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& desc)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = desc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (FAILED(device->CreateShaderResourceView(_dxID.Get(), &shaderResourceViewDesc, &_arraySrv)))
	{
		OutputDebugStringA("Can't create shader resource view. \n");
		exit(43);
	}
}


std::vector<uint8_t> Texture::LoadToSysMem(ID3D11Device* device, ID3D11DeviceContext* dc, const Texture& tex)
{
	// Create a staging texture to copy to, currently not the entire format is stored in texture so I only use number of channels
	Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture{};

	D3D11_TEXTURE2D_DESC texDesc{}; //Texture::create2DTexDesc(_w, _h, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_STAGING, 0u, D3D11_CPU_ACCESS_READ, 0u);
	texDesc.Width = tex.w();
	texDesc.Height = tex.h();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = N_TO_FORMAT_DX11[tex._nc - 1];
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	texDesc.BindFlags = 0;
	texDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&texDesc, 0, stagingTexture.GetAddressOf())))
	{
		OutputDebugStringA("Can't create off-screen texture. \n");
		exit(425);
	}

	// Copy data from the GPU texture to the staging texture
	dc->CopyResource(stagingTexture.Get(), tex._dxID.Get());

	D3D11_MAPPED_SUBRESOURCE msr;
	dc->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &msr);

	unsigned char* pDataPtr = static_cast<unsigned char*>(msr.pData);

	std::vector<unsigned char> result(pDataPtr, pDataPtr + msr.DepthPitch);

	dc->Unmap(stagingTexture.Get(), 0);

	return result;
}


void Texture::SaveToFile(ID3D11Device* device, ID3D11DeviceContext* dc, const Texture& tex, const char* filepath)
{
	auto imageData = LoadToSysMem(device, dc, tex);
	Image::SaveAsPng(filepath, tex.w(), tex.h(), tex.nc(), imageData.data());
}