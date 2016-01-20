/*	$Header: /PM8/App/CommonGroupedObjectDialog.h 1     3/03/99 6:04p Gbeddow $
//
//	Definition of the CPTimePieceDlg class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a generic compound graphic.
//	
// Author:     Mike Heydlauf
//
// Portability: Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
// $Log: /PM8/App/CommonGroupedObjectDialog.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     2/11/99 3:02p Rgrenfel
// Fixed aspect correct handling and flagged the reconstruction of the
// collection arrays.
// 
// 2     2/10/99 5:07p Rgrenfel
// Enabled the more graphics button and changed the edit path to use the
// embedded component rather than reading the object back out of the
// collection.
// 
// 1     2/01/99 5:42p Rgrenfel
// Base class for certain group component object dialogs.
*/

#ifndef _COMMONGROUPEDOBJECTDIALOG_H_
#define _COMMONGROUPEDOBJECTDIALOG_H_

#include "GenericGroupedObjectDialog.h"
#include "ApplicationGlobals.h"
//#include "RenaissanceResource.h"
#include "SearchResult.h"
#include "HeadlineInterface.h"
#include "ColorSettings.h"
//#include "RenaissanceView.h"
#include "StandaloneView.h"
#include "CollectionSearcher.h"
#include "GraphicCategorySearch.h"
#include "Resource.h"

#ifndef _WINCOLORDLG_H_
	#include "WinColorDlg.h"
#endif

//	Symbolic constant indicating no Graphic Selected.
const uLONG kInvalidGraphicId = 0;

///****************************************************************************
//
//  Class Name:		RGoHeadlineData
//
//  Description:		class containing all headline data pertaining to grouped 
//							object dialogs
//
// ****************************************************************************
//
const RIntPoint kDefaultShadowVanishPt				= RIntPoint( 5,5 );
const YFloatType kDefaultShadowDepth				= YFloatType( 0.2 );

const RIntPoint kDefaultProjectionVanishPt		= RIntPoint( 5,5 );
const YFloatType kDefaultProjectionDepth			= YFloatType( 0.3 );
const int kDefaultProjectionStages					= 6;
class RGOHeadlineData
{
public:
	// Constructors
	RGOHeadlineData();

	RGOHeadlineData(	EDistortEffects		eDistortEffect,     
							int						nShapeIndex,
							BOOLEAN					fDistort
						);
	void							UpdateHeadline( RHeadlineInterface* m_pInterface ) const;
public:
	RMBCString					m_rHeadlineText;
	RMBCString					m_rFontName;
	EDistortEffects			m_eDistortEffect;
	BOOLEAN						m_fDistort;
	ETextJustification		m_eTextJustification;
	YFontAttributes			m_yFontAttributes;
	uWORD							m_uwScale1;
	uWORD							m_uwScale2;
	uWORD							m_uwScale3;
	int							m_nShapeIndex;
	RColor						m_rOutlineFillColor;
	EOutlineEffects			m_eOutlineEffect;

	ELineWeight					m_eLineWeight;
	RSolidColor					m_rStrokeColor;

	EShadowEffects				m_eShadowEffect;
	RSolidColor					m_rShadowColor;
	RIntPoint					m_rShadowVanishPt;
	YFloatType					m_yShadowDepth;

	EProjectionEffects		m_eProjectionEffect;
	RIntPoint					m_rProjectionVanishPt;
	YFloatType					m_yProjectionDepth;
	int							m_nProjectionStages;

};

