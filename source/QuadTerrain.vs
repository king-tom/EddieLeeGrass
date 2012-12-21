cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
};


PixelInputType main( VertexInputType input )
{
	PixelInputType output;
  
	output.position = input.position;
	output.position.w = 1.0f;

    return output;
}