// ****************************************************************************
//
//  File Name:			menu.c
//
//  Project:			Renaissance Graphic Headline Renderer
//
//  Author:				R. Hood
//
//  Description:		Shell application menu handler routines
//
//  Portability:		Macintosh specific
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.			
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /Development/HeadlineComp/MacShell/Source/MENU.C $
//	 $Author:: Richh                                             $
//	 $Date:: 6/13/97 4:07 PM                                     $
//  $Revision:: 31                                              $
//
// ****************************************************************************


#include	<Quickdraw.h>
#include	<Memory.h>
#include	<Errors.h>
#include	<Menus.h>
#include	<Events.h>
#include	<Devices.h>
#include	<Windows.h>
#include	<Resources.h>
#include	<Printing.h>
#include	<StandardFile.h>
#include	<stdio.h>

#include	"HeadlinerIncludes.h"

ASSERTNAME

#include	"GpDrawingSurface.h"
#include	"FileStream.h"
#include	"Psd3Graphic.h"
#include	"Psd3GraphicLibrary.h"
#include "HeadlineGraphic.h"
#include	"HeadlinesCanned.h"
#include	"tpsbit.h"
#include	"init.h"
#include	"event.h"
#include	"update.h"
#include	"menu.h"


/**  defines  **/
#define  rAboutDLOG						1100
#define  kOkBUTTON							1
#define  kCancelBUTTON						2
#define  kOkUserITEM							3
#define  kListUserITEM						4
#define  kReturnKEY						0x0D
#define  kEnterKEY					 	0x03
#define  kPeriodKEY           		0x2E
#define  kUpArrowKEY           		0x1E
#define  kDownArrowKEY           	0x1F
#define  kLeftArrowKEY           	0x1C
#define  kRightArrowKEY           	0x1D
#define  kTriagleSpace					  11
#define  kIndentSpace					   5
#define  kMiscSpace						   7

/**  enums  **/
enum	{	kPlaceTop = 5, kPlaceLeft, kPlacekBottom, kPlaceRight	};

/**  macros  **/
#define  TopLeft(r)					(((Point *)&(r))[0])
#define  BottomRight(r)				(((Point *)&(r))[1])
#define  GetAppFontList()			gFontList
#define  IsTrueTypeAvailable()	gfTrueTypeAvailable
#define  IsATMAvailable()  	  	gfATMAvailable

/**  local prototypes  **/
static   		void		   _HandleMDA (short);
static   		void		   _HandleMFile (short, Boolean);
static   		void		   _HandleMEdit (short);
static   		void		   _HandleAbout (void);
static   		void		   _HandleGraphic (FSSpecPtr fsFile);
static   		void		   _HandleMerge (FSSpecPtr fsFile);
static  			void  		_HandleHeadline( char* text, short* font, Boolean );
static   		void		   _HandleMScale (short);
static   		void		   _HandleMFlip (short);
static   		void		   _HandleMRotate (short);
static   		void		   _HandleMShape (short, Boolean);
static   		void		   _HandleMPath (short);
static   		void		   _HandleMEffects (short);
static   		void		   _HandleMOutline (short, Boolean);
static   		void		   _HandleMWacky (short);
static   		void		   _HandleMFont (short);
static   		Boolean		_PageSetup (void);
static  			Boolean  	_SelectGraphic( uWORD numGraphics, char** pGraphicNames, char* name );
static  			Boolean  	_SelectPlacement( short* placement );
static pascal 	void	  		_ButtonFrameDraw (DialogPtr dlogPtr, short item);
static pascal  void     	_GraphicListDraw (DialogPtr dlogPtr, short item);
static pascal  Boolean  	_GraphicListFilter (DialogPtr, EventRecord *, short *);
static pascal 	Boolean   	_GetFileFilter (ParmBlkPtr pb);
static pascal  void  		_FontPopupDraw( DialogPtr dlogPtr, short item );
static pascal  Boolean  	_HeadlineDlogFilter (DialogPtr dialogPtr, EventRecord *eventPtr, short *itemHit);
static  			void  		_CloseGraphic (void);
static  			void  		_UpdateGraphic (void);
static			MenuHandle  _CreateFontMenu (short);
static  			Boolean  	_SelectOutlineColors( uWORD flags, uWORD which, EOutlineEffects effect, BOOLEAN fBlend );
static  			EProjectionEffects	_HandleProjection( short menuItem, YPointCount* nrStages, YRealDimension* depth, RIntPoint& vanishing );
static  			EShadowEffects			_HandleShadow( short menuItem, YRealDimension* depth, RIntPoint& vanishing );
static  			Boolean  	_HandleShear( YAngle* angle );
static 			void			_GetVersionString(Str255 outVersion);
static 			StringPtr 	_ConcatPStr(Str255 ioFirstStr, ConstStr255Param	inSecondStr, short inDestSize);

/**  window structures & pointers  **/
extern  WindowPtr				gMainWindow;
extern  GDHandle			 	gMainScreen;

/**  menu structures & pointers  **/
extern  MenuHandle		   hAppleMenu;
extern  MenuHandle		   hFileMenu;
extern  MenuHandle		   hEditMenu;
extern  MenuHandle		   hScaleMenu;
extern  MenuHandle		   hFlipMenu;
extern  MenuHandle		   hRotateMenu;
extern  MenuHandle       	hShapeMenu;
extern  MenuHandle       	hPathMenu;
extern  MenuHandle    		hEffectsMenu;
extern  MenuHandle         hOutlineMenu;
extern  MenuHandle      	hWackyMenu;
extern  MenuHandle      	hFontMenu;

/**  globals  **/
extern  Boolean			  	gQuit;
extern  THPrint			  	hPrintInfo;
extern  RHeadlineGraphic*	gpHeadline;
extern  RGraphic*				gpGraphic;
extern  RPath*					gpWarpPath;
extern  RGradientFill*		gpGradientBackdrop;
extern  Handle				 	ghbitGraphic;
extern  short				 	gScaleMenuItem;
extern  short				 	gFlipMenuItem;
extern  float				 	gRotateDegrees;
extern  EFlipDirection		gFlipDirection;
extern  BOOLEAN  			 	gfUseOffScreen;
extern  RFontList          gFontList;
extern  int                gnFontChoices;
extern  YFontInfo          gpFontChoices[];
extern  RIntSize				gScroll;

/**  locals  **/
static  short				 	_RotateMenuItem = 1;
static  Str63					_stGraphicName;
static  Str63					_stLibraryName;
static  ListHandle         _hGraphicListHdl = nil;
static  Rect               _rcGraphicListBox;
static  Boolean            _fOkButtonInActive = false;
static  short				 	_ShapeMenuItem = 0;
static  short				 	_PathMenuItem = 0;
static  short				 	_WackyMenuItem = 0;
static  short				 	_FontMenuItem = 0;
static  short				 	_OutlineMenuItem = 0;
static  short				 	_StackMenuItem = 0;
static  short				 	_ExtrusionMenuItem = 0;
static  short				 	_ShadowMenuItem = 0;
static  short				 	_FramedMenuItem = 0;
static  MenuHandle			_hFontMenuPopup = nil;
static  short					_sFontMenuItem = 0;
static  short					_sFontMenuWidth = 0;
//static  RFontList				_appFontList = GetAppFontList();
static  short				 	_sOldShapeIdMap[17] = { 0, 0, 0,
										kWarpShape1, kWarpShape2, kWarpShape3, kWarpShape4, kWarpShape5, kWarpShape6, kWarpShape7, kWarpShape8,
										kWarpShape9, kWarpShape10, kWarpShape11, kWarpShape12, kWarpShape13, kWarpShape14 };
static  short				 	_sHorzShapeIdMap[54] = { 0, 0, 0,
										kWarpShape110, kWarpShape120, kWarpShape130, kWarpShape140, kWarpShape150, kWarpShape160, kWarpShape170, kWarpShape180,
										kWarpShape210, kWarpShape220, kWarpShape230, kWarpShape240, kWarpShape250, kWarpShape260, kWarpShape270, kWarpShape280, 
										kWarpShape310, kWarpShape320, kWarpShape330, kWarpShape340, kWarpShape350, kWarpShape360, kWarpShape370, kWarpShape380, 
										kWarpShape410, kWarpShape420, kWarpShape430, kWarpShape440, kWarpShape450, kWarpShape460, kWarpShape470, kWarpShape480, 
										kWarpShape510, kWarpShape520, kWarpShape530, kWarpShape540, kWarpShape550, kWarpShape560, kWarpShape570, kWarpShape580, 
										kWarpShape610, kWarpShape620, kWarpShape630, kWarpShape640, kWarpShape650, kWarpShape660, kWarpShape670, kWarpShape680, 
										kWarpShape710, kWarpShape720, kWarpShape730 };
static  short				 	_sVertShapeIdMap[22] = { 0, 0, 0,
										kWarpShape810, kWarpShape820, kWarpShape830, kWarpShape840, kWarpShape850, kWarpShape860, 
										kWarpShape910, kWarpShape920, kWarpShape930, kWarpShape940, kWarpShape950, kWarpShape960, kWarpShape970, 
										kWarpShape1010, kWarpShape1020, kWarpShape1030, kWarpShape1040, kWarpShape1050, kWarpShape1060 };


/* ----------------------------------------------------------------------------
   HandleMenu 
   Process mouse down in menu bar. 
   Returns nothing.										
   ---------------------------------------------------------------------------- */
void  HandleMenu( long menuChoice, short modifiers )
{
short			theMenu, theItem;
WindowPtr	whichWindow;

	if ( menuChoice == 0 ) 
	   return;
	FlushEvents( everyEvent, 0 ); 
	theItem = menuChoice;
	theMenu = (menuChoice >> 16);
	switch ( theMenu ) 
	{
	   case kAppleMENU:
		  _HandleMDA( theItem );
		  break;
				
	   case kFileMENU:
		  _HandleMFile( theItem, ((modifiers & optionKey) != 0) );
		  break;

	   case kEditMENU:
		  whichWindow = FrontWindow();
		  if ( whichWindow != nil && ((WindowPeek)whichWindow)->windowKind < 0 )  
			 SystemEdit( theItem - 1 );
		  else  
			 _HandleMEdit( theItem );
		  break;

	   case kScaleMENU:
		  _HandleMScale( theItem );
		  break;

	   case kFlipMENU:
		  _HandleMFlip( theItem );
		  break;

	   case kRotateMENU:
		  _HandleMRotate( theItem );
		  break;

	   case kShapeMENU:
		  _HandleMShape( theItem, ((modifiers & optionKey) != 0) );
		  break;

	   case kPathMENU:
		  _HandleMPath( theItem );
		  break;

	   case kEffectsMENU:
		  _HandleMEffects( theItem );
		  break;

	   case kOutlineMENU:
		  _HandleMOutline( theItem, ((modifiers & optionKey) != 0) );
		  break;

	   case kWackyMENU:
		  _HandleMWacky( theItem );
		  break;

	   case kFontMENU:
		  _HandleMFont( theItem );
		  break;

	   default:
		  break;
	}
	HiliteMenu( 0 );
}


