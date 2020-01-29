#ifndef __PARTICLE_INCLUDES
#define __PARTICLE_INCLUDES

struct Particle
{
	float4 Color;
	float Age;
	float3 Position;
	float Size;
	float3 Velocity;
	float Alive;
	float3 padding;
};

struct ParticleDraw
{
	uint Index;
};

#endif