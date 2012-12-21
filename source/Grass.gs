cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float4 HeightList;
	float3 cameraLocation;
	float padding;
	float3 TerrainOffset;
	float rand;
	float Time;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 normal : NORMAL;
	//float4 v4Diffuse : COLOR;
};

struct PixelOutputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 normal : NORMAL;
	float rand : RAND;
	//float4 v4Diffuse : COLOR;
};

// Grass blade data
struct GrassBladeData
{
	float3	grassDir;
	float3	rootPos;
	float3	rootPosWorldSpace;
	float2	windVec;
	float	randSeed; // [0,1]
	float3x3	planeData;
};

static const float	kGrassWidth = 1*.035;
static const int	NumSegs[3] = { 5, 3, 2 };

static const float kWindCoeff = 87.0f; // Multiplies by navier wind
static const float kHeightDelta = 0.127f;

// Final stage of GS. Prepare vert to be sent into PS
void PrepareVertForPS( inout PixelInputType vert )
{
	vert.normal = normalize( vert.normal );
	vert.position = mul( mul( mul( vert.position, worldMatrix ), viewMatrix ), projectionMatrix );

}

float GetRandSeed( float3 rootPos )
{
	return clamp( (sin( ( 1.57 * frac( rootPos.z * rand ) + 1.57 * frac( rootPos.x * rand ) ) ) * 2.0 - 1.0 ), 0, 1 );
}

float3x3 GetPlane( float3 rootPos )
{
	float3x3 planeData;
	
	if ( rootPos.x < rootPos.z )
	{
		planeData[0] = float3( 0, HeightList.x, 8 );
		planeData[1] = float3( 8, HeightList.y, 8 );
		planeData[2] = float3( 0, HeightList.z, 0 );
	}
	else  
	{
		planeData[0] = float3( 0, HeightList.z, 0 );
		planeData[1] = float3( 8, HeightList.y, 8 );
		planeData[2] = float3( 8, HeightList.w, 0 );
	}

	return planeData;
}

// Get height from pos
float GetHeightFromPlane( float3x3 planeData, float3 pos )
{
	
	float3 startVector = pos;
	float3 directionVector = {0.0f, -1.0f, 0.0f};
	float3 edge1, edge2;
	float3 normal;

	edge1 = planeData[1] - planeData[0];
	edge2 = planeData[2] - planeData[0];

	normal = cross(edge1, edge2);

	normal = normalize(normal);

	float D = ((-normal.x * planeData[0][0]) + (-normal.y * planeData[0][1]) + (-normal.z * planeData[0][2]));

	float denominator = ((normal.x * directionVector.x) + (normal.y * directionVector.y) + (normal.z * directionVector.z));

	float  numerator = -1.0f * (((normal.x * startVector.x) + (normal.y * startVector.y) + (normal.z * startVector.z)) + D);
	float t = numerator / denominator;

	float3 Q = startVector + (directionVector * t);

	return Q.y;
}

// Get the angle of plane data from
float GetPlaneAngleFromFloor( float3x3 planeData )
{
	const float3 worldUp = { 0, 1, 0 };
	return acos( abs( dot( worldUp, normalize( cross( planeData[0] - planeData[1], planeData[2] - planeData[1] ) ) ) ) );
}


// Get the height of the root pos given the plane data
float GetHeight( float3 pos )
{
	float3x3 planeData = GetPlane( pos );
	return GetHeightFromPlane( planeData, pos );
}

// Get wind vector based on current vector and surrounding grids
float2 GetWindForce( float3 rootPos )
{
	return float2(/*sin(Time*0.5f)*30.0f*/0.0f, cos(Time*0.2f)*5.0f);
}


