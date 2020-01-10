
/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register( b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;	
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float3 position : POSITION;
	float2 tex  : TEXCOORD0;
	float3 normal : NORMAL;
};



struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex  : TEXCOORD0;
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VS(VertexInputType input)
{
	PixelInputType output;

	output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
	output.normal = input.normal;
	output.tex = input.tex;
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
cbuffer QuadNodeColor : register( b1)
{
	float4 quadColor;
};

float4 PS(PixelInputType input) : SV_TARGET
{	
	//float4 color = float4(0.0, 1.0, 0.0, 1.0); //Green	
	
	/*
	float  val = input.position.xyz;
	float4 color = float4(input.position.x / val, input.position.y / val, input.position.z / val , 1.0); 
	*/
	return quadColor;	
}