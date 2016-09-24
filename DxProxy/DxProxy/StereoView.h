/********************************************************************
Vireio Perception: Open-Source Stereoscopic 3D Driver
Copyright (C) 2012 Andres Hernandez

File <StereoView.h> and
Class <StereoView> :
Copyright (C) 2012 Andres Hernandez

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

#ifndef STEREOVIEW_H_INCLUDED
#define STEREOVIEW_H_INCLUDED

#include "D3DProxyDeviceEx.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <string.h>
#include <assert.h>

/**
* Stereo-view render class.
* Basic class to render in stereo.
*/
class StereoView
{
public:
	StereoView();
	virtual ~StereoView();

	/*** StereoView public methods ***/
	virtual void Init(IDirect3DDevice9Ex* pActualDevice) = 0;
	virtual void PrePresentEx(IDirect3DSurface9* pDirect3DSurface9, D3DProxyDeviceEx* pProxyDevice) = 0;
	virtual void PostPresentEx(IDirect3DSurface9* pDirect3DSurface9, D3DProxyDeviceEx* pProxyDevice) = 0;

	virtual void PostReset();

	/**
	* True if class is initialized. Needed since initialization is not done in constructor.
	***/
	bool initialized;


protected:

};

/**
* Declaration of texture vertex used for full screen render.
***/
const DWORD D3DFVF_TEXVERTEX = D3DFVF_XYZRHW | D3DFVF_TEX1;
/**
* Texture vertex used for full screen render.
***/
struct TEXVERTEX
{
	float x;
	float y;
	float z;
	float rhw;
	float u;
	float v;
};

#endif