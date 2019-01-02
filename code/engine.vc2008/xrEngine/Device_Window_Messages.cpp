#include "stdafx.h"

void CRenderDevice::MessageAltEnter( ) 
{
    
}

bool CRenderDevice::MessageClose( ) 
{
    if (editor()) return false; // в редакторе не можем выйти
    return true;
}

void CRenderDevice::MessageSetCursor()
{
    if (editor()) return;
    //.. т.к. в игре свой курсор врятли нужно устанавливать какойто еще
}

extern int ps_always_active; // чей-то костыль чтобы всегда отображать курсор в консоли "always_active"

void CRenderDevice::MessageActive()
{
    if (editor() || ps_always_active)
    {
        ShowCursor(TRUE);
    } else {
        ShowCursor(FALSE);
    }

    b_is_Active = TRUE;

    seqAppActivate.Process(rp_AppActivate);
}

void CRenderDevice::MessageInactive()
{ 
    ShowCursor(TRUE);
    
    b_is_Active = FALSE;

    if (ps_always_active)  b_is_Active = TRUE;

    seqAppDeactivate.Process(rp_AppDeactivate);
}

void CRenderDevice::MessageMove( ) 
{

}

void CRenderDevice::MessageResizeBegin( ) 
{

}

void CRenderDevice::MessageResize( ) 
{

}

void CRenderDevice::MessageResizeEnd( ) 
{
    // ресайзем девайс после ресайза окна, т.к. в окси много ресайзить, потом оптимизирую
    ///Device.ResizeProc(MainWindowHeight, MainWindowWidth);  // todo боже поменяйть местами h w
}

void CRenderDevice::MessageHide( ) 
{

}

void CRenderDevice::MessageShow( ) 
{

}
