#include <windows.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include "scene.h"


#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shlwapi.lib")

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	HANDLE hProcess = GetCurrentProcess();
	wchar_t exePath[MAX_PATH];
	DWORD s = GetModuleFileNameEx(hProcess, NULL, exePath, MAX_PATH);
	PathRemoveFileSpec(exePath);
	SetCurrentDirectory( exePath );

	wchar_t * className = L"Cellular Automata";
	wchar_t * windowTitle = L"Cellular Automata";

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );

	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle( NULL ), NULL, cursor, NULL, NULL, className, NULL
	};

	RegisterClassEx( &wc );

	Scene scene;


	DWORD windowStyle = WS_POPUPWINDOW | WS_THICKFRAME | WS_CAPTION;

	RECT rc;
	SetRect( &rc, 0, 0, 1280, 720 );
	AdjustWindowRect( &rc, windowStyle, false );

	int x = 100;
	int y = 100;

	HWND hWnd = CreateWindow( className, windowTitle, windowStyle, x, y, ( rc.right - rc.left ), ( rc.bottom - rc.top ), NULL, NULL, wc.hInstance, NULL );


	bool initResult = scene.Init(hWnd);
	if (!initResult)
	{
		MessageBox(hWnd, L"Can't initialize direct 3d", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}


	ShowWindow( hWnd, SW_SHOWDEFAULT );
	UpdateWindow( hWnd );


	HANDLE curThread = GetCurrentThread();
	SetThreadAffinityMask(curThread, 1);

	LARGE_INTEGER freq;
	QueryPerformanceFrequency( &freq );

	LARGE_INTEGER prevTime;
	QueryPerformanceCounter(&prevTime);

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );
	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		} else
		{
			LARGE_INTEGER curTime;
			QueryPerformanceCounter(&curTime);

			LONGLONG deltaTime = curTime.QuadPart - prevTime.QuadPart;
			prevTime = curTime;

			float fDeltaTime = (float)((double)deltaTime / (double)freq.QuadPart);

			scene.Draw(fDeltaTime);
		}
	}


	UnregisterClass( className, wc.hInstance );
	return 0;
}