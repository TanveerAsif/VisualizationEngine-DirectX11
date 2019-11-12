

struct VertexInput
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};


struct HullInput
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       VS : Vertex Shader                //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
HullInput MyVertexShader(VertexInput input)
{
    HullInput output;
    
	output.Pos = input.Pos;	
	output.Color = input.Color;
    
    return output;
};

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       HS : TWO STAGE                   //////////////////////////
//////////////////////////       1- PatchConstantFunction         //////////////////////////
//////////////////////////       2- Hull Shader                   //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

cbuffer TessellationFactor
{
	float fTessAmount;
	float3 padding;
};


struct PatchOutput
{
	float edge[3] : SV_TessFactor;
	float inside  : SV_InsideTessFactor;
};


PatchOutput PatchConstantFunction(InputPatch<HullInput, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
	PatchOutput out;
	
	out.edge[0] = fTessAmount;
	out.edge[1] = fTessAmount;
	out.edge[2] = fTessAmount;
	
	out.inside = fTessAmount;
	
	return out;
};


struct DomainInput
{
	float3 Pos : SV_POSITION;
	float4 Color : COLOR;
};

[domain("tri")]
[partitioning("integer")]
[OutputTopology("triangle_cw")]
[OutputControlPoints(3)]
[patchConstantFunction("PatchConstantFunction")]
DomainInput HullShader(InputPatch<HullInput, 3> inputPatch, uint pointId : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	DomainInput out;
	
	out.Pos = inputPatch[pointId].pos;
	out.Color = inputPatch[pointId].Color;
	
	return out;
};


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       DS : Domain Shader               //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

cbuffer MatrixBuffer
{
	matrix worldMat;
	matrix viewMat;
	matrix projMat;
};


[domain("tri")]
PixelInput DomainShader(PatchOutput input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<HullInput, 3> patch)
{
	PixelInput out;
	
	//Determine new position of vertex
	float3 vertexNewPos =  uvwCoord.x * patch[0].pos + uvwCoord.y * patch[1].pos + uvwCoord.z * patch[2].pos;
	
	
	out.position = float4(vertexNewPos, 1.0f);
	out.position = mul(out.position, worldMat);
	out.position = mul(out.position, viewMat);
	out.position = mul(out.position, projMat);
	
	
	out.color = patch[0].Color;	
	return out;
};




////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////       PS : Pixel Shader                //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
float4 PixelShader(PixelInput in) : SV_TARGET
{
	return in.color;
};