#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	float4 planeArr[5];
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

	//collision check with other particles
	for (int i = 0; i < activeCount && i!=id.x; ++i) 
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
	}

	//collision check with container
	for (uint i = 0; i < 5; ++i) 
	{
		float3 particlePos = particle.Position;
		float currentDis = planeArr[i].x * particlePos.x + planeArr[i].y * particlePos.y + planeArr[i].z * particlePos.z 
							+ planeArr[i].w - diametre / 2;

		if (currentDis > 0.1 * diametre) continue;
		
		float3 particleVel = particle.Velocity;
		particlePos += (particleVel * dt);
		float predictedDis = planeArr[i].x * (particlePos.x) + 
							 planeArr[i].y * (particlePos.y) +
							 planeArr[i].z * (particlePos.z) +
							 planeArr[i].w - diametre / 2;
		if (predictedDis < diametre * 0.05) 
		{
			//prevent particle from passing through the container boundary
			float3 normal = float3(planeArr[i].x, planeArr[i].y, planeArr[i].z);
			float dotProduct = dot(normal, particleVel);
			dotProduct /= length(normal);
			dotProduct *= -1;
			float3 velocityToBeApplied = normalize(normal)* dotProduct;
			particle.Velocity += velocityToBeApplied;
		}
	}

	ParticlePool[id.x] = particle;

	return;
}