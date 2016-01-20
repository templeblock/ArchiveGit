#include "pp.h"
#include "ppafx.h"
#include "macros.h"
#include "id.h"
#include "cpo.h"
#include "svrview.h"
#include "dlgchrma.h"

__declspec( dllimport ) LPKCMSEVALPROC lpKCMSEvalProc;

#define LINES_PER_BLT 20

LPTR 		CImage::m_lpPRDispBuf = NULL;
LPTR 		CImage::m_lpPRFileBuf = NULL;
LPTR 		CImage::m_lpPRSlimeBuf = NULL;
LPTR		CImage::m_lpWideSlimeBuf = NULL;
long		CImage::m_lDispBufSize = 0;
long		CImage::m_lFileBufSize = 0;
long 		CImage::m_lSlimeBufSize = 0;
long		CImage::m_lWideSlimeBufSize = 0;

BOOL CImage::Paint(HWND hWnd, HDC hDC, LPRECT lpRepair,
                LPRECT lpInvalid)
{
	LPDISPLAY lpDisplay = GetDisplayPtr(hWnd);
	SelectClipRect(hDC, &lpDisplay->DispRect, NULL);
	if (!PaintSetup(lpDisplay, lpInvalid != NULL))
    	return(FALSE);
	if (!PaintRaster(lpDisplay, hDC, lpRepair, lpInvalid))
    	return(FALSE);
	if (lpInvalid && lpInvalid->right >= lpInvalid->left)
    	ExcludeClipRect(hDC, lpInvalid->left, lpInvalid->top,
                    	lpInvalid->right+1, lpInvalid->bottom+1);
	if (!PaintDisplayHook(hWnd, hDC, lpRepair, lpInvalid))
    	return(FALSE);
	return( YES );
}

BOOL CImage::PaintSetup(LPDISPLAY lpDisplay, BOOL fDoInterrupt)
{
    int DispWidth, FileWidth;

    // head of list should be base frame
    // always use depth of base frame
    if (!(m_lpDispObject = GetDisplayObject()))
        return(FALSE);
    m_fDoInterrupt = fDoInterrupt;

    m_lpDispFrame = ObjGetEditFrame(m_lpDispObject);
    m_lpSlimeObject = GetSlimeObject();

    FrameGetTypeInfo(m_lpDispFrame, &m_TypeInfo);
    if ( !(m_depth = FrameDepth(m_lpDispFrame)) )
        m_depth = 1; // never worry about lineart

    m_sDepth = m_depth;

    // Force gray slime to be color...
    if (m_lpSlimeObject && m_sDepth == 1)
    {
        FRMTYPEINFO Type;
        FrameGetTypeInfo(m_lpDispFrame, &Type);
        if (Type.DataType == FDT_LINEART)
            Type.DataType = FDT_GRAYSCALE;
        FrameSetTypeInfo(&m_TypeInfo, FDT_RGBCOLOR, NULL);
        m_sDepth = 3;
		m_SlimeConvert.Init(Type, m_TypeInfo, 1);
    }

    // get display and file rects for faster access, set dither offsets
    // so the dither pattern holds when scrolling
    m_rDisp = lpDisplay->DispRect;
    m_rFile = lpDisplay->FileRect;
    m_xDiva = lpDisplay->xDiva;
    m_yDiva = lpDisplay->yDiva;

    // get pixel width of display and file
    DispWidth = RectWidth( &m_rDisp );
    FileWidth = RectWidth( &m_rFile );

    if (!PaintAlloc(FileWidth, DispWidth, m_depth, m_sDepth))
        {
        m_lpDispObject = NULL;
        return(FALSE);
        }

    // compute x and y display rates
    m_yrate = FGET(RectHeight( &m_rFile),RectHeight(&m_rDisp));
    m_xrate = FGET(FileWidth, DispWidth);
    m_fDoSample = m_xrate != FUNITY;
    
    return(TRUE);
}

