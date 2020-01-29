#include "ParticleIncludes.hlsli"

cbuffer externalDara : register (b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

StructuredBuffer<Particle>		ParticlePool	: register(t0);
StructuredBuffer<ParticleDraw>  DrawList		: register(t1);

struct VertexToPixel
{
	float4 position	: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
};

VertexToPixel main( uint id : SV_VertexID)
{

	VertexToPixel output;

	uint particleID = id / 4;
	uint cornerID = id % 4;

	ParticleDraw draw = DrawList.Load(particleID);
	Particle particle = ParticlePool.Load(draw.Index);

	float2 offsets[4];
	offsets[0] = float2(-1.0f, 1.0f);
	offsets[1] = float2(1.0f, 1.0f);
	offsets[2] = float2(1.0f, -1.0f);
	offsets[3] = float2(-1.0f, -1.0f);

	float3 pos = particle.Position;
	pos += float3(view._11, view._21, view._31) * offsets[cornerID].x * particle.Size;
	pos += float3(view._12, view._22, view._32) * offsets[cornerID].y * particle.Size;

	matrix wvp = mul(mul(world, view), projection);
	output.position = mul(float4(pos, 1.0f), wvp);

	float2 uvs[4];
	uvs[0] = float2(0, 0);
	uvs[1] = float2(1, 0);
	uvs[2] = float2(1, 1);
	uvs[3] = float2(0, 1);

	output.color = particle.Color;
	output.uv = saturate(uvs[cornerID]);

	return output;
}