//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern BOOL bHexBrush;      // used by hexcircle brush with virt keys

// constant converts radians to degrees
#define RAD 57.29577951
#define ZIGZAG_BRUSHSIZE 2

int HollowBrushScale;       // used by retouch.c
int CircleScale = 25;            // used by retouch.c

static void ZigZagDraw( POINT );
static void LoopDraw( POINT );
static int compute_theta( int, int );
static void create_loop_template( void );

static BOOL bMirror;
static int iTool;

/***********************************************************************/
BOOL WINPROC EXPORT DlgCrayonProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
    {
    case WM_INITDIALOG:
	RibbonInit( hDlg );
	ColorInit( hDlg );
	CheckRadioButton( hDlg, IDC_CRAYON_T1, IDC_CRAYON_T10,
		iTool+IDC_CRAYON_T1 );
	CheckDlgButton( hDlg, IDC_CRAYON_SW1, bMirror );
	#ifdef STUDIO
		InitSlide( hDlg, IDC_BRUSHSIZE, Retouch.BrushSize, 1, MAX_BRUSH_SIZE );
	#else
		switch (Control.iAdventBrush)
		{
			case BRUSH_SIZE_SMALL:	Retouch.BrushSize = 10; break;
			case BRUSH_SIZE_MEDIUM:	Retouch.BrushSize = 15; break;
			case BRUSH_SIZE_LARGE: 	Retouch.BrushSize = 20; break;
			default: 				Retouch.BrushSize = 15; break;
		}
//		Retouch.BrushSize = 15;
	#endif

	// Fall thru...

    case WM_CONTROLENABLE:
    case WM_DOCACTIVATED:
	Retouch.hBrush = CreateSimpleBrush( Retouch.hBrush );
	SetBrushOptions( Retouch.hBrush, iTool );
    break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, Tool.idRibbon ) );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	LayoutPaint( hDlg );
	break;

    case WM_DESTROY:
   	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
		case IDC_ACTIVECOLOR:
		case IDC_COLORS_CRAYON:
		ColorCommand( hDlg, wParam, lParam );
		break;

		case IDC_CRAYON_T1:
		case IDC_CRAYON_T2:
		case IDC_CRAYON_T3:
		case IDC_CRAYON_T4:
		case IDC_CRAYON_T5:
		case IDC_CRAYON_T6:
		case IDC_CRAYON_T7:
		case IDC_CRAYON_T8:
		case IDC_CRAYON_T9:
		case IDC_CRAYON_T10:
    	CheckRadioButton( hDlg, IDC_CRAYON_T1, IDC_CRAYON_T10, wParam );
	    iTool = wParam - IDC_CRAYON_T1;
   		SetBrushOptions( Retouch.hBrush, iTool );
    	break;

        case IDC_BRUSHSIZE:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;

		if (lpImage)
	   		DisplayBrush(0, 0, 0, OFF);
		Retouch.BrushSize = HandleSlide( hDlg, (ITEMID)wParam, (UINT)lParam, NULL );

   		SetBrushOptions( Retouch.hBrush, iTool );

		if (lpImage && Window.hCursor == Window.hNullCursor)
			DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
		break;

	    case IDC_CRAYON_SW1:
		bMirror = !bMirror;
		CheckDlgButton( hDlg, wParam, bMirror );
		break;

	    default:
		return( FALSE );
		}
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}

