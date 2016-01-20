/* (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
	      MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

				dlgsmap.h

*****************************************************************************
*/

#ifndef _DLGSMAP_H_
#define _DLGSMAP_H_

/* ****************************** Public Data ***************************** */

/* ****************************** Constants ******************************* */

#define VISUAL_BEFORE   1000
#define PRLINES 20

/* ****************************** Typedefs ******************************** */

//UN-COMMENT THIS SECTION WHEN MAPPROCS IS REMOVED!!!
typedef struct _mapvalues
{
        BOOL    bMap_Visual;            // Map dialog in visual or numeric mode?
        POINT   ptNumeric[MAPPOINTS];   // for Map dialog in numeric mode
        MAP*    pMap;
        WORD    wChannel;
        BOOL    fModified;
        DLGSECTION      MapSec;
} MAPVALUES, FAR *LPMAPVALUES;

/* ****************************** Classes ********************************* */

class CVisual;
class CContrastBrightnessVisual;
class CContrastBrightnessVisualDlg;
class CColorBalanceJoystickDlg;
class CColorBalanceVisualDlg;
class CContrastBrightnessJoystickDlg;
class CColorBalanceVisual;
class CContrastBrightnessVisualDlg;
class CHueMapDlg;
class CHueShiftDlg;
class CModifyColorMapsDlg;
class CMapLoadDlg;
class CMapSaveDlg;
class CModifyOptionsDlg;
class CPPVisualMapDlg;
class CPPMapDlg;
class CPPMapExDlg;
class CPosterizeDlg;
class CThresholdDlg;
class CPPVisualMapDlg;
class CMapPreviewHook;

//class CMapPreviewHook : public CHookHandler
//{
//public:
//    CMapPreviewHook() : CHookHandler() {};
//    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
//            LPTR lpDst, LPTR lpMsk, LPMASK lpMask); 
//    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect) { /*no implementation*/ };
//};

class CVisual
{
 public:
	virtual void DrawVisual(HDC, LPRECT, int, int)=0;
	virtual BOOL MoveVisual(int, int)=0;
	CVisual(){}
	~CVisual(){}
	LPIMAGE m_lpImage;              
	LPFRAME m_lpOrgFrame;           
	LPTR m_lpUser;                  
	int m_nStep;                            
	WORD m_wMoveType;                       
	LPFRAME m_lpBasisFrame; 
	LPFRAME m_lpWorkFrame;  
};

class CContrastBrightnessVisual : public CVisual
{
public:
	virtual void DrawVisual(HDC, LPRECT, int, int);
	virtual BOOL MoveVisual(int, int);
	CContrastBrightnessVisualDlg* m_pDlg;
	CContrastBrightnessVisual(){}
	void SetDlgPtr(CContrastBrightnessVisualDlg* pDlg) {m_pDlg = pDlg;}
	~CContrastBrightnessVisual(){}
};


class CColorBalanceVisual : public CVisual
{
public:
	virtual void DrawVisual(HDC, LPRECT, int, int);
	virtual BOOL MoveVisual(int, int);
	CColorBalanceVisualDlg* m_pDlg;
	CColorBalanceVisual(){};
	void SetDlgPtr(CColorBalanceVisualDlg* pDlg) {m_pDlg = pDlg;}
	~CColorBalanceVisual(){}
};
///////////////////////////////////////////////////

// for visaual contrast brigntness
typedef struct _cbstruct
{
	HWND hDlg;
	MAP *lpMap;
	WORD wChannel;
	WORD wBand;
	LPTR lpLuts;        // packed luts in order of id from IDC_VISUAL_00 to IDC_VISUAL_22
	LPTR lpBuffer;          // for display
}
CBSTRUCT, FAR *LPCBSTRUCT;

