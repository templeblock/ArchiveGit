// prefs.cpp : implementation file
//

#include "pp.h"
#include "prefs.h"
#include "ppafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern STRING szAppName;

extern LPSTR RamDisk_Str;
extern LPSTR MainMemMin_Str;
extern LPSTR MainMemFactor_Str;
extern LPSTR LineArtAsGray_Str;

extern LPSTR NoUndo_Str;
extern LPSTR UseApply_Str;
extern LPSTR UndoObjects_Str;
extern LPSTR UndoMasks_Str;
extern LPSTR DoCommandList_Str;

extern LPSTR MultipleObjects_Str;
extern LPSTR UseMaskAndObjects_Str;

extern LPSTR ScratchpadWidth_Str;
extern LPSTR ScratchpadHeight_Str;
extern LPSTR IsScratchpadColor_Str;

extern LPSTR UsePlugins_Str;
extern LPSTR PluginPath1_Str;
extern LPSTR PluginPath2_Str;

extern LPSTR Units_Str;
extern LPSTR UseRulers_Str;
extern LPSTR DisplayPixelValue_Str;
extern LPSTR ScreenWidth_Str;

extern LPSTR PouchPath_Str;
extern LPSTR UseTablet_Str;
extern LPSTR StartMacro_Str;
extern LPSTR MaskTint_Str;
extern LPSTR IndependentSettings_Str;
extern LPSTR PasteAtRealSize_Str;
extern LPSTR UseGrayScaleIcons_Str;
extern LPSTR DisableAutoScroll_Str;
extern LPSTR ColorManagerEnabled_Str;
extern LPSTR DisableDither_Str;
extern LPSTR DefaultFileType_Str;

/////////////////////////////////////////////////////////////////////////////
// CMemoryPrefPage property page

IMPLEMENT_DYNCREATE(CMemoryPrefPage, CPPPropertyPage)
 
CMemoryPrefPage::CMemoryPrefPage() : CPPPropertyPage(CMemoryPrefPage::IDD)
{
	lstrcpy( m_szRamDisk, Control.RamDisk );
	m_MainMemMin = Control.MainMemMin;
	m_MainMemFactor = Control.MainMemFactor;
	m_LineArtAsGray = Control.LineArtAsGray;
}

CMemoryPrefPage::~CMemoryPrefPage()
{
}

BEGIN_MESSAGE_MAP(CMemoryPrefPage, CPPPropertyPage)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemoryPrefPage message handlers

BOOL CMemoryPrefPage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	::CheckDlgButton( hDlg, IDC_PREF_LINEART, Control.LineArtAsGray);
	::SetDlgItemText( hDlg, IDC_PREF_RAMDISK, Control.RamDisk );
	InitDlgItemSpin( hDlg, IDC_PREF_MEMMIN,    Control.MainMemMin,    NO, 0, 16384 );
	SetDlgItemSpinStep( hDlg, IDC_PREF_MEMMIN, 64L );
	InitDlgItemSpin( hDlg, IDC_PREF_MEMFACTOR, Control.MainMemFactor, NO, 1, 100 );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMemoryPrefPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL 	Bool;
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_PREF_RAMDISK:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id, Control.RamDisk,
				sizeof(Control.RamDisk));
			FixPath( Lowercase( Control.RamDisk ) );
			SetModified(TRUE);
		break;

		case IDC_PREF_MEMMIN:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			Control.MainMemMin = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			SetModified(TRUE);
		break;

		case IDC_PREF_MEMFACTOR:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			Control.MainMemFactor = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			SetModified(TRUE);
		break;

		case IDC_PREF_LINEART:
			Control.LineArtAsGray = !Control.LineArtAsGray;
			::CheckDlgButton( hDlg, id, Control.LineArtAsGray );
			SetModified(TRUE);
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
    	}
	return(TRUE);
}

void CMemoryPrefPage::HandleOK()
{
	Control.MainMemFactor = bound( Control.MainMemFactor, 10, 100 );
	FrameSettings( Control.RamDisk,
		Control.MainMemMin,
		Control.MainMemFactor);
	CPPPropertyPage::HandleOK();
}
   
void CMemoryPrefPage::HandleCancel()
{
	lstrcpy( Control.RamDisk, m_szRamDisk);
	Control.MainMemMin = m_MainMemMin;
	Control.MainMemFactor = m_MainMemFactor;
	Control.LineArtAsGray = m_LineArtAsGray;
	CPPPropertyPage::HandleCancel();
}

void CMemoryPrefPage::OnSave()
{
	HandleOK();
	PutDefaultString( RamDisk_Str, Control.RamDisk );
	PutDefaultInt( MainMemMin_Str, Control.MainMemMin );
	PutDefaultInt( MainMemFactor_Str, Control.MainMemFactor );
	PutDefaultInt( LineArtAsGray_Str, Control.LineArtAsGray );
}
   
/////////////////////////////////////////////////////////////////////////////
// CUndoPrefPage property page

IMPLEMENT_DYNCREATE(CUndoPrefPage, CPPPropertyPage)
 
CUndoPrefPage::CUndoPrefPage() : CPPPropertyPage(CUndoPrefPage::IDD)
{
	m_UseApply = Control.UseApply;
	m_NoUndo = Control.NoUndo;
	m_UndoMasks = Control.UndoMasks;
	m_UndoObjects = Control.UndoObjects;
	m_DoCommandList = Control.DoCommandList;
}

