#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	float3 gravity;
	float kernel1;
	float h;
	float dt;
	int activeCount;
}

RWStructuredBuffer<Particle> ParticlePool : register (u0);

[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	if (id.x >= (uint)activeCount) { return; }

	Particle particle = ParticlePool.Load(id.x);

	float p0 = 1.0;

	float3 dir;
	float r2;
	Particle currentParticle;
	particle.Velocity += gravity * dt;
	particle.Density = 0;
	//calculate Density
	for (unsigned int i = 0; i < activeCount; ++i) 
	{
		currentParticle = ParticlePool.Load(i);
		dir = particle.Position - currentParticle.Position;
		r2 = dot(dir, dir);
		if ( r2 > 0 && r2 < (h * h) )
		{
			float W1 = kernel1 * pow(((h * h) - r2), 3);
			particle.Density += currentParticle.Mass * W1;
		}
	}
	particle.Density = max(p0, particle.Density);

	ParticlePool[id.x] = particle;
}