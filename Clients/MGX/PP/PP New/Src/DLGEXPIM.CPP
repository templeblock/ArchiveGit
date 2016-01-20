// dlgexpim.cpp
// CExpandImageDlg implementation

#include "pp.h"
#include "ppafx.h"
#include "ppmodal.h"
#include "dlgexpim.h"
#include "bsrutil.h"
#include "id.h"
#include "pickpal.h"
#include "ipframe.h"
#include "browser.h"
#include "ctlutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CExpandImageDlg dialog


CExpandImageDlg::CExpandImageDlg(CServerView *pView,CWnd* pParent /*=NULL*/)
        : CPPViewModalDlg(pView,CExpandImageDlg::IDD, pParent)
{
    
}


//void CExpandImageDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CPPViewModalDlg::DoDataExchange(pDX);
        
//}

BEGIN_MESSAGE_MAP(CExpandImageDlg, CPPViewModalDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_EXPANDRIGHT, OnDeltapos)
	ON_NOTIFY(UDN_DELTAPOS,	IDC_EXPANDLEFT, OnDeltapos) 
	ON_NOTIFY(UDN_DELTAPOS, IDC_EXPANDTOP, OnDeltapos)
	ON_NOTIFY(UDN_DELTAPOS,	IDC_EXPANDBOTTOM, OnDeltapos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HEIGHT, OnDeltapos)
	ON_NOTIFY(UDN_DELTAPOS, IDC_WIDTH, OnDeltapos)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CExpandImageDlg message handlers

BOOL CExpandImageDlg::OnInitDialog() 
{
	PreDoModal();
	DoExpandInits();
	return CPPViewModalDlg::OnInitDialog();
}

BOOL CExpandImageDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
int id = LOWORD(wParam);			//ID of control
WORD codeNotify = HIWORD(wParam);	//Control notification code
HWND hControl = (HWND) lParam;		//Handle of control


LPIMAGE     lpImage;
FRMTYPEINFO TypeInfo;
long        Color;

switch (id)
	{
	case IDC_EXPANDLEFT:
	case IDC_EXPANDRIGHT:
	case IDC_EXPANDTOP:
	case IDC_EXPANDBOTTOM:
	case IDC_HEIGHT:
	case IDC_WIDTH:
	case IDC_PREF_UNITS:
	DoExpandControls(id, codeNotify);
	break;
	
	case IDC_FILLINCOLOR:
	if (codeNotify != 1)
		break;
	lpImage = GetImage();
	if (lpImage)
	   ImgGetTypeInfo(lpImage, &TypeInfo);
	else
	   FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);

	if (TypeInfo.DataType == FDT_LINEART) 
		{
		if(m_FillColor.gray == 255)
			m_FillColor.gray = 0;
		else
			m_FillColor.gray = 255;
		SetColorInfo(&m_FillColor, &m_FillColor, CS_GRAY);
		}
	else if (TypeInfo.DataType == FDT_GRAYSCALE || TypeInfo.DataType == FDT_PALETTECOLOR) 
	   {
	   if (!PalettePicker(&m_FillColor, NULL, TypeInfo.ColorMap))
	      break;
	   }
	else if (!ColorPicker(&m_FillColor, NULL))
	   break;
	CopyRGB( &m_FillColor.rgb, &Color );
	SetWindowLong(::GetDlgItem(GetSafeHwnd(),IDC_FILLINCOLOR), GWL_ACTIVE, Color );
	AstralControlRepaint(GetSafeHwnd(), IDC_FILLINCOLOR);
	break;

	case IDOK:
	if ( (FMUL( m_Resolution, m_Width ) > MAX_IMAGE_WIDTH) ||
		 (FMUL( m_Resolution, m_Height) > MAX_IMAGE_HEIGHT))
		{
		Message( IDS_ETOOWIDE );
		break;
		}
	SetUnitResolution( 0 );
	Control.Units = m_SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes,
																		 Control.Points);
	AstralDlgEnd( GetSafeHwnd(), TRUE );
	break;
   
   default:
	break;
   }
	
	return CPPViewModalDlg::OnCommand(wParam, lParam);
}

BOOL CExpandImageDlg::PreDoModal (LPVOID lpVoid)
{
	LPIMAGE lpImage = NULL;
	LPFRAME lpFrame;
	
	if(lpImage = GetImage())
	{	
		lpFrame = ImgGetBaseEditFrame(lpImage);
		m_Units = m_SaveUnits = Control.Units; 
		m_Crop.top = m_Crop.left = 0;  
		m_Crop.right = lpImage->npix - 1; 
		m_Crop.bottom = lpImage->nlin - 1; 
		m_Resolution = m_CropResolution = m_OrigResolution = FrameResolution(lpFrame);
		m_Depth = FrameDepth(lpFrame);
	 
	}
	else
	{
		m_Resolution = m_CropResolution = 75;
		m_Depth = 1;
	
	}
	
	m_Width = m_OrigWidth = FGET(m_Crop.right - m_Crop.left + 1, m_CropResolution);
	m_Height = m_OrigHeight = FGET(m_Crop.bottom - m_Crop.top + 1, m_CropResolution);
	m_Left = m_Top = 0;
	m_FillColor.gray = 255;


	return CPPViewModalDlg::PreDoModal();

}


