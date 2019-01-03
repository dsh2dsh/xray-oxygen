#include "stdafx.h"
#include "../xrPlay/resource.h"

#define     WINDOW_RESIZE_TIMER 2

bool  CRenderDevice::MainWndProc	(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &result)
{
    // нажатие на крестик или alt+f4
    if (WM_CLOSE == uMsg) 
    { 
        if (MessageClose( )) 
        { 
            result = 1; 
            return true; 
        }
        return false; 
    }

    // Set min size
    if (WM_GETMINMAXINFO == uMsg) 
    {
        MINMAXINFO* pMinMaxInfo = ( MINMAXINFO* )lParam;
        pMinMaxInfo->ptMinTrackSize.x = MainWindowMinWidth;
        pMinMaxInfo->ptMinTrackSize.y = MainWindowMinHeight;
        return false;
    }

    // Disable background for dx
    if (WM_ERASEBKGND == uMsg) 
    { 
        result = 1; 
        return true; 
    }

    // Move
    if (WM_MOVE == uMsg) 
    { 
        UpdateWindowPosMonitor( );
        UpdateWindowPosAndSize( );
        result = 0; 
        return true; 
    }

    // Resize
    if (WM_SIZE == uMsg) 
    { 
        switch ( wParam ) 
        {
            case  SIZE_MINIMIZED: 
            {
                if ( bFlagShow == true ) MessageHide( );

                bFlagShow       = false;
                bFlagMinimize   = true;
                bFlagMaximize   = false;

                SavePosX        = MainWindowPosX;
                SavePosY        = MainWindowPosY;
                SaveWidth       = MainWindowWidth;
                SaveHeight      = MainWindowHeight;
                SaveWidthWin    = WindowRect.right - WindowRect.left;
                SaveHeightWin   = WindowRect.bottom - WindowRect.top;
                break;
            }
            case  SIZE_MAXIMIZED: 
            {
                if ( bFlagShow == false )  MessageShow( );

                bFlagShow       = true;
                bFlagMinimize   = false;
                bFlagMaximize   = true;

                UpdateWindowPosAndSize( );
                break;
            }
            case  SIZE_RESTORED: 
            {
                if ( bFlagMinimize ) 
                {
                    bFlagShow       = true;
                    bFlagMinimize   = false;
                    MessageShow( );
                    break;
                }

                if ( bFlagMaximize ) 
                {
                    bFlagShow       = true;
                    bFlagMaximize   = false;
                    UpdateWindowPosAndSize( );
                    break;
                }

                UpdateWindowPosAndSize( );

                SavePosX        = MainWindowPosX;
                SavePosY        = MainWindowPosY;
                SaveWidth       = MainWindowWidth;
                SaveHeight      = MainWindowHeight;
                SaveWidthWin    = WindowRect.right - WindowRect.left;
                SaveHeightWin   = WindowRect.bottom - WindowRect.top;
                break;
            }
            default:
                UpdateWindowPosAndSize( );
        }

        result = 0; 
        return true; 
    }

    // Resize & move begin end
    if (WM_ENTERSIZEMOVE == uMsg) 
    { 
        SetTimer( MainWindowHandle, WINDOW_RESIZE_TIMER, USER_TIMER_MINIMUM, NULL );
        bFlagResizeing = true;
        result = 0; 
        return true; 
    }

    if (WM_EXITSIZEMOVE == uMsg) 
    { 
        KillTimer( MainWindowHandle, WINDOW_RESIZE_TIMER );
        bFlagResizeing = false;
        result = 0; 
        return true; 
    }

    if (WM_TIMER == uMsg && wParam == WINDOW_RESIZE_TIMER) 
    { 
        RedrawWindow( MainWindowHandle, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT );
        result = 0; 
        return true; 
    }

    // Activate
    if ( WM_ACTIVATE == uMsg) 
    {
        if ( LOWORD( wParam ) != WA_INACTIVE ) 
        {
            bFlagActive = true;
            MessageActive();
        } else {
            bFlagActive = false;
            MessageInactive();
        }
        return false;
    }

    //Set cursor
    if ( WM_SETCURSOR == uMsg ) 
    {
        if ( LOWORD( lParam ) == HTCLIENT ) 
        {
            MessageSetCursor();
            bFlagPrevMouseOnClient = true;
            result = 1; 
            return true;
        }
        bFlagPrevMouseOnClient = false;
        return false;
    }

    // Alt+Enter
    if ( WM_SYSKEYDOWN == uMsg) 
    {
        if (( s32 )wParam == VK_RETURN) 
        { 
            MessageAltEnter( ); 
        }
        result = 0;
        return true;
    }

    return false;
}