/* ----------------------------------------------------------------------------
   _HandleMDA 
   Process Apple menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMDA (short theItem)
{
GrafPtr			  	savePort;
unsigned char		theString[256];
short					accNumber;

	if (theItem == kAboutAMENU) 
	   _HandleAbout();
	else 
	{
	   GetItem(hAppleMenu, theItem, theString);
	   GetPort(&savePort);
	   accNumber = OpenDeskAcc(theString);
	   SetPort(savePort);
	}
}


/* ----------------------------------------------------------------------------
   _HandleMFile 
   Process File menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMFile (short menuItem, Boolean fOption)
{
OSType					types[12]; 
StandardFileReply  	reply;
char				 		str[256];
short						font;
RGpDrawingSurface		drawingSurface;

	switch ( menuItem ) 
	{
		case kNewFMENU:
			*str = 0;
			font = 0;
			*_stGraphicName = 0;
			gScroll.m_dx = gScroll.m_dy = 0;
			_HandleHeadline( str, &font, fOption );
		 	if ( font == 0 || *str == 0 )
			  _CloseGraphic();
			else 
		  	{
				delete gpHeadline;						// free the old graphic
				gpHeadline = new RHeadlineGraphic;
				if ( gpHeadline != nil )
				{
					p2cstr( (StringPtr)str );
				  	if ( gpHeadline->SetText( RMBCString( str ) ) )
					{
						gpHeadline->SetFont( gFontList.GetRFont( font - 1 ) );
						_FontMenuItem = font;
					}
				  	SetWTitle( gMainWindow, "\pHeadline" );
					CheckItem( hScaleMenu, gScaleMenuItem, false );
				   gScaleMenuItem = k10SMENU;
					CheckItem( hScaleMenu, gScaleMenuItem, true );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
				}
        	}
		  	break;

	   case kOpenFMENU:
		  types[0] = 'pAUT';
		  types[1] = 'pBKG';
		  types[2] = 'pCLG';
		  types[3] = 'pKLG';
		  types[4] = 'pRWG';
		  types[5] = 'pSEL';
		  types[6] = 'pSPT';
		  types[7] = 'pSQG';
		  types[8] = 'pTMP';
		  types[9] = 'pBOR';
		  types[10] = 'pRUL';
		  StandardGetFile( nil, 11, types, &reply );
	     gScroll.m_dx = gScroll.m_dy = 0;
    	  SetPort( gMainWindow );												// application is SDI 
		  if ( reply.sfGood )
				if ( gpGraphic != nil || gpHeadline != nil ) 
			 	 	_HandleMerge( &reply.sfFile );
				else
			  {
					*_stGraphicName = 0;
					BlockMove( reply.sfFile.name, _stLibraryName, ((Size)(*reply.sfFile.name) + 1) ); 
				  	_HandleGraphic( &reply.sfFile );
					if ( *_stLibraryName == 0 || *_stGraphicName == 0 || gpGraphic == nil )
						BlockMove( "\pImager", str, 7 ); 
					else
					{
						BlockMove( _stLibraryName, str, ((Size)(*_stLibraryName) + 1) ); 
						BlockMove( " : ", (str + *str + 1), 3 ); 
						*str += 3;
						BlockMove( (_stGraphicName + 1), (str + *str + 1), (Size)*_stGraphicName ); 
						*str += *_stGraphicName;
					}
			  		SetWTitle( gMainWindow, (StringPtr)str );
	        }
		  break;
		  
	   case kCloseFMENU:
		  _CloseGraphic();
		  if ( _FontMenuItem > 0 )
		  {
			  CheckItem( hFontMenu, _FontMenuItem, false );
			  _FontMenuItem = 0;
			}
		  break;
		  
	   case kPageFMENU:
		  _PageSetup();
    	  SetPort( gMainWindow );												// application is SDI 
		  break;
				
	   case kPrintFMENU:
		  if ( gMainWindow != nil )
		  {
			 	PrintMainWindow();
    			SetPort( gMainWindow );											// application is SDI 
		  }
		  break;
				
	   case kQuitFMENU:
		  gQuit = true;
		  break;
		  
	   default:
		  break;
	}
	if ( !gQuit )
	{
		AdjustMenus();
		_UpdateGraphic();
	}
}


/* ----------------------------------------------------------------------------
   _HandleMEdit 
   Process Edit menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMEdit (short menuItem)
{
	switch (menuItem) 
	{
	   case kUndoEMENU:
		  break;
		  
	   case kCutEMENU:
		  if ( gMainWindow != nil && gpGraphic != nil )
		  {
		  	 DrawGraphicToPict( gMainWindow );
			 _CloseGraphic();
		  }
		  break;
		  
	   case kCopyEMENU:
		  if ( gMainWindow != nil && gpGraphic != nil )
		  	 DrawGraphicToPict( gMainWindow );
		  break;
		  
	   case kPasteEMENU:
		  break;
		  
	   case kClearEMENU:
		  if ( gMainWindow != nil && gpGraphic != nil )
			  _CloseGraphic();
		  break;
		  
	   default:
		  break;
	}
	AdjustMenus();
}


#define kVersionItem	6

/* ----------------------------------------------------------------------------
   _HandleAbout 
   Routine to display the about box.	
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleAbout()
{
GrafPtr			  	holdPort;
DialogPtr			aboutDialog;
Str255				vers;
short					itemhit;
Handle			   itemHdl;
short					itemType;
Rect				 	rect;
UserItemUPP		  	frameDrawProc = NewUserItemProc(_ButtonFrameDraw);
   
	GetPort(&holdPort);   
	ShowCursor();
	SetCursor(&qd.arrow);
	if ((aboutDialog = GetNewDialog(rAboutDLOG, nil, (WindowPtr)-1)) != nil)
	{
	   SetPort(aboutDialog);

		// Put the current version and executable type (PPC or 68k) in the about box
		_GetVersionString(vers);
#ifdef powerc
		_ConcatPStr(vers, "\p PPC", sizeof(Str255));
#else
		_ConcatPStr(vers, "\p 68k", sizeof(Str255));
#endif	// powerc
	   GetDItem(aboutDialog, kVersionItem, &itemType, &itemHdl, &rect);
	   SetDialogItemText(itemHdl, vers);

	   GetDItem(aboutDialog, kOkUserITEM, &itemType, &itemHdl, &rect);
	   if (itemType == userItem || itemType == itemDisable)   
		  SetDItem(aboutDialog, kOkUserITEM, itemType, (Handle)frameDrawProc, &rect);
	   ShowWindow(aboutDialog);
	   itemhit = 0;
	   while (itemhit != kOkBUTTON)
		 ModalDialog(nil, &itemhit);
	   DisposDialog(aboutDialog);
	}
	DisposeRoutineDescriptor(frameDrawProc);
	ObscureCursor();
	SetPort(holdPort);
}


/* ----------------------------------------------------------------------------
   _HandleGraphic 
	  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleGraphic( FSSpecPtr fsFile )
{
RFileStream 			stream;
RPsd3GraphicLibrary	library;
char				 		str[256];
uWORD 					numGraphics;
char** 					pGraphicNames;
   
	try
	{
		stream.Open( fsFile, kRead );
		if ( library.OpenLibrary( stream ) )
		{						
			pGraphicNames = library.GetGraphicNames( numGraphics );
			if ( _SelectGraphic( numGraphics, pGraphicNames, str ) )
			{
				BlockMove( str, (_stGraphicName + 1), (Size)strlen( str ) );
		 		*_stGraphicName = strlen( str );
				delete gpGraphic;						// free the old graphic
				gpGraphic = dynamic_cast<RGraphic*>(library.GetGraphic( stream, str ));
				if ( ghbitGraphic != nil )
					tbitFree( ghbitGraphic );
				ghbitGraphic = nil;
			}
		}
	}
	catch ( YException except )
	{
		switch ( except ) 
		{
			case kMemory:
				StopAlert( rLowMemoryALRT, nil );									// out of memeory
				break;
			default:
				AlertOnError( stopIcon, 17000, false );							// generic application file error
				break;
		}
	}
	catch ( ... )
	{
		AlertOnError( stopIcon, 19001, false );									// should not happen
	}
}


/* ----------------------------------------------------------------------------
   _HandleMerge 
	  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMerge( FSSpecPtr fsFile )
{
RFileStream 			stream;
RPsd3GraphicLibrary	library;
char				 		str[256];
uWORD 					numGraphics;
char** 					pGraphicNames;
short						placement;
R2dTransform			transform;  
RIntSize					graphicSize;
RGraphic*				pGraphic1 = ( ( gpGraphic != nil ) ? (RGraphic*)gpGraphic : (RGraphic*)gpHeadline );
RGraphic*				pGraphic2 = nil;

	TpsAssert(FALSE, "This is UNTESTED code, you may crash!");

	if ( pGraphic1 == nil )  
		return;
	try
	{
		stream.Open( fsFile, kRead );
		library.OpenLibrary( stream );
		pGraphicNames = library.GetGraphicNames( numGraphics );
		if ( _SelectGraphic( numGraphics, pGraphicNames, str ) )
			if ( _SelectPlacement( &placement ) )
			{
				pGraphic2 = (RGraphic *)library.GetGraphic( stream, str );
				if ( pGraphic2 != nil )
				{
					switch ( placement ) 
					{
						case kPlaceTop:
							graphicSize = ((RSingleGraphic*)pGraphic2)->GetGraphicSize();
							transform.PreTranslate( 0, -graphicSize.m_dy );
							break;
					   case kPlaceLeft:
							graphicSize = ((RSingleGraphic*)pGraphic2)->GetGraphicSize();
							transform.PreTranslate( -graphicSize.m_dx, 0 );
						  	break;
					   case kPlacekBottom:
							graphicSize = ((RSingleGraphic*)pGraphic1)->GetGraphicSize();
							transform.PreTranslate( 0, graphicSize.m_dy );
						  	break;
					   case kPlaceRight:
							graphicSize = ((RSingleGraphic*)pGraphic1)->GetGraphicSize();
							transform.PreTranslate( graphicSize.m_dx, 0 );
						  	break;
					   default:
						  break;
					}
					TpsAssertValid(pGraphic2);//((RSingleGraphic*)pGraphic2)->Validate();
					((RSingleGraphic*)pGraphic2)->MergeWith( transform, (*(RSingleGraphic*)pGraphic1) );
				}
				delete pGraphic2;
			}
	}
	catch ( YException except )
	{
		switch ( except ) 
		{
			case kMemory:
				StopAlert( rLowMemoryALRT, nil );									// out of memeory
				break;
			default:
				AlertOnError( stopIcon, 17000, false );							// generic application file error
				break;
		}
	}
	catch ( ... )
	{
		AlertOnError( stopIcon, 19001, false );									// should not happen
	}
	FillRect( &(gMainWindow->portRect), &qd.white );
	if ( ghbitGraphic != nil )
		tbitFree( ghbitGraphic );
	ghbitGraphic = nil;
	_UpdateGraphic();
}


/* ----------------------------------------------------------------------------
   _SelectGraphic 
	Bring up a dialog with a list of graphics in the library.
   Returns boolean indicating a graphic was selected.
   ---------------------------------------------------------------------------- */
static  Boolean  _SelectGraphic( uWORD numGraphics, char** pGraphicNames, char* name )
{
GrafPtr			  		holdPort;
DialogPtr				pDialog;
short						itemhit;
Handle			   	itemHdl;
short						itemType;
Rect				 		itemBox;
UserItemUPP		  		frameDrawProc, listDrawProc;
ModalFilterUPP		  	dialogFilterProc;
Point                cellSize;
Cell                 cell;
Rect                 dataBounds;
char				 		*str;
short						length;
BOOLEAN  			 	fUseOffScreen = false;
   
	// create dialog
	GetPort( &holdPort );   
	ShowCursor();
	SetCursor( &qd.arrow );
	if ( (pDialog = GetNewDialog( 5101, nil, (WindowPtr)-1) ) == nil )
		return false;
	SetPort( pDialog );
	frameDrawProc = NewUserItemProc(_ButtonFrameDraw);
	GetDItem( pDialog, kOkUserITEM, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, kOkUserITEM, itemType, (Handle)frameDrawProc, &itemBox );

	// create graphic list
	cellSize.h = cellSize.v = 0;
	SetRect( &dataBounds, 0, 0, 1, 0 );
	listDrawProc = NewUserItemProc(_GraphicListDraw);
	GetDItem( pDialog, kListUserITEM, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, kListUserITEM, itemType, (Handle)listDrawProc, &itemBox );
	itemBox.right = itemBox.right - 15;
	_hGraphicListHdl = LNew( &itemBox, &dataBounds, cellSize, 0, pDialog, false, false, false, true );
	(**_hGraphicListHdl).selFlags = lOnlyOne;  

	// populate graphic list 
	SetPt( &cell, 0, 0 );
	for ( int i = 0; i < numGraphics; i++ ) 
	{
		str = pGraphicNames[i];
		cell.v = LAddRow( 1, 8192, _hGraphicListHdl );
		LSetCell( str, strlen( str ), cell, _hGraphicListHdl );
	}
	LActivate( true, _hGraphicListHdl );
	LDoDraw( true, _hGraphicListHdl );

	// process dialog
	ShowWindow( pDialog );
	itemhit = 0;
	dialogFilterProc = NewModalFilterProc(_GraphicListFilter);
	while ( itemhit != kOkBUTTON && itemhit != kCancelBUTTON )
	{
    	GetDItem( pDialog, kOkBUTTON, &itemType, &itemHdl, &itemBox);
    	SetPt( &cell, 0, 0);
    	if (LGetSelect( true, &cell, _hGraphicListHdl) )
			_fOkButtonInActive = false;
    	else
			_fOkButtonInActive = true;
      HiliteControl( (ControlHandle)itemHdl, ( _fOkButtonInActive ? 255 : 0 ) );
		ModalDialog( dialogFilterProc, &itemhit );
		if ( itemhit == kOkBUTTON )
		{
    		SetPt( &cell, 0, 0 );
    		if ( LGetSelect( true, &cell, _hGraphicListHdl ) )
			{
				length = 255;
				LGetCell( name, &length, cell, _hGraphicListHdl);
				*(name + length) = 0;
				gfUseOffScreen = fUseOffScreen;
			}
			else
				itemhit = kCancelBUTTON;
		}
		else if ( itemhit == 6 )
		{
			GetDItem( pDialog, 6, &itemType, &itemHdl, &itemBox );
			SetCtlValue ((ControlHandle)itemHdl, ( fUseOffScreen ? 0 : 1 ) );
			fUseOffScreen = !fUseOffScreen;
		}
	}

	// cleanup
	LDispose( _hGraphicListHdl );  
	DisposDialog( pDialog );
	DisposeRoutineDescriptor( frameDrawProc );
	DisposeRoutineDescriptor( listDrawProc );
	DisposeRoutineDescriptor( dialogFilterProc );
	ObscureCursor();
	SetPort( holdPort );

	return( itemhit == kOkBUTTON );
}