LPVOID CExpandImageDlg::PostDoModal ()
{
	Edit.SaveUnits = m_SaveUnits;
	Edit.Crop.left = m_Crop.top;
	Edit.Crop.right = m_Crop.right;
	Edit.Crop.bottom = m_Crop.bottom;
	Edit.Resolution = m_Resolution; 
	Edit.CropResolution = m_CropResolution; 
	Edit.OrigResolution =	m_OrigResolution;
	Edit.Depth = m_Depth;
	Edit.OrigWidth = m_OrigWidth;
	Edit.OrigHeight	 = m_OrigHeight;
	Edit.FillColor = m_FillColor;


	return CPPViewModalDlg::PostDoModal();
	
}

/***********************************************************************/
VOID CExpandImageDlg::DoExpandInits(  )
/***********************************************************************/
{
long Color;
HWND hWnd = GetSafeHwnd();


InitDlgItemSpinFixed(hWnd, IDC_HEIGHT, m_Height, YES, 0L, FGET(9999,1) );
InitDlgItemSpinFixed(hWnd, IDC_WIDTH, m_Width, YES, 0L, FGET(9999,1) );
InitDlgItemSpinFixed(hWnd, IDC_EXPANDLEFT, m_Left, YES, 0L, FGET(9999,1) );
InitDlgItemSpinFixed(hWnd, IDC_EXPANDRIGHT,m_Width-m_OrigWidth-m_Left, YES, 0L, FGET(9999,1) );
InitDlgItemSpinFixed(hWnd, IDC_EXPANDTOP, m_Top, YES, 0L, FGET(9999,1) );
InitDlgItemSpinFixed(hWnd, IDC_EXPANDBOTTOM,m_Height-m_OrigHeight-m_Top, YES, 0L, FGET(9999,1) );
ImageMemory( hWnd, IDC_MEMORY, m_Width, m_Height, m_Resolution,m_Depth );
CheckComboItem(hWnd,IDC_PREF_UNITS, IDC_PREF_UNITFIRST,IDC_PREF_UNITLAST, m_Units);
ControlEnable(hWnd, IDOK, FALSE);
SetUnitLabels(hWnd);
SetColorInfo(&m_FillColor, &m_FillColor, CS_GRAY);
CopyRGB( &m_FillColor.rgb, &Color );
SetWindowLong( ::GetDlgItem(hWnd,IDC_FILLINCOLOR), GWL_ACTIVE, Color );
}


