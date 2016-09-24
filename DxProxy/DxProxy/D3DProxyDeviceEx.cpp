/********************************************************************
Vireio Perception: Open-Source Stereoscopic 3D Driver
Copyright (C) 2012 Andres Hernandez

File <D3DProxyDeviceEx.cpp> and
Class <D3DProxyDeviceEx> :
Copyright (C) 2012 Andres Hernandez
Modifications Copyright (C) 2013 Chris Drain

Vireio Perception Version History:
v1.0.0 2012 by Andres Hernandez
v1.0.X 2013 by John Hicks, Neil Schneider
v1.1.x 2013 by Primary Coding Author: Chris Drain
Team Support: John Hicks, Phil Larkson, Neil Schneider
v2.0.x 2013 by Denis Reischl, Neil Schneider, Joshua Brown
v2.0.4 onwards 2014 by Grant Bagwell, Simon Brown and Neil Schneider

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "D3DProxyDeviceEx.h"
#include "StereoViewFactory.h"

#include <typeinfo>
#include <assert.h>
#include <comdef.h>
#include <tchar.h>
#include <WinUser.h>
#include "Resource.h"
#include <D3DX9Shader.h>

#include "DebugWindow.h"

#include <windows.h>
#include "Main.h"

#ifdef _DEBUG
#include "DxErr.h"
#endif

#pragma comment(lib, "d3dx9.lib")

#define SMALL_FLOAT 0.001f
#define	SLIGHTLY_LESS_THAN_ONE 0.999f

#define PI 3.141592654
#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / PI))

#define OUTPUT_HRESULT(hr) { _com_error err(hr); LPCTSTR errMsg = err.ErrorMessage(); OutputDebugString(errMsg); }

#define MAX_PIXEL_SHADER_CONST_2_0 32
#define MAX_PIXEL_SHADER_CONST_2_X 32
#define MAX_PIXEL_SHADER_CONST_3_0 224

#define MENU_ITEM_SEPARATION  40

#ifdef x64
#define PR_SIZET "I64"
#else
#define PR_SIZET ""
#endif

using namespace vireio;


/**
* Constructor : creates game handler and sets various states.
***/
D3DProxyDeviceEx::D3DProxyDeviceEx(IDirect3DDevice9Ex* pDevice, BaseDirect3D9Ex* pCreatedBy):
	BaseDirect3DDevice9Ex(pDevice, pCreatedBy),
	presentFlag(0)
{
    D3DXCreateFont(pDevice, -30, 0, 0, 1, false, DEFAULT_CHARSET,
                        OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                        "Arial", &m_D3D9Font);

	SHOW_CALL("D3DProxyDeviceEx");
	OutputDebugString("D3D ProxyDev Created\n");

}

/**
* Destructor : calls ReleaseEverything() and releases swap chains.
* @see ReleaseEverything()
***/
D3DProxyDeviceEx::~D3DProxyDeviceEx()
{
	SHOW_CALL("~D3DProxyDeviceEx");
	
}

