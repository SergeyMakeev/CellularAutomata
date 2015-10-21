#include "PShader.h"

PShader::PShader() : pixelShader ( NULL )
{
}

PShader::~PShader()
{
	Cleanup();
}

void PShader::Cleanup()
{
	Shader::Cleanup();

	if (pixelShader)
	{
		pixelShader->Release();
		pixelShader = NULL;
	}
}

void PShader::Create(IDirect3DDevice9* device, const char * fileName)
{
	Cleanup();

	ID3DXBuffer * shaderCode = Shader::CreateFromFile(fileName, "ps_3_0", "mainPS");
	if (shaderCode)
	{
		device->CreatePixelShader( ( DWORD* )shaderCode->GetBufferPointer(), &pixelShader );
		shaderCode->Release();
	}
}

void PShader::Set(IDirect3DDevice9* device)
{
	device->SetPixelShader( pixelShader );
}

void PShader::SetSampler(IDirect3DDevice9* device, const char * name, IDirect3DBaseTexture9 * texture)
{
	if (!constantTable)
	{
		return;
	}

	D3DXHANDLE h = constantTable->GetConstantByName(NULL, name);
	if (!h)
	{
		return;
	}

	UINT samplerIndex = constantTable->GetSamplerIndex(h);

	device->SetTexture(samplerIndex, texture);
	device->SetSamplerState(samplerIndex, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(samplerIndex, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(samplerIndex, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	device->SetSamplerState(samplerIndex, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
}