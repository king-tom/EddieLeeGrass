#include "CDepthOfField.h"

CDepthOfField::CDepthOfField( CDirect3DSystem *d3d )
{
	m_FullScreenVertexBuffer = NULL;
	m_ColorRTView = NULL;
	m_ColorSRView = NULL;
	m_MSAAColorRTView = NULL;
	m_MSAAColorSRView = NULL;
	m_DepthStencilTexture = NULL;
	m_DepthStencilDSView = NULL;
	m_DepthStencilSRView = NULL;

	m_d3d = d3d;
}

CDepthOfField::~CDepthOfField(){}

HRESULT CDepthOfField::SetupFullScreenQuadVertexBuffer( ID3D11Device* device )
{
    
    D3D11_BUFFER_DESC       BufDesc;
    D3D11_SUBRESOURCE_DATA  SRData;
    VertexType              Verticies[ 4 ];
    HRESULT                 hr;

    //  
    //  setup full-screen quad vertex buffer
    //
    Verticies[ 0 ].position.x        = -1;
    Verticies[ 0 ].position.y        = -1;
    Verticies[ 0 ].position.z        =  1;

    Verticies[ 0 ].texture.x        =  0;
    Verticies[ 0 ].texture.y        =  1;

    Verticies[ 1 ].position.x        = -1;
    Verticies[ 1 ].position.y        =  1;
    Verticies[ 1 ].position.z        =  1;

    Verticies[ 1 ].texture.x        =  0;
    Verticies[ 1 ].texture.y        =  0;

    Verticies[ 2 ].position.x        =  1;
    Verticies[ 2 ].position.y        = -1;
    Verticies[ 2 ].position.z        =  1;

    Verticies[ 2 ].texture.x        =  1;
    Verticies[ 2 ].texture.y        =  1;

    Verticies[ 3 ].position.x        =  1;
    Verticies[ 3 ].position.y        =  1;
    Verticies[ 3 ].position.z        =  1;

    Verticies[ 3 ].texture.x        =  1;
    Verticies[ 3 ].texture.y        =  0;


    BufDesc.ByteWidth           = sizeof( VertexType ) * 4;
    BufDesc.Usage               = D3D11_USAGE_DEFAULT;
    BufDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    BufDesc.CPUAccessFlags      = 0;
    BufDesc.MiscFlags           = 0;

    SRData.pSysMem              = Verticies;
    SRData.SysMemPitch          = 0;
    SRData.SysMemSlicePitch     = 0;

    hr = device->CreateBuffer(
        &BufDesc,
        &SRData,
        &m_FullScreenVertexBuffer );

	if( FAILED( hr ) )
		return hr;

   return hr;
}