#define IF_GUID(riid,a,b,c,d,e,f,g,h,i,j,k) if ((riid.Data1==a)&&(riid.Data2==b)&&(riid.Data3==c)&&(riid.Data4[0]==d)&&(riid.Data4[1]==e)&&(riid.Data4[2]==f)&&(riid.Data4[3]==g)&&(riid.Data4[4]==h)&&(riid.Data4[5]==i)&&(riid.Data4[6]==j)&&(riid.Data4[7]==k))
/**
* Catch QueryInterface calls and increment the reference counter if necesarry. 
***/
HRESULT WINAPI D3DProxyDeviceEx::QueryInterface(REFIID riid, LPVOID* ppv)
{
	SHOW_CALL("QueryInterface");
	
	//DEFINE_GUID(IID_IDirect3DDevice9Ex, 0xb18b10ce, 0x2649, 0x405a, 0x87, 0xf, 0x95, 0xf7, 0x77, 0xd4, 0x31, 0x3a);
	IF_GUID(riid,0xb18b10ce,0x2649,0x405a,0x87,0xf,0x95,0xf7,0x77,0xd4,0x31,0x3a)
	{
		if (ppv==NULL)
			return E_POINTER;

		this->AddRef();
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	return BaseDirect3DDevice9Ex::QueryInterface(riid,ppv);
}

/**
* Currently base TestCooperativeLevel() functionality.
***/
HRESULT WINAPI D3DProxyDeviceEx::TestCooperativeLevel()
{
	SHOW_CALL("TestCooperativeLevel");
	
	return BaseDirect3DDevice9Ex::TestCooperativeLevel();
	// The calling application will start releasing resources after TestCooperativeLevel returns D3DERR_DEVICENOTRESET.
}

/**
* Calls SetCursorProperties() using the actual left surface from the proxy of pCursorBitmap.
***/
HRESULT WINAPI D3DProxyDeviceEx::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	SHOW_CALL("SetCursorProperties");
	
	return BaseDirect3DDevice9Ex::SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

/**
* Creates a proxy (or wrapped) swap chain.
* @param pSwapChain [in, out] Proxy (wrapped) swap chain to be returned.
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain)
{
	SHOW_CALL("CreateAdditionalSwapChain");
	
	HRESULT result = BaseDirect3DDevice9Ex::CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);

	return result;
}

/**
* Provides the swap chain from the intern vector of active proxy (wrapped) swap chains.
* @param pSwapChain [in, out] The proxy (wrapped) swap chain to be returned.
* @see D3D9ProxySwapChain
***/
HRESULT WINAPI D3DProxyDeviceEx::GetSwapChain(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain)
{
	SHOW_CALL("GetSwapChain");
	
	BaseDirect3DDevice9Ex::GetSwapChain(iSwapChain, pSwapChain);

	return D3D_OK;
}

/**
* Calls release functions here and in stereo view class, releases swap chains and restores everything.
* Subclasses which override this method must call through to super method at the end of the subclasses
* implementation.
* @see ReleaseEverything()
* @see StereoView::ReleaseEverything()
***/
HRESULT WINAPI D3DProxyDeviceEx::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	SHOW_CALL("Reset");
	
	HRESULT hr;

	//Force no VSYNC in DX9Ex mode
	pPresentationParameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
//	pPresentationParameters->BackBufferWidth = BACK_BUFFER_WIDTH;
//	pPresentationParameters->BackBufferHeight = BACK_BUFFER_HEIGHT;

	hr = BaseDirect3DDevice9Ex::Reset(pPresentationParameters);

	// if the device has been successfully reset we need to recreate any resources we created
	if (hr == D3D_OK)  {
		stereoView->PostReset();
	}
	else {
#ifdef _DEBUG
		debugf("Error: %s error description: %s\n",
			DXGetErrorString(hr), DXGetErrorDescription(hr));
#endif
		OutputDebugString("Device reset failed");
	}

	return hr;
}

/**
* Calls release functions here and in stereo view class, releases swap chains and restores everything.
* Subclasses which override this method must call through to super method at the end of the subclasses
* implementation.
* @see ReleaseEverything()
* @see StereoView::ReleaseEverything()
***/
HRESULT WINAPI D3DProxyDeviceEx::ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
	SHOW_CALL("Reset");
	
	HRESULT hr;

	//Force no VSYNC in DX9Ex mode
	presentationParameters = *pPresentationParameters;
	presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	hr = BaseDirect3DDevice9Ex::ResetEx(&presentationParameters, pFullscreenDisplayMode);

	// if the device has been successfully reset we need to recreate any resources we created
	if (hr == D3D_OK)  {
		stereoView->PostReset();
	}
	else {
#ifdef _DEBUG
		debugf("Error: %s error description: %s\n",
			DXGetErrorString(hr), DXGetErrorDescription(hr));
#endif
		OutputDebugString("Device reset failed");
	}

	return hr;
}
/**
* Here the chosen stereoviews draw function is called to render to wrapped back buffer.
* All other final screen output is also done here.
***/
HRESULT WINAPI D3DProxyDeviceEx::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	SHOW_CALL("Present");
	
	CalcFPS();

	HandleLandmarkMoment(DeviceBehavior::WhenToDo::BEFORE_COMPOSITING);
	
	IDirect3DSurface9* pBackBuffer = NULL;
	BaseDirect3DDevice9Ex::GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

	HandleLandmarkMoment(DeviceBehavior::WhenToDo::AFTER_COMPOSITING);

	if (stereoView->initialized)
		stereoView->PrePresentEx(pBackBuffer, this);

	//Render to debug window
	m_pDX9ExDebugWindow->copySurface(pBackBuffer);

	HRESULT hr = S_OK;
	IDirect3DDevice9Ex *pDirect3DDevice9Ex = NULL;
	//Old skool
	hr =  BaseDirect3DDevice9Ex::PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion,  presentFlag);	
	
	m_pDX9ExDebugWindow->present();

	if (stereoView->initialized)
		stereoView->PostPresentEx(pBackBuffer, this);

	pBackBuffer->Release();

	return hr;
}



