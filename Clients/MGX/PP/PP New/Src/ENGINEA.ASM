
;	Static Name Aliases
;
	TITLE   enginea.asm
	NAME    enginea

	.386p
ENGINEA_TEXT	SEGMENT  USE16 WORD PUBLIC 'CODE'
ENGINEA_TEXT	ENDS
_DATA	SEGMENT  USE16 WORD PUBLIC 'DATA'
lSeed	DD	0deadbabaH
_DATA	ENDS
CONST	SEGMENT  USE16 WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  USE16 WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME  CS: ENGINEA_TEXT, DS: DGROUP, SS: DGROUP
EXTRN	__chkstk:FAR
EXTRN	_clr:FAR
EXTRN	__aFldiv:FAR
EXTRN	_RGBtoHSL:FAR
EXTRN	_HSLtoRGB:FAR

_BSS      SEGMENT
COMM NEAR	PLOCALHEAP:	BYTE:	 2
_BSS      ENDS
ENGINEA_TEXT      SEGMENT
	ASSUME	CS: ENGINEA_TEXT

IFNDEF ASM_CODE
;
; void ScatterLine(lpSrc, lpDst, lpErrors, dx, iPressure)
;	LPTR lpSrc;
;	LPTR lpDst;
;	LPINT lpErrors;
;	int dx;
;	int iPressure

	PUBLIC	_ScatterLine
_ScatterLine	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	mov	ax,14
	call	FAR PTR __chkstk
	push	si
	push	di

;	lpSrc = 6
;	lpDst = 10
;	lpErrors = 14
;	dx = 18
;	iPressure = 20
;	lSeed = -14 lSeed+2 = -12
;	error = -10
;	iCount = -4
;	lpError = -8

	les	di,DWORD PTR [bp+10]	; es:[di] - lpDst
	mov	ax,WORD PTR lSeed
	mov	WORD PTR [bp-14],ax	; lSeed
	mov	ax,WORD PTR lSeed+2
	mov	WORD PTR [bp-12],ax	; lSeed+2
	push	ds

	lds	si,DWORD PTR [bp+14]	; ds:[si] - lpError = lpErrors
	mov	cx,WORD PTR [bp+18]	;dx

SM_2:
	mov	bx,WORD PTR [bp+20]	;error = iPressure
	mov	dx,WORD PTR ds:[si]
	add	bx,dx			;error += *lpError
	cmp	bx,127			; if (error > 127)
	jle	SM_4			; error <= 127
	mov	dx,ds			; save ds:[si] = lpError
	push	si
	lds	si,DWORD PTR [bp+6]	;lpSrc
	lodsb				; *lpSrc++
	mov	WORD PTR [bp+6],si
	pop	si
	mov	ds,dx
	stosb				; *lpDst++ = *lpSrc++
	sub	bx,255			;error -= 255
	jmp	SHORT SM_5

SM_4:
	sub	al,al
	stosb				; *lpDst++ = 0;
	inc	WORD PTR [bp+6]		; lpSrc++
SM_5:
	mov	dx,WORD PTR [bp-12]
	test	dx,2
	je	SM_6

	mov	ax,WORD PTR [bp-14]
	add	ax,ax			; lSeed += lSeed
	adc	dx,dx
	xor	ax,39			; lSeed ^= BITMASK
	mov	WORD PTR [bp-14],ax
	mov	WORD PTR [bp-12],dx
	mov	WORD PTR ds:[si],0	; *lpError = 0
	add	si,2			; ++lpError
	add	WORD PTR ds:[si],bx	; *lpError += error
	loop	SM_2
	jmp	SM_10
SM_6:
	mov	ax,WORD PTR [bp-14]
	add	WORD PTR [bp-14],ax
	adc	WORD PTR [bp-12],dx

	mov	WORD PTR ds:[si],bx
	add	si,2		
	dec	cx
	jz	SM_10
	jmp	SM_2
;	loop	SM_2

SM_10:
	pop	ds
	mov	ax,WORD PTR[bp-14]
	mov	WORD PTR lSeed,ax
	mov	ax,WORD PTR[bp-12]
	mov	WORD PTR lSeed+2,ax

	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_ScatterLine	ENDP
