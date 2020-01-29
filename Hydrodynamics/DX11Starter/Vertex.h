#pragma once

#include "types.h"

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The position of the vertex
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
};

struct WaterVertex 
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal = DirectX::XMFLOAT3(0,1,0);
	DirectX::XMFLOAT2 UV = DirectX::XMFLOAT2(0,0);
	DirectX::XMFLOAT3 Tangent = DirectX::XMFLOAT3(0, 0, 0);
};

struct Waves 
{
	DirectX::XMFLOAT4 AFSW;
	//DirectX::XMFLOAT4 WaveDirection;
};

struct TerrainVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
};