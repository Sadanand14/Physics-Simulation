#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	float3 gravity;
	float dt;
	float diametre;
	float separationSpeed;
	int activeCount;
}

RWStructuredBuffer<Particle> ParticlePool : register (u0);

[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	//if (id.x >= (uint)activeCount) { return; }

	Particle particle = ParticlePool.Load(id.x);

	if (particle.Alive == 0.0f) return;

	//update the initialized particles
	particle.Position += particle.Velocity * dt;
	particle.Velocity += gravity*dt;

	int r, rx,ry,rz;
	Particle currentParticle;
	for (uint i = 0; i < activeCount && i!=id.x; ++i) 
	{
		currentParticle = ParticlePool.Load(i);
		rx = currentParticle.Position.x - particle.Position.x;
		ry = currentParticle.Position.y - particle.Position.y;
		rz = currentParticle.Position.z - particle.Position.z;
		r = sqrt(rx*rx + ry*ry + rz*rz);
		if (r < diametre) 
		{
			float3 direction = particle.Position - currentParticle.Position;
			direction = normalize(direction);

			particle.Velocity += direction * separationSpeed * dt;
		}
		else 
		{
			//TODO::collision Check
		}
	}

	ParticlePool[id.x] = particle;

	return;
}