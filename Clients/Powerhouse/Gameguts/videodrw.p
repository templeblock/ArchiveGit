typedef void (FAR *VDP_SETPROC)( HWND hWindow, BOOL bOn );
typedef void (FAR *VDP_DRAWPROC)( HWND hWindow );

// Source file: videodrw.cpp
void Video_DrawProcInstall( HWND hWindow, VDP_SETPROC lpNewSetProc, VDP_DRAWPROC lpNewDrawProc, UINT wTimeDelay );
BOOL Video_DrawProcDraw( HWND hWindow, ITEMID idTimer );
void VDP_HotspotSet( HWND hWindow, BOOL bOn );
void VDP_HotspotDrawOne( long lHotspot, long lFlags, int iUnitWidth, int iUnitHeight );
void VDP_HotspotDraw( HWND hWindow );
void VDP_GridSet( HWND hWindow, BOOL bOn );
void VDP_GridDraw( HWND hWindow );
