#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float4 planeArr[5];
	float dt;
	float diametre;
	int activeCount;
}

RWStructuredBuffer<Particle> ParticlePool : register (u0);

[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	Particle particle = ParticlePool.Load(id.x);
	particle.Velocity += particle.Acceleration;
	particle.Acceleration.x = 0.0f;
	particle.Acceleration.y = 0.0f;
	particle.Acceleration.z = 0.0f;

	float3 expectedVelocity = particle.Velocity;
	bool collision = false;

	for (uint i = 0; i < 5; ++i)
	{
		float3 particlePos = particle.Position;
		float currentDis = planeArr[i].x * particlePos.x + planeArr[i].y * particlePos.y + planeArr[i].z * particlePos.z
			+ planeArr[i].w - diametre / 2;

		if (currentDis > 0.1 * diametre) continue;

		float3 particleVel = expectedVelocity;
		particlePos += (particleVel * dt);
		float predictedDis = planeArr[i].x * (particlePos.x) +
			planeArr[i].y * (particlePos.y) +
			planeArr[i].z * (particlePos.z) +
			planeArr[i].w - diametre / 2;
			
		if (abs(predictedDis) < diametre * 0.05)
		{
			collision = true;
			//prevent particle from passing through the container boundary
			float3 normal = float3(planeArr[i].x, planeArr[i].y, planeArr[i].z);
			float dotProduct = dot(normalize(normal), particleVel);
			//dotProduct /= length(normal);
			dotProduct *= -1;
			float3 velocityToBeApplied = normalize(normal) * dotProduct;

			particle.Acceleration += 0.65 *velocityToBeApplied;
		}
	}
	expectedVelocity += particle.Acceleration;

	float3 predictedPos1, predictedPos2 , dir;
	float predictDiff;
	Particle particle2;
	
	//inter Particle Collision
	for (unsigned int i = 0; i < activeCount && i != id.x; ++i) 
	{
		predictedPos1 = particle.Position + expectedVelocity * dt;
		particle2 = ParticlePool.Load(i);
		predictedPos2 = particle2.Position + particle2.Velocity * dt;
		dir = predictedPos1 - predictedPos2;
		predictDiff = dot(dir, dir);

		if ( predictDiff <(diametre * diametre))
		{
			dir = normalize(particle.Position - particle2.Position);
			float3 vel = dot(particle.Velocity + particle.Acceleration, dir) * dir;
			expectedVelocity -= vel;

			vel = dot(particle2.Velocity, dir) * dir;
			expectedVelocity += vel;
		}
	}

	if (collision)
	{
		for (uint i = 0; i < 5; ++i)
		{
			float3 particlePos = particle.Position;
			float currentDis = planeArr[i].x * particlePos.x + planeArr[i].y * particlePos.y + planeArr[i].z * particlePos.z
				+ planeArr[i].w - diametre / 2;

			if (currentDis > 0.1 * diametre) continue;

			float3 particleVel = expectedVelocity;
			particlePos += (particleVel * dt);
			float predictedDis = planeArr[i].x * (particlePos.x) +
				planeArr[i].y * (particlePos.y) +
				planeArr[i].z * (particlePos.z) +
				planeArr[i].w - diametre / 2;

			if (abs(predictedDis) < diametre * 0.05)
			{
				collision = true;
				//prevent particle from passing through the container boundary
				float3 normal = float3(planeArr[i].x, planeArr[i].y, planeArr[i].z);
				float dotProduct = dot(normalize(normal), particleVel);
				//dotProduct /= length(normal);
				dotProduct *= -1;
				float3 velocityToBeApplied = normalize(normal) * dotProduct;

				expectedVelocity += velocityToBeApplied;
			}
		}
	}

	particle.Acceleration = expectedVelocity - particle.Velocity;

	ParticlePool[id.x] = particle;
	return;
}