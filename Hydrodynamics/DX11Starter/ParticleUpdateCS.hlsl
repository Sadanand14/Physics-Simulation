#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	//float startSize;
	float4 startColor;
	float4 endColor;
	float dt;
	float lifeTime;
	float totalTime;
	float startSize;
	float endSize;
	int maxParticles;
}

RWStructuredBuffer<Particle> ParticlePool : register (u0);
AppendStructuredBuffer<uint> DeadList	  : register (u1);
RWStructuredBuffer<ParticleDraw> DrawList : register (u2);

[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	if (id.x >= (uint)maxParticles) return;

	Particle particle = ParticlePool.Load(id.x);

	if (particle.Alive == 0.0f) return;

	particle.Age += dt;
	particle.Alive = (float)(particle.Age < lifeTime);
	particle.Position += particle.Velocity * dt;

	particle.Age += dt;

	float agePercentage = particle.Age / lifeTime;
	particle.Color = startColor + (endColor - startColor) * agePercentage;
	particle.Size = startSize + (endSize - startSize) * agePercentage;

	ParticlePool[id.x] = particle;

	if (particle.Alive == 0.0f) 
	{
		DeadList.Append(id.x);
	}
	else 
	{
		uint drawIndex = DrawList.IncrementCounter();

		ParticleDraw drawData;
		drawData.Index = id.x;
		//drawData.DistanceSq = 0.0f;

		DrawList[drawIndex] = drawData;
	}
}