/* ----------------------------------------------------------------------------
   _SelectGraphic 
	Bring up a dialog with 
   Returns boolean indicating a graphic was selected.
   ---------------------------------------------------------------------------- */
static  Boolean  _SelectPlacement( short* placement )
{
GrafPtr			  		holdPort;
DialogPtr				pDialog;
short						itemhit;
Handle			   	itemHdl;
short						itemType;
Rect				 		itemBox;
UserItemUPP		  		frameDrawProc;
short						place = kPlaceRight;
   
	// create dialog
	GetPort( &holdPort );   
	ShowCursor();
	SetCursor( &qd.arrow );
	if ( (pDialog = GetNewDialog( 5102, nil, (WindowPtr)-1) ) == nil )
		return false;
	SetPort( pDialog );
	frameDrawProc = NewUserItemProc( _ButtonFrameDraw );
	GetDItem( pDialog, kOkUserITEM, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, kOkUserITEM, itemType, (Handle)frameDrawProc, &itemBox );

	// init
//	GetDItem( pDialog, kPlaceTop, &itemType, &itemHdl, &itemBox);
//	HiliteControl( (ControlHandle)itemHdl, 255 );
//	GetDItem( pDialog, kPlaceLeft, &itemType, &itemHdl, &itemBox);
//	HiliteControl( (ControlHandle)itemHdl, 255 );
	GetDItem( pDialog, kPlaceRight, &itemType, &itemHdl, &itemBox );
	SetCtlValue( (ControlHandle)itemHdl, 1 );

	// process dialog
	ShowWindow( pDialog );
	itemhit = 0;
	while ( itemhit != kOkBUTTON && itemhit != kCancelBUTTON )
	{
		ModalDialog( nil, &itemhit );
		switch ( itemhit ) 
		{
			case kOkBUTTON:
				*placement = place;
				break;
		   case kPlaceTop:
		   case kPlaceLeft:
		   case kPlaceRight:
		   case kPlacekBottom:
				place = itemhit;
			  	break;
			 	break;
		   default:
			  break;
		}
		GetDItem( pDialog, kPlaceTop, &itemType, &itemHdl, &itemBox );
		SetCtlValue( (ControlHandle)itemHdl, ( (kPlaceTop == place) ? 1 : 0 ) );
		GetDItem( pDialog, kPlaceLeft, &itemType, &itemHdl, &itemBox );
		SetCtlValue( (ControlHandle)itemHdl, ( (kPlaceLeft == place) ? 1 : 0 ) );
		GetDItem( pDialog, kPlacekBottom, &itemType, &itemHdl, &itemBox );
		SetCtlValue( (ControlHandle)itemHdl, ( (kPlacekBottom == place) ? 1 : 0 ) );
		GetDItem( pDialog, kPlaceRight, &itemType, &itemHdl, &itemBox );
		SetCtlValue( (ControlHandle)itemHdl, ( (kPlaceRight == place) ? 1 : 0 ) );
	}

	// cleanup
	DisposDialog( pDialog );
	DisposeRoutineDescriptor( frameDrawProc );
	ObscureCursor();
	SetPort( holdPort );

	return( itemhit == kOkBUTTON );
}


/* ----------------------------------------------------------------------------
   _HandleHeadline
	Bring up a dialog with 
   Nothing.
   ---------------------------------------------------------------------------- */
static  void  _HandleHeadline( char* text, short* font, Boolean fAltString )
{
GrafPtr			  		holdPort;
DialogPtr				pDialog;
short						itemhit;
Handle			   	itemHdl;
short						itemType;
Rect				 		itemBox;
UserItemUPP		  		frameDrawProc, menuDrawProc;
ModalFilterUPP		  	dialogFilterProc;
BOOLEAN  			 	fUseOffScreen = false;

	// create dialog
	GetPort( &holdPort );   
	ShowCursor();
	SetCursor( &qd.arrow );
	if ( (pDialog = GetNewDialog( 5103, nil, (WindowPtr)-1) ) == nil )
		return;
	SetPort( pDialog );
	frameDrawProc = NewUserItemProc( _ButtonFrameDraw );
	GetDItem( pDialog, kOkUserITEM, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, kOkUserITEM, itemType, (Handle)frameDrawProc, &itemBox );

	// setup font popup menu
	_sFontMenuItem = 10;
	if ( (_hFontMenuPopup = _CreateFontMenu(104)) != nil )
	{
		CalcMenuSize( _hFontMenuPopup );
		_sFontMenuWidth = (**_hFontMenuPopup).menuWidth;
	}
	menuDrawProc = NewUserItemProc( _FontPopupDraw );
	GetDItem( pDialog, 7, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, 7, itemType, (Handle)menuDrawProc, &itemBox );

	// process dialog
	GetDItem( pDialog, 4, &itemType, &itemHdl, &itemBox );
	if ( fAltString )
		SetIText(itemHdl, "\pABCDEFGHI" );
	SelIText( pDialog, 4, 0, 32767 );
	ShowWindow( pDialog );
	itemhit = 0;
	_fOkButtonInActive = false;
	dialogFilterProc = NewModalFilterProc( _HeadlineDlogFilter );
	while ( itemhit != kOkBUTTON && itemhit != kCancelBUTTON )
	{
		ModalDialog( dialogFilterProc, &itemhit );
		if ( itemhit == kOkBUTTON )
		{
			GetDItem( pDialog, 4, &itemType, &itemHdl, &itemBox );
			if ( itemType == editText )   
				GetIText( itemHdl, (StringPtr)text );
			*font = _sFontMenuItem;
			BlockMove( "\pTest", _stGraphicName, 5 ); 
		}
		else if ( itemhit == 6 )
		{
			GetDItem( pDialog, 6, &itemType, &itemHdl, &itemBox );
			SetCtlValue ((ControlHandle)itemHdl, ( fUseOffScreen ? 0 : 1 ) );
			fUseOffScreen = !fUseOffScreen;
		}
	}

	// cleanup
	DeleteMenu( 104 );
	if ( _hFontMenuPopup != nil )
		DisposeHandle( (Handle)_hFontMenuPopup );
	_hFontMenuPopup = nil;
	DisposDialog( pDialog );
	DisposeRoutineDescriptor( frameDrawProc );
	DisposeRoutineDescriptor( menuDrawProc );
	DisposeRoutineDescriptor( dialogFilterProc );
	ObscureCursor();
	SetPort( holdPort );
}


/* ----------------------------------------------------------------------------
   _SelectOutlineColors 
	Bring up a dialog with radio buttons to select colors for a headline outline.
   Returns boolean indicating colors were selected.
   ---------------------------------------------------------------------------- */
static  Boolean  _SelectOutlineColors( uWORD flags, uWORD which, EOutlineEffects effect, BOOLEAN fBlend )
{
GrafPtr			  		holdPort;
DialogPtr				pDialog;
short						itemhit;
Handle			   	itemHdl;
short						itemType;
Rect				 		itemBox;
UserItemUPP		  		frameDrawProc;
int						i, item;
RSolidColor				strokeColor( kBlack );
RSolidColor				fillColor( kBlack );
RSolidColor				hiliteColor( kBlack );
RSolidColor				shadowColor( kBlack );
RHeadlineColor 		fill;
   
	// create dialog
	GetPort( &holdPort );   
	ShowCursor();
	SetCursor( &qd.arrow );
	if ( (pDialog = GetNewDialog( 5105, nil, (WindowPtr)-1) ) == nil )
		return false;
	SetPort( pDialog );
	frameDrawProc = NewUserItemProc( _ButtonFrameDraw );
	GetDItem( pDialog, kOkUserITEM, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, kOkUserITEM, itemType, (Handle)frameDrawProc, &itemBox );
	if ( ( which & 0x01 ) != 0 )
		ParamText( "\pStroke", "\p", "\p", "\pOutline" );
	else if ( ( which & 0x02 ) != 0 )
		ParamText( "\pBackground", "\p", "\p", "\pFrame" );
	else if ( ( which & 0x04 ) != 0 )
		ParamText( "\pFill", "\p", "\p", "\pBlend" );
	else
		ParamText( "\p", "\p", "\p", "\p" );

	// turn off unneeded colors
	if ( ( flags & 0x01 ) == 0 )
	{
		for ( i = 5; i < 16; i++ )
		{
			GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
  	    	HiliteControl( (ControlHandle)itemHdl, 255 );
		}
	}
	else
	{
		GetDItem( pDialog, 5, &itemType, &itemHdl, &itemBox );
		SetCtlValue((ControlHandle)itemHdl, 1 );
	}
	if ( ( flags & 0x02 ) == 0 )
	{
		for ( i = 17; i < 28; i++ )
		{
			GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
  	    	HiliteControl( (ControlHandle)itemHdl, 255 );
		}
	}
	else
	{
		GetDItem( pDialog, 20, &itemType, &itemHdl, &itemBox );
		SetCtlValue((ControlHandle)itemHdl, 1 );
	}
	if ( ( flags & 0x04 ) == 0 )
	{
		for ( i = 29; i < 40; i++ )
		{
			GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
  	    	HiliteControl( (ControlHandle)itemHdl, 255 );
		}
	}
	else
	{
		GetDItem( pDialog, 38, &itemType, &itemHdl, &itemBox );
		SetCtlValue((ControlHandle)itemHdl, 1 );
	}
	if ( ( flags & 0x08 ) == 0 )
	{
		for ( i = 41; i < 52; i++ )
		{
			GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
  	    	HiliteControl( (ControlHandle)itemHdl, 255 );
		}
	}
	else
	{
		GetDItem( pDialog, 51, &itemType, &itemHdl, &itemBox );
		SetCtlValue((ControlHandle)itemHdl, 1 );
	}

	// process dialog
	ShowWindow( pDialog );
	itemhit = 0;
	while ( itemhit != kOkBUTTON && itemhit != kCancelBUTTON )
	{
		ModalDialog( nil, &itemhit );
		switch ( itemhit ) 
		{
		   case 5 :
		   case 6 :
		   case 7 :
		   case 8 :
		   case 9 :
		   case 10 :
		   case 11 :
		   case 12 :
		   case 13 :
		   case 14 :
		   case 15 :
				for ( i = 5; i < 16; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					SetCtlValue((ControlHandle)itemHdl, 0 );
				}
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, 1 );
			  	break;
		   case 17 :
		   case 18 :
		   case 19 :
		   case 20 :
		   case 21 :
		   case 22 :
		   case 23 :
		   case 24 :
		   case 25 :
		   case 26 :
		   case 27 :
				for ( i = 17; i < 28; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					SetCtlValue((ControlHandle)itemHdl, 0 );
				}
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, 1 );
			  	break;
		   case 29 :
		   case 30 :
		   case 31 :
		   case 32 :
		   case 33 :
		   case 34 :
		   case 35 :
		   case 36 :
		   case 37 :
		   case 38 :
		   case 39 :
				for ( i = 29; i < 40; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					SetCtlValue((ControlHandle)itemHdl, 0 );
				}
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, 1 );
			  	break;
		   case 41 :
		   case 42 :
		   case 43 :
		   case 44 :
		   case 45 :
		   case 46 :
		   case 47 :
		   case 48 :
		   case 49 :
		   case 50 :
		   case 51 :
				for ( i = 41; i < 52; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					SetCtlValue((ControlHandle)itemHdl, 0 );
				}
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, 1 );
			  	break;
		   case kOkBUTTON :
				item = 0;
				for ( i = 5; i < 16; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					if ( GetCtlValue( (ControlHandle)itemHdl ) != 0 )
						item = i;
				}
				switch ( item ) 
				{
				   case 5 :
						strokeColor = ( RSolidColor( kBlack ) );
					  	break;
				   case 6 :
						strokeColor = ( RSolidColor( kYellow ) );
					  	break;
				   case 7 :
						strokeColor = ( RSolidColor( kMagenta ) );
					  	break;
				   case 8 :
						strokeColor = ( RSolidColor( kRed ) );
					  	break;
				   case 9 :
						strokeColor = ( RSolidColor( kCyan ) );
					  	break;
				   case 10 :
						strokeColor = ( RSolidColor( kGreen ) );
					  	break;
				   case 11 :
						strokeColor = ( RSolidColor( kBlue ) );
					  	break;
				   case 12 :
						strokeColor = ( RSolidColor( kWhite ) );
					  	break;
				   case 13 :
						strokeColor = ( RSolidColor( kGray ) );
					  	break;
				   case 14 :
						strokeColor = ( RSolidColor( kLightGray ) );
					  	break;
				   case 15 :
						strokeColor = ( RSolidColor( kDarkGray ) );
					  	break;
				   default:
					  	break;
				}
				if ( item != 0 )
					gpHeadline->SetOutlineStroke( strokeColor );
				item = 0;
				for ( i = 17; i < 28; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					if ( GetCtlValue( (ControlHandle)itemHdl ) != 0 )
						item = i;
				}
				switch ( item ) 
				{
				   case 17 :
						fillColor = ( RSolidColor( kBlack ) );
					  	break;
				   case 18 :
						fillColor = ( RSolidColor( kYellow ) );
					  	break;
				   case 19 :
						fillColor = ( RSolidColor( kMagenta ) );
					  	break;
				   case 20 :
						fillColor = ( RSolidColor( kRed ) );
					  	break;
				   case 21 :
						fillColor = ( RSolidColor( kCyan ) );
					  	break;
				   case 22 :
						fillColor = ( RSolidColor( kGreen ) );
					  	break;
				   case 23 :
						fillColor = ( RSolidColor( kBlue ) );
					  	break;
				   case 24 :
						fillColor = ( RSolidColor( kWhite ) );
					  	break;
				   case 25 :
						fillColor = ( RSolidColor( kGray ) );
					  	break;
				   case 26 :
						fillColor = ( RSolidColor( kLightGray ) );
					  	break;
				   case 27 :
						fillColor = ( RSolidColor( kDarkGray ) );
					  	break;
				   default:
					  	break;
				}
				if ( item != 0 )
				{
					if ( fBlend )
						fill = RHeadlineColor( fillColor, strokeColor, kEast );
					else
						fill = RHeadlineColor( fillColor );
					gpHeadline->SetOutlineFill( effect, fill );
				}
				item = 0;
				for ( i = 29; i < 40; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					if ( GetCtlValue( (ControlHandle)itemHdl ) != 0 )
						item = i;
				}
				switch ( item ) 
				{
				   case 29 :
						hiliteColor = ( RSolidColor( kBlack ) );
					  	break;
				   case 30 :
						hiliteColor = ( RSolidColor( kYellow ) );
					  	break;
				   case 31 :
						hiliteColor = ( RSolidColor( kMagenta ) );
					  	break;
				   case 32 :
						hiliteColor = ( RSolidColor( kRed ) );
					  	break;
				   case 33 :
						hiliteColor = ( RSolidColor( kCyan ) );
					  	break;
				   case 34 :
						hiliteColor = ( RSolidColor( kGreen ) );
					  	break;
				   case 35 :
						hiliteColor = ( RSolidColor( kBlue ) );
					  	break;
				   case 36 :
						hiliteColor = ( RSolidColor( kWhite ) );
					  	break;
				   case 37 :
						hiliteColor = ( RSolidColor( kGray ) );
					  	break;
				   case 38 :
						hiliteColor = ( RSolidColor( kLightGray ) );
					  	break;
				   case 39 :
						hiliteColor = ( RSolidColor( kDarkGray ) );
					  	break;
				   default:
					  	break;
				}
				if ( item != 0 )
					gpHeadline->SetOutlineHilite( hiliteColor );
				item = 0;
				for ( i = 41; i < 52; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					if ( GetCtlValue( (ControlHandle)itemHdl ) != 0 )
						item = i;
				}
				switch ( item ) 
				{
				   case 41 :
						shadowColor = ( RSolidColor( kBlack ) );
					  	break;
				   case 42 :
						shadowColor = ( RSolidColor( kYellow ) );
					  	break;
				   case 43 :
						shadowColor = ( RSolidColor( kMagenta ) );
					  	break;
				   case 44 :
						shadowColor = ( RSolidColor( kRed ) );
					  	break;
				   case 45 :
						shadowColor = ( RSolidColor( kCyan ) );
					  	break;
				   case 46 :
						shadowColor = ( RSolidColor( kGreen ) );
					  	break;
				   case 47 :
						shadowColor = ( RSolidColor( kBlue ) );
					  	break;
				   case 48 :
						shadowColor = ( RSolidColor( kWhite ) );
					  	break;
				   case 49 :
						shadowColor = ( RSolidColor( kGray ) );
					  	break;
				   case 50 :
						shadowColor = ( RSolidColor( kLightGray ) );
					  	break;
				   case 51 :
						shadowColor = ( RSolidColor( kDarkGray ) );
					  	break;
				   default:
					  	break;
				}
				if ( item != 0 )
					gpHeadline->SetOutlineShadow( shadowColor );
			  	break;
		   default:
			  	break;
		}
	}

	// cleanup
	DisposDialog( pDialog );
	DisposeRoutineDescriptor( frameDrawProc );
	ObscureCursor();
	SetPort( holdPort );

	return( itemhit == kOkBUTTON );
}


