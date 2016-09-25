//-----------------------------------------------------------------------------
//           Name: dx9_initialization.cpp
//         Author: Kevin Harris
//  Last Modified: 05/27/05
//    Description: This sample demonstrates how to initialize Direct3D
//-----------------------------------------------------------------------------

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "resource.h"

#include "Main.h"

#include "DebugWindow.h"


//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
    switch( msg )
	{
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
			}
		}
        break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
		}
		
        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;

		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: init()
// Desc: 
//-----------------------------------------------------------------------------
void DX9ExDebugWindow::init( LPDIRECT3DDEVICE9EX pDevice )
{
	vireio::debugf("init()");

	g_pGameDevice = pDevice;

	WNDCLASS winClass;
	winClass.lpszClassName = "HL2_WINDOW_CLASS";
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = GetModuleHandle(NULL);
	winClass.hIcon	       = NULL;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClass(&winClass) )
		return;

	g_hWnd = CreateWindow("HL2_WINDOW_CLASS", 
                             "Half-life 2",
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					         0, 0, 945, 1050, NULL, NULL, GetModuleHandle(NULL), NULL );

	if( g_hWnd == NULL )
	{
		vireio::debugf("g_hWnd == NULL");
		return;
	}

    ShowWindow( g_hWnd, SW_SHOWNOACTIVATE);

    UpdateWindow( g_hWnd );

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

    d3dpp.BackBufferFormat       = D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount		 = 1;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_COPY;
	d3dpp.Windowed               = TRUE;
    d3dpp.EnableAutoDepthStencil = FALSE;
    //d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

	DWORD dwBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

    if( FAILED( g_pD3DEx->CreateDeviceEx( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                                      dwBehaviorFlags, &d3dpp, NULL, &g_pD3DExDevice ) ) )
	{
		// TO DO: Respond to failure of CreateDevice
		vireio::debugf("Failed g_pD3DEx->CreateDeviceEx");
		return;
	}


	// 
    // Initialize the surface queues
    //
    SURFACE_QUEUE_DESC  desc    = {0};
    desc.Width                  = 945;
    desc.Height                 = 1050;
    desc.Format                 = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.NumSurfaces            = 2;
    desc.MetaDataSize           = 0;
    desc.Flags                  = SURFACE_QUEUE_FLAG_SINGLE_THREADED;

	HRESULT hr = E_NOTIMPL;

	if (FAILED(hr = CreateSurfaceQueue(&desc, g_pGameDevice, &g_Debugto9Queue)))
    {
  		vireio::debugf("CreateSurfaceQueue failed. \n");
		return;
    }

    // Clone the queue
    SURFACE_QUEUE_CLONE_DESC CloneDesc = {0};
    CloneDesc.MetaDataSize      = 0;
    CloneDesc.Flags             = SURFACE_QUEUE_FLAG_SINGLE_THREADED;
    if (FAILED(hr = g_Debugto9Queue->Clone(&CloneDesc, &g_9toDebugQueue)))
    {
  		vireio::debugf("g_ABQueue->Clone failed. \n");
        return;
    }



	if (FAILED(hr = g_Debugto9Queue->OpenProducer(g_pD3DExDevice, &g_9Producer)))
    {
  		vireio::debugf("g_BAQueue->OpenProducer failed. \n");
        return;
   }

    if (FAILED(hr = g_9toDebugQueue->OpenConsumer(g_pD3DExDevice, &g_DebugConsumer)))
    {
  		vireio::debugf("g_ABQueue->OpenConsumer failed. \n");
        return;
    }

	if (FAILED(hr = g_9toDebugQueue->OpenProducer(g_pGameDevice, &g_DebugProducer)))
    {
  		vireio::debugf("g_ABQueue->OpenProducer failed. \n");
        return;
    }

    if (FAILED(hr = g_Debugto9Queue->OpenConsumer(g_pGameDevice, &g_9Consumer)))
    {
  		vireio::debugf("g_BAQueue->OpenConsumer failed. \n");
        return;
    }

}

//-----------------------------------------------------------------------------
// Name: shutDown()
// Desc: 
//-----------------------------------------------------------------------------
void DX9ExDebugWindow::shutDown( void )
{
    if( g_pD3DExDevice != NULL )
        g_pD3DExDevice->Release();

    if( g_pD3DEx != NULL )
        g_pD3DEx->Release();
}

//-----------------------------------------------------------------------------
// Name: render()
// Desc: 
//-----------------------------------------------------------------------------
void DX9ExDebugWindow::copySurface( IDirect3DSurface9* pDirect3DSurface9 )
{
	D3DSURFACE_DESC desc;
	pDirect3DSurface9->GetDesc(&desc);

	IDirect3DTexture9*      pTexture9Left;
	REFIID                  surfaceID9 = __uuidof(IDirect3DTexture9);
	HRESULT hr = g_9Consumer->Dequeue(surfaceID9, (void**)&pTexture9Left, NULL, NULL, 0);
    if (FAILED(hr))
    {
  		vireio::debugf("g_9Consumer->Dequeue(surfaceID9, (void**)&pTexture9Left, NULL, NULL, 0); failed %.8x. \n", hr);
		return;
    }

	// Get the top level surface from the texture
	IDirect3DSurface9*      pSurface9;
    pTexture9Left->GetSurfaceLevel(0, &pSurface9);

	RECT rect;
	rect.left = 0;
	rect.right = desc.Width / 2;
	rect.top = 0;
	rect.bottom = desc.Height;
    hr = g_pGameDevice->StretchRect(pDirect3DSurface9, &rect, pSurface9, NULL, D3DTEXF_NONE);

    pSurface9->Release();

    // Produce Surface
    hr = g_DebugProducer->Enqueue(pTexture9Left, NULL, NULL, 0);
    pTexture9Left->Release();

}

void DX9ExDebugWindow::present()
{

	IDirect3DTexture9*      pTexture9Left;
	REFIID                  surfaceID9 = __uuidof(IDirect3DTexture9);
	HRESULT hr = g_DebugConsumer->Dequeue(surfaceID9, (void**)&pTexture9Left, NULL, NULL, 0);
	if (FAILED(hr))
 	{
  		vireio::debugf("g_DebugConsumer->Dequeue(surfaceID9, (void**)&pTexture9Left, NULL, NULL, 0); failed: %u. \n", hr);
		return;
	}

	IDirect3DSurface9*      pSurface9;
	pTexture9Left->GetSurfaceLevel(0, &pSurface9);

    g_pD3DExDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );

    g_pD3DExDevice->BeginScene();

	// Get the top level surface from the texture
	IDirect3DSurface9* pRenderTarget;
	hr = g_pD3DExDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pRenderTarget);


    // Present with D3D9
    hr = g_pD3DExDevice->StretchRect(pSurface9, NULL, pRenderTarget, NULL, D3DTEXF_NONE);

    pRenderTarget->Release();

    g_pD3DExDevice->EndScene();


    pSurface9->Release();
						
    // Produce Surface
    hr = g_9Producer->Enqueue(pTexture9Left, NULL, NULL, 0);
    pTexture9Left->Release();


    g_pD3DExDevice->Present( NULL, NULL, NULL, NULL );

}