CUndoPrefPage::~CUndoPrefPage()
{
}

void CUndoPrefPage::WackCmdList()
{
	POSITION       TemplatePos = PictPubApp.GetFirstDocTemplate();
   	POSITION       DocPos;
   	CServerDoc     *pDoc;
   	CDocTemplate   *pTemplate;

	while (TemplatePos)
	{
 		pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
		DocPos = pTemplate->GetFirstDocPosition();
		while (DocPos)
	   	{
    		pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
    		if (pDoc)
        	{
        		pDoc->m_lpImage->lpCmdList->fSaveCommands = FALSE;
        		DestroyPacketList(&pDoc->m_lpImage->lpCmdList->PacketList);
        		DestroyPacketList(&pDoc->m_lpImage->lpCmdList->UndoPacketList);
        		DestroyPacketList(&pDoc->m_lpImage->lpCmdList->RedoPacketList);
        	}
   		}
	}
}

BEGIN_MESSAGE_MAP(CUndoPrefPage, CPPPropertyPage)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUndoPrefPage message handlers

BOOL CUndoPrefPage::OnInitDialog() 
{
	ITEMID id;
 	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	if ( Control.NoUndo )
			id = IDC_PREF_NOUNDO;
	else	id = (Control.UseApply + IDC_PREF_AUTOAPPLY);
	CheckComboItem(  hDlg, IDC_PREF_UNDOTYPE, IDC_PREF_NOUNDO, IDC_PREF_MANUALAPPLY, id );

	if (::GetDlgItem(hDlg, IDC_PREF_UNDOMODE))
		CheckComboItem(  hDlg, IDC_PREF_UNDOMODE,
						IDC_PREF_IMAGEUNDO, IDC_PREF_OBJECTUNDO,
						Control.UndoObjects + IDC_PREF_IMAGEUNDO );
	::CheckDlgButton( hDlg, IDC_PREF_MASKUNDO, !Control.UndoMasks );
	::CheckDlgButton( hDlg, IDC_PREF_DISABLECMDLIST, !Control.DoCommandList );
	ControlEnable( hDlg, IDC_PREF_MASKUNDO, !Control.NoUndo );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CUndoPrefPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_PREF_UNDOTYPE:
//		case IDC_PREF_NOUNDO:
//		case IDC_PREF_MANUALAPPLY:
//		case IDC_PREF_AUTOAPPLY:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			Control.NoUndo = (id == IDC_PREF_NOUNDO);
			Control.UseApply = (id == IDC_PREF_MANUALAPPLY);
			ControlEnable( hDlg, IDC_PREF_MASKUNDO, !Control.NoUndo );
			SetModified(TRUE);
		break;

		case IDC_PREF_UNDOMODE:
//		case IDC_PREF_IMAGEUNDO:
//		case IDC_PREF_OBJECTUNDO:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			Control.UndoObjects = (id == IDC_PREF_OBJECTUNDO);
			SetModified(TRUE);
		break;

		case IDC_PREF_MASKUNDO:
			Control.UndoMasks = !Control.UndoMasks;
			::CheckDlgButton( hDlg, id, !Control.UndoMasks );
			SetModified(TRUE);
		break;

		case IDC_PREF_DISABLECMDLIST:
    		Control.DoCommandList = !Control.DoCommandList;
	    	::CheckDlgButton( hDlg, id, !Control.DoCommandList );
			SetModified(TRUE);
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}

void CUndoPrefPage::HandleOK()
{
	if (m_DoCommandList && !Control.DoCommandList)
		WackCmdList();
	CPPPropertyPage::HandleOK();
}
   
void CUndoPrefPage::HandleCancel()
{
	Control.UseApply = m_UseApply;
	Control.NoUndo = m_NoUndo;
	Control.UndoMasks = m_UndoMasks;
	Control.UndoObjects = m_UndoObjects;
	Control.DoCommandList = m_DoCommandList;
	CPPPropertyPage::HandleCancel();
}

void CUndoPrefPage::OnSave()
{ 
	HandleOK();
	PutDefaultInt( NoUndo_Str, Control.NoUndo );
	PutDefaultInt( UseApply_Str, Control.UseApply );
	PutDefaultInt( UndoObjects_Str, Control.UndoObjects );
	PutDefaultInt( UndoMasks_Str, Control.UndoMasks );
	PutDefaultInt( DoCommandList_Str, Control.DoCommandList );
}
   
/////////////////////////////////////////////////////////////////////////////
// CObjectsPrefPage property page

IMPLEMENT_DYNCREATE(CObjectsPrefPage, CPPPropertyPage)
 
CObjectsPrefPage::CObjectsPrefPage() : CPPPropertyPage(CObjectsPrefPage::IDD)
{
	m_bObjects = Control.MultipleObjects;
	m_bObjectMasks = Control.UseMaskAndObjects;
}

CObjectsPrefPage::~CObjectsPrefPage()
{
}

BEGIN_MESSAGE_MAP(CObjectsPrefPage, CPPPropertyPage)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectsPrefPage message handlers

