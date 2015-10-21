#pragma once

#include "shader.h"


class VShader : public Shader
{
	IDirect3DVertexShader9 * vertexShader;


	virtual void Cleanup();

public:

	VShader();
	virtual ~VShader();

	virtual void Create(IDirect3DDevice9* device, const char * fileName);
	virtual void Set(IDirect3DDevice9* device);

};