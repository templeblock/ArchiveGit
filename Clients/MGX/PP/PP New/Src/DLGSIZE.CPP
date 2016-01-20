// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "svrview.h"
#include "dlgsize.h"



BEGIN_MESSAGE_MAP(CSizeImageDlg, CPPViewModalDlg)
    ON_WM_CLOSE()
END_MESSAGE_MAP()                      


CSizeImageDlg::CSizeImageDlg( CServerView *pView,  CWnd *pParentWnd ):
    CPPViewModalDlg (pView, IDD_SIZE, pParentWnd)
{
    ASSERT(pView);
    ASSERT(pView->GetDocument());
    ASSERT(pView->GetDocument()->m_lpImage);
}

BOOL CSizeImageDlg::OnInitDialog()
{
    PreDoModal(NULL);

    CenterPopup( GetSafeHwnd() );
    DoSizeInits( );
    return(TRUE);
}

afx_msg void CSizeImageDlg::OnClose()
{
    SetUnitResolution( 0 );
    CPPViewModalDlg::OnClose();
}

BOOL CSizeImageDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    HWND hDlg = this->GetSafeHwnd();
    BOOL bReturn = TRUE;
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

    switch (nID)
	{
	    case IDC_DISTORT:
	    case IDC_HEIGHT:
	    case IDC_WIDTH:
	    case IDC_RES:
	    case IDC_SCALEX:
	    case IDC_SCALEY:
	    case IDC_SMARTSIZE:
	    case IDC_MAINTAINSIZE:
	    case IDC_PREF_UNITS:
	        bReturn = DoSizeControls( nID, nCode, YES );
	        break;

        default:
            bReturn = FALSE;
	        break;
    }

    return bReturn ? bReturn : CPPViewModalDlg::OnCommand(wParam, lParam);
}

BOOL CSizeImageDlg::PreDoModal(LPVOID lpVoid)
{
    m_Units           = Control.Units;
    m_UnitRes         = Control.UnitRes; 
    m_Points          = Control.Points;
    m_SmartSize       = Edit.SmartSize;

    return CPPViewModalDlg::PreDoModal(lpVoid);
}

LPVOID CSizeImageDlg::PostDoModal()
{
    Control.Units        = m_Units;
    Control.UnitRes      = m_UnitRes; 
    Control.Points       = m_Points;
    Edit.SmartSize       = m_SmartSize;

    return CPPViewModalDlg::PostDoModal();
}

void CSizeImageDlg::OnOK()
{
    if ( (FMUL( m_Resolution, m_Width ) > MAX_IMAGE_WIDTH) ||
       (FMUL( m_Resolution, m_Height) > MAX_IMAGE_HEIGHT))
    {
        Message( IDS_ETOOWIDE );
        OnCancel();            
        return;
    }
	SetUnitResolution( 0 );
	SetUnitInfo((UNIT_TYPE)(m_Units-IDC_PREF_UNITINCHES), m_UnitRes, m_Points);

    PostDoModal();

    CPPViewModalDlg::OnOK();
}

void CSizeImageDlg::OnCancel()
{
    SetUnitResolution( 0 );
    SetUnitInfo((UNIT_TYPE)(m_Units-IDC_PREF_UNITINCHES), m_UnitRes, m_Points);

    CPPViewModalDlg::OnCancel();
}

int CSizeImageDlg::SetUnitResolution( int res )
{
	int OldUnitRes;
	LPIMAGE lpImage = GetImage();

	OldUnitRes = m_UnitRes;

	if ( !(m_UnitRes = res) )
	{
		m_UnitRes =
			( lpImage ? FrameResolution(ImgGetBaseEditFrame(lpImage)) : 1000 );
	}
	// setup control library
	SetUnitInfo((UNIT_TYPE)(m_Units-IDC_PREF_UNITINCHES), m_UnitRes, m_Points);

	return( OldUnitRes );
}

