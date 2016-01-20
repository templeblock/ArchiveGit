// ****************************************************************************
//
//  File Name:			event.c
//
//  Project:			Renaissance Graphic Headline Renderer
//
//  Author:				R. Hood
//
//  Description:		Shell application main event loop routines
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
//  $Logfile:: /Development/HeadlineComp/MacShell/Source/EVENT.C $
//	 $Author:: Richh                      $
//	 $Date:: 5/6/97 3:32 PM                 $
//  $Revision:: 22                      $
//
// ****************************************************************************
   

#include   "HeadlinerIncludes.h"

ASSERTNAME

#include	  "GpDrawingSurface.h"
#include	  "HeadlineGraphic.h"
#include	  "HeadlinesCanned.h"
#include   "menu.h"
#include   "update.h"
#include   "event.h"

/**  local prototypes  **/
static   void           _HandleMouseDown (void);
static   void           _HandleKeyDown (void);
static   void           _HandleResumeEvent (void);
static   void           _HandleSuspendEvent (void);
static   void           _HandleActivateEvent (WindowPtr);
static   void           _HandleNullEvent (void);
static   void           _HandleUpdateEvent (EventRecord *);
static   void           _ClickInMainWindow (EventRecord *);
static   void           _HandleGrowWindow (WindowPtr);
static  	void  			_UpdateGraphic (void);

/**  window structures & pointers  **/
extern  WindowPtr            gMainWindow;

/**  globals  **/
extern  Boolean              gQuit;
extern  Boolean              gInForeground;
extern  long                 gSleepTime;
extern  RGraphic*				  gpGraphic;
extern  RHeadlineGraphic*	  gpHeadline;
extern  RGradientFill*		  gpGradientBackdrop;
extern  BOOLEAN  			 	  gfShowPathPoints;
extern  RIntSize				  gScroll;
extern  BOOLEAN 				  gfFeedback;

/**  locals  **/
static  EventRecord         _Event;
static  Boolean             _fAlteredToLowMemory = false;
static  ETextCompensation 	 _eCompensation = kRegular;
static  ETextJustification  _eJustification = kLeftJust;
static  EVerticalPlacement  _eVerticality = kPin2Middle;
static  ELineWeight 	 		 _eLineWeight = kThinLine;
static  Boolean 				 _fLeading = TRUE;
static  Boolean 				 _fKerning = FALSE;
static  Boolean 				 _fDistort = FALSE;