///****************************************************************************
//	NOTES ABOUT RCOMMONGROUPEDOBJECTDIALOG: -MWH
// 
// RCommonGroupedObjectDialog is an abstract base class used for creating
// dialogs that contain headlines and graphics in a preview area.
//
// Assumptions:
//		When you derive a dialog class from RCommonGroupedObjectDialog the following
//		assumptions are made about that dialog:
//
//		-The dialog has at least one graphic and at least one headline both of which
//     are colorable.
//
//    -The dialog contains an edit control with the id CONTROL_EDIT_COMMONGO_HEADLINETEXT
//     used to get headline text.
//
//		-The dialog contains a listbox with the id CONTROL_LISTBOX_COMMONGO_GRAPHIC
//     used for holding the graphics names.
//		-The first entry of the graphics listbox is kNoneSelection.
//
//		-The dialog contains a listbox with the id CONTROL_LISTBOX_COMMONGO_EFFECTS
//		 usually used for holding canned headline effects though this behaviour
//	    like all the controls can be overridden (see SealDialog and Signature dialog)
//
//		-The dialog contains a button with the id CONTROL_BUTTON_COMMONGO_COLORGRAPHIC
//		 used to display the color dialog for selecting the graphic's color (if monchrome).
//
//		-The dialog contains a button with the id CONTROL_BUTTON_COMMONGO_TINTGRAPHIC
//		 used to display the color dialog for selecting the graphic's color (if monchrome).
//
//		-The dialog contains a button with id CONTROL_BUTTON_COMMONGO_COLORSTYLE used
//		 to display the color dialog for selecting headline's color.
//
//		-The dialog contains a combo box with id CONTROL_COMBO_COMMONGO_FONT used
//		 to hold the system font list.
//
//		-The dialog contains a checkbox with id CONTROL_CHKBOX_COMMONGO_BOLD used
//		 to apply the bold style to headlines.
//
//		-The dialog contains a checkbox with id CONTROL_CHKBOX_COMMONGO_ITALIC used
//		 to apply the italic style to headlines.
//
//		-The dialog contains a static with id CONTROL_STATIC_GENERICGO_PREVIEW_FRAME
//		 used for displaying and manipulating components.
//
// Other Notes:
//		-Although a lot of functionality is contained in this base class, much of
//		 it can be customized or ignored through overriding the class's virtual functions
//
//		-The class RGOHeadlineData is usually used for maintaining and applying 
//     headline effects.
//
//		-Be sure to call the base classes DoDataExchange
//
//		-If you don't want to use one of the assumed controls, be sure to include
//		 it in you dialog resource template with the Visible style unchecked.
//		-RCommonGroupedObjectDialog is derived from RGenericGroupedObjectDialog
//		 which contains all the component entry and manipulation functionality.
//
///****************************************************************************


///****************************************************************************
//
//  Class Name:		RCommonGroupedObjectDialog
//
//  Description:		Base class for grouped object dialogs
//
// ****************************************************************************
//
//const YFloatType kTextTop		= .10;
//const YFloatType kTextLeft		= .10; 
//const YFloatType kTextWidth	= 1.3;
//const YFloatType kTextHeight	= .75;
const RRealSize  kHeadlineScaleSize( .5, .5 ); 
const YFloatType kHeadlineOffset = 0.25;

//	Define a context data version based on a marker field in the stream.
//	If we make sure we are using the correct
//	'version' of the stream then we know we can read and write our data
//	without interfering with other stream users (like derived dialogs)
typedef RMBCString YContextDataVersion;
const YContextDataVersion kContextDataVersion2 = _T("kContextDataVersion2");		//	The size before any version2 data is written.

//	Index of None selection in Graphic list.
const int kNoGraphicSelected = 0;

//	The resize handle size in screen units.
const YIntDimension	kRotationResizeHandleHitSize	= 32;	//	cursor size of 32
const YIntDimension	kResizeHandleHitSize				= 16;	//	1/2 cursor size of 32

//const YFloatType kPercentToShrinkHeadlineX = .75;
//const YFloatType kPercentToShrinkHeadlineY = .25;

extern RMBCString kNoneSelection;

class RCommonGroupedObjectDialog : public RGenericGroupedObjectDialog
{
// Construction
public:
	RCommonGroupedObjectDialog( RStandaloneDocument* pParentDocument,
										 RComponentDocument* pEditedComponent,
										 int nResourceId = RCommonGroupedObjectDialog::IDD );

	// Force this class to be abstract...
	virtual ~RCommonGroupedObjectDialog() = 0;

// Dialog Data
	//{{AFX_DATA(RGenericGroupedObjectDialog)
	enum { IDD = IDD_COMMON_DIALOG_TEMPLATE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RGenericGroupedObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnButtonGOGraphicColor();
	afx_msg void OnButtonGOOtherGraphics();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RGenericGroupedObjectDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK( );
	virtual afx_msg void OnSelchangeListboxGraphics();
	virtual afx_msg void OnSelchangeListboxEffects();
	virtual afx_msg void OnSelchangeComboFont();
	virtual afx_msg void OnButtonGOEffectColor();
						 void OnButtonGOEffectBold();
						 void OnButtonGOEffectItalic();
	virtual afx_msg void OnButtonGOMoreGraphics();
	virtual afx_msg void OnEditChangeHeadlineText();
	virtual afx_msg void	OnTimer(UINT);
	virtual afx_msg void	OnCommandTint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Declarations
protected :

// Implementation
protected :
	//
	// The following enumeration is used to distinguish which listbox the user is 
	// selecting from.
	void										LoadGraphicsListBox( RMBCString& rGraphics );
	void										LoadEffectsListBox( RMBCString& rEffects );

