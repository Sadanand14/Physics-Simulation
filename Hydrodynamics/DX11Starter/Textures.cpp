#include "Textures.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

ID3D11SamplerState*  Texture::m_sampler;
unsigned int Texture::count = 0;

Texture::Texture(std::wstring path, ID3D11Device* device, ID3D11DeviceContext* context):m_srv(nullptr)
{
	count++;
	std::wstring ws= path.substr(path.find('.') + 1, path.length());
	if (ws == L"png"|| ws == L"jpg")
		DirectX::CreateWICTextureFromFile(device, context, path.c_str(), 0, &m_srv);
	else if (ws == L"dds")
		DirectX::CreateDDSTextureFromFile(device, path.c_str(), 0, &m_srv);

	if (!m_sampler) 
	{
		D3D11_SAMPLER_DESC samplerDesc = {}; // The {} part zeros out the struct!
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Setting this allows for mip maps to work! (if they exist)
		// Ask DirectX for the actual object
		device->CreateSamplerState(&samplerDesc, &m_sampler);
	}
}

Texture::~Texture()
{
	if(m_srv)m_srv->Release();
	if (--count == 0) m_sampler->Release();
}