#include "ParticleIncludes.hlsli"

cbuffer ExternalData : register(b0) 
{
	float4 planeArr[5];
	float3 gravity;
	float dt;
	float diametre;
	float separationSpeed;
	int activeCount;
}

RWStructuredBuffer<Particle> ParticlePool : register (u0);

[numthreads(32, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	//if (id.x >= (uint)activeCount) { return; }

	Particle particle = ParticlePool.Load(id.x);

	if (particle.Alive == 0.0f) return;

	//update the initialized particles
	particle.Position += particle.Velocity * dt;
	particle.Velocity += gravity*dt;

	float r, rx,ry,rz;
	Particle currentParticle;

	static int wallCollision = 0, intersection = 0, internalCollision = 0;
	static float3 blockedMoveDirections[5];

	//collision detection with walls
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
			wallCollision = 1;
			blockedMoveDirections[i] = -1 * float3(planeArr[i].x, planeArr[i].y, planeArr[i].z);
		}
		else 
		{
			blockedMoveDirections[i] = float3(0.0f, 0.0f, 0.0f);
		}
	}


	//collision detection with other particles
	for (int i = 0; i < activeCount && i!=id.x; ++i) 
	{
		
		//get current Particle
		currentParticle = ParticlePool.Load(i);

		//get distance between the two particles
		rx = currentParticle.Position.x - particle.Position.x;
		ry = currentParticle.Position.y - particle.Position.y;
		rz = currentParticle.Position.z - particle.Position.z;
		r = sqrt(rx*rx + ry*ry + rz*rz);

		//if they are intersecting already, push them away
		if (r < diametre) 
		{
			intersection = 1;
		}
		//if they are close to intersecting, conserve their momentum in the direction of the line joining their centres
		else if(r < diametre * 1.05) 
		{
			float3 postPos1 = particle.Position + particle.Velocity * dt;
			float3 postPos2 = currentParticle.Position + currentParticle.Velocity * dt;
			float postR, postRx, postRy, postRz;
			postRx = postPos1.x - postPos2.x;
			postRy = postPos1.y - postPos2.y;
			postRz = postPos1.z - postPos2.z;
			postR = sqrt(postRx * postRx + postRy * postRy + postRz * postRz);
			if (postR < diametre) 
			{
				float3 direction = normalize(particle.Position - currentParticle.Position);
				float speed1 = dot(particle.Velocity, direction);
				float speed2 = dot(currentParticle.Velocity, direction);
				float diff1 = (speed2 - speed1) / 2;
				float diff2 = (speed1 - speed2) / 2;
				particle.Velocity += diff1 * direction;
				currentParticle.Velocity += diff2 * direction;
			}
			ParticlePool[i] = currentParticle;
		}
	}

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
			particle.Velocity += velocityToBeApplied;
		}
	}

	//Collision response for intersection only
	/*float3 direction = particle.Position - currentParticle.Position;
	direction = normalize(direction);

	particle.Velocity += direction * separationSpeed * dt;*/

	ParticlePool[id.x] = particle;

	return;
}