bool CDepthOfField::Initialize( ID3D11Device* device )
{
	bool result;
	HRESULT hr;

	//ID3D10Device1*                      pd3dDevice1;
	D3D11_TEXTURE2D_DESC                TexDesc;
    D3D11_RENDER_TARGET_VIEW_DESC       RTDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC     SRDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC       DSDesc;
    FLOAT                               fAspectRatio;

	hr = InitializeShaders( device );

	if( FAILED( hr ) )
	{
		return false;
	}

	result = InitializeBuffers( device );			/// LAST THING?
	if( !result )
	{
		return false;
	}


	//
    //  Create full-screen render target and its views
    //

    TexDesc.Width                   = m_d3d->GetWidth();
    TexDesc.Height                  = m_d3d->GetHeight();
    TexDesc.MipLevels               = 0;
    TexDesc.ArraySize               = 1;
    TexDesc.Format                  = m_d3d->GetBackBufferFormat();//pBackBufferSurfaceDesc->Format;
    TexDesc.SampleDesc.Count        = 1;
    TexDesc.SampleDesc.Quality      = 0;
    TexDesc.Usage                   = D3D11_USAGE_DEFAULT;
    TexDesc.BindFlags               = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TexDesc.CPUAccessFlags          = 0;
    TexDesc.MiscFlags               = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	hr = device->CreateTexture2D(
    &TexDesc,
    NULL,
    &m_ColorTexture );

	DXGI_SAMPLE_DESC g_MSAASettings = { 1, 0 };

	//
    //  using msaa, create the msaa render target
    //
    TexDesc.MipLevels           = 1;
    TexDesc.SampleDesc          = g_MSAASettings;    
    TexDesc.MiscFlags           = 0;

    hr = device->CreateTexture2D(
        &TexDesc,
        NULL,
        &m_MSAAColorTexture );

	if( FAILED(hr) )
	{
		return false;
	}

	RTDesc.Format = m_d3d->GetBackBufferFormat();
    RTDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    RTDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(
    m_ColorTexture,
    &RTDesc,
    &m_ColorRTView );
	
	if( FAILED(hr) )
	{
		return false;
	}

	SRDesc.Format = m_d3d->GetBackBufferFormat();
    SRDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRDesc.Texture2D.MostDetailedMip = 0;
    SRDesc.Texture2D.MipLevels = 6;

    hr = device->CreateShaderResourceView(
        m_ColorTexture,
        &SRDesc,
        &m_ColorSRView );


	RTDesc.Format = m_d3d->GetBackBufferFormat();
    RTDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    RTDesc.Texture2D.MipSlice = 0;

	
    hr = device->CreateRenderTargetView(
            m_MSAAColorTexture,
            &RTDesc,
            &m_MSAAColorRTView );

    if ( FAILED( hr ) )
	{
		return false;
	}

        SRDesc.Format = m_d3d->GetBackBufferFormat();
        SRDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DMS;
        SRDesc.Texture2D.MostDetailedMip = 0;
        SRDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(
            m_MSAAColorTexture,
            &SRDesc,
            &m_MSAAColorSRView );

	if ( FAILED( hr ) )
	{
		return false;
	}

	/////////////////////////////////////////////////////////////////
	//
    //  Create depth/stencil view or depth color buffer.
    //    
    TexDesc.Width               = m_d3d->GetWidth();
    TexDesc.Height              = m_d3d->GetHeight();
    TexDesc.MipLevels           = 1;
    TexDesc.ArraySize           = 1;
    TexDesc.Format              = DXGI_FORMAT_R16_TYPELESS;
    TexDesc.SampleDesc          = g_MSAASettings;
    TexDesc.Usage               = D3D11_USAGE_DEFAULT;
    TexDesc.CPUAccessFlags      = 0;
    TexDesc.MiscFlags           = 0;

    TexDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(
        &TexDesc,
        NULL,
        &m_DepthStencilTexture );

    if ( FAILED( hr ) )
	{
		return false;
	}

    DSDesc.Format = DXGI_FORMAT_D16_UNORM;

    DSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	
    DSDesc.Texture2D.MipSlice = 0;

    hr = device->CreateDepthStencilView(
        m_DepthStencilTexture,
        &DSDesc,
        &m_DepthStencilDSView );

    if ( FAILED( hr ) )
	{
		return false;
	}
 
           //
            //  the depth/stencil buffer is msaa so we need dx10.1 to allow
            //  us to create a shader resource view.
            //
            D3D11_SHADER_RESOURCE_VIEW_DESC    SRDesc1;
            ID3D11ShaderResourceView*          pSRView1 = NULL;              
                
            SRDesc1.Format = DXGI_FORMAT_R16_UNORM;
            SRDesc1.ViewDimension = D3D10_1_SRV_DIMENSION_TEXTURE2DMS;

            hr = device->CreateShaderResourceView(
                m_DepthStencilTexture,
                &SRDesc1,
                &pSRView1 );

			if( FAILED( hr ) )
				return false;
			            
			//
            //  Get the original shader resource view interface from the dx10.1 interface
            //
            hr = pSRView1->QueryInterface(
                __uuidof( ID3D10ShaderResourceView ),
                (LPVOID*)&m_DepthStencilSRView );

			if( FAILED( hr ) )
				return false;

            pSRView1->Release();

    if ( FAILED( hr ) )
	{
		return false;
	}

	return true;
}

bool CDepthOfField::InitializeBuffers( ID3D11Device* device )
{
	HRESULT hr;

	hr = SetupFullScreenQuadVertexBuffer( device );

	if( FAILED( hr ) )
	{
		return false;
	}


	return true;
}

