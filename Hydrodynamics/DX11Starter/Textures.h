#pragma once
#include "d3d11.h"
#include "types.h"

class Texture 
{
private:
	ID3D11ShaderResourceView* m_srv;
	static unsigned int count;

public:
	static ID3D11SamplerState* m_sampler;
	Texture(std::wstring, ID3D11Device* , ID3D11DeviceContext*);

	inline ID3D11ShaderResourceView* GetSRV()const { return m_srv; }

	~Texture();

};