// Fetches the vertex of grass (used in GeoShader)
PixelOutputType GetGrassVert( GrassBladeData blade, int LOD, int seg, int posHorizCoeff )
{
	PixelOutputType output = (PixelOutputType)0;

	float GrassHeight = 1;
	float GrassWidth = .5 * .06;

	float vertTexOffset[3] = {0, 0.25f, 0.5f};

	const int bladeNumSegs = NumSegs[LOD];
	
	float distFromFloorNormalized = float(seg) / (float)(bladeNumSegs-1);
	
	// How much variation of height there is per blade?
	const float heightSkew = kHeightDelta * cos( blade.randSeed );
	
	float3 PatchPosition = float3( TerrainOffset.x, HeightList.z, TerrainOffset.z );

	// Height - decrease based on distance from camera
	float heightDistScale = distance( blade.rootPosWorldSpace, cameraLocation );
	heightDistScale = clamp( ( heightDistScale - 100.0 ) / 100.0, 0, 1 );
	heightDistScale = 1.0 - heightDistScale;
	
	// Grass blades in the dist must be thicker
	float grassWidth = lerp( kGrassWidth * 2.0, kGrassWidth, heightDistScale );

	// Find position
	output.position.xyz	= blade.rootPosWorldSpace;
	output.position.y	+= ( GrassHeight + heightSkew ) * distFromFloorNormalized * heightDistScale; // Height
	output.position.xyz	+= float3(sin(blade.randSeed * tan(37*blade.randSeed)), 0, cos(tan(blade.randSeed*20)*blade.randSeed)) * posHorizCoeff * ( grassWidth + grassWidth * 0.2 * 
								sin( blade.rootPos.x + blade.rootPos.z ) ) * 0.5; // Blade-facing direction
	output.position.w	= 1;
	
	static const float	WindCoeff[3][5] =	
		{
			{ 0.0, 0.015/3, 0.035/3, 0.06/3, 0.09/3 },
			{ 0.0, 0.035/3, 0.09/3, 0.0, 0.0 },
			{ 0.0, 0.09/3, 0.0, 0.0, 0.0 }								
		};
		
	static const float kOscillateDelta = 1.235;

	// Wind
	float	windCoeff = WindCoeff[LOD][seg];
	float2	windVec = blade.windVec;	
	float	windForce;
	
	// New wind impl
	{
		// Skew wind-vector depending on how sloped the plane is
		float planeAngFromFloor = GetPlaneAngleFromFloor( blade.planeData );
		windVec *= pow( cos( planeAngFromFloor ), 5.0 );
		
		float sinSkewCoeff = 3.14 * sin( ( output.position.z + 8 * 0.5 ) / 8 ) + 0.3 * sin( blade.rootPos.x * 2.0 );
	
		// Wind vector force skewed randomly
		windVec *= lerp( 0.7, 1.0, 1.0 - blade.randSeed );
		
		// Oscillate wind
		float lerpCoeff = ( sin( 5.0f * Time + sinSkewCoeff ) + 1.0 ) / 2;
		float2 leftWindBound = windVec * ( 1.0 - kOscillateDelta );
		float2 rightWindBound = windVec * ( 1.0 + kOscillateDelta );
		windVec = lerp( leftWindBound, rightWindBound, lerpCoeff );
		
		// Add wind vector in random direction
		float randAngle = lerp( -3.14, 3.14, blade.randSeed );
		float randMangitude = lerp( 0, 5.0, blade.randSeed ); 
		float2 randWindDir = float2( sin(randAngle), cos(randAngle) );
		windVec += randWindDir * randMangitude;
		
		windForce = length(windVec);
	}
	
	output.position.y += - windCoeff * (windForce * 0.8);
	output.position.xz += windVec.xy * windCoeff;
	
	// Texture coord
	output.tex.x = (posHorizCoeff + 1.0) * 0.5;
	output.tex.y = 1.0 - distFromFloorNormalized;
	
	output.rand = blade.randSeed;

	return output;
}

// Calc and generate the normals
void GenerateNormalOfTri(	inout PixelOutputType v0, 
							inout PixelOutputType v1, 
							inout PixelOutputType v2 )
{
	float3 vec0 = normalize(v0.position.xyz - v1.position.xyz);
	float3 vec1 = normalize(v2.position.xyz - v1.position.xyz);
	
	float3 normal = normalize( cross( vec0, vec1 ) );

	v0.normal = float4(normal.x, normal.y, normal.z, 1.0f);
	v1.normal = float4(normal.x, normal.y, normal.z, 1.0f);
	v2.normal = float4(normal.x, normal.y, normal.z, 1.0f);
}

