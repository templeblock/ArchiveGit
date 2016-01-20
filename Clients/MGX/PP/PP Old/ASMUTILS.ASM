	.286
	.model medium

	.data
lSeed	DD	0deadbabaH
EXTRN	_Matrix:BYTE

	.code

EXTRN	__chkstk:FAR
EXTRN	_frame_set:FAR

;
; void ProcessData8(lpDst, lpSrc, lpMsk, iCount
;	LPTR	lpDst;
;	LPTR	lpSrc;
;	LPTR	lpMsk;
;	int	iCount;
;
	PUBLIC	_ProcessData8
_ProcessData8	PROC FAR
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

;	lpDst = 6
;	lpSrc = 10
;	lpMsk = 14
;	iCount = 18
;	b = -4
;	s = -6
;	d = -8

	lds	si,DWORD PTR [bp+14]	;lpMsk
	les	di,DWORD PTR [bp+6]	;lpDst
	jmp	SHORT PD8_4
PD8_1:
	lodsb
	sub	ah,ah
	or	ax,ax
	jne	PD8_2
	inc 	WORD PTR [bp+10]	; lpSrc++
	inc	di			; lpDst++
	jmp	PD8_4

PD8_2:
	cmp	ax,127
	jbe	PD8_3
	inc	ax			;b

PD8_3:
	push	ds
	push	si
	lds	si,DWORD PTR [bp+10]	;lpSrc

	mov	cx,ax			; cx = mask value

	lodsb				; get the source
	sub	ah,ah			; unsigned word conversion
	mul	cx			; source * b
	mov	bx,ax			; bx = (source * b)
	
	mov	dx,256			
	sub	dx,cx			; b-256
	mov	al,BYTE PTR es:[di]	; get the destination
	sub	ah,ah			; unsigned word conversion
	mul	dx			; (dest * (256-b))
	
	add	bx,ax			; bx = (s * b) + (d * (256-b))
	mov	al,bh			; get HIBYTE
	stosb				; store in destination

	mov	WORD PTR [bp+10],si	; save lpSrc
	pop	si
	pop	ds
PD8_4:
	dec	WORD PTR [bp+18]	;iCount
	jns	PD8_1

	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_ProcessData8	ENDP

;
; void ProcessData24(lpDst, lpSrc, lpMsk, iCount
;	LPRGB	lpDst;
;	LPRGB	lpSrc;
;	LPTR	lpMsk;
;	int	iCount;
;
	PUBLIC	_ProcessData24
_ProcessData24	PROC FAR
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

;	lpDst = 6
;	lpSrc = 10
;	lpMsk = 14
;	iCount = 18
;	b = -4
;	s = -6
;	d = -8

	lds	si,DWORD PTR [bp+14]	;lpMsk
	les	di,DWORD PTR [bp+6]	;lpDst
	jmp	SHORT PD24_4
PD24_1:
	lodsb
	sub	ah,ah
	or	ax,ax
	jne	PD24_2
	add 	WORD PTR [bp+10],3	; lpSrc++
	add	di,3			; lpDst++
	jmp	PD24_4

PD24_2:
	cmp	ax,127
	jbe	PD24_3
	inc	ax			;b

PD24_3:
	push	ds
	push	si
	lds	si,DWORD PTR [bp+10]	;lpSrc

	mov	cx,ax			; cx = mask value

; Process the red channel
	lodsb				; get the source
	sub	ah,ah			; unsigned word conversion
	mul	cx			; source * b
	mov	bx,ax			; bx = (source * b)
	
	mov	dx,256			
	sub	dx,cx			; b-256
	mov	al,BYTE PTR es:[di]	; get the destination
	sub	ah,ah			; unsigned word conversion
	mul	dx			; (dest * (256-b))
	
	add	bx,ax			; bx = (s * b) + (d * (256-b))
	mov	al,bh			; get HIBYTE
	stosb				; store in destination

; Process the green channel
	lodsb				; get the source
	sub	ah,ah			; unsigned word conversion
	mul	cx			; source * b
	mov	bx,ax			; bx = (source * b)
	
	mov	dx,256			
	sub	dx,cx			; b-256
	mov	al,BYTE PTR es:[di]	; get the destination
	sub	ah,ah			; unsigned word conversion
	mul	dx			; (dest * (256-b))
	
	add	bx,ax			; bx = (s * b) + (d * (256-b))
	mov	al,bh			; get HIBYTE
	stosb				; store in destination

; Process the blue channel
	lodsb				; get the source
	sub	ah,ah			; unsigned word conversion
	mul	cx			; source * b
	mov	bx,ax			; bx = (source * b)
	
	mov	dx,256			
	sub	dx,cx			; b-256
	mov	al,BYTE PTR es:[di]	; get the destination
	sub	ah,ah			; unsigned word conversion
	mul	dx			; (dest * (256-b))
	
	add	bx,ax			; bx = (s * b) + (d * (256-b))
	mov	al,bh			; get HIBYTE
	stosb				; store in destination

	mov	WORD PTR [bp+10],si	; save lpSrc
	pop	si
	pop	ds
PD24_4:
	dec	WORD PTR [bp+18]	;iCount
	jns	PD24_1

	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_ProcessData24	ENDP


;
; void MergeMask(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, lpSrcRect,
;		 lpSectRect)
;	LPTR	lpDst;
;	LPRECT	lpDstRect;
;	int	DstWidth;
;	LPTR	lpSrc;
;	LPRECT	lpSrcRect;
;	int	SrcWidth;
;	LPRECT	lpSectRect;
;
; 
	PUBLIC	_MergeMask
_MergeMask	PROC FAR
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

;	lpDst = 6
;	lpDstRect = 10
;	DstWidth = 14
;	lpSrc = 16
;	lpSrcRect = 20
;	SrcWidth = 24
;	lpSectRect = 26

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	cx,WORD PTR ds:[bx+4]	;lpSectRect->right
	sub	cx,WORD PTR ds:[bx]	;lpSectRect->right-lpSectRect->left
	inc	cx			;RectWidth(lpSectRect)

	mov	dx,WORD PTR ds:[bx+6]	;lpSectRect->bottom
	sub	dx,WORD PTR ds:[bx+2]	;lpSectRect->bottom-lpSectRect->top
	inc	dx			;RectHeight(lpSectRect)

	push	cx
	push	dx

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	ax,WORD PTR ds:[bx+2]
	lds	bx,DWORD PTR [bp+10]	;lpDstRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	cx,WORD PTR ds:[bx]
	lds	bx,DWORD PTR [bp+10]	;lpDstRect
	sub	cx,WORD PTR ds:[bx]	; cx = dx

	mul 	WORD PTR [bp+14]	; dy * DstWidth
	add	ax,cx			; (dy * DstWidth) + dx
	add	ax,WORD PTR [bp+6]	;lpDst
	mov	dx,WORD PTR [bp+8]
	mov	di,ax			;lpDstBuf
	mov	es,dx

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	ax,WORD PTR ds:[bx+2]
	lds	bx,DWORD PTR [bp+20]	;lpSrcRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	cx,WORD PTR ds:[bx]
	lds	bx,DWORD PTR [bp+20]	;lpSrcRect
	sub	cx,WORD PTR ds:[bx]	; cx = dx

	mul	WORD PTR [bp+24]	; dy * SrcWidth
	add	ax,cx			; (dy * SrcWidth) + dx
	add	ax,WORD PTR [bp+16]	;lpSrc
	mov	dx,WORD PTR [bp+18]
	mov	si,ax			;lpSrcBuf
	mov	ds,dx

	pop	dx
	pop	bx

	sub	WORD PTR [bp+14],bx	; DstWidth
	sub	WORD PTR [bp+24],bx	; SrcWidth

	cld
	jmp	MM_5
MM_1:
	mov	cx,bx
MM_2:
	lodsb				; al = source
	cmp 	BYTE PTR es:[di],al	;
	jae	MM_3
	stosb
	loop	MM_2
	jmp	MM_4
MM_3:
	inc	di
	loop	MM_2
MM_4:
	add	si,WORD PTR [bp+24]	;SrcWidth
	add	di,WORD PTR [bp+14]	;DstWidth
MM_5:
	dec	dx
	jns	MM_1

	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	
_MergeMask	ENDP

;
; void MergeData8(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, lpSrcRect,
;		 lpSectRect)
;	LPTR	lpDst;
;	LPRECT	lpDstRect;
;	int	DstWidth;
;	LPTR	lpSrc;
;	LPRECT	lpSrcRect;
;	int	SrcWidth;
;	LPRECT	lpSectRect;
;
; 
	PUBLIC	_MergeData8
_MergeData8	PROC FAR
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

;	lpDst = 6
;	lpDstRect = 10
;	DstWidth = 14
;	lpSrc = 16
;	lpSrcRect = 20
;	SrcWidth = 24
;	lpSectRect = 26

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	cx,WORD PTR ds:[bx+4]	;lpSectRect->right
	sub	cx,WORD PTR ds:[bx]	;lpSectRect->right-lpSectRect->left
	inc	cx			;RectWidth(lpSectRect)

	mov	dx,WORD PTR ds:[bx+6]	;lpSectRect->bottom
	sub	dx,WORD PTR ds:[bx+2]	;lpSectRect->bottom-lpSectRect->top
	inc	dx			;RectHeight(lpSectRect)

	push	cx
	push	dx

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	ax,WORD PTR ds:[bx+2]
	lds	bx,DWORD PTR [bp+10]	;lpDstRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	cx,WORD PTR ds:[bx]
	lds	bx,DWORD PTR [bp+10]	;lpDstRect
	sub	cx,WORD PTR ds:[bx]	; cx = dx

	mul 	WORD PTR [bp+14]	; dy * DstWidth
	add	ax,cx			; (dy * DstWidth) + dx
	add	ax,WORD PTR [bp+6]	;lpDst
	mov	dx,WORD PTR [bp+8]
	mov	di,ax			;lpDstBuf
	mov	es,dx

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	ax,WORD PTR ds:[bx+2]
	lds	bx,DWORD PTR [bp+20]	;lpSrcRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	cx,WORD PTR ds:[bx]
	lds	bx,DWORD PTR [bp+20]	;lpSrcRect
	sub	cx,WORD PTR ds:[bx]	; cx = dx

	mul	WORD PTR [bp+24]	; dy * SrcWidth
	add	ax,cx			; (dy * SrcWidth) + dx
	add	ax,WORD PTR [bp+16]	;lpSrc
	mov	dx,WORD PTR [bp+18]
	mov	si,ax			;lpSrcBuf
	mov	ds,dx

	pop	dx
	pop	bx

	sub	WORD PTR [bp+14],bx	; DstWidth
	mov	ax,WORD PTR [bp+24]	; SrcWidth
	sub	ax,bx

	cld
	jmp	MD8_2
MD8_1:
	mov	cx,bx
	rep	movsb

	add	si,ax			;SrcWidth
	add	di,WORD PTR [bp+14]	;DstWidth
MD8_2:
	dec	dx
	jns	MD8_1

	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_MergeData8	ENDP


;
; void MergeData24(lpDst, lpDstRect, DstWidth, lpSrc, lpSrcRect, lpSrcRect,
;		 lpSectRect)
;	LPRGB	lpDst;
;	LPRECT	lpDstRect;
;	int	DstWidth;
;	LPRGB	lpSrc;
;	LPRECT	lpSrcRect;
;	int	SrcWidth;
;	LPRECT	lpSectRect;
;
; 
	PUBLIC	_MergeData24
_MergeData24	PROC FAR
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

;	lpDst = 6
;	lpDstRect = 10
;	DstWidth = 14
;	lpSrc = 16
;	lpSrcRect = 20
;	SrcWidth = 24
;	lpSectRect = 26

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	ax,WORD PTR ds:[bx+4]	;lpSectRect->right
	sub	ax,WORD PTR ds:[bx]	;lpSectRect->right-lpSectRect->left
	inc	ax			;RectWidth(lpSectRect)
	mov	cx,ax
	add	ax,ax
	add	cx,ax

	mov	dx,WORD PTR ds:[bx+6]	;lpSectRect->bottom
	sub	dx,WORD PTR ds:[bx+2]	;lpSectRect->bottom-lpSectRect->top
	inc	dx			;RectHeight(lpSectRect)

	push	cx
	push	dx

	mov	ax,WORD PTR [bp+24]	; get SrcWidth
	add	ax,ax
	add	WORD PTR [bp+24],ax	; save SrcWidth * Depth

	mov	ax,WORD PTR [bp+14]	; DstWidth
	add	ax,ax
	add	WORD PTR [bp+14],ax	; save DstWidth * Depth

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	ax,WORD PTR ds:[bx+2]
	lds	bx,DWORD PTR [bp+10]	;lpDstRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	cx,WORD PTR ds:[bx]
	lds	bx,DWORD PTR [bp+10]	;lpDstRect
	sub	cx,WORD PTR ds:[bx]	; cx = dx
	mov	bx,cx
	add	bx,bx
	add	cx,bx

	mul 	WORD PTR [bp+14]	; dy * DstWidth
	add	ax,cx			; (dy * DstWidth) + dx
	add	ax,WORD PTR [bp+6]	;lpDst
	mov	dx,WORD PTR [bp+8]
	mov	di,ax			;lpDstBuf
	mov	es,dx

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	ax,WORD PTR ds:[bx+2]
	lds	bx,DWORD PTR [bp+20]	;lpSrcRect
	sub	ax,WORD PTR ds:[bx+2]	; ax = dy

	lds	bx,DWORD PTR [bp+26]	;lpSectRect
	mov	cx,WORD PTR ds:[bx]
	lds	bx,DWORD PTR [bp+20]	;lpSrcRect
	sub	cx,WORD PTR ds:[bx]	; cx = dx
	mov	bx,cx
	add	bx,bx
	add	cx,bx

	mul	WORD PTR [bp+24]	; dy * SrcWidth
	add	ax,cx			; (dy * SrcWidth) + dx
	add	ax,WORD PTR [bp+16]	;lpSrc
	mov	dx,WORD PTR [bp+18]
	mov	si,ax			;lpSrcBuf
	mov	ds,dx

	pop	dx
	pop	bx

	sub	WORD PTR [bp+14],bx	; DstWidth
	mov	ax,WORD PTR [bp+24]	; SrcWidth
	sub	ax,bx

	cld
	jmp	MD24_2
MD24_1:
	mov	cx,bx
	rep	movsb

	add	si,ax			;SrcWidth
	add	di,WORD PTR [bp+14]	;DstWidth
MD24_2:
	dec	dx
	jns	MD24_1

	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_MergeData24	ENDP

;
; void copy(lpSrc, lpDst, iCount)
;	LPTR	lpSrc;
;	LPTR	lpDst;
;	int	iCount;
;
; 
	PUBLIC	_copy
_copy	PROC FAR
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

;	lpSrc = 6
;	lpDst = 10
;	iCount = 14
	
	lds	si,DWORD PTR [bp+6]	; ds:[si] - lpSrc
	les	di,DWORD PTR [bp+10]	; es:[di] - lpDst
	mov	cx,WORD PTR [bp+14]	; iCount
	cld
	shr	cx,1
	rep	movsw
	jnc	CopyDone
	lodsb
	stosb
CopyDone:
	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_copy	ENDP

;
; void clr(lpDst, iCount)
;	LPTR	lpDst;
;	int	iCount;
;
; 
	PUBLIC	_clr
_clr	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	push	di
;	lpDst = 6
;	iCount = 10
	
	les	di,DWORD PTR [bp+6]	; es:[di] - lpDst
	mov	cx,WORD PTR [bp+10]	; iCount
	cld
	xor	ax,ax
	shr	cx,1
	rep	stosw
	jnc	ClrDone
	stosb
ClrDone:
	pop	di
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_clr	ENDP

;
; void set(lpDst, iCount, iValue)
;	LPTR	lpDst;
;	int	iCount;
;	int	iValue;
;
; 
	PUBLIC	_set
_set	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	push	di

;	lpDst = 6
;	iCount = 10
;	iValue = 12
	
	les	di,DWORD PTR [bp+6]	; es:[di] - lpDst
	mov	cx,WORD PTR [bp+10]	; iCount
	mov	dx,WORD PTR [bp+12]	; iValue;
	xor	dh,dh
	mov	ax,dx
	shl	dx,8
	or	ax,dx
	cld
	shr	cx,1
	rep	stosw
	jnc	SetDone
	stosb
SetDone:
	pop	di
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_set	ENDP

;
; void set24(lpDst, iCount, iValue)
;	LPRGB	lpDst;
;	int	iCount;
;	long	lValue;
;
; 
	PUBLIC	_set24
_set24	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	push	di
;	lpDst = 6
;	iCount = 10
;	iValue = 12
	
	les	di,DWORD PTR [bp+6]	; es:[di] - lpDst
	mov	cx,WORD PTR [bp+10]	; iCount
	mov	ax,WORD PTR [bp+12]	; lValue - R and G
	mov	dx,WORD PTR [bp+14]	; lValue - B
	cld
Set24_1:
	stosw
	mov	es:[di],dl
	inc	di
	loop	Set24_1

	pop	di
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_set24	ENDP

;
; void setifset(lpDst, iCount, iValue)
;	LPTR	lpDst;
;	int	iCount;
;	int	iValue;
;
; 
	PUBLIC	_setifset
_setifset PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	push	di
;	lpDst = 6
;	iCount = 10
;	iValue = 12
	
	les	di,DWORD PTR [bp+6]	; es:[di] - lpDst
	mov	cx,WORD PTR [bp+10]	; iCount
	mov	ax,WORD PTR [bp+12]	; iValue;
	cld
SetIfSetLoop:
	mov	dl,BYTE PTR es:[di]
	or	dl,dl
	je	SetIfSetNone
	stosb
	loop	SetIfSetLoop
	jmp	SetIfSetDone
SetIfSetNone:
	inc	di
	loop	SetIfSetLoop
SetIfSetDone:
	pop	di
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_setifset ENDP


;
; void ScatterMask(lpSrc, lpDst, lpErrors, dx, dy)
;	LPTR lpSrc;
;	LPTR lpDst;
;	LPINT lpErrors;
;	int dx, dy;

	PUBLIC	_ScatterMask
_ScatterMask	PROC FAR
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
;	dy = 20
;	lSeed = -14 lSeed+2 = -12
;	error = -10
;	iCount = -4
;	lpError = -8

	mov	ax,WORD PTR [bp+18]	;dx
	inc	ax
	shl	ax,1
	push	ax
	push	WORD PTR [bp+16]
	push	WORD PTR [bp+14]	;lpErrors
	call	FAR PTR _clr
	add	sp,6
	les	di,DWORD PTR [bp+10]	; es:[di] - lpDst
	mov	ax,WORD PTR lSeed
	mov	WORD PTR [bp-14],ax	; lSeed
	mov	ax,WORD PTR lSeed+2
	mov	WORD PTR [bp-12],ax	; lSeed+2
	push	ds
	jmp	SM_10

SM_1:
	lds	si,DWORD PTR [bp+14]	; ds:[si] - lpError = lpErrors
	mov	cx,WORD PTR [bp+18]	;dx

SM_2:
	mov	bx,101			;error
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
	loop	SM_2

SM_10:
	dec	WORD PTR [bp+20]	;dy
	jns	SM_1
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

_ScatterMask	ENDP

;
;VOID MaskLoad( x, y, dx, lpM, onMask )
;int x, y, dx;
;LPTR lpM;
;int onMask;
;
	PUBLIC	_MaskLoad
_MaskLoad	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	mov	ax,20
	call	FAR PTR __chkstk
	push	si
	push	di
;
;	y = 8
;	dx = 10
;	lpM = 12
;	onMask = 16
;	lpFrame = -12
;	lpMask = -16
;	lpEdge = -8
;	State = -4
;	xend = -20
;	xe = -18
;	x = 6
;
	cmp	WORD PTR [bp+16],1	;onMask
	sbb	ax,ax
	neg	ax
	push	ax
	push	WORD PTR [bp+10]	;dx
	push	WORD PTR [bp+14]
	push	WORD PTR [bp+12]	;lpM
	call	FAR PTR _set
	add	sp,8
;
	push	0
	push	0
	call	FAR PTR _frame_set
	add	sp,4
	mov	WORD PTR [bp-12],ax	;lpFrame
	mov	WORD PTR [bp-10],dx
	or	dx,ax
	jne	MaskLoad_0
	jmp	MaskLoad_End
MaskLoad_0:
	les	bx,DWORD PTR [bp-12]	;lpFrame
	mov	ax,WORD PTR es:[bx+21]
	mov	dx,WORD PTR es:[bx+23]
	mov	WORD PTR [bp-16],ax	;lpMask
	mov	WORD PTR [bp-14],dx
	or	dx,ax
	jne	MaskLoad_2
	jmp	MaskLoad_End
MaskLoad_2:
	les	bx,DWORD PTR [bp-16]	;lpMask
	mov	ax,WORD PTR es:[bx+12]
	or	ax,WORD PTR es:[bx+14]
	jne	MaskLoad_4
	jmp	MaskLoad_End
MaskLoad_4:
	mov	ax,WORD PTR [bp+8]	;y
	cmp	WORD PTR es:[bx+10],ax
	jg	MaskLoad_6
	jmp	MaskLoad_End
MaskLoad_6:
	or	ax,ax
	jge	MaskLoad_8
	jmp	MaskLoad_End
MaskLoad_8:
	mov	si,ax
	shl	si,2
	les	bx,DWORD PTR es:[bx+12]
	mov	ax,es:[bx][si]
	mov	dx,es:[bx+2][si]
	mov	si,ax			; ds:[si] = lpEdge
	mov	ds,dx
	or	dx,ax
	je	MaskLoad_End
	
	les	di,DWORD PTR [bp+12]	; es:[di] = lpM
	sub	bx,bx			;bx = State = 0
	mov	dx,WORD PTR [bp+6]	;dx = x
	mov	cx,dx
	add	cx,WORD PTR [bp+10]	;dx
	dec	cx			;cx = xend
	jmp	MaskLoad_22
	nop	
	nop	
MaskLoad_10:
	mov	ax,WORD PTR ds:[si]	;lpEdge->x
	cmp	dx,ax			;if (x < lpEdge->x)
	jge	MaskLoad_20
	dec	ax			;min(lpEdge->x-1,xend)
	push	cx			;save xend
	cmp	ax,cx			;xend
	jg	MaskLoad_12
	mov	cx,ax			;lpEdge->x-1
MaskLoad_12:
	jmp	SHORT MaskLoad_18
	nop	
MaskLoad_14:
	or	bx,bx			;if (State)
	je	MaskLoad_16		;State == 0
	mov	al,BYTE PTR [bp+16]	;onMask
	mov	BYTE PTR es:[di],al	;*lpM = onMask
MaskLoad_16:
	inc	di			;lpM++
	inc	dx			;x++
MaskLoad_18:
	cmp	dx,cx			;while (x <= xe)
	jle	MaskLoad_14
	pop	cx			;restore xend
MaskLoad_20:
	mov	ax,WORD PTR [bp+8]	;y
	cmp	WORD PTR ds:[si+2],ax
	jne	MaskLoad_End
	add	si,4			;lpEdge++
	cmp	bx,1			;State
	sbb	bx,bx
	neg	bx
MaskLoad_22:
	cmp	dx,cx			; while (x <= xend)
	jle	MaskLoad_10
MaskLoad_End:
	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
_MaskLoad	ENDP

;
;void Convert24to24A( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, lpMatrix )
;LPRGB lpPixel;
;int iCount;
;LPINT lpError;
;LPTR lpOutput;
;int xDiva, yDiva;
;LPTR lpMatrix
;
	PUBLIC	_Convert24to24A
_Convert24to24A	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	xor	ax,ax
	call	FAR PTR __chkstk
	push	si
	push	di

;	iCount = 10
;	lpError = 12
;	lpOutput = 16
;	xDiva = 20
;	yDiva = 22
;	lpMatrix = 24
;	lpPixel = 6
	mov	cx,WORD PTR [bp+10]	;iCount
	or	cx,cx
	je	C24to24_3
	cld
	mov	bx,ds
	lds	si,DWORD PTR [bp+6]
	les	di,DWORD PTR [bp+16]
C24to24_1:
	mov	al,ds:[si+2]		;get blue
	stosb				;store blue
	mov	al,ds:[si+1]		;get green
	stosb				;store green
	mov	al,ds:[si]		;get red
	stosb				;store red
	add	si,3
	loop	C24to24_1

C24to24_3:
	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_Convert24to24A	ENDP

;
;void Convert8to24A( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, bPreview )
;LPTR lpPixel;
;int iCount;
;LPINT lpError;
;LPTR lpOutput;
;int xDiva, yDiva;
;LPTR lpMatrix
;

	PUBLIC	_Convert8to24A
_Convert8to24A	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	xor	ax,ax
	call	FAR PTR __chkstk
	push	si
	push	di

;	iCount = 10
;	lpError = 12
;	lpOutput = 16
;	xDiva = 20
;	yDiva = 22
;	lpMatrix = 24
;	lpPixel = 6
	mov	cx,WORD PTR [bp+10]	;iCount
	or	cx,cx
	je	C8to24_3
	cld
	mov	bx,ds
	lds	si,DWORD PTR [bp+6]
	les	di,DWORD PTR [bp+16]
C8to24_1:
	lodsb				;get gray
	stosb				;store blue
	stosb				;store green
	stosb				;store red
	loop	C8to24_1

C8to24_3:
	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_Convert8to24A	ENDP

;
;void Convert24to4A( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, ;lpMatrix )
;LPRGB lpPixel;
;int iCount;
;LPINT lpError;
;LPTR lpOutput;
;int xDiva, yDiva;
;LPTR lpMatrix;
;
	PUBLIC	_Convert24to4A
_Convert24to4A	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	mov	ax,30
	call	FAR PTR __chkstk
	push	si
	push	di

;	lpPixel = 6
;	iCount = 10
;	lpError = 12
;	lpOutput = 16
;	xDiva = 20
;	yDiva = 22
;	lpMatrix = 24
;	lpFirst = -20
;	lpLast = -8
;	lpMatrix = -14
;	red = -26
;	green = -10
;	blue = -24
;	register si = thresh
;	p = -4
;	op = -30
;	w = -22
;	IsOdd = -16
;	save_ds = -32

	mov	ax,ds
	mov	WORD PTR [bp-32],ax
	mov	ax,WORD PTR [bp+22]	;yDiva
	and	ax,15			;yDiva&15
	shl	ax,4			;(yDiva&15)*16
	add	ax,WORD PTR [bp+24]	;lpMatrix + ((yDiva&15)*16)
	mov	dx,WORD PTR [bp+28]	;lpMatrix segment
	mov	WORD PTR [bp-20],ax	;lpFirst

	add	ax,15
	mov	WORD PTR [bp-8],ax	;lpLast

	mov	bx,WORD PTR [bp+20]	;xDiva
	and	bx,15
	add	bx,WORD PTR [bp-20]	;lpFirst
	mov	WORD PTR [bp-12],dx	;es = lpMatrix segment
	mov	es,dx
	mov	WORD PTR [bp-16],0	;IsOdd
	lds	si,DWORD PTR [bp+6]	; ds:[si] = lpPixel
	mov	di,WORD PTR [bp+16]	; di = lpOutput

	jmp	C24to4_27
	nop	
C24to4_3:
	mov	cl,BYTE PTR ds:[si]	;cl = red
	mov	ch,BYTE PTR ds:[si+1]	;ch = green
	mov	dl,BYTE PTR ds:[si+2]	;dl = blue
	add	si,3

C24to4_5:
	mov	dh,BYTE PTR es:[bx]	;*lpMatrix
	inc	bx			;lpMatrix++

	cmp	bx,WORD PTR[bp-8]	;if (lpMatrix > lpLast)
	jbe	C24to4_7		;lpMatrix <= lpLast

	mov	bx,WORD PTR [bp-20]	;lpFirst
C24to4_7:
	cmp	cl,dh			;red
	jb	C24to4_15		;red < thresh

	cmp	ch,dh			;green
	jb	C24to4_11		;green < thresh

	cmp	dl,dh			;blue
	jb	C24to4_9		;blue < thresh

	mov	ah,15			;p=C_WHITE

	jmp	SHORT C24to4_23

C24to4_9:
	mov	ah,11			;p=C_YELLOW

	jmp	SHORT C24to4_23
C24to4_11:
	cmp	dl,dh			;blue
	jb	C24to4_13		;blue < thresh

	mov	ah,13			;p=C_MAGENTA

	jmp	SHORT C24to4_23

C24to4_13:
	mov	ah,9			;p=C_RED

	jmp	SHORT C24to4_23
C24to4_15:
	cmp	ch,dh			;green
	jb	C24to4_19		;green < thresh

	cmp	dl,dh			;blue
	jb	C24to4_17		;blue < thresh

	mov	ah,14			;p=C_CYAN

	jmp	SHORT C24to4_23
C24to4_17:
	mov	ah,10			;p=C_GREEN

	jmp	C24to4_23
C24to4_19:
	cmp	dl,dh			;blue
	jb	C24to4_21		;blue < thresh

	mov	ah,12			;p=C_BLUE

	jmp	SHORT C24to4_23

C24to4_21:
	sub	ah,ah			;p=C_BLACK
C24to4_23:
	cmp	WORD PTR [bp-16],0	;IsOdd
	je	C24to4_25

	add	al,ah			;op += p
	mov	es,WORD PTR [bp+18]
	stosb				; *lpOutput++ = op
	mov	es,WORD PTR [bp-12]	;lpMatrix segment
	mov	WORD PTR [bp-16],0	;IsOdd = NO
	jmp	C24to4_27
	nop	
C24to4_25:
	mov	al,ah			;op = p
	shl	al,4			;op = p << 4
	mov	WORD PTR [bp-16],1	;IsOdd
C24to4_27:
	dec	WORD PTR [bp+10]	;iCount
	js	C24to4_29
	jmp	C24to4_3
C24to4_29:
	cmp	WORD PTR [bp-16],0	;IsOdd
	je	C24to4_31
	mov	es,WORD PTR [bp+18]
	stosb				;*lpOutput++ = op
C24to4_31:
	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
	nop	

_Convert24to4A	ENDP


;	extern BYTE Matrix[];

;	/*******************************************************************/
;	void Convert24to8( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
;	/*******************************************************************/

	PUBLIC	_Convert24to8a
_Convert24to8a	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	mov	ax,26
	call	FAR PTR __chkstk
	push	si
	push	di

;	LPRGB lpPixel;
;	int iCount;
;	register LPINT lpError;
;	LPTR lpOutput;
;	int xDiva, yDiva;

;	lpPixel = 6
;	iCount = 10
;	lpError = 12
;	lpOutput = 16
;	xDiva = 20
;	yDiva = 22

;	int thresh, negthresh;
;	LPTR lp, lpLast, lpFirst;
;	int red, green, blue;
;	register BYTE p;

;	thresh = -26
;	negthresh = -18
;	lp = -24
;	lpLast = -8
;	lpFirst = -14
;	red = -20
;	green = -10
;	blue = -16
;	p = -4

;	lpFirst = Matrix + (16*(yDiva&15));

	mov	ax,WORD PTR [bp+22]	;yDiva
	and	ax,15
	shl	ax,4
	add	ax,OFFSET _Matrix
	mov	WORD PTR [bp-14],ax	;lpFirst
	mov	WORD PTR [bp-12],ds

;	lpLast = lpFirst + 15;

	mov	dx,ds
	add	ax,15
	mov	WORD PTR [bp-8],ax	;lpLast
	mov	WORD PTR [bp-6],dx

;	lp = lpFirst + (xDiva&15);

	mov	ax,WORD PTR [bp+20]	;xDiva
	and	ax,15
	add	ax,WORD PTR [bp-14]	;lpFirst
	mov	WORD PTR [bp-24],ax	;lp
	mov	WORD PTR [bp-22],dx

;
; the following changes were made in the loop
;	1) cx is used as the counter instead of a mem loc
;	2) ah and dx are used for the variable p
;	3) lpPixel is not incremented until the end
;	3) di is used as a temp for calculating each color value
;
;	while ( --iCount >= 0 )
;		{

	lds	si,DWORD PTR [bp+6]	; ds:[si] = lpPixel
	mov	cx,WORD PTR [bp+10]	;icount
	jmp	$L20001
$FC5300:

;		// Compute the accumulated pixel values

;		thresh = *lp / 5; // 0-255 maps to 0-51

	les	bx,DWORD PTR [bp-24]	;lp
	mov	al,BYTE PTR es:[bx]
	sub	ah,ah
	mov	dl,5
	div	dl
	sub	ah,ah
	mov	di,ax
;	mov	WORD PTR [bp-26],ax	;thresh

;		negthresh = thresh - 51; // 0-255 maps to (-51)-0

	sub	ax,51
	mov	WORD PTR [bp-18],ax	;negthresh

;		if ( ++lp > lpLast )

	inc	bx
	cmp	bx,WORD PTR [bp-8]	;lp <= lpLast
	jbe	$I5303

;			lp = lpFirst;

	mov	bx,WORD PTR [bp-14]	;lpFirst
$I5303:
	mov	WORD PTR [bp-24],bx	;save lp

;	// RED - Handle the red component
;		red   = lpPixel->red;
	lodsb
	sub	ah,ah

;		p = 0;
	sub	dl,dl		; p = 0

;		if ( red < 128 )
;			{

	cmp	ax,128	;red
	jge	$I5304

;			if ( red < 26 )		{ /* p += 36*0; red -= 0; */ }

	cmp	ax,26	;red
	jl	$I5313

;			else if ( red < 77 )	{ p += 36*1; red -= 51; }

	cmp	ax,77	;red
	jge	$I5307
	add	dl,36			;p
	sub	ax,51	;red
	jmp	SHORT $I5313
	nop	
$I5307:

;			     else		{ p += 36*2; red -= 102; }

	add	dl,72			;p
	sub	ax,102	;red

;			}

	jmp	SHORT $I5313
$I5304:

;		else	{
;			if ( red < 179 )	{ p += 36*3; red -= 153; }

	cmp	ax,179	;red
	jge	$I5310
	add	dl,108			;p
	sub	ax,153	;red
	jmp	SHORT $I5313
$I5310:

;			else if ( red < 230 )	{ p += 36*4; red -= 204; }

	cmp	ax,230	;red
	jge	$I5312
	add	dl,144			;p
	sub	ax,204	;red
	jmp	SHORT $I5313
$I5312:

;			     else		{ p += 36*5; red -= 255; }

	add	dl,180			;p
	sub	ax,255	;red

;			}

$I5313:

;		if ( red > thresh )

	cmp	ax,di	;red
	jle	$I5314

;			p += 36;

	add	dl,36
	jmp	SHORT $I5316

;		else

$I5314:

;		if ( red < negthresh )
	cmp	ax,WORD PTR [bp-18]	;red
	jge	$I5316

;			p -= 36;
	sub	dl,36			;p
$I5316:

;	
;	// GREEN - Handle the green component

;		green = lpPixel->green;

	lodsb
	sub	ah,ah

;		if ( green < 128 )

	cmp	ax,128	;green
	jge	$I5317

;			if ( green < 26 )	{/* p += 6*0; green -= 0; */ }

	cmp	ax,26	;green
	jl	$I5326

;			else if ( green < 77 )	{ p += 6*1; green -= 51; }

	cmp	ax,77	;green
	jge	$I5320
	add	dl,6			;p
	sub	ax,51	;green
	jmp	SHORT $I5326
$I5320:

;			     else		{ p += 6*2; green -= 102; }

	add	dl,12			;p
	sub	ax,102	;green

;			}

	jmp	SHORT $I5326
