#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	float dt;
	int activeCount;
	float2 trash;
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
	return;
	//particle.Age += dt;
	//particle.Alive = (float)(particle.Age < lifeTime);
	

	//particle.Age += dt;

	//float agePercentage = particle.Age / lifeTime;
	//particle.Color = startColor + (endColor - startColor) * agePercentage;
	//particle.Size = startSize + (endSize - startSize) * agePercentage;

	//ParticlePool[id.x] = particle;


	//uint drawIndex = DrawList.IncrementCounter();

	//ParticleDraw drawData;
	//drawData.Index = id.x;
	////drawData.DistanceSq = 0.0f;

	//DrawList[drawIndex] = drawData;
}