BOOL CObjectsPrefPage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	::CheckDlgButton( hDlg, IDC_PREF_OBJECTS, Control.MultipleObjects );
	//	if (!Control.MultipleObjects)
	//		Control.UseMaskAndObjects = FALSE;
	::CheckDlgButton( hDlg, IDC_PREF_OBJECTMASKS, Control.UseMaskAndObjects );
	ControlEnable(  hDlg, IDC_PREF_OBJECTMASKS, Control.MultipleObjects );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CObjectsPrefPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_PREF_OBJECTS:
			Control.MultipleObjects = !Control.MultipleObjects;
			::CheckDlgButton( hDlg, id, Control.MultipleObjects );
//			if (!Control.MultipleObjects)
//				Control.UseMaskAndObjects = FALSE;
//			::CheckDlgButton( hDlg, IDC_PREF_OBJECTMASKS, Control.UseMaskAndObjects );
			ControlEnable(  hDlg, IDC_PREF_OBJECTMASKS, Control.MultipleObjects );
			SetModified(TRUE);
		break;

		case IDC_PREF_OBJECTMASKS:
			if (!Control.MultipleObjects)
				break;
			Control.UseMaskAndObjects = !Control.UseMaskAndObjects;
			::CheckDlgButton( hDlg, id, Control.UseMaskAndObjects );
			SetModified(TRUE);
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}

void CObjectsPrefPage::HandleOK()
{
	HWND hActiveWnd;
	if (Control.MultipleObjects != m_bObjects)
		PictPubApp.EnableObjectMenu(AfxGetMainWnd()->GetMenu());
	LPIMAGE lpImage = NULL;
	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);
	if (!Control.UseMaskAndObjects &&
		Control.UseMaskAndObjects != m_bObjectMasks)
	{
		Control.UseMaskAndObjects = YES;
		RemoveObjectMarquee(lpImage);
		Control.UseMaskAndObjects = NO;
	}
	CPPPropertyPage::HandleOK();
}
   
void CObjectsPrefPage::HandleCancel()
{
	Control.MultipleObjects = m_bObjects;
	Control.UseMaskAndObjects = m_bObjectMasks;
	CPPPropertyPage::HandleCancel();
}

void CObjectsPrefPage::OnSave()
{
	HandleOK();
	PutDefaultInt( MultipleObjects_Str, Control.MultipleObjects );
	PutDefaultInt( UseMaskAndObjects_Str, Control.UseMaskAndObjects );
}
   
/////////////////////////////////////////////////////////////////////////////
// CScratchpadPrefPage property page

IMPLEMENT_DYNCREATE(CScratchpadPrefPage, CPPPropertyPage)
 
CScratchpadPrefPage::CScratchpadPrefPage() : CPPPropertyPage(CScratchpadPrefPage::IDD)
{
	m_ScratchpadWidth   = Control.ScratchpadWidth;
	m_ScratchpadHeight  = Control.ScratchpadHeight;
	m_IsScratchpadColor = Control.IsScratchpadColor;
}

CScratchpadPrefPage::~CScratchpadPrefPage()
{
}

BEGIN_MESSAGE_MAP(CScratchpadPrefPage, CPPPropertyPage)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScratchpadPrefPage message handlers

BOOL CScratchpadPrefPage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	CheckComboItem(  hDlg, IDC_PREF_SCRATCHTYPE,
				IDC_PREF_SCRATCHGRAY, IDC_PREF_SCRATCHCOLOR,
				Control.IsScratchpadColor + IDC_PREF_SCRATCHGRAY);
	InitDlgItemSpin( hDlg, IDC_PREF_SCRATCHX, Control.ScratchpadWidth,  NO, 1, 500 );
	InitDlgItemSpin( hDlg, IDC_PREF_SCRATCHY, Control.ScratchpadHeight, NO, 1, 500 );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CScratchpadPrefPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL 	Bool;
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_PREF_SCRATCHX:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Control.ScratchpadWidth = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			SetModified(TRUE);
		break;

		case IDC_PREF_SCRATCHY:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Control.ScratchpadHeight = ::GetDlgItemSpin( hDlg, id, &Bool, NO );
			SetModified(TRUE);
		break;

		case IDC_PREF_SCRATCHTYPE:
//		case IDC_PREF_SCRATCHGRAY:
//		case IDC_PREF_SCRATCHCOLOR:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			Control.IsScratchpadColor = id - IDC_PREF_SCRATCHGRAY;
			SetModified(TRUE);
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}

void CScratchpadPrefPage::HandleOK()
{
	CPPPropertyPage::HandleOK();
}
   
void CScratchpadPrefPage::HandleCancel()
{
	Control.ScratchpadWidth   = m_ScratchpadWidth;
	Control.ScratchpadHeight  = m_ScratchpadHeight;
	Control.IsScratchpadColor = m_IsScratchpadColor;
	CPPPropertyPage::HandleCancel();
}

void CScratchpadPrefPage::OnSave()
{
	HandleOK();
	PutDefaultInt( ScratchpadWidth_Str, Control.ScratchpadWidth );
	PutDefaultInt( ScratchpadHeight_Str, Control.ScratchpadHeight );
	PutDefaultInt( IsScratchpadColor_Str, Control.IsScratchpadColor );
}
   
/////////////////////////////////////////////////////////////////////////////
// CPluginPrefPage property page

IMPLEMENT_DYNCREATE(CPluginPrefPage, CPPPropertyPage)
 
CPluginPrefPage::CPluginPrefPage() : CPPPropertyPage(CPluginPrefPage::IDD)
{
	m_UsePlugins = Control.UsePlugins;
	lstrcpy( m_szPlugin1, Control.PluginPath1 );
	lstrcpy( m_szPlugin2, Control.PluginPath2 );
}

