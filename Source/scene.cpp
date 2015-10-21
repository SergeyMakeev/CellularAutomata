#include "scene.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

Scene::Scene()
	: d3d9(NULL)
	, device(NULL)
	, vertexDecl(NULL)
	, quadVB(NULL)
	, initialState(NULL)
	, state0(NULL)
	, state1(NULL)
	, rtMain(NULL)
	, rtState0(NULL)
	, rtState1(NULL)
	, parity(0)
	, hWnd(NULL)
{
	wchar_t curDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, curDir);

	shaderDirWatcherHandle = FindFirstChangeNotification(curDir, TRUE, 
		FILE_NOTIFY_CHANGE_FILE_NAME |
		FILE_NOTIFY_CHANGE_DIR_NAME |
		FILE_NOTIFY_CHANGE_ATTRIBUTES |
		FILE_NOTIFY_CHANGE_SIZE |
		FILE_NOTIFY_CHANGE_LAST_WRITE |
		FILE_NOTIFY_CHANGE_SECURITY
		); 

	if (shaderDirWatcherHandle != INVALID_HANDLE_VALUE)
	{
		OutputDebugString(L"Watch for dir '");
		OutputDebugString(curDir);
		OutputDebugString(L"'\n");
	} else
	{
		OutputDebugString(L"Watch for dir - failed!\n");
	}

	//D3DXQuaternionRotationYawPitchRoll(&cameraRotation, 0.0f, 0.0f, 0.0f);
}

Scene::~Scene()
{
	Cleanup();
}

void Scene::Cleanup()
{
	if (rtMain)
	{
		rtMain->Release();
		rtMain = NULL;
	}

	if (rtState0)
	{
		rtState0->Release();
		rtState0 = NULL;
	}

	if (rtState1)
	{
		rtState1->Release();
		rtState1 = NULL;
	}

	if (initialState)
	{
		initialState->Release();
		initialState = NULL;
	}

	if (state0)
	{
		state0->Release();
		state0 = NULL;
	}

	if (state1)
	{
		state1->Release();
		state1 = NULL;
	}

	if (quadVB)
	{
		quadVB->Release();
		quadVB = NULL;
	}

	if (vertexDecl)
	{
		vertexDecl->Release();
		vertexDecl = NULL;
	}

	if (d3d9)
	{
		d3d9->Release();
		d3d9 = NULL;
	}

	if (device)
	{
		device->Release();
		device = NULL;
	}
}

bool Scene::Init(HWND _hWnd)
{
	hWnd = _hWnd;

	d3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	if (!d3d9)
	{
		return false;
	}

	D3DPRESENT_PARAMETERS presentParams;
	ZeroMemory( &presentParams, sizeof( presentParams ) );
	presentParams.Windowed = TRUE;
	presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParams.BackBufferFormat = D3DFMT_UNKNOWN; //D3DFMT_X8R8G8B8;
	presentParams.EnableAutoDepthStencil = TRUE;
	presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
	presentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	presentParams.BackBufferCount = 1;
	presentParams.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
	presentParams.MultiSampleQuality = 1;

	d3d9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParams, &device );

	if (!device)
	{
		presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
		presentParams.MultiSampleQuality = 0;
		d3d9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParams, &device );
	}


	if (!device)
	{
		Cleanup();
		return false;
	}

	const D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	device->CreateVertexDeclaration( decl, &vertexDecl );
	if (!vertexDecl)
	{
		Cleanup();
		return false;
	}

	if (!CreateQuad())
	{
		Cleanup();
		return false;
	}

	D3DXCreateTextureFromFileEx(device, L"InitialState.dds", D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &initialState);
	if (!initialState)
	{
		Cleanup();
		return false;
	}


	D3DSURFACE_DESC textureDesc;
	initialState->GetLevelDesc(0, &textureDesc);

	device->CreateTexture(textureDesc.Width, textureDesc.Height, 1, D3DUSAGE_RENDERTARGET, textureDesc.Format, D3DPOOL_DEFAULT, &state0, NULL);
	if (!state0)
	{
		Cleanup();
		return false;
	}

	device->CreateTexture(textureDesc.Width, textureDesc.Height, 1, D3DUSAGE_RENDERTARGET, textureDesc.Format, D3DPOOL_DEFAULT, &state1, NULL);
	if (!state1)
	{
		Cleanup();
		return false;
	}

	device->GetRenderTarget(0, &rtMain);
	if (!rtMain)
	{
		Cleanup();
		return false;
	}

	state0->GetSurfaceLevel(0, &rtState0);
	if (!rtState0)
	{
		Cleanup();
		return false;
	}
	
	state1->GetSurfaceLevel(0, &rtState1);
	if (!rtState1)
	{
		Cleanup();
		return false;
	}

	LoadShaders();

	Reset();

	return true;
}


