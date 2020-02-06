#include "ParticleIncludes.hlsli"

cbuffer externalDara : register (b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

StructuredBuffer<Particle>ParticlePool	: register(t0);

struct VS_Input 
{
	float3 position	: POSITION;
};

struct VertexToPixel
{
	float4 position	: SV_POSITION;
};

VertexToPixel main(VS_Input input, uint instanceID : SV_InstanceID)
{
	VertexToPixel output;
	Particle particle = ParticlePool.Load(instanceID);
	float3 inPos = input.position + particle.Position;

	matrix wvp = mul(mul(world, view), projection);
	output.position = mul(float4(inPos, 1.0f), wvp);

	return output;
}