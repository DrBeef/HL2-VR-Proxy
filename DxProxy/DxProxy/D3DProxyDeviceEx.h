/********************************************************************
Vireio Perception: Open-Source Stereoscopic 3D Driver
Copyright (C) 2012 Andres Hernandez

File <D3DProxyDevice.h> and
Class <D3DProxyDevice> :
Copyright (C) 2012 Andres Hernandez
Modifications Copyright (C) 2013 Chris Drain

Vireio Perception Version History:
v1.0.0 2012 by Andres Hernandez
v1.0.X 2013 by John Hicks, Neil Schneider
v1.1.x 2013 by Primary Coding Author: Chris Drain
Team Support: John Hicks, Phil Larkson, Neil Schneider
v2.0.x 2013 by Denis Reischl, Neil Schneider, Joshua Brown

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

#ifndef D3DPROXYDEVICEEX_H_INCLUDED
#define D3DPROXYDEVICEEX_H_INCLUDED

#define VPMENU_PIXEL_WIDTH 1920
#define VPMENU_PIXEL_HEIGHT 1080

#pragma warning (disable : 4005)

#include "Direct3DDevice9Ex.h"
#include "Main.h"


#define _SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; } 
#define RECT_WIDTH(x) (x.right - x.left)
#define RECT_HEIGHT(x) (x.bottom - x.top)

#define COOLDOWN_ONE_FRAME 0.7f
#define COOLDOWN_SHORT 2.0f
#define COOLDOWN_LONG 4.0f
#define COOLDOWN_EXTRA_LONG 10.0f


// Define SHOW_CALLS to have each method output a debug string when it is invoked
//#define SHOW_CALLS

class StereoView;
class DX9ExDebugWindow;


/**
* Direct 3D proxy device class. 
* Basically this class hosts all proxy wrappers accordingly to the methods called by the game.
*/
class D3DProxyDeviceEx : public BaseDirect3DDevice9Ex
{
public:
	D3DProxyDeviceEx(IDirect3DDevice9Ex* pDevice, BaseDirect3D9Ex* pCreatedBy);
	virtual ~D3DProxyDeviceEx();

	friend class D3D9ProxyStateBlock;

	/*** IUnknown methods ***/
	virtual HRESULT WINAPI QueryInterface(REFIID riid, LPVOID* ppv);

	/*** IDirect3DDevice9 methods ***/
	virtual HRESULT WINAPI TestCooperativeLevel();
	virtual HRESULT WINAPI SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap);
	virtual HRESULT WINAPI CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain);
	virtual HRESULT WINAPI GetSwapChain(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain);
	virtual HRESULT WINAPI Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
	virtual HRESULT WINAPI Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	virtual HRESULT WINAPI GetBackBuffer(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	virtual HRESULT WINAPI CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
	virtual HRESULT WINAPI CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle);
	virtual HRESULT WINAPI CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
	virtual HRESULT WINAPI CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
	virtual HRESULT WINAPI CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle);
	virtual HRESULT WINAPI CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	virtual HRESULT WINAPI CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	virtual HRESULT WINAPI UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint);
	virtual HRESULT WINAPI UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture);
	virtual HRESULT WINAPI GetRenderTargetData(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface);
	virtual HRESULT WINAPI GetFrontBufferData(UINT iSwapChain,IDirect3DSurface9* pDestSurface);
	virtual HRESULT WINAPI StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter);
	virtual HRESULT WINAPI ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color);
	virtual HRESULT WINAPI CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	virtual HRESULT WINAPI SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
	virtual HRESULT WINAPI GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
	virtual HRESULT WINAPI SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);
	virtual HRESULT WINAPI GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface);
	virtual HRESULT WINAPI BeginScene();
	virtual HRESULT WINAPI EndScene();
	virtual HRESULT WINAPI Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	virtual HRESULT WINAPI SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	virtual HRESULT WINAPI MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	virtual HRESULT WINAPI SetViewport(CONST D3DVIEWPORT9* pViewport);
	virtual HRESULT WINAPI CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB);
	virtual HRESULT WINAPI BeginStateBlock();
	virtual HRESULT WINAPI EndStateBlock(IDirect3DStateBlock9** ppSB);
	virtual HRESULT WINAPI GetTexture(DWORD Stage,IDirect3DBaseTexture9** ppTexture);	
	virtual HRESULT WINAPI SetTexture(DWORD Stage,IDirect3DBaseTexture9* pTexture);
	virtual HRESULT WINAPI DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	virtual HRESULT WINAPI DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
	virtual HRESULT WINAPI DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT WINAPI DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	virtual HRESULT WINAPI ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags);
	virtual HRESULT WINAPI CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl);
	virtual HRESULT WINAPI SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl);
	virtual HRESULT WINAPI GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl);
	virtual HRESULT WINAPI CreateVertexShader(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
	virtual HRESULT WINAPI SetVertexShader(IDirect3DVertexShader9* pShader);
	virtual HRESULT WINAPI GetVertexShader(IDirect3DVertexShader9** ppShader);
	virtual HRESULT WINAPI SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	virtual HRESULT WINAPI GetVertexShaderConstantF(UINT StartRegister,float* pData, UINT Vector4fCount);
	virtual HRESULT WINAPI SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
	virtual HRESULT WINAPI GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride);
	virtual HRESULT WINAPI SetIndices(IDirect3DIndexBuffer9* pIndexData);
	virtual HRESULT WINAPI GetIndices(IDirect3DIndexBuffer9** ppIndexData);
	virtual HRESULT WINAPI CreatePixelShader(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);
	virtual HRESULT WINAPI SetPixelShader(IDirect3DPixelShader9* pShader);
	virtual HRESULT WINAPI GetPixelShader(IDirect3DPixelShader9** ppShader);
	virtual HRESULT WINAPI SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	virtual HRESULT WINAPI GetPixelShaderConstantF(UINT StartRegister,float* pData, UINT Vector4fCount);
	virtual HRESULT WINAPI DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
	virtual HRESULT WINAPI DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
	virtual HRESULT WINAPI CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery);

	virtual HRESULT WINAPI PresentEx(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion, DWORD flags);
	virtual HRESULT WINAPI ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX      *pFullscreenDisplayMode);

	/*** D3DProxyDevice public methods ***/
	HRESULT WINAPI CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle, bool isBackBufferOfPrimarySwapChain);
	virtual void   Init();

	/**
	* The chosen stereo renderer.
	* @see StereoView
	**/
	StereoView* stereoView;
	
protected:
	//Calculate FPS, called every Present
	float fps;
	void CalcFPS();


	/********************************/
	
	DX9ExDebugWindow		*m_pDX9ExDebugWindow;


	/**
	* Struct commands device behavior.
	***/
	struct DeviceBehavior
	{
		/**
		* Determines when to render the VP menu for that game profile.
		***/
		enum WhenToDo
		{
			BEFORE_COMPOSITING,
			AFTER_COMPOSITING,
			BEGIN_SCENE,
			END_SCENE,
		};


	} m_deviceBehavior;

	void HandleLandmarkMoment(DeviceBehavior::WhenToDo when);

private:
	LPD3DXFONT       m_D3D9Font;
	DWORD			presentFlag;
	D3DPRESENT_PARAMETERS presentationParameters;

};

#endif //D3DPROXYDEVICEEX_H_INCLUDED