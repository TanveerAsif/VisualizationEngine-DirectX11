

struct VertexInput
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};


struct HullInput
{
	float3 Position : POSITION;
	float4 Color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       VS : Vertex Shader                //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
HullInput MyVertexShader(VertexInput input)
{
    HullInput output;
    
	output.Position = input.Pos;	
	output.Color = input.Color;
    
    return output;
};

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       HS : TWO STAGE                   //////////////////////////
//////////////////////////       1- PatchConstantFunction         //////////////////////////
//////////////////////////       2- Hull Shader                   //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

cbuffer TessellationFactor : register(b0)
{
	float fTessAmount;
	//float3 padding;
	float fCamDistance;
	float2 padding;
};


struct PatchOutput
{
	float edge[3] : SV_TessFactor;
	float inside  : SV_InsideTessFactor;
};


PatchOutput PatchConstantFunction(InputPatch<HullInput, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
	PatchOutput output;
	
	float fMinDistance = 0; 
	float fMaxDistance = 10; 
	float s = saturate((fMaxDistance - fCamDistance) / (fMaxDistance - fMinDistance));
	float fTessValue = pow(2, lerp(fMinDistance, fMaxDistance, s));
	output.edge[0] = fTessValue;//fTessAmount;
	output.edge[1] = fTessValue;//fTessAmount;
	output.edge[2] = fTessValue;//fTessAmount;
	
	output.inside = fTessValue;//fTessAmount;
	
	return output;
};


struct DomainInput
{
	float3 Pos : SV_POSITION;
	float4 Color : COLOR;
};

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
DomainInput MyHullShader(InputPatch<HullInput, 3> inputPatch, uint pointId : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	DomainInput output;
	
	output.Pos = inputPatch[pointId].Position;
	output.Color = inputPatch[pointId].Color;
	
	return output;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       DS : Domain Shader               //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

cbuffer MatrixBuffer: register(b1)
{
	matrix worldMat;
	matrix viewMat;
	matrix projMat;
};


cbuffer TickBuffer: register(b2)
{
	float fTick;
	float3 fPadding;
};

[domain("tri")]
PixelInput MyDomainShader(PatchOutput input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<HullInput, 3> patch)
{
	PixelInput output;
	
	//Determine new position of vertex
	float3 vertexNewPos =  uvwCoord.x * patch[0].Position + uvwCoord.y * patch[1].Position + uvwCoord.z * patch[2].Position;
	
	//position.x += a * sin(k * position.y + f * time);	
	//y = a * sin(k - w * time);
	
	/*float a = 10.0f, k = 2.0f, w = 10.0;
	float yChange = a * sin(k - w * fTick);
	if(yChange > 0)
		vertexNewPos.z += yChange  ;*/
	
	output.position = float4(vertexNewPos, 1.0f);
	output.position = mul(output.position, worldMat);
	output.position = mul(output.position, viewMat);
	output.position = mul(output.position, projMat);
	
	
	output.color = patch[0].Color;	
	return output;
};




////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       PS : Pixel Shader                //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
float4 MyPixelShader(PixelInput input) : SV_TARGET
{
	return input.color;
};