ENDIF

;/************************************************************************/
;void TexturizeData24(lpDst, lpMsk, iCount)
;/************************************************************************/
;LPRGB	lpDst;
;LPTR	lpMsk;
;int	iCount;
	PUBLIC	_TexturizeData24
_TexturizeData24	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	mov	ax,8
	call	FAR PTR __chkstk
	push	si
	push	di
;
;	lpDst = 6
;	lpMsk = 10
;	iCount = 14
;	wMsk = -4
;	hsl = -8
;
	mov	cx,WORD PTR[bp+14]	;iCount
	or	cx,cx
	jle	TD24_END
	les	di,DWORD PTR [bp+6]	;lpDst
	lds	si,DWORD PTR [bp+10]	;lpMsk
	cld
;	jmp	TD24_3
TD24_1:
;	push	es			; save es
;	push	ss			; &hsl
;	lea	bx,WORD PTR [bp-8]	;hsl
;	push	bx
;	mov	al,BYTE PTR es:[di+2]	; blue
;	push	ax
;	mov	al,BYTE PTR es:[di+1]	; green
;	push	ax
;	mov	al,BYTE PTR es:[di]	; red
;	push	ax
;	call	FAR PTR _RGBtoHSL
;	add	sp,10			; 5 pops
;	pop	es			; restore es

	mov	bl,BYTE PTR ds:[si]
	inc	si
	sub	bh,bh
	inc	bx			;wMsk

TD24_2:
;	mov	dl,BYTE PTR [bp-6]	; hsl.lum

	sub	ah,ah
	mov	al,BYTE PTR es:[di]	; red
	mul	bx			;wMsk
	mov	al,ah			; >> 8
	stosb

	sub	ah,ah
	mov	al,BYTE PTR es:[di]	; green
	mul	bx			;wMsk
	mov	al,ah			; >> 8
	stosb

	sub	ah,ah
	mov	al,BYTE PTR es:[di]	; blue
	mul	bx			;wMsk
	mov	al,ah			; >> 8
	stosb

;	sub	ah,ah

;	push	es			; lpDst
;	push	di
;	push	ax			; hsl.lum
;	mov	al,BYTE PTR [bp-7]
;	push	ax			; hsl.sat
;	mov	al,BYTE PTR [bp-8]
;	push	ax			; hsl.hue
;	call	FAR PTR _HSLtoRGB
;	add	sp,8			; 4 pops
;	pop	es
;	add	di,3

TD24_3:
	loop	TD24_1
;	dec	WORD PTR [bp+14]	;iCount
;	jns	TD24_1

TD24_END:
	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	

_TexturizeData24	ENDP

;/************************************************************************/
;void TexturizeData8(lpDst, lpMsk, iCount)
;/************************************************************************/
;LPTR	lpDst;
;LPTR	lpMsk;
;int	iCount;
	PUBLIC	_TexturizeData8
_TexturizeData8	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	mov	ax,8
	call	FAR PTR __chkstk
	push	si
	push	di
;
;	lpDst = 6
;	lpMsk = 10
;	iCount = 14
;	wMsk = -4
;	hsl = -8
;
	mov	cx,WORD PTR[bp+14]	;iCount
	or	cx,cx
	jle	TD8_END
	les	di,DWORD PTR [bp+6]	;lpDst
	lds	si,DWORD PTR [bp+10]	;lpMsk
	cld
TD8_1:
	lodsb
	sub	ah,ah
	inc	ax			;wMsk

TD8_2:
	mov	bl,BYTE PTR es:[di]	; gray0
	sub	bh,bh
	mul	bx			;wMsk
	mov	al,ah			; >> 8
	stosb

TD8_3:
	loop	TD8_1

TD8_END:
	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	

_TexturizeData8		ENDP

;/************************************************************************/
;static void pair2byte (lpSum, lpNum, lp, iCount)
;/************************************************************************/
;LPLONG lpSum, lpNum;
;LPTR lp;
;int iCount;
	PUBLIC	_pair2byte