// for visual color balance
typedef struct _balancestruct
{
	HWND hDlg;
	LPTR lpLuts;            // packed lusts in order of id from IDC_VISUAL_00 to IDC_VISUAL_22
	LPTR lpBuffer;          // for display
	WORD wBand;
	BOOL bMaintainDensity;
}
BALANCESTRUCT, FAR *LPBALANCESTRUCT;

#define PACKEDLUTSIZE (7L*256L)
#define GRIDSIZE 64
#define PREVIEW_TIMER 1
#define PREVIEW_WAIT 500


class CPPMapDlg : public CPPViewModalDlg, public CHookHandler
{
public:
	CPPMapDlg(CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd = NULL );
	~CPPMapDlg() {SetImageHook(GetViewHwnd(),NULL);}
    HWND m_hActiveWnd;


	BOOL GetAutoPreviewPref() const {return m_bAutoPreview;}
	VOID SetAutoPreviewPref(BOOL bAutoPreview) {m_bAutoPreview = bAutoPreview;}
	BOOL GetPercentagesPref() const {return m_bPercentages;}
	VOID SetPercentagesPref(BOOL bPercentages) {m_bPercentages = bPercentages;}
	int GetCurveTypePref() const {return m_nCurveType;}
	VOID SetCurveTypePref(int nCurveType) {m_nCurveType = nCurveType;}
	BOOL GetGridPref() const {return m_bGrid;}
	VOID SetGridPref(BOOL bGrid) { m_bGrid = bGrid;}
	int GetPointsPref() const {return m_nPoints;}
	VOID SetPointsPref(int nPoints) { m_nPoints = nPoints;}
 
protected:
    BOOL m_bNumericEdit;        //New; Determines whether editing is visual or numeric
    BOOL m_bColorize;

    BOOL m_bProbing;
    int m_iBubbles;     
    UINT m_bPreviewTimer;
    int m_iCombo[8];            //filled by ChannelsCombo_Setup
    int m_nCombo;                       //corresponds to the number of channels for a given frame-type.
    WORD m_wBand;
    BOOL m_bIsInPreview;

    //Preferences for all dialogs derived from CPPMapDlg
    BOOL m_bAutoPreview;                
    int m_nPoints;                              
    BOOL m_bGrid;                               
    BOOL m_bPercentages;                
    BOOL m_bIntensity;                  
    BOOL m_bCurves;                             
    int m_nCurveType;

    MAPVALUES m_MapValues;
    MAP m_MasterMap;
    MAP m_RedMap;
    MAP m_GreenMap;
    MAP m_BlueMap;
    MAP m_BlackMap;
    MAP m_SatMap;
    MAP m_HueMap;
    RGBS m_PreviewRGBmap[256];
    BOOL m_bApplyMToK;
	LPTR m_lpHLut;
	LPTR m_lpSLut;
	LPTR m_lpMLut;
	LPTR m_lpRLut;
	LPTR m_lpGLut;
	LPTR m_lpBLut;
	LPTR m_lpKLut;
	BOOL m_fMyLuts;

