#include "CGrassShader.h"

CGrassShader::CGrassShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_geometryShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_Time = 0;
	m_wireFrame = false;
	//m_gradientBuffer = 0;

	srand(37);
}


CGrassShader::CGrassShader(const CGrassShader& other)
{
}


CGrassShader::~CGrassShader()
{
}


bool CGrassShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex, pixel, and geometry shaders.
	result = InitializeShader(device, hwnd, "..\\..\\source\\Grass.vs", "..\\..\\source\\Grass.ps", "..\\..\\source\\Grass.gs");
	if(!result)
	{
		return false;
	}

	return true;
}


void CGrassShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool CGrassShader::Render(ID3D11DeviceContext* deviceContext, 
						  int indexCount, 
						  D3DXMATRIX worldMatrix, 
						  D3DXMATRIX viewMatrix, 
						  D3DXMATRIX projectionMatrix, 
						  D3DXVECTOR4 ambientColor, 
						  D3DXVECTOR4 diffuseColor, 
						  D3DXVECTOR3 cameraLocation, 
						  D3DXVECTOR3 cameraDirection, 
						  D3DXVECTOR4 heightList,
						  D3DXVECTOR3 terrainOffset,
						  unsigned long time )
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters( deviceContext, 
		worldMatrix, 
		viewMatrix, 
		projectionMatrix, 
		ambientColor, 
		diffuseColor, 
		cameraLocation,
		cameraDirection,
		heightList,
		terrainOffset,
		time );

	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

void CGrassShader::Frame()
{
	m_Time += (double)0.0066667;
}

bool CGrassShader::InitializeShader(ID3D11Device* device, HWND hwnd, char * vsFilename, char * psFilename, char *gsFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	ID3D10Blob* geometryShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC gradientBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
	D3D11_SAMPLER_DESC samplerDesc2;
	D3D11_BUFFER_DESC lightBufferDesc;

	result = D3DX11CreateShaderResourceViewFromFile(device, "..\\..\\data\\grassAlphaLOD1.png", NULL, NULL, &m_grassAlphaTexture, NULL);

	if(FAILED(result))
	{
		MessageBox(hwnd, "Could create resource from grassAlphaLOD1.png in CGrassShader.cpp line 109", "Error", MB_OK);
		return false;
	}

	result = D3DX11CreateShaderResourceViewFromFile(device, "..\\..\\data\\grassDiffuseLOD1.png", NULL, NULL, &m_grassTexture, NULL);

	if(FAILED(result))
	{
		MessageBox(hwnd, "Could create resource from grassDiffuseLOD1.png in CGrassShader.cpp line 117", "Error", MB_OK);
		return false;
	}

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;
	geometryShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}
	
	// Compile the geometry shader code.
	result = D3DX11CompileFromFile(gsFilename, NULL, NULL, "main" , "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &geometryShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, gsFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if(FAILED(result))
	{
		return false;
	}

	result = device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_geometryShader);
	if(FAILED(result))
	{
		return false;
	}

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 
									   &m_layout);
	if(FAILED(result))
	{
		return false;
	}


	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	
	geometryShaderBuffer->Release();
	geometryShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	/*
	// Setup the description of the gradient constant buffer that is in the pixel shader.
	gradientBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	gradientBufferDesc.ByteWidth = sizeof(GradientBufferType);
	gradientBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gradientBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gradientBufferDesc.MiscFlags = 0;
	gradientBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&gradientBufferDesc, NULL, &m_gradientBuffer);
	if(FAILED(result))
	{
		return false;
	}
	*/

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		return false;
	}

	RSWireFrameDesc.FillMode = D3D11_FILL_SOLID; 
	RSWireFrameDesc.CullMode = D3D11_CULL_NONE;
	RSWireFrameDesc.ScissorEnable = FALSE;
	RSWireFrameDesc.FrontCounterClockwise = FALSE;
	RSWireFrameDesc.DepthBias = 0;
	RSWireFrameDesc.DepthBiasClamp = 0.0f;
	RSWireFrameDesc.SlopeScaledDepthBias = 0.0f;
	RSWireFrameDesc.MultisampleEnable = FALSE;
	RSWireFrameDesc.AntialiasedLineEnable = FALSE;

	device->CreateRasterizerState ( &RSWireFrameDesc , &m_rasterizerState );  

	D3D11_BLEND_DESC BlendState;

	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));

	BlendState.AlphaToCoverageEnable = TRUE;
	BlendState.IndependentBlendEnable = FALSE;

	//BlendState.RenderTarget = new D3D11_RENDER_TARGET_BLEND_DESC[1];
 
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&BlendState, &m_blendState);

	return true;
}


