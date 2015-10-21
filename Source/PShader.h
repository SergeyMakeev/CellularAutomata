#pragma once

#include "shader.h"


class PShader : public Shader
{
	IDirect3DPixelShader9 * pixelShader;


	virtual void Cleanup();

public:

	PShader();
	virtual ~PShader();

	virtual void Create(IDirect3DDevice9* device, const char * fileName);
	virtual void Set(IDirect3DDevice9* device);

	virtual void SetSampler(IDirect3DDevice9* device, const char * name, IDirect3DBaseTexture9 * texture);

};