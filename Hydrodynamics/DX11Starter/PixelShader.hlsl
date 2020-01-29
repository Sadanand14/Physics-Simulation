

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	//float4 color		: COLOR;
	float3 Normal		: NORMAL;
	float2 UV           : TEXCOORD;
};


struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

cbuffer externalData: register(b0)
{
	DirectionalLight Light1;
	DirectionalLight Light2;
}

Texture2D Texture		  :	register (t0);
SamplerState BasicSampler : register (s0);
//helper function for calculating directional light from each light on the surface
//float4 CalculateLight(DirectionalLight dL, float3 normal)
//{
//	normal = normalize(normal);
//	float3 nlight = normalize(dL.Direction*(-1));
//	float dotproduct = saturate(dot(normal, nlight));
//	float4 finalColor = dotproduct * dL.DiffuseColor + dL.AmbientColor;
//	return finalColor;
//}

float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

	float4 finalColor = Texture.Sample(BasicSampler, input.UV);
	return finalColor;
	
}

