	.286
	.model medium

	.data
lSeed	DD	0deadbabaH
	.code

EXTRN	__chkstk:FAR
EXTRN	__aFldiv:FAR
EXTRN	__aFlmul:FAR
EXTRN	_fget:FAR

IFNDEF ASM_CODE
;/************************************************************************/
;void ProcessVignette24(lpSrc, iCount, lpd, lpD, nRepetitions, SoftTransition, ;startR, startG, startB, Rrange, Grange, Brange )
;/************************************************************************/
;LPRGB lpSrc;
;int iCount;
;LPLONG lpd;
;LPLONG lpD;
;int nRepetitions;
;BOOL SoftTransition;
;int startR, startG, startB;
;long Rrange, Grange, Brange;
	PUBLIC	_ProcessVignette24
_ProcessVignette24	PROC FAR
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
	push	di
	push	si
;
;	lpSrc = 6
;	iCount = 10
;	lpd = 12
;	lpD = 16
;	nRepetitions = 20
;	SoftTransition = 22
;	startR = 24
;	startG = 26
;	startB = 28
;	Rrange = 30
;	Grange = 34
;	Brange = 38
;	d = -14
;	D = -10
;	Rvalue = -22
;	Gvalue = -6
;	Bvalue = -20
;	r = -4
;	rate = -18
;	lSeed+2 = -24 - HIWORD
;	lSeed = -26   - LOWORD
;
	mov	ax,WORD PTR lSeed
	mov	WORD PTR [bp-26],ax
	mov	ax,WORD PTR lSeed+2
	mov	WORD PTR [bp-24],ax
	mov	di,WORD PTR [bp+6]	; es:[di] = lpSrc
	cld
	jmp	PV24_22
PV24_1:
	les	si,DWORD PTR [bp+12]	;lpd
	add	WORD PTR [bp+12],4	;lpd
	mov	bx,WORD PTR es:[si]	;BX/DX = d
	mov	dx,WORD PTR es:[si+2]
	les	si,DWORD PTR [bp+16]	;lpD
	add	WORD PTR [bp+16],4	;lpD
	mov	cx,WORD PTR es:[si]	;cx = D
	cmp	dx,0			; is d negative?
	jge	PV24_2
	mov	WORD PTR [bp-4],0	;r
	sub	bx,bx			; BX = d = 0
	jmp	SHORT PV24_6
	nop	
PV24_2:					; positive d
	cmp	bx,cx			; d > D
	jb	PV24_4
PV24_3:
	mov	WORD PTR [bp-4],0	;r
	mov	bx,cx			;SI = d = D
	jmp	SHORT PV24_6
PV24_4:
	mov	ax,WORD PTR [bp-26]	; lSeed
	mov	dx,WORD PTR [bp-24]	; lSeed+2
	add	ax,ax			; lSeed += lSeed
	adc	WORD PTR [bp-24],dx
	test	dx,2			; if (lSeed & BIT28)
	je	PV24_7
	xor	ax,39			; lSeed ^= BITMASK
PV24_7:
	mov	WORD PTR [bp-26],ax
	and	ax,7
	sub	ax,3
	mov	WORD PTR [bp-4],ax	;r
PV24_6:
	cmp	WORD PTR [bp+20],1	;nRepetitions
	jne	PV24_9
	sub	ax,ax			;
	push	ax			; no hiword of D
	push	cx			; LOWORD(D)
	push	bx			; d<<16
	push	ax			; no loword of d<<16
	call	FAR PTR __aFldiv
PV24_8:
	mov	WORD PTR [bp-18],ax	;rate
	mov	WORD PTR [bp-16],dx
	jmp	SHORT PV24_12
	nop	
PV24_9:
	sub	ax,ax
	push	ax			; no hiword of D
	push	cx			; D
	push	ax			; no hiword of d
	push	bx			; d
	mov	ax,WORD PTR [bp+20]	;nRepetitions
	cwd	
	push	dx
	push	ax
	call	FAR PTR __aFlmul
	push	dx
	push	ax
	call	FAR PTR _fget
	add	sp,8
	mov	WORD PTR [bp-18],ax	;rate
	mov	WORD PTR [bp-16],dx
	cmp	dx,1
	jl	PV24_12
	jg	PV24_10
	or	ax,ax
	je	PV24_12
PV24_10:
	sub	WORD PTR [bp-18],1	;rate
	sbb	WORD PTR [bp-16],0
	cmp	WORD PTR [bp+22],0	;SoftTransition
	je	PV24_11
	test	BYTE PTR [bp-16],1
	je	PV24_11
	mov	ax,WORD PTR [bp-18]	;rate
	sub	dx,dx
	sub	ax,dx
	sbb	dx,1
	neg	ax
	adc	dx,0
	neg	dx
	jmp	SHORT PV24_8