CPluginPrefPage::~CPluginPrefPage()
{
}

BEGIN_MESSAGE_MAP(CPluginPrefPage, CPPPropertyPage)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPluginPrefPage message handlers

BOOL CPluginPrefPage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	::SetDlgItemText( hDlg, IDC_PREF_PLUGINPATH1, Control.PluginPath1);
	::SetDlgItemText( hDlg, IDC_PREF_PLUGINPATH2, Control.PluginPath2);
	::CheckDlgButton( hDlg, IDC_PREF_USEPLUGINS,  Control.UsePlugins);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPluginPrefPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_PREF_USEPLUGINS:
			Control.UsePlugins = !Control.UsePlugins;
			::CheckDlgButton( hDlg, id, Control.UsePlugins );
			SetModified(TRUE);
		break;

		case IDC_PREF_PLUGINPATH1:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
		    	break;
			::GetDlgItemText( hDlg, id, Control.PluginPath1,
				sizeof(Control.PluginPath1));
			FixPath( Lowercase( Control.PluginPath1 ) );
			SetModified(TRUE);
		break;

		case IDC_PREF_PLUGINPATH2:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		    	break;
			if ( codeNotify != EN_CHANGE )
		    	break;
			::GetDlgItemText( hDlg, id, Control.PluginPath2,
				sizeof(Control.PluginPath2));
			FixPath( Lowercase( Control.PluginPath2 ) );
			SetModified(TRUE);
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}

void CPluginPrefPage::HandleOK()
{
	CPPPropertyPage::HandleOK();
}
   
void CPluginPrefPage::HandleCancel()
{
	Control.UsePlugins = m_UsePlugins;
	lstrcpy( Control.PluginPath1, m_szPlugin1 );
	lstrcpy( Control.PluginPath2 , m_szPlugin2 );
	CPPPropertyPage::HandleCancel();
}

void CPluginPrefPage::OnSave()
{
	HandleOK();
	PutDefaultString( PluginPath1_Str, Control.PluginPath1 );
	PutDefaultString( PluginPath2_Str, Control.PluginPath2 );
	PutDefaultInt( UsePlugins_Str, Control.UsePlugins );
}
   
/////////////////////////////////////////////////////////////////////////////
// CUnitsPrefPage property page

IMPLEMENT_DYNCREATE(CUnitsPrefPage, CPPPropertyPage)
 
CUnitsPrefPage::CUnitsPrefPage() : CPPPropertyPage(CUnitsPrefPage::IDD)
{
	m_Units = Control.Units;
	m_ScreenWidth = Control.ScreenWidth;
	m_UseRulers = View.UseRulers;
	m_DisplayPixelValue = Control.DisplayPixelValue;
	SetUnitResolution( 1 ); // For the disabled Screen Width edit box
}

CUnitsPrefPage::~CUnitsPrefPage()
{
}

BEGIN_MESSAGE_MAP(CUnitsPrefPage, CPPPropertyPage)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitsPrefPage message handlers

BOOL CUnitsPrefPage::OnInitDialog() 
{
	LPSTR lpUnit;
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();

	CheckComboItem( hDlg, IDC_PREF_UNITS, IDC_PREF_UNITINCHES, IDC_PREF_UNITPIXELS, Control.Units );
	ControlEnable(  hDlg, IDC_PREF_SCREENWIDTH, Control.Units != IDC_PREF_UNITPIXELS );
	::CheckDlgButton( hDlg, IDC_PREF_USERULERS, View.UseRulers );
	::CheckDlgButton( hDlg, IDC_PREF_DISPLAYPIXELS,
		 		!Control.DisplayPixelValue );
	if ( Control.Units != IDC_PREF_UNITPIXELS )
		AstralSetDlgItemFixed( hDlg, IDC_PREF_SCREENWIDTH,
			Control.ScreenWidth, NO );
	else
		::SetDlgItemInt( hDlg, IDC_PREF_SCREENWIDTH,
			GetSystemMetrics( SM_CXSCREEN ), NO );
	if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES, &lpUnit ))
		::SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CUnitsPrefPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	BOOL 	Bool;
	LPSTR 	lpUnit;
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_PREF_USERULERS:
			View.UseRulers = !View.UseRulers;
			::CheckDlgButton( hDlg, id, View.UseRulers );
			SetModified(TRUE);
		break;

		case IDC_PREF_DISPLAYPIXELS:
			Control.DisplayPixelValue = !Control.DisplayPixelValue;
			::CheckDlgButton( hDlg, id, !Control.DisplayPixelValue );
			SetModified(TRUE);
		break;

		case IDC_PREF_SCREENWIDTH:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			Control.ScreenWidth = AstralGetDlgItemFixed( hDlg, id, &Bool, NO);
			SetModified(TRUE);
		break;

		case IDC_PREF_UNITS:
