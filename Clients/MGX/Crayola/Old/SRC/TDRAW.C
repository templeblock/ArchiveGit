// ( c ) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX	75081

#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"


extern HINSTANCE hInstAstral;

// used by select.c
int nScale = 12;			// perspective scale value		  

// local variables
static void OSDrawShapeDataProc( int y, int left, int right, LPTR lpDst, LPTR lpSrc, int depth );
static void OSDrawShapeMaskProc( int y, int left, int right, LPTR lpMsk );
static LPFRAME OSBrushShape( LPSHAPE lpShape, HMGXBRUSH hBrush, LPRECT lpOutRect, BOOL fClosed );

static LPMASK lpDrawMask;
static POINT DrawMaskOffset;
static RECT SelectRect, SelectRectNext;
static LPFRAME lpExteriorFrame;
static RECT ExteriorRect;
static LPTR lpColorLuts[4];
static BOOL fShift;

static int iTool;
static int nSnap = 12;				// default grid spacing 
static int nButtonClicks = 1;		// mouse button counter for 3D shapes
static BOOL bWireFrame = NO;		// filled on by default 
static BOOL b3D = NO;				// 2D shapes on by default


// !!! REMOVE THIS AT YOUR OWN RISK !!!
// There is a possible optimization problem that causes this to trap
#pragma optimize("",off)

//////////////////////////////////////////////////////////////////////////////
BOOL WINPROC EXPORT DlgDrawShapesProc(
//////////////////////////////////////////////////////////////////////////////
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
COLORINFO ColorStruct;

	switch ( msg )
	{
		case WM_INITDIALOG:
			RibbonInit( hDlg );
			ColorInit( hDlg );
			UpdateRibbon( hDlg );

			CheckRadioButton( hDlg, IDC_SHAPES_T1, IDC_SHAPES_T10, 
				iTool+IDC_SHAPES_T1 );

			CheckDlgButton( hDlg, IDC_SHAPES_SW1, bWireFrame );
			CheckDlgButton( hDlg, IDC_SHAPES_SW2, b3D );

			ShieldsSave();
			Shields( OFF );

			nScale = GetDefaultInt( "scale", nScale );
			nSnap  = GetDefaultInt( "snap", nSnap );

			// default is snap turned on
			GridSave();						// save current grid
			Grid( ON, 0, 0, nSnap, nSnap ); // new grid

			// Fall thru...

		case WM_DOCACTIVATED:
		case WM_CONTROLENABLE:
			if ( !(Retouch.hBrush = CreateDefaultBrush( Retouch.hBrush )) )
				break;
			ColorStruct.ColorSpace = CS_GRAY;
			ColorStruct.gray = 0;
			SetMgxBrushColor( Retouch.hBrush, &ColorStruct );
			SetMgxBrushShape( Retouch.hBrush, IDC_BRUSHSQUARE, NULL );
			SetMgxBrushSize( Retouch.hBrush, 2 );
			SetSimpleBrushSpacing( Retouch.hBrush, 50 );
			break;


		case WM_SETCURSOR:
			return( SetupCursor( wParam, lParam, Tool.idRibbon ));


		case WM_ERASEBKGND:
			break;			// handle ERASEBKGND and do nothing
							// PAINT covers everything

		case WM_PAINT:
			LayoutPaint( hDlg );
			break;


		case WM_DESTROY:
			ShieldsRestore();
			GridRestore();
			break;


		case WM_CLOSE:
			GridRestore();
			AstralDlgEnd( hDlg, FALSE );
			break;


		case WM_CTLCOLOR:
			return( (BOOL)SetControlColors((HDC)wParam, hDlg,
				(HWND)LOWORD( lParam ), HIWORD( lParam )));


		case WM_COMMAND:
			switch( wParam )
			{
				case IDC_ACTIVECOLOR:
				case IDC_COLORS_SHAPES:
					ColorCommand( hDlg, wParam, lParam );
					break;

				case IDC_SHAPES_T1:
				case IDC_SHAPES_T2:
				case IDC_SHAPES_T3:
				case IDC_SHAPES_T4:
				case IDC_SHAPES_T5:
				case IDC_SHAPES_T6:
				case IDC_SHAPES_T7:
				case IDC_SHAPES_T8:
				case IDC_SHAPES_T9:
				case IDC_SHAPES_T10:
					CheckRadioButton( hDlg, IDC_SHAPES_T1, IDC_SHAPES_T10, wParam );
					iTool = wParam - IDC_SHAPES_T1;
					break;


				case IDC_SHAPES_SW1:
					bWireFrame	= !bWireFrame;
					CheckDlgButton( hDlg, wParam, bWireFrame );

					UpdateRibbon( hDlg );
					break;


				case IDC_SHAPES_SW2:
					b3D	 = !b3D;
					CheckDlgButton( hDlg, wParam, b3D );

					UpdateRibbon( hDlg );
					break;


				default:
					return( FALSE );
			}
		default:
			return( FALSE );
	}
	return( TRUE );
}


//////////////////////////////////////////////////////////////////////////////
int FAR DrawShapesProc(
//////////////////////////////////////////////////////////////////////////////
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
	static int Style;

	if(( iTool >= 0 && iTool <= 3 ) ||		// closed geometric shapes
	   ( iTool >= 5 && iTool <= 8 ))
	{
		if( b3D )							// draw 3D
			return( DrawExtrudedGeometricProc( hWindow, lParam, msg ));
		else								// draw 2D
			return( DrawGeometricProc( hWindow, lParam, msg ));
	}

	if( iTool == 4 || iTool == 9 )			// freehand open, freehand closed, cones
	{
		if ( b3D )							// cones
			return( DrawExtrudedGeometricProc( hWindow, lParam, msg ));
		else								// freehand shapes
		{
			// turn off snap for freehand shapes  
			Grid( OFF, 0, 0, nSnap, nSnap ); 

			return( DrawFreehandProc( hWindow, lParam, msg ));
		}
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
int FAR DrawGeometricProc(
//////////////////////////////////////////////////////////////////////////////
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
	int x, y;
	POINT Points[12];
	LPSHAPE lpShape;
	int nPoints;
	static int Style;
	POINT deltas, mids;

	x = LOWORD( lParam );
	y = HIWORD( lParam );

	switch ( msg )
	{
		case WM_CREATE: // The first mouse down message
			if (!ONIMAGE(x, y))
				break;
			Display2File(&x, &y);
			if ( !ImgSelectObjectType( lpImage, x, y,
				YES/*bAllowBase*/, YES/*bAllowMovable*/, NO/*bAllowPlayable*/ ) )
					break;
			if( !Retouch.hBrush )
				break;

			AstralUpdateWindow( hWindow );

			Tool.bActive = YES;

			break;

		case WM_SETCURSOR:
		return( SetToolCursor( hWindow, NULL/*idMoveObject*/, ID_PLAY_OBJECT ) );

		case WM_ACTIVATE:
			if( !lParam ) // a deactivate
			{
				if( Retouch.hBrush )
					DestroyMgxBrush( Retouch.hBrush );

				Retouch.hBrush = NULL;
			}
			else
			{			// an activate ( to re-create brush )
				if( Tool.hRibbon )
					SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
			}
			break;
	

		case WM_LBUTTONDOWN:
			fShift = NO;

			SetRect( &SelectRect, 32767, 32767, -32767, -32767 );

			if( iTool == 0 )	Style = SL_BOX;			else
			if( iTool == 1 )	Style = SL_ELLIPSE;		else
			if( iTool == 2 )	Style = SL_HTRIANGLE;	else
			if( iTool == 3 )	Style = SL_VTRIANGLE;	else
			if( iTool == 5 )	Style = SL_HEXAGON;		else
			if( iTool == 6 )	Style = SL_OCTAGON;		else
			if( iTool == 7 )	Style = SL_HSEMICIRCLE; else
			if( iTool == 8 )	Style = SL_VSEMICIRCLE;		

			StartSelection( hWindow, NULL, &SelectRect, Style | SL_SPECIAL, 
							MAKEPOINT( lParam ), 0L );

			break;


		case WM_LBUTTONUP:
			Tool.bActive = NO;

			EndSelection( hWindow, NULL, &SelectRect, Style, YES );

			// set minimum shape size
			if( abs( SelectRect.bottom - SelectRect.top ) < nSnap )
				SelectRect.bottom = SelectRect.top + nSnap;

			if( abs( SelectRect.right - SelectRect.left ) < nSnap )
				SelectRect.right = SelectRect.left + nSnap;

			if( Style == SL_ELLIPSE )
				DrawEllipse( &SelectRect, IDS_UNDOCHANGE );
			else
			{
				if( Style == SL_BOX )
				{
					Points[0].x = SelectRect.left;
					Points[0].y = SelectRect.top;
					Points[1].x = SelectRect.right;
					Points[1].y = SelectRect.top;
					Points[2].x = SelectRect.right;
					Points[2].y = SelectRect.bottom;
					Points[3].x = SelectRect.left;
					Points[3].y = SelectRect.bottom;

					nPoints = 4;
				}
				else
				if( Style == SL_HTRIANGLE )
				{
					Points[0].x = SelectRect.left;
					Points[0].y = SelectRect.top;
					Points[1].x = SelectRect.left;
					Points[1].y = SelectRect.bottom;
					Points[2].x = SelectRect.right;
					Points[2].y = ( SelectRect.top + SelectRect.bottom ) / 2;

					nPoints = 3;
				}
				else
				if( Style == SL_VTRIANGLE )
				{
					Points[0].x = SelectRect.left;
					Points[0].y = SelectRect.bottom;
					Points[1].x = SelectRect.right;
					Points[1].y = SelectRect.bottom;
					Points[2].x = ( SelectRect.left + SelectRect.right ) / 2;
					Points[2].y = SelectRect.top;

					nPoints = 3;
				}
				else if( Style == SL_HEXAGON )
				{
					Points[0].x = SelectRect.left;
					Points[0].y = ( SelectRect.top + SelectRect.bottom ) / 2;
					Points[1].x = SelectRect.left + ( SelectRect.right - SelectRect.left ) / 4; 
					Points[1].y = SelectRect.bottom;
					Points[2].x = SelectRect.left + 3 * ( SelectRect.right - SelectRect.left ) / 4;
					Points[2].y = SelectRect.bottom;
					Points[3].x = SelectRect.right;
					Points[3].y = ( SelectRect.top + SelectRect.bottom ) / 2;
					Points[4].x = SelectRect.left + 3 * ( SelectRect.right - SelectRect.left ) / 4;
					Points[4].y = SelectRect.top;
					Points[5].x = SelectRect.left + ( SelectRect.right - SelectRect.left ) / 4;
					Points[5].y = SelectRect.top;

					nPoints = 6;
				}
				else 
				if( Style == SL_OCTAGON ) 
				{
					Points[0].x = SelectRect.left;
					Points[0].y = SelectRect.top + ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 );
					Points[1].x = SelectRect.left;
					Points[1].y = SelectRect.bottom - ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 );
					Points[2].x = SelectRect.left + ( 3 * ( SelectRect.right - SelectRect.left ) / 10 );
					Points[2].y = SelectRect.bottom;
					Points[3].x = SelectRect.right - ( 3 * ( SelectRect.right - SelectRect.left ) / 10 );
					Points[3].y = SelectRect.bottom;
					Points[4].x = SelectRect.right;
					Points[4].y = SelectRect.bottom - ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 );
					Points[5].x = SelectRect.right;
					Points[5].y = SelectRect.top + ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 );
					Points[6].x = SelectRect.right - ( 3 * ( SelectRect.right - SelectRect.left ) / 10 );
					Points[6].y = SelectRect.top;
					Points[7].x = SelectRect.left + ( 3 * ( SelectRect.right - SelectRect.left ) / 10 );
					Points[7].y = SelectRect.top;

					nPoints = 8;
				}
				else
				if( Style == SL_HSEMICIRCLE )
				{
					deltas.x = ( 2 * (SelectRect.right - SelectRect.left)) / 9;
					deltas.y = ( 2 * (SelectRect.bottom - SelectRect.top)) / 9;
					mids.x = SelectRect.left;
					mids.y = ( SelectRect.top + SelectRect.bottom ) / 2;

					// upper right bezier arc
					Points[0].x = BEZIER_MARKER;
					Points[0].y = 4;
					Points[1].x = mids.x;
					Points[1].y = SelectRect.top;
					Points[2].x = SelectRect.right - deltas.x;
					Points[2].y = SelectRect.top;
					Points[3].x = SelectRect.right;
					Points[3].y = SelectRect.top + deltas.y;
					Points[4].x = SelectRect.right;
					Points[4].y = mids.y;

					// lower right bezier arc
					Points[5].x = BEZIER_MARKER;
					Points[5].y = 4;
					Points[6].x = SelectRect.right;
					Points[6].y = mids.y;
					Points[7].x = SelectRect.right;
					Points[7].y = SelectRect.bottom - deltas.y;
					Points[8].x = SelectRect.right - deltas.x;
					Points[8].y = SelectRect.bottom;
					Points[9].x = mids.x;
					Points[9].y = SelectRect.bottom;

					nPoints = 10;
				}
				else
				if( Style == SL_VSEMICIRCLE )
				{
					deltas.x = ( 2 * (SelectRect.right - SelectRect.left)) / 9;
					deltas.y = ( 2 * (SelectRect.bottom - SelectRect.top)) / 9;
					mids.x = ( SelectRect.left + SelectRect.right ) / 2;
					mids.y = SelectRect.bottom;

					// upper left bezier arc
					Points[0].x = BEZIER_MARKER;
					Points[0].y = 4;
					Points[1].x = SelectRect.left;
					Points[1].y = mids.y;
					Points[2].x = SelectRect.left;
					Points[2].y = SelectRect.top + deltas.y;
					Points[3].x = SelectRect.left + deltas.x;
					Points[3].y = SelectRect.top;
					Points[4].x = mids.x;
					Points[4].y = SelectRect.top;

					// upper right bezier arc
					Points[5].x = BEZIER_MARKER;
					Points[5].y = 4;
					Points[6].x = mids.x;
					Points[6].y = SelectRect.top;
					Points[7].x = SelectRect.right - deltas.x;
					Points[7].y = SelectRect.top;
					Points[8].x = SelectRect.right;
					Points[8].y = SelectRect.top + deltas.y;
					Points[9].x = SelectRect.right;
					Points[9].y = mids.y;

					nPoints = 10;
				}

				if( lpShape = MaskCreateShapes( Points, &nPoints, 1, SHAPE_ADD, YES, NULL ))
				{
					DrawShapeClosed( lpShape, IDS_UNDOCHANGE );
					FreeUpShapes( lpShape );
				}
				else
					Message( IDS_EMEMALLOC );
			}
			break;


		case WM_MOUSEMOVE:	// sent when ToolActive is on
			fShift = SHIFT;

			UpdateSelection( hWindow, NULL, &SelectRect, Style, 
							 MAKEPOINT( lParam ), CONSTRAINASPECT, 1L, 1L, 
							 Window.fRButtonDown, FROMCENTER );

			break;


		case WM_LBUTTONDBLCLK:
			break;


		case WM_DESTROY:	// The cancel operation message
			if( !Tool.bActive )
				break;

			Tool.bActive = NO;

			EndSelection( hWindow, NULL, &SelectRect, Style, YES );
			break;

	}
	return( TRUE );
}


