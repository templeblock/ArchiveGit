// Source file: effects.c

BOOL DoEffect( LPIMAGE lpImage, LPEFFECT_PARMS lpParms );
BOOL DoEffectsDlg(LPIMAGE lpImage, LPFRAME lpFrame, LPMASK lpMaskParam, LPRECT lpEffectRect, LPRECT lpEditRect);
BOOL WINPROC DlgEffectsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
int DLL_Message( int idStr, ... );
int DLL_AstralClockCursor( int part, int whole );
LPFRAME DLL_frame_open( WORD Depth, WORD x_size, WORD y_size, WORD resolution );
void DLL_frame_close( LPFRAME lpFrame );
LPTR DLL_frame_ptr( LPFRAME lpFrame, int Depth, int x, int y, int bModify );
int DLL_frame_write( LPFRAME lpFrame, int x, int y, int dx, LPTR pixels, int count );
LPTR DLL_frame_set( LPTR lpFrame );

