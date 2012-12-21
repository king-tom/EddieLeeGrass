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
    float3 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
};


PixelInputType main( VertexInputType input )
{
	PixelInputType output;

	output.position = float4(input.position.x, input.position.y, input.position.z, 1.0f);

    return output;
}