void CRenderDevice::UpdateWindowPosMonitor( ) 
{
    MonitorHandle = MonitorFromWindow( MainWindowHandle, MONITOR_DEFAULTTONEAREST );
    if ( MonitorHandle ) 
    {
        MONITORINFOEXW mi;
        mi.cbSize = sizeof( mi );
        if ( GetMonitorInfoW( MonitorHandle, &mi ) ) 
        {
            MonitorPosX     = mi.rcMonitor.left;
            MonitorPosY     = mi.rcMonitor.top;
            MonitorWidth    = mi.rcMonitor.right - mi.rcMonitor.left;
            MonitorHeight   = mi.rcMonitor.bottom - mi.rcMonitor.top;

            if ( bMonitorSizeShowMenubar ) 
            {
                MonitorPosX = mi.rcWork.left;
                MonitorPosY = mi.rcWork.top;
                MonitorWidth = mi.rcWork.right - mi.rcWork.left;
                MonitorHeight = mi.rcWork.bottom - mi.rcWork.top;
            } else {
                MonitorPosX = mi.rcMonitor.left;
                MonitorPosY = mi.rcMonitor.top;
                MonitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
                MonitorHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
            }
        }
    } else { // когда монитор один могу тбыть глюки и невозможно получить размер. альтернатива
        HDC hDC = GetDC( GetDesktopWindow( ) );
        MonitorPosX = 0;
        MonitorPosY = 0;
        MonitorWidth = GetDeviceCaps( hDC, HORZRES );
        MonitorHeight = GetDeviceCaps( hDC, VERTRES );
        ReleaseDC( GetDesktopWindow( ), hDC );
    }
}

void CRenderDevice::UpdateWindowPosAndSize( ) 
{
    bool bResize = false;
    bool bMove = false;

    if (!GetWindowRect( MainWindowHandle, &WindowRect )) return;
    if (!GetClientRect( MainWindowHandle, &ClientRect )) return;

    s32 X       = WindowRect.left;
    s32 Y       = WindowRect.top;
    s32 Width   = ClientRect.right - ClientRect.left;
    s32 Height  = ClientRect.bottom - ClientRect.top;

    if ( Width > 0 && Height > 0 && ( (s32)MainWindowWidth != Width || (s32)MainWindowHeight != Height ) ) 
    {
        MainWindowWidth = (u32)Width;
        MainWindowHeight = (u32)Height;
        bResize = true;
    }

    if ( ( MainWindowPosX != X ) || ( MainWindowPosY != Y ) ) 
    {
        MainWindowPosX = X;
        MainWindowPosY = Y;
        bMove = true;
    }

    if ( bResize ) 
    {
        MessageResize( );
    }

    if ( bMove ) 
    {
        MessageMove( );
    }
}