    void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc, LPTR lpDst, LPTR lpMsk, LPMASK lpMask); 
    void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect) { /*no implementation*/ };
    void SetMapGlobals();
    BOOL HandleMapCommand(ITEMID id, UINT codeNotify);   
    void HandleMapLoad();
    BOOL HandleMapOptions(ITEMID id, UINT codeNotify);    
    BOOL HandleMapChannels(ITEMID id, UINT codeNotify);  
    BOOL HandleResetAll(); 
    BOOL HandleMapOut(ITEMID id, UINT codeNotify);   
    BOOL HandleMapIn(ITEMID id, UINT codeNotify); 
    BOOL HandleMap(ITEMID id, UINT codeNotify);          
    BOOL HandleMapPreview();
    BOOL HandleMapShift(ITEMID id);       
    void HandleMapInit(FRMDATATYPE type, BOOL fUseProbe);  
    BOOL HandleReset();   
    void HandleMapEnd();
    void HandleMapMouseMove(int x, int y, UINT keyFlags); 
    BOOL HandleMapGradient();  
    BOOL HandleMapProbe(UINT codeNotify);
    BOOL HandleMapTool(ITEMID id, UINT codeNotify); 
    void HandleMapLButtonUp(int x, int y, UINT keyFlags); 
    BOOL HandleMapCurves(ITEMID id); 
    void HandleMapDestroy();
    BOOL HandleMapGamma(ITEMID id, UINT codeNotify);  
    void MapSetupPanes(BOOL fUseProbe);  
    void MapActivatePane(ITEMID idTool);  
    VOID AutoPreview_OnDestroy();
    BOOL AutoPreview_OnTimer(UINT id);
    VOID Map_DlgSetNewMap(LPMAP lpMap);
    int Map_Get_Color(LPCOLORINFO lpColor); 
    VOID UpdateCurrentMap();  
    VOID UnAutoPreview();
    void Probe_End();
    BOOL Probe_OnMouseMove(int x, int y, UINT keyFlags, LPCOLORINFO lpProbeColor);
    BOOL Probe_OnLButtonUp(int x, int y, UINT keyFlags, LPCOLORINFO lpProbeColor);
    BOOL Probe_Begin(UINT codeNotify);
    MAP* ResetCBChannel(int mul=1);
    MAP* ResetMapChannel(WORD wChannel );
    MAP* ResetMapChannel();       
    void InitPreview();
    void DoPreview(BOOL f);
    VOID AutoPreview_Button(LPMAP lpMap);
    VOID AutoPreview(LPMAP lpMap, BOOL bAuto, BOOL bDoNow);
    void Hue_SetPatch(ITEMID idPatch, int hue, BOOL bSetText = FALSE);
    VOID ChannelsCombo_Setup(int idControl, FRMDATATYPE type); 
    int ChannelsCombo_Handle(int idControl, UINT codeNotify);
    void MapSetGradients(); 
    int order_points( LPPOINT lpPoints, int nPoints);
    void map_to_mapinfo(LPMAP lpMap, LPMAPINFO lpInfo);
    BOOL Previewing();
    void ResetAllMaps( BOOL bapplyMToK = NO );
    void ResetAllMapsEx( int type, BOOL bapplyMToK = NO );
    void ResetBandAllMaps(WORD wBand);
    MAP* ActivateChannel( WORD wChannel );
	VOID DisplayInOutValues(int x, int y);
	BOOL Probe_IsProbing();
	BOOL ChannelsCombo_IsEnabled(WORD wChannel);
	void UndoMapPreview();
	void UndoMapPreview(HWND hWnd) {UndoMapPreview();}
	void MapPreview();
	void MapPreview(HWND hWnd) {MapPreview();}
	void MapSetPackedLut(LPTR lpLuts);
	BOOL MapSetupLuts( LPTR HLut, LPTR SLut, LPTR MLut, LPTR RLut, LPTR GLut, LPTR BLut, LPTR KLut );
	LPTR MapGetPackedLut(LPTR lpDst,LPTR HLut,LPTR SLut,LPTR MLut,LPTR RLut,LPTR GLut,LPTR BLut, LPTR KLut);
	void Map8(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx);
	void Map24(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx);
	void Map32(LPTR lpDst, LPTR lpSrc, LPTR lpMsk, int dx);
	int  DisplayMappedImage(HDC hDC, LPFRAME lpFrame,LPRECT lpSourceRect, POINT destPoint, LPTR lpOutBuffer,int xDiva,int yDiva, LPVOID lpCmsXform);
	BOOL AnimateMap(LPMAP lpMap, BOOL fDoGamma);
	void MapGray( LPTR lpGray );
	void MapRGB( LPTR lpRed, LPTR lpGreen, LPTR lpBlue );
	void MapCMYK( LPTR lpC, LPTR lpM, LPTR lpY, LPTR lpK );
	void map_proc(int y, int left, int right, LPTR lpSrc, LPTR lpDst, int depth, LPVOID lpParms  );
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreDoModal(LPVOID lpVoid);
	virtual LPVOID PostDoModal();
	virtual void OnOK();
	virtual void OnCancel();

