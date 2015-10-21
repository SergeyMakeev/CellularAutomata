#pragma once

#include <d3d9.h>
#include <d3dx9.h>


class Shader
{
protected:

	class ShaderIncluder : public ID3DXInclude 
	{

	public:

		HRESULT STDMETHODCALLTYPE Open (D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
		HRESULT STDMETHODCALLTYPE Close (LPCVOID pData);

	};

	
	ID3DXConstantTable * constantTable;

	ID3DXBuffer * CreateFromFile(const char * fileName, const char * profile, const char * entryPoint);
	virtual void Cleanup();

public:

	Shader();
	virtual ~Shader();


	virtual void Create(IDirect3DDevice9* device, const char * fileName) = 0;
	virtual void Set(IDirect3DDevice9* device) = 0;


	virtual void SetFloat4x4(IDirect3DDevice9* device, const char * name, const D3DXMATRIX & v);
	virtual void SetFloat4(IDirect3DDevice9* device, const char * name, const D3DXVECTOR4 & v);

};