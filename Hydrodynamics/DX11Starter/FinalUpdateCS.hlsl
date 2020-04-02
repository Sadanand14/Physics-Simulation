#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float dt;
}

RWStructuredBuffer<Particle> ParticlePool : register (u0);

[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	Particle particle = ParticlePool.Load(id.x);

	particle.Velocity += particle.Acceleration;
	particle.Acceleration = 0;
	particle.Position += particle.Velocity * dt;

	ParticlePool[id.x] = particle;
	return;
}