/**
* Here the chosen stereoviews draw function is called to render to wrapped back buffer.
* All other final screen output is also done here.
***/
HRESULT WINAPI D3DProxyDeviceEx::PresentEx(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion, DWORD flags)
{
	SHOW_CALL("PresentEx");
	
	HandleLandmarkMoment(DeviceBehavior::WhenToDo::BEFORE_COMPOSITING);
	
	IDirect3DSurface9* pBackBuffer = NULL;
	BaseDirect3DDevice9Ex::GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

	HandleLandmarkMoment(DeviceBehavior::WhenToDo::AFTER_COMPOSITING);

	if (stereoView->initialized)
		stereoView->PrePresentEx(pBackBuffer, this);

	HRESULT hr = S_OK;
	hr = BaseDirect3DDevice9Ex::PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion,   presentFlag);
	
	if (stereoView->initialized)
		stereoView->PostPresentEx(pBackBuffer, this);

	pBackBuffer->Release();

	return hr;
}

/**
* Calls the backbuffer using the stored active proxy (wrapped) swap chain.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetBackBuffer(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer)
{
	SHOW_CALL("GetBackBuffer");
	
	HRESULT result = BaseDirect3DDevice9Ex::GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);

	return result;
}

/**
* Creates a proxy (or wrapped) texture (D3DProxyTexture).
* Texture to be created only gets both stereo textures if game handler agrees.
* @see D3DProxyTexture
* @see GameHandler::ShouldDuplicateTexture()
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle)
{
	SHOW_CALL("CreateTexture");
	
	HRESULT creationResult;

	D3DPOOL newPool = Pool;

	HRESULT hr = S_OK;
	IDirect3DDevice9Ex *pDirect3DDevice9Ex = NULL;
	if (SUCCEEDED(getActual()->QueryInterface(IID_IDirect3DDevice9Ex, reinterpret_cast<void**>(&pDirect3DDevice9Ex))) &&
		Pool == D3DPOOL_MANAGED)
	{
		newPool = D3DPOOL_DEFAULT;
		pDirect3DDevice9Ex->Release();
	}

	// try and create left
	creationResult = BaseDirect3DDevice9Ex::CreateTexture(Width, Height, Levels, Usage, Format, newPool, ppTexture, pSharedHandle);

	return creationResult;
}

/**
* Creates a a proxy (or wrapped) volume texture (D3D9ProxyVolumeTexture).
* Volumes can't be used as render targets and therefore don't need to be stereo (in DX9).
* @see D3D9ProxyVolumeTexture
***/	
HRESULT WINAPI D3DProxyDeviceEx::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle)
{
	SHOW_CALL("CreateVolumeTexture");

	HRESULT hr = S_OK;
	IDirect3DDevice9Ex *pDirect3DDevice9Ex = NULL;
	if (SUCCEEDED(getActual()->QueryInterface(IID_IDirect3DDevice9Ex, reinterpret_cast<void**>(&pDirect3DDevice9Ex))) &&
		Pool == D3DPOOL_MANAGED)
	{
		Pool = D3DPOOL_DEFAULT;
		pDirect3DDevice9Ex->Release();
	}

	

	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);


	return creationResult;
}

