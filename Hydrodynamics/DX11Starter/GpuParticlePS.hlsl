#include "ParticleIncludes.hlsli"


struct VertexToPixel 
{
	float4 position : SV_POSITION;
};



float4 main(VertexToPixel input) : SV_TARGET
{
	//float4 Color = particleTex.Sample(Sampler, input.uv) * input.color;
	return float4(1.0f,0.0f,0.0f,1.0f);
}