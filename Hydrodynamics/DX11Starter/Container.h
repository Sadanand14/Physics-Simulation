#pragma once

#include "types.h"
#include "Vertex.h"
#include "SimpleShader.h"
#include "Camera.h"
class Container 
{
private: 
	ID3D11Buffer* m_vb = nullptr, *m_ib = nullptr;
	SimpleVertexShader* m_VS = nullptr;
	SimplePixelShader* m_PS = nullptr;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	ID3D11RasterizerState* m_debugRaster = nullptr;
	std::vector<DirectX::XMFLOAT3> m_cornerArr;
	XMFLOAT4X4 m_modelMatrix;
	XMFLOAT3 m_scale;
	XMFLOAT3 m_position;

	void CalculatePlanes();

	//std::vector <>
public:
	
	Container(DirectX::XMFLOAT3 * cornerArr, ID3D11Device* device, ID3D11DeviceContext* context, SimpleVertexShader* vs, SimplePixelShader* ps, XMFLOAT3  pos, XMFLOAT3  scale);
	~Container();
	
	void DrawContainer(Camera* camera);
};