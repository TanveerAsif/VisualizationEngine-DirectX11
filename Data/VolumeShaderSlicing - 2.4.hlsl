
/////////////
// GLOBALS //
/////////////



cbuffer ConstantBuffer : register(b0)
{
	matrix w;	
	matrix v;	
	matrix p;	
};

struct VSInput
{
	float3 position : POSITION;
	float3 tex	: TEXCOORD0;
	
};



struct VSOutput
{
	float4 Position : SV_POSITION;
	float3 Tex  : TEXCOORD0;
};



////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VSOutput VS( VSInput In )
{
	VSOutput Out = (VSOutput)0;
	Out.Position = float4(In.position.xyz, 1.0f);
	Out.Position = mul(Out.Position, w);
	Out.Position = mul(Out.Position, v);
	Out.Position = mul(Out.Position, p);
	Out.Tex = In.tex;
	
	return Out;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
Texture3D Volume3DTexture : register(t0);
SamplerState textureSampler : register(s0);



float4 PS(VSOutput input) : SV_TARGET
{	
	float color = Volume3DTexture.Sample(textureSampler, input.Tex); 
	return float4(color, color, color, 1.0f);
}


