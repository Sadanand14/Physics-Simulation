#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float4 startColor;

	float3 startPos;
	int emitCount;

	float3 posRange;
	float totalTime;

	float3 startVel;
	float startSize;

	float3 velRange;
	int maxParticle;

}

RWStructuredBuffer<Particle> ParticlePool : register(u0);
ConsumeStructuredBuffer<uint> DeadList	  : register(u1);

float rand(float2 co) {
	return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}

[numthreads(32, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	if (id.x >= (uint) emitCount)
	{
		return;
	}

	uint emitIndex = DeadList.Consume();

	//random number generation
	float random[6];
	for (unsigned int i = 0; i < 6; i++)
	{
		random[i] = rand((float2((float)emitIndex, (float)i)));
	}

	Particle emitParticle = ParticlePool.Load(emitIndex);

	emitParticle.Age = 0.0f;
	emitParticle.Size = startSize;
	emitParticle.Color = startColor;
	emitParticle.Alive = 1.0f;

	emitParticle.Velocity.x = startVel.x + (((random[0] - 0.5) * 4) - 1)* velRange.x;
	emitParticle.Velocity.y = startVel.y + (((random[1] - 0.5) * 4) - 1)* velRange.y;
	emitParticle.Velocity.z = startVel.z + (((random[2] - 0.5) * 4) - 1)* velRange.z;

	emitParticle.Position.x = startPos.x + (((random[3] - 0.5) * 4) - 1)* posRange.x;
	emitParticle.Position.y = startPos.y + (((random[4] - 0.5) * 4) - 1) * posRange.y;
	emitParticle.Position.z = startPos.z + (((random[5] - 0.5) * 4) - 1) * posRange.z;

	ParticlePool[emitIndex] = emitParticle;
}