/* ----------------------------------------------------------------------------
   _HandleMScale 
   Process Scale menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMScale ( short menuItem )
{
	if ( gScaleMenuItem != menuItem )
	{
		FillRect( &(gMainWindow->portRect), &qd.white );
		if ( ghbitGraphic != nil )
			tbitFree ( ghbitGraphic );
		ghbitGraphic = nil;
	}
	CheckItem( hScaleMenu, gScaleMenuItem, false );
	switch ( menuItem ) 
	{
	   case k10SMENU:
	   case k25SMENU:
	   case k50SMENU:
	   case k75SMENU:
	   case k100SMENU:
	   case k150SMENU:
	   case k200SMENU:
	   case k300SMENU:
	   case k500SMENU:
		  gScaleMenuItem = menuItem;
		  break;
	   default:
		  gScaleMenuItem = k100SMENU;
		  break;
	}
	CheckItem( hScaleMenu, gScaleMenuItem, true );
	if ( gpHeadline != nil )
	{
		RGpDrawingSurface	drawingSurface;
		RRealSize			rSize = CalcDefaultHeadlineScreenSize();
		gpHeadline->Construct( rSize, drawingSurface );
	}
	_UpdateGraphic();
}

/* ----------------------------------------------------------------------------
   _HandleMFlip 
   Process Flip menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMFlip ( short menuItem )
{
	if ( gFlipMenuItem != menuItem )
	{
		FillRect( &(gMainWindow->portRect), &qd.white );
		if ( ghbitGraphic != nil )
			tbitFree( ghbitGraphic );
		ghbitGraphic = nil;
	}
	CheckItem( hFlipMenu, gFlipMenuItem, false );

	gFlipMenuItem = menuItem;

	switch ( menuItem ) 
	{
		case kNonePMENU:
			gFlipDirection = kFlipNone;
			break;

		case kHorizPMENU:
			gFlipDirection = kFlipHorizontal;
			break;

		case kVertPMENU:
			gFlipDirection = kFlipVertical;
			break;

		case kBothPMENU:
		  gFlipDirection = kFlipHorzAndVert;
		 break;
		  
		default:
			gFlipMenuItem = kFlipNone;
		 break;
	}
	CheckItem( hFlipMenu, gFlipMenuItem, true );
	_UpdateGraphic();
}


/* ----------------------------------------------------------------------------
   _HandleMRotate 
   Process Rotate menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMRotate ( short menuItem )
{
	if ( _RotateMenuItem != menuItem )
	{
		FillRect( &(gMainWindow->portRect), &qd.white );
		if ( ghbitGraphic != nil )
			tbitFree( ghbitGraphic );
		ghbitGraphic = nil;
	}
	CheckItem( hRotateMenu, _RotateMenuItem, false );
	_RotateMenuItem = menuItem;
	switch ( menuItem ) 
	{
	   case k0RMENU:
		  gRotateDegrees = 0;
		  break;
	   case k90RMENU:
		  gRotateDegrees = 90;
		  break;
	   case k180RMENU:
		  gRotateDegrees = 180;
		  break;
	   case k270RMENU:
		  gRotateDegrees = 270;
		  break;
	   case kOtherRMENU:
		  gRotateDegrees = 0;
		  break;
	   default:
		  gRotateDegrees = 0;
		  _RotateMenuItem = 1;
		  break;
	}
	CheckItem( hRotateMenu, _RotateMenuItem, true );
	_UpdateGraphic();
}


/* ----------------------------------------------------------------------------
   _HandleMShape 
   Process Shape menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMShape ( short menuItem, Boolean fOption )
{
Boolean					success;
RGpDrawingSurface		drawingSurface;
ETextEscapement		escapement;
Boolean					fVertical = FALSE;

	if ( _ShapeMenuItem != menuItem )
	{
		FillRect( &(gMainWindow->portRect), &qd.white );
		if ( ghbitGraphic != nil )
			tbitFree( ghbitGraphic );
		ghbitGraphic = nil;
	}
	CheckItem( hShapeMenu, _ShapeMenuItem, false );
	CheckItem( hWackyMenu, kShearWMENU, false );
	CheckItem( hWackyMenu, kAltShearWMENU, false );
	CheckItem( hWackyMenu, kAltBaselineWMENU, false );
	_WackyMenuItem = 0;
	CheckItem( hPathMenu, _PathMenuItem, false );
	_PathMenuItem = 1;
	CheckItem( hPathMenu, _PathMenuItem, true );
	delete gpWarpPath; 
	gpWarpPath = nil;
	if ( gpHeadline == nil )
	{
		_ShapeMenuItem = 0;
		return;
	}
	escapement = gpHeadline->GetTextEscapement();
	if ( escapement == kTop2BottomLeft2Right || 
			escapement == kTop2BottomRight2Left || 
			escapement == kBottom2TopLeft2Right || 
			escapement == kBottom2TopRight2Left )
		fVertical = TRUE;
	else
		fVertical = FALSE;
	if ( fOption )
	{
		if ( menuItem < 3 ) 
			success = DefineHeadlineWarp( kNoWarpShape, gpHeadline );
		else if ( menuItem > 16 ) 
			success = false;
		else if ( fVertical ) 
			success = false;
		else 
			success = DefineHeadlineWarp( _sOldShapeIdMap[menuItem], gpHeadline );
	}
	else
	{
		if ( menuItem < 3 ) 
			success = DefineHeadlineWarp( kNoWarpShape, gpHeadline );
		else if ( ( fVertical && menuItem > 21 ) || ( !fVertical && menuItem > 53 ) )
			success = false;
		else if ( fVertical ) 
			success = DefineHeadlineWarp( _sVertShapeIdMap[menuItem], gpHeadline );
		else 
			success = DefineHeadlineWarp( _sHorzShapeIdMap[menuItem], gpHeadline );
	}
	if ( success || menuItem == 0 )
		_ShapeMenuItem = menuItem;
	else
	{
		(void)gpHeadline->ApplyNonDistort( CalcDefaultHeadlineScreenSize() );
		_ShapeMenuItem = 1;
	}
	if ( gpHeadline != nil )
		gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
	if ( menuItem > 0 )
	{
		CheckItem( hShapeMenu, _ShapeMenuItem, true );
		_UpdateGraphic();
	}
}

/* ----------------------------------------------------------------------------
   _HandleMPath 
   Process Path menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMPath ( short menuItem )
{
Boolean					success;
RGpDrawingSurface		drawingSurface;

	if ( _PathMenuItem != menuItem )
	{
		FillRect( &(gMainWindow->portRect), &qd.white );
		if ( ghbitGraphic != nil )
			tbitFree( ghbitGraphic );
		ghbitGraphic = nil;
	}
	CheckItem( hPathMenu, _PathMenuItem, false );
	CheckItem( hWackyMenu, kShearWMENU, false );
	CheckItem( hWackyMenu, kAltShearWMENU, false );
	CheckItem( hWackyMenu, kAltBaselineWMENU, false );
	_WackyMenuItem = 0;
	CheckItem( hShapeMenu, _ShapeMenuItem, false );
	_ShapeMenuItem = 1;
	CheckItem( hShapeMenu, _ShapeMenuItem, true );
	delete gpWarpPath; 
	gpWarpPath = nil;
	if ( gpHeadline == nil )
	{
		_PathMenuItem = 0;
		return;
	}
	switch ( menuItem ) 
	{
	   case kNoPathGMENU:
			success = DefineHeadlineWarp( kNoWarpShape, gpHeadline );
		  	break;
	   case kPath10GMENU:
			success = DefineHeadlinePath( kWarpPath10, gpHeadline );
		  	break;
	   case kPath20GMENU:
			success = DefineHeadlinePath( kWarpPath20, gpHeadline );
		  	break;
	   case kPath30GMENU:
			success = DefineHeadlinePath( kWarpPath30, gpHeadline );
		  	break;
	   case kPath40GMENU:
			success = DefineHeadlinePath( kWarpPath40, gpHeadline );
		  	break;
	   case kPath50GMENU:
			success = DefineHeadlinePath( kWarpPath50, gpHeadline );
		  	break;
	   case kPath60GMENU:
			success = DefineHeadlinePath( kWarpPath60, gpHeadline );
		  	break;
	   case kPath70GMENU:
			success = DefineHeadlinePath( kWarpPath70, gpHeadline );
		  	break;
	   case kPath80GMENU:
			success = DefineHeadlinePath( kWarpPath80, gpHeadline );
		  	break;
	   case kPath90GMENU:
			success = DefineHeadlinePath( kWarpPath90, gpHeadline );
		  	break;
	   case kPath100GMENU:
			success = DefineHeadlinePath( kWarpPath100, gpHeadline );
		  	break;
	   case kPath110GMENU:
			success = DefineHeadlinePath( kWarpPath110, gpHeadline );
		  	break;
	   case kPath120GMENU:
			success = DefineHeadlinePath( kWarpPath120, gpHeadline );
		  	break;
	   case kPath130GMENU:
			success = DefineHeadlinePath( kWarpPath130, gpHeadline );
		  	break;
	   default:
		  	success = false;
		  	break;
	}
	if ( success || menuItem == 0 )
	{
		_PathMenuItem = menuItem;
		gpHeadline->ApplyTextJustification( kCenterJust );
	}
	else
	{
		(void)gpHeadline->ApplyNonDistort( CalcDefaultHeadlineScreenSize() );
		_PathMenuItem = 1;
	}
	if ( gpHeadline != nil )
		gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
	if ( menuItem > 0 )
	{
		CheckItem( hPathMenu, _PathMenuItem, true );
		_UpdateGraphic();
	}
}

/* ----------------------------------------------------------------------------
   _HandleMEffects 
   Process Effects menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMEffects ( short menuItem )
{
EProjectionEffects		projection = kNoProjection;
EShadowEffects				shadow = kNoShadow;
YPointCount 				nrStages;
RIntPoint 					pt;
YRealDimension 			depth;
RGpDrawingSurface			drawingSurface;

	FillRect( &(gMainWindow->portRect), &qd.white );
	if ( ghbitGraphic != nil )
		tbitFree( ghbitGraphic );
	ghbitGraphic = nil;
	if ( _StackMenuItem > 0 )
		CheckItem( hEffectsMenu, _StackMenuItem, false );
	if ( _ShadowMenuItem > 0 )
		CheckItem( hEffectsMenu, _ShadowMenuItem, false );
	if ( _ExtrusionMenuItem > 0 )
		CheckItem( hEffectsMenu, _ExtrusionMenuItem, false );
	if ( _FramedMenuItem > 0 )
		CheckItem( hEffectsMenu, _FramedMenuItem, false );
	switch ( menuItem ) 
	{
	   case kNoneCMENU:
		  gpHeadline->ApplyProjectionEffect( kNoProjection, 0, 0.0, RIntPoint( 0, 0 ) );
		  gpHeadline->ApplyShadowEffect( kNoShadow, 0.0, RIntPoint( 0, 0 ) );
		  gpHeadline->ApplyFrameEffect( kFrameNoFrame );
		  _StackMenuItem = 0;
		  _ExtrusionMenuItem = 0;
		  _ShadowMenuItem = 0;
		  _FramedMenuItem = 0;
		  break;
	   case kStackCMENU:
	   case kSpinStackCMENU:
	   case kPerspStackCMENU:
			if ( _StackMenuItem == menuItem )
			{
				gpHeadline->ApplyProjectionEffect( kNoProjection, 0, 0.0, RIntPoint( 0, 0 ) );
				_StackMenuItem = 0;
			}
			else
			{
				projection = _HandleProjection( menuItem, &nrStages, &depth, pt );
				if ( projection != kNoProjection )
				{
					gpHeadline->ApplyProjectionEffect( projection, nrStages, depth, pt );
					_StackMenuItem = menuItem;
				}
			}
		  _ExtrusionMenuItem = 0;
		  _FramedMenuItem = 0;
		  break;
	   case kExtruCMENU:
	   case kPerspExtruCMENU:
	   case kIllumExtruCMENU:
		  _StackMenuItem = 0;
			if ( _ExtrusionMenuItem == menuItem )
			{
				gpHeadline->ApplyProjectionEffect( kNoProjection, 0, 0.0, RIntPoint( 0, 0 ) );
				_ExtrusionMenuItem = 0;
			}
			else
			{
				projection = _HandleProjection( menuItem, &nrStages, &depth, pt );
				if ( projection != kNoProjection )
				{
					gpHeadline->ApplyProjectionEffect( projection, nrStages, depth, pt );
					_ExtrusionMenuItem = menuItem;
				}
			}
		  _FramedMenuItem = 0;
		  break;
	   case kDropShadowCMENU:
	   case kCastShadowCMENU:
	   case kSoftDropShadowCMENU:
	   case kSoftCastShadowCMENU:
			if ( _ShadowMenuItem == menuItem )
			{
				gpHeadline->ApplyShadowEffect( kNoShadow, 0.0, RIntPoint( 0, 0 ) );
				_ShadowMenuItem = 0;
			}
			else
			{
				shadow = _HandleShadow( menuItem, &depth, pt );
				if ( shadow != kNoShadow )
				{
					gpHeadline->ApplyShadowEffect( shadow, depth, pt );
					_ShadowMenuItem = menuItem;
				}
			}
		  _FramedMenuItem = 0;
		  break;
	   case kEmbossCMENU:
		  _StackMenuItem = 0;
		  _ShadowMenuItem = 0;
		  _ExtrusionMenuItem = 0;
			if ( _FramedMenuItem == menuItem )
			{
				gpHeadline->ApplyFrameEffect( kFrameNoFrame );
				_FramedMenuItem = 0;
			}
			else
			{
				if ( _SelectOutlineColors( 0x0E, 0x02, kEmbossedOutline, false ) )
				{
					gpHeadline->ApplyFrameEffect( kEmboss );
					_FramedMenuItem = menuItem;
				}
			}
		  break;
	   case kDebossCMENU:
		  _StackMenuItem = 0;
		  _ShadowMenuItem = 0;
		  _ExtrusionMenuItem = 0;
			if ( _FramedMenuItem == menuItem )
			{
				gpHeadline->ApplyFrameEffect( kFrameNoFrame );
				_FramedMenuItem = 0;
			}
			else
			{
				if ( _SelectOutlineColors( 0x0E, 0x02, kDebossedOutline, false ) )
				{
					gpHeadline->ApplyFrameEffect( kDeboss );
					_FramedMenuItem = menuItem;
				}
			}
		  break;
	   case kSilhouetteCMENU:
		  _StackMenuItem = 0;
		  _ShadowMenuItem = 0;
		  _ExtrusionMenuItem = 0;
			if ( _FramedMenuItem == menuItem )
			{
				gpHeadline->ApplyFrameEffect( kFrameNoFrame );
				_FramedMenuItem = 0;
			}
			else
			{
				if ( _SelectOutlineColors( 0x02, 0x01, kSimpleFill, false ) )
				{
					RHeadlineColor	fillColor;
					gpHeadline->GetOutlineFill( fillColor );
					gpHeadline->SetFrameFill( kSilhouette, fillColor );
					gpHeadline->ApplyFrameEffect( kSilhouette );
					_FramedMenuItem = menuItem;
				}
			}
		  break;
	   default:
		  _StackMenuItem = 0;
		  _ShadowMenuItem = 0;
		  _ExtrusionMenuItem = 0;
		  _FramedMenuItem = 0;
		  break;
	}
	if ( gpHeadline != nil )
		gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
	if ( _StackMenuItem > 0 )
		CheckItem( hEffectsMenu, _StackMenuItem, true );
	if ( _ShadowMenuItem > 0 )
		CheckItem( hEffectsMenu, _ShadowMenuItem, true );
	if ( _ExtrusionMenuItem > 0 )
		CheckItem( hEffectsMenu, _ExtrusionMenuItem, true );
	if ( _FramedMenuItem > 0 )
		CheckItem( hEffectsMenu, _FramedMenuItem, true );
	if ( menuItem > 0 )
		_UpdateGraphic();
}


/* ----------------------------------------------------------------------------
   _HandleMOutline 
   Process Outline Effects menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMOutline ( short menuItem, Boolean fOption )
{
	RGpDrawingSurface		drawingSurface;

	FillRect( &(gMainWindow->portRect), &qd.white );
	if ( ghbitGraphic != nil )
		tbitFree( ghbitGraphic );
	ghbitGraphic = nil;
	if ( _OutlineMenuItem > 0 )
		CheckItem( hOutlineMenu, _OutlineMenuItem, false );
	switch ( menuItem ) 
	{
	   case kStrokeOMENU:
			if ( _SelectOutlineColors( 0x01, 0x01, kSimpleStroke, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kSimpleStroke );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case kFillOMENU:
			if ( fOption )
			{
				if ( _SelectOutlineColors( 0x03, 0x04, kSimpleFill, true ) )
				{
					gpHeadline->ApplyOutlineEffect( kSimpleFill );
					_OutlineMenuItem = menuItem;
				}
			}
			else
			{
				if ( _SelectOutlineColors( 0x02, 0x01, kSimpleFill, false ) )
				{
					gpHeadline->ApplyOutlineEffect( kSimpleFill );
					_OutlineMenuItem = menuItem;
				}
			}
		  break;
	   case kStrokeFillOMENU:
			if ( _SelectOutlineColors( 0x03, 0x01, kSimpleStrokeFill, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kSimpleStrokeFill );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case k1OutlineOMENU:
			if ( _SelectOutlineColors( 0x03, 0x01, kHeavyOutline, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kHeavyOutline );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case k2OutlineOMENU:
			if ( _SelectOutlineColors( 0x0B, 0x01, kDoubleOutline, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kDoubleOutline );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case k2HairOutlineOMENU:
			if ( _SelectOutlineColors( 0x0B, 0x01, kDoubleHairlineOutline, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kDoubleHairlineOutline );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case k3OutlineOMENU:
			if ( _SelectOutlineColors( 0x0F, 0x01, kTripleOutline, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kTripleOutline );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case kHair2OutlineOMENU:
			if ( _SelectOutlineColors( 0x0B, 0x01, kHairlineDoubleOutline, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kHairlineDoubleOutline );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case kBlurOMENU:
			if ( _SelectOutlineColors( 0x02, 0x01, kBlurredOutline, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kBlurredOutline );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case kRoundOMENU:
			if ( _SelectOutlineColors( 0x00, 0x01, kRoundedOutline, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kRoundedOutline );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case kGlintOMENU:
			if ( _SelectOutlineColors( 0x00, 0x01, kGlintOutline, false ) )
			{
				gpHeadline->ApplyOutlineEffect( kGlintOutline );
				_OutlineMenuItem = menuItem;
			}
		  break;
	   case kEmbossOMENU:
			if ( fOption )
			{
				if ( _SelectOutlineColors( 0x0E, 0x01, kEmbossedOutline, false ) )
				{
					gpHeadline->ApplyOutlineEffect( kEmbossedOutline );
					_OutlineMenuItem = menuItem;
				}
			}
			else
			{
				if ( _SelectOutlineColors( 0x0E, 0x01, kDebossedOutline, false ) )
				{
					gpHeadline->ApplyOutlineEffect( kDebossedOutline );
					_OutlineMenuItem = menuItem;
				}
			}
		  break;
	   default:
		  _OutlineMenuItem = 0;
		  break;
	}
	if ( _OutlineMenuItem > 0 )
		CheckItem( hOutlineMenu, _OutlineMenuItem, true );
	if ( menuItem > 0 )
	{
		if ( gpHeadline != nil )
		{
			gpHeadline->Redefine();
			gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
		}
		_UpdateGraphic();
	}
}


/* ----------------------------------------------------------------------------
   _HandleMWacky 
   Process Wacky menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMWacky ( short menuItem )
{
YAngle 												angle;

	FillRect( &(gMainWindow->portRect), &qd.white );
	if ( ghbitGraphic != nil )
		tbitFree( ghbitGraphic );
	ghbitGraphic = nil;
	if ( _WackyMenuItem > 0 )
		CheckItem( hWackyMenu, _WackyMenuItem, false );
	switch ( menuItem ) 
	{
		case kShearWMENU:
			CheckItem( hShapeMenu, _ShapeMenuItem, false );
			_ShapeMenuItem = 1;
			CheckItem( hShapeMenu, _ShapeMenuItem, true );
			CheckItem( hPathMenu, _PathMenuItem, false );
			_PathMenuItem = 1;
			CheckItem( hPathMenu, _PathMenuItem, true );
			if ( _WackyMenuItem == menuItem )
			{
				(void)gpHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
				_WackyMenuItem = 0;
			}
			else
			{
				if ( _HandleShear( &angle ) )
				{
					gpHeadline->ApplyShearEffect( angle );	 
					_WackyMenuItem = menuItem;
				}
				else
				{
					(void)gpHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
					_WackyMenuItem = 0;
				}
			}
			break;
	   case kAltShearWMENU:
			CheckItem( hShapeMenu, _ShapeMenuItem, false );
			_ShapeMenuItem = 1;
			CheckItem( hShapeMenu, _ShapeMenuItem, true );
			CheckItem( hPathMenu, _PathMenuItem, false );
			_PathMenuItem = 1;
			CheckItem( hPathMenu, _PathMenuItem, true );
			if ( _WackyMenuItem == menuItem )
			{
				(void)gpHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
				_WackyMenuItem = 0;
			}
			else
			{
				if ( _HandleShear( &angle ) )
				{
					gpHeadline->ApplyAltShearEffect( angle, angle ); 
					_WackyMenuItem = menuItem;
				}
				else
				{
					(void)gpHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
					_WackyMenuItem = 0;
				}
			}
			break;
	   case kAltBaselineWMENU:
			CheckItem( hShapeMenu, _ShapeMenuItem, false );
			_ShapeMenuItem = 1;
			CheckItem( hShapeMenu, _ShapeMenuItem, true );
			CheckItem( hPathMenu, _PathMenuItem, false );
			_PathMenuItem = 1;
			CheckItem( hPathMenu, _PathMenuItem, true );
			if ( _WackyMenuItem == menuItem )
			{
				(void)gpHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
				_WackyMenuItem = 0;
			}
			else
			{
				gpHeadline->ApplyAlternateBaselineEffect( 0.25 );
				_WackyMenuItem = menuItem;
			}
			break;
	   case kAltShear2WMENU:
			CheckItem( hShapeMenu, _ShapeMenuItem, false );
			_ShapeMenuItem = 1;
			CheckItem( hShapeMenu, _ShapeMenuItem, true );
			CheckItem( hPathMenu, _PathMenuItem, false );
			_PathMenuItem = 1;
			CheckItem( hPathMenu, _PathMenuItem, true );
			if ( _WackyMenuItem == menuItem )
			{
				(void)gpHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
				_WackyMenuItem = 0;
			}
			else
			{
				if ( _HandleShear( &angle ) )
				{
					gpHeadline->ApplyAltShearEffect( angle ); 
					_WackyMenuItem = menuItem;
				}
				else
				{
					(void)gpHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( gpHeadline->IsHorizontal() ) );
					_WackyMenuItem = 0;
				}
			}
			break;
	   default:
			CheckItem( hWackyMenu, kShearWMENU, false );
			CheckItem( hWackyMenu, kAltShearWMENU, false );
			CheckItem( hWackyMenu, kAltBaselineWMENU, false );
			CheckItem( hWackyMenu, kAltShear2WMENU, false );
		  	_WackyMenuItem = 0;
		  	break;
	}
	if ( _WackyMenuItem > 0 )
		CheckItem( hWackyMenu, _WackyMenuItem, true );
	if ( menuItem > 0 )
	{
		RGpDrawingSurface	drawingSurface;
		if ( gpHeadline != nil )
			gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
		_UpdateGraphic();
	}
}

/* ----------------------------------------------------------------------------
   _HandleMFont 
   Process Font menu selection.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _HandleMFont ( short menuItem )
{
	FillRect( &(gMainWindow->portRect), &qd.white );
	if ( ghbitGraphic != nil )
		tbitFree( ghbitGraphic );
	ghbitGraphic = nil;
	if ( _FontMenuItem > 0 )
		CheckItem( hFontMenu, _FontMenuItem, false );
	_FontMenuItem = menuItem;
	if ( _FontMenuItem > 0 )
	{
		RGpDrawingSurface	drawingSurface;
		CheckItem( hFontMenu, _FontMenuItem, true );
		if ( gpHeadline != nil )
		{
			gpHeadline->SetFont( gFontList.GetRFont( _FontMenuItem - 1 ) );
			gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
		}
	}
	_UpdateGraphic();
}

/* ----------------------------------------------------------------------------
   _HandleProjection


   ---------------------------------------------------------------------------- */