	virtual void							UpdatePreview( BOOLEAN fUpdateHeadline = TRUE, BOOLEAN fUpdateGraphic = TRUE );

	virtual YContextDataVersion		GetContextDataVersion( RUIContextData& contextData ) const;
	virtual void							WriteUIContextData( RUIContextData& contextData ) const;
	virtual void							WriteVersion2UIContextData( RUIContextData& contextData ) const;
	virtual RUIContextData*				ReadUIContextData( );
	virtual RUIContextData*				ReadVersion2UIContextData( RUIContextData& contextData );
	virtual void							StartContextDataVersion( RUIContextData& contextData, const YContextDataVersion& dataVersion ) const;

	virtual BOOLEAN						IsAdditionalGraphic( );
	virtual SearchResult					LookupSquareGraphic( uLONG uGraphicId, RCollectionSearcher<RGraphicCategorySearchCollection>* pGraphicCollectionSearcher );
	virtual void							DisplayGraphic( const SearchResult& rSelGraphic, RComponentDocument*& pGraphicDoc, uLONG& uSelectedGraphicId ) ;
	virtual void							DisplayGraphic( const SearchResult& rSelGraphic, RComponentDocument*& pGraphicDoc ) ;
	virtual void							DisplayGraphic( RComponentDocument *pNewGraphicDoc, RComponentDocument*& pGraphicDoc );
	virtual void							UpdateGraphicDocument( RComponentDocument*& pTempGraphicDoc , RComponentDocument*& pGraphicDoc );
	virtual void							CheckForMonochromeGraphic( RComponentDocument*& pGraphicDoc );
	virtual RRealRect						GetDefaultHeadlineRect( );
	virtual void							VerifyBoundingRect( YComponentBoundingRect& rBoundingRect );
	virtual void							SetDefaultGraphicBoundingRect( );

	virtual void							CreateGraphicsList( RMBCString& rList ) = 0;
	virtual void							CreateEffectsList( RMBCString& rList ) = 0;

// Members
protected :
	CComboBox								m_cFontCombo;
	CListBox									m_cGraphicsListBox;
	CListBox									m_cEffectsListBox;
	RWinColorBtn							m_cGraphicColorBtn;
	RWinColorBtn							m_cStyleColorBtn;
	CComboBox								m_cComboTint;
	CMenu										m_MenuTint;									//	Tint menu
	CEdit										m_cHeadlineEditCtrl;

	RMBCString								m_rHeadlineText;
	RMBCString								m_rFontName;

	BOOLEAN									m_fInitializing;

	RColor									m_rColorGraphic;
	RColor									m_rColorEffect;
	YFontAttributes						m_yFontAttributes;

	// A timer used in making text updates perform more smoothly
	UINT										m_uTimerID;

	RComponentDocument*					m_pGraphicDoc;
	RHeadlineInterface*					m_pHeadlineInterface;
	RGOHeadlineData						m_rHeadlineData;
	RComponentView*						m_pHeadlineView;

	// CMap is used to map graphic SearchResults to the corresponding combobox index
	CMap<uLONG, uLONG, SearchResult, SearchResult>					m_cGraphicMap;	
	CArray<SearchResult, SearchResult&>									m_cGraphicArray;
	uLONG																			m_uSelectedGraphicId;

	RCollectionArray						m_rCollectionArray;
	BOOLEAN									m_fRebuiltGraphicArrays;
#if 0	// We now use the Art Gallery for additional graphics.
	RCollectionArray						m_rSquareGraphicCollectionArray;
#endif
	RRealVectorRect						m_rGraphicRect;				//	The bounding rect of the graphic component.
	BOOLEAN									m_fBoundingRectPresent;		//	TRUE: the above bounding rect is valid.
	BOOLEAN									m_fFirstEdit;					//	TRUE: this is the first edit of this logo.
	YTint										m_nTint;
private:
	CBitmap									m_cBoldBmp;
	CBitmap									m_cItalicBmp;
	BOOLEAN									m_fBold;
	BOOLEAN									m_fItalic;
};
#endif //_COMMONGROUPEDOBJECTDIALOG_H_