/************************************************************************/
static void SetBrushOptions( HMGXBRUSH hBrush, int iTool )
/************************************************************************/
{
	if (Retouch.BrushSize > MAX_BRUSH_SIZE)
		Retouch.BrushSize = MAX_BRUSH_SIZE;

	if (Retouch.BrushSize < 1)
		Retouch.BrushSize = 1;

    bHexBrush = NO;

	SetMgxBrushOverlap( Retouch.hBrush, NO/*bOverlap*/ );
    switch( iTool )
    {
	    case 0:     // circle
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
    	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 25 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;            

	    case 1:     // glitter
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHGLITTERCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
    	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 25 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;            

	    case 2:     // swirl
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHSWIRLCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
    	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 25 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;            

	    case 3:     // six circles
            bHexBrush = YES;
            CircleScale = 25;
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHHEXCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );

            // brushsize must be even to maintain precision
            if(( Retouch.BrushSize % 2 ) && ( Retouch.BrushSize > 6 ))
                Retouch.BrushSize++;

       	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 50 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;

	    case 4:     // hollow 
            HollowBrushScale = 80;
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHHOLLOWCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
    	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 50 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;

	    case 5:     // scatter
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSCATTER );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
    	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 25 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;            

	    case 6:     // ripple
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
    	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 100 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;            

	    case 7:     // fade
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
    	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 25 );
            SetSimpleBrushFade( Retouch.hBrush, 75 );
            break;            

	    case 8:     // zigzag
	    case 9:     // loops
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
            SetSimpleBrushSize( Retouch.hBrush, ZIGZAG_BRUSHSIZE );
            SetCommonBrushSize( ZIGZAG_BRUSHSIZE );
            SetSimpleBrushSpacing( Retouch.hBrush, 25 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;

        default:
            SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHCIRCLE );
            SetSimpleBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
            SetSimpleBrushStylePressure( Retouch.hBrush, 50 );
    	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
            SetCommonBrushSize( Retouch.BrushSize );
            SetSimpleBrushSpacing( Retouch.hBrush, 25 );
            SetSimpleBrushFade( Retouch.hBrush, 0 );
            break;            
    }
}

static POINT ptPrev;
static LPPOINT lpPoints;
static int nPoints;
/************************************************************************/
int CrayonProc( HWND hWindow, LPARAM lParam, UINT msg )
/************************************************************************/
{
    POINT ptBrush;
    int x, y, fx, fy;
    LPOBJECT lpObject;

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    switch (msg)
    {
        case WM_CREATE:	// The first mouse down message
		if (!ONIMAGE(x, y))
			break;
		Display2File(&x, &y);
		if ( !ImgSelectObjectType( lpImage, x, y,
			YES/*bAllowBase*/, YES/*bAllowMovable*/, NO/*bAllowPlayable*/ ) )
				break;
	    if( !Retouch.hBrush )
		    break;

	    Tool.bActive = YES;

        if( iTool == 9 )
            create_loop_template();

	    break;

		case WM_SETCURSOR:
		return( SetToolCursor( hWindow, NULL/*idMoveObject*/, ID_PLAY_OBJECT ) );

	    case WM_ACTIVATE:
	    if (!lParam) // a deactivate
		    {
		    if (Retouch.hBrush)
			    DestroySimpleBrush(Retouch.hBrush);
		    Retouch.hBrush = NULL;
		    }
	    else
		    { // an activate ( to re-create brush)
		    if ( Tool.hRibbon )
			    SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
		    }
	    break;

        case WM_LBUTTONDOWN:
	    Tool.bActive = NO;
	    if (ImgCountSelObjects(lpImage, NULL) == 1)
		    lpObject = ImgGetSelObject(lpImage, NULL);
	    else
		    {
		    fx = x;
		    fy = y;
		    Display2File( &fx, &fy );
		    lpObject = ImgFindSelObject(lpImage, fx, fy, NO);
		    }
	    if (!lpObject)
		    break;
	    if (!ImgEditInit(lpImage, ET_OBJECT, UT_DATA, lpObject))
		    break;

#ifdef ADVENT
		switch (Control.iAdventBrush)
		{
			case BRUSH_SIZE_SMALL:	Retouch.BrushSize = 10; break;
			case BRUSH_SIZE_MEDIUM:	Retouch.BrushSize = 15; break;
			case BRUSH_SIZE_LARGE: 	Retouch.BrushSize = 20; break;
			default: 				Retouch.BrushSize = 15; break;
		}
   	    SetSimpleBrushSize (Retouch.hBrush, Retouch.BrushSize);
#endif
	    if (!SimpleBrushBegin(hWindow, &lpObject->Pixmap,
		    lpObject->rObject.left, lpObject->rObject.top, Retouch.hBrush))
	        break;
		SetSimpleBrushMirrorMode (Retouch.hBrush, bMirror);
	    ImgEditedObject(lpImage, lpObject, IDS_UNDOCHANGE, NULL);
	    Tool.bActive = SimpleBrushActive();

        if(( iTool == 8 || iTool == 9 ) && Retouch.BrushSize >= ( 4 * ZIGZAG_BRUSHSIZE ))
        {
            // put brush down to begin drawing
            ptPrev.x = fx = x;
            ptPrev.y = fy = y;

            if( iTool == 8 )
            {
                Display2File( &fx, &fy );
                SimpleBrushStroke( fx, fy, x, y );
            }
        }

	    /* fall through to WM_MOUSEMOVE */

        case WM_MOUSEMOVE:	// sent when ToolActive is on
        if(( iTool == 8 || iTool == 9 ) && Retouch.BrushSize >= ( 4 * ZIGZAG_BRUSHSIZE ))
        {
            ptBrush.x = x;
            ptBrush.y = y;    

            if( iTool == 8 )
                ZigZagDraw( ptBrush );
            else
                LoopDraw( ptBrush );
        }
        else
        {
	        fx = x;
	        fy = y;
	        Display2File( &fx, &fy );
	        SimpleBrushStroke(fx, fy, x, y);
        }
	    break;

        case WM_LBUTTONUP:
	    SimpleBrushEnd(NO);
	    Tool.bActive = NO;
	    break;

        case WM_LBUTTONDBLCLK:
	    break;

        case WM_DESTROY:	// The cancel operation message
	    SimpleBrushEnd(NO);
	    Tool.bActive = NO;
	    break;
    }

    return(TRUE);
}


