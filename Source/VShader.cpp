#include "VShader.h"

VShader::VShader() : vertexShader ( NULL )
{
}

VShader::~VShader()
{
	Cleanup();
}

void VShader::Cleanup()
{
	Shader::Cleanup();

	if (vertexShader)
	{
		vertexShader->Release();
		vertexShader = NULL;
	}
}

void VShader::Create(IDirect3DDevice9* device, const char * fileName)
{
	Cleanup();

	ID3DXBuffer * shaderCode = Shader::CreateFromFile(fileName, "vs_3_0", "mainVS");
	if (shaderCode)
	{
		device->CreateVertexShader( ( DWORD* )shaderCode->GetBufferPointer(), &vertexShader );
		shaderCode->Release();
	}
}

void VShader::Set(IDirect3DDevice9* device)
{
	device->SetVertexShader( vertexShader );
}
