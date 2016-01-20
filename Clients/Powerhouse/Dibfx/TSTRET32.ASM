page    ,132
;----------------------------Module-Header------------------------------;
; Module Name: TSTRET32.ASM
;
; StretchBLT for DIBs with Transparency
;
; NOTES:
;       - Does not handle mirroring in x or y.
;       - Does not handle pixel translation
;       - will not work "in place"
;
;       - This is the 32 bit version (32 bit code seg that is...)
;
;  AUTHOR: ToddLa (Todd Laney) Microsoft
;
;  JonBl   - Modified for 8 bits with transparency
;
;  Copyright (c) 1994 Microsoft Corporation
;-----------------------------------------------------------------------;
?PLM=1
?WIN=0
	.xlist
        include cmacro32.inc
;       include cmacros.inc
        include windows.inc
	.list

sBegin  Data
sEnd    Data

ifndef SEGNAME
    SEGNAME equ <_TEXT32>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

sBegin  CodeSeg
        .386
        assumes cs,CodeSeg
        assumes ds,nothing
        assumes es,nothing

;--------------------------------------------------------------------------;
;
;   DDA type constants returned from stretch_init_dda
;
;--------------------------------------------------------------------------;

STRETCH_1to1    equ     (0*4)       ;   dst = src
STRETCH_1toN    equ     (1*4)       ;   dst > src
STRETCH_Nto1    equ     (2*4)       ;   dst < src
STRETCH_VOID    equ     (3*4)       ;   not used.

;--------------------------------------------------------------------------;
;
; x_stretch_functions
;
;     functions for stretching a single scanline, this table is indexed
;     by the x dda type (see above) and the bit depth
;
;--------------------------------------------------------------------------;

x_stretch_functions label dword             ;function table for x_stretch

x_stretch_8_functions label dword           ;function table for x_stretch
        dd      x_stretch_1to1              ; STRETCH_1to1
        dd      x_stretch_8_1toN            ; STRETCH_1toN
        dd      x_stretch_8_Nto1            ; STRETCH_Nto1

;--------------------------------------------------------------------------;
;
; y_stretch_functions
;
;     functions for stretching in the y direction, indexed by the y dda type
;
;--------------------------------------------------------------------------;

y_stretch_functions label dword             ;function table for y_stretch
        dd      y_stretch_1toN              ; STRETCH_1to1
        dd      y_stretch_1toN              ; STRETCH_1toN
        dd      y_stretch_Nto1              ; STRETCH_Nto1

;--------------------------------------------------------------------------;
;
;   DoDibTransparentStretch
;
;   Entry:
;       Stack based parameters as described below.
;
;   Returns:
;       none
;
;   Registers Preserved:
;       DS,ES,ESI,EDI,EBP
;
;--------------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,nothing        

cProc   DoDibTransparentStretch,<FAR,PUBLIC,PASCAL>,<>
        parmD   biDst                   ;--> BITMAPINFO of dest
        parmD   lpDst                   ;--> to destination bits
        parmW   DstX                    ;Destination origin - x coordinate
        parmW   DstY                    ;Destination origin - y coordinate
        parmW   DstXE                   ;x extent of the BLT
        parmW   DstYE                   ;y extent of the BLT
        parmD   biSrc                   ;--> BITMAPINFO of source
        parmD   lpSrc                   ;--> to source bits
        parmW   SrcX                    ;Source origin - x coordinate
        parmW   SrcY                    ;Source origin - y coordinate
        parmW   SrcXE                   ;x extent of the BLT
        parmW   SrcYE                   ;y extent of the BLT
	parmB   iTransparent            ;transparent index color

        localD  x_stretch               ;X stretch function
        localD  y_stretch               ;Y stretch function

        localD  x_stretch_dda           ;X stretch DDA
        localD  x_stretch_dda_fract     ;X stretch DDA fract

        localD  y_stretch_dda           ;Y stretch DDA
        localD  y_stretch_dda_fract     ;Y stretch DDA fract

        localD  ScanCount               ;number of scans left to do
        localD  Yerr                    ;Y dda error

        localD  SrcWidth                ;width of source dib in bytes
        localD  DstWidth                ;width of dest in bytes

        localD  SrcBytes                ;width of source blt in bytes
        localD  DstBytes                ;width of dest blt in bytes

        localD  SrcInc
        localD  DstInc

        localD  lDstXE                  ;x extent of the BLT
        localD  lDstYE                  ;y extent of the BLT
        localD  lSrcXE                  ;x extent of the BLT
        localD  lSrcYE                  ;y extent of the BLT