void Scene::Reset()
{
	device->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 255, 0, 0 ), 1.0f, 0 );
	device->BeginScene();
	DrawFullScreenQuad(rtState0, copyVS, copyPS, initialState);
	device->EndScene();

}

void Scene::DrawFullScreenQuad(IDirect3DSurface9 * renerTarget, VShader & vs, PShader & ps, IDirect3DTexture9 * texture)
{
	D3DSURFACE_DESC rtDesc;
	HRESULT hr = renerTarget->GetDesc(&rtDesc);

	device->SetRenderTarget(0, renerTarget);

	device->SetRenderState( D3DRS_ZENABLE, FALSE );
	device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	device->SetRenderState( D3DRS_LIGHTING, FALSE );
	device->SetRenderState( D3DRS_FOGENABLE, FALSE );
	device->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
	device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	device->SetVertexDeclaration(vertexDecl );

	device->SetStreamSource(0, quadVB, 0, sizeof(Vertex));

	D3DXVECTOR4 fViewportDimensions((float)rtDesc.Width, (float)rtDesc.Height, 0.0f, 0.0f);

	vs.SetFloat4(device, "fViewportDimensions", fViewportDimensions);
	vs.Set(device);

	ps.SetFloat4(device, "fViewportDimensions", fViewportDimensions);
	ps.SetSampler(device, "sourceTexture", texture);
	ps.Set(device);

	device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

}

bool Scene::CreateQuad()
{
	device->CreateVertexBuffer(6 * sizeof(Vertex), 0, 0, D3DPOOL_DEFAULT, &quadVB, NULL);
	if (!quadVB)
	{
		return false;
	}

	Vertex* vertexData = NULL;
	quadVB->Lock( 0, 0, ( void** )&vertexData, 0 );
	if (!vertexData)
	{
		return false;
	}

	/*

	0           2(4)
	*-----------*
	|           |
	|           |
	|           |
	|           |
	*-----------*
	1(5)        3

	*/

	vertexData[0].p = D3DXVECTOR4(-1.0f, -1.0f, 0.0f, 1.0f);
	vertexData[1].p = D3DXVECTOR4(-1.0f,  1.0f, 0.0f, 1.0f);
	vertexData[2].p = D3DXVECTOR4( 1.0f, -1.0f, 0.0f, 1.0f);
	vertexData[3].p = D3DXVECTOR4( 1.0f,  1.0f, 0.0f, 1.0f);
	vertexData[4] = vertexData[2];
	vertexData[5] = vertexData[1];

	quadVB->Unlock();

	return true;
}



void Scene::Draw(float fDeltaTime)
{
	if (GetAsyncKeyState('R') < 0 && GetForegroundWindow() == hWnd)
	{
		Reset();
	}

	device->SetRenderTarget(0, rtMain);

	device->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( rand(), rand(), rand() ), 1.0f, 0 );
	device->BeginScene();

	IDirect3DTexture9 * src = NULL;
	IDirect3DSurface9 * dest = NULL;
	IDirect3DTexture9 * res = NULL;

	if (parity == 0)
	{
		src = state0;
		dest = rtState1;
		res = state1;
		parity = 1;
	} else
	{
		src = state1;
		dest = rtState0;
		res = state0;
		parity = 0;
	}

	//step
	DrawFullScreenQuad(dest, stepVS, stepPS, src);

	//show to screen
	DrawFullScreenQuad(rtMain, copyVS, copyPS, res);

	device->EndScene();

	device->Present( NULL, NULL, NULL, NULL );

	//Simplest file watcher
	DWORD waitStatus = WaitForSingleObject(shaderDirWatcherHandle, 0);
	if (waitStatus == WAIT_OBJECT_0)
	{
		LoadShaders();
		FindNextChangeNotification(shaderDirWatcherHandle);
	}
}

void Scene::LoadShaders()
{
	stepVS.Create(device, "Step.hlsl");
	stepPS.Create(device, "Step.hlsl");

	copyVS.Create(device, "Copy.hlsl");
	copyPS.Create(device, "Copy.hlsl");

	OutputDebugStringA("--- Shaders created ----------------\n");
}