/************************************************************************/
BOOL CImage::PaintRaster(LPDISPLAY lpDisplay, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid)
/************************************************************************/
{
    int sdx, ddx, ddy;
    int PaintWidth;
    int y, yline, ylast, xstart, xend, xtotal;
    int dxCount, increment, LinesPerBlt, dxBytes;
    LFIXED yoffset, xoffset;
    RECT rPaint;
    BLTSESSION BltSession;
    LPTR lpFilePtr, lpFileLine, lpDispPtr, lpWideSlimePtr;
    long lOffset;
    CHookHandler *lpImageHook;
    LPMASKHOOK lpMaskHook;

    // head of list should be base frame
    // always use depth of base frame
    if (!m_lpDispObject)
        return(NO);

    lpImageHook = lpDisplay->lpImageHook;
    lpMaskHook = lpDisplay->lpMaskHook;

    // if no repair rect, we are redisplay the whole image
    if ( !lpRepair )
        rPaint = m_rDisp;
    else
    { // bound repair rect to the display rect
        rPaint = *lpRepair;
        BoundRect( &rPaint, m_rDisp.left, m_rDisp.top, m_rDisp.right, m_rDisp.bottom );
    }

    // get pixel width of display and file
    PaintWidth = RectWidth( &rPaint );

    if ((PaintWidth <= 0) || (RectHeight(&rPaint) <= 0))
        return(NO);

	dxBytes = ( ( ( PaintWidth * m_depth ) + 3 ) / 4 ) * 4;

	 // CMS Lines per blt
    LinesPerBlt = ( Control.CMSEnabled && m_cmsXform != NULL ) ?
		min( RectHeight( &rPaint ), 65500 / dxBytes ) : LINES_PER_BLT;

    // get offsets from display edge to paint rect to adjust xoffset/yoffset
    ddx = rPaint.left - m_rDisp.left;
    ddy = rPaint.top - m_rDisp.top;
    dxCount = ddx + PaintWidth;
    yoffset = (m_yrate*ddy)+FDIV2(m_yrate);
    xoffset = (m_xrate*ddx)+FDIV2(m_xrate);

    // Find x-position in file coordinates where painting begins and ends
    sdx = WHOLE(xoffset);
    xstart = m_rFile.left + sdx;
    xoffset += (m_xrate*(PaintWidth-1));
    xend = m_rFile.left + WHOLE(xoffset);

    if (xend > m_rFile.right)
        xend = m_rFile.right;

    xtotal = xend - xstart + 1;

    lOffset = (long)sdx * (long)m_depth;
    lpFilePtr = m_lpPRFileBuf + lOffset;
    lpFileLine = m_lpPRFileBuf;

    lpWideSlimePtr = m_lpWideSlimeBuf + ((long)sdx * (long)m_sDepth);

    if (m_fDoSample)
        lpDispPtr = m_lpPRDispBuf;
    else
    {
        if (m_sDepth == m_depth)
            lpDispPtr = lpFilePtr;
        else
            lpDispPtr = lpWideSlimePtr;
    }

    ylast = -1;
    increment = LinesPerBlt;

    StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &rPaint, m_TypeInfo,
        LinesPerBlt, m_xDiva, m_yDiva, YES, m_cmsXform, lpKCMSEvalProc,
		  NULL, m_bDoCmsGamma );

    for ( y=rPaint.top; y<=rPaint.bottom; y++ )
    {
        yline = m_rFile.top + WHOLE(yoffset);
        yoffset += m_yrate;
        if ( yline != ylast )
        {
            ylast = yline;
            if (m_depth == m_sDepth)
            {
                ImgGetLine(this, m_lpDispObject, xstart, yline, xtotal,
                                lpFilePtr, lpImageHook);
                if (m_lpSlimeObject)
                    SlimeLine(lpMaskHook, m_lpSlimeObject, xstart, yline, xtotal,
                        lpFilePtr, m_lpPRSlimeBuf, m_depth);

                if (m_fDoSample)
                    FrameSample(m_lpDispFrame, lpFileLine, 0, m_lpPRDispBuf,
                        ddx, dxCount, m_xrate);
            }
            else
            {
                ImgGetLine(this, m_lpDispObject, xstart, yline, xtotal,
                            lpFilePtr, lpImageHook);
                // Expand the gray data to RGB...
                m_SlimeConvert.ConvertData( lpFilePtr, lpWideSlimePtr, y, xtotal );

                SlimeLine(lpMaskHook, m_lpSlimeObject, xstart, yline, xtotal,
                    lpWideSlimePtr, m_lpPRSlimeBuf, m_sDepth);

                if (m_fDoSample)
                    Sample_24(m_lpWideSlimeBuf, 0, m_lpPRDispBuf,
                        ddx, dxCount, m_xrate);
            }
        }

        SuperBlt( &BltSession, lpDispPtr );

		// dont do interrupt if there is no lpInvalid!!
        if ( lpInvalid && m_fDoInterrupt && (--increment <= 0) && y < rPaint.bottom )
        {
            increment = LinesPerBlt;
            if (InterruptableAbort())
            {
                lpInvalid->left = rPaint.left;
                lpInvalid->right = rPaint.right;
                lpInvalid->top = y+1;
                lpInvalid->bottom = rPaint.bottom;
                break;
            }
        }
    }
	    
    SuperBlt( &BltSession, NULL );

    return( YES );
}