LRESULT CALLBACK CRenderDevice::GlobalMainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{
    CRenderDevice* pRenderDevice = ( CRenderDevice* )GetWindowLongPtrW( hWnd, GWLP_USERDATA );
    if (pRenderDevice) 
    { 
        // message funct
        LRESULT	result = 0;
        if (pRenderDevice->MainWndProc(uMsg, wParam, lParam, result)) return result;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool CRenderDevice::CreateEngineWindow( HWND hWndParent, s32 Width, s32 Height  ) 
{
    MainWindowWidth     = Width;
    MainWindowHeight    = Height;
    MainWindowParent    = hWndParent;

    // окно не должно иметь нулевой размер!
    if (MainWindowWidth <= 0) MainWindowWidth = 1;
    if (MainWindowHeight <= 0) MainWindowHeight = 1;

    MainWindowClass = { 0 };
    MainWindowClass.cbSize          = sizeof(MainWindowClass);
    MainWindowClass.style           = CS_HREDRAW | CS_VREDRAW;
    MainWindowClass.lpfnWndProc     = GlobalMainWndProc;
    MainWindowClass.hInstance       = GetModuleHandleW(0);
    MainWindowClass.hIcon           = LoadIconA(MainWindowClass.hInstance, MAKEINTRESOURCE(IDI_ICON1));
    MainWindowClass.hIconSm         = MainWindowClass.hIcon;
    MainWindowClass.hCursor         = LoadCursorA(NULL, IDC_ARROW); 
    MainWindowClass.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
    MainWindowClass.lpszMenuName    = nullptr;
    MainWindowClass.lpszClassName   = L"_XRAY_1_7_F_2";

    if ( !RegisterClassExW( &MainWindowClass ) ) 
    {
        FATAL("Window class create failed");
        return false;
    }

    MainWindowStyleEx = 0; // WS_EX_TOPMOST не хорошо использовать этот флаг
    MainWindowStyle = 0;

    s32 WindowRectWidth = MainWindowWidth;
    s32 WindowRectHeight = MainWindowHeight;

	if (MainWindowParent)
	{
		MainWindowStyle |= WS_CHILD;
		if (IsDebuggerPresent()) MainWindowStyleEx = 0; // todo удалить
	}
	else 
	{
        MainWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX ;
        MainWindowStyle |= WS_THICKFRAME | WS_MAXIMIZEBOX; // support window resize

                                                           // calculate window size from client size
        RECT  TempRect{ 0, 0, (s32)MainWindowWidth, (s32)MainWindowHeight };
        AdjustWindowRectEx( &TempRect, MainWindowStyle, FALSE, MainWindowStyleEx );
        MainWindowWidth = TempRect.right - TempRect.left;
        MainWindowHeight = TempRect.bottom - TempRect.top;

        RECT  TempRectMin{ 0, 0, 1, 1 };
        AdjustWindowRectEx( &TempRectMin, MainWindowStyle, FALSE, MainWindowStyleEx );
        MainWindowMinWidth  = TempRect.right - TempRect.left;
        MainWindowMinHeight = TempRect.bottom - TempRect.top;
    }

    MainWindowHandle = CreateWindowExW(
        MainWindowStyleEx, 
        MainWindowClass.lpszClassName, 
        L"X-Ray Oxygen", 
        MainWindowStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WindowRectWidth, WindowRectHeight, 
        MainWindowParent, 0, 
        MainWindowClass.hInstance, nullptr);

    if ( !MainWindowHandle ) 
    {
        FATAL("Window create failed");
        return false;
    }

    SetWindowLongPtrA( MainWindowHandle, GWLP_USERDATA, ( LONG_PTR )this );

    Log("Create game window");

    // todo удалить всё нах. костыли для потдержки старого кода
    m_hWnd          = MainWindowHandle;
    gGameWindow     = MainWindowHandle;
    
    return true;
}

void CRenderDevice::DestroyEngineWindow( ) 
{
    if (MainWindowHandle) 
    {
        DestroyWindow( MainWindowHandle );
        UnregisterClassW( MainWindowClass.lpszClassName, MainWindowClass.hInstance );
        MainWindowHandle = nullptr;
        MainWindowClass = {0};
        Log("Destroy game window");
    }

    // todo удалить всё нах. костыли для потдержки старого кода
    m_hWnd = nullptr;
    gGameWindow= nullptr;
}