cBegin
        cld

        push    esi
        push    edi
        push    ds

	call	stretch_init	; init all the frame variables
        jc      short StretchDIBExit

        call    y_stretch       ; do it!

StretchDIBExit:
        pop     ds
        pop     edi
        pop     esi
cEnd

;--------------------------------------------------------------------------;
;
;   stretch_init_dda
;
;       initialize the parameters of a DDA from ax to dx.
;
;   Entry:
;       eax         - source coord
;       edx         - dest coord
;   Returns:
;       eax         - STRETCH_1to1
;                     STRETCH_1toN
;                     STRETCH_Nto1
;
;       edx         - src / dst
;       ebx         - src / dst fraction
;
;--------------------------------------------------------------------------;

stretch_init_dda proc near

        cmp     eax,edx
        je      short stretch_init_dda_1to1
        ja      short stretch_init_dda_Nto1
        errn$   stretch_init_dda_1toN

stretch_init_dda_1toN:
        mov     ebx,edx             ; ebx = dest
        mov     edx,eax             ; edx = src
        xor     eax,eax             ; edx:eax = src<<32
        div     ebx                 ; eax = (src<<32)/dst

        mov     ebx,eax
        xor     edx,edx
        mov     eax,STRETCH_1toN
        ret

stretch_init_dda_Nto1:
        mov     ebx,edx             ; ebx = dst
        xor     edx,edx             ; edx:eax = src
        mov     eax,eax
        div     ebx                 ; eax = src/dst edx = rem
        push    eax
        xor     eax,eax             ; edx:eax = rem<<32
        div     ebx                 ; eax = rem<<32/dst
        mov     ebx,eax
        pop     edx
        mov     eax,STRETCH_Nto1
        ret

stretch_init_dda_1to1:
        mov     edx, 1
        xor     ebx, ebx
        mov     eax, STRETCH_1to1
        ret

stretch_init_dda endp

;--------------------------------------------------------------------------;
;
;   stretch_init
;
;   init local frame vars for StretchDIB
;
;   ENTRY:
;       ss:bp   --> stretchdib frame
;
;--------------------------------------------------------------------------;

stretch_init_error:
        stc
        ret

stretch_init proc near

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;   expand word params to dwords
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
        movzx   eax,DstXE
        movzx   ebx,DstYE
        movzx   ecx,SrcXE
        movzx   edx,SrcYE

        mov     lDstXE,eax
        mov     lDstYE,ebx
        mov     lSrcXE,ecx
        mov     lSrcYE,edx

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
; Make sure they didn't give us an extent of zero anywhere
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
        or      eax,eax
        jz      short stretch_init_error

        or      ebx,ebx
        jz      short stretch_init_error

        or      ecx,ecx
        jz      short stretch_init_error

        or      edx,edx
        jz      short stretch_init_error

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;   make sure the bit depth of the source and dest match and are valid
;   we only handle 8 bit depths.
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
        xor     edi,edi                     ; be sure the high words are clear.
	xor	esi,esi
        xor     ecx,ecx

        lds     si, biSrc
        les     di, biDst

        mov     cx, [esi].biBitCount        ; get the bit depth
        cmp     cx, es:[edi].biBitCount     ; make sure they are the same.
        jne     short stretch_init_error

        cmp     ecx,8
        jne     short stretch_init_error
        errn$   stretch_init_bit_depth_ok