BOOL CImage::PaintDisplayHook(HWND hWnd, HDC hDC, LPRECT lpRepair, LPRECT lpInvalid)
{
	LPDISPLAY lpDisplay = GetDisplayPtr(hWnd);
	if (lpDisplay && lpDisplay->lpDisplayHook)
    	lpDisplay->lpDisplayHook->DisplayHookProc(hWnd, hDC, lpRepair);
	return(YES);
}


BOOL CImage::PaintAlloc(int FileWidth, int DispWidth, int depth, int sDepth)
{
    long lDispBufNeeded, lFileBufNeeded, lSlimeBufNeeded, lWideSlimeBufNeeded;

    // allocate display buffer
    lDispBufNeeded = (long)DispWidth * sDepth;
    if (lDispBufNeeded > m_lDispBufSize)
    {
        if (m_lpPRDispBuf)
            FreeUp(m_lpPRDispBuf);
        m_lDispBufSize = lDispBufNeeded;
        if (!(m_lpPRDispBuf = Alloc(m_lDispBufSize)))
        {
            m_lDispBufSize = 0;
            return(FALSE);
        }
    }

    // allocate file buffer
    lFileBufNeeded = (long)FileWidth * depth;
    if (lFileBufNeeded > m_lFileBufSize)
    {
        if (m_lpPRFileBuf)
            FreeUp(m_lpPRFileBuf);
        m_lFileBufSize = lFileBufNeeded;
        if (!(m_lpPRFileBuf = Alloc(m_lFileBufSize)))
        {
            m_lpPRFileBuf = NULL;
            m_lFileBufSize = 0;
            FreeUp(m_lpPRDispBuf);
            m_lpPRDispBuf = NULL;
            m_lDispBufSize = 0;
            return(FALSE);
        }
    }

    // allocate slime buffer
    lSlimeBufNeeded = (long)FileWidth;
    if (lSlimeBufNeeded > m_lSlimeBufSize)
    {
        if (m_lpPRSlimeBuf)
            FreeUp(m_lpPRSlimeBuf);
        m_lSlimeBufSize = lSlimeBufNeeded;
        if (!(m_lpPRSlimeBuf = Alloc(m_lSlimeBufSize)))
        {
            m_lSlimeBufSize = 0;
            FreeUp(m_lpPRFileBuf);
            m_lpPRFileBuf = NULL;
            m_lFileBufSize = 0;
            FreeUp(m_lpPRDispBuf);
            m_lpPRDispBuf = NULL;
            m_lDispBufSize = 0;
            return(FALSE);
        }
    }

    if (depth != sDepth)
    {
        // allocate wide slime buffer
        lWideSlimeBufNeeded = (long)FileWidth * sDepth;
        if (lWideSlimeBufNeeded > m_lWideSlimeBufSize)
        {
            if (m_lpWideSlimeBuf)
                FreeUp(m_lpWideSlimeBuf);
            m_lWideSlimeBufSize = lWideSlimeBufNeeded;
            if (!(m_lpWideSlimeBuf = Alloc(m_lWideSlimeBufSize)))
            {
                m_lWideSlimeBufSize = 0;
                FreeUp(m_lpPRFileBuf);
                m_lpPRFileBuf = NULL;
                m_lFileBufSize = 0;
                FreeUp(m_lpPRDispBuf);
                m_lpPRDispBuf = NULL;
                m_lDispBufSize = 0;
                FreeUp(m_lpPRSlimeBuf);
                m_lpPRSlimeBuf = NULL;
                m_lSlimeBufSize = 0;
                return(FALSE);
            }
        }
    }
    return(TRUE);
}

