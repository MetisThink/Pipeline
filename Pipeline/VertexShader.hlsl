
// Vertex Shader
struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Colour : COLOUR;
};

// Pixel Shader
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Colour : COLOUR;
};

// Passthrough Shader
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	output.Pos = input.Pos;
	output.Colour = input.Colour.rgb;
	return output;
}