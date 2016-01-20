// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern WINDOW img_win;
extern BOOL CapsLock, ShiftKey, OptionKey, ControlKey;
extern BOOL processing_change, processing_undo, processing_apply;
extern Rect DocClientRect;

static int	mnx,mxx,mny,mxy;
static int x_start,x_end,y_start,y_end, begin_x, begin_y, begin_fx, begin_fy;
static BOOL fCircle, maskWaitMouseUp, rect_valid, fAbortAutoMask;
static int dx, dy, mx, my;
static FILE_SPEC clipfile;
static FILE *rscr;
static char	stry[20];
static int veccnt;
static LPPOINT vecptr;
static RECT CBRect;
static TFORM	tform, lastTform;
static RECT RBRect;
static int RBAspectW, RBAspectH;
static BOOL RBEllipse;
static BOOL maskshape_active = FALSE;
static BOOL RubberBanding, RubberBandingConstrain, RubberBandingAspect;

/************************************************************************/
HandleMaskScroll(cmd)
/************************************************************************/
    int cmd;
    {
    switch (cmd)
        {
        case IDM_SCROLLUP:
	    /* If we're in the middle of a mask operation, turn off the */
	    /* DIRTY_MASK flag so we can scroll - The mask routines will */
	    /* check the flag so it knows when to redraw the mask */
	    if ( lpImage->dirty & DIRTY_MASK ) /* If its on turn it off */
		lpImage->dirty &= (~DIRTY_MASK); /* beware the squiggle */
	    sameview( (lpImage->DispRect.left + lpImage->DispRect.right)/2,
		      min( lpImage->DispRect.top, lpImage->DispRect.bottom ) );
	    break;

        case IDM_SCROLLDOWN:
	    /* If we're in the middle of a mask operation, turn off the */
	    /* DIRTY_MASK flag so we can scroll - The mask routines will */
	    /* check the flag so it knows when to redraw the mask */
	    if ( lpImage->dirty & DIRTY_MASK ) /* If its on turn it off */
		lpImage->dirty &= (~DIRTY_MASK); /* beware the squiggle */
	    sameview( (lpImage->DispRect.left + lpImage->DispRect.right)/2,
		      max( lpImage->DispRect.bottom, lpImage->DispRect.top ) );
	    break;

        case IDM_SCROLLLEFT:
	    /* If we're in the middle of a mask operation, turn off the */
	    /* DIRTY_MASK flag so we can scroll - The mask routines will */
	    /* check the flag so it knows when to redraw the mask */
	    if ( lpImage->dirty & DIRTY_MASK ) /* If its on turn it off */
		lpImage->dirty &= (~DIRTY_MASK); /* beware the squiggle */
	    sameview( min( lpImage->DispRect.left, lpImage->DispRect.right ),
		      (lpImage->DispRect.top + lpImage->DispRect.bottom)/2 );
	    break;

        case IDM_SCROLLRIGHT:
	    /* If we're in the middle of a mask operation, turn off the */
	    /* DIRTY_MASK flag so we can scroll - The mask routines will */
	    /* check the flag so it knows when to redraw the mask */
	    if ( lpImage->dirty & DIRTY_MASK ) /* If its on turn it off */
		lpImage->dirty &= (~DIRTY_MASK); /* beware the squiggle */
	    sameview( max( lpImage->DispRect.right, lpImage->DispRect.left ),
	 	      (lpImage->DispRect.top + lpImage->DispRect.bottom)/2 );
	    break;
	    default:
	    break;
	    }
	}
	
/************************************************************************/
BOOL maskfree_backup()
/************************************************************************/
    {
    int x, y;
    
    if (!img_win)
        return(FALSE);
    SaveDrawState();
    AstralSetCurWindow(img_win);
    AstralResetForeBack();
    
        mouseget(&x, &y);
        
  	    /* If the user wants to delete a vector */
	    /* Undraw the guide line */
	    dline( mx, my, dx, dy, OFF, NULL,TRUE );
	    if (veccnt <= 1)
	        {
	        lpImage->dirty &= (~DIRTY_MASK); /* beware the squiggle in XyWrite */
            maskfree_active = FALSE;
            RestoreDrawState();
		    return(FALSE);
		    }
	    vecptr--; veccnt--;
	    /* Undraw the vector */
	     dline( (vecptr-1)->x, (vecptr-1)->y,
			     vecptr->x, vecptr->y, OFF, 1,TRUE );
	    mx = (vecptr - 1)->x; my = (vecptr - 1)->y;
	    /* Draw the guide line */
	    dx = x; dy = y;
		dline( mx, my, dx, dy, ON, NULL,TRUE );
		RestoreDrawState();
		return(TRUE);
	}

/************************************************************************/
BOOL disp_mask( lpPoint, lpTForm, flag, paste )
/************************************************************************/
LPPOINT lpPoint;
LPTFORM lpTForm;
int flag, paste;
{
int x, y, dx, dy, i, j, k, fCircle;
int x1, x2, y1, y2;
BYTE pval;
SPAN span;

if ( veccnt > 0 ) /* vector chain */
	{
	for (i=0; i<veccnt; i+=2)
		{
		/* Don't transform edges that indicate a circle is next */
		if ( PointIsNull( lpPoint ) && PointIsNull( (lpPoint+1) ) )
			{
			i += 2;
			lpPoint += 2;
			fCircle = YES;
			}
		else	fCircle = NO;
		Transformer( lpTForm, lpPoint, &x1, &y1 );
		lpPoint++;
		Transformer( lpTForm, lpPoint, &x2, &y2 );
		lpPoint++;
		if ( fCircle )
		    {
		    x = (x2+x1)/2;
		    y = (y2+y1)/2;
		    dx = (x2-x1)/2;
		    dy = (y2-y1)/2;
			ellipse(x, y, dx, dy, flag, NULL);
			}
		else
		    {
		    dline( x1, y1, x2, y2, flag, NULL, FALSE);
		    }
		}
	}

return( TRUE );
}


/************************************************************************/
void check_mask_redraw( x, y )
/************************************************************************/
    int x, y;
    {
    LPPOINT vptr;
    int tmpx, tmpy, i;
    
    if (maskfree_active)
        {
		if ( !(lpImage->dirty & DIRTY_MASK) )
			{
			/* If the dirty bit got turned off because of */
			/* scrolling, then offset and redraw the mask */
			lpImage->dirty |= DIRTY_MASK;
			begin_x = begin_fx;
			begin_y = begin_fy;
			file2disp( &begin_x, &begin_y );
			vptr = (LPPOINT)LineBuffer[0];
			tmpx = begin_x - vptr->x;
			tmpy = begin_y - vptr->y;
			mx = begin_x;
			my = begin_y;
			for ( i=0; i<veccnt; i++ )
				{
				vptr->x += tmpx;
				vptr->y += tmpy;
				dline( mx, my, vptr->x, vptr->y, ON, 1, FALSE );
				mx = vptr->x;
				my = vptr->y;
				vptr++;
				}
			/* Draw a new guide line */
			dx = x; dy = y;
			dline( mx, my, dx, dy, ON, NULL, TRUE );
	  	    }
	  	}
    }
    