/************************************************************************/
void CImage::SlimeLine(LPMASKHOOK lpMaskHook, LPOBJECT lpObject,
                        int x, int y, int dx,
                        LPTR lpLine, LPTR lpSlime, int sDepth)
/************************************************************************/
{
    int Slime1, Slime2, Slime3, Slime4;
    int iCount = dx;
    int m1, m2;
    FRMTYPEINFO TypeInfo;

    switch(Control.MaskTint)
    {
        case 0 : // RED
            Slime1 = 255;
            Slime2 = 0;
            Slime3 = 0;
            Slime4 = 0;
        break;

        case 1 : // GREEN
            Slime1 = 0;
            Slime2 = 255;
            Slime3 = 0;
            Slime4 = 0;
        break;

        case 2 : // BLUE
            Slime1 = 0;
            Slime2 = 0;
            Slime3 = 255;
            Slime4 = 0;
        break;

        case 3 : // CYAN
            Slime1 = 0;
            Slime2 = 255;
            Slime3 = 255;
            Slime4 = 0;
        break;

        case 4 : // MAGENTA
            Slime1 = 255;
            Slime2 = 0;
            Slime3 = 255;
            Slime4 = 0;
        break;

        case 5 : // YELLOW
            Slime1 = 255;
            Slime2 = 255;
            Slime3 = 0;
            Slime4 = 0;
        break;
    }

    if (lpObject == GetDisplayObject())
		// if the object we are editing (lpObject) is the same
		// as the object we are displaying then just load the
		// mask for the object we are editing
		// this means we are not in mask edit mode
        MaskLoad( lpObject->lpAlpha, x, y, dx, lpSlime);
    else
		// if we are in mask edit mode and in slime mode,
		// then we must get combined data for the mask
		// to be used for the slime
        ImgGetLine(this, lpObject, x, y, dx, lpSlime);

    ImgGetTypeInfo(this, &TypeInfo);
    
    if (m_depth == 1)
      TypeInfo.DataType = FDT_RGBCOLOR;
    if (lpMaskHook)
        ((CChromaMask*)lpMaskHook)->ChromaProc(dx, TypeInfo, lpLine, lpSlime, ImgGetMask(this) != NULL);
      
    switch(sDepth)
    {
        case 0 :
        case 1 :
            while (iCount-- > 0)
            {
                // slime off pixels
                if (*lpSlime++ <= 127)
                    *lpLine = (*lpLine + Slime1) >> 1;
                ++lpLine;
            }
        break;

        case 3 :
#ifdef C_CODE           
            while (iCount-- > 0)
            {
                m1 = ((*lpSlime++)/2)+128;
                m2 = 255 - m1;
                *lpLine++ = ((*lpLine*m1) + (Slime1*m2)) >> 8;
                *lpLine++ = ((*lpLine*m1) + (Slime2*m2)) >> 8;
                *lpLine++ = ((*lpLine*m1) + (Slime3*m2)) >> 8;
            }
#else // !C_CODE
            __asm {
                push ds
                push es

            s24begin:
                lds si, lpSlime ; Slime Data pointer
                les di, lpLine  ; Scanline Data Pointer
                mov cx, iCount  ; Pixel Count

            s24l1:
                xor ax, ax      ; Clear AX
                lodsb           ; Get Slime value -> AL
                cmp al, 255     ; Check Hi Value
                je  s24zero     ; No Slime here...
                cmp al, 0       ; Check our slime
                jnz s24grad     ; No special, so slime it

            s24full:
                xor ax, ax
                mov al, es:[di]   ; Get Source Red value
                add ax, Slime1    ; Add Red Slime Value
                shr ax, 1         ; Divide by 2
                mov es:[di], al   ; Put back Red value

                xor ax, ax
                mov al, es:[di+1] ; Get Source Green value
                add ax, Slime2    ; Add Green Slime Value
                shr ax, 1         ; Divide by 2
                mov es:[di+1], al ; Put back Green value

                xor ax, ax
                mov al, es:[di+2] ; Get Source Blue value
                add ax, Slime3    ; Add Blue Slime Value
                shr ax, 1         ; Divide by 2
                mov es:[di+2], al ; Put back Blue value

            s24zero:
                add di, 3
                dec cx
                jnz s24l1
                jmp s24done

            s24grad:
                mov dl, al        ; Put Slime value -> DL
                shr dl, 1         ; Divide by 2
                add dl, 128       ; Add 128 for m1
                mov dh, 255       ;
                sub dh, dl        ; m2 = 255 - m1

                xor ax, ax        ; Clear AX
                mov al, es:[di]   ; Get Source Red value   -> AL
                mul dl            ; Multiply Red * m1      -> AX
                mov bx, ax        ; Temp storage
                mov ax, Slime1    ; Get Red Slime Value
                mul dh            ; Multiply Slime *m2     -> AX
                add ax, bx        ; Add values -> AX
                mov es:[di], ah   ; Save new Red Value

                xor ax, ax        ; Clear AX
                mov al, es:[di+1] ; Get Source Green value -> AL
                mul dl            ; Multiply Green * m1    -> AX
                mov bx, ax        ; Temp storage
                mov ax, Slime2    ; Get Green Slime Value
                mul dh            ; Multiply Slime *m2     -> AX
                add ax, bx        ; Add values -> AX
                mov es:[di+1], ah ; Save new Green Value

                xor ax, ax        ; Clear AX
                mov al, es:[di+2] ; Get Source Blue value  -> AL
                mul dl            ; Multiply Blue * m1     -> AX
                mov bx, ax        ; Temp storage
                mov ax, Slime3    ; Get Blue Slime Value
                mul dh            ; Multiply Slime *m2     -> AX
                add ax, bx        ; Add values -> AX
                mov es:[di+2], ah ; Save new Green Value

                add di, 3
                dec cx
                jz  s24done
                jmp s24l1

            s24done:
                pop es
                pop ds
            }
#endif // !C_CODE
        break;

        case 4 :
            Slime1 ^= 0xFF;
            Slime2 ^= 0xFF;
            Slime3 ^= 0xFF;
#ifdef C_CODE
            while (iCount-- > 0)
            {
                m1 = ((*lpSlime++)/2)+128;
                m2 = (255 - m1);
                *lpLine++ = ((*lpLine*m1) + (Slime1*m2)) >> 8;
                *lpLine++ = ((*lpLine*m1) + (Slime2*m2)) >> 8;
                *lpLine++ = ((*lpLine*m1) + (Slime3*m2)) >> 8;
                *lpLine++ = ((*lpLine*m1) + (Slime4*m2)) >> 8;
            }
#else // !C_CODE
            __asm {
                push ds
                push es

            s32begin:
                lds si, lpSlime ; Slime Data pointer
                les di, lpLine  ; Scanline Data Pointer
                mov cx, iCount  ; Pixel Count

            s32l1:
                xor ax, ax      ; Clear AX
                lodsb           ; Get Slime value -> AL
                cmp al, 255     ; Check Hi Value
                je  s32zero     ; No Slime here...
                cmp al, 0       ; Check our slime
                jnz s32grad     ; No special, so slime it

            s32full:
                xor ax, ax
                mov al, es:[di]   ; Get Source Yellow value
                add ax, Slime1    ; Add Yellow Slime Value
                shr ax, 1         ; Divide by 2
                mov es:[di], al   ; Put back Yellow value

                xor ax, ax
                mov al, es:[di+1] ; Get Source Magenta value
                add ax, Slime2    ; Add Magenta Slime Value
                shr ax, 1         ; Divide by 2
                mov es:[di+1], al ; Put back Magenta value

                xor ax, ax
                mov al, es:[di+2] ; Get Source Cyan value
                add ax, Slime3    ; Add Cyan Slime Value
                shr ax, 1         ; Divide by 2
                mov es:[di+2], al ; Put back Cyan value

                xor ax, ax
                mov al, es:[di+3] ; Get Source Black value
                add ax, Slime4    ; Add Black Slime Value
                shr ax, 1         ; Divide by 2
                mov es:[di+3], al ; Put back Black value

            s32zero:
                add di, 4
                dec cx
                jnz s32l1
                jmp s32done

            s32grad:
                mov dl, al        ; Put Slime value -> DL
                shr dl, 1         ; Divide by 2
                add dl, 128       ; Add 128 for m1
                mov dh, 255       ;
                sub dh, dl        ; m2 = 255 - m1

                xor ax, ax        ; Clear AX
                mov al, es:[di]   ; Get Source Yellow value   -> AL
                mul dl            ; Multiply Yellow * m1      -> AX
                mov bx, ax        ; Temp storage
                mov ax, Slime1    ; Get Yellow Slime Value
                mul dh            ; Multiply Slime *m2     -> AX
                add ax, bx        ; Add values -> AX
                mov es:[di], ah   ; Save new Yellow Value

                xor ax, ax        ; Clear AX
                mov al, es:[di+1] ; Get Source Magenta value -> AL
                mul dl            ; Multiply Magenta * m1    -> AX
                mov bx, ax        ; Temp storage
                mov ax, Slime2    ; Get Magenta Slime Value
                mul dh            ; Multiply Slime *m2     -> AX
                add ax, bx        ; Add values -> AX
                mov es:[di+1], ah ; Save new Magenta Value

                xor ax, ax        ; Clear AX
                mov al, es:[di+2] ; Get Source Cyan value  -> AL
                mul dl            ; Multiply Cyan * m1     -> AX
                mov bx, ax        ; Temp storage
                mov ax, Slime3    ; Get Cyan Slime Value
                mul dh            ; Multiply Slime *m2     -> AX
                add ax, bx        ; Add values -> AX
                mov es:[di+2], ah ; Save new Cyan Value

                xor ax, ax        ; Clear AX
                mov al, es:[di+3] ; Get Source Black value  -> AL
                mul dl            ; Multiply Black * m1     -> AX
                mov bx, ax        ; Temp storage
                mov ax, Slime4    ; Get Black Slime Value
                mul dh            ; Multiply Slime *m2     -> AX
                add ax, bx        ; Add values -> AX
                mov es:[di+3], ah ; Save new Black Value

                add di, 4
                dec cx
                jz  s32done
                jmp s32l1

            s32done:
                pop es
                pop ds
            }
#endif // !C_CODE
        break;
    }
}

BOOL CImage::InterruptableAbort()
{ // Only interrupt on things that the user initiates
	MSG msg;

	if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE|PM_NOYIELD ) )
    	return(TRUE);
	return(FALSE);
}