/***********************************************************************/
VOID CExpandImageDlg::DoExpandControls(ITEMID id, UINT codeNotify )
/***********************************************************************/
{
BOOL Bool;
WORD idUnits;
LFIXED Right, Bottom;

switch( id )
    {
	case IDC_PREF_UNITS:
	if ( !(idUnits = HandleCombo( GetSafeHwnd(), id, codeNotify )) )
		break;
	Control.Units = idUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	CheckComboItem(GetSafeHwnd(), IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
		IDC_PREF_UNITLAST, Control.Units);
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_WIDTH, m_Width, YES );
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_HEIGHT, m_Height, YES );
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_EXPANDLEFT, m_Left, YES );
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_EXPANDRIGHT, m_Width-m_OrigWidth-m_Left, YES );
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_EXPANDTOP, m_Top, YES );
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_EXPANDBOTTOM, m_Height-m_OrigHeight-m_Top, YES );
	SetUnitLabels(GetSafeHwnd());
	break;

   case IDC_HEIGHT:
	if (codeNotify == EN_KILLFOCUS)
		{
		if (m_Height < m_OrigHeight)
			{
			m_Height = m_OrigHeight;
			m_Top = 0;
			MessageBeep(0);
			SetDlgItemSpinFixed( GetSafeHwnd(), IDC_HEIGHT, m_Height, YES );
			ControlEnable(GetSafeHwnd(), IDOK, 	(m_Height > m_OrigHeight) ||
								(m_OrigHeight > m_OrigWidth));
			}
		else
			break;
		}
	else
		{
		if ( ::GetFocus() != ::GetDlgItem( GetSafeHwnd(), id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		m_Height = GetDlgItemSpinFixed( GetSafeHwnd(), IDC_HEIGHT, &Bool, YES);
		if (m_Height < m_OrigHeight)
			{
			ControlEnable(GetSafeHwnd(), IDOK, FALSE);
			::SetFocus(::GetDlgItem( GetSafeHwnd(),id));
			break;
			}
		}
	if ((m_OrigHeight + m_Top) > m_Height)
		m_Top = m_Height - m_OrigHeight;
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_EXPANDTOP, m_Top, YES );
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_EXPANDBOTTOM,m_Height - m_OrigHeight- m_Top,YES );
	ImageMemory( GetSafeHwnd(), IDC_MEMORY, m_Width, m_Height, m_Resolution,
				 m_Depth );
	ControlEnable(GetSafeHwnd(), IDOK, 	(m_Height > m_OrigHeight) || (m_Width > m_OrigWidth));
	break;

    case IDC_WIDTH:
	if (codeNotify == EN_KILLFOCUS)
		{
		if (m_Width < m_OrigWidth)
			{
			m_Width = m_OrigWidth;
			m_Left = 0;
			MessageBeep(0);
			SetDlgItemSpinFixed( GetSafeHwnd(), IDC_WIDTH, m_Width, YES );
			ControlEnable(GetSafeHwnd(), IDOK, 	(m_Height > m_OrigHeight)
																 ||(m_Width > m_OrigWidth));
			}
		else
			break;
		}
	else
		{
		if ( ::GetFocus() != ::GetDlgItem( GetSafeHwnd(), id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		m_Width = GetDlgItemSpinFixed( GetSafeHwnd(), IDC_WIDTH, &Bool, YES );
		if (m_Width < m_OrigWidth)
			{
			ControlEnable(GetSafeHwnd(), IDOK, FALSE);
			::SetFocus(::GetDlgItem(GetSafeHwnd(),id));
			break;
			}
		}
	if (m_OrigWidth + m_Left > m_Width)
		m_Left = m_Width - m_OrigWidth;
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_EXPANDLEFT, m_Left, YES );
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_EXPANDRIGHT, m_Width - m_OrigWidth - m_Left, YES );
	ImageMemory( GetSafeHwnd(), IDC_MEMORY, m_Width, m_Height, m_Resolution, m_Depth );
	ControlEnable(GetSafeHwnd(), IDOK, 	(m_Height > m_OrigHeight) ||(m_Width > m_OrigWidth));
	break;

	case IDC_EXPANDLEFT:
	if ( GetFocus() != GetDlgItem(id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Right = m_Width - m_OrigWidth - m_Left;
	m_Left = GetDlgItemSpinFixed(GetSafeHwnd(), id, &Bool, YES);
	m_Width = (m_OrigWidth + m_Left + Right);
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_WIDTH, m_Width, YES );
	ImageMemory( GetSafeHwnd(), IDC_MEMORY, m_Width, m_Height, m_Resolution,m_Depth );
	ControlEnable(GetSafeHwnd(), IDOK, (m_Height > m_OrigHeight) ||(m_Width > m_OrigWidth));
	break;

	case IDC_EXPANDRIGHT:
	if ( GetFocus() != GetDlgItem( id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Right = GetDlgItemSpinFixed(GetSafeHwnd(), id, &Bool, YES);
	m_Width = m_OrigWidth + m_Left+ Right;
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_WIDTH, m_Width, YES );
	ImageMemory( GetSafeHwnd(), IDC_MEMORY, m_Width, m_Height, m_Resolution,m_Depth );
	ControlEnable(GetSafeHwnd(), IDOK, 	(m_Height > m_OrigHeight) ||(m_Width > m_OrigWidth));
	break;

	case IDC_EXPANDTOP:
	if ( GetFocus() != GetDlgItem( id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Bottom = m_Height- m_OrigHeight - m_Top;
	m_Top = GetDlgItemSpinFixed(GetSafeHwnd(), id, &Bool, YES);
	m_Height = m_OrigHeight + m_Top + Bottom;
	SetDlgItemSpinFixed(GetSafeHwnd(), IDC_HEIGHT, m_Height, YES );
	ImageMemory( GetSafeHwnd(), IDC_MEMORY, m_Width, m_Height, m_Resolution,m_Depth );
	ControlEnable(GetSafeHwnd(), IDOK, 	(m_Height > m_OrigHeight) ||(m_Width > m_OrigWidth));
	break;

	case IDC_EXPANDBOTTOM:
	if ( GetFocus() != GetDlgItem(id) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Bottom = GetDlgItemSpinFixed(GetSafeHwnd(), id, &Bool, YES);
	m_Height = m_OrigHeight + m_Top + Bottom;
	SetDlgItemSpinFixed( GetSafeHwnd(), IDC_HEIGHT, m_Height, YES );
	ImageMemory( GetSafeHwnd(), IDC_MEMORY, m_Width, m_Height, m_Resolution,m_Depth );
	ControlEnable(GetSafeHwnd(), IDOK, 	(m_Height > m_OrigHeight) ||(m_Width > m_OrigWidth));
	break;

    default:
	break;
    }
}



void CExpandImageDlg::OnCancel() 
{
	SetUnitResolution( 0 );
	Control.Units = m_SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	
	CPPViewModalDlg::OnCancel();
}

void CExpandImageDlg::OnOK() 
{

	
	CPPViewModalDlg::OnOK();
}


void CExpandImageDlg::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPPModalDlg::OnDeltapos(pNMHDR, pResult);
}

