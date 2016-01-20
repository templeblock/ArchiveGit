// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include "pp.h"
#include "ppafx.h"
#include "svrview.h"
#include "id.h"

/************************************************************************/
BOOL IsDoc( HWND doc )
/************************************************************************/
{
    if (!doc || !::IsWindow(doc))  
        return (FALSE);
    
    CWnd *pWnd = CWnd::FromHandle(doc);
    ASSERT(pWnd);

    return (pWnd->IsKindOf(RUNTIME_CLASS(CServerView)) || 
        (::Window.fHasZoom && (hZoomWindow == doc)));        
}