//////////////////////////////////////////////////////////////////////////////
void ZigZagDraw( POINT ptBrush )
// line is modeled along +x, -y axis and rotated to desired position.
//////////////////////////////////////////////////////////////////////////////
{
    POINT ptFile, ptLast, ptTransform;
    TFORM tform;
    int nLength, x_inc, y_inc, nPeaks;
    int lx, ly, tx, ty;                     // vectors
    int i, j;                               // loop counters
    int theta;                              // rotation angle
    double dx, dy;

    // if mouse hasn't moved, get out
    if( ptPrev.x == ptBrush.x && ptPrev.y == ptBrush.y )      
        return;

    // compute length of zigzag line rotation
    tx = lx = ptBrush.x - ptPrev.x; 
    ty = ly = ptPrev.y  - ptBrush.y; 

    // length = ( x^2 + y^2 )^1/2
    dx = pow( lx, 2 );        // square x distance
    dy = pow( ly, 2 );        // square y distance
    nLength = ( int )( sqrt( dx + dy ));    // Pythagarus lives!!!

    // has the mouse moved enough to draw at least one peak?
    if(( nPeaks = nLength / ( Retouch.BrushSize / 2 )) < 1 )
        return;

    // we can draw something!!!
    theta = compute_theta( tx, ty );

    // create unity matrix
    TInit( &tform ); 

    // set transform matrix
	TMove( &tform, -ptPrev.x, -ptPrev.y );
	TRotate( &tform, theta, theta );
	TMove( &tform, ptPrev.x, ptPrev.y ) ;

    // peak width is the brush size / 4
    x_inc = Retouch.BrushSize / 4;

    // peak height is the brush size
    y_inc = Retouch.BrushSize;

    // start drawing where we stopped previously
    ptLast.x = ptPrev.x;
    ptLast.y = ptPrev.y;  

    for( i = 0; i < nPeaks; i++ )
    {
        for( j = 0; j < 2; j++ )
        {
            ptLast.x += x_inc;              // 1/2 of a peak

            if( !j )                        // upstroke
                ptLast.y -= y_inc;
            else                            // downstroke
                ptLast.y += y_inc;

            // preserve point
            ptTransform.x = ptLast.x;       
            ptTransform.y = ptLast.y;

            // apply transform matrix
            Transformer( &tform, &ptTransform, (LPINT)&ptTransform.x, (LPINT)&ptTransform.y );

            ptFile.x = ptTransform.x;
            ptFile.y = ptTransform.y;

            Display2File( (LPINT)&ptFile.x, (LPINT)&ptFile.y );
       	    SimpleBrushStroke( ptFile.x, ptFile.y, ptTransform.x, ptTransform.y );
        }
    }
    // save transformed terminal position
    ptPrev.x = ptTransform.x;   
    ptPrev.y = ptTransform.y;
}