/**
* Creates a proxy (or wrapped) cube texture (D3D9ProxyCubeTexture).
* Texture to be created only gets both stereo textures if game handler agrees.
* @see D3D9ProxyCubeTexture
* @see GameHandler::ShouldDuplicateCubeTexture() 
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
	SHOW_CALL("CreateCubeTexture");

	D3DPOOL newPool = Pool;

	HRESULT hr = S_OK;
	IDirect3DDevice9Ex *pDirect3DDevice9Ex = NULL;
	if (SUCCEEDED(getActual()->QueryInterface(IID_IDirect3DDevice9Ex, reinterpret_cast<void**>(&pDirect3DDevice9Ex))) &&
		Pool == D3DPOOL_MANAGED)
	{
		newPool = D3DPOOL_DEFAULT;
		pDirect3DDevice9Ex->Release();
	}


	HRESULT creationResult;

	// try and create left
	creationResult = BaseDirect3DDevice9Ex::CreateCubeTexture(EdgeLength, Levels, Usage, Format, newPool, ppCubeTexture, pSharedHandle);

	return creationResult;
}

/**
* Creates base vertex buffer pointer (BaseDirect3DVertexBuffer9).
* @see BaseDirect3DVertexBuffer9
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	SHOW_CALL("CreateVertexBuffer");
		
	HRESULT hr = S_OK;
	IDirect3DDevice9Ex *pDirect3DDevice9Ex = NULL;
	if (SUCCEEDED(getActual()->QueryInterface(IID_IDirect3DDevice9Ex, reinterpret_cast<void**>(&pDirect3DDevice9Ex))) &&
		Pool == D3DPOOL_MANAGED)
	{
		Pool = D3DPOOL_DEFAULT;
		pDirect3DDevice9Ex->Release();
	}

	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);


	return creationResult;
}

/**
* * Creates base index buffer pointer (BaseDirect3DIndexBuffer9).
* @see BaseDirect3DIndexBuffer9
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle)
{
	SHOW_CALL("CreateIndexBuffer");

	HRESULT hr = S_OK;
	IDirect3DDevice9Ex *pDirect3DDevice9Ex = NULL;
	if (SUCCEEDED(getActual()->QueryInterface(IID_IDirect3DDevice9Ex, reinterpret_cast<void**>(&pDirect3DDevice9Ex))) &&
		Pool == D3DPOOL_MANAGED)
	{
		Pool = D3DPOOL_DEFAULT;
		pDirect3DDevice9Ex->Release();
	}
	
	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);

	return creationResult;
}

/**
* Calls the public overloaded function.
* this method should ensure that this remains true.
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
												  DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
	SHOW_CALL("CreateRenderTarget1");

	// call public overloaded function
	return CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, false);
}

/**
* Surface to be created only gets both stereo surfaces if game handler agrees.
* @see GameHandler::ShouldDuplicateDepthStencilSurface() 
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
	SHOW_CALL("CreateDepthStencilSurface");
	
	HRESULT creationResult;

	//Override multisampling if DX9Ex
	D3DMULTISAMPLE_TYPE newMultiSample = MultiSample;
	DWORD newMultisampleQuality = MultisampleQuality;


	HRESULT hr = S_OK;
	IDirect3DDevice9Ex *pDirect3DDevice9Ex = NULL;
	if (SUCCEEDED(getActual()->QueryInterface(IID_IDirect3DDevice9Ex, reinterpret_cast<void**>(&pDirect3DDevice9Ex))))
	{
		newMultiSample = D3DMULTISAMPLE_NONE;
		newMultisampleQuality = 0;
	}
	
	// create left/mono
	creationResult = BaseDirect3DDevice9Ex::CreateDepthStencilSurface(Width, Height, Format, newMultiSample, newMultisampleQuality, Discard, ppSurface, pSharedHandle);
	
	return creationResult;
}

/**
* Copies rectangular subsets of pixels from one proxy (wrapped) surface to another.
* @see D3D9ProxySurface
***/
HRESULT WINAPI D3DProxyDeviceEx::UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint)
{
	SHOW_CALL("UpdateSurface");
	
	if (!pSourceSurface || !pDestinationSurface)
		return D3DERR_INVALIDCALL;

	HRESULT result = BaseDirect3DDevice9Ex::UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
	
	return result;
}

/**
* Calls a helper function to unwrap the textures and calls the super method for both sides.
* The super method updates the dirty portions of a texture.
* @see vireio::UnWrapTexture()
***/
HRESULT WINAPI D3DProxyDeviceEx::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	SHOW_CALL("UpdateTexture");
	
	if (!pSourceTexture || !pDestinationTexture)
		return D3DERR_INVALIDCALL;


	HRESULT result = BaseDirect3DDevice9Ex::UpdateTexture(pSourceTexture, pDestinationTexture);
	
	return result;
}

/**
* Copies the render-target data from proxy (wrapped) source surface to proxy (wrapped) destination surface.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetRenderTargetData(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface)
{
	SHOW_CALL("GetRenderTarget");
	
	if ((pDestSurface == NULL) || (pRenderTarget == NULL))
		return D3DERR_INVALIDCALL;

	HRESULT result = BaseDirect3DDevice9Ex::GetRenderTargetData(pRenderTarget, pDestSurface);

	return result;
}

/**
* Gets the front buffer data from the internal stored active proxy (or wrapped) swap chain.
* @see D3D9ProxySwapChain
***/
HRESULT WINAPI D3DProxyDeviceEx::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	SHOW_CALL("GetFrontBufferData");
	
	HRESULT result = BaseDirect3DDevice9Ex::GetFrontBufferData(iSwapChain, pDestSurface);

	return result;
}

/**
* Copy the contents of the source proxy (wrapped) surface rectangles to the destination proxy (wrapped) surface rectangles.
***/
HRESULT WINAPI D3DProxyDeviceEx::StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
	SHOW_CALL("StretchRect");
	
	if (!pSourceSurface || !pDestSurface)
		return D3DERR_INVALIDCALL;

	HRESULT result = BaseDirect3DDevice9Ex::StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);


	return result;
}