public:
	void _MapPreviewProcEx(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc, LPTR lpDst, LPTR lpMsk, LPMASK lpMask);

    friend class CMapSaveDlg;
    friend class CMapLoadDlg;
    DECLARE_DYNAMIC(CPPMapDlg)
};

class CPPMapExDlg : public CPPMapDlg	  //base class for CToneBalanceClg and CHistogramDlg
{
	
protected:
	LPDWORD m_lpGetHistos[5];
	virtual BOOL OnInitDialog();
	void set_gradient(int id, WORD wChannel);
	LPDWORD  get_histo(LPDWORD FAR *lpHistos, WORD wChannel, LPINT lpbReverse, LPINT lpNum = NULL);
	VOID free_histograms(LPDWORD FAR *lpHistos);
	long compute_yscale(LPDWORD lpHisto);
	BOOL ReadEngineSelObj(LPRECT lpEditRect);
	BOOL ReadProcessObj(LPOBJECT lpObject, BOOL fReadAlpha, LPRECT lpRect);
	void MapLineHisto( int y, int x1, int x2, LPTR lpSrc, FRMTYPEINFO TypeInfo );
public:
	CPPMapExDlg(CServerView* pView, UINT nIDTemplate, CWnd* pParent=NULL);
DECLARE_DYNCREATE(CPPMapExDlg)
};

class CPPVisualMapDlg: public CPPMapDlg
{
protected:
	CPPVisualMapDlg(CServerView *pView, UINT nIDTemplate, CWnd* pParentWnd = NULL );

    virtual CVisual* GetVisualPtr() = 0;

    void SetupFrames();
    BOOL Move2DDirectional(int x, int y);
    BOOL MoveCenterOnly(int x, int y);
    void UpdateVisual(int id);
    HWND GetExtraWindow(HWND hParent, int id, int n);
    BOOL MoveVisual(int x, int y);
    BOOL SetVisual(int x, int y);
    BOOL GetPosFromID(ITEMID id, LPINT lpx, LPINT lpy);
    int GetIDFromPos(int x, int y);
    
    BOOL OnCommandEx(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnDestroy();
    
public:
    //virtual void _DrawVisualEx(HDC hDC,LPRECT lpRect, int x,int y) = 0;
    //virtual BOOL _MoveVisualEx(int x, int y) = 0;
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CPPVisualMapDlg)
};
	


///////////////Preference Strings; these will eventually be located elsewhere////////////////

#define MODIFYCOLORMAPS_AUTOPREVIEW                             _T("ModifyColorMaps_AutoPreview")
#define MODIFYCOLORMAPS_POINTS                                  _T("ModifyColorMaps_Points")
#define MODIFYCOLORMAPS_GRID                                    _T("ModifyColorMaps_Grid")
#define MODIFYCOLORMAPS_PERCENTAGES                             _T("ModifyColorMaps_Percentages")
#define MODIFYCOLORMAPS_INTENSITY                               _T("ModifyColorMaps_Intensity")
#define MODIFYCOLORMAPS_CURVES                                  _T("ModifyColorMaps_Curves")
#define MODIFYCOLORMAPS_CURVETYPE                               _T("ModifyColorMaps_CurveType")
#define MODIFYCOLORMAPS_NUMERICEDIT                             _T("ModifyColorMaps_NumericEdit")
#define CONTRASTBRIGHTNESSJOYSTICK_AUTOPREVIEW  				_T("ContrastBrightnessJoystick_AutoPreview")
#define COLORBALANCEJOYSTICK_AUTOPREVIEW                		_T("ColorBalanceJoystick_AutoPreview")
#define HUESHIFT_AUTOPREVEIW                                    _T("HueShift_AutoPreview")
#define HUESHIFT_COLORIZE                                       _T("HueShift_Colorize")
#define POSTERIZE_AUTOPREVIEW                                   _T("Posterize_AutoPreview")
#define THRESHOLD_AUTOPREVIEW                                   _T("Threshold_AutoPreview")
#define HUESHIFT_AUTOPREVIEW                                    _T("HueShift_AutoPreview")
#define HUEMAP_AUTOPREVIEW                                      _T("HueMap_AutoPreview")

