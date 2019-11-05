
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
Texture2D shaderTexture;
SamplerState textureSampler;



float4 PS(PixelInputType input) : SV_TARGET
{	
	
	/*float4 color = shaderTexture.Sample(textureSampler, input.tex);

	float gray = (input.normal.r + input.normal.g + input.normal.b)/3.0;
	float4 FinalColor = float4(gray, gray, gray , 1.0f)  ;
	return FinalColor;*/
	
	float4 texColor = shaderTexture.Sample(textureSampler, input.tex);
	return texColor;

	
}
