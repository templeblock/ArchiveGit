//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
#ifndef __routines
#define __routines

/***********************************************************************
 * Routines.h	declarations of all procedures in picture publisher
***********************************************************************/

typedef BOOL bool;

#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "machack.h"
#include "types.h"
#include "framelib.h"
#include "superlib.h"
#include "mgxutils.h"
#include "macros.h"

#include "object.h"

#ifndef _INC_OLE
#include <ole.h>
#endif
#include "brush.h"
#include "drive.h"
#include "engine.h"
#include "image.h"
#include "mask.h"
#include "objio.h"
#include "pixmap.h"
#include "ppole.h"
#include "select.h"
#include "shape.h"
#include "tiffio.h"

#include "app.h"
#include "data.h"

 #include "about.p"
 #include "animate.p"
 #include "animbtns.p"
 #include "astral.p"
 #include "automask.p"
 #include "blend.p"
 #include "bmprdr.p"
 #include "bmpwrt.p"
 #include "bomb.p"
 #include "bruprocs.p"
 #include "brush.p"
 #include "brushsim.p"
 #include "brushup.p"
 #include "check.p"
 #include "class.p"
 #include "cmask.p"
 #include "color.p"
 #include "combo.p"
 #include "commands.p"
 #include "compress.p"
 #include "constrai.p"
 #include "controls.p"
 #include "cpo.p"
 #include "cursors.p"
 #include "cut.p"
 #include "dda.p"
 #include "defaults.p"
 #include "dlgfile.p"
 #include "dlgmgr.p"
 #include "doclist.p"
 #include "drawfram.p"
 #include "drive.p"
 #include "effects.p"
 #include "engine.p"
 #include "explode.p"
 #include "export.p"
 #include "extname.p"
 #include "filt.p"
 #include "filter.p"
 #include "flood.p"
 #include "fonts.p"
 #include "getdev.p"
 #include "gallery.p"
 #include "help.p"
 #include "hidden.p"
 #include "image.p"
 #include "imgutil.p"
 #include "intelio.p"
 #include "intermsn.p"
 #include "layout.p"
 #include "makepath.p"
 #include "marquee.p"
 #include "mask.p"
 #include "maskio.p"
 #include "measure.p"
 #include "menu.p"
 #include "merge.p"
 #include "message.p"
 #include "mgxdde.p"
 #include "object.p"
 #include "objedit.p"
 #include "objio.p"
 #include "palio.p"
 #include "pallist.p"
 #include "palprocs.p"
 #include "path.p"
 #include "pixmap.p"
 #include "popups.p"
 #include "ppole.p"
 #include "ppoledoc.p"
 #include "ppoleobj.p"
 #include "ppolesvr.p"
 #include "print.p"
 #include "prnprocs.p"
 #include "process.p"
 #include "progmgr.p"
 #include "ps.p"
 #include "raddon.p"
 #include "radio.p"
 #include "rdot2dot.p"
 #include "register.p"
 #include "retouch.p"
 #include "rgallery.p"
 #include "rhidden.p"
 #include "rjokes.p"
 #include "rmazes.p"
 #include "rmixup.p"
 #include "rpuzzle.p"
 #include "rshapes.p"
 #include "rslide.p"
 #include "rspotdot.p"
 #include "rooms.p"
 #include "rotate.p"
 #include "rsubsets.p"
 #include "rulers.p"
 #include "rwho.p"
 #include "rworld.p"
 #include "rwrong.p"
 #include "saveload.p"
 #include "scroll.p"
 #include "select.p"
 #include "slider.p"
 #include "smart.p"
 #include "sound.p"
 #include "spin.p"
 #include "tanimate.p"
 #include "tcontrol.p"
 #include "tcrayon.p"
 #include "tdraw.p"
 #include "text.p"
 #include "teffects.p"
 #include "terase.p"
 #include "tfill.p"
 #include "tgallery.p"
 #include "tifrdr.p"
 #include "tifwrt.p"
 #include "tlines.p"
 #include "tmarker.p"
 #include "toolbar.p"
 #include "toolproc.p"
 #include "tpaint.p"
 #include "tprobe.p"
 #include "tracer.p"
 #include "track.p"
 #include "tstamp.p"
 #include "ttext.p"
 #include "ttform.p"
 #include "view.p"
 #include "vignette.p"
 #include "wacom.p"
 #include "wallpape.p"
 #include "winimage.p"
 #include "winpal.p"
 #include "winprocs.p"
 #include "wintools.p"

/************************************************************************/
/* Macro Subroutines							*/
/************************************************************************/

#define lstrchr(s,c) lStrChr(s,c)

#define CreateProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_CREATE )
#define ButtonDownProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_LBUTTONDOWN )
#define ButtonUpProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_LBUTTONUP )
#define MoveProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_MOUSEMOVE )
#define DoubleClickProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_LBUTTONDBLCLK )
#define TimerProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_TIMER )
#define DestroyProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_DESTROY )
#define CommandProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_COMMAND )
#define ActivateProc( hWindow, lParam ) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_ACTIVATE )
#define MsgProc( hWindow, lParam, msg ) \
	(*Tool.lpToolProc)( hWindow, lParam, msg )
#define CursorProc(hWindow, lParam) \
	(*Tool.lpToolProc)( hWindow, lParam, WM_SETCURSOR)

/* enginea.asm */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

// "C" functions - extern "C"
void ScatterMask(LPTR lpSrc, LPTR lpDst, LPINT lpErrors, int dx, int dy, int iPressure);
void MergeData8 (LPTR  lpDst, LPRECT lpDstRect, int DstWidth, LPTR  lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);
void MergeData24(LPRGB lpDst, LPRECT lpDstRect, int DstWidth, LPRGB lpSrc, LPRECT lpSrcRect, int SrcWidth, LPRECT lpSectRect);

#ifdef __cplusplus
}            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

#endif