static  EProjectionEffects  _HandleProjection( short menuItem, YPointCount* nrStages, YRealDimension* depth, RIntPoint& vanishing )
{
EProjectionEffects	projection;
GrafPtr			  		holdPort;
DialogPtr				pDialog;
short						itemhit;
Handle			   	itemHdl;
short						itemType;
Rect				 		itemBox;
UserItemUPP		  		frameDrawProc;
int						i, item;
Str255					string;
long						l;
float						r;
Boolean					fBlend = false;
RSolidColor				fillColor, blendColor;
RHeadlineColor			fill;

	switch ( menuItem ) 
	{
	   case kStackCMENU:
		  projection = kSimpleStack;
		  break;
	   case kSpinStackCMENU:
		  projection = kSpinStack;
		  break;
	   case kPerspStackCMENU:
		  projection = kPseudoPerspectiveStack;
		  break;
	   case kExtruCMENU:
		  projection = kSolidExtrusion;
		  *nrStages = 1;
		  break;
	   case kPerspExtruCMENU:
		  projection = kPseudoPerspectiveExtrusion;
		  *nrStages = 1;
		  break;
	   case kIllumExtruCMENU:
		  projection = kIlluminateExtrusion;
		  *nrStages = 1;
		  break;
	   default:
		  projection = kNoProjection;
		  break;
	}
   
	// create dialog
	GetPort( &holdPort );   
	ShowCursor();
	SetCursor( &qd.arrow );
	if ( (pDialog = GetNewDialog( 5107, nil, (WindowPtr)-1) ) == nil )
		return kNoProjection;
	SetPort( pDialog );
	frameDrawProc = NewUserItemProc( _ButtonFrameDraw );
	GetDItem( pDialog, kOkUserITEM, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, kOkUserITEM, itemType, (Handle)frameDrawProc, &itemBox );

	// turn off unneeded colors
	GetDItem( pDialog, 23, &itemType, &itemHdl, &itemBox );
	SetCtlValue((ControlHandle)itemHdl, 1 );
	for ( i = 26; i < 37; i++ )
	{
		GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
  	   HiliteControl( (ControlHandle)itemHdl, 255 );
	}
   if ( projection == kSolidExtrusion || 
		  projection == kPseudoPerspectiveExtrusion || 
		  projection == kIlluminateExtrusion )
	{
		HideDItem( pDialog, 4 );
		HideDItem( pDialog, 5 );
		GetDItem( pDialog, 7, &itemType, &itemHdl, &itemBox );
		SetIText( itemHdl, "\p0.2" );
	}
   else if ( projection == kSpinStack )
	{
		GetDItem( pDialog, 9, &itemType, &itemHdl, &itemBox );
		SetIText( itemHdl, "\p0" );
		GetDItem( pDialog, 11, &itemType, &itemHdl, &itemBox );
		SetIText( itemHdl, "\p1000" );
	}

	// process dialog
	ShowWindow( pDialog );
	itemhit = 0;
	while ( itemhit != kOkBUTTON && itemhit != kCancelBUTTON )
	{
		ModalDialog( nil, &itemhit );
		switch ( itemhit ) 
		{
		   case 14 :
		   case 15 :
		   case 16 :
		   case 17 :
		   case 18 :
		   case 19 :
		   case 20 :
		   case 21 :
		   case 22 :
		   case 23 :
		   case 24 :
				for ( i = 14; i < 25; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					SetCtlValue((ControlHandle)itemHdl, 0 );
				}
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, 1 );
			  	break;
		   case 26 :
		   case 27 :
		   case 28 :
		   case 29 :
		   case 30 :
		   case 31 :
		   case 32 :
		   case 33 :
		   case 34 :
		   case 35 :
		   case 36 :
				for ( i = 26; i < 37; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					SetCtlValue((ControlHandle)itemHdl, 0 );
				}
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, 1 );
			  	break;
		   case 12 :
				fBlend = !fBlend;
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, ( fBlend ? 1 : 0 ) );
				if ( fBlend )
				{
					for ( i = 26; i < 37; i++ )
					{
						GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
 				 	   HiliteControl( (ControlHandle)itemHdl, 0 );
					}
					SetCtlValue((ControlHandle)itemHdl, 1 );
				}
				else
				{
					for ( i = 26; i < 37; i++ )
					{
						GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
						SetCtlValue((ControlHandle)itemHdl, 0 );
 				 	   HiliteControl( (ControlHandle)itemHdl, 255 );
					}
				}
			  	break;
		   case kCancelBUTTON :
			   projection = kNoProjection;
			  	break;
		   case kOkBUTTON :
			   if ( projection == kSolidExtrusion || 
				  	  projection == kPseudoPerspectiveExtrusion || 
				 	  projection == kIlluminateExtrusion )
			  		*nrStages = 1;
				else
				{
					GetDItem( pDialog, 5, &itemType, &itemHdl, &itemBox );
					GetIText( itemHdl, string );
					StringToNum( string, &l );
					*nrStages = l;
				}
				GetDItem( pDialog, 7, &itemType, &itemHdl, &itemBox );
				GetIText( itemHdl, string );
				p2cstr( string );
				sscanf( (char *)string, "%f", &r );
				*depth = r;
				GetDItem( pDialog, 9, &itemType, &itemHdl, &itemBox );
				GetIText( itemHdl, string );
				StringToNum( string, &l );
				vanishing.m_x = l;
				GetDItem( pDialog, 11, &itemType, &itemHdl, &itemBox );
				GetIText( itemHdl, string );
				StringToNum( string, &l );
				vanishing.m_y = l;
				item = 0;
				for ( i = 14; i < 25; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					if ( GetCtlValue( (ControlHandle)itemHdl ) != 0 )
						item = i;
				}
				switch ( item ) 
				{
				   case 14 :
						fillColor = RSolidColor( kBlack );
					  	break;
				   case 15 :
						fillColor = RSolidColor( kYellow );
					  	break;
				   case 16 :
						fillColor = RSolidColor( kMagenta );
					  	break;
				   case 17 :
						fillColor = RSolidColor( kRed );
					  	break;
				   case 18 :
						fillColor = RSolidColor( kCyan );
					  	break;
				   case 19 :
						fillColor = RSolidColor( kGreen );
					  	break;
				   case 20 :
						fillColor = RSolidColor( kBlue );
					  	break;
				   case 21 :
						fillColor = RSolidColor( kWhite );
					  	break;
				   case 22 :
						fillColor = RSolidColor( kGray );
					  	break;
				   case 23 :
						fillColor = RSolidColor( kLightGray );
					  	break;
				   case 24 :
						fillColor = RSolidColor( kDarkGray );
					  	break;
				   default:
						fillColor = RSolidColor( kWhite );
					  	break;
				}
				if ( fBlend )
				{
					item = 0;
					for ( i = 26; i < 37; i++ )
					{
						GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
						if ( GetCtlValue( (ControlHandle)itemHdl ) != 0 )
							item = i;
					}
					switch ( item ) 
					{
					   case 26 :
							blendColor = RSolidColor( kBlack );
						  	break;
					   case 27 :
							blendColor = RSolidColor( kYellow );
						  	break;
					   case 28 :
							blendColor = RSolidColor( kMagenta );
						  	break;
					   case 29 :
							blendColor = RSolidColor( kRed );
						  	break;
					   case 30 :
							blendColor = RSolidColor( kCyan );
						  	break;
					   case 31 :
							blendColor = RSolidColor( kGreen );
						  	break;
					   case 32 :
							blendColor = RSolidColor( kBlue );
						  	break;
					   case 33 :
							blendColor = RSolidColor( kWhite );
						  	break;
					   case 34 :
							blendColor = RSolidColor( kGray );
						  	break;
					   case 35 :
							blendColor = RSolidColor( kLightGray );
						  	break;
					   case 36 :
							blendColor = RSolidColor( kDarkGray );
						  	break;
					   default:
							blendColor = RSolidColor( kWhite );
						  	break;
					}
				}
				if ( fBlend )
					fill = RHeadlineColor( fillColor, blendColor, kEast );
				else
					fill = RHeadlineColor( fillColor );
				gpHeadline->SetProjectionFill( projection, fill );
			  	break;
		   default:
			  	break;
		}
	}

	// cleanup
	DisposDialog( pDialog );
	DisposeRoutineDescriptor( frameDrawProc );
	ObscureCursor();
	SetPort( holdPort );

	return projection;
}

