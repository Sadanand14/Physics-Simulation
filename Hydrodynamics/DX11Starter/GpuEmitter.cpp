#include "GpuEmitter.h"
#include <iostream>

float GPUEmitter::s_emitTimeCounter = 0;

GPUEmitter::GPUEmitter
(
	unsigned int maxParticles, unsigned int emitRate,
	DirectX::XMFLOAT3 emitterPos, DirectX::XMFLOAT3 startVel, DirectX::XMFLOAT3 posRange, DirectX::XMFLOAT3 velRange,
	ID3D11Device* device, ID3D11DeviceContext* context,
	SimpleComputeShader* updateParticles, SimpleComputeShader* emitParticles,
	SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader,
	ID3D11Buffer* vertexBuffer, unsigned int vertexCount, float modelWidth,
	std::vector<DirectX::XMFLOAT4> planeArr
)
	: m_maxParticles(maxParticles),m_context(context), m_emitParticleCS(emitParticles), m_updateParticleCS(updateParticles),
	m_VS(vertexShader), m_PS(pixelShader), m_vertexBuffer(vertexBuffer), m_vertexCount(vertexCount), m_currentCount(0), m_width(modelWidth),
	m_planeArr(planeArr)
{	

	m_emitterPos = emitterPos;
	m_startVel = startVel;
	m_posRange = posRange;
	m_velRange = velRange;
	m_context = context;

	m_emitRate = emitRate;

	m_timePerEmit = 1.0f / emitRate;

	s_emitTimeCounter = 0.0f;

	//create particle pool SRV and UAV
	ID3D11Buffer* particlePoolBuff;
	D3D11_BUFFER_DESC poolDesc = {};
	poolDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	poolDesc.ByteWidth = sizeof(GPUParticle) * m_maxParticles;
	poolDesc.CPUAccessFlags = 0;
	poolDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	poolDesc.StructureByteStride = sizeof(GPUParticle);
	poolDesc.Usage = D3D11_USAGE_DEFAULT;
	device->CreateBuffer(&poolDesc, 0, &particlePoolBuff);

	D3D11_UNORDERED_ACCESS_VIEW_DESC poolUAVDesc = {};
	poolUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	poolUAVDesc.Buffer.FirstElement = 0;
	poolUAVDesc.Buffer.Flags = 0;
	poolUAVDesc.Buffer.NumElements = m_maxParticles;
	poolUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	device->CreateUnorderedAccessView(particlePoolBuff, &poolUAVDesc, &m_particlePoolUAV);

	D3D11_SHADER_RESOURCE_VIEW_DESC poolSRVDesc = {};
	poolSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	poolSRVDesc.Buffer.FirstElement = 0;
	poolSRVDesc.Buffer.NumElements = m_maxParticles;
	poolSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	device->CreateShaderResourceView(particlePoolBuff, &poolSRVDesc, &m_particlePoolSRV);

	particlePoolBuff->Release();

}

GPUEmitter::~GPUEmitter()
{
	//release SRV 's
	if (m_particlePoolSRV)	m_particlePoolSRV->Release();

	//release UAV's
	if (m_particlePoolUAV)	m_particlePoolUAV->Release();

}

void GPUEmitter::Update(float dt, float totaltime)
{
	ID3D11UnorderedAccessView* none[8] = {};
	m_context->CSSetUnorderedAccessViews(0, 8, none, 0);
	
	s_emitTimeCounter += dt;
	if (s_emitTimeCounter >= m_timePerEmit && m_currentCount<= m_maxParticles)
	{
		int emitCount = (int)(s_emitTimeCounter / m_timePerEmit);
		emitCount = min(emitCount, 65535);
		s_emitTimeCounter = fmod(s_emitTimeCounter, m_timePerEmit);
		m_emitParticleCS->SetShader();
		m_emitParticleCS->SetFloat3("startPos", m_emitterPos);
		m_emitParticleCS->SetFloat3("posRange", m_posRange);
		m_emitParticleCS->SetFloat3("startVel", m_startVel);
		m_emitParticleCS->SetFloat3("velRange", m_velRange);
		m_emitParticleCS->SetFloat("startSize", m_startSize);
		m_emitParticleCS->SetInt("activeParticles",m_currentCount);
		m_emitParticleCS->SetInt("emitCount", emitCount);
		m_emitParticleCS->SetInt("maxParticle", m_maxParticles);
		m_emitParticleCS->SetUnorderedAccessView("ParticlePool", m_particlePoolUAV);
		m_emitParticleCS->CopyAllBufferData();
		m_emitParticleCS->DispatchByThreads(emitCount, 1, 1);

		m_currentCount += min(emitCount, (m_maxParticles-m_currentCount));
	}

	m_context->CSSetUnorderedAccessViews(0, 8, none, 0);

	//Update Particles
	m_updateParticleCS->SetShader();

	static DirectX::XMFLOAT3 gravity = DirectX::XMFLOAT3(0.0f,-9.8f,0.0f);
	static float separationSpeed = 2.0f;
	m_updateParticleCS->SetData("planeArr", m_planeArr.data(), 5 * sizeof(XMFLOAT4));
	m_updateParticleCS->SetFloat("separationSpeed", separationSpeed);
	m_updateParticleCS->SetFloat("dt", dt);
	m_updateParticleCS->SetFloat3("gravity", gravity);
	m_updateParticleCS->SetFloat("diametre", m_width);
	m_updateParticleCS->SetInt("activeCount", m_currentCount);
	m_updateParticleCS->SetUnorderedAccessView("ParticlePool", m_particlePoolUAV);
	m_updateParticleCS->CopyAllBufferData();
	m_updateParticleCS->DispatchByThreads(m_currentCount, 1, 1);

	m_context->CSSetUnorderedAccessViews(0, 8, none, 0);

}

void GPUEmitter::Draw(Camera* camera)
{
	//m_context->OMSetBlendState(m_blendState, 0, 0xFFFFFFFF);
	//m_context->OMSetDepthStencilState(m_depthState, 0);

	static unsigned int stride = sizeof(Vertex);
	static unsigned int offset = 0;
	m_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	m_context->VSSetShaderResources(0, 1, &m_particlePoolSRV);

	m_VS->SetShader();
	m_VS->SetMatrix4x4("world", DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
	m_VS->SetMatrix4x4("view", camera->GetView());
	m_VS->SetMatrix4x4("projection", camera->GetProjection());
	m_VS->CopyAllBufferData();

	m_PS->SetShader();
	m_context->DrawInstanced(m_vertexCount, m_currentCount, 0, 0);

	ID3D11ShaderResourceView* none[16] = {};
	m_context->VSSetShaderResources(0, 16, none);

	
//	m_context->OMSetBlendState(0, 0, 0xFFFFFFFF);
	//m_context->OMSetDepthStencilState(0, 0);
}