/* ----------------------------------------------------------------------------
   _HandleMouseDown 
   Process mouse down event. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _HandleMouseDown()
{
WindowPtr            whichWindow;
WindowPtr            frontWindow;
short                thePart;

    frontWindow = FrontWindow();
    thePart = FindWindow(_Event.where, &whichWindow);
    switch (thePart) 
    {
       case inMenuBar:
          AdjustMenus();
          HandleMenu(MenuSelect(_Event.where), _Event.modifiers);
          break;
            
       case inSysWindow:
          SystemClick(&_Event, whichWindow);
          break;
            
       case inDesk:
//***          SysBeep(5);  
          break;
            
       case inContent:
          if (whichWindow != frontWindow)
          {
             SelectWindow(whichWindow);
             SetPort(whichWindow);
          }
          else if (whichWindow == gMainWindow)  
             _ClickInMainWindow(&_Event);
          break;

       case inGoAway:
          if (frontWindow != whichWindow)
             SelectWindow(whichWindow);
//          else if (whichWindow == gMainWindow)  
//          {
//             if (TrackGoAway(whichWindow, _Event.where))  
//                if (gMainWindow != nil)
//                   gQuit = true;
//          }
          break;
            
       case inGrow:
          if (frontWindow != whichWindow)
             SelectWindow(whichWindow);
          else if (whichWindow == gMainWindow)  
             _HandleGrowWindow(whichWindow);
          break;
            
       case inDrag:
          if (whichWindow == gMainWindow)
             DragWindow(whichWindow, _Event.where, &qd.screenBits.bounds);
          if (whichWindow != frontWindow)
          {
             SelectWindow(whichWindow);
             SetPort(whichWindow);
          }
          break;
            
       case inZoomIn:
       case inZoomOut:
          if (frontWindow != whichWindow)
             SelectWindow(whichWindow);
          else if (whichWindow == gMainWindow)  
          {
             if (TrackBox(whichWindow, _Event.where, thePart)) 
             {
                SetPort(whichWindow); 
                EraseRect(&whichWindow->portRect); 
                ZoomWindow(whichWindow, thePart, true); 
                InvalRect(&whichWindow->portRect); 
					 gScroll.m_dx = gScroll.m_dy = 0;
             }
          }
          break;
            
       default:
          break;
    }
}


/* ----------------------------------------------------------------------------
   _HandleKeyDown 
   Process key down event. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _HandleKeyDown()
{
RgnHandle				hRgn;
Rect						rect;
RGpDrawingSurface		drawingSurface;
char						key = (char)( _Event.message & charCodeMask );
Boolean					shifted = ( ( _Event.modifiers & shiftKey ) != 0 );

	if ( (_Event.modifiers & cmdKey) != 0 )  
	{
		switch ( key )
		{
			case '1' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					gpHeadline->AdjustLineScale( 1, 1, 1 );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case '2' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					gpHeadline->AdjustLineScale( 8, 4, 2 );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case '3' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					gpHeadline->AdjustLineScale( 2, 4, 2 );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case '4' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					gpHeadline->AdjustLineScale( 2, 4, 8 );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case '0' :
				gScroll.m_dx = gScroll.m_dy = 0;
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
				}
				break;

			case '9' :
				if ( gpHeadline != NULL )
				{
					if ( gpHeadline->GetDistortEffect() == kFollowPath )
					{
						YPercentage	scale = gpHeadline->GetMaxPathHeightScale();
						SetCursor( *(GetCursor( watchCursor )) );
						if ( ::AreFloatsEqual( scale, 1.0 ) )
							scale = 0.8;
						else if ( ::AreFloatsEqual( scale, 0.9 ) )
							scale = 1.0;
						else 
							scale = 0.9;
						(void)gpHeadline->ApplyPathToFollow( gpHeadline->GetPathToFollowId(), scale );
						gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
						FillRect( &(gMainWindow->portRect), &qd.white );
	           	 	InvalRect( &(gMainWindow->portRect) ); 
						SetCursor( &qd.arrow );
					}
				}
				break;

			case 0x1E :				// up arrow
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					if ( ( hRgn = NewRgn() ) != nil )
					{
						gScroll.m_dy -= 10;
						rect = gMainWindow->portRect;
						rect.left = rect.right - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						rect.top = rect.bottom - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						ScrollRect( &rect, 0, -10, hRgn);
						rect.top = rect.bottom - 10;
	          	  	InvalRect( &rect ); 
	          	  	InvalRgn( hRgn ); 
	          	  	DisposeRgn( hRgn );
    					_UpdateGraphic();                  
					}
				}
				break;

			case 0x1F :				// down arrow
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					if ( ( hRgn = NewRgn() ) != nil )
					{
						gScroll.m_dy += 10;
						rect = gMainWindow->portRect;
						rect.left = rect.right - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						rect.top = rect.bottom - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						ScrollRect( &rect, 0, 10, hRgn);
						rect.bottom = rect.top + 10;
	          	  	InvalRect( &rect ); 
	          	  	InvalRgn( hRgn ); 
	            	DisposeRgn( hRgn );
    					_UpdateGraphic();                  
					}
				}
				break;

			case 0x1C :				// left arrow
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					if ( ( hRgn = NewRgn() ) != nil )
					{
						gScroll.m_dx -= 10;
						rect = gMainWindow->portRect;
						rect.left = rect.right - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						rect.top = rect.bottom - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						ScrollRect( &rect, -10, 0, hRgn);
						rect.left = rect.right - 10;
	          	  	InvalRect( &rect ); 
	          	  	InvalRgn( hRgn ); 
	            	DisposeRgn( hRgn );
    					_UpdateGraphic();                  
					}
				}
				break;

			case 0x1D :				// right arrow
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					if ( ( hRgn = NewRgn() ) != nil )
					{
						gScroll.m_dx += 10;
						rect = gMainWindow->portRect;
						rect.left = rect.right - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						rect.top = rect.bottom - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						ScrollRect( &rect, 10, 0, hRgn);
						rect.right = rect.left + 10;
	          	  	InvalRect( &rect ); 
	          	  	InvalRgn( hRgn ); 
	            	DisposeRgn( hRgn );
    					_UpdateGraphic();                  
					}
				}
				break;

			case '-' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					gpHeadline->Redefine();
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'h' :
			case 'H' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					_eCompensation = gpHeadline->GetCharCompensation();
				   switch ( _eCompensation ) 
				   {
				       case kRegular :
							if ( shifted )
								_eCompensation = kLoose;
							else
								_eCompensation = kTight;
				          break;
				       case kTight :
							if ( shifted )
								_eCompensation = kRegular;
							else
								_eCompensation = kLoose;
				          break;
				       case kLoose :
							if ( shifted )
								_eCompensation = kTight;
							else
								_eCompensation = kRegular;
				          break;
		   		    default :
							_eCompensation = kRegular;
		       		   break;
		 		   }
					gpHeadline->ApplyCharCompensation( _eCompensation );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 't' :
			case 'T' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					_eLineWeight = gpHeadline->GetLineWeight( );
				   switch ( _eLineWeight ) 
				   {
				       case kHairLine :
							if ( shifted )
								_eLineWeight = kExtraThickLine;
							else
								_eLineWeight = kThinLine;
				          break;
				       case kThinLine :
							if ( shifted )
								_eLineWeight = kHairLine;
							else
								_eLineWeight = kMediumLine;
				          break;
				       case kMediumLine :
							if ( shifted )
								_eLineWeight = kThinLine;
							else
								_eLineWeight = kThickLine;
				          break;
				       case kThickLine :
							if ( shifted )
								_eLineWeight = kMediumLine;
							else
								_eLineWeight = kExtraThickLine;
				          break;
				       case kExtraThickLine :
							if ( shifted )
								_eLineWeight = kThickLine;
							else
								_eLineWeight = kHairLine;
				          break;
		   		    default :
							_eLineWeight = kHairLine;
		       		   break;
		 		   }
					gpHeadline->ApplyLineWeight( _eLineWeight );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'L' :
			case 'l' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					_fLeading = !gpHeadline->IsLineLeading();
					gpHeadline->EnableLineLeading( _fLeading );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'j' :
			case 'J' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					_eJustification = gpHeadline->GetTextJustification();
				   switch ( _eJustification ) 
				   {
				       case kLeftJust :
							if ( shifted )
							{
								if ( gpHeadline->GetDistortEffect() == kFollowPath )
									_eJustification = kRightJust;
								else
									_eJustification = kFullJust;
							}
							else
								_eJustification = kRightJust;
				         break;
				       case kRightJust :
							if ( shifted )
								_eJustification = kLeftJust;
							else
								_eJustification = kCenterJust;
				         break;
				       case kCenterJust :
							if ( shifted )
								_eJustification = kRightJust;
							else
							{
								if ( gpHeadline->GetDistortEffect() == kFollowPath )
									_eJustification = kLeftJust;
								else
									_eJustification = kFullJust;
							}
				         break;
				       case kFullJust :
							if ( shifted )
								_eJustification = kCenterJust;
							else
								_eJustification = kLeftJust;
				         break;
		   		    default :
							_eJustification = kLeftJust;
		       		   break;
		 		   }
					gpHeadline->ApplyTextJustification( _eJustification );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'M' :
			case 'm' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
			//		_eVerticality = gpHeadline->??();
				   switch ( _eVerticality ) 
				   {
				       case kPin2Top :
							_eVerticality = kPin2Middle;
				          break;
				       case kPin2Middle :
							_eVerticality = kPin2Bottom;
				          break;
				       case kPin2Bottom :
							_eVerticality = kPin2Top;
				          break;
		   		    default :
							_eVerticality = kPin2Middle;
		       		   break;
		 		   }
					gpHeadline->ApplyVerticalPlacement( _eVerticality );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'K' :
			case 'k' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					_fKerning = !gpHeadline->IsCharKerning();
					gpHeadline->EnableCharKerning( _fKerning );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'D' :
			case 'd' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					_fDistort = !gpHeadline->IsDistortMode();
					gpHeadline->ApplyDistort( _fDistort );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'V' :
			case 'v' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					if ( gpHeadline->IsHorizontal() )
						gpHeadline->ApplyTextEscapement( kTop2BottomLeft2Right );
					else
						gpHeadline->ApplyTextEscapement( kLeft2RightTop2Bottom );
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'P' :
			case 'p' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					gfShowPathPoints = !gfShowPathPoints;
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'F' :
			case 'f' :
				if ( gpHeadline != NULL )
				{
					gfFeedback = !gfFeedback;
	       	   InvalRect( &(gMainWindow->portRect) ); 
				}
				else
					gfFeedback = false;
				break;

			case 'B' :
			case 'b' :
				if ( gpHeadline != NULL )
				{
					SetCursor( *(GetCursor( watchCursor )) );
					if ( gpHeadline->GetFrameEffect( ) == kSimpleFrame )
						gpHeadline->ApplyFrameEffect( kFrameNoFrame );
					else
					{
						RHeadlineColor	fillColor( RSolidColor( kWhite ), RSolidColor( kRed ), kEast );
						gpHeadline->SetFrameFill( kSimpleFrame, RHeadlineColor( 	fillColor.GetSolidColor( 0.5 ) ) );
						gpHeadline->ApplyFrameEffect( kSimpleFrame );
					}
					gpHeadline->Construct( CalcDefaultHeadlineScreenSize(), drawingSurface );
					FillRect( &(gMainWindow->portRect), &qd.white );
	            InvalRect( &(gMainWindow->portRect) ); 
					SetCursor( &qd.arrow );
				}
				break;

			case 'G' :
			case 'g' :
				if ( gpGraphic == nil && gpHeadline == nil )  
				{
					if ( gpGradientBackdrop == nil )  	
					{
						static RGradientRampPoint	RampPoints[] =
						{
							//						  color,				midpoint,				ramppoint
							//--------------------------------------------------------------------------
							RGradientRampPoint( RSolidColor(kBlue),	YPercentage(0.00),	YPercentage(0.0000) ),
							RGradientRampPoint( RSolidColor(kRed),		YPercentage(0.50),	YPercentage(0.1666) ),
							RGradientRampPoint( RSolidColor(kGreen),	YPercentage(0.50),	YPercentage(0.5000) ),
							RGradientRampPoint( RSolidColor(kBlue),	YPercentage(1.00),	YPercentage(0.8666) ),
							RGradientRampPoint( RSolidColor(kRed),		YPercentage(0.00),	YPercentage(1.0000) )
						};
						static RGradientRamp	GradientRamp( kLinear, RampPoints, ( sizeof(RampPoints) / sizeof(RampPoints[0]) ) );
						static YPercentage	pctRampLen( 0.5 );
						R2dTransform	xform;
						gpGradientBackdrop = new RGradientFill( kLinear,						// gradient type
														new RGradientRamp( GradientRamp ),			// ramp data
														RIntPoint( 0, 0 ),								// ramp vector origin
														0.0,													// ramp vector angle
														100.0,												// ramp vector length
														xform,												// gradient transformation
														0.0,													// hilite position
														0.0 );												// hilite angle
					}
					else
					{
						delete const_cast<RGradientRamp*>( gpGradientBackdrop->GetRamp( ) );
						delete gpGradientBackdrop;
						gpGradientBackdrop = NULL;
					}
				}
				else if ( gpGradientBackdrop != nil )  	
				{
					delete const_cast<RGradientRamp*>( gpGradientBackdrop->GetRamp( ) );
					delete gpGradientBackdrop;
					gpGradientBackdrop = NULL;
				}
				FillRect( &(gMainWindow->portRect), &qd.white );
	         InvalRect( &(gMainWindow->portRect) ); 
				break;
				
			case 0x09 :										// tab
				FillRect( &(gMainWindow->portRect), &qd.white );
	         InvalRect( &(gMainWindow->portRect) ); 
				break;
				
			default:
				HandleMenu( MenuKey( key ), _Event.modifiers );
				break;
		}
	}
	else
	{
		switch ( key )
		{
			case 0x1E :				// up arrow
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					if ( ( hRgn = NewRgn() ) != nil )
					{
						gScroll.m_dy -= 1;
						rect = gMainWindow->portRect;
						rect.left = rect.right - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						rect.top = rect.bottom - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						ScrollRect( &rect, 0, -1, hRgn);
						rect.top = rect.bottom - 1;
	          	  	InvalRect( &rect ); 
	          	  	InvalRgn( hRgn ); 
	          	  	DisposeRgn( hRgn );
    					PaintMainWindow();                  
					}
				}
				break;
			case 0x1F :				// down arrow
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					if ( ( hRgn = NewRgn() ) != nil )
					{
						gScroll.m_dy += 1;
						rect = gMainWindow->portRect;
						rect.left = rect.right - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						rect.top = rect.bottom - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						ScrollRect( &rect, 0, 1, hRgn);
						rect.bottom = rect.top + 1;
	          	  	InvalRect( &rect ); 
	          	  	InvalRgn( hRgn ); 
	            	DisposeRgn( hRgn );
    					PaintMainWindow();                  
					}
				}
				break;
			case 0x1C :				// left arrow
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					if ( ( hRgn = NewRgn() ) != nil )
					{
						gScroll.m_dx -= 1;
						rect = gMainWindow->portRect;
						rect.left = rect.right - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						rect.top = rect.bottom - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						ScrollRect( &rect, -1, 0, hRgn);
						rect.left = rect.right - 1;
	          	  	InvalRect( &rect ); 
	          	  	InvalRgn( hRgn ); 
	            	DisposeRgn( hRgn );
    					PaintMainWindow();                  
					}
				}
				break;
			case 0x1D :				// right arrow
				if ( gpHeadline != NULL || gpGraphic != nil )
				{
					if ( ( hRgn = NewRgn() ) != nil )
					{
						gScroll.m_dx += 1;
						rect = gMainWindow->portRect;
						rect.left = rect.right - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						rect.top = rect.bottom - 15;
						FillRect( &rect, &qd.white );
						rect = gMainWindow->portRect;
						ScrollRect( &rect, 1, 0, hRgn);
						rect.right = rect.left + 1;
	          	  	InvalRect( &rect ); 
	          	  	InvalRgn( hRgn ); 
	            	DisposeRgn( hRgn );
    					PaintMainWindow();                  
					}
				}
				break;
			default:
//***          SysBeep(5);  
				break;
		}
	}
}


/* ----------------------------------------------------------------------------
   _HandleUpdateEvent 
   Process update event. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _HandleUpdateEvent (EventRecord *pEvent)
{
GrafPtr              savePort;
WindowPtr            whichWindow = (WindowPtr)pEvent->message;

    GetPort(&savePort);
    SetPort(whichWindow);
    BeginUpdate(whichWindow);
    if (whichWindow == gMainWindow)
       PaintMainWindow();                  
    ValidRect(&whichWindow->portRect);
    EndUpdate(whichWindow);
    SetPort(savePort);
}


/* ----------------------------------------------------------------------------
   _HandleResumeEvent 
   Process resume event. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _HandleResumeEvent()
{
    gInForeground = true;
    if ( gMainWindow != nil )
    {
       PaintMainWindow();                  
       ShowWindow( gMainWindow );
       SelectWindow( gMainWindow );
    }
    _HandleActivateEvent( FrontWindow() );
}


/* ----------------------------------------------------------------------------
   _HandleSuspendEvent 
   Process suspend event. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _HandleSuspendEvent()
{
    gInForeground = false;
//    if ( gMainWindow != nil )
//       HideWindow( gMainWindow );
}


/* ----------------------------------------------------------------------------
   _HandleActivateEvent 
   Process activate event. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _HandleActivateEvent (WindowPtr whichWindow)
{
   if ( (_Event.modifiers & activeFlag) != 0 )  
   	if ( whichWindow != nil )  
      	SetPort( whichWindow );
	TpsAssert( ( whichWindow == gMainWindow ), "Who's window is this?" );
   AdjustMenus();
}


/* ----------------------------------------------------------------------------
   _HandleNullEvent 
   Process null event. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _HandleNullEvent()
{
    if ( !gInForeground )
       return;

}


/* ----------------------------------------------------------------------------
   _ClickInMainWindow 
   Process mouse click in main window. 
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _ClickInMainWindow (EventRecord *pEvent)
{
WindowPtr            whichWindow = (WindowPtr)pEvent->message;
Point                location;

	if ( whichWindow == nil )  
		return;
	TpsAssert( ( whichWindow == gMainWindow ), "Who's window is this?" );
	SetPort( whichWindow );
	location = pEvent->where;
	GlobalToLocal( &location );





}

/* ----------------------------------------------------------------------------
   _HandleGrowWindow 
   
   Returns nothing.                                        
   ---------------------------------------------------------------------------- */
