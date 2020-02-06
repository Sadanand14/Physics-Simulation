

struct VertexToPixel
{
	float4 position		: SV_POSITION;
};



cbuffer externalData: register(b0)
{

}


float4 main(VertexToPixel input) : SV_TARGET
{
	float4 finalColor = float4(0.0f,0.6f,0.0f,1.0f);
	return finalColor;
}

