#include "GpuEmitter.h"

float GPUEmitter::s_emitTimeCounter = 0;

GPUEmitter::GPUEmitter
(
	unsigned int maxParticles, unsigned int emitRate, float lifeTime, float startSize, float EndSize,
	DirectX::XMFLOAT3 emitterPos, DirectX::XMFLOAT3 startVel, DirectX::XMFLOAT3 posRange, DirectX::XMFLOAT3 velRange,
	DirectX::XMFLOAT4 rotRange, DirectX::XMFLOAT4 startColor, DirectX::XMFLOAT4 endColor,
	ID3D11Device* device, ID3D11DeviceContext* context,
	SimpleComputeShader* initParticles, SimpleComputeShader* updateParticles, SimpleComputeShader* emitParticles,
	SimpleComputeShader* updateArgsBuffer, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* texture
)
{
	m_startSize = startSize;
	m_endSize = EndSize;
	m_emitterPos = emitterPos;
	m_startVel = startVel;
	m_posRange = posRange;
	m_velRange = velRange;
	m_rotRange = rotRange;
	m_startColor = startColor;
	m_endColor = endColor;
	m_context = context;
	m_texture = texture;

	m_maxParticles = maxParticles;
	m_emitRate = emitRate;
	m_lifeTime = lifeTime;
	m_initParticlesCS = initParticles;
	m_updateParticleCS = updateParticles;
	m_emitParticleCS = emitParticles;
	m_updateArgsBufferCS = updateArgsBuffer;
	m_VS = vertexShader;
	m_PS = pixelShader;

	m_timePerEmit = 1.0f / emitRate;

	s_emitTimeCounter = 0.0f;

	//index buffer creation
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.ByteWidth = sizeof(unsigned long) * 6 * m_maxParticles;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;

	unsigned long* indexArr = new unsigned long[m_maxParticles * 6];
	unsigned long index = 0;
	for (unsigned long i = 0; i < m_maxParticles; i++)
	{
		indexArr[index++] = 0 + 4 * i;
		indexArr[index++] = 1 + 4 * i;
		indexArr[index++] = 2 + 4 * i;
		indexArr[index++] = 0 + 4 * i;
		indexArr[index++] = 2 + 4 * i;
		indexArr[index++] = 3 + 4 * i;
	}

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indexArr;
	device->CreateBuffer(&ibDesc, &indexData, &m_indexBuff);

	delete[] indexArr;


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

	//DeadList
	ID3D11Buffer* deadBuffer;
	D3D11_BUFFER_DESC deadDesc = {};
	deadDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	deadDesc.ByteWidth = sizeof(unsigned int) * m_maxParticles;
	deadDesc.CPUAccessFlags = 0;
	deadDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	deadDesc.StructureByteStride = sizeof(unsigned int);
	deadDesc.Usage = D3D11_USAGE_DEFAULT;
	device->CreateBuffer(&deadDesc, 0, &deadBuffer);

	D3D11_UNORDERED_ACCESS_VIEW_DESC deadUAVDesc = {};
	deadUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	deadUAVDesc.Buffer.FirstElement = 0;
	deadUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	deadUAVDesc.Buffer.NumElements = m_maxParticles;
	deadUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	device->CreateUnorderedAccessView(deadBuffer, &deadUAVDesc, &m_deadParticleUAV);

	deadBuffer->Release();

	//DrawList
	ID3D11Buffer* drawBuff;
	D3D11_BUFFER_DESC drawDesc = {};
	drawDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	drawDesc.ByteWidth = sizeof(ParticleSort) * m_maxParticles;
	drawDesc.CPUAccessFlags = 0;
	drawDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	drawDesc.StructureByteStride = sizeof(ParticleSort);
	drawDesc.Usage = D3D11_USAGE_DEFAULT;
	device->CreateBuffer(&drawDesc, 0, &drawBuff);

	D3D11_UNORDERED_ACCESS_VIEW_DESC drawUAVDesc = {};
	drawUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	drawUAVDesc.Buffer.FirstElement = 0;
	drawUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	drawUAVDesc.Buffer.NumElements = m_maxParticles;
	drawUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	device->CreateUnorderedAccessView(drawBuff, &drawUAVDesc, &m_drawParticleUAV);

	D3D11_SHADER_RESOURCE_VIEW_DESC drawSRVDesc = {};
	drawSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	drawSRVDesc.Buffer.FirstElement = 0;
	drawSRVDesc.Buffer.NumElements = m_maxParticles;
	drawSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	device->CreateShaderResourceView(drawBuff, &drawSRVDesc, &m_drawParticleSRV);

	drawBuff->Release();

	{

		//draw Argument Buffer
		D3D11_BUFFER_DESC drawArgsDesc = {};
		drawArgsDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		drawArgsDesc.ByteWidth = sizeof(unsigned int) * 5;
		drawArgsDesc.CPUAccessFlags = 0;
		drawArgsDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		drawArgsDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateBuffer(&drawArgsDesc, 0, &m_drawArgsBuff);

		D3D11_UNORDERED_ACCESS_VIEW_DESC drawArgUAVDesc = {};
		drawArgUAVDesc.Format = DXGI_FORMAT_R32_UINT;
		drawArgUAVDesc.Buffer.FirstElement = 0;
		drawArgUAVDesc.Buffer.Flags = 0;
		drawArgUAVDesc.Buffer.NumElements = 5;
		drawArgUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(m_drawArgsBuff, &drawArgUAVDesc, &m_drawArgsUAV);
	}

	D3D11_BLEND_DESC blendStateDesc = {};
	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.IndependentBlendEnable = false;
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendStateDesc, &m_blendState);

	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.DepthEnable = true;
	device->CreateDepthStencilState(&depthDesc, &m_depthState);

	//initialize the dead list
	m_initParticlesCS->SetShader();
	m_initParticlesCS->SetInt("maxParticles", m_maxParticles);
	m_initParticlesCS->SetUnorderedAccessView("DeadList", m_deadParticleUAV);
	m_initParticlesCS->CopyAllBufferData();
	m_initParticlesCS->DispatchByThreads(m_maxParticles, 1, 1);
}