stretch_init_bit_depth_ok:

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;
;  Set up the initial source pointer
;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
        mov     eax,[esi].biWidth
        shl     eax,3
        add     eax,31
        and     eax,not 31
        shr     eax,3
        mov     SrcWidth,eax
        mov     SrcInc,eax

        lds     si,lpSrc

        movzx   edx,SrcY
        mul     edx
	add	esi,eax

        movzx   eax,SrcX
        add     esi,eax

        mov     eax,lSrcXE
        mov     SrcBytes,eax
        sub     SrcInc,eax

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;
;  Set up the initial dest pointer
;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
        mov     eax,es:[edi].biWidth
        shl     eax,3
        add     eax,31
        and     eax,not 31
        shr     eax,3
        mov     DstWidth,eax
        mov     DstInc,eax

        cmp     es:[edi].biHeight,0            ; init a upside down DIB
        jge     short @f
        movsx   ebx,DstY
        add     ebx,es:[edi].biHeight
        not     ebx
        mov     DstY,bx
        neg     DstWidth
        neg     DstInc
@@:
        les     di,lpDst

        movsx   edx,DstY
        mul     edx
	add	edi,eax

        movsx   eax,DstX
        add     edi,eax

        mov     eax,lDstXE
        mov     DstBytes,eax
        sub     DstInc,eax

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;
;  Setup y_stretch function pointer
;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
        mov     eax,lSrcYE
        mov     edx,lDstYE
        call    stretch_init_dda
        mov     y_stretch_dda,edx
        mov     y_stretch_dda_fract,ebx

        mov     ebx,eax
        mov     edx,y_stretch_functions[ebx]
        mov     y_stretch,edx

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;
;  Setup x_stretch function pointer
;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
        push    eax                     ; Save Y stretch type
        mov     eax,lSrcXE
        mov     edx,lDstXE

        call    stretch_init_dda
        mov     x_stretch_dda,edx
        mov     x_stretch_dda_fract,ebx

        mov     ebx,eax                 ; get x stretch
        mov     edx,x_stretch_functions[ebx]
        mov     x_stretch,edx

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
;
;  check for a special case stretch routine
;
;  Hey, there aren't any!
;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
        pop     edx                     ; get y stretch back

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ;

stretch_init_exit:
        clc
        ret

stretch_init endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;--------------------------------------------------------------------------;
;
;   y_stretch_1toN
;
;   do the entire stretch, y stretching  (DstYE > SrcYE)
;
;   Entry:
;       ds:esi  --> (SrcX, SrcY) in source
;       es:edi  --> (DstX, DstY) in destination
;       ss:bp   --> stack frame of StretchDIB
;   Returns:
;       none. (stretch is done.)
;--------------------------------------------------------------------------;
y_stretch_1toN proc near
        mov     ecx,lDstYE
        mov     ScanCount,ecx

        mov     edx,lDstYE          ; dda error
        dec     edx
        mov     Yerr,edx

y_stretch_1toN_loop:
        push    esi
        push    edi
        call    x_stretch
        pop     edi
        pop     esi

        add     edi, DstWidth       ; next dest scan.

        mov     eax,lSrcYE
        sub     Yerr,eax
        jnc     short y_stretch_1toN_next

        mov     eax,lDstYE
        add     Yerr,eax

        add     esi, SrcWidth       ; next source scan.

y_stretch_1toN_next:
        dec     ScanCount
        jnz     short y_stretch_1toN_loop

        ret

y_stretch_1toN endp

;--------------------------------------------------------------------------;
;
;   y_stretch_Nto1
;
;   do the entire stretch, y shrinking   (DstYE < SrcYE)
;
;   Entry:
;       ds:esi  --> (SrcX, SrcY) in source
;       es:edi  --> (DstX, DstY) in destination
;       ss:bp   --> stack frame of StretchDIB
;   Returns:
;       none. (stretch is done.)
;--------------------------------------------------------------------------;
y_stretch_Nto1 proc near
        mov     ecx,lDstYE
        mov     ScanCount,ecx

        mov     edx,lSrcYE          ; dda error
        dec     edx
        mov     Yerr,edx