$I5317:

;		else	{
;			if ( green < 179 )	{ p += 6*3; green -= 153; }

	cmp	ax,179	;green
	jge	$I5323
	add	dl,18			;p
	sub	ax,153	;green
	jmp	SHORT $I5326
$I5323:

;			else if ( green < 230 )	{ p += 6*4; green -= 204; }

	cmp	ax,230	;green
	jge	$I5325
	add	dl,24			;p
	sub	ax,204	;green
	jmp	SHORT $I5326
$I5325:

;			     else		{ p += 6*5; green -= 255; }

	add	dl,30			;p
	sub	ax,255	;green
$I5326:

;			}
;		if ( green > thresh )

	cmp	ax,di	;green
	jle	$I5327

;			p += 6;

	add	dl,6			;p
	jmp	SHORT $I5329
$I5327:

;		else
;		if ( green < negthresh )

	cmp	ax,WORD PTR [bp-18]	;green
	jge	$I5329

;			p -= 6;

	sub	dl,6			;p
$I5329:

;	// BLUE - Handle the blue component

;		blue  = lpPixel->blue;

	lodsb
	sub	ah,ah

;		if ( blue < 128 )

	cmp	ax,128	;blue
	jge	$I5330

;			{
;			if ( blue < 26 )	{ /* p += 0; blue -= 0; */ }

	cmp	ax,26	;blue
	jl	$I5339

;			else if ( blue < 77 )	{ p += 1; blue -= 51; }

	cmp	ax,77	;blue
	jge	$I5333
	inc	dl			;p
	sub	ax,51	;blue
	jmp	SHORT $I5339
$I5333:

;			     else		{ p += 2; blue -= 102; }

	add	dl,2			;p
	sub	ax,102	;blue

;			}

	jmp	SHORT $I5339