GPUEmitter::~GPUEmitter()
{
	//release misc buffers
	if (m_depthState)		m_depthState->Release();
	if (m_blendState)		m_blendState->Release();
	if (m_indexBuff)		m_indexBuff->Release();
	if (m_drawArgsBuff)		m_drawArgsBuff->Release();

	//release SRV 's
	if (m_particlePoolSRV)	m_particlePoolSRV->Release();
	if (m_drawParticleSRV)	m_drawParticleSRV->Release();

	//release UAV's
	if (m_particlePoolUAV)	m_particlePoolUAV->Release();
	if (m_drawParticleUAV)	m_drawParticleUAV->Release();
	if (m_deadParticleUAV)	m_deadParticleUAV->Release();
	if (m_drawArgsUAV)		m_drawArgsUAV->Release();

}

void GPUEmitter::Update(float dt, float totaltime)
{
	ID3D11UnorderedAccessView* none[8] = {};
	m_context->CSSetUnorderedAccessViews(0, 8, none, 0);

	s_emitTimeCounter += dt;

	if (s_emitTimeCounter >= m_timePerEmit)
	{
		int emitCount = (int)(s_emitTimeCounter / m_timePerEmit);
		emitCount = min(emitCount, 65535);

		s_emitTimeCounter = fmod(s_emitTimeCounter, m_timePerEmit);

		m_emitParticleCS->SetShader();
		m_emitParticleCS->SetFloat3("startPos", m_emitterPos);
		m_emitParticleCS->SetFloat3("posRange", m_posRange);
		m_emitParticleCS->SetFloat3("startVel", m_startVel);
		m_emitParticleCS->SetFloat3("velRange", m_velRange);
		m_emitParticleCS->SetFloat4("startColor", m_startColor);
		m_emitParticleCS->SetFloat("startSize", m_startSize);
		m_emitParticleCS->SetFloat("totalTime", totaltime);
		m_emitParticleCS->SetInt("emitCount", emitCount);
		m_emitParticleCS->SetInt("maxParticle", m_maxParticles);
		m_emitParticleCS->SetUnorderedAccessView("ParticlePool", m_particlePoolUAV);
		m_emitParticleCS->SetUnorderedAccessView("DeadList", m_deadParticleUAV);
		m_emitParticleCS->CopyAllBufferData();
		m_emitParticleCS->DispatchByThreads(emitCount, 1, 1);
	}

	m_context->CSSetUnorderedAccessViews(0, 8, none, 0);

	//Update Particles
	m_updateParticleCS->SetShader();

	m_updateParticleCS->SetFloat4("startColor", m_startColor);
	m_updateParticleCS->SetFloat4("endColor", m_endColor);
	m_updateParticleCS->SetFloat("dt", dt);
	m_updateParticleCS->SetFloat("totalTime", totaltime);
	m_updateParticleCS->SetFloat("lifeTime", m_lifeTime);
	m_updateParticleCS->SetFloat("startSize", m_startSize);
	m_updateParticleCS->SetFloat("endSize", m_endSize);
	m_updateParticleCS->SetInt("maxParticles", m_maxParticles);
	m_updateParticleCS->SetUnorderedAccessView("ParticlePool", m_particlePoolUAV);
	m_updateParticleCS->SetUnorderedAccessView("DeadList", m_deadParticleUAV);
	m_updateParticleCS->SetUnorderedAccessView("DrawList", m_drawParticleUAV, 0);
	m_updateParticleCS->CopyAllBufferData();
	m_updateParticleCS->DispatchByThreads(m_maxParticles, 1, 1);

	m_context->CSSetUnorderedAccessViews(0, 8, none, 0);

	m_updateArgsBufferCS->SetShader();
	m_updateArgsBufferCS->SetInt("vertsPerParticle", 6);
	m_updateArgsBufferCS->SetUnorderedAccessView("DrawArgs", m_drawArgsUAV);
	m_updateArgsBufferCS->SetUnorderedAccessView("DrawList", m_drawParticleUAV);
	m_updateArgsBufferCS->CopyAllBufferData();
	m_updateArgsBufferCS->DispatchByThreads(1, 1, 1);

	m_context->CSSetUnorderedAccessViews(0, 8, none, 0);
}