PV24_11:
	sub	WORD PTR [bp-18],1	;rate
	sbb	WORD PTR [bp-16],0
	mov	ax,WORD PTR [bp-18]	;rate
	mov	WORD PTR [bp-16],0
	add	WORD PTR [bp-18],1	;rate
	adc	WORD PTR [bp-16],0
PV24_12:
	mov	es,WORD PTR[bp+8]	;lpSrc Segment
	push    WORD PTR [bp+32]
	push	WORD PTR [bp+30]	;Rrange
	push	WORD PTR [bp-16]
	push	WORD PTR [bp-18]	;rate
	call	FAR PTR __aFlmul
	mov	ax,dx
	add	ax,WORD PTR [bp+24]	; add startR
	add	ax,WORD PTR [bp-4]	; add r
	or	ax,ax			; < 0
	jl	PV24_13			; yes
	cmp	ax,255			; > 255
	jle	PV24_15			; nope
	mov	al,255			; yes, set to 255
	jmp	PV24_15
PV24_13:
	sub	al,al
PV24_15:
	stosb				; store Red
	push	WORD PTR [bp+36]
	push	WORD PTR [bp+34]	;Grange
	push	WORD PTR [bp-16]
	push	WORD PTR [bp-18]	;rate
	call	FAR PTR __aFlmul
	mov	ax,dx
	add	ax,WORD PTR [bp+26]	; add startG
	add	ax,WORD PTR [bp-4]	; add r 
	or	ax,ax			; green < 0
	jl	PV24_16			; yes
	cmp	ax,255			; green > 255
	jle	PV24_18			; nope
	mov	al,255			; yes, set to 255
	jmp	PV24_18
PV24_16:
	sub	al,al
PV24_18:
	stosb
	push	WORD PTR [bp+40]
	push	WORD PTR [bp+38]	;Brange
	push	WORD PTR [bp-16]
	push	WORD PTR [bp-18]	;rate
	call	FAR PTR __aFlmul
	mov	ax,dx
	add	ax,WORD PTR [bp+28]	; add startB
	add	ax,WORD PTR [bp-4]	; add r
	or	ax,ax			; blue < 0
	jl	PV24_20			; yes
	cmp	ax,255			; blue > 255
	jle	PV24_21			; nope
	mov	al,255			; yes, set to 255
	jmp	PV24_21
PV24_20:
	sub	al,al
PV24_21:
	stosb
PV24_22:
	dec	WORD PTR[bp+10]		; iCount
	js	PV24_100
	jmp	PV24_1
PV24_100:
	mov	ax,WORD PTR[bp-26]	; lSeed
	mov	WORD PTR lSeed,ax
	mov	ax,WORD PTR[bp-24]	; lSeed+2
	mov	WORD PTR lSeed+2,ax
	pop	si
	pop	di
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
_ProcessVignette24	ENDP
ENDIF

IFNDEF ASM_CODE
;/************************************************************************/
;void ProcessVignette8(lpSrc, iCount, lpd, lpD, nRepetitions, SoftTransition, ;startR, startG, startB, Rrange, Grange, Brange )
;/************************************************************************/
;LPTR lpSrc;
;int iCount;
;LPLONG lpd;
;LPLONG lpD;
;int nRepetitions;
;BOOL SoftTransition;
;int startR, startG, startB;
;long Rrange, Grange, Brange;
	PUBLIC	_ProcessVignette8
_ProcessVignette8	PROC FAR
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
	push	di
	push	si
;
;	lpSrc = 6
;	iCount = 10
;	lpd = 12
;	lpD = 16
;	nRepetitions = 20
;	SoftTransition = 22
;	startR = 24
;	startG = 26
;	startB = 28
;	Rrange = 30
;	Grange = 34
;	Brange = 38
;	d = -14
;	D = -10
;	Rvalue = -22
;	Gvalue = -6
;	Bvalue = -20
;	r = -4
;	rate = -18
;	lSeed+2 = -24 - HIWORD
;	lSeed = -26   - LOWORD
;
	mov	ax,WORD PTR lSeed
	mov	WORD PTR [bp-26],ax
	mov	ax,WORD PTR lSeed+2
	mov	WORD PTR [bp-24],ax
	mov	di,WORD PTR [bp+6]	; es:[di] = lpSrc
	cld
	jmp	PV8_22