class CModifyColorMapsDlg : public CPPMapDlg
{
private:
protected:
	MODIFYMAP_PARMS m_Parms;

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLoadMap();
	afx_msg void OnSaveMap();
public:
	enum{IDD=IDD_MAP};
	CModifyColorMapsDlg(CServerView* pView, CWnd* pParent=NULL);


	LPMODIFYMAP_PARMS GetParmsPtr() { return &m_Parms;}     

DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CModifyColorMapsDlg)
};

class CModifyOptionsDlg : public CPPModalDlg
{
private:
	BOOL m_bAutoPreview;
	int m_nPoints;
	BOOL m_bGrid;
	BOOL m_bPercentages;
	int m_nCurveType;
	CModifyColorMapsDlg* m_pParent;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
public:
	virtual int DoModal() {PreDoModal(); return CDialog::DoModal();}

public:
	enum{IDD=IDD_MAP_OPTIONS};
	CModifyOptionsDlg(CWnd* pParent = NULL);
	afx_msg void OnControlMapNum();



DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CModifyOptionsDlg)
};

class CMapLoadDlg : public CPPModalDlg
{
private:
	FNAME m_szFileName;
	CPPMapDlg* m_pParent;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
public:
	enum{IDD=IDD_MAPLOAD};
	CMapLoadDlg(CWnd* pParent = NULL);

DECLARE_DYNAMIC(CMapLoadDlg)
};

class CMapSaveDlg : public CPPModalDlg
{
private:
	FNAME m_szFileName;
	CPPMapDlg* m_pParent;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
public:
	enum{IDD=IDD_MAPSAVE};
	CMapSaveDlg(CWnd* pParent = NULL);

DECLARE_DYNAMIC(CMapSaveDlg)
};              

class CContrastBrightnessJoystickDlg : public CPPMapDlg
{
private:
protected:
	CONTBRIT_PARMS m_Parms;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);

public:
	enum{IDD=IDD_CONTBRIT_STICK};
	CContrastBrightnessJoystickDlg(CServerView* pView, CWnd* pParent=NULL);

	LPCONTBRIT_PARMS GetParmsPtr() {return &m_Parms;}
DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CContrastBrightnessJoystickDlg)
};

class CContrastBrightnessVisualDlg : public CPPVisualMapDlg
{
private:
protected:
	CONTBRIT_PARMS m_Parms;
	CBSTRUCT m_Data;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryNewPalette();
	BOOL ContBritVisual_GetNew(int x ,int y, LPINT cont, LPINT brit);
	void EnableContrastVisual(BOOL bEnable);
	CVisual* GetVisualPtr() {return &m_Visual;}
public:
//      virtual void _DrawVisualEx(HDC hDC,LPRECT lpRect, int x,int);
//      virtual BOOL _MoveVisualEx(int x ,int y);
	//afx_msg void OnControlBand();
public:
	enum{IDD=IDD_CONTBRIT_VISUAL};
	CContrastBrightnessVisualDlg(CServerView* pView, UINT nIDTemplate, CWnd* pParent=NULL);
	~CContrastBrightnessVisualDlg() {}      
	LPCONTBRIT_PARMS GetParmsPtr() {return &m_Parms;}
	friend class CContrastBrightnessVisual;
	CContrastBrightnessVisual m_Visual;
DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CContrastBrightnessVisualDlg)
};


class CColorBalanceJoystickDlg : public CPPMapDlg
{
private:
	