/* ----------------------------------------------------------------------------
   _HandleShadow


   ---------------------------------------------------------------------------- */
static  EShadowEffects  _HandleShadow( short menuItem, YRealDimension* depth, RIntPoint& vanishing )
{
EShadowEffects	shadow;
GrafPtr			  		holdPort;
DialogPtr				pDialog;
short						itemhit;
Handle			   	itemHdl;
short						itemType;
Rect				 		itemBox;
UserItemUPP		  		frameDrawProc;
int						i, item;
Str255					string;
long						l;
float						r;
Boolean					fBlend = false;
RSolidColor				fillColor, blendColor;
RHeadlineColor			fill;

	*depth = 1;
	switch ( menuItem ) 
	{
	   case kDropShadowCMENU:
		  shadow = kDropShadow;
		  break;
	   case kCastShadowCMENU:
		  shadow = kCastShadow;
		  break;
	   case kSoftDropShadowCMENU:
		  shadow = kSoftDropShadow;
		  break;
	   case kSoftCastShadowCMENU:
		  shadow = kSoftCastShadow;
		  break;
	   default:
		  shadow = kNoShadow;
		  break;
	}

	// create dialog
	GetPort( &holdPort );   
	ShowCursor();
	SetCursor( &qd.arrow );
	if ( (pDialog = GetNewDialog( 5107, nil, (WindowPtr)-1) ) == nil )
		return kNoShadow;
	SetPort( pDialog );
	frameDrawProc = NewUserItemProc( _ButtonFrameDraw );
	GetDItem( pDialog, kOkUserITEM, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, kOkUserITEM, itemType, (Handle)frameDrawProc, &itemBox );
	HideDItem( pDialog, 4 );
	HideDItem( pDialog, 5 );
	GetDItem( pDialog, 7, &itemType, &itemHdl, &itemBox );
	SetIText( itemHdl, "\p1.0" );

	// turn off unneeded colors
	GetDItem( pDialog, 23, &itemType, &itemHdl, &itemBox );
	SetCtlValue((ControlHandle)itemHdl, 1 );
	for ( i = 26; i < 37; i++ )
	{
		GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
  	   HiliteControl( (ControlHandle)itemHdl, 255 );
	}

	// process dialog
	ShowWindow( pDialog );
	itemhit = 0;
	while ( itemhit != kOkBUTTON && itemhit != kCancelBUTTON )
	{
		ModalDialog( nil, &itemhit );
		switch ( itemhit ) 
		{
		   case 14 :
		   case 15 :
		   case 16 :
		   case 17 :
		   case 18 :
		   case 19 :
		   case 20 :
		   case 21 :
		   case 22 :
		   case 23 :
		   case 24 :
				for ( i = 14; i < 25; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					SetCtlValue((ControlHandle)itemHdl, 0 );
				}
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, 1 );
			  	break;
		   case 26 :
		   case 27 :
		   case 28 :
		   case 29 :
		   case 30 :
		   case 31 :
		   case 32 :
		   case 33 :
		   case 34 :
		   case 35 :
		   case 36 :
				for ( i = 26; i < 37; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					SetCtlValue((ControlHandle)itemHdl, 0 );
				}
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, 1 );
			  	break;
		   case 12 :
				fBlend = !fBlend;
				GetDItem( pDialog, itemhit, &itemType, &itemHdl, &itemBox );
				SetCtlValue((ControlHandle)itemHdl, ( fBlend ? 1 : 0 ) );
				if ( fBlend )
				{
					for ( i = 26; i < 37; i++ )
					{
						GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
 				 	   HiliteControl( (ControlHandle)itemHdl, 0 );
					}
					SetCtlValue((ControlHandle)itemHdl, 1 );
				}
				else
				{
					for ( i = 26; i < 37; i++ )
					{
						GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
						SetCtlValue((ControlHandle)itemHdl, 0 );
 				 	   HiliteControl( (ControlHandle)itemHdl, 255 );
					}
				}
			  	break;
		   case kCancelBUTTON :
			   shadow = kNoShadow;
			  	break;
		   case kOkBUTTON :
				GetDItem( pDialog, 7, &itemType, &itemHdl, &itemBox );
				GetIText( itemHdl, string );
				p2cstr( string );
				sscanf( (char *)string, "%f", &r );
				*depth = r;
				GetDItem( pDialog, 9, &itemType, &itemHdl, &itemBox );
				GetIText( itemHdl, string );
				StringToNum( string, &l );
				vanishing.m_x = l;
				GetDItem( pDialog, 11, &itemType, &itemHdl, &itemBox );
				GetIText( itemHdl, string );
				StringToNum( string, &l );
				vanishing.m_y = l;
				item = 0;
				for ( i = 14; i < 25; i++ )
				{
					GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
					if ( GetCtlValue( (ControlHandle)itemHdl ) != 0 )
						item = i;
				}
				switch ( item ) 
				{
				   case 14 :
						fillColor = RSolidColor( kBlack );
					  	break;
				   case 15 :
						fillColor = RSolidColor( kYellow );
					  	break;
				   case 16 :
						fillColor = RSolidColor( kMagenta );
					  	break;
				   case 17 :
						fillColor = RSolidColor( kRed );
					  	break;
				   case 18 :
						fillColor = RSolidColor( kCyan );
					  	break;
				   case 19 :
						fillColor = RSolidColor( kGreen );
					  	break;
				   case 20 :
						fillColor = RSolidColor( kBlue );
					  	break;
				   case 21 :
						fillColor = RSolidColor( kWhite );
					  	break;
				   case 22 :
						fillColor = RSolidColor( kGray );
					  	break;
				   case 23 :
						fillColor = RSolidColor( kLightGray );
					  	break;
				   case 24 :
						fillColor = RSolidColor( kDarkGray );
					  	break;
				   default:
						fillColor = RSolidColor( kWhite );
					  	break;
				}
				if ( fBlend )
				{
					item = 0;
					for ( i = 26; i < 37; i++ )
					{
						GetDItem( pDialog, i, &itemType, &itemHdl, &itemBox );
						if ( GetCtlValue( (ControlHandle)itemHdl ) != 0 )
							item = i;
					}
					switch ( item ) 
					{
					   case 26 :
							blendColor = RSolidColor( kBlack );
						  	break;
					   case 27 :
							blendColor = RSolidColor( kYellow );
						  	break;
					   case 28 :
							blendColor = RSolidColor( kMagenta );
						  	break;
					   case 29 :
							blendColor = RSolidColor( kRed );
						  	break;
					   case 30 :
							blendColor = RSolidColor( kCyan );
						  	break;
					   case 31 :
							blendColor = RSolidColor( kGreen );
						  	break;
					   case 32 :
							blendColor = RSolidColor( kBlue );
						  	break;
					   case 33 :
							blendColor = RSolidColor( kWhite );
						  	break;
					   case 34 :
							blendColor = RSolidColor( kGray );
						  	break;
					   case 35 :
							blendColor = RSolidColor( kLightGray );
						  	break;
					   case 36 :
							blendColor = RSolidColor( kDarkGray );
						  	break;
					   default:
							blendColor = RSolidColor( kWhite );
						  	break;
					}
				}
				if ( fBlend )
					fill = RHeadlineColor( fillColor, blendColor, kEast );
				else
					fill = RHeadlineColor( fillColor );
				gpHeadline->SetShadowFill( shadow, fill );
			  	break;
		   default:
			  	break;
		}
	}

	// cleanup
	DisposDialog( pDialog );
	DisposeRoutineDescriptor( frameDrawProc );
	ObscureCursor();
	SetPort( holdPort );

	return shadow;
}


