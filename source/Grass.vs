/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 HeightList;
	float3 TerrainOffset;
	float3 cameraLocation;
	float Time;					// Nothing is 16-byte aligned. But should be!
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float4 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 normal : NORMAL;
};


PixelInputType main( VertexInputType input )
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	output.position = input.position;
	output.tex = float2(0,0);

    return output;
}