static  void  _HandleGrowWindow (WindowPtr window)
{
Rect                 rect;
long                 size;

    SetRect( &rect, 100, 50, qd.screenBits.bounds.right, qd.screenBits.bounds.bottom );
    if ( (size = GrowWindow( window, _Event.where, &rect )) != 0 )
    {
    	SetPort( window );
    	SizeWindow( window, LoWord(size), HiWord(size), true );
		EraseRect( &window->portRect );
		PaintMainWindow();                  
    	ValidRect( &window->portRect );
    }
}


/* ----------------------------------------------------------------------------
   MainEventLoop 
   Application main event loop. 
   Returns nothing.               
   ---------------------------------------------------------------------------- */
void  MainEventLoop()
{
unsigned char        osSelect;
Point                pt;


    if ( !WaitNextEvent(everyEvent, &_Event, gSleepTime, nil) )
       _Event.what = nullEvent;
       
    switch ( _Event.what ) 
    {
       case nullEvent:
          _HandleNullEvent();
          break;
                    
       case updateEvt:
          _HandleUpdateEvent( &_Event );
          break;
                  
       case activateEvt:
          _HandleActivateEvent( FrontWindow() );
          break;
                    
       case mouseDown:
          _HandleMouseDown();
          break;
                    
       case keyDown: 
       case autoKey:
          _HandleKeyDown();
          break;
                    
       case osEvt:
          if ( (osSelect = (_Event.message >> 24)) == suspendResumeMessage )
          {
             if ( _Event.message & resumeFlag )
                _HandleResumeEvent();
             else
                _HandleSuspendEvent();
          }
          else if ( osSelect == mouseMovedMessage )
          {
          }
          break;
                
       case kHighLevelEvent:
          AEProcessAppleEvent( &_Event );
          break;

       case diskEvt:
          if ( HiWord(_Event.message) != noErr ) 
          {
             SetPt( &pt, 0x0070, 0x0050 );
             if ( DIBadMount( pt, _Event.message ) != noErr ) 
             {
             }
          }
          break;

       default:
          break;
    }
}


