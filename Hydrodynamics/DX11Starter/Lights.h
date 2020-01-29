#pragma once
#include "d3d11.h"
#include "types.h"

namespace DirectX {
	struct DirectionalLight
	{
		XMFLOAT4 AmbientColor;
		XMFLOAT4 DiffuseColor;
		XMFLOAT3 Direction;
	};
}