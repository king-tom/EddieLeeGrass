#include "CQuadTreeShader.h"


CQuadTreeShader::CQuadTreeShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_wireFrame = false;
}


CQuadTreeShader::CQuadTreeShader( const CQuadTreeShader& other )
{
}


CQuadTreeShader::~CQuadTreeShader()
{
}


bool CQuadTreeShader::Initialize( ID3D11Device* device, HWND hwnd )
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader( device, hwnd, "../../source/QuadTerrain.vs", "../../source/QuadTerrain.gs", "../../source/QuadTerrain.ps" );
	if( !result )
	{
		return false;
	}

	return true;
}


void CQuadTreeShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool CQuadTreeShader::Render( ID3D11DeviceContext* deviceContext, int indexCount, 
									   D3DXMATRIX worldMatrix, 
									   D3DXMATRIX viewMatrix,
									   D3DXMATRIX projectionMatrix )
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters( deviceContext, worldMatrix, viewMatrix, projectionMatrix );
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader( deviceContext, indexCount );

	return true;
}


bool CQuadTreeShader::InitializeShader( ID3D11Device* device, HWND hwnd, char * vsFilename, char* gsFilename, char * psFilename )
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	ID3D10Blob* geometryShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;



	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile( vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &vertexShaderBuffer, &errorMessage, NULL );
	if( FAILED( result ) )
	{
		// If the shader failed to compile it should have writen something to the error message.
		if( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, vsFilename );
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox( hwnd, vsFilename, "Missing Shader File", MB_OK );
		}

		return false;
	}

	result = D3DX11CompileFromFile( gsFilename, NULL, NULL, "main", "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &geometryShaderBuffer, &errorMessage, NULL );
	if( FAILED( result ) )
	{
		// If the shader failed to compile it should have writen something to the error message.
		if( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, vsFilename );
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox( hwnd, vsFilename, "Missing Shader File", MB_OK );
		}

		return false;
	}

    // Compile the pixel shader code.
	result = D3DX11CompileFromFile( psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &pixelShaderBuffer, &errorMessage, NULL );
	if( FAILED( result ) )
	{
		// If the shader failed to compile it should have writen something to the error message.
		if( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, psFilename );
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox( hwnd, psFilename, "Missing Shader File", MB_OK );
		}

		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader( vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader );
	if(FAILED(result))
	{
		return false;
	}
	   
	// Create the vertex shader from the buffer.
    result = device->CreateGeometryShader( geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_geometryShader );
	if(FAILED(result))
	{
		return false;
	}

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader( pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader );
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
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);

	if( FAILED( result ) )
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


	return true;
}


void CQuadTreeShader::ShutdownShader()
{


	// Release the layout.
	if( m_layout )
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if( m_pixelShader )
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if( m_vertexShader )
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


void CQuadTreeShader::OutputShaderErrorMessage( ID3D10Blob* errorMessage, 
											  HWND hwnd, 
											  char * shaderFilename )
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = ( char* )( errorMessage->GetBufferPointer() );

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open( "shader-error.txt" );

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
	MessageBox( hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK );

	return;
}


bool CQuadTreeShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
									   D3DXMATRIX worldMatrix, 
									   D3DXMATRIX viewMatrix,
									   D3DXMATRIX projectionMatrix )
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource1;
	MatrixBufferType* dataPtr;

	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource1);
	
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource1.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
    deviceContext->GSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void CQuadTreeShader::RenderShader( ID3D11DeviceContext* deviceContext, int indexCount )
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout( m_layout );

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader( m_vertexShader, NULL, 0 );
	deviceContext->GSSetShader( m_geometryShader, NULL, 0 );
    deviceContext->PSSetShader( m_pixelShader, NULL, 0 );

	// Render the triangles.
	//deviceContext->DrawIndexed( indexCount, 0, 0 );

	return;
}

