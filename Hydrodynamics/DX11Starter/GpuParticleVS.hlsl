#include "ParticleIncludes.hlsli"

cbuffer externalDara : register (b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

StructuredBuffer<Particle>		ParticlePool	: register(t0);

struct VS_Input 
{
	
};

struct VertexToPixel
{
	float4 position	: SV_POSITION;
};

VertexToPixel main(VS_Input input, uint instanceID : SV_InstanceID)
{
	VertexToPixel output;

	matrix wvp = mul(mul(world, view), projection);
	output.position = mul(float4(pos, 1.0f), wvp);

	return output;
}