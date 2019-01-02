#include "stdafx.h"
#ifdef RAW_INPUT_USE
#include "xr_input.h"
#endif
#ifdef XINPUT_USE
#include <XInput.h>
#pragma comment(lib, "xinput.lib")
#endif
extern ENGINE_API BOOL g_bRendering;

bool CRenderDevice::on_message	(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &result)
{
#ifdef RAW_INPUT_USE
	CInput::DataInput = lParam;
#endif
#ifdef XINPUT_USE
	if (WM_ACTIVATEAPP == uMsg) XInputEnable((BOOL)wParam); // Controller Input Wrapper
#endif
    
    // call my new window code
    if (MainWndProc(uMsg, wParam, lParam, result)) return result;
	return false;
}
