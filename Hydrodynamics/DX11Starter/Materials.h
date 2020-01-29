#pragma once
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "Mesh.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "Game.h"
#include "types.h"


class Materials 
{
	SimpleVertexShader* vShader;
	SimplePixelShader* pShader;

public:
	Materials(SimpleVertexShader* a, SimplePixelShader* b);
	~Materials();
};