VOID CSizeImageDlg::DoSizeInits( )
{
    HWND hDlg = GetSafeHwnd();
    LPFRAME lpFrame;
    LPIMAGE lpImage = GetImage();
    FRMDATATYPE Type = FDT_RGBCOLOR;

    ImgGetInfo(lpImage, NULL, NULL, NULL, &Type);

    /* Be careful to set all values before doing a SetDlg... with them */
    /* The Edit structure is a temorary holding area for sizing dialog boxes */
    m_Distortable = NO;
    m_MaintainSize = NO;
    m_ScaleX = m_ScaleY = 100;
    if ( lpImage )
    {
	    lpFrame = ImgGetBaseEditFrame(lpImage);
	    m_Crop.left  = m_Crop.top = 0;
	    m_Crop.right = lpImage->npix - 1;
	    m_Crop.bottom = lpImage->nlin - 1;
	    m_Resolution = m_CropResolution = m_OrigResolution =
  				          FrameResolution(lpFrame);
	    m_Depth = FrameDepth(lpFrame);
    }
    else
    {
	    m_Resolution = m_CropResolution = 75;
	    m_Depth = 1;
    }
    m_Angle = 0;
    m_Width  = FGET( CROPWIDTH(), m_CropResolution );
    m_Height = FGET( CROPHEIGHT(), m_CropResolution );

    CheckDlgButton( IDC_DISTORT, m_Distortable);
    CheckDlgButton( IDC_SMARTSIZE, m_SmartSize );
    CheckDlgButton( IDC_MAINTAINSIZE, m_MaintainSize );
 
    ControlEnable(hDlg, IDC_DISTORT, !m_MaintainSize);
    ControlEnable(hDlg, IDC_MAINTAINSIZE, !m_Distortable );
    ControlEnable( hDlg, IDC_SMARTSIZE, !m_MaintainSize && 
        !m_Distortable && CANDOTRANSPARENCY(Type));

    InitDlgItemSpinFixed( hDlg, IDC_HEIGHT, m_Height, YES, 0L, FGET(9999,1) );
    InitDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES, 0L, FGET(9999,1) );
    InitDlgItemSpin( hDlg, IDC_SCALEX, m_ScaleX, YES, 1, 10000 );
    InitDlgItemSpin( hDlg, IDC_SCALEY, m_ScaleY, YES, 1, 10000 );
    InitDlgItemSpin( hDlg, IDC_RES, m_Resolution, NO, 1, 10000 );

    ImageMemory( hDlg, IDC_MEMORY, m_Width, m_Height, m_Resolution,
	    m_Depth );

    CheckComboItem(hDlg, IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
	                     IDC_PREF_UNITLAST, m_Units);

    SetUnitLabels();
}