//////////////////////////////////////////////////////////////////////////////
void LoopDraw( POINT ptBrush )
// line is modeled along +x, -y axis and rotated to desired position.
//////////////////////////////////////////////////////////////////////////////
{
    POINT ptFile, ptTransform;
    TFORM tform;
    int nLength, nLoops, nOffset_x, nOffset_y, nPointsSkipped;
    int lx, ly, tx, ty;                     // vectors
    int i, j;                            // loop counters
    int theta;                              // rotation angle
    double dx, dy;


    // if mouse hasn't moved, get out
    if( ptPrev.x == ptBrush.x && ptPrev.y == ptBrush.y )      
        return;

    // compute length of loop line rotation
    tx = lx = ptBrush.x - ptPrev.x; 
    ty = ly = ptPrev.y  - ptBrush.y; 

    // length = ( x^2 + y^2 )^1/2
    dx = pow( lx, 2 );        // square x distance
    dy = pow( ly, 2 );        // square y distance
    nLength = ( int )( sqrt( dx + dy ));    // Pythagarus lives!!!


    // has the mouse moved enough to draw at least one loop?
    if(( nLoops = nLength / ( Retouch.BrushSize / 2 )) < 1 )
        return;

    // we can draw something!!!
    nOffset_x = ptPrev.x;
    nOffset_y = ptPrev.y - Retouch.BrushSize;

    // compute angle of loop line rotation
    theta = compute_theta( tx, ty );

    // create unity matrix
    TInit( &tform ); 

    // set transform matrix
	TMove( &tform, -ptPrev.x, -ptPrev.y );
	TRotate( &tform, theta, theta );
	TMove( &tform, ptPrev.x, ptPrev.y );

    // nPointSkipped = square root of (( nPoints ) - 1 ) gives good 
    // granularity with large brushes and speeds up brushes of all sizes.
    if(( nPointsSkipped = ( int )( sqrt(( double )( nPoints - 1 )) - 1 )) < 1 )
        nPointsSkipped = 1;

    for( i = 0; i < nLoops; i++ )
    {
        for( j = 0; j < nPoints; j += nPointsSkipped )      
        {
            // preserve point
            ptTransform.x = lpPoints[j].x + nOffset_x;       
            ptTransform.y = lpPoints[j].y + nOffset_y;

            // apply transform matrix
            Transformer( &tform, &ptTransform, (LPINT)&ptTransform.x, (LPINT)&ptTransform.y );

            ptFile.x = ptTransform.x;
            ptFile.y = ptTransform.y;

            Display2File( (LPINT)&ptFile.x, (LPINT)&ptFile.y );
       	    SimpleBrushStroke( ptFile.x, ptFile.y, ptTransform.x, ptTransform.y );
        }

        // draw last point to finish up 
        if(( j - nPointsSkipped ) < ( nPoints - 1 ))
        {
            // preserve point
            ptTransform.x = lpPoints[nPoints - 1].x + nOffset_x;       
            ptTransform.y = lpPoints[nPoints - 1].y + nOffset_y;

            // apply transform matrix
            Transformer( &tform, &ptTransform, (LPINT)&ptTransform.x, (LPINT)&ptTransform.y );

            ptFile.x = ptTransform.x;
            ptFile.y = ptTransform.y;

            Display2File( (LPINT)&ptFile.x, (LPINT)&ptFile.y );
       	    SimpleBrushStroke( ptFile.x, ptFile.y, ptTransform.x, ptTransform.y );
        }
        nOffset_x += Retouch.BrushSize / 2;
    }

    // save transformed terminal position
    ptPrev.x = ptTransform.x;   
    ptPrev.y = ptTransform.y;
}


