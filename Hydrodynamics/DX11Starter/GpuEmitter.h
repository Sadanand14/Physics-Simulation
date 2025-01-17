#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "SimpleShader.h"
#include "Camera.h"
#include "Textures.h"

struct GPUParticle 
{
	DirectX::XMFLOAT3 position;
	float Density;
	DirectX::XMFLOAT3 velocity;
	float Mass;
	DirectX::XMFLOAT3 wallVel;
	float padding;
};

struct ParticleSort
{
	int index;
};

class GPUEmitter 
{
private:

	static float s_emitTimeCounter;
	unsigned int m_maxParticles, m_emitRate, m_currentCount, m_vertexCount;
	float m_timePerEmit, m_lifeTime, m_width, m_kernel1,m_kernel2,m_h;

	//emitterDescriptors
	float m_startSize, m_endSize;
	DirectX::XMFLOAT3 m_emitterPos, m_startRot, m_startVel , m_posRange, m_velRange;
	DirectX::XMFLOAT4 m_startColor, m_endColor, m_rotRange;
	std::vector<XMFLOAT4> m_planeArr;

	SimpleComputeShader* m_initParticlesCS = nullptr, * m_updateParticleCS = nullptr, * m_emitParticleCS = nullptr,
		* m_updateArgsBufferCS = nullptr, * m_fluidUpdateCS = nullptr, * m_collisionCS = nullptr,
		* m_finalUpdateCS = nullptr;

	SimpleVertexShader* m_VS = nullptr;
	SimplePixelShader* m_PS = nullptr;
	ID3D11DeviceContext* m_context = nullptr;


	ID3D11Buffer* m_indexBuff = nullptr , * m_drawArgsBuff = nullptr, *m_vertexBuffer = nullptr;
	ID3D11UnorderedAccessView* m_particlePoolUAV = nullptr, * m_deadParticleUAV = nullptr , * m_drawParticleUAV = nullptr, * m_drawArgsUAV = nullptr;
	ID3D11ShaderResourceView* m_particlePoolSRV = nullptr, * m_drawParticleSRV = nullptr;
	ID3D11DepthStencilState* m_depthState = nullptr;
	ID3D11BlendState* m_blendState = nullptr;

public:

	GPUEmitter
	(
		unsigned int maxParticles, unsigned int emitRate,
		DirectX::XMFLOAT3 emitterPos, DirectX::XMFLOAT3 startVel, DirectX::XMFLOAT3 posRange, DirectX::XMFLOAT3 velRange,
		ID3D11Device* device, ID3D11DeviceContext* context,
		SimpleComputeShader* updateParticles, SimpleComputeShader* emitParticles,
		SimpleComputeShader* fluidUpdate, SimpleComputeShader* collisionUpdate,
		SimpleComputeShader* finalUpdate, SimpleVertexShader* vertexShader,
		SimplePixelShader* pixelShader, ID3D11Buffer* vertexBuffer, 
		unsigned int vertexCount, float modelWidth,
		std::vector<DirectX::XMFLOAT4> planeArr
	);

	~GPUEmitter();

	void Update(float deltaTime, float totalTime);

	void Draw(Camera* camera);
};