PV8_1:
	les	si,DWORD PTR [bp+12]	;lpd
	add	WORD PTR [bp+12],4	;lpd
	mov	bx,WORD PTR es:[si]	;BX/DX = d
	mov	dx,WORD PTR es:[si+2]
	les	si,DWORD PTR [bp+16]	;lpD
	add	WORD PTR [bp+16],4	;lpD
	mov	cx,WORD PTR es:[si]	;cx = D
	cmp	dx,0			; is d negative?
	jge	PV8_2
	mov	WORD PTR [bp-4],0	;r
	sub	bx,bx			; BX = d = 0
	jmp	SHORT PV8_6
	nop	
PV8_2:					; positive d
	cmp	bx,cx			; d > D
	jb	PV8_4
PV8_3:
	mov	WORD PTR [bp-4],0	;r
	mov	bx,cx			;SI = d = D
	jmp	SHORT PV8_6
PV8_4:
	mov	ax,WORD PTR [bp-26]	; lSeed
	mov	dx,WORD PTR [bp-24]	; lSeed+2
	add	ax,ax			; lSeed += lSeed
	adc	WORD PTR [bp-24],dx
	test	dx,2			; if (lSeed & BIT28)
	je	PV8_7
	xor	ax,39			; lSeed ^= BITMASK
PV8_7:
	mov	WORD PTR [bp-26],ax
	and	ax,7
	sub	ax,3
	mov	WORD PTR [bp-4],ax	;r
PV8_6:
	cmp	WORD PTR [bp+20],1	;nRepetitions
	jne	PV8_9
	sub	ax,ax			;
	push	ax			; no hiword of D
	push	cx			; LOWORD(D)
	push	bx			; d<<16
	push	ax			; no loword of d<<16
	call	FAR PTR __aFldiv
PV8_8:
	mov	WORD PTR [bp-18],ax	;rate
	mov	WORD PTR [bp-16],dx
	jmp	SHORT PV8_12
	nop	
PV8_9:
	sub	ax,ax
	push	ax			; no hiword of D
	push	cx			; D
	push	ax			; no hiword of d
	push	bx			; d
	mov	ax,WORD PTR [bp+20]	;nRepetitions
	cwd	
	push	dx
	push	ax
	call	FAR PTR __aFlmul
	push	dx
	push	ax
	call	FAR PTR _fget
	add	sp,8
	mov	WORD PTR [bp-18],ax	;rate
	mov	WORD PTR [bp-16],dx
	cmp	dx,1
	jl	PV8_12
	jg	PV8_10
	or	ax,ax
	je	PV8_12
PV8_10:
	sub	WORD PTR [bp-18],1	;rate
	sbb	WORD PTR [bp-16],0
	cmp	WORD PTR [bp+22],0	;SoftTransition
	je	PV8_11
	test	BYTE PTR [bp-16],1
	je	PV8_11
	mov	ax,WORD PTR [bp-18]	;rate
	sub	dx,dx
	sub	ax,dx
	sbb	dx,1
	neg	ax
	adc	dx,0
	neg	dx
	jmp	SHORT PV8_8
PV8_11:
	sub	WORD PTR [bp-18],1	;rate
	sbb	WORD PTR [bp-16],0
	mov	ax,WORD PTR [bp-18]	;rate
	mov	WORD PTR [bp-16],0
	add	WORD PTR [bp-18],1	;rate
	adc	WORD PTR [bp-16],0
PV8_12:
	mov	es,WORD PTR[bp+8]	;lpSrc Segment
	push	WORD PTR [bp+32]
	push	WORD PTR [bp+30]	;Rrange
	push	WORD PTR [bp-16]
	push	WORD PTR [bp-18]	;rate
	call	FAR PTR __aFlmul
	mov	ax,dx
	add	ax,WORD PTR [bp+24]	; add startR
	add	ax,WORD PTR [bp-4]	; add r
	or	ax,ax			; < 0
	jl	PV8_13			; yes
	cmp	ax,255			; > 255
	jle	PV8_15			; nope
	mov	al,255			; yes, set to 255
	jmp	PV8_15
PV8_13:
	sub	al,al
PV8_15:
	stosb				; store Red
PV8_22:
	dec	WORD PTR[bp+10]		; iCount
	js	PV8_100
	jmp	PV8_1
PV8_100:
	mov	ax,WORD PTR[bp-26]	; lSeed
	mov	WORD PTR lSeed,ax
	mov	ax,WORD PTR[bp-24]	; lSeed+2
	mov	WORD PTR lSeed+2,ax
	pop	si
	pop	di
	sub	bp,2
	mov	sp,bp
	pop	ds
	pop	bp
	dec	bp
	ret	
_ProcessVignette8	ENDP
ENDIF

END