//		case IDC_PREF_UNITINCHES:
//		case IDC_PREF_UNITMM:
//		case IDC_PREF_UNITCM:
//		case IDC_PREF_UNITPICAS:
//		case IDC_PREF_UNITPIXELS:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			Control.Units = id;
			SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
			if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES,
	     		&lpUnit ) )
				::SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
			ControlEnable( hDlg, IDC_PREF_SCREENWIDTH,
				Control.Units != IDC_PREF_UNITPIXELS );

			if ( Control.Units != IDC_PREF_UNITPIXELS )
				AstralSetDlgItemFixed( hDlg, IDC_PREF_SCREENWIDTH, Control.ScreenWidth, NO );
			else
				::SetDlgItemInt( hDlg, IDC_PREF_SCREENWIDTH, GetSystemMetrics( SM_CXSCREEN ), NO );
			SetModified(TRUE);
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}

void CUnitsPrefPage::HandleOK()
{
	if (Control.Units != m_Units)
		SetupAllRulers();
	if (m_UseRulers != View.UseRulers)
		EnableRulers(View.UseRulers);
	SetUnitResolution( 0 );
	CPPPropertyPage::HandleOK();
}
   
void CUnitsPrefPage::HandleCancel()
{
	Control.Units = m_Units;
	Control.ScreenWidth = m_ScreenWidth;
	View.UseRulers = m_UseRulers;
	Control.DisplayPixelValue = m_DisplayPixelValue;
	SetUnitResolution( 0 );
	CPPPropertyPage::HandleCancel();
}

void CUnitsPrefPage::OnSave()
{
	char t[50];

	HandleOK();
	PutDefaultInt( Units_Str, Control.Units - IDC_PREF_UNITINCHES );
	PutDefaultInt( UseRulers_Str, View.UseRulers );
	PutDefaultInt( DisplayPixelValue_Str, Control.DisplayPixelValue );
	PutDefaultString(ScreenWidth_Str,FixedAscii(Control.ScreenWidth,t,-1));
}
   
/////////////////////////////////////////////////////////////////////////////
// CMiscPrefPage property page

static ITEMID FileType_Table[] =
	{
	IDT_PPCOMPOUND,		
	IDT_PP,		
	IDT_TIFF,	
	IDT_TARGA, 	
	IDT_GIF, 	
	IDT_BMP, 	
	IDT_DIB, 	
	IDT_PCX, 	
	IDT_JPEG, 	
	IDT_SCITEX, 
	IDT_PP4
	};

static ITEMID FileType_TableOEM[] =
	{
	IDT_PPCOMPOUND,		
	IDT_PP,		
	IDT_TIFF,	
	IDT_TARGA, 	
	IDT_GIF, 	
	IDT_BMP, 	
	IDT_DIB, 	
	IDT_PCX, 	
	IDT_JPEG, 	
	IDT_PP4
	};

IMPLEMENT_DYNCREATE(CMiscPrefPage, CPPPropertyPage)
 
CMiscPrefPage::CMiscPrefPage() : CPPPropertyPage(CMiscPrefPage::IDD)
{
	lstrcpy( m_szPouchPath, Control.PouchPath );
	lstrcpy(m_szStartMacro, Names.StartMacro);
	m_UseTablet = Control.UseTablet;
	m_MaskTint = Control.MaskTint;
	m_IndependentSettings = Control.IndependentSettings;
	m_PasteAtRealSize = Mask.PasteAtRealSize;
	m_UseGrayScaleIcons = Control.UseGrayScaleIcons;
	m_DisableAutoScroll = Control.DisableAutoScroll;
	m_DisableDither = Vignette.DisableDither;
	m_DefaultFileType = Control.DefaultFileType;
}

CMiscPrefPage::~CMiscPrefPage()
{
}

BEGIN_MESSAGE_MAP(CMiscPrefPage, CPPPropertyPage)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiscPrefPage message handlers

BOOL CMiscPrefPage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

  	CPPPropertyPage::OnInitDialog();
	::SetDlgItemText(  hDlg, IDC_PREF_POUCH, Control.PouchPath );
	CheckComboItem(  hDlg, IDC_PREF_TABLET, IDC_PREF_TABLETUNUSED, IDC_PREF_TABLETBOTH, Control.UseTablet+IDC_PREF_TABLETUNUSED );
   	if (::GetDlgItem(hDlg, IDC_PREF_STARTMACRO))
		InitExtNameN( hDlg, IDC_PREF_STARTMACRO, IDC_FILENAME, Names.StartMacro, IDN_MACRO, IDS_NONE, NO );
	CheckComboItem(  hDlg, IDC_PREF_MASKTINT, IDC_PREF_MASKTINT1, IDC_PREF_MASKTINT6, Control.MaskTint + IDC_PREF_MASKTINT1);
	::CheckDlgButton( hDlg, IDC_PREF_TOOLSETTINGS, Control.IndependentSettings);
	::CheckDlgButton( hDlg, IDC_CMS_ENABLED, Control.CMSEnabled);
	::CheckDlgButton( hDlg, IDC_PREF_PASTEATREALSIZE, Mask.PasteAtRealSize);
	::CheckDlgButton( hDlg, IDC_PREF_USEGRAYSCALEICONS, Control.UseGrayScaleIcons);
	::CheckDlgButton( hDlg, IDC_PREF_DISABLEAUTOSCROLL, Control.DisableAutoScroll);
	::CheckDlgButton( hDlg, IDC_PREF_DISABLEDITHER, Vignette.DisableDither);
    if( Control.Retail )
    {
	    TableCheckComboItem(hDlg, IDC_PREF_FILETYPE, FileType_Table,
				    sizeof(FileType_Table)/sizeof(ITEMID),
				    LocalFileType(Control.DefaultFileType),
				    FALSE);
    }
    else
    {
	    TableCheckComboItem(hDlg, IDC_PREF_FILETYPE, FileType_TableOEM,
				    sizeof(FileType_TableOEM)/sizeof(ITEMID),
				    LocalFileType(Control.DefaultFileType),
				    FALSE);
    }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMiscPrefPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_PREF_POUCH:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id, Control.PouchPath,
				sizeof(Control.PouchPath));
			FixPath( Lowercase( Control.PouchPath ) );
			SetModified(TRUE);
		break;

		case IDC_PREF_TABLET:
//		case IDC_PREF_TABLETUNUSED:
//		case IDC_PREF_TABLETOPACITY:
//		case IDC_PREF_TABLETSIZE:
//		case IDC_PREF_TABLETBOTH:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			Control.UseTablet = id-IDC_PREF_TABLETUNUSED;
			SetModified(TRUE);
		break;

		case IDC_PREF_STARTMACRO:
		   	GetExtNameN( hDlg, id, IDC_FILENAME, Names.StartMacro,
		    	IDN_MACRO, codeNotify, IDS_NONE, NO );
			SetModified(TRUE);
		break;

		case IDC_PREF_MASKTINT:
//		case IDC_PREF_MASKTINT1:
//		case IDC_PREF_MASKTINT2:
//		case IDC_PREF_MASKTINT3:
//		case IDC_PREF_MASKTINT4:
//		case IDC_PREF_MASKTINT5:
//		case IDC_PREF_MASKTINT6:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			Control.MaskTint = id - IDC_PREF_MASKTINT1;
			SetModified(TRUE);
		break;

		case IDC_PREF_TOOLSETTINGS:
			Control.IndependentSettings = !Control.IndependentSettings;
			::CheckDlgButton(hDlg, id, Control.IndependentSettings);
			SetModified(TRUE);
		break;

		case IDC_CMS_ENABLED:
			Control.CMSEnabled = CMSOpen( !Control.CMSEnabled );
			::CheckDlgButton(hDlg, id, Control.CMSEnabled);
			SetModified(TRUE);
		break;

		case IDC_PREF_PASTEATREALSIZE:
			Mask.PasteAtRealSize = !Mask.PasteAtRealSize;
			::CheckDlgButton( hDlg, IDC_PREF_PASTEATREALSIZE, Mask.PasteAtRealSize);
			SetModified(TRUE);
		break;

		case IDC_PREF_USEGRAYSCALEICONS:
			Control.UseGrayScaleIcons = !Control.UseGrayScaleIcons;
			::CheckDlgButton( hDlg, IDC_PREF_USEGRAYSCALEICONS, Control.UseGrayScaleIcons);
			SetModified(TRUE);
		break;

		case IDC_PREF_DISABLEAUTOSCROLL:
			Control.DisableAutoScroll = !Control.DisableAutoScroll;
			::CheckDlgButton( hDlg, IDC_PREF_DISABLEAUTOSCROLL, Control.DisableAutoScroll);
			SetModified(TRUE);
		break;

		case IDC_PREF_DISABLEDITHER:
			Vignette.DisableDither = !Vignette.DisableDither;
			::CheckDlgButton( hDlg, IDC_PREF_DISABLEDITHER, Vignette.DisableDither);
			SetModified(TRUE);
		break;

		case IDC_PREF_FILETYPE:
    		if( Control.Retail )
    		{
	    		if (! (id = TableHandleCombo(hDlg, FileType_Table,
					    		sizeof(FileType_Table)/sizeof(ITEMID),
					    		id, codeNotify)))
            		break;
    		}
    		else
    		{
	    		if (! (id = TableHandleCombo(hDlg, FileType_TableOEM,
					    		sizeof(FileType_TableOEM)/sizeof(ITEMID),
					    		id, codeNotify)))
            		break;
    		}
			Control.DefaultFileType = AppFileType(id);
			SetModified(TRUE);
		break;

	   	default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}

void CMiscPrefPage::HandleOK()
{
	HWND hActiveWnd;
	LPIMAGE lpImage = NULL;
	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);
	if (lpImage && m_MaskTint != Control.MaskTint &&
		ImgGetViewMode(lpImage) == VM_SLIME)
	{
		RECT rRect;

		rRect = GetDisplayPtr(hActiveWnd)->DispRect;
		AstralToWindowsRect(&rRect);
		::InvalidateRect( hActiveWnd, &rRect, FALSE );
	}
	if (!Control.IndependentSettings &&
		Control.IndependentSettings != m_IndependentSettings)
		SetCommonSettings();
	if (Control.UseTablet)
    {
		Control.UseTablet = TabletStart();
        if (!Control.UseTablet)
        {
            LPSTR    lp;

            AstralStr(IDS_NOTABLET, &lp);
            ::MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, 
               MB_ICONHAND | MB_OK | MB_TASKMODAL);
        }
    }
	CPPPropertyPage::HandleOK();
}
   
void CMiscPrefPage::HandleCancel()
{
	lstrcpy( Control.PouchPath, m_szPouchPath );
	lstrcpy(Names.StartMacro, m_szStartMacro);
	Control.UseTablet = m_UseTablet;
	Control.MaskTint = m_MaskTint;
	Control.IndependentSettings = m_IndependentSettings;
	Mask.PasteAtRealSize = m_PasteAtRealSize;
	Control.UseGrayScaleIcons = m_UseGrayScaleIcons;
	Control.DisableAutoScroll = m_DisableAutoScroll;
	Vignette.DisableDither = m_DisableDither;
	Control.DefaultFileType = m_DefaultFileType;
	CPPPropertyPage::HandleCancel();
}

