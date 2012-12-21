/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
};


[maxvertexcount(4)]
void main( point PixelInputType input[1], inout TriangleStream< PixelInputType > output )
{
	float grassHeight = 1;
	float grassWidth = grassHeight * .5;//Perfect ratio multiplier 0.0648402;


	float4  vertices[4];
    vertices[0] = float4(input[0].position.x - grassWidth, input[0].position.y, input[0].position.z + grassWidth, input[0].position.w);
    vertices[1] = float4(input[0].position.x + grassWidth, input[0].position.y, input[0].position.z + grassWidth, input[0].position.w);
    vertices[2] = float4(input[0].position.x - grassWidth, input[0].position.y + grassHeight, input[0].position.z - grassWidth, input[0].position.w);
	vertices[3] = float4(input[0].position.x + grassWidth, input[0].position.y + grassHeight, input[0].position.z - grassWidth, input[0].position.w);

    PixelInputType ov = (PixelInputType)0;

    for (int i = 0; i <= 3; i++)
    {
		// Calculate the position of the vertex against the world, view, and projection matrices.
		ov.position = mul(vertices[i], worldMatrix);
		ov.position = mul(ov.position, viewMatrix);
		ov.position = mul(ov.position, projectionMatrix);

        output.Append(ov);
    }	
	output.RestartStrip();
}