// Given a root position, will build grass geometry and add to TriStream
void BuildBlade_LOD0( float3 rootPos, inout TriangleStream<PixelOutputType> TriStream )
{
	int i = 0;
	
	// Get blade properties
	const int bladeNumSegs = NumSegs[0];
	const int numVertsPerBlade = bladeNumSegs * 4;
	
	float3x3 planeData = GetPlane( rootPos );
	
	// Blade Data
	GrassBladeData blade = {
		{ abs(sin(rootPos.x * rand)), 0, abs(cos(rootPos.y * rand)) }, // grass direction
		rootPos, // object space
		float3( TerrainOffset.x + rootPos.x, GetHeight( rootPos ), TerrainOffset.z + rootPos.z ), 
		GetWindForce( rootPos ),
		GetRandSeed( rootPos ),
		planeData
	};

	PixelOutputType vertices[ numVertsPerBlade ];
	
	// Append to the triangle strip
	for ( i = 0; i < bladeNumSegs; i++)
	{
			vertices[ i * 4 ] = GetGrassVert( blade, 0, i, 1 );
			vertices[ i * 4 + 1 ] = GetGrassVert( blade, 0, i, -1 );
			if( i*4+2 < 18)
			{
				vertices[ i * 4 + 2 ] = GetGrassVert( blade, 0, i+1, 1 );
				vertices[ i * 4 + 3 ] = GetGrassVert( blade, 0, i+1, -1 );
				GenerateNormalOfTri( vertices[i*4+0], vertices[i*4+1], vertices[i*4+2] );
				GenerateNormalOfTri( vertices[i*4+1], vertices[i*4+3], vertices[i*4+2] );
			}

			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[i*4].position = mul(vertices[i*4].position, worldMatrix);
			vertices[i*4].position = mul(vertices[i*4].position, viewMatrix);
			vertices[i*4].position = mul(vertices[i*4].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4] );

			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, worldMatrix);
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, viewMatrix);
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4 + 1] );

		if( i*4+2 < 18)
		{
			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, worldMatrix);
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, viewMatrix);
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4 + 2] );

			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[i*4+3].position = mul(vertices[i*4+3].position, worldMatrix);
			vertices[i*4+3].position = mul(vertices[i*4+3].position, viewMatrix);
			vertices[i*4+3].position = mul(vertices[i*4+3].position, projectionMatrix);

			TriStream.Append( vertices[i * 4 + 3] );
		}
	}
	return;
}

// Given a root position, will build grass geometry and add to TriStream
void BuildBlade_LOD1( float3 rootPos, inout TriangleStream<PixelOutputType> TriStream )
{
	int i = 0;
	
	// Get blade properties
	const int bladeNumSegs = NumSegs[1];
	const int numVertsPerBlade = bladeNumSegs * 4;
	
	float3x3 planeData = GetPlane( rootPos );
	
	// Blade Data
	GrassBladeData blade = {
		{ sin(rootPos.x), 0, cos(rootPos.y) }, // grass direction
		rootPos, // object space
		float3( TerrainOffset.x + rootPos.x, GetHeight( rootPos ), TerrainOffset.z + rootPos.z ),
		GetWindForce( rootPos ),
		GetRandSeed( rootPos ),
		planeData
	};
	

	PixelOutputType vertices[ numVertsPerBlade ];
	
	// Append to the triangle strip
	for ( i = 0; i < bladeNumSegs; i++)
	{
			vertices[ i * 4 ] = GetGrassVert( blade, 1, i, 1 );
			vertices[ i * 4 + 1 ] = GetGrassVert( blade, 1, i, -1 );
			if( i*4+2 < 10)
			{
				vertices[ i * 4 + 2 ] = GetGrassVert( blade, 1, i+1, 1 );
				vertices[ i * 4 + 3 ] = GetGrassVert( blade, 1, i+1, -1 );
				GenerateNormalOfTri( vertices[i*4+0], vertices[i*4+1], vertices[i*4+2] );
				GenerateNormalOfTri( vertices[i*4+1], vertices[i*4+3], vertices[i*4+2] );
			}

			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[i*4].position = mul(vertices[i*4].position, worldMatrix);
			vertices[i*4].position = mul(vertices[i*4].position, viewMatrix);
			vertices[i*4].position = mul(vertices[i*4].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4] );

			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, worldMatrix);
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, viewMatrix);
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4 + 1] );

		if( i*4+2 < 10)
		{
			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, worldMatrix);
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, viewMatrix);
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4 + 2] );

			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[i*4+3].position = mul(vertices[i*4+3].position, worldMatrix);
			vertices[i*4+3].position = mul(vertices[i*4+3].position, viewMatrix);
			vertices[i*4+3].position = mul(vertices[i*4+3].position, projectionMatrix);

			TriStream.Append( vertices[i * 4 + 3] );
		}
	}
	return;
}