/* ----------------------------------------------------------------------------
   _HandleShear


   ---------------------------------------------------------------------------- */
static  Boolean  _HandleShear( YAngle* angle )
{
GrafPtr			  		holdPort;
DialogPtr				pDialog;
short						itemhit;
Handle			   	itemHdl;
short						itemType;
Rect				 		itemBox;
UserItemUPP		  		frameDrawProc;
Str255					string;
long						l;

	// create dialog
	GetPort( &holdPort );   
	ShowCursor();
	SetCursor( &qd.arrow );
	if ( (pDialog = GetNewDialog( 5108, nil, (WindowPtr)-1) ) == nil )
		return false;
	SetPort( pDialog );
	frameDrawProc = NewUserItemProc( _ButtonFrameDraw );
	GetDItem( pDialog, kOkUserITEM, &itemType, &itemHdl, &itemBox );
	if ( itemType == userItem || itemType == itemDisable )   
	  	SetDItem( pDialog, kOkUserITEM, itemType, (Handle)frameDrawProc, &itemBox );
	SelIText( pDialog, 5, 0, 32767 );

	// process dialog
	ShowWindow( pDialog );
	itemhit = 0;
	while ( itemhit != kOkBUTTON && itemhit != kCancelBUTTON )
	{
		ModalDialog( nil, &itemhit );
		switch ( itemhit ) 
		{
		   case kOkBUTTON :
				GetDItem( pDialog, 5, &itemType, &itemHdl, &itemBox );
				GetIText( itemHdl, string );
				StringToNum( string, &l );
				*angle = l;
			  	break;
		   default:
			  	break;
		}
	}

	// cleanup
	DisposDialog( pDialog );
	DisposeRoutineDescriptor( frameDrawProc );
	ObscureCursor();
	SetPort( holdPort );

	return( itemhit == kOkBUTTON );
}


/* ----------------------------------------------------------------------------
   AdjustMenus 
   Enable or disable menu items.	
   Returns nothing.										
   ---------------------------------------------------------------------------- */
void  AdjustMenus (void)
{
	if ( gpGraphic == nil && gpHeadline == nil )
	{
 	   EnableItem(hFileMenu, kNewFMENU); 
 	   EnableItem(hFileMenu, kOpenFMENU); 
  	   DisableItem(hFileMenu, kCloseFMENU); 
  	   DisableItem(hFileMenu, kPrintFMENU); 
	}
	else
	{
 	   DisableItem(hFileMenu, kNewFMENU); 
// 	   DisableItem(hFileMenu, kOpenFMENU); 
  	   EnableItem(hFileMenu, kCloseFMENU); 
  	   EnableItem(hFileMenu, kPrintFMENU); 
	}
	EnableItem(hFileMenu, kQuitFMENU); 
	DisableItem(hEditMenu, kUndoEMENU); 
	if ( gpGraphic == nil && gpHeadline == nil )
	{
		DisableItem(hEditMenu, kCutEMENU); 
		DisableItem(hEditMenu, kCopyEMENU); 
		DisableItem(hEditMenu, kClearEMENU); 
	}
	else
	{
		EnableItem(hEditMenu, kCutEMENU); 
		EnableItem(hEditMenu, kCopyEMENU); 
		EnableItem(hEditMenu, kClearEMENU); 
	}
	DisableItem(hEditMenu, kPasteEMENU); 
	CheckItem(hScaleMenu, gScaleMenuItem, true);
	CheckItem(hFlipMenu, gFlipMenuItem, true);
	CheckItem(hRotateMenu, _RotateMenuItem, true);
	if ( gpGraphic == nil && gpHeadline == nil )
	{
 		DisableItem(hScaleMenu, 0); 
 		DisableItem(hFlipMenu, 0); 
  		DisableItem(hRotateMenu, 0); 
	}
	else
	{
 		EnableItem(hScaleMenu, 0); 
 		EnableItem(hFlipMenu, 0); 
 		EnableItem(hRotateMenu, 0); 
		DisableItem(hRotateMenu, kOtherRMENU); 
	}
	if ( gpHeadline == nil )
	{
 		DisableItem(hShapeMenu, 0); 
 		DisableItem(hPathMenu, 0); 
 		DisableItem(hEffectsMenu, 0); 
  		DisableItem(hWackyMenu, 0); 
  		DisableItem(hOutlineMenu, 0); 
  		if (hFontMenu != nil)
  			DisableItem(hFontMenu, 0); 
	}
	else
	{
 		EnableItem(hShapeMenu, 0); 
 		EnableItem(hPathMenu, 0); 
 		EnableItem(hEffectsMenu, 0); 
 		EnableItem(hWackyMenu, 0); 
 		EnableItem(hOutlineMenu, 0); 
		if ( _WackyMenuItem > 0 )
			CheckItem(hWackyMenu, _WackyMenuItem, true);
		if ( _ShapeMenuItem > 0 )
			CheckItem(hShapeMenu, _ShapeMenuItem, true);
		if ( _PathMenuItem > 0 )
			CheckItem(hPathMenu, _PathMenuItem, true);
		if ( _StackMenuItem > 0 )
			CheckItem(hEffectsMenu, _StackMenuItem, true);
		if ( _ExtrusionMenuItem > 0 )
			CheckItem(hEffectsMenu, _ExtrusionMenuItem, true);
		if ( _FramedMenuItem > 0 )
			CheckItem(hEffectsMenu, _FramedMenuItem, true);
		if ( _OutlineMenuItem > 0 )
			CheckItem(hOutlineMenu, _OutlineMenuItem, true);
  		if (hFontMenu == nil)
		{
  			hFontMenu = _CreateFontMenu(10);
			InsertMenu(hFontMenu, 0);
			if ( _FontMenuItem > 0 )
				CheckItem( hFontMenu, _FontMenuItem, true );
		}
  		else
 			EnableItem(hFontMenu, 0); 
	}
}


/* ----------------------------------------------------------------------------
   _PageSetup 
   
   Returns boolean indicating if page setup was successfully.									 
   ---------------------------------------------------------------------------- */
Boolean  _PageSetup (void)
{
	PrOpen();
	if (PrError() != noErr)
	   return false;
	if (hPrintInfo == nil)
	{
	   hPrintInfo = (THPrint)NewHandle(sizeof(TPrint));
	   if (hPrintInfo == nil)
		  	return false;
	   PrintDefault(hPrintInfo);
	   if (PrError() != noErr)
		 	return false;
	}
	PrStlDialog(hPrintInfo);
	if (PrError() != noErr)
	   return false;
	PrClose();

	return true;
}