BOOL CSizeImageDlg::DoSizeControls( ITEMID id, UINT codeNotify, BOOL bResAdjust )
{
    BOOL bResult = TRUE;
    HWND hDlg = GetSafeHwnd();
    BOOL Bool;
    long x, y;
    int npix, nlin;
    HWND hWnd, hControl;
    RECT rArea;
    LFIXED scale;
    LPIMAGE lpImage;
    FRMDATATYPE Type;

    lpImage = GetImage();
    ImgGetInfo(lpImage, NULL, NULL, NULL, &Type);
    npix = CROPWIDTH();
    nlin = CROPHEIGHT();

    switch( id )
    {
	    case IDC_SMARTSIZE:
	        m_SmartSize = !m_SmartSize;
	        CheckDlgButton( IDC_SMARTSIZE, m_SmartSize );
            break;

	    case IDC_MAINTAINSIZE:
	        m_MaintainSize = !m_MaintainSize;
	        if (m_MaintainSize)
		        m_Distortable = NO;

	        ControlEnable(hDlg, IDC_DISTORT, !m_MaintainSize);
	        ControlEnable(hDlg, IDC_SMARTSIZE, !m_MaintainSize);
	        Bool = !m_MaintainSize || m_Units != IDC_PREF_UNITPIXELS;
	        ControlEnable(hDlg, IDC_WIDTH, Bool);
	        ControlEnable(hDlg, IDC_HEIGHT, Bool);
	        ControlEnable(hDlg, IDC_SCALEX, Bool);
	        ControlEnable(hDlg, IDC_SCALEY, Bool);

	        if (m_MaintainSize)
	        {
		        m_Resolution = m_CropResolution = m_OrigResolution;
		        m_Width  = FGET( npix, m_CropResolution );
		        m_Height = FGET( nlin, m_CropResolution );
		        SetUnitResolution( m_CropResolution );
		        SetDlgItemSpin( hDlg, IDC_RES, m_Resolution, NO );
		        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES );
		        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, m_Height, YES );
	        }
	        CheckDlgButton( IDC_MAINTAINSIZE, m_MaintainSize );
	        CheckDlgButton( IDC_SMARTSIZE, !m_MaintainSize && m_SmartSize );
	        break;

	    case IDC_PREF_UNITS:
			{
			WORD wUnits;				
	        if ( !(wUnits = HandleCombo( hDlg, id, codeNotify )) )
		        break;
	        
			m_Units = wUnits;
	        SetUnitInfo((UNIT_TYPE)(m_Units-IDC_PREF_UNITINCHES), m_UnitRes, 
                m_Points);
	        CheckComboItem(hDlg, IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
		        IDC_PREF_UNITLAST, m_Units);

	        Bool = !m_MaintainSize || m_Units != IDC_PREF_UNITPIXELS;
	        ControlEnable(hDlg, IDC_WIDTH, Bool);
	        ControlEnable(hDlg, IDC_HEIGHT, Bool);
	        ControlEnable(hDlg, IDC_SCALEX, Bool);
	        ControlEnable(hDlg, IDC_SCALEY, Bool);

	        if (m_MaintainSize && m_Units == IDC_PREF_UNITPIXELS)
	        {
		        m_CropResolution = m_Resolution;
		        SetUnitResolution( m_CropResolution );
		        m_Width  = FGET( npix, m_CropResolution );
		        m_Height = FGET( nlin, m_CropResolution );
		        y = FMUL( m_CropResolution, m_Height );
		        m_ScaleY = ((y*100L) + nlin/2) / nlin;
		        SetDlgItemSpin( hDlg, IDC_SCALEY, m_ScaleY, NO );
		        x = FMUL( m_CropResolution, m_Width );
		        m_ScaleX = ((x*100L) + npix/2) / npix;
		        SetDlgItemSpin( hDlg, IDC_SCALEX, m_ScaleX, NO );
	        }
	       	SetDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES );
	       	SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, m_Height, YES );
	        SetUnitLabels();
			}
	        break;

        case IDC_DISTORT:
	        m_Distortable = !m_Distortable;
	        CheckDlgButton( IDC_DISTORT, m_Distortable );
	        if (m_Distortable)
		        m_MaintainSize = NO;
	        ControlEnable(hDlg, IDC_MAINTAINSIZE, !m_MaintainSize);
	        if ( m_ScaleX != m_ScaleY )
	        {
		        m_ScaleX = m_ScaleY;
		        x = (((long)npix * m_ScaleX)+50L)/100L;
		        m_Width  = FGET( x, m_CropResolution );
		        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES );
		        SetDlgItemSpin( hDlg, IDC_SCALEX, m_ScaleX, NO );
	        }
	        CheckDlgButton( IDC_SMARTSIZE,m_SmartSize && !m_Distortable);
	        ControlEnable( hDlg, IDC_SMARTSIZE, !m_Distortable && CANDOTRANSPARENCY(Type));
	        ControlEnable(hDlg, IDC_MAINTAINSIZE, !m_Distortable && lpImage);
	        if ( Page.PrintNumeric )
		        break;
	        if ( !(hWnd = ::GetDlgItem( hDlg,IDC_PRINTVIEWPAGE)) )
		        break;
	        if ( !(hControl = ::GetDlgItem( hDlg,IDC_PRINTVIEWIMAGE)) )
		        break;
	        SetLongBit(hControl, GWL_STYLE, IS_RATIO_CONSTRAIN,
		        !m_Distortable );
	        if (!m_Distortable)
	        {
		        ::GetWindowRect(hControl, &rArea);
		        ::ScreenToClient(hWnd, (LPPOINT)&rArea.left);
		        ::ScreenToClient(hWnd, (LPPOINT)&rArea.right);
		        ::MoveWindow(hControl, rArea.left, rArea.top,
			        RectWidth(&rArea)-1, RectHeight(&rArea)-1, TRUE);
	        }
	        break;

        case IDC_HEIGHT:
	        if ( ::GetFocus() != ::GetDlgItem( hDlg,id ) )
		        break;
	        if ( codeNotify != EN_CHANGE )
		        break;
	        m_Height = GetDlgItemSpinFixed( hDlg, IDC_HEIGHT, &Bool, YES);
	        if (m_MaintainSize)
	        {
		        scale = FGET(FUNITY, m_Height);
		        m_Resolution = FMUL(nlin, scale);
		        SetUnitResolution( m_Resolution );
		        SetDlgItemSpin( hDlg, IDC_RES, m_Resolution, NO );
	        }
	        // y = number of new pixels x 100
	        y = FMUL( m_CropResolution, m_Height );
	        // calculate scale factor = (new pixels * 100) / old pixels
	        m_ScaleY = ((y * 100L) + nlin/2) / nlin;
	        SetDlgItemSpin( hDlg, IDC_SCALEY, m_ScaleY, NO );
	        if ( !m_Distortable )
	        {
		        m_ScaleX = m_ScaleY;
				x = (((long)npix * (long)y) + (nlin/2))/ (long)nlin;
		        m_Width  = FGET( x, m_CropResolution );
		        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES );
		        SetDlgItemSpin( hDlg, IDC_SCALEX, m_ScaleX, NO );
	        }
	        ImageMemory( hDlg, IDC_MEMORY,
			        m_MaintainSize ? FGET(npix,1) : m_Width,
			        m_MaintainSize ? FGET(nlin,1) : m_Height,
			        m_MaintainSize ? 1 : m_Resolution, m_Depth );
	        break;

        case IDC_WIDTH:
	        if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		        break;
	        if ( codeNotify != EN_CHANGE )
		        break;
	        m_Width = GetDlgItemSpinFixed( hDlg, IDC_WIDTH, &Bool, YES );
	        if (m_MaintainSize)
	        {
		        scale = FGET(FUNITY, m_Width);
		        m_Resolution = FMUL(npix, scale);
		        SetUnitResolution( m_Resolution );
		        SetDlgItemSpin( hDlg, IDC_RES, m_Resolution, NO );
	        }
	        x = FMUL( m_CropResolution, m_Width );
	        m_ScaleX = ((x * 100L) + npix/2L) / (long)npix;
	        SetDlgItemSpin( hDlg, IDC_SCALEX, m_ScaleX, NO );
	        if ( !m_Distortable )
	        {
				m_ScaleY = m_ScaleX;
				y = (((long)nlin * (long)x) + (npix/2))/ (long)npix;
		        m_Height  = FGET( y, m_CropResolution );
		        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, m_Height, YES );
		        SetDlgItemSpin( hDlg, IDC_SCALEY, m_ScaleY, NO );
	        }
	        ImageMemory( hDlg, IDC_MEMORY,
			        m_MaintainSize ? FGET(npix,1) : m_Width,
			        m_MaintainSize ? FGET(nlin,1) : m_Height,
			        m_MaintainSize ? 1 : m_Resolution, m_Depth );
	        break;

        case IDC_RES:
	        if ( ::GetFocus() != ::GetDlgItem( hDlg, id ) )
		        break;
	        if ( codeNotify != EN_CHANGE )
		        break;
	        m_Resolution = GetDlgItemSpin( hDlg, IDC_RES, &Bool, NO );
	        if ( m_Resolution <= 0 )
		        m_Resolution = 2;
	        if (m_MaintainSize)
	        {
		        m_Width = FGET(npix, m_Resolution);
		        m_Height = FGET(nlin, m_Resolution);
		        SetUnitResolution( m_Resolution );
		        if (m_Units != IDC_PREF_UNITPIXELS)
		        {
			        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES );
			        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, m_Height, YES );
			        y = FMUL( m_CropResolution, m_Height );
			        m_ScaleY = ((y*100L) + nlin/2) / nlin;
			        SetDlgItemSpin( hDlg, IDC_SCALEY, m_ScaleY, NO );
			        x = FMUL( m_CropResolution, m_Width );
			        m_ScaleX = ((x*100L) + npix/2) / npix;
			        SetDlgItemSpin( hDlg, IDC_SCALEX, m_ScaleX, NO );
		        }
	        }
	
	        if ( m_Units == IDC_PREF_UNITPIXELS )
            {
	            if ( bResAdjust )
                { // Extract the values again (inch values changed)
			        m_CropResolution = m_Resolution;
			        SetUnitResolution( m_Resolution );
			        ::SetFocus( hWnd = ::GetDlgItem( hDlg,IDC_HEIGHT ) );
					FORWARD_WM_COMMAND(hDlg, IDC_HEIGHT, hWnd, EN_CHANGE,
									   ::SendMessage);
			        ::SetFocus( hWnd = ::GetDlgItem( hDlg,IDC_WIDTH ) );
					FORWARD_WM_COMMAND(hDlg, IDC_WIDTH, hWnd, EN_CHANGE,
									   ::SendMessage);
			        ::SetFocus( ::GetDlgItem( hDlg,id ) );
		        }
	            else
		        { // Change the edit box
			        SetUnitResolution( m_Resolution );
			        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES);
			        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, m_Height, YES );
		        }
            }
	        ImageMemory( hDlg, IDC_MEMORY,
			        m_MaintainSize ? FGET(npix,1) : m_Width,
			        m_MaintainSize ? FGET(nlin,1) : m_Height,
			        m_MaintainSize ? 1 : m_Resolution, m_Depth );
	        break;

        case IDC_SCALEX:
	        if ( GetFocus() != GetDlgItem( id ) )
		        break;
	        if ( codeNotify != EN_CHANGE )
		        break;
	        m_ScaleX = GetDlgItemSpin( hDlg, IDC_SCALEX, &Bool, NO );
	        x = (((long)npix * m_ScaleX)+50L)/100L;
	        m_Width  = FGET( x, m_CropResolution );
	        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES );
	        if (m_MaintainSize)
	        {
		        scale = FGET(FUNITY, m_Width);
		        m_Resolution = FMUL(npix, scale);
		        SetUnitResolution( m_Resolution );
		        SetDlgItemSpin( hDlg, IDC_RES, m_Resolution, NO );
	        }
	        if ( !m_Distortable )
	        {
		        m_ScaleY = m_ScaleX;
		        y = (((long)nlin * m_ScaleY)+50)/100L;
		        m_Height  = FGET( y, m_CropResolution );
		        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, m_Height, YES );
		        SetDlgItemSpin( hDlg, IDC_SCALEY, m_ScaleY, NO );
	        }
	        ImageMemory( hDlg, IDC_MEMORY,
			        m_MaintainSize ? FGET(npix,1) : m_Width,
			        m_MaintainSize ? FGET(nlin,1) : m_Height,
			        m_MaintainSize ? 1 : m_Resolution, m_Depth );
	        break;

        case IDC_SCALEY:
	        if ( GetFocus() != GetDlgItem( id ) )
		        break;
	        if ( codeNotify != EN_CHANGE )
		        break;
	        m_ScaleY = GetDlgItemSpin( hDlg, IDC_SCALEY, &Bool, NO );
	        y = (((long)nlin * m_ScaleY)+50)/100L;
	        m_Height = FGET( y, m_CropResolution );
	        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, m_Height, YES );
	        if (m_MaintainSize)
	        {
		        scale = FGET(FUNITY, m_Height);
		        m_Resolution = FMUL(nlin, scale);
		        SetUnitResolution( m_Resolution );
		        SetDlgItemSpin( hDlg, IDC_RES, m_Resolution, NO );
	        }
	        if ( !m_Distortable )
	        {
		        m_ScaleX = m_ScaleY;
		        x = (((long)npix * m_ScaleX)+50L)/100L;
		        m_Width  = FGET( x, m_CropResolution );
		        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, m_Width, YES);
		        SetDlgItemSpin( hDlg, IDC_SCALEX, m_ScaleX, NO );
	        }
	        ImageMemory( hDlg, IDC_MEMORY,
			        m_MaintainSize ? FGET(npix,1) : m_Width,
			        m_MaintainSize ? FGET(nlin,1) : m_Height,
			        m_MaintainSize ? 1 : m_Resolution, m_Depth );
	        break;

        case IDC_MEMORY:
        default:
            bResult = FALSE;
    	    break;
    }
    return bResult;        
}


VOID CSizeImageDlg::SetUnitLabels()
{
    int idCtl;
    LPSTR lpUnit;

    if ( !AstralStr( m_Units-IDC_PREF_UNITINCHES+IDS_INCHES, &lpUnit ) )
	    return;

    for (idCtl = IDC_FIRSTLABEL; idCtl <= IDC_LASTLABEL; ++idCtl)
	{
    	if (GetDlgItem(idCtl))
    		SetDlgItemText( idCtl, lpUnit );
	}
}