_pair2byte	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

	push	si
	push	di
	pushf

; 	lpSum = 6
; 	lpNum = 10
; 	lp = 14
; 	iCount = 18

	les	di,DWORD PTR [bp+14]	; lp
	lds	si,DWORD PTR [bp+10]   ; lpNum
	cld
	jmp	P2B_LOOP
P2B_1:
; if (*lpNum)
	mov	ax,WORD PTR ds:[si]	; lpNum
	mov	dx,WORD PTR ds:[si+2]
	add	si,4
	or	ax,dx
	je	P2B_10
	mov	WORD PTR [bp+10],si	; lpNum
; l = (*lpSum++) / (*lpNum++);
	push	dx			; lpNum
	push	ax
	lds	si,DWORD PTR [bp+6]	; lpSum
	push	WORD PTR ds:[si+2]
	push	WORD PTR ds:[si]
	add	si,4
	mov	WORD PTR [bp+6],si	; lpSum
	call	FAR PTR __aFldiv
; *lp++ = (l > 255) ? 255 : (BYTE)l;
	lds	si,DWORD PTR [bp+10]	; lpNum
	cmp	dx,0
	jne	P2B_2
	cmp	ax,255
	jbe	P2B_12
P2B_2:
	mov	al,255	;00ffH
P2B_3:
	jmp	P2B_12

P2B_10:
	add	WORD PTR [bp+6],4
; l = 128;
	mov	al,128
P2B_12:
	stosb
P2B_LOOP:
	dec	WORD PTR [bp+18]
	jns	P2B_1

P2B_END:
	popf
	pop	di
	pop	si

	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
_pair2byte	ENDP

	PUBLIC	_ProcessNormal24
_ProcessNormal24	PROC FAR	;  COMDAT
; Line 1226
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	sub	sp,10	;000aH
	push	di
	push	si
;	sm = -4
;	dm = -6
;	s = -8
;	d = -10
;	iCount = 18
;	lpMsk = 14
;	lpSrc = 10
;	lpDst = 6

	cmp	WORD PTR [bp+18],0
	jne	PD_START
	jmp	PD_END

PD_START:
	lds	si,DWORD PTR [bp+10]	;lpSrc
	les	di,DWORD PTR [bp+6]	;lpDst
	lfs	bx,DWORD PTR [bp+14]	;lpMsk

PD_LOOP:
	mov	al,fs:[bx]		;get mask value
	inc	bx			;bump mask pointer
	or	al,al			;any mask there
	jne	PD_HAVEMASK		;some mask bits
	add	si,3			;no mask, bump source
	add	di,3			;no mask, bump dest
	dec	WORD PTR [bp+18]	;iCount
	jnz	PD_LOOP			;go back to top of loop
	jmp	PD_END			;all done

PD_HAVEMASK:
	cmp	al,255			; all source?
	je	PD_COPYSOURCE		; yup
	sub	ah,ah			; get rid of hi-bytes
	cmp	al,127			; see if we need to bump
	jbe	PD_NOINC		; no need to bump
	inc	ax
PD_NOINC:
	push	bx			; save offset to mask
	mov	cx,ax			; put mask value in cx

	; process red channel
	lodsb				; get red
	sub	ah,ah			; clear hi-byte
	mul	cx			; CX = s * sm
	mov	bx,ax			; BX = (s * sm)
	mov	dx,256			; flip mask
	sub	dx,cx			; DX = 256-sm
	mov	WORD PTR [bp-6],dx	; save dm for other channels
	mov	al,BYTE PTR es:[di]	; AL = d
	sub	ah,ah			; cast to WORD
	mul	dx			; AX = d * (256-sm)
	add	ax,bx			; BX = (s * sm) + (d * (256-sm))
	add	ax,128			; add 128 for rounding
	mov	BYTE PTR es:[di],ah	; get HIBYTE
	inc	di

	; process green channel
	lodsb				; get green
	sub	ah,ah			; clear hi-byte
	mul	cx			; CX = s * sm
	mov	bx,ax			; BX = (s * sm)
	mov	al,BYTE PTR es:[di]	; AL = d
	sub	ah,ah			; cast to WORD
	mul	WORD PTR [bp-6]		; AX = d * dm
	add	ax,bx			; BX = (s * sm) + (d * (256-sm))
	add	ax,128			; add 128 for rounding
	mov	BYTE PTR es:[di],ah	; get HIBYTE
	inc	di
	
	; process blue channel
	lodsb				; get blue
	sub	ah,ah			; clear hi-byte
	mul	cx			; CX = s * sm
	mov	bx,ax			; BX = (s * sm)
	mov	al,BYTE PTR es:[di]	; AL = d
	sub	ah,ah			; cast to WORD
	mul	WORD PTR [bp-6]		; AX = d * dm
	add	ax,bx			; BX = (s * sm) + (d * (256-sm))
	add	ax,128			; add 128 for rounding
	mov	BYTE PTR es:[di],ah	; get HIBYTE
	inc	di

	pop	bx			; restore offset to mask

	jmp	PD_NEXT	