y_stretch_Nto1_loop:
        push    esi
        push    edi
        call    x_stretch
        pop     edi
        pop     esi

        add     edi, DstWidth       ; next dest scan.

        mov     eax, lDstYE
@@:     add     esi, SrcWidth       ; next source scan.
        sub     Yerr, eax
        jnc     short @b

        mov     eax,lSrcYE
        add     Yerr,eax

y_stretch_Nto1_next:
        dec     ScanCount
        jnz     short y_stretch_Nto1_loop

        ret

y_stretch_Nto1 endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;--------------------------------------------------------------------------;
;
;   x_stretch_1to1
;
;   handle a stretch of a scanline  (DstXE == SrcXE)
;
;   Entry:
;       ds:esi  --> begining of scan
;       es:edi  --> destination scan
;       ss:bp   --> stack frame of StretchDIB
;   Returns:
;       ds:esi  --> at end of scan
;       es:edi  --> at end of scan
;   
;--------------------------------------------------------------------------;
align 4
x_stretch_1to1 proc near

        mov     ah, iTransparent
        mov     ecx,DstBytes        ; number of bytes to copy

x_stretch_1to1_loop:
        mov     al, byte ptr ds:[esi]
        cmp     al, ah
        jz      short transparent_pixel_1to1
        mov     byte ptr es:[edi], al
transparent_pixel_1to1:
        inc     esi
        inc     edi
        dec     ecx
        jnz     short x_stretch_1to1_loop
        ret

x_stretch_1to1 endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;8 BIT;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;--------------------------------------------------------------------------;
;
;   x_stretch_8_1toN
;
;   handle a stretch of a scanline  (DstXE > SrcXE)
;
;   Entry:
;       ds:esi  --> begining of scan
;       es:edi  --> destination scan
;       ss:bp   --> stack frame of StretchDIB
;   Returns:
;       ds:esi  --> at end of scan
;       es:edi  --> at end of scan
;   
;--------------------------------------------------------------------------;
align 4
x_stretch_8_1toN proc near

        mov     ah, iTransparent
        mov     ebx,x_stretch_dda_fract
        xor     edx,edx
        mov     ecx,lDstXE          ; # bytes to store

align 4
x_stretch_8_1toN_loop:
        mov     al,byte ptr ds:[esi]
        add     edx,ebx
        adc     esi,0
        cmp     al, ah
        jz      short transparent_pixel_1toN
        mov     byte ptr es:[edi], al
transparent_pixel_1toN:
        inc     edi
        dec     ecx
        jnz     short x_stretch_8_1toN_loop
        ret

x_stretch_8_1toN endp

;--------------------------------------------------------------------------;
;
;   x_stretch_8_Nto1
;
;   handle a shrink of a scanline  (DstXE < SrcXE)
;
;   Entry:
;       ds:esi  --> begining of scan
;       es:edi  --> destination scan
;       ecx     -   destination pixels to write
;       edx     -   source pixels to copy
;       ss:bp   --> stack frame of StretchDIB
;   Returns:
;       ds:esi  --> at end of scan
;       es:edi  --> at end of scan
;   
;--------------------------------------------------------------------------;
align 4
x_stretch_8_Nto1 proc near

        mov     ah, iTransparent
        mov     ebx,x_stretch_dda_fract
        xor     edx,edx

        mov     ecx,lDstXE                      ; # bytes to store
        push    ebp
        mov     ebp,x_stretch_dda

align 4
x_stretch_8_Nto1_loop:
        mov     al,byte ptr ds:[esi]
        add     edx,ebx
        adc     esi,ebp
        cmp     al, ah
        jz      short transparent_pixel_Nto1
        mov     byte ptr es:[edi],al
transparent_pixel_Nto1:
        inc     edi
        dec     ecx
        jnz     short x_stretch_8_Nto1_loop

        pop     ebp
        ret
x_stretch_8_Nto1 endp

sEnd    CodeSeg

end