/**
* Fills the rectangle for both stereo sides if switchDrawingSide() agrees and sets the render target accordingly.
* @see switchDrawingSide()
***/
HRESULT WINAPI D3DProxyDeviceEx::ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color)
{
	SHOW_CALL("ColorFill");
	
	HRESULT result;
	
	result = BaseDirect3DDevice9Ex::ColorFill(pSurface, pRect, color);


	return result;
}

HRESULT WINAPI D3DProxyDeviceEx::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{	
	SHOW_CALL("CreateOffscreenPlainSurface");
	
	D3DPOOL newPool = Pool;

	HRESULT hr = S_OK;
	IDirect3DDevice9Ex *pDirect3DDevice9Ex = NULL;
	if (SUCCEEDED(getActual()->QueryInterface(IID_IDirect3DDevice9Ex, reinterpret_cast<void**>(&pDirect3DDevice9Ex))) &&
		Pool == D3DPOOL_MANAGED)
	{
		newPool = D3DPOOL_DEFAULT;
		pDirect3DDevice9Ex->Release();
	}

	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateOffscreenPlainSurface(Width, Height, Format, newPool, ppSurface, pSharedHandle);

	return creationResult;
}

/**
* Updates render target accordingly to current render side.
* Updates proxy collection of stereo render targets to reflect new actual render target.
***/
HRESULT WINAPI D3DProxyDeviceEx::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	SHOW_CALL("SetRenderTarget");
	
	//// Update actual render target ////
	HRESULT result = BaseDirect3DDevice9Ex::SetRenderTarget(RenderTargetIndex, pRenderTarget);
	
	return result;
}

/**
* Provides render target from the internally stored active proxy (wrapped) render targets.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget)
{
	SHOW_CALL("GetRenderTarget");
	
	HRESULT result = BaseDirect3DDevice9Ex::GetRenderTarget(RenderTargetIndex, ppRenderTarget);
	return result;
}

/**
* Updates depth stencil accordingly to current render side.
* Updates stored proxy (or wrapped) depth stencil.
***/
HRESULT WINAPI D3DProxyDeviceEx::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	SHOW_CALL("SetDepthStencilSurface");
	
	HRESULT result = BaseDirect3DDevice9Ex::SetDepthStencilSurface(pNewZStencil);


	return result;
}

/**
* Provides the active proxy (wrapped) depth stencil.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	SHOW_CALL("GetDepthStencilSurface");
	
	return BaseDirect3DDevice9Ex::GetDepthStencilSurface(ppZStencilSurface);
}

/**
* Updates tracker if device says it should.  Handles controls if this is the first scene of the frame.
* Because input for this frame would already have been handled here so injection of any mouse 
* manipulation ?
***/
HRESULT WINAPI D3DProxyDeviceEx::BeginScene()
{
	SHOW_CALL("BeginScene");

	//HACK
	static int presentCount = 0;
	if (GetAsyncKeyState(VK_INSERT) &&
		!stereoView->initialized)
	{
		stereoView->Init(getActual());
	}

	HandleLandmarkMoment(DeviceBehavior::WhenToDo::BEGIN_SCENE);

	return BaseDirect3DDevice9Ex::BeginScene();
}

/**
* VPMENU called here for source engine games.
***/
HRESULT WINAPI D3DProxyDeviceEx::EndScene()
{
	SHOW_CALL("EndScene");

	HandleLandmarkMoment(DeviceBehavior::WhenToDo::END_SCENE);

	return BaseDirect3DDevice9Ex::EndScene();
}

/**
* Clears both stereo sides if switchDrawingSide() agrees.
***/
HRESULT WINAPI D3DProxyDeviceEx::Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	SHOW_CALL("Clear");
	
	HRESULT result;

	result = BaseDirect3DDevice9Ex::Clear(Count, pRects, Flags, Color, Z, Stencil);

	return result;
}

/**
* Catches transform for stored proxy state block accordingly or updates proxy device.
* @see D3D9ProxyStateBlock
***/
HRESULT WINAPI D3DProxyDeviceEx::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	SHOW_CALL("SetTransform");
	return BaseDirect3DDevice9Ex::SetTransform(State, pMatrix);
}

/**
* Not implemented now - fix in case it needs fixing, calls super method.
***/
HRESULT WINAPI D3DProxyDeviceEx::MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	SHOW_CALL("MultiplyTransform");
	
	return BaseDirect3DDevice9Ex::MultiplyTransform(State, pMatrix);
}

/**
* Try and set, if success save viewport.
* Also, it captures the viewport state in stored proxy state block.
* If viewport width and height match primary render target size and zmin is 0 and zmax 1 set 
* m_bActiveViewportIsDefault flag true.
* @see D3D9ProxyStateBlock::SelectAndCaptureState()
* @see m_bActiveViewportIsDefault
***/
HRESULT WINAPI D3DProxyDeviceEx::SetViewport(CONST D3DVIEWPORT9* pViewport)
{	
	SHOW_CALL("SetViewport");
	
	HRESULT result = BaseDirect3DDevice9Ex::SetViewport(pViewport);
		
	return result;
}

