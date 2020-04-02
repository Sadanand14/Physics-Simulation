#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float3 gravity;
	float dt;
	float diametre;
	float kernel1;
	float kernel2;
	float h;
	int activeCount;
}

RWStructuredBuffer<Particle> ParticlePool : register (u0);


[numthreads(32, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	if (id.x >= (uint)activeCount) { return; }

	Particle particle = ParticlePool.Load(id.x);
	particle.Acceleration = 0;

	float K = 500.0f, p0 = 1.0f, e = 0.016f;

	float3 dir;

	//Fluid Behaviour
	//////////////////////////////////////////////////////////////////////////////////
	float3 P_Force = 0;
	float3 V_Force = 0;

	float pressure1 = K * (particle.Density - p0);
	float pressure2 = 0.0f;

	
	float W2, W3, r3, r2, r;
	Particle currentParticle;
	for (unsigned int i = 0; i < activeCount && i != id.x; ++i)
	{
		currentParticle = ParticlePool.Load(i);
		pressure2 = K * (currentParticle.Density - p0);

		dir = particle.Position - currentParticle.Position;
		r2 = dot(dir, dir);
		r = sqrt(r2);
		if (r > 0 && r < h)
		{
			const float3 rNorm = normalize(dir) ;
			const float W2 = kernel2 * pow(h - r, 2);
			const float r3 = r2 * r;
			const float W3 = -(r3 / (2 * h*h*h)) + (r2 / h*h) + (h / (2 * r)) - 1;

			//add pressure forces;
			P_Force +=(currentParticle.Mass/particle.Mass)*((pressure1 + pressure2)/ (2 * currentParticle.Density * particle.Density))
				*W2*(rNorm);

			//add Viscous Forces
			//V_Force += (currentParticle.Mass / particle.Mass) * (1.0f / currentParticle.Density) * (currentParticle.Velocity - particle.Velocity) * W3 * rNorm;
		}

	}
	P_Force *= -1;
	V_Force *= e;

	particle.Acceleration = dt *(((P_Force + V_Force) / particle.Density));

	ParticlePool[id.x] = particle;

	return;
}

