
#include "SharedSurface\SurfaceQueue.h"

class DX9ExDebugWindow
{
public:
	DX9ExDebugWindow() {}
	void init(IDirect3DDevice9Ex *pDevice);
	void shutDown(void);
	void copySurface(IDirect3DSurface9* pDirect3DSurface9);
	void present();

private:
	ISurfaceQueue*			g_9toDebugQueue;
	ISurfaceQueue*			g_Debugto9Queue;

	ISurfaceConsumer*		g_9Consumer;
	ISurfaceProducer*		g_9Producer;
	ISurfaceConsumer*		g_DebugConsumer;
	ISurfaceProducer*		g_DebugProducer;

	IDirect3DDevice9Ex*		g_pGameDevice;

	HWND					g_hWnd;
	IDirect3DDevice9Ex*		g_pD3DExDevice;
};