/**
* Creates proxy state block.
* Also, selects capture type option according to state block type.
* @param ppSB [in, out] The proxy (or wrapped) state block returned.
* @see D3DProxyStateBlock
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB)
{
	SHOW_CALL("CreateStateBlock");
	
	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateStateBlock(Type, ppSB);

	return creationResult;
}

/**
* Creates and stores proxy state block.
* @see D3DProxyStateBlock
***/
HRESULT WINAPI D3DProxyDeviceEx::BeginStateBlock()
{
	SHOW_CALL("BeginStateBlock");
	
	HRESULT result = BaseDirect3DDevice9Ex::BeginStateBlock();

	return result;
}

/**
* Calls both super method and method from stored proxy state block.
* @param [in, out] The returned proxy (or wrapped) state block.
* @see D3D9ProxyStateBlock::EndStateBlock()
***/
HRESULT WINAPI D3DProxyDeviceEx::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	SHOW_CALL("ppSB");
	
	HRESULT creationResult = BaseDirect3DDevice9Ex::EndStateBlock(ppSB);

	return creationResult;
}

/**
* Provides texture from stored active (mono) texture stages.
* @see D3D9ProxyTexture
***/
HRESULT WINAPI D3DProxyDeviceEx::GetTexture(DWORD Stage,IDirect3DBaseTexture9** ppTexture)
{
	SHOW_CALL("GetTexture");
	

	return BaseDirect3DDevice9Ex::GetTexture(Stage,ppTexture);
}

/**
* Calls a helper function to unwrap the textures and calls the super method for both sides.
* Update stored active (mono) texture stages if new texture was successfully set.
*
* @see vireio::UnWrapTexture() 
***/
HRESULT WINAPI D3DProxyDeviceEx::SetTexture(DWORD Stage,IDirect3DBaseTexture9* pTexture)
{
	SHOW_CALL("SetTexture");
	
	HRESULT result;
	result = BaseDirect3DDevice9Ex::SetTexture(Stage, pTexture);
	return result;
}

/**
* Applies all dirty shader registers, draws both stereo sides if switchDrawingSide() agrees.
* @see switchDrawingSide()
***/
HRESULT WINAPI D3DProxyDeviceEx::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	SHOW_CALL("DrawPrimitive");
	
	HRESULT result = BaseDirect3DDevice9Ex::DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);

	return result;
}

/**
* Applies all dirty shader registers, draws both stereo sides if switchDrawingSide() agrees.
* @see switchDrawingSide()
***/
HRESULT WINAPI D3DProxyDeviceEx::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount)
{
	SHOW_CALL("DrawIndexedPrimitive");
	
	HRESULT result = BaseDirect3DDevice9Ex::DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

	return result;
}

/**
* Applies all dirty shader registers, draws both stereo sides if switchDrawingSide() agrees.
* @see switchDrawingSide()
***/
HRESULT WINAPI D3DProxyDeviceEx::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	SHOW_CALL("DrawPrimitiveUP");
	
	HRESULT result = BaseDirect3DDevice9Ex::DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);

	return result;
}

/**
* Applies all dirty shader registers, draws both stereo sides if switchDrawingSide() agrees.
* @see switchDrawingSide()
***/
HRESULT WINAPI D3DProxyDeviceEx::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	SHOW_CALL("DrawIndexedPrimitiveUP");
	
	HRESULT result = BaseDirect3DDevice9Ex::DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);

	return result;
}

/**
* Applies all dirty shader registers, processes vertices.
***/
HRESULT WINAPI D3DProxyDeviceEx::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags)
{
	SHOW_CALL("ProcessVertices");
	
	HRESULT result = BaseDirect3DDevice9Ex::ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);

	return result;
}

/**
* Creates base vertex declaration (BaseDirect3DVertexDeclaration9).
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl)
{
	SHOW_CALL("CreateVertexDeclaration");
	
	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateVertexDeclaration(pVertexElements, ppDecl );

	return creationResult;
}

/**
* Catches vertex declaration in stored proxy state block.
* First, set vertex declaration by base function.
* @see D3D9ProxyStateBlock
***/
HRESULT WINAPI D3DProxyDeviceEx::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	SHOW_CALL("SetVertexDeclaration");
	
	// Update actual Vertex Declaration
	HRESULT result = BaseDirect3DDevice9Ex::SetVertexDeclaration(pDecl);

	return result;
}

