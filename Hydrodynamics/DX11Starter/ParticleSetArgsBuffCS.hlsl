#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register (b0) 
{
	int vertsPerParticle;
}

RWBuffer<uint> DrawArgs					  : register(u0);
RWStructuredBuffer<ParticleDraw> DrawList : register(u1);

[numthreads(1, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	DrawArgs[0] = DrawList.IncrementCounter() * vertsPerParticle;
	DrawArgs[1] = 1;
	DrawArgs[2] = 0;
	DrawArgs[3] = 0;
	DrawArgs[4] = 0;
}