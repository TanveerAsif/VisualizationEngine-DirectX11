


cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};


struct InputType
{
	float4 position : POSITION;
	float2 tex  : TEXCOORD0;
};


struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex  : TEXCOORD0;
};



OutputType VS(InputType input)
{
		OutputType output;
		output.position.w = 1;
		
		output.position = mul(input.position, world);
		output.position = mul(output.position, view);
		output.position = mul(output.position, projection);
		
		output.tex = input.tex;
		
		return output;
}


Texture2D shaderTexture;
SamplerState samplerState;
cbuffer PixelBuffer
{
	float4 PixelColor;
};

float4 PS(OutputType input) : SV_TARGET
{
	float4 color = shaderTexture.Sample(samplerState, input.tex);

	if(color.r == 0)
	{
		color.a = 0;
	}
	else
	{
		color.rgb = PixelColor.rgb;
		color.a = 1;		
	}
	return color;
}

