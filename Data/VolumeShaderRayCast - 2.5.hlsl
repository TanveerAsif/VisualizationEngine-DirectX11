
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
	float3 tex		: TEXCOORD0;	
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float3 Tex  : TEXCOORD0;
};



////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VSOutput FaceVS( VSInput In )
{
	VSOutput Out = (VSOutput)0;
	
	Out.Position = mul(float4(In.position, 1.0f), w);
	Out.Position = mul(Out.Position, v);
	Out.Position = mul(Out.Position, p);
	
	Out.Tex = In.tex;
	
	return Out;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////

SamplerState textureSampler : register(s0);
Texture2D txFrontFace : register(t0);
Texture2D txBackFace  : register(t1);
Texture3D txVolume: register(t2);


float4 FacePS(VSOutput input) : SV_TARGET
{	
	float4 color = float4(input.Tex, 1.0f);	
	return color;
}



VSOutput RayVS(VSInput In)
{
	VSOutput Out = (VSOutput)0;

	Out.Position = mul(float4(In.position, 1.0f), w);
	Out.Position = mul(Out.Position, v);
	Out.Position = mul(Out.Position, p);

	Out.Tex = In.tex;

	return Out;
}




float4 RayPS(VSOutput input) : SV_TARGET
{
	float stepSize = 0.05f;
	float2 screenPos;
	screenPos.x = input.Position.x / 1920;// 640;
	screenPos.y = input.Position.y / 1080;// 480;
	float3 start = txFrontFace.Sample(textureSampler, screenPos);
	float3 end = txBackFace.Sample(textureSampler, screenPos);
	float3 dir = end - start;
	float len = length(dir);
	clip(len);
	dir = normalize(dir);

	float m = 0.0f;
	for (float step = 0.0f; step < len; step += stepSize) 
	{
		float3 samplePos = start + dir * step;
		float value = txVolume.SampleLevel(textureSampler, samplePos, 0).r;
		m = max(m, value);
	}
	return float4(m, m, m, 1);	
}