// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "hwcaps.h"

#ifndef _MAYA_EXPORT
#include "stats_manager.h"
#endif

class  CHW
	:	public pureAppActivate, 
		public pureAppDeactivate
{
//	Functions section
public:
	CHW();
	~CHW();

	void					CreateD3D				();
	void					DestroyD3D				();
	void					CreateDevice			(HWND hw, bool move_window);

	void					DestroyDevice			();

	void					Reset					(HWND hw);

	void					SelectResolution		(u32 &dwWidth, u32 &dwHeight, BOOL bWindowed);
	D3DFORMAT				SelectFmtTarget			();
	D3DFORMAT				SelectFmtDepthStencil	(D3DFORMAT);
	void					ResizeWindowProc		(WORD h, WORD w);
	u32						SelectPresentInterval	();
	u32						SelectGPU				();
	u32						SelectRefresh			(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt);

	void					FillVidModeList			();
	void					FreeVidModeList			();

	void	                Validate(void)	{};

//	Variables section
public:
	IDXGIAdapter1*			m_pAdapter;	//	pD3D equivalent
	ID3D11Device*			pDevice;	//	combine with DX9 pDevice via typedef
	ID3D11DeviceContext*    pContext;	//	combine with DX9 pDevice via typedef
	IDXGISwapChain*         m_pSwapChain;
	ID3D11RenderTargetView*	pBaseRT;	//	combine with DX9 pBaseRT via typedef
	ID3D11DepthStencilView*	pBaseZB;

	CHWCaps					Caps;

	D3D_DRIVER_TYPE			m_DriverType;	//	DevT equivalent
	DXGI_SWAP_CHAIN_DESC	m_ChainDesc;	//	DevPP equivalent
	bool					m_bUsePerfhud;
	D3D_FEATURE_LEVEL		FeatureLevel;
	bool					IsFormatSupported		(DXGI_FORMAT fmt);

#ifndef _MAYA_EXPORT
	stats_manager			stats_manager;
#endif

	void			UpdateViews();
	DXGI_RATIONAL	SelectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);

	virtual	void	OnAppActivate();
	virtual void	OnAppDeactivate();

private:
	bool					m_move_window;
};

extern ECORE_API CHW		HW;