// Given a root position, will build grass geometry and add to TriStream
void BuildBlade_LOD2( float3 rootPos, inout TriangleStream<PixelOutputType> TriStream )
{
	int i = 0;
	
	// Get blade properties
	const int bladeNumSegs = NumSegs[2];
	const int numVertsPerBlade = bladeNumSegs * 4;
	
	float3x3 planeData = GetPlane( rootPos );
	
	// Blade Data
	GrassBladeData blade = {
		{ sin(rootPos.x), 0, cos(rootPos.y) }, // grass direction
		rootPos, // object space
		float3( TerrainOffset.x + rootPos.x, GetHeight( rootPos ), TerrainOffset.z + rootPos.z ),
		GetWindForce( rootPos ),
		GetRandSeed( rootPos ),
		planeData
	};
	
	PixelOutputType vertices[ numVertsPerBlade ];
	
	// Append to the triangle strip
	for ( i = 0; i < bladeNumSegs; i++)
	{
			vertices[ i * 4 ] = GetGrassVert( blade, 2, i, 1 );
			vertices[ i * 4 + 1 ] = GetGrassVert( blade, 2, i, -1 );
			if( i*4+2 < 4)
			{
				vertices[ i * 4 + 2 ] = GetGrassVert( blade, 2, i+1, 1 );
				vertices[ i * 4 + 3 ] = GetGrassVert( blade, 2, i+1, -1 );
				GenerateNormalOfTri( vertices[i*4+0], vertices[i*4+1], vertices[i*4+2] );
				GenerateNormalOfTri( vertices[i*4+1], vertices[i*4+3], vertices[i*4+2] );
			}
			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[i*4].position = mul(vertices[i*4].position, worldMatrix);
			vertices[i*4].position = mul(vertices[i*4].position, viewMatrix);
			vertices[i*4].position = mul(vertices[i*4].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4] );

			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, worldMatrix);
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, viewMatrix);
			vertices[ i * 4 + 1].position = mul(vertices[ i * 4 + 1].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4 + 1] );

		if( i*4+2 < 4)
		{
			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, worldMatrix);
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, viewMatrix);
			vertices[ i * 4 + 2 ].position = mul(vertices[ i * 4 + 2 ].position, projectionMatrix);
		
			TriStream.Append( vertices[i * 4 + 2] );

			// Calculate the position of the vertex against the world, view, and projection matrices.
			vertices[i*4+3].position = mul(vertices[i*4+3].position, worldMatrix);
			vertices[i*4+3].position = mul(vertices[i*4+3].position, viewMatrix);
			vertices[i*4+3].position = mul(vertices[i*4+3].position, projectionMatrix);

			TriStream.Append( vertices[i * 4 + 3] );
		}

		
	}
	return;
}

[maxvertexcount(30)]
void main( point PixelInputType input[1], inout TriangleStream< PixelOutputType > output )
{
	float grassHeight = .5;
	float grassWidth = grassHeight * .075;//Perfect ratio multiplier 0.0648402;
	
	float2 g_texcoords[4] = 
    { 
        float2(0,1), 
        float2(1,1),
        float2(0,0),
        float2(1,0),
    };

	float4  vertices[4];

	float grassPlaneHeight = GetHeight( float3( input[0].position.x, input[0].position.y, input[0].position.z ) );

	float distanceLOD = distance( cameraLocation, input[0].position + TerrainOffset );
	
	if( distanceLOD <= 10.0f )
	{
		BuildBlade_LOD0( input[0].position, output );
	}
	else if( distanceLOD > 10.0f && distanceLOD <= 25.0f )
	{
		BuildBlade_LOD1( input[0].position, output );
	}
	else if( distanceLOD > 25.0f && distanceLOD <= 100.0f )
	{
		BuildBlade_LOD2( input[0].position, output );
	}
	
	output.RestartStrip();
}


	
	/*
	float3 e1 = planeData[0] - planeData[1];
	float3 e2 = planeData[2] - planeData[1];

	float3 edgeNormal = cross(e1, normal);

	float3 temp = Q - planeData[0];
	
	float determinant;
	determinant = ((edgeNormal.x * temp.x) + (edgeNormal.y * temp.y) + (edgeNormal.z * temp.z));


	edgeNormal = cross(e2, normal);

	determinant = ((edgeNormal.x * temp.x) + (edgeNormal.y * temp.y) + (edgeNormal.z * temp.z));
	
	edgeNormal = cross(e3, normal);

	temp = Q - planeData._m2;
	
	determinant = ((edgeNormal.x * temp.x) + (edgeNormal.y * temp.y) + (edgeNormal.z * temp.z));
	*/