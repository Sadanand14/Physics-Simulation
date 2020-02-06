

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

}

struct VertexToPixelInput
{
	float4 position		: SV_POSITION;
};


float4 main(VertexToPixel input) : SV_TARGET
{
	float4 finalColor = float4(0.0f,0.6f,0.0f,0.3f);
	return finalColor;
}

