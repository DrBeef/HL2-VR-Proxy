/********************************************************************
Vireio Perception: Open-Source Stereoscopic 3D Driver
Copyright (C) 2012 Andres Hernandez

File <StereoView.cpp> and
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

#include "StereoView.h"

using namespace vireio;

/**
* Tiny debug helper.
* Outputs debug info if object reference counter is not zero when release.
***/
inline void releaseCheck(char* object, int newRefCount)
{
//#ifdef _DEBUG
	if (newRefCount > 0) {
		debugf("Error: %s count = %d\n", object, newRefCount);
	}
//#endif
}

/**
* Constructor.
* Sets game configuration data. Sets all member pointers to NULL to prevent uninitialized objects being used.
***/ 
StereoView::StereoView()	
{
	SHOW_CALL("StereoView::StereoView()\n");

	initialized = false;

}

/**
* Empty destructor.
***/
StereoView::~StereoView()
{
	SHOW_CALL("StereoView::~StereoView()");
}


void StereoView::PostReset() {}