//////////////////////////////////////////////////////////////////////////////
int compute_theta( int lx, int ly )
//  determines the angle of a line. clockwise rotation is positive between 0 
//  and 360 degrees.
//////////////////////////////////////////////////////////////////////////////
{
    int theta;

    // arctan function not defined for 360 mod 90 = zero
    if( lx == 0 )           // if only move vertically
    {
        if( ly > 0 )        // moved up
            theta = 270;
        else                // moved down
            theta = 90;
    }
    else
    if( ly == 0 )           // if only moved horizontally
    {
        if( lx < 0 )        // moved left
            theta = 180;
        else                // moved right 
            theta = 0;
    }
    else
    {
        // compute angle and convert to degrees
        theta = ( int )( atan(( double )ly / ( double )lx ) * RAD );

        if( theta == 0 )    //  truncated when cast to int
        {
            if( lx > 0 )
                theta = 0;
            else
                theta = 180;
        }
        else
        {
            if( theta < 0 )           
            {
                if( lx < 0 )                // 2nd quad
                    theta += 180;     
            }
            else                        
            {                           
                if( ly < 0 )            
                    theta -= 180;       
            }
            theta *= -1;               
        }
    }
    return theta;
}

//////////////////////////////////////////////////////////////////////////////
void create_loop_template( void )
//  generate points for one loop. this loop is then mapped into the current line.
//////////////////////////////////////////////////////////////////////////////
{
    RECT rect;
    POINT deltas, mids;
    POINT Points[20];
    TFORM tform;

    // create loop rect = Retouch.BrushSize x Retouch.BrushSize / 2
	SetRect( &rect, 0, 0, Retouch.BrushSize / 2, Retouch.BrushSize );

	deltas.x = ( 2 * ( rect.right - rect.left )) / 9;
	deltas.y = ( 2 * ( rect.bottom - rect.top )) / 9;
	mids.x   = ( rect.left + rect.right ) / 2;
	mids.y   = ( rect.top + rect.bottom ) / 2;

    // lower right bezier arc
	Points[0].x = BEZIER_MARKER;
	Points[0].y = 4;
	Points[1].x = rect.left;
	Points[1].y = rect.bottom;
	Points[2].x = rect.right - deltas.x;
	Points[2].y = rect.bottom;
	Points[3].x = rect.right;
	Points[3].y = rect.bottom - deltas.y;
	Points[4].x = rect.right;
	Points[4].y = mids.y;

    // upper right bezier arc
	Points[5].x = BEZIER_MARKER;
	Points[5].y = 4;
	Points[6].x = rect.right;
	Points[6].y = mids.y;
	Points[7].x = rect.right;
	Points[7].y = rect.top + deltas.y;
	Points[8].x = rect.right - deltas.x;
	Points[8].y = rect.top;
	Points[9].x = mids.x;
	Points[9].y = rect.top;

    // upper left bezier arc
	Points[10].x = BEZIER_MARKER;
	Points[10].y = 4;
	Points[11].x = mids.x;
	Points[11].y = rect.top;
	Points[12].x = rect.left + deltas.x;
	Points[12].y = rect.top;
	Points[13].x = rect.left;
	Points[13].y = rect.top + deltas.y;
	Points[14].x = rect.left;
	Points[14].y = mids.y;

    // lower left bezier arc
	Points[15].x = BEZIER_MARKER;
	Points[15].y = 4;
	Points[16].x = rect.left;
	Points[16].y = mids.y;
	Points[17].x = rect.left;
	Points[17].y = rect.bottom - deltas.y;
	Points[18].x = rect.left + deltas.x;
	Points[18].y = rect.bottom;
	Points[19].x = rect.right;
	Points[19].y = rect.bottom;

    nPoints = 20;

	lpPoints = ( LPPOINT )LineBuffer[0];

    TInit( &tform );

    nPoints = ExpandPoints( &Points[0], lpPoints, nPoints, 
         				    &tform, &lpPoints[0], &lpPoints[ nPoints - 1 ] );
}


