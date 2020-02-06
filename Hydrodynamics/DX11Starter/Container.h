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
	std::vector<DirectX::XMFLOAT3> m_cornerArr;
	//std::vector <>
public:
	Container(DirectX::XMFLOAT3 * cornerArr, ID3D11Device* device, ID3D11DeviceContext* context, SimpleVertexShader* vs, SimplePixelShader* ps);
	~Container();
	
	void DrawContainer(Camera* camera);
};