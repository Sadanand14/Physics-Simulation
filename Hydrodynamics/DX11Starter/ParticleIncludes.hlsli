#ifndef __PARTICLE_INCLUDES
#define __PARTICLE_INCLUDES

struct Particle
{
	float3 Position;
	float Density;
	float3 Velocity;
	float Mass;
	float Pressure;
	float3 garbage;
};

//struct ParticleDraw
//{
//	uint Index;
//};

#endif