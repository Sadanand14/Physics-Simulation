#include "ParticleIncludes.hlsli"


struct VertexToPixel 
{
	float4 position : SV_POSITION;
};



float4 main(VertexToPixel input) : SV_TARGET
{
	//float4 Color = particleTex.Sample(Sampler, input.uv) * input.color;
	return float4(0.8f,0.8f,0.3f,1.0f);
}