/**
* Provides currently stored vertex declaration.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	SHOW_CALL("GetVertexDelcaration");

	HRESULT result = BaseDirect3DDevice9Ex::GetVertexDeclaration(ppDecl);

	return D3D_OK;
}

/**
* Creates proxy (wrapped) vertex shader.
* @param ppShader [in, out] The created proxy vertex shader.
* @see D3D9ProxyVertexShader
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateVertexShader(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader)
{
	SHOW_CALL("CreateVertexShader");
	
	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateVertexShader(pFunction, ppShader);


	return creationResult;
}

/**
* Sets and updates stored proxy vertex shader.
* @see D3D9ProxyVertexShader
***/
HRESULT WINAPI D3DProxyDeviceEx::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	SHOW_CALL("SetVertexSHader");
	
	// Update actual Vertex shader
	HRESULT result = BaseDirect3DDevice9Ex::SetVertexShader(pShader);
	return result;
}

/**
* Returns the stored and active proxy vertex shader.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	SHOW_CALL("GetVertexShader");
	
	HRESULT result = BaseDirect3DDevice9Ex::GetVertexShader(ppShader);

	return D3D_OK;
}

/**
* Sets shader constants either at stored proxy state block or in managed shader register class.
* @see D3D9ProxyStateBlock
* @see ShaderRegisters
***/
HRESULT WINAPI D3DProxyDeviceEx::SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
	SHOW_CALL("SEtVertexShadewrConstantF");
	
	HRESULT result = BaseDirect3DDevice9Ex::SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	
	return result;
}

/**
* Provides constant registers from managed shader register class.
* @see ShaderRegisters
* @see ShaderRegisters::GetVertexShaderConstantF()
***/
HRESULT WINAPI D3DProxyDeviceEx::GetVertexShaderConstantF(UINT StartRegister,float* pData,UINT Vector4fCount)
{
	SHOW_CALL("GetVertexShaderConstantF");
	
	HRESULT result = BaseDirect3DDevice9Ex::GetVertexShaderConstantF(StartRegister, pData, Vector4fCount);
	return result;
}

/**
* Sets stream source and updates stored vertex buffers.
* Also, it calls proxy state block to capture states.
* @see D3D9ProxyStateBlock::SelectAndCaptureState()
***/
HRESULT WINAPI D3DProxyDeviceEx::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{	
	SHOW_CALL("SetStreamSource");
	
	HRESULT result = BaseDirect3DDevice9Ex::SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
	return result;
}

/**
* Provides stream data from stored vertex buffers.
* TODO ppStreamData is marked in and out in docs. Potentially it can be a get when the stream hasn't been set before???
* Category of probleme: Worry about it if it breaks.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride)
{
	SHOW_CALL("GetSTreamSource");
	
	HRESULT result = BaseDirect3DDevice9Ex::GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride);
	return result;
}

/**
* Sets indices and calls proxy state block to capture states.
* @see D3D9ProxyStateBlock::SelectAndCaptureState()
***/
HRESULT WINAPI D3DProxyDeviceEx::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	SHOW_CALL("SetIndices");
	
	return BaseDirect3DDevice9Ex::SetIndices(pIndexData);
}

/**
* Provides stored indices.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	SHOW_CALL("GetIndices");
	
	return BaseDirect3DDevice9Ex::GetIndices(ppIndexData);
}

/**
* Base CreatePixelShader functionality.
***/
HRESULT WINAPI D3DProxyDeviceEx::CreatePixelShader(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader)
{
	SHOW_CALL("CreatePixelSHader");
	
	HRESULT creationResult = BaseDirect3DDevice9Ex::CreatePixelShader(pFunction, ppShader);

	return creationResult;
}

/**
* Sets pixel shader and calls proxy state block to capture states.
* @see D3D9ProxyStateBlock::SelectAndCaptureState()
***/
HRESULT WINAPI D3DProxyDeviceEx::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	SHOW_CALL("SetPixelShader");
	
	// Update actual pixel shader
	HRESULT result = BaseDirect3DDevice9Ex::SetPixelShader(pShader);

	return result;
}

/**
* Provides stored pixel shader.
***/
HRESULT WINAPI D3DProxyDeviceEx::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	SHOW_CALL("GetPixelSHader");
	
	HRESULT result = BaseDirect3DDevice9Ex::GetPixelShader(ppShader);

	return D3D_OK;
}

/**
* Sets shader constants either at stored proxy state block or in managed shader register class.
* @see D3D9ProxyStateBlock
* @see ShaderRegisters
***/
HRESULT WINAPI D3DProxyDeviceEx::SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
	SHOW_CALL("SetPixelShaderConstantF");
	
	HRESULT result = BaseDirect3DDevice9Ex::SetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount);

	return result;
}

