#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register (b0) 
{
	int vertsPerParticle;
}

RWBuffer<uint> DrawArgs					  : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{

}