void CMiscPrefPage::OnSave()
{
	HandleOK();
	PictPubApp.WriteLocalProfileFileName( PouchPath_Str, Control.PouchPath );
	PutDefaultInt( UseTablet_Str, Control.UseTablet );
	PutDefaultString( StartMacro_Str, Names.StartMacro );
	PutDefaultInt( MaskTint_Str, Control.MaskTint );
	PutDefaultInt( IndependentSettings_Str, Control.IndependentSettings );
	PutDefaultInt( PasteAtRealSize_Str, Mask.PasteAtRealSize );
	PutDefaultInt( UseGrayScaleIcons_Str, Control.UseGrayScaleIcons);
	PutDefaultInt( DisableAutoScroll_Str, Control.DisableAutoScroll);
	PutDefaultInt( ColorManagerEnabled_Str, Control.CMSEnabled );
	PutDefaultInt( DisableDither_Str, Vignette.DisableDither);
	PutDefaultInt( DefaultFileType_Str, Control.DefaultFileType-IDN_FIRSTFILETYPE);
}
   
/////////////////////////////////////////////////////////////////////////////
// CLocationsPrefPage property page

IMPLEMENT_DYNCREATE(CLocationsPrefPage, CPPPropertyPage)
 
CLocationsPrefPage::CLocationsPrefPage() : CPPPropertyPage(CLocationsPrefPage::IDD)
{
	GetExtNamePath( m_Clipbits, IDN_CLIPBOARD );
	GetExtNamePath( m_Printstyles, IDN_STYLE );
	GetExtNamePath( m_Brushes, IDN_CUSTOM );
	GetExtNamePath( m_Palettes,IDN_PALETTE );
	GetExtNamePath( m_Textures, IDN_TEXTURE );
	GetExtNamePath( m_Devices, IDN_SCANNER );
	GetExtNamePath( m_Effects, IDN_SPECIAL );
    GetExtNamePath( m_Macros, IDN_MACRO );
    GetExtNamePath( m_Importfilters, IDN_IMPORTFILTERS );
}

CLocationsPrefPage::~CLocationsPrefPage()
{
}

BEGIN_MESSAGE_MAP(CLocationsPrefPage, CPPPropertyPage)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocationsPrefPage message handlers

BOOL CLocationsPrefPage::OnInitDialog() 
{
	HWND	hDlg = GetSafeHwnd();

 	CPPPropertyPage::OnInitDialog();
	::SetDlgItemText( hDlg, IDC_LOCATE_CLIPBITS, Lowercase(m_Clipbits) );
	::SetDlgItemText( hDlg, IDC_LOCATE_PRNSTYLES, Lowercase(m_Printstyles) );
	::SetDlgItemText( hDlg, IDC_LOCATE_BRUSHES, Lowercase(m_Brushes) );
	::SetDlgItemText( hDlg, IDC_LOCATE_PALETTES, Lowercase(m_Palettes) );
	::SetDlgItemText( hDlg, IDC_LOCATE_TEXTURES, Lowercase(m_Textures) );
	::SetDlgItemText( hDlg, IDC_LOCATE_DEVICES, Lowercase(m_Devices) );
	::SetDlgItemText( hDlg, IDC_LOCATE_FILTERS, Lowercase(m_Effects) );
	if (::GetDlgItem(hDlg, IDC_LOCATE_MACROS))
	    ::SetDlgItemText( hDlg, IDC_LOCATE_MACROS, Lowercase(m_Macros) );
	if (::GetDlgItem(hDlg, IDC_LOCATE_IMPORTFILTERS))
	    ::SetDlgItemText( hDlg, IDC_LOCATE_IMPORTFILTERS, Lowercase(m_Importfilters));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CLocationsPrefPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	HWND   	hActiveWnd = GetActiveDoc();
	HWND	hDlg = GetSafeHwnd();
	int 	id = GET_WM_COMMAND_ID(wParam, lParam);
	UINT 	codeNotify = GET_WM_COMMAND_CMD(wParam, lParam);

	switch (id)
	{
		case IDC_LOCATE_CLIPBITS:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id, m_Clipbits,
				sizeof(m_Clipbits));
			FixPath( Lowercase( m_Clipbits ) );
			SetModified(TRUE);
		break;

		case IDC_LOCATE_PRNSTYLES:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id, m_Printstyles,
				sizeof(m_Printstyles));
			FixPath( Lowercase( m_Printstyles ) );
			SetModified(TRUE);
		break;
	
		case IDC_LOCATE_BRUSHES:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id, m_Brushes,
				sizeof(m_Brushes));
			FixPath( Lowercase( m_Brushes ) );
			SetModified(TRUE);
		break;

		case IDC_LOCATE_PALETTES:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id,m_Palettes ,
				sizeof(m_Palettes));
			FixPath( Lowercase( m_Palettes ) );
			SetModified(TRUE);
		break;

		case IDC_LOCATE_TEXTURES:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id,m_Textures ,
				sizeof(m_Textures));
			FixPath( Lowercase( m_Textures ) );
			SetModified(TRUE);
		break;

		case IDC_LOCATE_DEVICES:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id,m_Devices ,
				sizeof(m_Devices));
			FixPath( Lowercase( m_Devices ) );
			SetModified(TRUE);
		break;

		case IDC_LOCATE_FILTERS:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id,m_Effects ,
				sizeof(m_Effects));
			FixPath( Lowercase( m_Effects ) );
			SetModified(TRUE);
		break;

		case IDC_LOCATE_MACROS:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id,m_Macros ,
				sizeof(m_Macros));
			FixPath( Lowercase( m_Macros ) );
			SetModified(TRUE);
		break;

		case IDC_LOCATE_IMPORTFILTERS:
			if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
	    		break;
			if ( codeNotify != EN_CHANGE )
	    		break;
			::GetDlgItemText( hDlg, id,m_Importfilters ,
				sizeof(m_Importfilters));
			FixPath( Lowercase( m_Importfilters ) );
			SetModified(TRUE);
		break;

		default:
			return (CPPPropertyPage::OnCommand(wParam, lParam));   	
		break;
   	}
	return(TRUE);
}