/**
* Provides constant registers from managed shader register class.
* @see ShaderRegisters
* @see ShaderRegisters::GetPixelShaderConstantF()
***/
HRESULT WINAPI D3DProxyDeviceEx::GetPixelShaderConstantF(UINT StartRegister,float* pData,UINT Vector4fCount)
{
	SHOW_CALL("GetPixelSHaderConstantF");
	
	HRESULT result = BaseDirect3DDevice9Ex::GetPixelShaderConstantF(StartRegister,pData,Vector4fCount);
	return result;
}

/**
* Applies all dirty registers, draws both stereo sides if switchDrawingSide() agrees.
* @see switchDrawingSide()
***/
HRESULT WINAPI D3DProxyDeviceEx::DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	SHOW_CALL("DrawRectPatch");
	
	HRESULT result = BaseDirect3DDevice9Ex::DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
	

	return result;
}

/**
* Applies all dirty registers, draws both stereo sides if switchDrawingSide() agrees.
* @see switchDrawingSide() 
***/
HRESULT WINAPI D3DProxyDeviceEx::DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	SHOW_CALL("DrawTriPatch");
	
	HRESULT result = BaseDirect3DDevice9Ex::DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
	return result;
}

/**
* Base CreateQuery functionality.
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery)
{
	SHOW_CALL("CreateQuery");
	
	// this seems a weird response to me but it's what the actual device does.
	if (!ppQuery)
		return D3D_OK;

	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateQuery(Type, ppQuery);

	return creationResult;
}

/**
* Duplicates render target if game handler agrees.
* @see GameHandler::ShouldDuplicateRenderTarget()
***/
HRESULT WINAPI D3DProxyDeviceEx::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
												  DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle, bool isSwapChainBackBuffer)
{
	SHOW_CALL("CreateRenderTarget2");

	HRESULT creationResult = BaseDirect3DDevice9Ex::CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);

	return creationResult;
}

/**
* This method must be called on the proxy device before the device is returned to the calling application.
* Inits by game configuration.
* Subclasses which override this method must call through to super method.
* Anything that needs to be done before the device is used by the actual application should happen here.
* @param The game (or engine) specific configuration.
***/
void D3DProxyDeviceEx::Init()
{
	SHOW_CALL("Init");
	
	OutputDebugString("D3D ProxyDev Init\n");

	stereoView = StereoViewFactory::Get();

	//Debug Window
	m_pDX9ExDebugWindow = new DX9ExDebugWindow();
	
	//Init the debug window
	m_pDX9ExDebugWindow->init(getActual());
}


void D3DProxyDeviceEx::HandleLandmarkMoment(DeviceBehavior::WhenToDo when)
{
	if (when == DeviceBehavior::WhenToDo::BEFORE_COMPOSITING)
	{
		D3DCOLOR colour = D3DCOLOR_ARGB(255, 0, 255, 0);
		RECT rect = {20, 20, 1000, 500};

		char buffer[256];
		sprintf_s(buffer, "FPS: %.1f", fps);
		m_D3D9Font->DrawText(NULL, buffer, -1, &rect, 0, colour);

		if (!stereoView->initialized)
		{
			sprintf_s(buffer, "Press INSERT to start HMD Direct Mode");
			rect.top = 90;
			m_D3D9Font->DrawText(NULL, buffer, -1, &rect, 0, colour);
		}
	}
}

//FPS Calculator

#define MAXSAMPLES 20

void D3DProxyDeviceEx::CalcFPS()
{
	static bool init=false;
	static int tickindex=0;
	static LONGLONG ticksum=0;
	static LONGLONG ticklist[MAXSAMPLES];
	static LONGLONG prevTick;
	static LARGE_INTEGER perffreq;
	if (!init)
	{
		//Initialise - should only ever happen once
		memset(ticklist, 0, sizeof(LONGLONG) * MAXSAMPLES);
		QueryPerformanceFrequency(&perffreq);
		init=true;
	}

	//Get the new tick
	LARGE_INTEGER newPerfCount;
	QueryPerformanceCounter(&newPerfCount);

	LONGLONG newtick = newPerfCount.QuadPart - prevTick;
	ticksum -= ticklist[tickindex];
    ticksum += newtick;
    ticklist[tickindex] = newtick;
    tickindex = ++tickindex % MAXSAMPLES;
	prevTick = newPerfCount.QuadPart;

	fps = (float)((double)MAXSAMPLES / ((double)ticksum / (double)perffreq.QuadPart));
}