	protected:
	BALANCE_PARMS m_Parms;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);

	void Balance_Set_Gradient(); 
public:
	enum{IDD=IDD_COLORBAL_STICK};
	CColorBalanceJoystickDlg(CServerView* pView, CWnd* pParent=NULL);

	LPBALANCE_PARMS GetParmsPtr() {return &m_Parms;}
DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CColorBalanceJoystickDlg)
};

	
class CColorBalanceVisualDlg : public CPPVisualMapDlg
{
private:
protected:
	BALANCESTRUCT m_Data;
	BALANCE_PARMS m_Parms;
	int m_BalVals[4][4];

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryNewPalette();
	void BalanceVisual_MakeMaps(LPMAP lpMap, int newBrit, LPBALANCESTRUCT lpBAL, BOOL bPerminant);
	MAP* BalanceVisual_GetNew(int x ,int y, LPINT newBrit);
	void BalanceVisual_PrepareValues();
	void BalanceVisual_RestoreValues();
	CVisual* GetVisualPtr() {return &m_Visual;}
public:
//      virtual void _DrawVisualEx(HDC hDC,LPRECT lpRect, int x,int y);
//      virtual BOOL _MoveVisualEx(int x ,int y);
	

public:
	enum{IDD=IDD_COLORBAL_VISUAL};
	CColorBalanceVisualDlg(CServerView* pView, UINT nIDTemplate, CWnd* pParent=NULL);
	~CColorBalanceVisualDlg(){}
	LPBALANCE_PARMS GetParmsPtr() {return &m_Parms;}
	friend class CColorBalanceVisual;
	CColorBalanceVisual m_Visual;

DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CColorBalanceVisualDlg)
};

class CPosterizeDlg : public CPPMapDlg
{
private:
protected:
	POSTERIZE_PARMS m_Parms;


protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);
	MAP* Posterize_ResetChannel(WORD wChannel );
public:
	enum{IDD=IDD_POSTERIZE};
	CPosterizeDlg(CServerView* pView, CWnd* pParent=NULL);
	LPPOSTERIZE_PARMS GetParmsPtr() {return &m_Parms;}
DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CPosterizeDlg)
};

class CThresholdDlg : public CPPMapDlg
{
private:
protected:
POSTERIZE_PARMS m_Parms;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);
	MAP* Threshold_ResetChannel(WORD wChannel );
public:
	enum{IDD=IDD_THRESHOLD};
	CThresholdDlg(CServerView* pView, CWnd* pParent=NULL);
	LPPOSTERIZE_PARMS GetParmsPtr() {return &m_Parms;}
DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CThresholdDlg)
};

class CHueShiftDlg : public CPPMapDlg
{
private:
protected:
	HUE_PARMS m_Parms;
	int m_nInHue;
	int m_nOutHue;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);
	void HueShift_SetControls(BOOL bUpdateHueVal );
	void HueShift_Make_Map();
public:
	enum{IDD=IDD_HUE_SHIFT};
	CHueShiftDlg(CServerView* pView, CWnd* pParent=NULL);
	LPHUE_PARMS GetParmsPtr() {return &m_Parms;}

DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CHueShiftDlg)
};

class CHueMapDlg : public CPPMapDlg
{
private:
protected:
	int m_HueStart;                 // The hue of HueMap.Pnt[1] (in hue coords)
	HUE_PARMS m_Parms;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnTimer(UINT nIDEvent);
	void Hue_ResetControls();
	void Hue_Reset(int slot );
	void Hue_Fix_Map();
public:
	enum{IDD=IDD_HUE};
	CHueMapDlg(CServerView* pView, CWnd* pParent=NULL);
	LPHUE_PARMS GetParmsPtr() {return &m_Parms;}
DECLARE_MESSAGE_MAP()
DECLARE_DYNAMIC(CHueMapDlg)
};
	
// ------------------------------------------------------------------
#endif //_DLGSMAP_H_




