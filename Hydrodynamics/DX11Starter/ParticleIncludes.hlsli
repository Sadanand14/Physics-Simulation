#ifndef __PARTICLE_INCLUDES
#define __PARTICLE_INCLUDES

struct Particle
{
	float3 Position;
	float Density;
	float3 Velocity;
	float Mass;
	float3 Acceleration;
	float padding;
};


#endif