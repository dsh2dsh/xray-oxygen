////////////////////////////////////
#include <d3d9.h>
#include <d3d11.h>
////////////////////////////////////
#pragma comment(lib, "xrCore.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#include "../xrCore/xrCore.h"
#include <stdio.h>
#include <iostream>
///////////////////////////////////////////////
#define DLL_API __declspec(dllimport)
#define _RELEASE(x) { if(x) { (x)->Release(); (x)=NULL; } }
DLL_API xr_vector<xr_token> vid_quality_token;
/////////////////////////////////////////////////////
bool SupportsDX11Rendering() 
{
	// Для проверки потдержки дх11 ненадо создавать окно + дх10
    D3D_FEATURE_LEVEL pFeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL FeatureLevel;

	ID3D11Device* pd3dDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;

	HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, pFeatureLevels, 1, D3D11_SDK_VERSION, &pd3dDevice, &FeatureLevel, &pContext);

	if (FAILED(hr)) Msg("* D3D11: device creation failed with hr=0x%08x", hr);

	_RELEASE(pd3dDevice);
	_RELEASE(pContext);

	return SUCCEEDED(hr);
}

void CreateRendererList()
{

	if (!vid_quality_token.empty()) return;

	xr_vector<xr_token> modes;

	SetErrorMode(0);

	if (SupportsDX11Rendering())
	{
		modes.push_back(xr_token("renderer_r4", 0));
        modes.emplace_back(xr_token("renderer_r4_extended", 1));
	}
	
	modes.emplace_back(xr_token(nullptr, -1));

	Msg("Available render modes[%d]:", modes.size());

	for (xr_token& mode : modes)
	{
		if (mode.name)
		{
			Log(mode.name);
		}
	}

	vid_quality_token = std::move(modes);
}