bool CDepthOfField::InitializeShaders( ID3D11Device* device )
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC gradientBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile("..\\..\\source\\DepthOfField.vs", NULL, NULL, "vsDepthOfField", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		return false;
	}
	    
	// Compile the pixel shader code.
	result = D3DX11CompileFromFile("..\\..\\source\\DepthOfField.ps", NULL, NULL, "SkyDomePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		return false;
	}

	    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
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
		
	const D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	
	HRESULT hr = device->CreateInputLayout( 
        layout, 
        4,
		vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), 
        &m_VertexLayout );

	if( FAILED( hr ) )
		return false;

	return true;
}
//--------------------------------------------------------------------------------------
//  Render the scene using the D3D10 device
//--------------------------------------------------------------------------------------
void CDepthOfField::Render( 
    ID3D11Device*   device, 
    DOUBLE          fTime, 
    FLOAT           fElapsedTime, 
    void*           pUserContext,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix)
{
    //
    // get the back-buffer target views
    ID3D11RenderTargetView*     pRTView             = m_d3d->GetRenderTargetView();
    ID3D11DepthStencilView*     pDSView             = m_d3d->GetDepthStencilView;
    
    const FLOAT                 fClearColor[ 4 ]    = { 0.9569f, 0.9569f, 1.0f, 0.0f };
    const FLOAT                 fDepthInfo[ 4 ]     = { 0., (FLOAT)m_d3d->GetWidth(), (FLOAT)m_d3d->GetHeight(), g_fFocalPlaneVSDepth };

    const UINT                  uOffset             = 0;
    const UINT                  uStride             = sizeof( VertexType );

    D3DXMATRIX                  mWorld;
    D3DXMATRIX                  mView;
    D3DXMATRIX                  mProj;
    D3DXMATRIX                  mInvProj;
    D3DXMATRIX                  mWorldViewProj;
    
	/*
	//	DON'T NEED THIS STUFF--ALREADY RENDERED IN QUADTREE RENDER CALL
    //  If the settings dialog is being shown, then render it instead of rendering the app's scene
    //
    if( g_D3DSettingsDlg.IsActive() ) {

        //
        //  Clear and Set the backbuffer rendertarget.
        //
        pd3dDevice->ClearRenderTargetView( 
            pRTView, 
            fClearColor );

        pd3dDevice->OMSetRenderTargets(
            1,
            &pRTView,
            NULL );

        g_D3DSettingsDlg.OnRender( fElapsedTime );
        
        return;
    }
 */
    //
    //  Clear and set the render targets based on the user selections
    //
    ID3D11ShaderResourceView *pNullView = NULL; 
    m_d3d->GetDeviceContext()->PSSetShaderResources( 1, 1, &pNullView  );

    m_d3d->GetDeviceContext()->ClearRenderTargetView( 
            m_MSAAColorRTView, 
            fClearColor );

    m_d3d->GetDeviceContext()->ClearDepthStencilView( 
            m_DepthStencilDSView, 
            D3D11_CLEAR_DEPTH, 
            1.0, 
            0 );

    m_d3d->GetDeviceContext()->OMSetRenderTargets(
            1,
            & m_MSAAColorRTView,
            m_DepthStencilDSView );



    mWorld = worldMatrix;
    mProj = projectionMatrix;
    mView = viewMatrix;
    mWorldViewProj = mWorld*mView*mProj;

    D3DXMatrixInverse(
        &mInvProj,
        NULL,
        &mProj );

    //m_d3d->GetDeviceContext()->IASetInputLayout( m_VertexLayout );
    
	//g_pmWorldViewProj->SetMatrix( (float*)&mWorldViewProj );  Already rendered Stuff
    //g_mInvProj->SetMatrix( (float*)&mInvProj );

    //
    //  Render the city
    //
    /*g_CityMesh.Render( 
        pd3dDevice, 
        g_pRenderTextured, 
        g_pDiffuseTex );

    g_ColumnMesh.Render( 
        pd3dDevice, 
        g_pRenderTextured, 
        g_pDiffuseTex );*/

    //
    //  Render the microscopes themselves if the bounding volumes rendered
    //
   /* for( UINT uIdx = 0; uIdx < NUM_MICROSCOPE_INSTANCES; ++uIdx ) 
    {
        D3DXMATRIX mMatRot;
        D3DXMATRIX mWVP;
        
        D3DXMatrixRotationY( 
            &mMatRot, 
            uIdx * ( D3DX_PI / 3.0f ) );
        
        mWVP = mMatRot*mWorldViewProj;
        g_pmWorldViewProj->SetMatrix( (float*)&mWVP );

        g_HeavyMesh.Render( 
            pd3dDevice, 
            g_pRenderTextured, 
            g_pDiffuseTex );
    }
	*/
   // if( 1 != g_MSAASettings.Count ) 
   // {
    
        //
        //  Resolve the color buffer to use as the color source for the 
        //  final, blurred result
        //
        m_d3d->GetDeviceContext()->ResolveSubresource(
            m_ColorTexture,
            0,
            m_MSAAColorTexture,
            0,
            m_d3d->GetBackBufferFormat() );
    //} 

    //if( enRenderUsingNonMSAADepth == g_RenderType )					NON-MSAA DON'T NEED
    //{
        //
        //  Re-render the scene writing the depth value to the depth texture.
        //  
    /*    const FLOAT fClearColor[ 4 ] = { 1.f, 1.f, 1.f, 1.f };

        m_d3d->GetDeviceContext()->ClearRenderTargetView(
            m_DepthColorRTV,
            fClearColor );

        pd3dDevice->ClearDepthStencilView( 
            pDSView, 
            D3D10_CLEAR_DEPTH, 
            1.0, 
            0 );

        pd3dDevice->OMSetRenderTargets(
            1,
            &g_pDepthColorRTV,
            pDSView );

        g_pmWorldViewProj->SetMatrix( (float*)&mWorldViewProj );

        //
        //  Render the city
        //
        g_CityMesh.Render( 
            pd3dDevice, 
            g_pRenderDepth, 
            g_pDiffuseTex );

        g_ColumnMesh.Render( 
            pd3dDevice, 
            g_pRenderDepth, 
            g_pDiffuseTex );

        //
        //  Render the microscopes themselves if the bounding volumes rendered
        //
        for( UINT uIdx = 0; uIdx < NUM_MICROSCOPE_INSTANCES; ++uIdx ) 
        {
            D3DXMATRIX mMatRot;
            D3DXMATRIX mWVP;
            
            D3DXMatrixRotationY( 
                &mMatRot, 
                uIdx * ( D3DX_PI / 3.0f ) );
            
            mWVP = mMatRot*mWorldViewProj;
            g_pmWorldViewProj->SetMatrix( (float*)&mWVP );

            g_HeavyMesh.Render( 
                pd3dDevice, 
                g_pRenderDepth, 
                g_pDiffuseTex );
        }
    }
	*/
    //
    //  Set swap chain render targets to draw to the screen
    //
    m_d3d->GetDeviceContext()->OMSetRenderTargets(
        1,
        &pRTView,
        pDSView );

    //
    //  Create the mips so we can sample the levels based on distance from
    //  the viewer to approximate a blur effect.
    //
    m_d3d->GetDeviceContext()->GenerateMips(
        m_ColorSRView );

   /* m_DiffuseTex->SetResource(					SHADER STUFF--NEED
        g_pColorSRView );

     
    g_pvDepthInfo->SetFloatVector( (FLOAT*)fDepthInfo );
	*/
    m_d3d->GetDeviceContext()->IASetVertexBuffers(
        0,
        1,
        &m_FullScreenVertexBuffer,
        &uStride,
        &uOffset );

    //if( enRenderUsingNonMSAADepth == g_RenderType )				DONT NEED
    //{
    //    g_pDepthTex->SetResource( g_pDepthColorSRV );
    //} 
    //else
    //{
        //
        //  Set the depth surface into the appropriate 
        //
    //    if( 1 == g_MSAASettings.Count )
    //        g_pDepthTex->SetResource( g_pDepthStencilSRView );
    //    else
            g_pDepthMSAATex->SetResource( g_pDepthStencilSRView );			// SHADER STUFF--NEED
    //}
   
    //
    //  Apply the post-processing effect
    //
    //if( 1 == g_MSAASettings.Count )
    //    g_pRenderVerticiesQuad->GetPassByIndex( 0 )->Apply( 0 );
    //else
        g_pRenderVerticiesQuadMSAA->GetPassByIndex( 0 )->Apply( 0 );
    
    m_d3d->GetDeviceContext()->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

    m_d3d->GetDeviceContext()->Draw(
        4,
        0 );
}
