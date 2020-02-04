#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float3 startPos;
	int emitCount;

	float3 posRange;
	int activeParticles;

	float3 startVel;
	int maxParticle;

	float3 velRange;
	float startSize;
}

RWStructuredBuffer<Particle> ParticlePool : register(u0);

float rand(float2 co) {
	return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}

[numthreads(32, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	uint particleIndex = id.x + activeParticles;
	uint totalCount = emitCount + activeParticles;
	if (particleIndex >= (uint)totalCount)
		return;
	
	//random number generation
	float random[6];
	for (unsigned int i = 0; i < 6; i++)
	{
		random[i] = rand((float2((float)particleIndex, (float)i)));
	}

	Particle emitParticle = ParticlePool.Load(particleIndex);

	emitParticle.Size = startSize;
	emitParticle.Alive = 1.0f;

	emitParticle.Velocity.x = startVel.x + (((random[0] - 0.5) * 4) - 1)* velRange.x;
	emitParticle.Velocity.y = startVel.y + (((random[1] - 0.5) * 4) - 1)* velRange.y;
	emitParticle.Velocity.z = startVel.z + (((random[2] - 0.5) * 4) - 1)* velRange.z;

	emitParticle.Position.x = startPos.x + (((random[3] - 0.5) * 4) - 1)* posRange.x;
	emitParticle.Position.y = startPos.y + (((random[4] - 0.5) * 4) - 1) * posRange.y;
	emitParticle.Position.z = startPos.z + (((random[5] - 0.5) * 4) - 1) * posRange.z;

	ParticlePool[particleIndex] = emitParticle;
}