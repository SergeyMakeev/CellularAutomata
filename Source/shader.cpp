#include <stdio.h>
#include "shader.h"




HRESULT Shader::ShaderIncluder::Open (D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	FILE * file = NULL;
	
	errno_t openResult = fopen_s(&file, pFileName, "rb");

	if (openResult != 0 || file == NULL)
	{
		return  D3DERR_NOTFOUND;
	}

	fseek(file, 0, SEEK_END);
	long  fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);


	void * buffer = malloc(fileSize);

	size_t readedBytes = fread(buffer, 1, fileSize, file);
	if (readedBytes != fileSize)
	{
		free(buffer);
		buffer = NULL;

		fclose(file);
		file = NULL;

		return D3DERR_INVALIDCALL;
	}


	fclose(file);
	file = NULL;

	*ppData = buffer;
	*pBytes = fileSize;

	return S_OK;
}

HRESULT Shader::ShaderIncluder::Close (LPCVOID pData)
{
	if (pData)
	{
		free((void*)pData);
	}

	return S_OK;
}



Shader::Shader() : constantTable ( NULL )
{

}

Shader::~Shader()
{
	Cleanup();
}


void Shader::Cleanup()
{
	if (constantTable)
	{
		constantTable->Release();
		constantTable = NULL;
	}
}

ID3DXBuffer * Shader::CreateFromFile(const char * fileName, const char * profile, const char * entryPoint)
{
	ID3DXBuffer * shaderCode = NULL;
	ID3DXBuffer * compileErrors = NULL;
	DWORD compileFlag = D3DXSHADER_SKIPOPTIMIZATION | D3DXSHADER_DEBUG;

	static ShaderIncluder includeHandler;
	HRESULT hr = D3DXCompileShaderFromFileA(fileName, NULL, &includeHandler, entryPoint, profile, compileFlag, &shaderCode, &compileErrors, &constantTable);

	if (hr == D3D_OK)
	{
		return shaderCode;
	}

	OutputDebugStringA("Shader compile error:\n");
	OutputDebugStringA(fileName);
	OutputDebugStringA("\n");
	if (compileErrors)
	{
		const char * errorMessage = (const char *)compileErrors->GetBufferPointer();
		OutputDebugStringA(errorMessage);
	} else
	{
		OutputDebugStringA("Unknown error\n");
	}
	OutputDebugStringA("\n---------------------------------------------\n");
	

	return NULL;
}


void Shader::SetFloat4x4(IDirect3DDevice9* device, const char * name, const D3DXMATRIX & v)
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

	constantTable->SetMatrix(device, h, &v);
}

void Shader::SetFloat4(IDirect3DDevice9* device, const char * name, const D3DXVECTOR4 & v)
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

	constantTable->SetVector(device, h, &v);
}