$I5330:

;		else	{
;			if ( blue < 179 )	{ p += 3; blue -= 153; }

	cmp	ax,179	;blue
	jge	$I5336
	add	dl,3			;p
	sub	ax,153	;blue
	jmp	SHORT $I5339
$I5336:

;			else if ( blue < 230 )	{ p += 4; blue -= 204; }

	cmp	ax,230	;blue
	jge	$I5338
	add	dl,4			;p
	sub	ax,204	;blue
	jmp	SHORT $I5339
$I5338:

;			     else		{ p += 5; blue -= 255; }

	add	dl,5			;p
	sub	ax,255	;blue
$I5339:

;			}
;		if ( blue > thresh )

	cmp	ax,di	;blue
	jle	$I5340

;			p++;

	inc	dl	;p
	jmp	SHORT $I5342
$I5340:

;		else
;		if ( blue < negthresh )

	cmp	ax,WORD PTR [bp-18]	;blue
	jge	$I5342

;			p--;

	dec	dl	;p
$I5342:

;		*lpOutput++ = p; // Output the computed pixel

	les	bx,DWORD PTR [bp+16]	;lpOutput
	inc	WORD PTR [bp+16]	;lpOutput
	mov	BYTE PTR es:[bx],dl

$L20001:

;		}

	dec	cx		;iCount
	js	$JCC534
	jmp	$FC5300
$JCC534:

;	}

	pop	di
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	

_Convert24to8a	ENDP

END

