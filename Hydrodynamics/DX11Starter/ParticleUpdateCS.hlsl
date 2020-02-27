#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	float4 planeArr[5];
	float3 gravity;
	float dt;
	float diametre;
	float separationSpeed;
	float kernel1;
	float kernel2;
	float h;
	int activeCount;
}

RWStructuredBuffer<Particle> ParticlePool : register (u0);

const float K = 250.0f, p0 = 1.0f;


[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	if (id.x >= (uint)activeCount) { return; }

	Particle particle = ParticlePool.Load(id.x);

	/*if (particle.Alive == 0.0f) return;*/

	//update the initialized particles
	particle.Position += particle.Velocity * dt;
	particle.Velocity += gravity*dt;

	float r;
	Particle currentParticle;

	for (unsigned int i = 0; i < activeCount; ++i) 
	{
		currentParticle = ParticlePool.Load(i);
		const float3 dir = particle.Position - currentParticle.Position;
		const float r2 = dot(dir, dir);
		if (r2 < (h * h)) 
		{
			const float W1 = kernel1 * pow(((h * h) - r2), 3);
			particle.Density += currentParticle.Mass * W1;
		}
	}

	particle.Density = max(p0, particle.Density);

	particle.Pressure = K * (particle.Density - p0);

	particle.P_Force = 0;


	for (unsigned int i = 0; i < activeCount&& i!=id.x; ++i)
	{
		currentParticle = ParticlePool.Load(i);
		const float3 dir = particle.Position - currentParticle.Position;
		const float r2 = dot(dir, dir);
		const float r = sqrt(r2);
		if (r2 < (h * h))
		{
			const float3 rNorm = dir / r;
			const float W2 = kernel2 * pow(h - r, 2);
			particle.P_Force += (currentParticle.Mass / particle.Mass) * (2 * particle.Density * currentParticle.Density) * W2 * rNorm;
		}
	}	

	particle.P_Force *= -1;
	
	//int wallCollision = 0, intersection = 0, internalCollision = 0;
	//float3 blockedMoveDirections[5];

	////collision detection with walls
	//for (uint i = 0; i < 5; ++i)
	//{
	//	float3 particlePos = particle.Position;
	//	float currentDis = planeArr[i].x * particlePos.x + planeArr[i].y * particlePos.y + planeArr[i].z * particlePos.z
	//		+ planeArr[i].w - diametre / 2;

	//	if (currentDis > 0.1 * diametre) continue;

	//	float3 particleVel = particle.Velocity;
	//	particlePos += (particleVel * dt);
	//	float predictedDis = planeArr[i].x * (particlePos.x) +
	//		planeArr[i].y * (particlePos.y) +
	//		planeArr[i].z * (particlePos.z) +
	//		planeArr[i].w - diametre / 2;
	//	if (predictedDis < diametre * 0.05)
	//	{
	//		wallCollision = 1;
	//		blockedMoveDirections[i] = float3(planeArr[i].x, planeArr[i].y, planeArr[i].z);
	//	}
	//	else 
	//	{
	//		blockedMoveDirections[i] = float3(0.0f, 0.0f, 0.0f);
	//	}
	//}

	//float3 dir;

	////collision detection with other particles
	//for (int i = 0; i < activeCount && i!=id.x; ++i) 
	//{
	//	//get current Particle
	//	currentParticle = ParticlePool.Load(i);

	//	//get distance between the two particles
	//	dir = currentParticle.Position - particle.Position;
	//	r = sqrt(dot(dir, dir));

	//	//if they are intersecting already, push them away
	//	if (r < diametre) 
	//	{
	//		intersection = 1;
	//	}
	//	//if they are close to intersecting, conserve their momentum in the direction of the line joining their centres
	//	else if(r < diametre * 1.05) 
	//	{
	//		float3 postPos1 = particle.Position + particle.Velocity * dt;
	//		float3 postPos2 = currentParticle.Position + currentParticle.Velocity * dt;
	//		float postR;
	//		float3 postDir;
	//		postDir = postPos1 - postPos2;
	//		postR = sqrt(dot(postDir,postDir));
	//		if (postR < diametre) 
	//		{
	//			float3 direction = normalize(particle.Position - currentParticle.Position);
	//			float speed1 = dot(particle.Velocity, direction);
	//			float speed2 = dot(currentParticle.Velocity, direction);
	//			float diff1 = (speed2 - speed1) / 2;
	//			float diff2 = (speed1 - speed2) / 2;
	//			float3 vel1 = diff1 * direction;
	//			float3 vel2 = diff2 * direction;

	//			for (unsigned int j = 0; j < 5; ++j) 
	//			{
	//				float3 blockDirection = blockedMoveDirections[j];
	//				float3 blockedVel = -1*dot(blockDirection, vel1) * blockDirection;
	//				vel1 += blockedVel;
	//				float3 addedVel = dot(blockedVel, direction) * direction;
	//				vel2 += addedVel;
	//				//TODO::add the component of blocked vel perpendicular to addedVel to vel 1
	//			}

	//			particle.Velocity += vel1;
	//			currentParticle.Velocity += vel2;
	//		}
	//		ParticlePool[i] = currentParticle;
	//	}
	//}

	//collision check with container
	for (uint i = 0; i < 5; ++i) 
	{
		float3 particlePos = particle.Position;
		float currentDis = planeArr[i].x * particlePos.x + planeArr[i].y * particlePos.y + planeArr[i].z * particlePos.z 
							+ planeArr[i].w - diametre / 2;

		if (currentDis > 0.1 * diametre) continue;
		
		float3 particleVel = particle.Velocity;
		particlePos += (particleVel * dt);
		float predictedDis = planeArr[i].x * (particlePos.x) + 
							 planeArr[i].y * (particlePos.y) +
							 planeArr[i].z * (particlePos.z) +
							 planeArr[i].w - diametre / 2;
		if (predictedDis < diametre * 0.05) 
		{
			//prevent particle from passing through the container boundary
			float3 normal = float3(planeArr[i].x, planeArr[i].y, planeArr[i].z);
			float dotProduct = dot(normal, particleVel);
			dotProduct /= length(normal);
			dotProduct *= -1;
			float3 velocityToBeApplied = normalize(normal)* dotProduct;
			particle.Velocity +=  velocityToBeApplied;
		}
	}

	//Collision response for intersection only
	/*float3 direction = particle.Position - currentParticle.Position;
	direction = normalize(direction);

	particle.Velocity += direction * separationSpeed * dt;*/

	ParticlePool[id.x] = particle;

	return;
}