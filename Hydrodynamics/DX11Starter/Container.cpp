#include "Container.h"

Container::Container(DirectX::XMFLOAT3* cornerArr, ID3D11Device* device, ID3D11DeviceContext* context,
	SimpleVertexShader* vs, SimplePixelShader* ps, XMFLOAT3 pos, XMFLOAT3 scale)
	: m_device(device), m_context(context), m_VS(vs), m_PS(ps), m_position(pos), m_scale(scale)
{

	XMMATRIX trans = XMMatrixTranslation(pos.x,pos.y,pos.z);
	XMMATRIX Scale = XMMatrixScaling(scale.x, scale.y, scale.z);
	
	XMMATRIX res = Scale * trans;
	XMStoreFloat4x4(&m_modelMatrix, XMMatrixTranspose(res));

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
	
	CalculatePlanes();

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

	//rasterState for debugging particles
	D3D11_RASTERIZER_DESC rasterdesc = {};
	rasterdesc.CullMode = D3D11_CULL_NONE;
	rasterdesc.DepthClipEnable = true;
	rasterdesc.FillMode = D3D11_FILL_WIREFRAME;
	device->CreateRasterizerState(&rasterdesc, &m_debugRaster);
	
	
}


void Container::CalculatePlanes() 
{
	unsigned int indices[] = { 0,1,2
							  ,5,3,6
							  ,1,4,7
							  ,4,5,6
							  ,2,7,6 };

	XMVECTOR displacement;

	for(unsigned int i = 0; i < 5; ++i) 
	{
		XMMATRIX model = XMMatrixTranspose(XMLoadFloat4x4(&m_modelMatrix));

		//TODO:: ADD DISPLACEMENT BASED ON MODEL SCALE AND TRANSLATION
		XMVECTOR vec1 = XMLoadFloat3(&m_cornerArr[indices[i * 3]]);
		vec1 = XMVector3Transform(vec1, model);
		XMFLOAT4 point;
		XMStoreFloat4(&point, vec1);

		XMVECTOR vec2 = XMLoadFloat3(&m_cornerArr[indices[i * 3 +1]]);
		vec2 = XMVector3Transform(vec2, model);

		XMVECTOR vec3 = XMLoadFloat3(&m_cornerArr[indices[i * 3 +2]]);
		vec3 = XMVector3Transform(vec3, model);

		/*XMFLOAT3 VecA = XMFLOAT3(vec2.x-vec1.x, vec2.y - vec1.y, vec2.z- vec1.z);
		XMFLOAT3 VecB = XMFLOAT3(vec3.x-vec1.x, vec3.y - vec1.y, vec3.z- vec1.z);*/
		XMVECTOR vecA = vec2 - vec1;
		XMVECTOR vecB = vec3 - vec1;
		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(vecA, vecB));
		//XMVECTOR normal = XMVector3Cross(vecA, vecB);
		XMFLOAT3 Normal;
		XMStoreFloat3(&Normal, normal);

		float d = point.x*Normal.x + point.y * Normal.y + point.z * Normal.z;
		d *= -1;
		XMFLOAT4 coeff = XMFLOAT4(Normal.x, Normal.y, Normal.z, d);
		//XMVECTOR plane = XMVector4Normalize(XMLoadFloat4(&coeff));
		
		 
		//XMStoreFloat4(&coeff, plane);
		m_planeArr.push_back(coeff);
	}
}

Container::~Container() 
{
	if (m_debugRaster != nullptr) m_debugRaster->Release();
	if (m_vb) m_vb->Release();
	if (m_ib) m_ib->Release();
}

void Container::DrawContainer(Camera * camera) 
{
	static unsigned int offset = 0;
	static unsigned int stride = sizeof(Vertex);
	m_context->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);
	m_context->IASetIndexBuffer(m_ib, DXGI_FORMAT_R32_UINT, 0);

	m_context->RSSetState(m_debugRaster);
	m_VS->SetShader();
	m_PS->SetShader();
	m_VS->SetMatrix4x4("world", m_modelMatrix);
	m_VS->SetMatrix4x4("view", camera->GetView());
	m_VS->SetMatrix4x4("projection", camera->GetProjection());
	m_VS->CopyAllBufferData();
	m_context->DrawIndexed(30, 0, 0);
	m_context->RSSetState(0);
}