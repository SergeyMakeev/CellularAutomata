#pragma once

#include <windows.h>
#include <d3d9.h>

#include "VShader.h"
#include "PShader.h"

class Scene
{
	struct Vertex
	{
		D3DXVECTOR4 p;
	};

	HWND hWnd;

	IDirect3D9 * d3d9;
	IDirect3DDevice9 * device;

	IDirect3DVertexDeclaration9 * vertexDecl;

	IDirect3DVertexBuffer9 * quadVB;

	IDirect3DTexture9 * initialState;

	IDirect3DTexture9 * state0;
	IDirect3DTexture9 * state1;

	IDirect3DSurface9 * rtMain;
	IDirect3DSurface9 * rtState0;
	IDirect3DSurface9 * rtState1;

	VShader stepVS;
	PShader stepPS;

	VShader copyVS;
	PShader copyPS;

	HANDLE shaderDirWatcherHandle;

	int parity;

	void Cleanup();

	void LoadShaders();

	bool CreateQuad();

	void Reset();

	void DrawFullScreenQuad(IDirect3DSurface9 * renerTarget, VShader & vs, PShader & ps, IDirect3DTexture9 * texture);


public:

	Scene();
	virtual ~Scene();

	bool Init(HWND hWnd);

	void Draw(float fDeltaTime);
};