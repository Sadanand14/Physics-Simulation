#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "SimpleShader.h"
#include "Camera.h"
#include "Textures.h"

struct GPUParticle 
{
	DirectX::XMFLOAT4 Color;
	float Age;
	DirectX::XMFLOAT3 position;
	float Size;
	DirectX::XMFLOAT3 velocity;
	float Alive;
	DirectX::XMFLOAT3 padding;
};

struct ParticleSort
{
	int index;
};

class GPUEmitter 
{
private:

	static float s_emitTimeCounter;
	unsigned int m_maxParticles, m_emitRate;
	float m_timePerEmit, m_lifeTime;

	//emitterDescriptors
	float m_startSize, m_endSize;
	DirectX::XMFLOAT3 m_emitterPos, m_startRot, m_startVel , m_posRange, m_velRange;
	DirectX::XMFLOAT4 m_startColor, m_endColor, m_rotRange;

	SimpleComputeShader* m_initParticlesCS = nullptr,* m_updateParticleCS = nullptr,* m_emitParticleCS = nullptr,* m_updateArgsBufferCS = nullptr;
	SimpleVertexShader* m_VS = nullptr;
	SimplePixelShader* m_PS = nullptr;
	ID3D11DeviceContext* m_context = nullptr;


	ID3D11Buffer* m_indexBuff = nullptr , * m_drawArgsBuff = nullptr;
	ID3D11UnorderedAccessView* m_particlePoolUAV = nullptr, * m_deadParticleUAV = nullptr , * m_drawParticleUAV = nullptr, * m_drawArgsUAV = nullptr;
	ID3D11ShaderResourceView* m_particlePoolSRV = nullptr, * m_drawParticleSRV = nullptr, * m_texture = nullptr;
	ID3D11DepthStencilState* m_depthState = nullptr;
	ID3D11BlendState* m_blendState = nullptr;

public:

	GPUEmitter
	(
		unsigned int maxParticles, unsigned int emitRate, float lifeTime, float startSize, float EndSize,
		DirectX::XMFLOAT3 emitterPos, DirectX::XMFLOAT3 startVel, DirectX::XMFLOAT3 posRange, DirectX::XMFLOAT3 velRange,
		DirectX::XMFLOAT4 rotRange, DirectX::XMFLOAT4 startColor, DirectX::XMFLOAT4 endColor,
		ID3D11Device* device, ID3D11DeviceContext* context,
		SimpleComputeShader* initParticles, SimpleComputeShader* updateParticles, SimpleComputeShader* emitParticles, 
		SimpleComputeShader* updateArgsBuffer, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* texture
	);

	~GPUEmitter();

	void Update(float deltaTime, float totalTime);

	void Draw(Camera* camera);
};