void CLocationsPrefPage::HandleOK()
{
	FixPath(m_Clipbits);
	PictPubApp.WriteLocalProfileFileName( "ClipBit", m_Clipbits );
	FixPath( m_Printstyles ); 
	PictPubApp.WriteLocalProfileFileName( "PrnStyle", m_Printstyles );
	FixPath( m_Brushes ); 
	PictPubApp.WriteLocalProfileFileName( "Brush", m_Brushes );
	FixPath( m_Palettes ); 
	PictPubApp.WriteLocalProfileFileName( "Palette", m_Palettes );
	FixPath( m_Textures ); 
	PictPubApp.WriteLocalProfileFileName( "Texture", m_Textures );
	FixPath( m_Devices ); 
	PictPubApp.WriteLocalProfileFileName( "Device", m_Devices );
	FixPath( m_Effects ); 
	PictPubApp.WriteLocalProfileFileName( "Effects", m_Effects );
	FixPath( m_Macros ); 
	PictPubApp.WriteLocalProfileFileName( "Macros", m_Macros );
  	FixPath( m_Importfilters ); 
  	PictPubApp.WriteLocalProfileFileName( "Filters", m_Importfilters );
	LoadAllExtNamePaths( NO );
	CPPPropertyPage::HandleOK();
}
   
void CLocationsPrefPage::HandleCancel()
{
	CPPPropertyPage::HandleCancel();
}

void CLocationsPrefPage::OnSave()
{
	HandleOK();
}
   
/////////////////////////////////////////////////////////////////////////////
// CPreferencesPropSheet

IMPLEMENT_DYNAMIC(CPreferencesPropSheet, CPPPropertySheet)

int CPreferencesPropSheet::m_nLastPage = 0;

CPreferencesPropSheet::CPreferencesPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT nIDHelp)
	:CPPPropertySheet(nIDCaption, pParentWnd, nIDHelp, GetSelectPage())
{
	CommonConstruct();
}

CPreferencesPropSheet::CPreferencesPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT nIDHelp)
	:CPPPropertySheet(pszCaption, pParentWnd, nIDHelp, GetSelectPage())
{
	CommonConstruct();
}

void CPreferencesPropSheet::CommonConstruct()
{
	EnableStackedTabs(FALSE);
	AddPage(&m_MemoryPage);
	AddPage(&m_UndoPage);
	AddPage(&m_ObjectsPage);
	AddPage(&m_ScratchpadPage);
	AddPage(&m_PluginPage);
	AddPage(&m_UnitsPage);
	AddPage(&m_MiscPage);
	AddPage(&m_LocationsPage);
}

CPreferencesPropSheet::~CPreferencesPropSheet()
{
	m_nLastPage = m_nCurPage;
}

UINT CPreferencesPropSheet::GetSelectPage()
{
	return(m_nLastPage);
}

void CPreferencesPropSheet::HandleOK()
{
	HWND hDlg = GetSafeHwnd();
	CPPPropertySheet::HandleOK();
}

void CPreferencesPropSheet::HandleCancel()
{
	CPPPropertySheet::HandleCancel();
}

/////////////////////////////////////////////////////////////////////////////
// CPreferencesPropSheet message handlers

BEGIN_MESSAGE_MAP(CPreferencesPropSheet, CPPPropertySheet)
	ON_COMMAND(ID_APPLY_NOW, OnSave)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CPreferencesPropSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Add the preview window to the property sheet.

	int nReturn;
	if ((nReturn = CPPPropertySheet::OnCreate(lpCreateStruct)) != 0)
		return nReturn;

	CWnd *pApplyWnd = GetDlgItem(ID_APPLY_NOW);
	if (pApplyWnd)
		pApplyWnd->SetWindowText("Save");
	return 0;
}

void CPreferencesPropSheet::OnSave()
{
	ASSERT_VALID(this);

	AstralCursor(IDC_WAIT);
	int iNumPages = GetPageCount();
	for (int iPage = 0; iPage < iNumPages; iPage++)
	{
   		CPPPropertyPage * pPage =  (CPPPropertyPage *)GetPage(iPage);
   		if (pPage && pPage->PageEverActive())
    		pPage->SendMessage(WM_COMMAND, ID_APPLY_NOW);
	}
	AstralCursor(NULL);
	CPPPropertySheet::OnOK();
}
   

