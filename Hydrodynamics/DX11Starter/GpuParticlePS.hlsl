#include "ParticleIncludes.hlsli"


struct VertexToPixel 
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};

Texture2D particleTex : register (t0);
SamplerState Sampler : register (s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	float4 Color = particleTex.Sample(Sampler, input.uv) * input.color;
	return Color;

	//return float4(1.0f,0.0f,0.0f,1.0f);
}