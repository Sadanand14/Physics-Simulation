#include "Container.h"

Container::Container(DirectX::XMFLOAT3* cornerArr, ID3D11Device* device, ID3D11DeviceContext* context,
	SimpleVertexShader* vs, SimplePixelShader* ps)
	: m_device(device), m_context(context), m_VS(vs), m_PS(ps)
{
	m_cornerArr.reserve(8);
	std::vector<Vertex> vertexArr;
	for (unsigned int i = 0; i < 8; ++i) 
	{
		m_cornerArr.push_back(cornerArr[i]);
		Vertex V;
		V.Position = cornerArr[i];
		V.Normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertexArr.push_back(V);
	}
	
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertexArr.data();
	m_device->CreateBuffer(&vbd, &initialVertexData, &m_vb);

	unsigned int indexArr[] = {0,1,2,0,2,3
							  ,5,3,6,5,0,3
							  ,1,4,7,1,7,2
							  ,4,5,6,4,6,7
							  ,2,7,6,2,6,3};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * 30;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indexArr;
	m_device->CreateBuffer(&ibd, &initialIndexData, &m_ib);
}

Container::~Container() 
{
	if (m_vb) m_vb->Release();
	if (m_ib) m_ib->Release();
}

void Container::DrawContainer(Camera * camera) 
{
	static unsigned int offset = 0;
	static unsigned int stride = 0;
	m_context->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);
	m_context->IASetIndexBuffer(m_ib, DXGI_FORMAT_R32_UINT, 0);

	m_VS->SetShader();
	m_PS->SetShader();
	m_VS->SetMatrix4x4("world", DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
	m_VS->SetMatrix4x4("view", camera->GetView());
	m_VS->SetMatrix4x4("projection", camera->GetProjection());
	m_VS->CopyAllBufferData();
	m_context->DrawIndexed(30, 0, 0);
}