//////////////////////////////////////////////////////////////////////////
int FAR DrawExtrudedGeometricProc(
//////////////////////////////////////////////////////////////////////////
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
	LPSHAPE lpShape;
	POINT deltas, mids, apex;
	POINT PointsFront[20], PointsBack[4], PointsTop[20], PointsBottom[20], PointsLeft[4], PointsRight[4];
	int	  nPointsFront, nPointsBack, nPointsTop, nPointsBottom, nPointsLeft, nPointsRight;
	static int Style;
	int	  x, y;

	POINT dist;
	POINT ptCenterRect, ptCenterRectNext;

	float aspect;
	int distance, scale;

	POINT PointsCone[100];
	int nPoints, nPointsCone;
	int index, nPointCounter,offset;
	float LineSlope, VectorSlope;
	LPPOINT lpPoints;
	LPOBJECT lpObject;
	RECT rUndo;

	x = LOWORD( lParam );
	y = HIWORD( lParam );

	switch( msg )
	{
		case WM_CREATE:				// The first mouse down message
			if (!ONIMAGE(x, y))
				break;
			Display2File(&x, &y);
			if ( !ImgSelectObjectType( lpImage, x, y,
				YES/*bAllowBase*/, YES/*bAllowMovable*/, NO/*bAllowPlayable*/ ) )
					break;
			if( !Retouch.hBrush )
				break;

			AstralUpdateWindow( hWindow );

			Tool.bActive = YES;
			break;

		case WM_SETCURSOR:
		return( SetToolCursor( hWindow, NULL/*idMoveObject*/, ID_PLAY_OBJECT ) );

		case WM_ACTIVATE:
			if( !lParam )				//	deactivate
			{
				if( Retouch.hBrush )
					DestroyMgxBrush( Retouch.hBrush );

				Retouch.hBrush = NULL;
			}
			else						// activate( to re-create brush )
			{					
				if( Tool.hRibbon )
					SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
			}
			break;
	

		case WM_LBUTTONDOWN:
			fShift	 = NO;

			if( nButtonClicks == 1 )	// if first button click, start shape
			{
				SetRect( &SelectRect, 32767, 32767, -32767, -32767 );

				if( iTool == 0 )	Style = SL_CUBE;		else
				if( iTool == 1 )	Style = SL_CYLINDER;	else
				if( iTool == 2 )	Style = SL_PYRAMID;		else
				if( iTool == 3 )	Style = SL_WEDGE;		else
				if( iTool == 4 )	Style = SL_HCONE;		else
				if( iTool == 5 )	Style = SL_PCUBE;		else
				if( iTool == 6 )	Style = SL_PCYLINDER;	else
				if( iTool == 7 )	Style = SL_PPYRAMID;	else
				if( iTool == 8 )	Style = SL_PWEDGE;		else
				if( iTool == 9 )	Style = SL_VCONE;	   

				StartSelection( hWindow, NULL, &SelectRect, Style | SL_SPECIAL, 
					MAKEPOINT( lParam ), 0L );
			}
			break;


		case WM_LBUTTONUP:
			if( nButtonClicks == 1 )	  
			{
				EndSelection( hWindow, NULL, &SelectRect, Style, YES );

				// set minimum shape size
				if( abs( SelectRect.bottom - SelectRect.top ) < nSnap )
					SelectRect.bottom = SelectRect.top + nSnap;

				if( abs( SelectRect.right - SelectRect.left ) < nSnap )
					SelectRect.right = SelectRect.left + nSnap;

				SelectRectNext = SelectRect;

				if( Style == SL_HCONE || Style == SL_VCONE )
					InvertExtrudedSelection( hWindow, NULL, &SelectRect, &SelectRectNext, Style );

				nButtonClicks++;
			}
			else if( nButtonClicks == 2 )	   
			{
				Tool.bActive = NO;
				EndExtrudedSelection( hWindow, NULL, &SelectRect, &SelectRectNext, Style, YES );

				if ( !ImgEditInit( lpImage, ET_SELOBJECTS, UT_DATA, NULL ) )
						break;

				ptCenterRect.x = ( SelectRect.left + SelectRect.right ) / 2;
				ptCenterRect.y = ( SelectRect.top + SelectRect.bottom ) / 2;

				ptCenterRectNext.x = ( SelectRectNext.left + SelectRectNext.right ) / 2;
				ptCenterRectNext.y = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

				// perspective shapes
				if( Style == SL_PCUBE || Style == SL_PCYLINDER || Style == SL_PPYRAMID || Style == SL_PWEDGE )
				{
					dist.x = abs( ptCenterRectNext.x - (( SelectRect.left + SelectRect.right ) / 2 )); 
					dist.y = abs( ptCenterRectNext.y - (( SelectRect.top + SelectRect.bottom ) / 2 ));
					distance = max( dist.x, dist.y );
					scale = distance * nScale;

					// special case for these two styles when
					// apex points down
					if(( Style == SL_PPYRAMID || Style == SL_PWEDGE ) &&
						( SelectRectNext.bottom < SelectRectNext.top ))
					{
						SelectRectNext.top	+= 2 * scale / 100; 
						SelectRectNext.left -= 2 * scale / 100; 
					}
					else
					if( Style == SL_PCYLINDER )
					{
						// height/width
						aspect = ( float )( SelectRect.bottom - SelectRect.top ) /
								 ( float )( SelectRect.right - SelectRect.left );
   
						// if vertical
						if(( SelectRect.bottom - SelectRect.top ) <=
						   ( SelectRect.right - SelectRect.left ))
						{
							SelectRectNext.left	   -= (int)(scale / 100); 
							SelectRectNext.top	   -= (int)(scale * ( aspect / 100 )); 
							SelectRectNext.right   += (int)(scale / 100); 
							SelectRectNext.bottom  += (int)(scale * ( aspect / 100 ));
						}
						// if horizontal
						else
						{
							SelectRectNext.left	   -= (int)(scale / ( aspect * 100 )); 
							SelectRectNext.top	   -= (int)(scale / 100); 
							SelectRectNext.right   += (int)(scale / ( aspect * 100 )); 
							SelectRectNext.bottom  += (int)(scale / 100);
						}
					}
					else	// all other shapes and cases
					{
						SelectRectNext.left -= (int)(2 * scale / 100); 
						SelectRectNext.top	-= (int)(2 * scale / 100); 
					}
				}

				switch( Style )
				{
					case SL_CUBE:
					case SL_PCUBE:
						// front side
						PointsFront[0].x = SelectRectNext.left;
						PointsFront[0].y = SelectRectNext.top;
						PointsFront[1].x = SelectRectNext.right;
						PointsFront[1].y = SelectRectNext.top;
						PointsFront[2].x = SelectRectNext.right;
						PointsFront[2].y = SelectRectNext.bottom;
						PointsFront[3].x = SelectRectNext.left;
						PointsFront[3].y = SelectRectNext.bottom;
						nPointsFront = 4;

						// top side
						PointsTop[0].x = SelectRectNext.left;
						PointsTop[0].y = SelectRectNext.top;
						PointsTop[1].x = SelectRectNext.right;
						PointsTop[1].y = SelectRectNext.top;
						PointsTop[2].x = SelectRect.right;
						PointsTop[2].y = SelectRect.top;
						PointsTop[3].x = SelectRect.left;
						PointsTop[3].y = SelectRect.top;
						nPointsTop = 4;

						// bottom side
						PointsBottom[0].x = SelectRectNext.left;
						PointsBottom[0].y = SelectRectNext.bottom;
						PointsBottom[1].x = SelectRectNext.right;
						PointsBottom[1].y = SelectRectNext.bottom;
						PointsBottom[2].x = SelectRect.right;
						PointsBottom[2].y = SelectRect.bottom;
						PointsBottom[3].x = SelectRect.left;
						PointsBottom[3].y = SelectRect.bottom;
						nPointsBottom = 4;

						// left side
						PointsLeft[0].x = SelectRectNext.left;
						PointsLeft[0].y = SelectRectNext.bottom;
						PointsLeft[1].x = SelectRectNext.left;
						PointsLeft[1].y = SelectRectNext.top;
						PointsLeft[2].x = SelectRect.left;
						PointsLeft[2].y = SelectRect.top;
						PointsLeft[3].x = SelectRect.left;
						PointsLeft[3].y = SelectRect.bottom;
						nPointsLeft = 4;

						// right side
						PointsRight[0].x = SelectRectNext.right;
						PointsRight[0].y = SelectRectNext.bottom;
						PointsRight[1].x = SelectRectNext.right;
						PointsRight[1].y = SelectRectNext.top;
						PointsRight[2].x = SelectRect.right;
						PointsRight[2].y = SelectRect.top;
						PointsRight[3].x = SelectRect.right;
						PointsRight[3].y = SelectRect.bottom;
						nPointsRight = 4;

						// if centered, draw only front
						if(( ptCenterRectNext.y == ptCenterRect.y ) && ( ptCenterRectNext.x == ptCenterRect.x ))
						{
							// draw front
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}

						// if 1st quadrant
						else
						if(( ptCenterRectNext.y <= ptCenterRect.y ) && ( ptCenterRectNext.x >= ptCenterRect.x ))
						{
							if( bWireFrame )	// draw hidden sides
							{
								// draw Top 
								if( lpShape = MaskCreateShapes( PointsTop, &nPointsTop, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
						
								// draw right 
								if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw left side
							if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw bottom 
							if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}

						// if 2nd quadrant
						else if(( ptCenterRectNext.y <= ptCenterRect.y ) && ( ptCenterRectNext.x <= ptCenterRect.x ))
						{
							if( bWireFrame )	// draw hidden sides
							{
								// draw top 
								if( lpShape = MaskCreateShapes( PointsTop, &nPointsTop, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );

								// draw left side
								if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw right side 
							if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw bottom 
							if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front 
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}

						// if 3rd quadrant
						else if(( ptCenterRectNext.y >= ptCenterRect.y ) && ( ptCenterRectNext.x <= ptCenterRect.x ))
						{
							if( bWireFrame )	// draw hidden sides
							{
								// draw Bottom
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );

								// draw Left
								if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw right side 
							if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw top
							if( lpShape = MaskCreateShapes( PointsTop, &nPointsTop, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front 
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}

						// if 4th quadrant
						else if(( ptCenterRectNext.y >= ptCenterRect.y ) && ( ptCenterRectNext.x >= ptCenterRect.x ))
						{
							if( bWireFrame )	// draw hidden sides
							{
								// draw Bottom
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );

								// draw right side
								if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw left side
							if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw top
							if( lpShape = MaskCreateShapes( PointsTop, &nPointsTop, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front 
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}
						break;


					case SL_CYLINDER:
					case SL_PCYLINDER:
						// if vertical
						if(( SelectRect.bottom - SelectRect.top ) <= ( SelectRect.right - SelectRect.left ))
						{
							// if dragged down 
							if( ptCenterRectNext.y >= ptCenterRect.y )
							{
								deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
								deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
								mids.x	 = ( SelectRectNext.left + SelectRectNext.right ) / 2;
								mids.y	 = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

								// bottom lower right bezier
								PointsFront[0].x = BEZIER_MARKER;
								PointsFront[0].y = 4;
								PointsFront[1].x = SelectRectNext.right;
								PointsFront[1].y = mids.y;
								PointsFront[2].x = SelectRectNext.right;
								PointsFront[2].y = SelectRectNext.bottom - deltas.y;
								PointsFront[3].x = SelectRectNext.right - deltas.x;
								PointsFront[3].y = SelectRectNext.bottom;
								PointsFront[4].x = mids.x;
								PointsFront[4].y = SelectRectNext.bottom;

								// bottom lower left bezier
								PointsFront[5].x = BEZIER_MARKER;
								PointsFront[5].y = 4;
								PointsFront[6].x = mids.x;
								PointsFront[6].y = SelectRectNext.bottom;
								PointsFront[7].x = SelectRectNext.left + deltas.x;
								PointsFront[7].y = SelectRectNext.bottom;
								PointsFront[8].x = SelectRectNext.left;
								PointsFront[8].y = SelectRectNext.bottom - deltas.y;
								PointsFront[9].x = SelectRectNext.left;
								PointsFront[9].y = mids.y;

								// recalculate for top 1/2 bezier 
								deltas.x = ( 2 * ( SelectRect.right - SelectRect.left )) / 9;
								deltas.y = ( 2 * ( SelectRect.bottom - SelectRect.top )) / 9;
								mids.x	 = ( SelectRect.left + SelectRect.right ) / 2;
								mids.y	 = ( SelectRect.top + SelectRect.bottom ) / 2;

								// Top lower left bezier
								PointsFront[10].x = BEZIER_MARKER;
								PointsFront[10].y = 4;
								PointsFront[11].x = SelectRect.left;
								PointsFront[11].y = mids.y;
								PointsFront[12].x = SelectRect.left;
								PointsFront[12].y = SelectRect.bottom - deltas.y;
								PointsFront[13].x = SelectRect.left + deltas.x;
								PointsFront[13].y = SelectRect.bottom;
								PointsFront[14].x = mids.x;
								PointsFront[14].y = SelectRect.bottom;

								// Top lower right bezier
								PointsFront[15].x = BEZIER_MARKER;
								PointsFront[15].y = 4;
								PointsFront[16].x = mids.x;
								PointsFront[16].y = SelectRect.bottom;
								PointsFront[17].x = SelectRect.right - deltas.x;
								PointsFront[17].y = SelectRect.bottom;
								PointsFront[18].x = SelectRect.right;
								PointsFront[18].y = SelectRect.bottom - deltas.y;
								PointsFront[19].x = SelectRect.right;
								PointsFront[19].y = mids.y;
							}
							else	// cylinder dragged up
							{
								deltas.x = ( 2 * ( SelectRect.right - SelectRect.left )) / 9;
								deltas.y = ( 2 * ( SelectRect.bottom - SelectRect.top )) / 9;
								mids.x	 = ( SelectRect.left + SelectRect.right ) / 2;
								mids.y	 = ( SelectRect.top + SelectRect.bottom ) / 2;

								// bottom upper right bezier
								PointsFront[0].x = BEZIER_MARKER;
								PointsFront[0].y = 4;
								PointsFront[1].x = SelectRect.right;
								PointsFront[1].y = mids.y;
								PointsFront[2].x = SelectRect.right;
								PointsFront[2].y = SelectRect.top + deltas.y;
								PointsFront[3].x = SelectRect.right - deltas.x;
								PointsFront[3].y = SelectRect.top;
								PointsFront[4].x = mids.x;
								PointsFront[4].y = SelectRect.top;

								// bottom upper left bezier
								PointsFront[5].x = BEZIER_MARKER;
								PointsFront[5].y = 4;
								PointsFront[6].x = mids.x;
								PointsFront[6].y = SelectRect.top;
								PointsFront[7].x = SelectRect.left + deltas.x;
								PointsFront[7].y = SelectRect.top;
								PointsFront[8].x = SelectRect.left;
								PointsFront[8].y = SelectRect.top + deltas.y;
								PointsFront[9].x = SelectRect.left;
								PointsFront[9].y = mids.y;
								
								// recalculate for top 1/2 bezier 
								deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
								deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
								mids.x	 = ( SelectRectNext.left + SelectRectNext.right ) / 2;
								mids.y	 = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

								// top upper left bezier
								PointsFront[10].x = BEZIER_MARKER;
								PointsFront[10].y = 4;
								PointsFront[11].x = SelectRectNext.left;
								PointsFront[11].y = mids.y;
								PointsFront[12].x = SelectRectNext.left;
								PointsFront[12].y = SelectRectNext.top + deltas.y;
								PointsFront[13].x = SelectRectNext.left + deltas.x;
								PointsFront[13].y = SelectRectNext.top;
								PointsFront[14].x = mids.x;
								PointsFront[14].y = SelectRectNext.top;

								// top upper right bezier
								PointsFront[15].x = BEZIER_MARKER;
								PointsFront[15].y = 4;
								PointsFront[16].x = mids.x;
								PointsFront[16].y = SelectRectNext.top;
								PointsFront[17].x = SelectRectNext.right - deltas.x;
								PointsFront[17].y = SelectRectNext.top;
								PointsFront[18].x = SelectRectNext.right;
								PointsFront[18].y = SelectRectNext.top + deltas.y;
								PointsFront[19].x = SelectRectNext.right;
								PointsFront[19].y = mids.y;
							}
						}
						else	// cylinder dragged horizontally
						{
							// if dragged right
							if( ptCenterRectNext.x >= ptCenterRect.x )
							{
								deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
								deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
								mids.x	 = ( SelectRectNext.left + SelectRectNext.right ) / 2;
								mids.y	 = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

								// right upper right bezier 
								PointsFront[0].x = BEZIER_MARKER;
								PointsFront[0].y = 4;
								PointsFront[1].x = mids.x;
								PointsFront[1].y = SelectRectNext.top;
								PointsFront[2].x = SelectRectNext.right - deltas.x;
								PointsFront[2].y = SelectRectNext.top;
								PointsFront[3].x = SelectRectNext.right;
								PointsFront[3].y = SelectRectNext.top + deltas.y;
								PointsFront[4].x = SelectRectNext.right;
								PointsFront[4].y = mids.y;

								// right lower right bezier
								PointsFront[5].x = BEZIER_MARKER;
								PointsFront[5].y = 4;
								PointsFront[6].x = SelectRectNext.right;
								PointsFront[6].y = mids.y;
								PointsFront[7].x = SelectRectNext.right;
								PointsFront[7].y = SelectRectNext.bottom - deltas.y;
								PointsFront[8].x = SelectRectNext.right - deltas.x;
								PointsFront[8].y = SelectRectNext.bottom;
								PointsFront[9].x = mids.x;
								PointsFront[9].y = SelectRectNext.bottom;

								// recalculate for top 1/2 bezier 
								deltas.x = ( 2 * ( SelectRect.right - SelectRect.left )) / 9;
								deltas.y = ( 2 * ( SelectRect.bottom - SelectRect.top )) / 9;
								mids.x	 = ( SelectRect.left + SelectRect.right ) / 2;
								mids.y	 = ( SelectRect.top + SelectRect.bottom ) / 2;

								// left lower left bezier
								PointsFront[10].x = BEZIER_MARKER;
								PointsFront[10].y = 4;
								PointsFront[11].x = mids.x;
								PointsFront[11].y = SelectRect.bottom;
								PointsFront[12].x = SelectRect.left + deltas.x;
								PointsFront[12].y = SelectRect.bottom;
								PointsFront[13].x = SelectRect.left;
								PointsFront[13].y = SelectRect.bottom - deltas.y;
								PointsFront[14].x = SelectRect.left;
								PointsFront[14].y = mids.y;

								// left upper left bezier
								PointsFront[15].x = BEZIER_MARKER;
								PointsFront[15].y = 4;
								PointsFront[16].x = SelectRect.left;
								PointsFront[16].y = mids.y;
								PointsFront[17].x = SelectRect.left;
								PointsFront[17].y = SelectRect.top + deltas.y;
								PointsFront[18].x = SelectRect.left + deltas.x;
								PointsFront[18].y = SelectRect.top;
								PointsFront[19].x = mids.x;
								PointsFront[19].y = SelectRect.top;
							}
							else	// cylinder dragged left
							{
								deltas.x = ( 2 * ( SelectRect.right - SelectRect.left )) / 9;
								deltas.y = ( 2 * ( SelectRect.bottom - SelectRect.top )) / 9;
								mids.x	 = ( SelectRect.left + SelectRect.right ) / 2;
								mids.y	 = ( SelectRect.top + SelectRect.bottom ) / 2;

								// right  upper left  bezier
								PointsFront[0].x = BEZIER_MARKER;
								PointsFront[0].y = 4;
								PointsFront[1].x = mids.x;
								PointsFront[1].y = SelectRect.top;
								PointsFront[2].x = SelectRect.left + deltas.x;
								PointsFront[2].y = SelectRect.top;
								PointsFront[3].x = SelectRect.left;
								PointsFront[3].y = SelectRect.top + deltas.y;
								PointsFront[4].x = SelectRect.left;
								PointsFront[4].y = mids.y;

								// right lower left bezier
								PointsFront[5].x = BEZIER_MARKER;
								PointsFront[5].y = 4;
								PointsFront[6].x = SelectRect.left;
								PointsFront[6].y = mids.y;
								PointsFront[7].x = SelectRect.left;
								PointsFront[7].y = SelectRect.bottom - deltas.y;
								PointsFront[8].x = SelectRect.left + deltas.x;
								PointsFront[8].y = SelectRect.bottom;
								PointsFront[9].x = mids.x;
								PointsFront[9].y = SelectRect.bottom;

								// recalculate for top 1/2 bezier 
								deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
								deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
								mids.x	 = ( SelectRectNext.left + SelectRectNext.right ) / 2;
								mids.y	 = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

								// left lower left bezier
								PointsFront[10].x = BEZIER_MARKER;
								PointsFront[10].y = 4;
								PointsFront[11].x = mids.x;
								PointsFront[11].y = SelectRectNext.bottom;
								PointsFront[12].x = SelectRectNext.left + deltas.x;
								PointsFront[12].y = SelectRectNext.bottom;
								PointsFront[13].x = SelectRectNext.left;
								PointsFront[13].y = SelectRectNext.bottom - deltas.y;
								PointsFront[14].x = SelectRectNext.left;
								PointsFront[14].y = mids.y;

								// left upper left bezier
								PointsFront[15].x = BEZIER_MARKER;
								PointsFront[15].y = 4;
								PointsFront[16].x = SelectRectNext.left;
								PointsFront[16].y = mids.y;
								PointsFront[17].x = SelectRectNext.left;
								PointsFront[17].y = SelectRectNext.top + deltas.y;
								PointsFront[18].x = SelectRectNext.left + deltas.x;
								PointsFront[18].y = SelectRectNext.top;
								PointsFront[19].x = mids.x;
								PointsFront[19].y = SelectRectNext.top;
							}
						}

						nPointsFront = 20;

						// draw cylinder front
						if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
						{
							DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
							FreeUpShapes( lpShape );
						}
						else
							Message( IDS_EMEMALLOC );

						deltas.x = ( 2 * ( SelectRect.right - SelectRect.left )) / 9;
						deltas.y = ( 2 * ( SelectRect.bottom - SelectRect.top )) / 9;
						mids.x	 = ( SelectRect.left + SelectRect.right ) / 2;
						mids.y	 = ( SelectRect.top + SelectRect.bottom ) / 2;

						// Draw cylinder top 
						// top right bezier arc
						PointsTop[0].x = BEZIER_MARKER;
						PointsTop[0].y = 4;
						PointsTop[1].x = mids.x;
						PointsTop[1].y = SelectRect.top;
						PointsTop[2].x = SelectRect.right - deltas.x;
						PointsTop[2].y = SelectRect.top;
						PointsTop[3].x = SelectRect.right;
						PointsTop[3].y = SelectRect.top + deltas.y;
						PointsTop[4].x = SelectRect.right;
						PointsTop[4].y = mids.y;

						// lower right bezier arc
						PointsTop[5].x = BEZIER_MARKER;
						PointsTop[5].y = 4;
						PointsTop[6].x = SelectRect.right;
						PointsTop[6].y = mids.y;
						PointsTop[7].x = SelectRect.right;
						PointsTop[7].y = SelectRect.bottom - deltas.y;
						PointsTop[8].x = SelectRect.right - deltas.x;
						PointsTop[8].y = SelectRect.bottom;
						PointsTop[9].x = mids.x;
						PointsTop[9].y = SelectRect.bottom;

						// lower left bezier arc
						PointsTop[10].x = BEZIER_MARKER;
						PointsTop[10].y = 4;
						PointsTop[11].x = mids.x;
						PointsTop[11].y = SelectRect.bottom;
						PointsTop[12].x = SelectRect.left + deltas.x;
						PointsTop[12].y = SelectRect.bottom;
						PointsTop[13].x = SelectRect.left;
						PointsTop[13].y = SelectRect.bottom - deltas.y;
						PointsTop[14].x = SelectRect.left;
						PointsTop[14].y = mids.y;

						// upper left bezier arc
						PointsTop[15].x = BEZIER_MARKER;
						PointsTop[15].y = 4;
						PointsTop[16].x = SelectRect.left;
						PointsTop[16].y = mids.y;
						PointsTop[17].x = SelectRect.left;
						PointsTop[17].y = SelectRect.top + deltas.y;
						PointsTop[18].x = SelectRect.left + deltas.x;
						PointsTop[18].y = SelectRect.top;
						PointsTop[19].x = mids.x;
						PointsTop[19].y = SelectRect.top;

						nPointsTop = 20; 

						if( lpShape = MaskCreateShapes( PointsTop, &nPointsTop, 1, SHAPE_ADD, YES, NULL ))
						{
							DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
							FreeUpShapes( lpShape );
						}
						else
							Message( IDS_EMEMALLOC );

						if( bWireFrame )	// draw hidden ellipse
						{
							deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
							deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
							mids.x	 = ( SelectRectNext.left + SelectRectNext.right ) / 2;
							mids.y	 = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

							// Draw cylinder top
							// top right bezier arc
							PointsBottom[0].x = BEZIER_MARKER;
							PointsBottom[0].y = 4;
							PointsBottom[1].x = mids.x;
							PointsBottom[1].y = SelectRectNext.top;
							PointsBottom[2].x = SelectRectNext.right - deltas.x;
							PointsBottom[2].y = SelectRectNext.top;
							PointsBottom[3].x = SelectRectNext.right;
							PointsBottom[3].y = SelectRectNext.top + deltas.y;
							PointsBottom[4].x = SelectRectNext.right;
							PointsBottom[4].y = mids.y;

							// lower right bezier arc
							PointsBottom[5].x = BEZIER_MARKER;
							PointsBottom[5].y = 4;
							PointsBottom[6].x = SelectRectNext.right;
							PointsBottom[6].y = mids.y;
							PointsBottom[7].x = SelectRectNext.right;
							PointsBottom[7].y = SelectRectNext.bottom - deltas.y;
							PointsBottom[8].x = SelectRectNext.right - deltas.x;
							PointsBottom[8].y = SelectRectNext.bottom;
							PointsBottom[9].x = mids.x;
							PointsBottom[9].y = SelectRectNext.bottom;

							// lower left bezier arc
							PointsBottom[10].x = BEZIER_MARKER;
							PointsBottom[10].y = 4;
							PointsBottom[11].x = mids.x;
							PointsBottom[11].y = SelectRectNext.bottom;
							PointsBottom[12].x = SelectRectNext.left + deltas.x;
							PointsBottom[12].y = SelectRectNext.bottom;
							PointsBottom[13].x = SelectRectNext.left;
							PointsBottom[13].y = SelectRectNext.bottom - deltas.y;
							PointsBottom[14].x = SelectRectNext.left;
							PointsBottom[14].y = mids.y;

							// upper left bezier arc
							PointsBottom[15].x = BEZIER_MARKER;
							PointsBottom[15].y = 4;
							PointsBottom[16].x = SelectRectNext.left;
							PointsBottom[16].y = mids.y;
							PointsBottom[17].x = SelectRectNext.left;
							PointsBottom[17].y = SelectRectNext.top + deltas.y;
							PointsBottom[18].x = SelectRectNext.left + deltas.x;
							PointsBottom[18].y = SelectRectNext.top;
							PointsBottom[19].x = mids.x;
							PointsBottom[19].y = SelectRectNext.top;

							nPointsBottom = 20; 

							if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}
						break;


					case SL_PYRAMID:
					case SL_PPYRAMID:
						apex.x = ( SelectRect.left + SelectRect.right + SelectRectNext.left + SelectRectNext.right ) / 4;
						apex.y = ( SelectRect.top + SelectRectNext.top ) / 2;

						PointsBottom[0].x = SelectRectNext.left;
						PointsBottom[0].y = SelectRectNext.bottom;
						PointsBottom[1].x = SelectRectNext.right;
						PointsBottom[1].y = SelectRectNext.bottom;
						PointsBottom[2].x = SelectRect.right;
						PointsBottom[2].y = SelectRect.bottom;
						PointsBottom[3].x = SelectRect.left;
						PointsBottom[3].y = SelectRect.bottom;
						nPointsBottom = 4;

						PointsFront[0].x = SelectRectNext.left;
						PointsFront[0].y = SelectRectNext.bottom;
						PointsFront[1].x = SelectRectNext.right;
						PointsFront[1].y = SelectRectNext.bottom;
						PointsFront[2].x = apex.x;
						PointsFront[2].y = apex.y;
						nPointsFront = 3;

						PointsLeft[0].x = SelectRect.left;
						PointsLeft[0].y = SelectRect.bottom;
						PointsLeft[1].x = SelectRectNext.left;
						PointsLeft[1].y = SelectRectNext.bottom;
						PointsLeft[2].x = apex.x;
						PointsLeft[2].y = apex.y;
						nPointsLeft = 3;

						PointsBack[0].x = SelectRect.right;
						PointsBack[0].y = SelectRect.bottom;
						PointsBack[1].x = SelectRect.left;
						PointsBack[1].y = SelectRect.bottom;
						PointsBack[2].x = apex.x;
						PointsBack[2].y = apex.y;
						nPointsBack = 3;

						PointsRight[0].x = SelectRectNext.right;
						PointsRight[0].y = SelectRectNext.bottom;
						PointsRight[1].x = SelectRect.right;
						PointsRight[1].y = SelectRect.bottom;
						PointsRight[2].x = apex.x;
						PointsRight[2].y = apex.y;
						nPointsRight = 3;

						// if centered
						if(( ptCenterRectNext.y == ptCenterRect.y ) && ( ptCenterRectNext.x == ptCenterRect.x ))
						{
							// draw front
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}

						else
						// if 1st quadrant
						if(( ptCenterRectNext.y <= ptCenterRect.y ) && ( ptCenterRectNext.x >= ptCenterRect.x ))
						{
							// if apex points down
							if(( SelectRectNext.bottom < SelectRectNext.top ) && bWireFrame )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw back side
							if( lpShape = MaskCreateShapes( PointsBack, &nPointsBack, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw right side
							if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw left side
							if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// if apex points up
							if( SelectRectNext.bottom >= SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}
						}

						// if 2nd quadrant
						else if(( ptCenterRectNext.y <= ptCenterRect.y ) && ( ptCenterRectNext.x <= ptCenterRect.x ))
						{
							// if apex points down
							if( SelectRectNext.bottom < SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw back side
							if( lpShape = MaskCreateShapes( PointsBack, &nPointsBack, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw left side 
							if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw right side 
							if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front 
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// if apex points up
							if( SelectRectNext.bottom >= SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}
						}

						// if 3rd quadrant
						else if(( ptCenterRectNext.y >= ptCenterRect.y ) && ( ptCenterRectNext.x <= ptCenterRect.x ))
						{
							// if apex points down
							if( SelectRectNext.bottom >= SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw back side
							if( lpShape = MaskCreateShapes( PointsBack, &nPointsBack, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw left side 
							if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw right side 
							if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front 
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// if apex points down
							if( SelectRectNext.bottom < SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}
						}

						// if 4th quadrant
						else if(( ptCenterRectNext.y >= ptCenterRect.y ) && ( ptCenterRectNext.x >= ptCenterRect.x ))
						{
							// if apex points up
							if( SelectRectNext.bottom >= SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw back side
							if( lpShape = MaskCreateShapes( PointsBack, &nPointsBack, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw right side
							if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw left side
							if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front 
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// if apex points down
							if( SelectRectNext.bottom < SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}
						}
						break;


					case SL_WEDGE:
					case SL_PWEDGE:
						PointsFront[0].x = SelectRectNext.left;
						PointsFront[0].y = SelectRectNext.bottom;
						PointsFront[1].x = SelectRectNext.right;
						PointsFront[1].y = SelectRectNext.bottom;
						PointsFront[2].x = ( SelectRectNext.left+SelectRectNext.right ) / 2;
						PointsFront[2].y = SelectRectNext.top;
						nPointsFront = 3;

						PointsBottom[0].x = SelectRectNext.left;
						PointsBottom[0].y = SelectRectNext.bottom;
						PointsBottom[1].x = SelectRectNext.right;
						PointsBottom[1].y = SelectRectNext.bottom;
						PointsBottom[2].x = SelectRect.right;
						PointsBottom[2].y = SelectRect.bottom;
						PointsBottom[3].x = SelectRect.left;
						PointsBottom[3].y = SelectRect.bottom;
						nPointsBottom = 4;

						PointsLeft[0].x = SelectRectNext.left;
						PointsLeft[0].y = SelectRectNext.bottom;
						PointsLeft[1].x = SelectRect.left;
						PointsLeft[1].y = SelectRect.bottom;
						PointsLeft[2].x = ( SelectRect.right + SelectRect.left ) / 2;
						PointsLeft[2].y = SelectRect.top;
						PointsLeft[3].x = ( SelectRectNext.right + SelectRectNext.left ) / 2;
						PointsLeft[3].y = SelectRectNext.top;
						nPointsLeft = 4;

						PointsRight[0].x = SelectRectNext.right;
						PointsRight[0].y = SelectRectNext.bottom;
						PointsRight[1].x = SelectRect.right;
						PointsRight[1].y = SelectRect.bottom;
						PointsRight[2].x = ( SelectRect.right + SelectRect.left ) / 2;
						PointsRight[2].y = SelectRect.top;
						PointsRight[3].x = ( SelectRectNext.right + SelectRectNext.left ) / 2;
						PointsRight[3].y = SelectRectNext.top;
						nPointsRight = 4;

						// if centered
						if( ptCenterRectNext.y == ptCenterRect.y && ptCenterRectNext.x == ptCenterRect.x )
						{
							// draw front
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}

						// if 1st quadrant
						else
						if(( ptCenterRectNext.y <= ptCenterRect.y ) && ( ptCenterRectNext.x >= ptCenterRect.x ))
						{
							if( bWireFrame )
							{
								// if apex points down
								if( SelectRectNext.bottom < SelectRectNext.top )
								{
									// draw bottom 
									if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
									{
										DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
										FreeUpShapes( lpShape );
									}
									else
										Message( IDS_EMEMALLOC );
								}
							}
						 
							// if apex points down
							if( SelectRectNext.bottom < SelectRectNext.top )
							{
								// draw right side
								if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw left side
							if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// if apex points up
							if( SelectRectNext.bottom >= SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}
						}

						// if 2nd quadrant
						else if(( ptCenterRectNext.y <= ptCenterRect.y ) && ( ptCenterRectNext.x <= ptCenterRect.x ))
						{
							if( bWireFrame )
							{
								// if apex points down
								if( SelectRectNext.bottom < SelectRectNext.top )
								{
									// draw bottom 
									if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
									{
										DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
										FreeUpShapes( lpShape );
									}
									else
										Message( IDS_EMEMALLOC );
								}
							}

							// if apex points down
							if( SelectRectNext.bottom < SelectRectNext.top )
							{
								// draw left side 
								if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw right side 
							if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front 
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// if apex points up
							if( SelectRectNext.bottom >= SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}
						}

						// if 3rd quadrant
						else if(( ptCenterRectNext.y >= ptCenterRect.y ) && ( ptCenterRectNext.x <= ptCenterRect.x ))
						{
							if( bWireFrame )
							{
								// if apex points up
								if( SelectRectNext.bottom >= SelectRectNext.top )
								{
									// draw bottom 
									if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
									{
										DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
										FreeUpShapes( lpShape );
									}
									else
										Message( IDS_EMEMALLOC );
								}
							}

							// if apex points up
							if( SelectRectNext.bottom >= SelectRectNext.top )
							{
								// draw left side 
								if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw right side 
							if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// if apex points down
							if( SelectRectNext.bottom < SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}
						}

						// if 4th quadrant
						else if(( ptCenterRectNext.y >= ptCenterRect.y ) && ( ptCenterRectNext.x >= ptCenterRect.x ))
						{
							if( bWireFrame )
							{
								// if apex points up
								if( SelectRectNext.bottom >= SelectRectNext.top )
								{
									// draw bottom 
									if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
									{
										DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
										FreeUpShapes( lpShape );
									}
									else
										Message( IDS_EMEMALLOC );
								}
							}

							// if apex points up
							if( SelectRectNext.bottom >= SelectRectNext.top )
							{
								// draw right side
								if( lpShape = MaskCreateShapes( PointsRight, &nPointsRight, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}

							// draw left side
							if( lpShape = MaskCreateShapes( PointsLeft, &nPointsLeft, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// draw front
							if( lpShape = MaskCreateShapes( PointsFront, &nPointsFront, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );

							// if apex points down
							if( SelectRectNext.bottom < SelectRectNext.top )
							{
								// draw bottom 
								if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
								{
									DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
									FreeUpShapes( lpShape );
								}
								else
									Message( IDS_EMEMALLOC );
							}
						}
						break;


					case SL_HCONE:
						deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
						deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
						mids.x	 = ( SelectRectNext.left + SelectRectNext.right ) / 2;
						mids.y	 = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

						// upper right arc
						PointsCone[0].x = BEZIER_MARKER;
						PointsCone[0].y = 4;
						PointsCone[1].x = mids.x;
						PointsCone[1].y = SelectRectNext.top;
						PointsCone[2].x = SelectRectNext.right - deltas.x;
						PointsCone[2].y = SelectRectNext.top;
						PointsCone[3].x = SelectRectNext.right;
						PointsCone[3].y = SelectRectNext.top + deltas.y;
						PointsCone[4].x = SelectRectNext.right;
						PointsCone[4].y = mids.y;

						// lower right arc
						PointsCone[5].x = BEZIER_MARKER;
						PointsCone[5].y = 4;
						PointsCone[6].x = SelectRectNext.right;
						PointsCone[6].y = mids.y;
						PointsCone[7].x = SelectRectNext.right;
						PointsCone[7].y = SelectRectNext.bottom - deltas.y;
						PointsCone[8].x = SelectRectNext.right - deltas.x;
						PointsCone[8].y = SelectRectNext.bottom;
						PointsCone[9].x = mids.x;
						PointsCone[9].y = SelectRectNext.bottom;

						// lower left arc
						PointsCone[10].x = BEZIER_MARKER;
						PointsCone[10].y = 4;
						PointsCone[11].x = mids.x;
						PointsCone[11].y = SelectRectNext.bottom;
						PointsCone[12].x = SelectRectNext.left + deltas.x;
						PointsCone[12].y = SelectRectNext.bottom;
						PointsCone[13].x = SelectRectNext.left;
						PointsCone[13].y = SelectRectNext.bottom - deltas.y;
						PointsCone[14].x = SelectRectNext.left;
						PointsCone[14].y = mids.y;

						// upper left arc
						PointsCone[15].x = BEZIER_MARKER;
						PointsCone[15].y = 4;
						PointsCone[16].x = SelectRectNext.left;
						PointsCone[16].y = mids.y;
						PointsCone[17].x = SelectRectNext.left;
						PointsCone[17].y = SelectRectNext.top + deltas.y;
						PointsCone[18].x = SelectRectNext.left + deltas.x;
						PointsCone[18].y = SelectRectNext.top;
						PointsCone[19].x = mids.x;
						PointsCone[19].y = SelectRectNext.top;

						nPointsCone = 20;

						if( lpShape = MaskCreateShapes( PointsCone, &nPointsCone, 1, SHAPE_ADD, YES, NULL ))
						{
							// get expanded point list
							lpPoints = ( LPPOINT )LineBuffer[0];

							nPoints = ShapeConvertPoints( lpShape, lpPoints );

							lpPoints = ( LPPOINT )LineBuffer[0];

							FreeUpShapes( lpShape );
						}
						else
							Message( IDS_EMEMALLOC );

						// if dragged right
						if( SelectRectNext.left >= (( SelectRect.left + SelectRect.right ) / 2 ))
						{
							for( index = ( nPoints / 2 ) - 1; index < 3 * ( nPoints / 4 ) - 1; index++ )
							{
								if( lpPoints[index].x - lpPoints[index + 1].x )
								{
									VectorSlope = ( float )( lpPoints[index + 1].y - lpPoints[index].y ) /
												  ( float )( lpPoints[index].x - lpPoints[index + 1].x );

									LineSlope = ( float )((( SelectRect.top + SelectRect.bottom ) / 2 ) - lpPoints[index].y ) / 
												( float )( lpPoints[index].x - (( SelectRect.left + SelectRect.right ) / 2 ));

									if( VectorSlope < LineSlope )		// slopes are positive
									{
										nPointsCone = 0;

										for( nPointCounter = 0; nPointCounter < index - ( nPoints / 2 ); nPointCounter++ )
										{
											PointsCone[nPointCounter].x = lpPoints[index - nPointCounter].x;
											PointsCone[nPointCounter].y = lpPoints[index - nPointCounter].y;
											nPointsCone++;
										}

										// lower right arc
										PointsCone[nPointCounter].x = BEZIER_MARKER;
										PointsCone[nPointCounter++].y = 4;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = mids.x;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right - deltas.x;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom - deltas.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right;
										PointsCone[nPointCounter++].y = mids.y;
										nPointsCone++;	   

										// upper right arc
										PointsCone[nPointCounter].x = BEZIER_MARKER;
										PointsCone[nPointCounter++].y = 4;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right;
										PointsCone[nPointCounter++].y = mids.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right;
										PointsCone[nPointCounter++].y = SelectRectNext.top + deltas.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right - deltas.x;
										PointsCone[nPointCounter++].y = SelectRectNext.top;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = mids.x;
										PointsCone[nPointCounter++].y = SelectRectNext.top;
										nPointsCone++;	   

										for( nPointCounter = nPointsCone, offset = 0; offset < index - ( nPoints / 2 ); nPointCounter++, offset++ )
										{
											PointsCone[nPointCounter].x = lpPoints[nPoints - 1 - offset].x;
											PointsCone[nPointCounter].y = lpPoints[nPoints - 1 - offset].y;
											nPointsCone++;
										}

										PointsCone[nPointCounter].x = ( SelectRect.left + SelectRect.right ) / 2;
										PointsCone[nPointCounter].y = ( SelectRect.top + SelectRect.bottom ) / 2;
										nPointsCone++; 

										break;
									}
								}
							}
						}
						else	// if dragged left
						if( SelectRectNext.right <= (( SelectRect.left + SelectRect.right ) / 2 ))
						{
							for( index = 0; index < nPoints / 4 - 1; index++ )
							{
								if( lpPoints[index].x - lpPoints[index+1].x )
								{
									VectorSlope = ( float )( lpPoints[index+1].y - lpPoints[index].y ) /
												  ( float )( lpPoints[index].x - lpPoints[index+1].x );

									LineSlope = ( float )( lpPoints[index].y - (( SelectRect.top + SelectRect.bottom ) / 2 )) / 
												( float )((( SelectRect.left + SelectRect.right ) / 2 ) - lpPoints[index].x );

									if( VectorSlope < LineSlope )		// slopes are positve  
									{
										nPointsCone = 0;

										for( nPointCounter = 0; nPointCounter < index; nPointCounter++ )
										{
											PointsCone[nPointCounter].x = lpPoints[nPointCounter + ( nPoints / 2 - 1 ) - index].x;
											PointsCone[nPointCounter].y = lpPoints[nPointCounter + ( nPoints / 2 - 1 ) - index].y;
											nPointsCone++;
										}

										// lower left bezier arc
										PointsCone[nPointCounter].x = BEZIER_MARKER;
										PointsCone[nPointCounter++].y = 4;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = mids.x;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left + deltas.x;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom - deltas.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left;
										PointsCone[nPointCounter++].y = mids.y;
										nPointsCone++;	   

										// upper left bezier arc
										PointsCone[nPointCounter].x = BEZIER_MARKER;
										PointsCone[nPointCounter++].y = 4;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left;
										PointsCone[nPointCounter++].y = mids.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left;
										PointsCone[nPointCounter++].y = SelectRectNext.top + deltas.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left + deltas.x;
										PointsCone[nPointCounter++].y = SelectRectNext.top;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = mids.x;
										PointsCone[nPointCounter++].y = SelectRectNext.top;
										nPointsCone++;	   

										for( nPointCounter = nPointsCone, offset = 0; offset <= index; nPointCounter++, offset++ )
										{
											PointsCone[nPointCounter].x = lpPoints[offset].x;
											PointsCone[nPointCounter].y = lpPoints[offset].y;
											nPointsCone++;
										}

										PointsCone[nPointCounter].x = ( SelectRect.left + SelectRect.right ) / 2;
										PointsCone[nPointCounter].y = ( SelectRect.top + SelectRect.bottom ) / 2;
										nPointsCone++; 

										break;
									}
								}
							}
						}

						// draw cylinder front
						if( lpShape = MaskCreateShapes( PointsCone, &nPointsCone, 1, SHAPE_ADD, YES, NULL ))
						{
							DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
							FreeUpShapes( lpShape );
						}
						else
							Message( IDS_EMEMALLOC );


						if( bWireFrame )	// if wireframe
						{
							// top right bezier
							PointsBottom[0].x = BEZIER_MARKER;
							PointsBottom[0].y = 4;
							PointsBottom[1].x = mids.x;
							PointsBottom[1].y = SelectRectNext.top;
							PointsBottom[2].x = SelectRectNext.right - deltas.x;
							PointsBottom[2].y = SelectRectNext.top;
							PointsBottom[3].x = SelectRectNext.right;
							PointsBottom[3].y = SelectRectNext.top + deltas.y;
							PointsBottom[4].x = SelectRectNext.right;
							PointsBottom[4].y = mids.y;

							// lower right bezier
							PointsBottom[5].x = BEZIER_MARKER;
							PointsBottom[5].y = 4;
							PointsBottom[6].x = SelectRectNext.right;
							PointsBottom[6].y = mids.y;
							PointsBottom[7].x = SelectRectNext.right;
							PointsBottom[7].y = SelectRectNext.bottom - deltas.y;
							PointsBottom[8].x = SelectRectNext.right - deltas.x;
							PointsBottom[8].y = SelectRectNext.bottom;
							PointsBottom[9].x = mids.x;
							PointsBottom[9].y = SelectRectNext.bottom;

							// lower left bezier
							PointsBottom[10].x = BEZIER_MARKER;
							PointsBottom[10].y = 4;
							PointsBottom[11].x = mids.x;
							PointsBottom[11].y = SelectRectNext.bottom;
							PointsBottom[12].x = SelectRectNext.left + deltas.x;
							PointsBottom[12].y = SelectRectNext.bottom;
							PointsBottom[13].x = SelectRectNext.left;
							PointsBottom[13].y = SelectRectNext.bottom - deltas.y;
							PointsBottom[14].x = SelectRectNext.left;
							PointsBottom[14].y = mids.y;

							// upper left bezier
							PointsBottom[15].x = BEZIER_MARKER;
							PointsBottom[15].y = 4;
							PointsBottom[16].x = SelectRectNext.left;
							PointsBottom[16].y = mids.y;
							PointsBottom[17].x = SelectRectNext.left;
							PointsBottom[17].y = SelectRectNext.top + deltas.y;
							PointsBottom[18].x = SelectRectNext.left + deltas.x;
							PointsBottom[18].y = SelectRectNext.top;
							PointsBottom[19].x = mids.x;
							PointsBottom[19].y = SelectRectNext.top;

							nPointsBottom = 20; 

							if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}
						break;


					case SL_VCONE:
						deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
						deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
						mids.x	 = ( SelectRectNext.left + SelectRectNext.right ) / 2;
						mids.y	 = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

						// upper right arc
						PointsCone[0].x = BEZIER_MARKER;
						PointsCone[0].y = 4;
						PointsCone[1].x = mids.x;
						PointsCone[1].y = SelectRectNext.top;
						PointsCone[2].x = SelectRectNext.right - deltas.x;
						PointsCone[2].y = SelectRectNext.top;
						PointsCone[3].x = SelectRectNext.right;
						PointsCone[3].y = SelectRectNext.top + deltas.y;
						PointsCone[4].x = SelectRectNext.right;
						PointsCone[4].y = mids.y;

						// lower right arc
						PointsCone[5].x = BEZIER_MARKER;
						PointsCone[5].y = 4;
						PointsCone[6].x = SelectRectNext.right;
						PointsCone[6].y = mids.y;
						PointsCone[7].x = SelectRectNext.right;
						PointsCone[7].y = SelectRectNext.bottom - deltas.y;
						PointsCone[8].x = SelectRectNext.right - deltas.x;
						PointsCone[8].y = SelectRectNext.bottom;
						PointsCone[9].x = mids.x;
						PointsCone[9].y = SelectRectNext.bottom;

						// lower left arc
						PointsCone[10].x = BEZIER_MARKER;
						PointsCone[10].y = 4;
						PointsCone[11].x = mids.x;
						PointsCone[11].y = SelectRectNext.bottom;
						PointsCone[12].x = SelectRectNext.left + deltas.x;
						PointsCone[12].y = SelectRectNext.bottom;
						PointsCone[13].x = SelectRectNext.left;
						PointsCone[13].y = SelectRectNext.bottom - deltas.y;
						PointsCone[14].x = SelectRectNext.left;
						PointsCone[14].y = mids.y;

						// upper left arc
						PointsCone[15].x = BEZIER_MARKER;
						PointsCone[15].y = 4;
						PointsCone[16].x = SelectRectNext.left;
						PointsCone[16].y = mids.y;
						PointsCone[17].x = SelectRectNext.left;
						PointsCone[17].y = SelectRectNext.top + deltas.y;
						PointsCone[18].x = SelectRectNext.left + deltas.x;
						PointsCone[18].y = SelectRectNext.top;
						PointsCone[19].x = mids.x;
						PointsCone[19].y = SelectRectNext.top;

						nPointsCone = 20;

						if( lpShape = MaskCreateShapes( PointsCone, &nPointsCone, 1, SHAPE_ADD, YES, NULL ))
						{
							// get expanded point list
							lpPoints = ( LPPOINT )LineBuffer[0];

							nPoints = ShapeConvertPoints( lpShape, lpPoints );

							lpPoints = ( LPPOINT )LineBuffer[0];

							FreeUpShapes( lpShape );
						}
						else
							Message( IDS_EMEMALLOC );

						// if dragged down
						if( SelectRectNext.top >= (( SelectRect.top + SelectRect.bottom ) / 2 ))
						{
							for( index = nPoints / 4 - 1 ; index > 0; index-- )
							{
								if( lpPoints[index].x - lpPoints[index - 1].x )
								{
									VectorSlope = ( float )( lpPoints[index - 1].y - lpPoints[index].y ) /
												  ( float )( lpPoints[index].x - lpPoints[index - 1].x );

									LineSlope = ( float )((( SelectRect.top + SelectRect.bottom ) / 2 ) - lpPoints[index].y ) / 
												( float )( lpPoints[index].x - (( SelectRect.left + SelectRect.right ) / 2 ));

									if( VectorSlope > LineSlope )
									{
										nPointsCone = 0;

										for( nPointCounter = 0; nPointCounter < ( nPoints / 4 - 1 ) - index; nPointCounter++ )
										{
											PointsCone[nPointCounter].x = lpPoints[nPointCounter + index].x;
											PointsCone[nPointCounter].y = lpPoints[nPointCounter + index].y;
											nPointsCone++;
										}

										// lower right bezier arc
										PointsCone[nPointCounter].x = BEZIER_MARKER;
										PointsCone[nPointCounter++].y = 4;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right;
										PointsCone[nPointCounter++].y = mids.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom - deltas.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right - deltas.x;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = mids.x;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom;
										nPointsCone++;	   

										// lower left bezier arc
										PointsCone[nPointCounter].x = BEZIER_MARKER;
										PointsCone[nPointCounter++].y = 4;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = mids.x;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left + deltas.x;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left;
										PointsCone[nPointCounter++].y = SelectRectNext.bottom - deltas.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left;
										PointsCone[nPointCounter++].y = mids.y;
										nPointsCone++;	   

										for( nPointCounter = nPointsCone, offset = 3 * nPoints / 4 - 1; offset < ( nPoints - 1 ) - index; nPointCounter++, offset++ )
										{
											PointsCone[nPointCounter].x = lpPoints[offset].x;
											PointsCone[nPointCounter].y = lpPoints[offset].y;
											nPointsCone++;
										}

										PointsCone[nPointCounter].x = ( SelectRect.left + SelectRect.right ) / 2;
										PointsCone[nPointCounter++].y = ( SelectRect.top + SelectRect.bottom ) / 2;
										nPointsCone++; 

										break;
									}
								}
							}
						}
						else	// if dragged up
						if( SelectRectNext.bottom <= (( SelectRect.top + SelectRect.bottom ) / 2 ))
						{
							for( index = nPoints / 4 - 1 ; index < ( nPoints / 2 - 1 ); index++ )
							{
								if( lpPoints[index].x - lpPoints[index + 1].x )
								{
									VectorSlope = ( float )( lpPoints[index + 1].y - lpPoints[index].y ) /
												  ( float )( lpPoints[index].x - lpPoints[index + 1].x );

									LineSlope = ( float )((( SelectRect.top + SelectRect.bottom ) / 2 ) - lpPoints[index].y ) / 
												( float )( lpPoints[index].x - (( SelectRect.left + SelectRect.right ) / 2 ));

									if( VectorSlope < LineSlope )	// slope is positive
									{
										nPointsCone = 0;

										for( nPointCounter = 0; nPointCounter < index - ( nPoints / 4 - 1 ); nPointCounter++ )
										{
											PointsCone[nPointCounter].x = lpPoints[index - nPointCounter].x;
											PointsCone[nPointCounter].y = lpPoints[index - nPointCounter].y;
											nPointsCone++;
										}

										// top right bezier
										PointsCone[nPointCounter].x = BEZIER_MARKER;
										PointsCone[nPointCounter++].y = 4;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right;
										PointsCone[nPointCounter++].y = mids.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right;
										PointsCone[nPointCounter++].y = SelectRectNext.top + deltas.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.right - deltas.x;
										PointsCone[nPointCounter++].y = SelectRectNext.top;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = mids.x;
										PointsCone[nPointCounter++].y = SelectRectNext.top;
										nPointsCone++;	   

										// upper left bezier
										PointsCone[nPointCounter].x = BEZIER_MARKER;
										PointsCone[nPointCounter++].y = 4;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = mids.x;
										PointsCone[nPointCounter++].y = SelectRectNext.top;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left + deltas.x;
										PointsCone[nPointCounter++].y = SelectRectNext.top;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left;
										PointsCone[nPointCounter++].y = SelectRectNext.top + deltas.y;
										nPointsCone++;	   
										PointsCone[nPointCounter].x = SelectRectNext.left;
										PointsCone[nPointCounter++].y = mids.y;
										nPointsCone++;	   

										for( nPointCounter = nPointsCone, offset = 3 * nPoints / 4 - 1; offset > ( nPoints - 1 ) - index; nPointCounter++, offset-- )
										{
											PointsCone[nPointCounter].x = lpPoints[offset].x;
											PointsCone[nPointCounter].y = lpPoints[offset].y;
											nPointsCone++;
										}

										PointsCone[nPointCounter].x = ( SelectRect.left + SelectRect.right ) / 2;
										PointsCone[nPointCounter++].y = ( SelectRect.top + SelectRect.bottom ) / 2;
										nPointsCone++; 

										break;
									}
								}
							}
						}

						// draw cone front
						if( lpShape = MaskCreateShapes( PointsCone, &nPointsCone, 1, SHAPE_ADD, YES, NULL ))
						{
							DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
							FreeUpShapes( lpShape );
						}
						else
							Message( IDS_EMEMALLOC );

						// draw hidden ellipse if wireframe turned on or cone dragged up
						if( bWireFrame || ( SelectRectNext.bottom <= (( SelectRect.top + SelectRect.bottom ) / 2 )))
						{
							// top right bezier
							PointsBottom[0].x = BEZIER_MARKER;
							PointsBottom[0].y = 4;
							PointsBottom[1].x = mids.x;
							PointsBottom[1].y = SelectRectNext.top;
							PointsBottom[2].x = SelectRectNext.right - deltas.x;
							PointsBottom[2].y = SelectRectNext.top;
							PointsBottom[3].x = SelectRectNext.right;
							PointsBottom[3].y = SelectRectNext.top + deltas.y;
							PointsBottom[4].x = SelectRectNext.right;
							PointsBottom[4].y = mids.y;

							// lower right bezier
							PointsBottom[5].x = BEZIER_MARKER;
							PointsBottom[5].y = 4;
							PointsBottom[6].x = SelectRectNext.right;
							PointsBottom[6].y = mids.y;
							PointsBottom[7].x = SelectRectNext.right;
							PointsBottom[7].y = SelectRectNext.bottom - deltas.y;
							PointsBottom[8].x = SelectRectNext.right - deltas.x;
							PointsBottom[8].y = SelectRectNext.bottom;
							PointsBottom[9].x = mids.x;
							PointsBottom[9].y = SelectRectNext.bottom;

							// lower left bezier
							PointsBottom[10].x = BEZIER_MARKER;
							PointsBottom[10].y = 4;
							PointsBottom[11].x = mids.x;
							PointsBottom[11].y = SelectRectNext.bottom;
							PointsBottom[12].x = SelectRectNext.left + deltas.x;
							PointsBottom[12].y = SelectRectNext.bottom;
							PointsBottom[13].x = SelectRectNext.left;
							PointsBottom[13].y = SelectRectNext.bottom - deltas.y;
							PointsBottom[14].x = SelectRectNext.left;
							PointsBottom[14].y = mids.y;

							// upper left bezier
							PointsBottom[15].x = BEZIER_MARKER;
							PointsBottom[15].y = 4;
							PointsBottom[16].x = SelectRectNext.left;
							PointsBottom[16].y = mids.y;
							PointsBottom[17].x = SelectRectNext.left;
							PointsBottom[17].y = SelectRectNext.top + deltas.y;
							PointsBottom[18].x = SelectRectNext.left + deltas.x;
							PointsBottom[18].y = SelectRectNext.top;
							PointsBottom[19].x = mids.x;
							PointsBottom[19].y = SelectRectNext.top;

							nPointsBottom = 20; 

							if( lpShape = MaskCreateShapes( PointsBottom, &nPointsBottom, 1, SHAPE_ADD, YES, NULL ))
							{
								DrawExtrudedShapeClosed( lpShape, IDS_UNDOCHANGE, Style );
								FreeUpShapes( lpShape );
							}
							else
								Message( IDS_EMEMALLOC );
						}
						break;
				}

				if ( lpObject = ImgGetSelObject(lpImage, NULL) )
					{
					AstralUnionRect( &rUndo, &SelectRect, &SelectRectNext );
					ImgEditedObject( lpImage, lpObject,
						IDS_UNDOCHANGE, &rUndo );//&lpObject->rObject );
					}

				nButtonClicks = 1;			// reset after second click
			}
			break;


		case WM_MOUSEMOVE:	// sent when ToolActive is on
			fShift = SHIFT;

			if( nButtonClicks == 1 )
			{	
				// set shape
				UpdateSelection( hWindow, NULL, &SelectRect, Style, 
					MAKEPOINT( lParam ), CONSTRAINASPECT, 1L, 1L, 
					Window.fRButtonDown, FROMCENTER );
				}
			else
			{	// extrude shape
				UpdateExtrudedSelection( hWindow, NULL, &SelectRect, &SelectRectNext, Style, 
					MAKEPOINT( lParam ), CONSTRAINASPECT, 1L, 1L, 
					1L, FROMCENTER );
			}
			break;


		case WM_LBUTTONDBLCLK:
			break;


		case WM_DESTROY:					// CANCEL operation message <ESC>
			if( !Tool.bActive )
				break;

			Tool.bActive = NO;

			if( nButtonClicks == 1 )
				EndSelection( hWindow, NULL, &SelectRect, Style, YES );
			else
			if( nButtonClicks == 2 )		// if extruded shape created
				EndExtrudedSelection( hWindow, NULL, &SelectRect, &SelectRectNext, Style, YES );

			nButtonClicks = 1;

			break;
	}
	return( TRUE );
}


//////////////////////////////////////////////////////////////////////////////
static BOOL SetupDrawShapeColorLut( int depth )
//////////////////////////////////////////////////////////////////////////////
{
	LPTR lpLut;
	int i;
	COLORINFO ActCol, AltCol;
	BYTE active[4];
	BYTE alternate[4];
	int val;
	
	if( depth == 0 )
		depth = 1;

	lpLut = Alloc( 256*depth );
	if( !lpLut )
		return( FALSE );
	lpColorLuts[0] = lpLut;
	for( i=1;i<depth;i++ ) 
		lpColorLuts[i] = &lpColorLuts[i-1][256];
	GetAlternateColorFromType( FrameType( ImgGetBaseEditFrame( lpImage )), &AltCol );
	GetActiveColorFromType( FrameType( ImgGetBaseEditFrame( lpImage )), &ActCol );
	if( depth == 4 )
	{
		active[0] = ActCol.cmyk.c; alternate[0] = AltCol.cmyk.c;
		active[1] = ActCol.cmyk.m; alternate[1] = AltCol.cmyk.m;
		active[2] = ActCol.cmyk.y; alternate[2] = AltCol.cmyk.y;
		active[3] = ActCol.cmyk.k; alternate[3] = AltCol.cmyk.k;
	}
	else if( depth == 3 )
	{
		active[0] = ActCol.rgb.red; alternate[0] = AltCol.rgb.red;
		active[1] = ActCol.rgb.green; alternate[1] = AltCol.rgb.green;
		active[2] = ActCol.rgb.blue; alternate[2] = AltCol.rgb.blue;
	}
	else if( depth == 1 )
	{
		active[0] = ActCol.gray; alternate[0] = AltCol.gray;
	}
	else
		return( FALSE );

	// Force the alternate color ( border ) to be black
	alternate[0] = alternate[1] = alternate[2] = alternate[3] = 0;

	while( depth-- )
		for ( i=0; i<=255; i++ )
		{
			val = (( long )alternate[depth]*i+( long )active[depth]*( 255-i ))/255;
			lpColorLuts[depth][i] = bound( val, 0, 255 );
		}
	return( TRUE );
}

//////////////////////////////////////////////////////////////////////////////
static void FreeDrawShapeColorLut( void )
//////////////////////////////////////////////////////////////////////////////
{
	FreeUp( lpColorLuts[0] );
}


//////////////////////////////////////////////////////////////////////////////
void DrawShapeClosed(
//	Draws all shapes in the shape list.
//	Uses a buffer.
//////////////////////////////////////////////////////////////////////////////
LPSHAPE 	lpShape,
int 		dirty)
{
	BOOL fInterior, fExterior;
	int BrushFull, BrushHalf;
	RECT rShape, rArea;
	LPMASK lpMask;
	LPSHAPE lpCurShape;
	ENGINE Engine;
	LPFRAME lpBaseFrame;
	int brushOpacity, brushStyle, brushPressure, brushScale, brushMode;

	fInterior = !bWireFrame;
	BrushFull = GetMgxBrushSize( Retouch.hBrush );
	BrushHalf = BrushFull / 2;
	fExterior = ( BrushFull > 0 );

	if( !fInterior && !fExterior )
		return;

	lpMask = lpDrawMask = NULL;
	lpExteriorFrame = NULL;
	
	rShape = lpShape->rShape;
	lpCurShape = lpShape->lpNext;

	while ( lpCurShape )
	{
		AstralUnionRect( &rShape, &rShape, &lpCurShape->rShape );
		lpCurShape = lpCurShape->lpNext;
	}
	
	// expand rShape to allow for brush
	rArea = rShape;
	rArea.top  -= BrushHalf; // brush engine adds this to UndoRect
	rArea.left -= BrushHalf;
	rArea.bottom += ( BrushFull-BrushHalf );
	rArea.right	 += ( BrushFull-BrushHalf );

	// setup interior
	if( fInterior )
	{
		// create bitmap from shape
		lpMask = MaskCreate( NULL, RectWidth( &rArea ), RectHeight( &rArea ), NO, YES );

		if( !lpMask )
			goto ExitError;

		// move shapes to ( BrushHalf, BrushHalf )
		lpCurShape = lpShape;

		while ( lpCurShape )
		{
			TMove( &lpCurShape->tform, -rArea.left, -rArea.top );
			OffsetRect( &lpCurShape->rShape, -rArea.left, -rArea.top );
			lpCurShape = lpCurShape->lpNext;
		}
		MaskAddShapes( lpMask, lpShape );

		// move shapes back to original origin
		lpCurShape = lpShape;

		while ( lpCurShape )
		{
			OffsetRect( &lpCurShape->rShape, rArea.left, rArea.top );
			TMove( &lpCurShape->tform, rArea.left, rArea.top );
			lpCurShape = lpCurShape->lpNext;
		}
		DrawMaskOffset.x = rArea.left;
		DrawMaskOffset.y = rArea.top;
		lpDrawMask = lpMask;
	}
	
	ProgressBegin( fExterior ? 2:1, dirty - IDS_UNDOFIRST + IDS_PROGFIRST );
	
	brushOpacity = GetMgxBrushOpacity( Retouch.hBrush );
	brushScale = GetMgxBrushOpacityScale( Retouch.hBrush );
	brushMode = GetMgxBrushMergeMode( Retouch.hBrush, NULL );
	brushStyle = GetMgxBrushStyle( Retouch.hBrush );
	brushPressure = GetMgxStylePressure( Retouch.hBrush );

	if( fExterior )
	{
		// setup exterior
		brushStyle = SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
		brushPressure = SetMgxStylePressure( Retouch.hBrush, 255 );
		brushOpacity = SetMgxBrushOpacity( Retouch.hBrush, 255 );
		brushScale = SetMgxBrushOpacityScale( Retouch.hBrush, 100 );
		lpExteriorFrame = OSBrushShape( lpShape, Retouch.hBrush, &ExteriorRect, YES );

		SetMgxBrushStyle( Retouch.hBrush, brushStyle );
		SetMgxStylePressure( Retouch.hBrush, brushPressure );
		SetMgxBrushOpacity( Retouch.hBrush, brushOpacity );
		SetMgxBrushOpacityScale( Retouch.hBrush, brushScale );

		if( !lpExteriorFrame )
			goto ExitError;
	}
	else 
		ExteriorRect = rArea;

	lpBaseFrame = ImgGetBaseEditFrame( lpImage );
	
	if( !SetupDrawShapeColorLut( FrameDepth( lpBaseFrame )) )
		goto ExitError;
		
	brushOpacity = (( brushOpacity * brushScale ) + 50 ) / 100;

	SetEngineDraw( &Engine, OSDrawShapeDataProc, brushOpacity, (MERGE_MODE)brushMode );

	Engine.lpEditRect = &ExteriorRect;
	Engine.lpMaskProc = OSDrawShapeMaskProc;

	if( brushStyle == IDC_BRUSHSCATTER )
		Engine.ScatterPressure = brushPressure;

	LineEngineSelObj( lpImage, &Engine, dirty );

	FreeDrawShapeColorLut();

	if( lpMask )
		MaskClose( lpMask );
	if( lpExteriorFrame )

		FrameClose( lpExteriorFrame );

	ProgressEnd();
	UpdateImage( &rArea, YES );
	
	return;
	
ExitError:
	Message( IDS_EMEMALLOC );
	if( lpMask )
		MaskClose( lpMask );
	if( lpExteriorFrame )
		FrameClose( lpExteriorFrame );
	ProgressEnd();
}


///////////////////////////////////////////////////////////////////////////
void DrawExtrudedShapeClosed(
///////////////////////////////////////////////////////////////////////////
LPSHAPE 	lpShape,
int 		dirty,
int 		style)
{
	BOOL	fInterior, fExterior;
	int		BrushFull, BrushHalf;
	RECT	rShape, rArea;
	LPMASK	lpMask;
	LPSHAPE lpCurShape;
	ENGINE	Engine;
	LPFRAME lpBaseFrame;
	int		brushOpacity, brushStyle, brushPressure, brushScale, brushMode;

	fInterior = !bWireFrame; // don't fill
	BrushFull = GetMgxBrushSize( Retouch.hBrush );
	BrushHalf = BrushFull / 2;
	fExterior = ( BrushFull > 0 );

	if( !fInterior && !fExterior )
		return;

	lpMask = lpDrawMask = NULL;
	lpExteriorFrame = NULL;
	
	rShape = lpShape->rShape;
	lpCurShape = lpShape->lpNext;

	while( lpCurShape )
	{
		AstralUnionRect( &rShape, &rShape, &lpCurShape->rShape );
		lpCurShape = lpCurShape->lpNext;
	}
	
	// expand rShape to allow for brush
	rArea = rShape;
	rArea.top  -= BrushHalf; // brush engine adds this to UndoRect
	rArea.left -= BrushHalf;
	rArea.bottom += ( BrushFull - BrushHalf );
	rArea.right	 += ( BrushFull - BrushHalf );

	// setup interior
	if( fInterior )
	{
		// create bitmap from shape
		lpMask = MaskCreate( NULL, RectWidth( &rArea ), RectHeight( &rArea ), NO, YES );

		if( !lpMask )
			goto ExitError;

		// move shapes to ( BrushHalf, BrushHalf )
		lpCurShape = lpShape;

		while( lpCurShape )
		{
			TMove( &lpCurShape->tform, -rArea.left, -rArea.top );
			OffsetRect( &lpCurShape->rShape, -rArea.left, -rArea.top );
			lpCurShape = lpCurShape->lpNext;
		}

		MaskAddShapes( lpMask, lpShape );

		// move shapes back to original origin
		lpCurShape = lpShape;

		while( lpCurShape )
		{
			OffsetRect( &lpCurShape->rShape, rArea.left, rArea.top );
			TMove( &lpCurShape->tform, rArea.left, rArea.top );
			lpCurShape = lpCurShape->lpNext;
		}

		DrawMaskOffset.x = rArea.left;
		DrawMaskOffset.y = rArea.top;
		lpDrawMask = lpMask;
	}
	
	ProgressBegin( fExterior ? 2:1, dirty - IDS_UNDOFIRST + IDS_PROGFIRST );
	
	brushOpacity  = GetMgxBrushOpacity( Retouch.hBrush );
	brushScale	  = GetMgxBrushOpacityScale( Retouch.hBrush );
	brushMode	  = GetMgxBrushMergeMode( Retouch.hBrush, NULL );
	brushStyle	  = GetMgxBrushStyle( Retouch.hBrush );
	brushPressure = GetMgxStylePressure( Retouch.hBrush );

	if( fExterior )
	{
		// setup exterior
		brushStyle = SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
		brushPressure = SetMgxStylePressure( Retouch.hBrush, 255 );
		brushOpacity = SetMgxBrushOpacity( Retouch.hBrush, 255 );
		brushScale = SetMgxBrushOpacityScale( Retouch.hBrush, 100 );
		lpExteriorFrame = OSBrushShape( lpShape, Retouch.hBrush, &ExteriorRect, YES );

		SetMgxBrushStyle( Retouch.hBrush, brushStyle );
		SetMgxStylePressure( Retouch.hBrush, brushPressure );
		SetMgxBrushOpacity( Retouch.hBrush, brushOpacity );
		SetMgxBrushOpacityScale( Retouch.hBrush, brushScale );

		if( !lpExteriorFrame )
			goto ExitError;
	}
	else 
		ExteriorRect = rArea;

	lpBaseFrame = ImgGetBaseEditFrame( lpImage );
	
	if( !SetupDrawShapeColorLut( FrameDepth( lpBaseFrame )))
		goto ExitError;
		
	brushOpacity = (( brushOpacity * brushScale ) + 50 ) / 100;

	SetEngineDraw( &Engine, OSDrawShapeDataProc, brushOpacity, (MERGE_MODE)brushMode );

	Engine.lpEditRect = &ExteriorRect;
	Engine.lpMaskProc = OSDrawShapeMaskProc;
	Engine.fNoUndo = TRUE;

	if( brushStyle == IDC_BRUSHSCATTER )
		Engine.ScatterPressure = brushPressure;

	LineEngineSelObj( lpImage, &Engine, dirty );

	FreeDrawShapeColorLut();

	if( lpMask )
		MaskClose( lpMask );

	if( lpExteriorFrame )
		FrameClose( lpExteriorFrame );

	ProgressEnd();

	UpdateImage( &rArea, YES );
	RandomizeActiveColor();
	return;
	
ExitError:
	Message( IDS_EMEMALLOC );

	if( lpMask )
		MaskClose( lpMask );

	if( lpExteriorFrame )
		FrameClose( lpExteriorFrame );

	ProgressEnd();
}


//////////////////////////////////////////////////////////////////////////////
void DrawShapeOpen(
//////////////////////////////////////////////////////////////////////////////
LPSHAPE 	lpShape,
int 		dirty)
{
BOOL fExterior;
int BrushFull, BrushHalf;
RECT rShape;
LPSHAPE lpCurShape;

BrushFull = GetMgxBrushSize( Retouch.hBrush );
BrushHalf = BrushFull / 2;
fExterior = ( BrushFull > 0 ) && Draw.DrawExterior;

rShape = lpShape->rShape;
lpCurShape = lpShape->lpNext;
while ( lpCurShape )
	{
	AstralUnionRect( &rShape, &rShape, &lpCurShape->rShape );
	lpCurShape = lpCurShape->lpNext;
	}

// Draw exterior
BrushShapeObj( lpImage, lpShape, Retouch.hBrush, YES, NO, dirty );

rShape.top -= BrushHalf; // brush engine adds this to UndoRect
rShape.left -= BrushHalf;
rShape.bottom += ( BrushFull-BrushHalf );
rShape.right += ( BrushFull-BrushHalf );
UpdateImage( &rShape, YES );
}


//////////////////////////////////////////////////////////////////////////////
void BrushShapeObj( LPIMAGE lpImage, LPSHAPE lpShape, HMGXBRUSH hBrush, 
					BOOL bCacheInit, BOOL bClosed, int dirty )
//	brushes all shape in shape list
//////////////////////////////////////////////////////////////////////////////
{
int BrushFull, BrushHalf;
LPPOINT lpPoints, lpBase;
int nPoints, i;
LPOBJECT lpObject = NULL;
RECT rDraw, rTemp;
LPSHAPE lpCurShape;

if( !lpImage )
	return;
if( bCacheInit )
	if( !ImgEditInit( lpImage, ET_SELOBJECTS, UT_DATA, NULL ))
		return;
ProgressBegin( 1, 0 );
// get drawing area
BrushFull = GetMgxBrushSize( hBrush );
BrushHalf = BrushFull / 2;
rDraw = lpShape->rShape;
lpCurShape = lpShape->lpNext;
while ( lpCurShape )
	{
	AstralUnionRect( &rDraw, &rDraw, &lpCurShape->rShape );
	lpCurShape = lpCurShape->lpNext;
	}
rDraw.top -= BrushHalf;
rDraw.left -= BrushHalf;
rDraw.bottom += ( BrushFull-BrushHalf );
rDraw.right += ( BrushFull-BrushHalf );

while ( lpObject = ImgGetSelObject( lpImage, lpObject ))
	{
	if( !AstralIntersectRect( &rTemp, &rDraw, &lpObject->rObject ))
		continue;
	lpCurShape = lpShape;
	while ( lpCurShape )
		{
		// get expanded point list
		lpPoints = lpBase = ( LPPOINT )LineBuffer[0];
		nPoints = ShapeConvertPoints( lpCurShape, lpPoints );
		if( !MgxBrushBegin( lpImage->hWnd, &lpObject->Pixmap, 
			lpObject->rObject.left, lpObject->rObject.top, hBrush ))
			continue;
		lpPoints = ( LPPOINT )LineBuffer[0];
		for ( i = 0; i < nPoints; i++ )
			{
			AstralClockCursor( i, nPoints, NO );
			MgxBrushStroke( 0, 0, 
				lpPoints[i].x, lpPoints[i].y, 0, 0 );
			}
		if( bClosed )
			MgxBrushStroke( 0, 0, 
				lpPoints[0].x, lpPoints[0].y, 0, 0 );
		MgxBrushEnd( NO );
		lpCurShape = lpCurShape->lpNext;
		}
	if( bCacheInit )
		ImgEditedObject( lpImage, lpObject, dirty, &rTemp );
	}
ProgressEnd();
}

//////////////////////////////////////////////////////////////////////////////
void DrawEllipse( LPRECT lpRect, int dirty )
// Renders an ellipse -- mirrors DrawShape
//////////////////////////////////////////////////////////////////////////////
{
	BOOL fInterior, fExterior;
	int BrushFull, BrushHalf;
	LPMASK lpMask;
	ENGINE Engine;
	LPFRAME lpBaseFrame;
	RECT rEllipse;
	RECT rArea;
	int brushOpacity, brushStyle, brushPressure, brushScale, brushMode;

	fInterior = !bWireFrame;
	BrushFull = GetMgxBrushSize( Retouch.hBrush );
	BrushHalf = BrushFull / 2;
	fExterior = ( BrushFull > 0 );

	if( !fInterior && !fExterior )
		return;

	lpMask = lpDrawMask = NULL;
	lpExteriorFrame = NULL;
	
	rEllipse = *lpRect;
	
	if( fInterior )
	{
		// expand rShape to allow for brush
		rArea = rEllipse;
		rArea.top -= BrushHalf; // brush engine adds this to UndoRect
		rArea.left -= BrushHalf;
		rArea.bottom += ( BrushFull-BrushHalf );
		rArea.right += ( BrushFull-BrushHalf );
		
		// Draw interior
		// create bitmap from shape
		lpMask = MaskCreate( NULL, RectWidth( &rArea ), RectHeight( &rArea ), NO, YES );
		if( !lpMask )
			goto ExitError;
		
		// setup interior 
		// move rect to origin
		OffsetRect( &rEllipse, -rArea.left, -rArea.top );
		MaskAddEllipse( lpMask, &rEllipse, SHAPE_ADD );

		// move shapes back to original origin
		DrawMaskOffset.x = rArea.left;
		DrawMaskOffset.y = rArea.top;
		lpDrawMask = lpMask;
	}
	
	rEllipse = *lpRect;

	ProgressBegin( fExterior ? 2:1, dirty-IDS_UNDOFIRST + IDS_PROGFIRST );
	
	brushOpacity = GetMgxBrushOpacity( Retouch.hBrush );
	brushScale = GetMgxBrushOpacityScale( Retouch.hBrush );
	brushMode = GetMgxBrushMergeMode( Retouch.hBrush, NULL );
	brushStyle = GetMgxBrushStyle( Retouch.hBrush );
	brushPressure = GetMgxStylePressure( Retouch.hBrush );

	// setup exterior 
	if( fExterior )
	{
		brushStyle = SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
		brushPressure = SetMgxStylePressure( Retouch.hBrush, 255 );
		brushOpacity = SetMgxBrushOpacity( Retouch.hBrush, 255 );
		brushScale = SetMgxBrushOpacityScale( Retouch.hBrush, 100 );
		// stroke the interior
		lpExteriorFrame = StrokeEllipse( Retouch.hBrush, &rEllipse, &ExteriorRect );
		SetMgxBrushStyle( Retouch.hBrush, brushStyle );
		SetMgxStylePressure( Retouch.hBrush, brushPressure );
		SetMgxBrushOpacity( Retouch.hBrush, brushOpacity );
		SetMgxBrushOpacityScale( Retouch.hBrush, brushScale );
		if( !lpExteriorFrame )
			goto ExitError;
	}
	else 
		ExteriorRect = rArea;
	
	// draw it
	lpBaseFrame = ImgGetBaseEditFrame( lpImage );

	if( !SetupDrawShapeColorLut( FrameDepth( lpBaseFrame )) )
		goto ExitError;

	brushOpacity = (( brushOpacity * brushScale ) + 50 ) / 100;

	SetEngineDraw( &Engine, OSDrawShapeDataProc, brushOpacity, (MERGE_MODE)brushMode );

	Engine.lpEditRect = &ExteriorRect;
	Engine.lpMaskProc = OSDrawShapeMaskProc;

	if( brushStyle == IDC_BRUSHSCATTER )
		Engine.ScatterPressure = brushPressure;

	LineEngineSelObj( lpImage, &Engine, dirty );

	FreeDrawShapeColorLut();

	if( lpMask )
		MaskClose( lpMask );
	if( lpExteriorFrame )
		FrameClose( lpExteriorFrame );
	ProgressEnd();
	UpdateImage( &rArea, YES );
	return;
	
ExitError:
	Message( IDS_EMEMALLOC );
	if( lpMask )
		MaskClose( lpMask );
	if( lpExteriorFrame )
		FrameClose( lpExteriorFrame );
	ProgressEnd();
}


//////////////////////////////////////////////////////////////////////////////
static LPFRAME OSBrushShape( LPSHAPE lpShape, HMGXBRUSH hBrush, LPRECT lpOutRect, 
							 BOOL fClosed )
//	Brushes all the shapes in the list into a frame.
//	Returns the frame, and sets lpOutRect to be the frames rectangle int the base.
//////////////////////////////////////////////////////////////////////////////
{
	int BrushFull, BrushHalf;
	LPPOINT lpPoints, lpBase;
	int nPoints, i;
	LPOBJECT lpObject = NULL;
	RECT rDraw;
	LPSHAPE lpCurShape;	 
	LPFRAME lpFrame;

	ProgressBegin( 1, 0 );
	// get drawing area
	BrushFull = GetMgxBrushSize( hBrush );
	BrushHalf = BrushFull / 2;
	rDraw = lpShape->rShape;  
	lpCurShape = lpShape->lpNext;
	while ( lpCurShape )
	{
		AstralUnionRect( &rDraw, &rDraw, &lpCurShape->rShape );
		lpCurShape = lpCurShape->lpNext;
	}
	rDraw.top -= BrushHalf;
	rDraw.left -= BrushHalf;
	rDraw.bottom += ( BrushFull-BrushHalf );
	rDraw.right += ( BrushFull-BrushHalf );

	if( !MgxOSBrushBegin( hBrush, &rDraw, &rDraw ))
		return( NULL );
		
	lpCurShape = lpShape;
	while ( lpCurShape )
	{
		// get expanded point list
		lpPoints = lpBase = ( LPPOINT )LineBuffer[0];
		nPoints = ShapeConvertPoints( lpCurShape, lpPoints );
		MgxOSPickupBrush();
		lpPoints = ( LPPOINT )LineBuffer[0];
		for ( i = 0; i < nPoints; i++ )
		{
			AstralClockCursor( i, nPoints, NO );
			MgxOSBrushStroke( lpPoints[i].x, lpPoints[i].y );
		}
		if( fClosed )
			MgxOSBrushStroke( lpPoints[0].x, lpPoints[0].y );
		lpCurShape = lpCurShape->lpNext;
	}
	lpFrame =  MgxOSBrushEnd( lpOutRect );
	ProgressEnd();
	return( lpFrame );
}

//////////////////////////////////////////////////////////////////////////////
static void OSDrawShapeDataProc( int y, int left, int right, LPTR lpDst, 
								 LPTR lpSrc, int depth )
//////////////////////////////////////////////////////////////////////////////
{
	int dx, width, d;
	LPTR lpMaskBuf, lpInterior;
	LPTR lpExteriorLine, lpExt;
	LPTR lpSource;
	
	if( lpDrawMask && ( left < DrawMaskOffset.x || y < DrawMaskOffset.y ))
		return; // should NEVER happen
	
	width = right-left+1;
	lpMaskBuf = lpExteriorLine = NULL;
	lpSource = lpSrc;
	
	if( lpExteriorFrame )
		lpExteriorLine = FramePointer( lpExteriorFrame, 
							left-ExteriorRect.left, y-ExteriorRect.top, NO );

	if( !lpExteriorLine )
	{
		// pure internal color
		lpSource = lpSrc;
		for ( d=0;d<depth;d++ )
		{
			dx = width;
			while ( --dx >= 0 )
			{
				*lpSrc = lpColorLuts[d][0];
				lpSrc += depth;
			}
			lpSrc = ++lpSource;
		}
		return;
	}
	
	if( lpDrawMask )
	{
		lpMaskBuf = lpDrawMask->mbuffer;
		MaskLoad( lpDrawMask, left-DrawMaskOffset.x, y-DrawMaskOffset.y, 
					width, lpMaskBuf );
	}
	
	lpInterior = lpMaskBuf;
	lpExt = lpExteriorLine;
	// do one channel at a time, combining data
	for ( d=0;d<depth;d++ )
	{
		dx = width;
		if( !lpMaskBuf )
			while ( --dx >= 0 )
			{
				*lpSrc = lpColorLuts[d][255];
				lpSrc += depth;
			}
		else
			while ( --dx >= 0 )
			{
				if( !( *lpInterior++ ))
					// pure external color
					*lpSrc = lpColorLuts[d][255];
				else
					// combine data
					*lpSrc = lpColorLuts[d][*lpExt];
				lpSrc += depth;
				lpExt++;
			}
		// begin next plane
		lpInterior = lpMaskBuf;
		lpExt = lpExteriorLine;
		lpSrc = ++lpSource; 
	}
}


//////////////////////////////////////////////////////////////////////////////
static void OSDrawShapeMaskProc( int y, int left, int right, LPTR lpMsk )
//////////////////////////////////////////////////////////////////////////////
{
	int width;
	LPTR lpMaskBuf;
	LPTR lpExteriorLine;

	if( lpDrawMask && ( left < DrawMaskOffset.x || y < DrawMaskOffset.y ))
		return; // should NEVER happen
	
	width = right-left+1;
	lpMaskBuf = lpExteriorLine = NULL;
	
	if( lpExteriorFrame )
		lpExteriorLine = FramePointer( lpExteriorFrame, 
							left-ExteriorRect.left, y-ExteriorRect.top, NO );
			
	if( lpDrawMask )
	{
		lpMaskBuf = lpDrawMask->mbuffer;
		MaskLoad( lpDrawMask, left-DrawMaskOffset.x, y-DrawMaskOffset.y, 
					width, lpMaskBuf );
	}
	
	if( !lpExteriorLine && !lpMaskBuf )
		return; // should NEVER happen
	if( !lpExteriorLine )
	{	
		// mask interior only
		and( lpMaskBuf, lpMsk, width );	 
		return;
	}
	
	if( !lpMaskBuf )
		// Scale lpMsk by lpExteriorLine
		while ( --width >= 0 )
			*lpMsk++ = ( *lpMsk*( long )*( lpExteriorLine++ ))/255;
	else
		while ( --width >= 0 )
		{
			if( *lpMaskBuf++ )
			{
				// pure mask
				lpMsk++;
				lpExteriorLine++;
			}
			else
				// Scale lpMsk by lpExteriorLine
				*lpMsk++ = ( *lpMsk*( long )*( lpExteriorLine++ ))/255;
		}
}


static int vx, vy, gx, gy, begin_fx, begin_fy, veccnt;
static LPPOINT vecptr, vecbase;
static BOOL bClosed;
//////////////////////////////////////////////////////////////////////////////
int FAR DrawFreehandProc(
//////////////////////////////////////////////////////////////////////////////
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
	LPRECT lpRect;
	int x, y;
	BOOL fCancel;
	LPSHAPE lpShape;

	fCancel = FALSE;
	x = LOWORD( lParam );
	y = HIWORD( lParam );

	switch ( msg )
	{
		case WM_CREATE: // The first mouse down message
			if (!ONIMAGE(x, y))
				break;
			Display2File(&x, &y);
			if ( !ImgSelectObjectType( lpImage, x, y,
				YES/*bAllowBase*/, YES/*bAllowMovable*/, NO/*bAllowPlayable*/ ) )
					break;
			if( !Retouch.hBrush )
				break;

			AstralUpdateWindow( hWindow );

			Tool.bActive = YES;

			bClosed = ( iTool == 4 );

			vecptr = vecbase = ( LPPOINT )LineBuffer[0];
			veccnt = 0;

			gx = vx = begin_fx = x;
			gy = vy = begin_fy = y;

			// Draw a guide line from the last point to the cursor
			DrawGuideLine( 0, vx, vy, gx, gy, ON );

			SetDisplayHook( hWindow, FreehandDrawProc );
			break;

		case WM_SETCURSOR:
		return( SetToolCursor( hWindow, NULL/*idMoveObject*/, ID_PLAY_OBJECT ) );

		case WM_LBUTTONDOWN:
			if( !bClosed && veccnt )
			{		// finish on the down if !closed and only moved a little
				if( abs( gx-vx ) <= CLOSENESS && abs( gy-vy ) <= CLOSENESS )
					goto Dbl;
			}
			lpRect = &lpImage->lpDisplay->DispRect;

			x = bound( x, lpRect->left, lpRect->right );
			y = bound( y, lpRect->top, lpRect->bottom );

			Display2File( &x, &y );

			vector_draw( x, y, YES );

			break;


		case WM_MOUSEMOVE:	// sent when ToolActive is on
			lpRect = &lpImage->lpDisplay->DispRect;

			x = bound( x, lpRect->left, lpRect->right );
			y = bound( y, lpRect->top, lpRect->bottom );

			Display2File( &x, &y );

			vector_draw( x, y, Window.fLButtonDown );
		//	vector_draw( x, y, bClosed ? Window.fLButtonDown : NO );

			break;


		case WM_DESTROY:	// The cancel operation message
			if( !Tool.bActive )
				break;

			if( lParam == 1 )	 // cancel function 
			{
				Tool.bActive = NO;
				SetDisplayHook( hWindow, NULL );

				// Undraw the guide line
				DrawGuideLine( 0, vx, vy, gx, gy, OFF );

				// Draw the final segment 
				if( bClosed )
					DrawGuideLine( 0, vx, vy, begin_fx, begin_fy, ON );

				// Undraw the shape 
				FreehandDraw( NULL, ( LPPOINT )LineBuffer[0], &veccnt, 1, 
					bClosed, NO );
				break;
			}
			// fall through to ending function 


		case WM_LBUTTONUP:
			if( !bClosed ) // don't finish on the up if !closed
				break;

			// fall through to ending function


		case WM_LBUTTONDBLCLK:
Dbl:
			if( bClosed && msg == WM_LBUTTONDBLCLK )
				FreehandBackup();

			lpRect = &lpImage->lpDisplay->DispRect;
			x = bound( x, lpRect->left, lpRect->right );
			y = bound( y, lpRect->top, lpRect->bottom );

			Display2File( &x, &y );

			Tool.bActive = NO;

			SetDisplayHook( hWindow, NULL );

			// Undraw the guide line
			DrawGuideLine( 0, vx, vy, gx, gy, OFF );

			if( veccnt <= 1 )
				break;

			// Draw the final segment
			if( bClosed )
				DrawGuideLine( 0, vx, vy, begin_fx, begin_fy, ON );

			if( lpShape = MaskCreateShapes( vecbase, &veccnt, 1, SHAPE_DEFAULT, NO, NULL ))
			{
				if( bClosed )
					DrawShapeClosed( lpShape, IDS_UNDOCHANGE );
				else
					DrawShapeOpen( lpShape, IDS_UNDOCHANGE );

				FreeUpShapes( lpShape );
			}
			else
				Message( IDS_EMEMALLOC );

			break;
	}

	return( TRUE );
}


//////////////////////////////////////////////////////////////////////////////
static void FreehandDrawProc( HDC hDC, LPRECT lpRect )
//////////////////////////////////////////////////////////////////////////////
{
	// draw the shape
	FreehandDraw( hDC, vecbase, &veccnt, 1, bClosed, YES );

	// draw the guide line
	DrawGuideLine( hDC, vx, vy, gx, gy, ON );
}


//////////////////////////////////////////////////////////////////////////////
static void FreehandDraw(
//	hDC can be NULL
//////////////////////////////////////////////////////////////////////////////
HDC 	hDC,
LPPOINT lpPoint,
LPINT 	npoints,
int 	nShapes,
BOOL 	fClosed,
BOOL 	fOn)	// draw or undraw
{
int count, i;
LPPOINT lpNextPoint, lpFirstPoint;

lpFirstPoint = lpPoint;
for ( i = 0; i < nShapes; ++i )
	{
	count = npoints[i];
	while ( --count >= 0 )
		{
		if( count )
			lpNextPoint = lpPoint + 1;
		else
			{
			if( !fClosed )
				break;
			lpNextPoint = lpFirstPoint;
			}

		DrawGuideLine( hDC, lpPoint->x, lpPoint->y, 
			lpNextPoint->x, lpNextPoint->y, fOn );
		++lpPoint;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
void FreehandBackup()
//////////////////////////////////////////////////////////////////////////////
{
int x2, y2;

if( !lpImage || !Tool.bActive )
	return;

// Undraw the guide line ... 
DrawGuideLine( 0, vx, vy, gx, gy, OFF );
if( veccnt <= 1 )
	{
	SetDisplayHook( lpImage->hWnd, NULL );
	Tool.bActive = NO;
	}
else
	{
	vecptr--; veccnt--;
	// Undraw the vector 
	vx = ( vecptr - 1 )->x; vy = ( vecptr - 1 )->y;
	x2 = vecptr->x; y2 = vecptr->y;
	DrawGuideLine( 0, vx, vy, x2, y2, OFF );
	// Draw the guide line 
	DrawGuideLine( 0, vx, vy, gx, gy, ON );
	}
}


//////////////////////////////////////////////////////////////////////////////
void vector_draw(
//////////////////////////////////////////////////////////////////////////////
int 	x,
int		y,
BOOL 	bCapture)
{
	if( !bCapture )
	{
		// if the cursor didn't move... 
		if( x == gx && y == gy )
				return;

		// Undraw the guide line ... 
		DrawGuideLine( 0, vx, vy, gx, gy, OFF );

		// .. and draw a new one 
		gx = x; gy = y;
		DrawGuideLine( 0, vx, vy, gx, gy, ON );
		return;
	}

	// Undraw the guide line ... 
	DrawGuideLine( 0, vx, vy, gx, gy, OFF );
	DrawGuideLine( 0, vx, vy, x, y, ON );		// draw the new vector

	// Capture the new vector 
	vecptr->x = x;
	vecptr->y = y;
	vecptr++;
	veccnt++;
	vx = gx = x; vy = gy = y;

	DrawGuideLine( 0, vx, vy, gx, gy, ON );		// draw the guide line 
}

//////////////////////////////////////////////////////////////////////////////
void UpdateRibbon( HWND hDlg )
//	changes icons in tool ribbon based on switch settings
//////////////////////////////////////////////////////////////////////////////
{
	ITEMID idFirst = IDC_SHAPES_T1, id, idOffset;
	HWND hControl;
	int iCount;


	if( !bWireFrame && !b3D )		 // 2D filled
		idOffset = IDC_SHAPES_T1 - idFirst;

	else 
	if( bWireFrame && !b3D )		 // 2D unfilled
		idOffset = IDC_SHAPES_T11 - idFirst;

	else
	if( !bWireFrame && b3D )		 // 3D filled
		idOffset = IDC_SHAPES_T21 - idFirst;

	else
		idOffset = IDC_SHAPES_T31 - idFirst;	// 3D unfilled

	iCount = 10;

	for( id = idFirst; id < idFirst + iCount; id++ )
	{
		if( !( hControl = GetDlgItem( hDlg, id )))
			continue;

		SetWindowWord( hControl, GWW_ICONID, id + idOffset );

		if( IsWindowVisible( hControl ))
			InvalidateRect( hControl, NULL, FALSE );
	}
}

