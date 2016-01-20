// GradientFillBackdropDialog.cpp : implementation file
//

#include "FrameworkIncludes.h"
#include "GradientFillBackdropDialog.h"


#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//	REVIEW ESV 12/15/96 - temporary ui
#ifdef _WINDOWS
#define IDD_GRADIENTFILLBACKDROP        130
#define CONTROL_CHECKBOX_GRADIENTFILLBACKDROP 1007
#define CONTROL_EDIT_PRECISION          1002
#define CONTROL_EDIT_HILITEPOSITION     1009
#define CONTROL_EDIT_HILITEANGLE        1010
#define CONTROL_RADIO_LINEAR            1003
#define CONTROL_EDIT_STARTX             1031
#define CONTROL_EDIT_STARTY             1032
#define CONTROL_EDIT_ENDX               1036
#define CONTROL_EDIT_ENDY               1038
#define CONTROL_STATIC_HILITEPOSITION   1013
#define CONTROL_STATIC_HILITEANGLE      1014
#define CONTROL_STATIC_GRADIENTTYPEBOX  1015
#define CONTROL_RADIO_RADIAL            1004
#define CONTROL_RADIO_CIRCULARBURST     1005
#define CONTROL_RADIO_SQUAREBURST       1006
#define CONTROL_STATIC_RAMPBOX          1040
#define CONTROL_STATIC_STARTX           1033
#define CONTROL_STATIC_STARTY           1034
#define CONTROL_STATIC_START            1041
#define CONTROL_STATIC_END              1042
#define CONTROL_STATIC_ENDX             1035
#define CONTROL_STATIC_ENDY             1037
#define CONTROL_STATIC_PRECISION        1011
#endif

/////////////////////////////////////////////////////////////////////////////
// RGradientFillBackdropDialog dialog

RGradientFillBackdropDialog::RGradientFillBackdropDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_GRADIENTFILLBACKDROP, pParent)
{
	m_fGradientFillBackdrop = FALSE;
	m_pctHilitePosition = 0.0f;
	m_angHiliteAngle = 0.0f;
	m_pctPrecision = 100.0f;
	m_pctStartX = 0.0f;
	m_pctStartY = 0.0f;
	m_pctEndX = 100.0f;
	m_pctEndY = 0.0f;
	m_iGradType = 0;
}


void RGradientFillBackdropDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//
	// Gradient fill backdrop enable/disable
	DDX_Check(pDX, CONTROL_CHECKBOX_GRADIENTFILLBACKDROP, m_fGradientFillBackdrop);

	//
	// Precision (50-100%)
	DDX_Text( pDX,	CONTROL_EDIT_PRECISION, m_pctPrecision );
	DDV_MinMaxFloat( pDX, m_pctPrecision, 50.f, 100.f );

	//
	// Hilite Position (0-100% of gradient length)
	DDX_Text( pDX, CONTROL_EDIT_HILITEPOSITION, m_pctHilitePosition );
	DDV_MinMaxFloat( pDX, m_pctHilitePosition, 0.f, 100.f );

	//
	// Hilite Angle (-360-360)
	DDX_Text( pDX, CONTROL_EDIT_HILITEANGLE, m_angHiliteAngle );
	DDV_MinMaxFloat( pDX, m_angHiliteAngle, -360.f, 360.f );

	//
	// Gradient type
	DDX_Radio( pDX, CONTROL_RADIO_LINEAR, m_iGradType );

	//
	// Gradient ramp start position
	DDX_Text( pDX, CONTROL_EDIT_STARTX, m_pctStartX );
	DDV_MinMaxFloat( pDX, m_pctStartX, 0.f, 100.f );
	DDX_Text( pDX, CONTROL_EDIT_STARTY, m_pctStartY );
	DDV_MinMaxFloat( pDX, m_pctStartY, 0.f, 100.f );

	//
	// Gradient ramp end position (ignored)
	DDX_Text( pDX, CONTROL_EDIT_ENDX, m_pctEndX );
	DDV_MinMaxFloat( pDX, m_pctEndX, 0.f, 100.f );
	DDX_Text( pDX, CONTROL_EDIT_ENDY, m_pctEndY );
	DDV_MinMaxFloat( pDX, m_pctEndY, 0.f, 100.f );
}


BEGIN_MESSAGE_MAP(RGradientFillBackdropDialog, CDialog)
	ON_BN_CLICKED( CONTROL_CHECKBOX_GRADIENTFILLBACKDROP, OnGradientfillbackdrop )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RGradientFillBackdropDialog message handlers


BOOL RGradientFillBackdropDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnGradientfillbackdrop();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RGradientFillBackdropDialog::OnGradientfillbackdrop() 
{
	static int idControls[] = 
		{
		CONTROL_STATIC_PRECISION,
		CONTROL_EDIT_PRECISION,
		CONTROL_STATIC_HILITEPOSITION,
		CONTROL_EDIT_HILITEPOSITION,
		CONTROL_STATIC_HILITEANGLE,
		CONTROL_EDIT_HILITEANGLE,
		CONTROL_STATIC_GRADIENTTYPEBOX,
		CONTROL_RADIO_LINEAR,
		CONTROL_RADIO_RADIAL,
		CONTROL_RADIO_CIRCULARBURST,
		CONTROL_RADIO_SQUAREBURST,
		CONTROL_STATIC_RAMPBOX,
		CONTROL_STATIC_START,
		CONTROL_STATIC_STARTX,
		CONTROL_EDIT_STARTX,
		CONTROL_STATIC_STARTY,
		CONTROL_EDIT_STARTY,
		CONTROL_STATIC_END,
		CONTROL_STATIC_ENDX,
		CONTROL_EDIT_ENDX,
		CONTROL_STATIC_ENDY,
		CONTROL_EDIT_ENDY
		};
	BOOL	fEnable	= ((CButton*)GetDlgItem(CONTROL_CHECKBOX_GRADIENTFILLBACKDROP))->GetCheck();
	for( int i=0 ; i<sizeof(idControls)/sizeof(idControls[0]) ; ++i )
		GetDlgItem(idControls[i])->EnableWindow( fEnable );
}