void CGrassShader::ShutdownShader()
{
	// Release the matrix constant buffer.
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if(m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = 0;
	}

	return;
}


void CGrassShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char * shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool CGrassShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
									   D3DXMATRIX worldMatrix, 
									   D3DXMATRIX viewMatrix,
									   D3DXMATRIX projectionMatrix,
									   D3DXVECTOR4 ambientColor,
									   D3DXVECTOR4 diffuseColor,
									   D3DXVECTOR3 cameraLocation, 
									   D3DXVECTOR3 cameraDirection,
									   D3DXVECTOR4 heightList,
									   D3DXVECTOR3 terrainOffset,
									   unsigned long time )
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource1;
	D3D11_MAPPED_SUBRESOURCE mappedResource2;
	D3D11_MAPPED_SUBRESOURCE mappedResource3;
	MatrixBufferType* dataPtr;
	//GradientBufferType* dataPtr2;
	unsigned int bufferNumber;
	LightBufferType* dataPtr2;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource1);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource1.pData;
	srand((unsigned int) terrainOffset.x);
	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->HeightList = heightList;
	dataPtr->TerrainOffset = terrainOffset;
	dataPtr->CameraLocation = cameraLocation;
	dataPtr->rand = rand();//( RAND_MAX - rand() ) / RAND_MAX;
	dataPtr->time = m_Time;
	dataPtr->padding2 = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	// Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map( m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource3 );
	if( FAILED( result ) )
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = ( LightBufferType* )mappedResource3.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->cameraDirection = cameraDirection;
	dataPtr2->rand = dataPtr->rand;
	dataPtr2->cameraLocation = cameraLocation;
	dataPtr2->padding1 = 0;
	// Unlock the constant buffer.
	deviceContext->Unmap( m_lightBuffer, 0 );

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	//deviceContext->RSSetState( m_rasterizerState ); 
	
	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers( bufferNumber, 1, &m_lightBuffer );

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources( 0, 1, &m_grassAlphaTexture );

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 2, &m_grassTexture);

	float a[1] = {1.0f};

	deviceContext->RSSetState( m_rasterizerState ); 

	deviceContext->OMSetBlendState(m_blendState, a, 0xffffffff);

	return true;
}


void CGrassShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

    // Set the vertex, geometry, and pixel shaders that will be used to render the triangles.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->GSSetShader(m_geometryShader, NULL, 0);				// Set gs shader to lod functions based on camera distance from grass patch
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void CGrassShader::ToggleWireFrame( ID3D11Device* device )			//TODO: change to SetFillMode
{
	//TODO: timeSinceLastWireFrameToggle;

	m_wireFrame = !m_wireFrame;
	
	if( m_wireFrame )
	{
		RSWireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
		RSWireFrameDesc.CullMode = D3D11_CULL_BACK;
		RSWireFrameDesc.ScissorEnable = FALSE;
		RSWireFrameDesc.FrontCounterClockwise = FALSE;
		RSWireFrameDesc.DepthBias = 0;
		RSWireFrameDesc.DepthBiasClamp = 0.0f;
		RSWireFrameDesc.SlopeScaledDepthBias = 0.0f;
		RSWireFrameDesc.MultisampleEnable = FALSE;
		RSWireFrameDesc.AntialiasedLineEnable = FALSE;
	}
	else
	{
		RSWireFrameDesc.FillMode = D3D11_FILL_SOLID;
		RSWireFrameDesc.CullMode = D3D11_CULL_BACK;
		RSWireFrameDesc.ScissorEnable = FALSE;
		RSWireFrameDesc.FrontCounterClockwise = FALSE;
		RSWireFrameDesc.DepthBias = 0;
		RSWireFrameDesc.DepthBiasClamp = 0.0f;
		RSWireFrameDesc.SlopeScaledDepthBias = 0.0f;
		RSWireFrameDesc.MultisampleEnable = FALSE;
		RSWireFrameDesc.AntialiasedLineEnable = FALSE;
	}
	
	m_rasterizerState->Release();
	device->CreateRasterizerState( &RSWireFrameDesc, &m_rasterizerState );

}