void GPUEmitter::Draw(Camera* camera)
{
	m_context->OMSetBlendState(m_blendState, 0, 0xFFFFFFFF);
	m_context->OMSetDepthStencilState(m_depthState, 0);

	m_context->IASetIndexBuffer(m_indexBuff, DXGI_FORMAT_R32_UINT, 0);

	m_context->VSSetShaderResources(0, 1, &m_particlePoolSRV);
	m_context->VSSetShaderResources(1, 1, &m_drawParticleSRV);

	m_VS->SetShader();
	m_VS->SetMatrix4x4("world", DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
	m_VS->SetMatrix4x4("view", camera->GetView());
	m_VS->SetMatrix4x4("projection", camera->GetProjection());
	m_VS->CopyAllBufferData();

	m_PS->SetShader();
	m_PS->SetShaderResourceView("particleTex", m_texture);
	m_PS->SetSamplerState("Sampler", Texture::m_sampler);
	m_context->DrawIndexedInstancedIndirect(m_drawArgsBuff, 0);

	ID3D11ShaderResourceView* none[16] = {};
	m_context->VSSetShaderResources(0, 16, none);

	
	m_context->OMSetBlendState(0, 0, 0xFFFFFFFF);
	m_context->OMSetDepthStencilState(0, 0);
}