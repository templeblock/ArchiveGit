// Source file: maskio.c
BOOL ShapeMaskImage(LPIMAGE lpImage, LPSHAPEMASK_PARMS lpParms);
BOOL FreeMaskImage(LPIMAGE lpImage, LPFREEMASK_PARMS lpParms);
BOOL PointEditImage(LPIMAGE lpImage, LPPOINTEDIT_PARMS lpParms);
BOOL PointEditImageEx(LPIMAGE lpImage, LPPOINTEDIT_PARMS lpParms, BOOL bInitUndo);
BOOL ApplyMaskSelObj(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL RemoveMaskImage(LPIMAGE lpImage, LPCMD_PARMS lpParms);
BOOL WINPROC DlgFeatherProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgFeatherObjProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL FeatherAlphaSelObj(LPIMAGE lpImage, LPFEATHERALPHA_PARMS lpParms);
BOOL FeatherMaskImage(LPIMAGE lpImage, LPFEATHERALPHA_PARMS lpParms);
BOOL IsDisplayMasked( int x, int y );
LPFRAME ClearMaskRect( LPIMAGE lpImage, LPRECT lpRect, BYTE value, BOOL bGetBackup);
void RestoreMaskRect(LPRECT lpRect, LPFRAME lpRestoreFrame);
void SetNewMaskEllipse(LPRECT lpRect, int dirty, int iModes);
BOOL InvertMaskImage(LPIMAGE lpImage, LPCMD_PARMS lpParms);

BOOL WINPROC DlgMaskSmoothProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void CompressPoly(int MinEdge, int Tolerance, LPCPolyList lpPolyList);
void CompressPoly(int MinEdge, int Tolerance, LPCPoly lpPoly);

void RemoveMask( void );
void RemoveMaskEx( LPIMAGE lpImage, int dirty );
BOOL WriteMask(LPTSTR szClipName, LPMASK lpMask, BOOL fEntireMask);
LPMASK ReadMask(LPIMAGE lpImage, LPTSTR szClipName, int iMaxWidth, int iMaxHeight);
BOOL mask_active(LPFRAME lpFrame);
BOOL mask_rect(LPFRAME lpFrame, LPRECT lpRect);
void mask_link( LPFRAME lpFrame, LPMASK lpMask );
void mask_unlink( LPFRAME lpFrame, LPMASK lpMask );
void mask_close( LPMASK lpMask );

BOOL RemoveHolesImage(LPIMAGE lpImage, LPREMOVE_HOLES_PARMS lpParms);
BOOL MaskSmoothImage(LPIMAGE lpImage, LPMASK_SMOOTH_PARMS lpParms);


