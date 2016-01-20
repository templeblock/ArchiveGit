//****************************************************************************
//
// File Name:   WinColorDlg.h
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Definition of the RWinColorDlg dialog object, and
//              the RWinColorBtn object.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/WinColorDlg.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _WINCOLORDLG_H_
#define _WINCOLORDLG_H_

#include "ColorDialogInterface.h"

#include "ColorPalette.h"
#include "WinAngleCtrl.h"
#include "WHPTriple.h"
#include "GradientFill.h"
#include "FrameworkResourceIDs.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Symbolic argument for forcing button updates.
const BOOLEAN kMultiColorUpdate = TRUE;

/////////////////////////////////////////////////////////////////////////////
// RWinColorBtn window

class FrameworkLinkage RWinColorBtn : public CButton // ROwnerDrawBtn
{
// Construction
public:

	enum 	
	{	  kDefault			= 0	//	Color button with no inser border
		, kInset				= 1	//	Inset border with color inside
	};

						RWinColorBtn( uWORD uwFlags = kDefault ) ;
	virtual			~RWinColorBtn() ;

	const RColor&	GetColor() const ;

	void				SetGradientAngle( YAngle angle ) ;
	void				SetGradientParams( EGradientType eType, RGradientRamp& ramp, YAngle angle ) ;
	void				SetGradientType( EGradientType eType ) ;
	virtual void	SetColor( RColor color ) ;

// Implementation
protected:

	virtual BOOLEAN Initialize( RDcDrawingSurface& ds, RIntSize size ) ;
	void				GetGradientRect( RIntRect& rect ) ;
//	void				SetGradientOrigin() ;
	void				SetBoundingRect();
	virtual void	UpdateImage() ;

	virtual void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct ) ;

	// Generated message map functions
	//{{AFX_MSG(RWinColorBtn)
	virtual void	PreSubclassWindow( );
	afx_msg void	OnDestroy( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	uWORD								m_uwFlags ;
	RColor							m_crFillColor ;

	RScratchBitmapImage			m_biImage ;
	HBITMAP							m_hbmSave ;
} ;

/////////////////////////////////////////////////////////////////////////////
// RWinColorBtn window

class FrameworkLinkage RWinMultiColorBtn : public RWinColorBtn
{

public:
	enum				EMultiColorState
						{ 
						kSingleColor = 0		//	Show the button's color.
						, kMultiColor			//	Show the multi color bitmap.
						, kMultiGrayscale		//	Show the multi grayscale bitmap.
						};

						RWinMultiColorBtn( uWORD uwFlags = kDefault ) ;
	virtual BOOLEAN Initialize( RDcDrawingSurface& ds, RIntSize size ) ;
	void				SetMultiColor( const BOOLEAN fUpdate = TRUE );
	void				SetMultiGrayscale( const BOOLEAN fUpdate = TRUE );
	virtual void	UpdateImage() ;
	virtual void	SetColor( RColor color ) ;

private:

	EMultiColorState				m_eMultiColorState;			//	Display the multi color or multi grayscale bitmap or the color.
	YResourceId						m_yMultiColorBitmapId;		//	Resource id of the multi color bitmap.
	YResourceId						m_yMultiGrayscaleBitmapId;	//	Resource id of the grayscale multi color bitmap.
};

inline const RColor& RWinColorBtn::GetColor() const
{
	return m_crFillColor ;
}



/////////////////////////////////////////////////////////////////////////////
// RWinColorDlg window



class FrameworkLinkage RWinColorPaletteDlg : public CDialog
{
// Construction
public:
	
							enum { IDD = DIALOG_COLOR_DIALOG };
	
							RWinColorPaletteDlg( 
								CWnd* pParent = NULL, 
								BOOLEAN fShowTransparent = TRUE, 
								UINT nIDTemplate = IDD );

   virtual RColor		SelectedColor() const { return m_crColor; }
   virtual void		SetColor( RColor crColor );