PD_COPYSOURCE:
	movsw
	movsb

PD_NEXT:		
	dec	WORD PTR [bp+18]	;iCount
	jnz	PD_LOOP			;go back to top of loop

PD_END:
	pop	si
	pop	di
	lea	sp,WORD PTR [bp-2]
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	
_ProcessNormal24	ENDP

	PUBLIC	_ProcessNormal8
_ProcessNormal8	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	sub	sp,10	;000aH
	push	di
	push	si
;	sm = -4
;	dm = -6
;	s = -8
;	d = -10
;	iCount = 18
;	lpMsk = 14
;	lpSrc = 10
;	lpDst = 6

	cmp	WORD PTR [bp+18],0
	jne	PD8_START
	jmp	PD8_END

PD8_START:
	lds	si,DWORD PTR [bp+10]	;lpSrc
	les	di,DWORD PTR [bp+6]	;lpDst
	lfs	bx,DWORD PTR [bp+14]	;lpMsk

PD8_LOOP:
	mov	al,fs:[bx]		;get mask value
	inc	bx			;bump mask pointer
	or	al,al			;any mask there
	jne	PD8_HAVEMASK		;some mask bits
	inc	si			;no mask, bump source
	inc	di			;no mask, bump dest
	dec	WORD PTR [bp+18]	;iCount
	jnz	PD8_LOOP			;go back to top of loop
	jmp	PD8_END			;all done

PD8_HAVEMASK:
	cmp	al,255			; all source?
	je	PD8_COPYSOURCE		; yup
	sub	ah,ah			; get rid of hi-bytes
	cmp	al,127			; see if we need to bump
	jbe	PD8_NOINC		; no need to bump
	inc	ax
PD8_NOINC:
	push	bx			; save offset to mask
	mov	cx,ax			; put mask value in cx

	; process gray channel
	lodsb				; get red
	sub	ah,ah			; clear hi-byte
	mul	cx			; CX = s * sm
	mov	bx,ax			; BX = (s * sm)
	mov	dx,256			; flip mask
	sub	dx,cx			; DX = 256-sm
	mov	al,BYTE PTR es:[di]	; AL = d
	sub	ah,ah			; cast to WORD
	mul	dx			; AX = d * (256-sm)
	add	ax,bx			; BX = (s * sm) + (d * (256-sm))
	add	ax,128			; add 128 for rounding
	mov	BYTE PTR es:[di],ah	; get HIBYTE
	inc	di

	pop	bx			; restore offset to mask

	jmp	PD8_NEXT	

PD8_COPYSOURCE:
	movsb

PD8_NEXT:		
	dec	WORD PTR [bp+18]	;iCount
	jnz	PD8_LOOP			;go back to top of loop

PD8_END:
	pop	si
	pop	di
	lea	sp,WORD PTR [bp-2]
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	
_ProcessNormal8	ENDP

	PUBLIC	_ProcessNormal8P
_ProcessNormal8P	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	sub	sp,10	;000aH
	push	di
	push	si