/* ----------------------------------------------------------------------------
   _ButtonFrameDraw 
   Update procedure for a dialog button frame.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static pascal void  _ButtonFrameDraw( DialogPtr dlogPtr, short item )
{
short				itemType;
Rect				itemBox;
Handle			itemHdl;
GrafPtr			holdPort;
PenState			holdPen;

	GetPort( &holdPort );   
	GetPenState( &holdPen );
	SetPort( dlogPtr );
	GetDItem( dlogPtr, item, &itemType, &itemHdl, &itemBox );
	PenSize( 3, 3 );
	InsetRect( &itemBox, -4, -4 );
	FrameRoundRect( &itemBox, 16, 16 );
	SetPort( holdPort );
	SetPenState( &holdPen );
}  


/* ----------------------------------------------------------------------------
   _FontPopupDraw 
   Update procedure for a dialog popup menu.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static pascal void  _FontPopupDraw( DialogPtr dlogPtr, short item )
{
short				itemType;
Rect				itemBox, rect;
Handle			itemHdl;
GrafPtr			holdPort;
Str255			str;
PenState			holdPen;
PolyHandle		triangle;

	// setup
	GetPort( &holdPort );   
	GetPenState( &holdPen );
	SetPort( dlogPtr );
	GetDItem( dlogPtr, item, &itemType, &itemHdl, &itemBox );
	// text
	if ( _hFontMenuPopup != nil )
	{
		itemBox.right = itemBox.left + _sFontMenuWidth;
		GetItem( _hFontMenuPopup, _sFontMenuItem, str );
		SetRect( &rect, itemBox.left, itemBox.top, itemBox.right - kTriagleSpace - kMiscSpace, itemBox.bottom );
		TextBox( (Ptr)&str[1], (Size)str[0], &rect, teFlushRight );
	}
	// frame
	FrameRect( &itemBox );
	// drop shadow
	MoveTo( itemBox.left + 3, itemBox.bottom );
	LineTo( itemBox.right, itemBox.bottom );
	LineTo( itemBox.right, itemBox.top + 3 );
	// triangle
	SetRect( &rect, itemBox.left + kIndentSpace, itemBox.top, itemBox.right, itemBox.bottom );
	triangle = OpenPoly();
	MoveTo(rect.right - 14, rect.top + 7);
	LineTo(rect.right - 3, rect.top + 7);
	LineTo(rect.right - 8, rect.top + 13);
	LineTo(rect.right - 14, rect.top + 7);
	ClosePoly();
	FillPoly( triangle, &qd.black );
	KillPoly( triangle );
	// cleanup
	SetPort( holdPort );
	SetPenState( &holdPen );
}  


/* ----------------------------------------------------------------------------
	_GraphicListDraw
	Dialog user item update procedure for select graphic dialog graphic list.
	Returns nothing. 
	---------------------------------------------------------------------------- */
static  pascal  void  _GraphicListDraw( DialogPtr dlogPtr, short item )
{
#pragma unused( item )
Rect			listRect;

/**  no list  **/
    if ( _hGraphicListHdl == nil )
       return;

/**  draw frame  **/
    listRect = (*_hGraphicListHdl)->rView;
    InsetRect( &listRect, -1, -1 );
    FrameRect (&listRect );
    
/**  handle update  **/
    LUpdate( dlogPtr->visRgn, _hGraphicListHdl );
}


/* ----------------------------------------------------------------------------
	_GraphicListFilter
	Dialog filter procedure for select graphic dialog.
	Returns boolean indicating it event handled. 
	---------------------------------------------------------------------------- */
static pascal  Boolean  _GraphicListFilter (DialogPtr dialogPtr, EventRecord *eventPtr, short *itemHit)
{
Boolean              filter;
short                itemType;
Rect                 itemBox;
Handle               itemHdl;
Point                eventPoint;
long                 ticks;

	filter = false;
	switch ( eventPtr->what ) 
	{
		case keyDown:
			switch ( (eventPtr->message & charCodeMask) ) 
			{
				case kPeriodKEY:
					if ( (eventPtr->modifiers & cmdKey) != 0 )  
					{
						GetDItem( dialogPtr, kCancelBUTTON, &itemType, &itemHdl, &itemBox );
						HiliteControl( (ControlHandle)itemHdl, inButton );
						Delay( 4L, &ticks );
						HiliteControl( (ControlHandle)itemHdl, 0 );
						*itemHit = kCancelBUTTON;
						filter = true;
					}
					break;
				case kEnterKEY:
				case kReturnKEY:
					if ( !_fOkButtonInActive )
					{
						GetDItem( dialogPtr, kOkBUTTON, &itemType, &itemHdl, &itemBox );
						HiliteControl( (ControlHandle)itemHdl, inButton );
						Delay( 4L, &ticks );
						HiliteControl( (ControlHandle)itemHdl, 0 );
						*itemHit = kOkBUTTON;
						filter = true;
					}
					break;
				case kUpArrowKEY:														// REVEIW RAH handle these
					break;
				case kDownArrowKEY:
				 	break;
				case kLeftArrowKEY:
				case kRightArrowKEY:
				 	break;
				default:
				 	break;
			}
			break;
	
			case mouseDown:
				eventPoint = eventPtr->where;
				GlobalToLocal( &eventPoint );
				GetDItem( dialogPtr, kListUserITEM, &itemType, &itemHdl, &itemBox );
				if (PtInRect( eventPoint, &itemBox) )  
				{
					if ( LClick( eventPoint, eventPtr->modifiers, _hGraphicListHdl ) )
						*itemHit = kOkBUTTON;
					filter = true;
				}
				break;
	
			case activateEvt:
				LActivate( true, _hGraphicListHdl );
				break;
	                 
			default:
				break;
	}
	 
	return( filter );
}


/* ----------------------------------------------------------------------------
	_GraphicListFilter
	Dialog filter procedure for headline graphic setup dialog.
	Returns boolean indicating it event handled. 
	---------------------------------------------------------------------------- */
static pascal  Boolean  _HeadlineDlogFilter (DialogPtr dialogPtr, EventRecord *eventPtr, short *itemHit)
{
Boolean              filter;
short                itemType;
Rect                 itemBox;
Handle               itemHdl;
Point                eventPoint;
long                 ticks;
Rect						labelBox;
uLONG						lResult;

	filter = false;
	switch ( eventPtr->what ) 
	{
		case keyDown:
			switch ( (eventPtr->message & charCodeMask) ) 
			{
				case kPeriodKEY:
					if ( (eventPtr->modifiers & cmdKey) != 0 )  
					{
						GetDItem( dialogPtr, kCancelBUTTON, &itemType, &itemHdl, &itemBox );
						HiliteControl( (ControlHandle)itemHdl, inButton );
						Delay( 4L, &ticks );
						HiliteControl( (ControlHandle)itemHdl, 0 );
						*itemHit = kCancelBUTTON;
						filter = true;
					}
					break;
				case kEnterKEY:
					if ( !_fOkButtonInActive )
					{
						GetDItem( dialogPtr, kOkBUTTON, &itemType, &itemHdl, &itemBox );
						HiliteControl( (ControlHandle)itemHdl, inButton );
						Delay( 4L, &ticks );
						HiliteControl( (ControlHandle)itemHdl, 0 );
						*itemHit = kOkBUTTON;
						filter = true;
					}
					break;
				default:
				 	break;
			}
			break;
	
			case mouseDown:
				eventPoint = eventPtr->where;
				GlobalToLocal( &eventPoint );
				GetDItem( dialogPtr, 7, &itemType, &itemHdl, &itemBox );
				GetDItem( dialogPtr, 8, &itemType, &itemHdl, &labelBox );
				labelBox.top = labelBox.top;
				labelBox.bottom = labelBox.bottom;
				if ( _hFontMenuPopup != nil )
					if ( PtInRect( eventPoint, &itemBox ) || PtInRect( eventPoint, &labelBox ) )  
					{
						InvertRect( &labelBox );
						InsertMenu( _hFontMenuPopup, hierMenu );
						LocalToGlobal( &((Point *)(&itemBox))[0] );
						CheckItem( _hFontMenuPopup, _sFontMenuItem, true );
						lResult = PopUpMenuSelect( _hFontMenuPopup, itemBox.top, itemBox.left, _sFontMenuItem );
						CheckItem( _hFontMenuPopup, _sFontMenuItem, false );
						DeleteMenu( 104 );
						InvertRect( &labelBox );
						if ( HiWord( lResult ) != 0 && LoWord( lResult ) != 0 )
							if ( _sFontMenuItem != LoWord( lResult ) )
							{
								_sFontMenuItem = LoWord( lResult );
								GetDItem( dialogPtr, 7, &itemType, &itemHdl, &itemBox );
								GetDItem( dialogPtr, 8, &itemType, &itemHdl, &labelBox );
								UnionRect( &itemBox, &labelBox, &itemBox );
								InvalRect( &itemBox );
							}
						filter = true;
					}
				break;
	
			case activateEvt:
				break;
	                 
			default:
				break;
	}
	 
	return( filter );
}


/* ----------------------------------------------------------------------------
   _GetFileFilter 
	 
   Returns boolean										
   ---------------------------------------------------------------------------- */
static pascal Boolean  _GetFileFilter (ParmBlkPtr pb)
{
long		ext;
long		length = *(pb->fileParam.ioNamePtr);

	if ( length < sizeof(long) )
 	   return true;
	BlockMove( (pb->fileParam.ioNamePtr + length - 3), &ext, sizeof(long) );
	if ( ext == '.PSG' || ext == '.psg' || ext == '.Psg' || ext == '.PSg' || ext == '.PsG' )
		return false;
	return true;
}  


/* ----------------------------------------------------------------------------
   _CloseGraphic 
   Close graphic or headline and reset . . . everything.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _CloseGraphic (void)
{
	delete gpGraphic;
	gpGraphic = nil;
	delete gpHeadline;
	gpHeadline = nil;
	delete gpWarpPath;
	gpWarpPath = nil;
	if ( ghbitGraphic != nil )
		tbitFree( ghbitGraphic );
	ghbitGraphic = nil;
	if ( gpGradientBackdrop != nil )  	
		delete const_cast<RGradientRamp*>( gpGradientBackdrop->GetRamp( ) );
	delete gpGradientBackdrop;
	gpGradientBackdrop = NULL;
	_HandleMScale( k100SMENU );
	_HandleMFlip( kNonePMENU );
	_HandleMRotate( k0RMENU );
	_HandleMShape( 0, false );
	_HandleMEffects( 0 );
	_HandleMWacky( 0 );
	_HandleMOutline( 0, false );
	_HandleMFont( 0 );
	SetWTitle( gMainWindow, "\pRenderer" );
	gScroll.m_dx = gScroll.m_dy = 0;
	PaintMainWindow();
}


/* ----------------------------------------------------------------------------
   _UpdateGraphic 
   Short-cut update event.  
   Returns nothing.										
   ---------------------------------------------------------------------------- */
static  void  _UpdateGraphic (void)
{
    SetPort( gMainWindow );
    PaintMainWindow();                  
    BeginUpdate( gMainWindow );
    ValidRect( &gMainWindow->portRect );
    EndUpdate( gMainWindow );
    SetPort( gMainWindow );													// application is SDI
}

/* ----------------------------------------------------------------------------
   _CreateFontMenu 
    
   Returns menu handle.										
   ---------------------------------------------------------------------------- */
static  MenuHandle  _CreateFontMenu (short id)
{
MenuHandle		hMenu;
Str255			szName;
YFont				font;

	if ( (hMenu = NewMenu( id, "\pFont" )) == nil )
		return nil;

	for ( int j = 0; j < gnFontChoices; ++j )
	{
		font.info = gpFontChoices[j];
		strcpy( (Ptr)szName, font.info.sbName );
		c2pstr( (Ptr)szName );
		AppendMenu( hMenu, szName );
	}
	return hMenu;
}

// ---------------------------------------------------------------------------
//		¥ GetVersionString
// ---------------------------------------------------------------------------
// return the version as a string from 'vers' resource ID 1

#define kByteOffset	6

static void _GetVersionString(Str255 outVersion)
{
	Handle	h = nil;
	
	outVersion[0] = 0;
	h = Get1Resource('vers', 1);
	if (h)
	{
		short vsLen = (short)((unsigned char)(*(*h + kByteOffset))); 
		BlockMove((*h + kByteOffset), outVersion, (vsLen + 1));
		ReleaseResource(h);
	}
}

// ---------------------------------------------------------------------------
//		¥ ConcatPStr
// ---------------------------------------------------------------------------
//	Concatenate two Pascal strings. The first string becomes the combination
//	of the first and second strings.
//
//		Returns a pointer to the concatenated string
//
//		inDestSize specifies the maximum size of the concatenated string
//			(INCLUDING the length byte). The default is sizeof Str255.
//
//		By calling this function as:
//
//			ConcatPStr(io, appendMe, sizeof(io));
//
//		you can change the declaration of io without having to
//		remember to change the call.

static StringPtr _ConcatPStr(Str255 ioFirstStr, ConstStr255Param	inSecondStr, short inDestSize)
{
	// Limit combined string to inDestSize chars
	short	charsToCopy = inSecondStr[0];
	if (ioFirstStr[0] + charsToCopy > inDestSize - 1)
		charsToCopy = inDestSize - 1 - ioFirstStr[0];

	// Copy second to end of first string
	::BlockMoveData(inSecondStr + 1,  ioFirstStr + ioFirstStr[0] + 1, charsToCopy);
    
	// Set length of combined string
	ioFirstStr[0] += charsToCopy;

	return ioFirstStr;
}