	void					AddSelectedToMRU();

// Dialog Data
	//{{AFX_DATA(RWinColorPaletteDlg)
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RWinColorPaletteDlg)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	static YPlatformColor	mruColors[6];

	// Generated message map functions
	//{{AFX_MSG(RWinColorPaletteDlg)
   afx_msg void		OnColorClicked( UINT nID );
	afx_msg void		OnColorMore();
	afx_msg LRESULT	OnColorChange( WPARAM wParam, LPARAM lParam ) ;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:


	RWinColorPalette			m_scPalette;
	RWinColorPaletteWell		m_scMRUList[6];
	RWinColorPaletteWell		m_scTransparent;

	BOOLEAN						m_fShowTransparent;
	RColor						m_crColor;
};

class FrameworkLinkage RWinColorPaletteDlgEx : public RWinColorPaletteDlg
{
// Construction
public:
	
								RWinColorPaletteDlgEx( CWnd* pParent = NULL, BOOLEAN fShowTransparent = TRUE ) ;


   virtual RColor			SelectedColor() const;
	virtual void			SetColor( RColor crColor ) ;

// Dialog Data
	//{{AFX_DATA(RWinColorDlg)
	enum { IDD = DIALOG_COLOR_DIALOG_EX };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RWinColorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	static int			GetTemplateIndex( RColor& crColor ) ;

	static void			BuildRamp( 
								RGradientRamp& ramp, 
								RWHPTriple crStartColor, 
								const RColorTemplateEntry* pTemplate, 
								sWORD swCount,
								uWORD uwFlags = 0 ) ;

	void					InitStyleButtons() ;
	void					SetGradientColors() ;

	// Generated message map functions
	//{{AFX_MSG(RWinColorDlgEx)
	virtual BOOL		OnInitDialog();
	afx_msg void		OnGradientType();
	afx_msg LRESULT	OnAngleChange( WPARAM wParam, LPARAM lParam ) ;
	afx_msg LRESULT	OnColorChange( WPARAM wParam, LPARAM lParam ) ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	int				m_nGradientType ;
	int				m_nGradientSel ;

	RSolidColor		m_crColor ;

	RWinAngleCtrl	m_wndBlendAngle ;
	RWinColorBtn	m_rbGradientArray[11] ;
	RWinColorBtn	m_rbStyleArray[8] ;

	friend class RWinColorDlg;
};



class FrameworkLinkage RWinColorDlg 
{

// Construction
public:
	
								RWinColorDlg( CWnd* pParent = NULL, uWORD uwFlags = kColorDefault ) ;
	virtual					~RWinColorDlg();

	BOOLEAN					IsSelectedColorSolid() const;
	const RSolidColor		SelectedSolidColor() const;
	const RColor&			SelectedColor() const;
	const RColor&			SelectedColor( const RIntRect& rcBounds );
	void						SetColor( const RColor& color ) ;
	int						DoModal( );

	static RColor			ColorToWhite( RSolidColor crStart, uWORD nGradientType, YAngle angle, BOOLEAN fInvert = FALSE ) ;
	static void				GetGradientInfo( const RColor& crColor, uWORD& nGradientType, YAngle& nGradientAngle ) ;
	static void				SetGradientInfo( RColor& crColor, uWORD& nGradientType, YAngle& nGradientAngle, BOOLEAN fInvert = FALSE ) ;

	void					SetTitle( UINT );
	void					SetTitle( CString& );

private:

	RColorDialogInterface*	m_pColorInterface;
	CWnd*							m_pParent ;
	uWORD							m_uwFlags ;
};

inline BOOLEAN	RWinColorDlg::IsSelectedColorSolid() const
{
	return m_pColorInterface->IsSelectedColorSolid();
}

inline const RSolidColor RWinColorDlg::SelectedSolidColor() const
{
	return m_pColorInterface->SelectedSolidColor();
}

inline const RColor& RWinColorDlg::SelectedColor() const
{
	return m_pColorInterface->SelectedColor();
}

inline const RColor&	RWinColorDlg::SelectedColor( const RIntRect& rcBounds )
{
	return m_pColorInterface->SelectedColor( rcBounds );
}

inline void	RWinColorDlg::SetColor( const RColor& color ) 
{
	m_pColorInterface->SetColor( color );
}

inline int RWinColorDlg::DoModal( )
{
	return m_pColorInterface->DoModal( m_pParent, m_uwFlags );
}

inline void RWinColorDlg::SetTitle( UINT )
{
}
inline void RWinColorDlg::SetTitle( CString& )
{
}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif  // _WINCOLORDLG_H_