;	sm = -4
;	dm = -6
;	s = -8
;	d = -10
;	iCount = 18
;	lpMsk = 14
;	lpSrc = 10
;	lpDst = 6

	cmp	WORD PTR [bp+18],0
	jne	PD8P_START
	jmp	PD8P_END

PD8P_START:
	lds	si,DWORD PTR [bp+10]	;lpSrc
	les	di,DWORD PTR [bp+6]	;lpDst
	lfs	bx,DWORD PTR [bp+14]	;lpMsk

PD8P_LOOP:
	mov	al,fs:[bx]		;get mask value
	inc	bx			;bump mask pointer
	or	al,al			;any mask there
	jne	PD8P_HAVEMASK		;some mask bits
	inc	si			;no mask, bump source
	inc	di			;no mask, bump dest
	dec	WORD PTR [bp+18]	;iCount
	jnz	PD8P_LOOP			;go back to top of loop
	jmp	PD8P_END			;all done

PD8P_HAVEMASK:
	movsb

PD8P_NEXT:		
	dec	WORD PTR [bp+18]	;iCount
	jnz	PD8P_LOOP			;go back to top of loop

PD8P_END:
	pop	si
	pop	di
	lea	sp,WORD PTR [bp-2]
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	
_ProcessNormal8P	ENDP

	PUBLIC	_MapLut
_MapLut	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	sub	sp,4
	push	si
	push	di

;	wCount = 18
;	lpLut = 14
;	lpDst = 10
;	lpSrc = 6
	mov	cx,WORD PTR [bp+18]	; wCount
	cmp	cx,0
	je	ML_DONE			; no count
	lds	si,DWORD PTR [bp+6]	; lpSrc
	les	di,DWORD PTR [bp+10]	; lpDst
	lfs	bx,DWORD PTR [bp+14]	; lpLut
	mov	dx,bx
	sub	ah,ah
ML_LOOP:
	lodsb				; get source
	mov	bx,dx			; get offset to lpLut
	add	bx,ax			; add value to offset
	mov	al,fs:[bx]		; lookup in lpLut
	stosb				; store in dest
	dec	cx			; decrement count
	jnz	ML_LOOP			; not zero, keep going
ML_DONE:
	pop	di
	pop	si
	lea	sp,WORD PTR [bp-2]
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	

_MapLut	ENDP

	PUBLIC	_MapLut32
_MapLut32	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	sub	sp,4
	push	si
	push	di

;	iCount = 18 number of pixels, not number of bytes
;	lpLut = 14
;	lpDst = 10
;	lpSrc = 6
	mov	cx,WORD PTR [bp+18]	; iCount
	cmp	cx,0
	je	MLI_DONE		; no count
	lds	si,DWORD PTR [bp+6]	; lpSrc
	les	di,DWORD PTR [bp+10]	; lpDst
	lfs	bx,DWORD PTR [bp+14]	; lpLut
	mov	dx,bx
	sub	ah,ah
MLI_LOOP:
	; cyan
	lodsb				; get source
	xor	al,255			; invert value
	mov	bx,dx			; get offset to lpLut
	add	bx,ax			; add value to offset
	mov	al,fs:[bx]		; lookup in lpLut
	xor	al,255			; invert value
	stosb				; store in dest
	; magenta
	lodsb				; get source
	xor	al,255			; invert value
	mov	bx,dx			; get offset to lpLut
	add	bx,ax			; add value to offset
	mov	al,fs:[bx]		; lookup in lpLut
	xor	al,255			; invert value
	stosb				; store in dest
	; yellow
	lodsb				; get source
	xor	al,255			; invert value
	mov	bx,dx			; get offset to lpLut
	add	bx,ax			; add value to offset
	mov	al,fs:[bx]		; lookup in lpLut
	xor	al,255			; invert value
	stosb				; store in dest
	; black
	lodsb				; get source
	stosb				; store in dest
	dec	cx			; decrement count
	jnz	MLI_LOOP		; not zero, keep going
MLI_DONE:
	pop	di
	pop	si
	lea	sp,WORD PTR [bp-2]
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	

_MapLut32	ENDP

ENGINEA_TEXT	ENDS
END

