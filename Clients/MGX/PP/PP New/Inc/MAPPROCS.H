
extern "C"
{
#include "dyndlg.h"
}

// Source file: mapprocs.c
typedef struct _mapvalues
{
	BOOL 	bMap_Visual;		// Map dialog in visual or numeric mode?
	POINT 	ptNumeric[MAPPOINTS];	// for Map dialog in numeric mode
	MAP*	pMap;
	WORD	wChannel;
	BOOL	fModified;
	DLGSECTION	MapSec;
} MAPVALUES, FAR *LPMAPVALUES;

BOOL WINPROC DlgMapProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgContBritVisualProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgBalanceVisualProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgContBritProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgBalanceProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPosterizeProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgHueProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgHueShiftProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgGammaProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgMapLoadProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgMapSaveProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
MAP *ResetMapChannel( HWND hDlg, WORD wChannel );
MAP *ResetMapChannel( HWND hDlg, LPMAPVALUES lpValues );
MAP *ActivateChannel( WORD wChannel );

VOID DisplayInOutValues(HWND hDlg, int x, int y);
void HandleMapInit(HWND hDlg, LPMAPVALUES lpValues, HWND hActiveWnd, FRMDATATYPE type, BOOL fUseProbe);
void HandleMapEnd(HWND hDlg, HWND hActiveWnd);
BOOL HandleMapCommand(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMap(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapIn(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapOut(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapOptions(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapCurves(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapTool(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapGamma(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleResetAll(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleReset(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapShift(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapPreview(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapChannels(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapGradient(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
BOOL HandleMapProbe(HWND hDlg, ITEMID id, UINT codeNotify, LPMAPVALUES lpValues, HWND hActiveWnd);
void HandleMapLoad(HWND hDlg, LPMAPVALUES lpValues, HWND hActiveWnd);
void HandleMapDestroy(HWND hDlg);
void HandleMapMouseMove(HWND hDlg, int x, int y, UINT keyFlags,	LPMAPVALUES lpValues);
void HandleMapLButtonUp(HWND hDlg, int x, int y, UINT keyFlags, LPMAPVALUES lpValues);
VOID UpdateCurrentMap(HWND hDlg, LPMAPVALUES lpValues);
void MapSetupPanes(HWND hDlg, LPMAPVALUES lpValues, BOOL fUseProbe);
void MapActivatePane( HWND hDlg, ITEMID idTool, LPMAPVALUES lpValues );
void MapSetGradients(HWND hDlg, BOOL bIntensity, WORD wChannel);

VOID AutoPreview_Button(HWND hPreviewWnd, LPMAP lpMap);
VOID AutoPreview(LPMAP lpMap, BOOL bAuto, BOOL bDoNow, HWND hDlg, HWND hPreviewWnd);
VOID UnAutoPreview(HWND hDlg, HWND hPreviewWnd);
VOID AutoPreview_OnDestroy(HWND hDlg);
BOOL AutoPreview_OnTimer(HWND hDlg, UINT id);

VOID ChannelsCombo_Setup(HWND hDlg, int idControl, FRMDATATYPE type, WORD wChannel);
int ChannelsCombo_Handle(HWND hDlg, int idControl, UINT codeNotify);
BOOL ChannelsCombo_IsEnabled(HWND hDlg, WORD wChannel);

BOOL Probe_Begin(HWND hDlg, UINT codeNotify);
BOOL Probe_IsProbing();
BOOL Probe_OnMouseMove(HWND hDlg, int x, int y, UINT keyFlags, LPCOLORINFO lpProbeColor);
BOOL Probe_OnLButtonUp(HWND hDlg, int x, int y, UINT keyFlags, LPCOLORINFO lpProbeColor);
void Probe_End(HWND hDlg);