/* ----------------------------------------------------------------------------
   AlertOnError 
   Display an error alert dialog.
   Returns nothing.               
   ---------------------------------------------------------------------------- */
void  AlertOnError (sWORD severity, TERRCODE error, BOOLEAN fatal)
{
Handle               hdl;
unsigned char        theStr[256];
unsigned char        theNumber[12];
short                Id;

/**  no error  **/
    if (error == noErr)  
      return;

/**  construct error message  **/
    hdl = GetResource('ErrC', error);   
    if (ResError() == noErr && hdl != nil)  
    {
       HLock(hdl);
       BlockMove(*hdl, theStr, GetHandleSize(hdl));
       HUnlock(hdl);
       HPurge(hdl);
    }
    else
    {
       NumToString((long)error, theNumber);
       BlockMove("\perror#\0", theStr, 8L);
       BlockMove(&theNumber[1], &theStr[theStr[0] + 1], (Size)theNumber[0]); 
       theStr[0] = theStr[0] + theNumber[0];
    }

/**  select alert to display  **/
    if (error > 19500)
       Id = rGenericALRT;
    else if (error > 17000)
       Id = rApplicALRT;
    else
       Id = rSystemALRT;

/**  display alert with message  **/
    SetCursor(&qd.arrow);
    ParamText("\p", "\p", "\p", theStr);
    if (severity == cautionIcon)
       CautionAlert(Id, nil);
    else if (severity == noteIcon)
       NoteAlert(Id, nil);
    else
       StopAlert(Id, nil);

/**  terminate  **/
   if (fatal)
      ExitToShell(); 
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

