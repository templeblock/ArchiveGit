;	Static Name Aliases
;
IFNDEF C_CODE
	TITLE   qmul.asm
	.386p
	.387

_DATA	SEGMENT  USE16 WORD PUBLIC 'DATA'
_DATA	ENDS

CONST	SEGMENT  USE16 WORD PUBLIC 'CONST'
CONST	ENDS

_BSS	SEGMENT  USE16 WORD PUBLIC 'BSS'
_BSS	ENDS

DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME DS: DGROUP, SS: DGROUP, CS: QMUL_TEXT

QMUL_TEXT SEGMENT  USE16 WORD PUBLIC 'CODE'

;
; void Convert8to8ASM(LPTR lpSrc, LPTR lpDst, LPTR lpLut, int iCount);
;

	PUBLIC	_Convert8to8ASM

_Convert8to8ASM	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

; Sanity Certify
	push	ds
	push	es
	push	fs
	push	di
	push	si

	lds	si, DWORD PTR[bp+ 6]	; lpSrc
	les	di, DWORD PTR[bp+10]	; lpDst
	lfs	bx, DWORD PTR[bp+14]	; lpLut
	mov	cx, WORD  PTR[bp+18]	; iCount

	cmp	cx, 0			; Anything to do?
	jle	CNV8to8D		; Nope, give up...
	cld				; Make sure we go the right way...
CNV8to8L:
	xor	ax,ax			; Clear AX
	lodsb				; Get *lpSrc++ -> AH
	add	bx,ax			; Add index
	mov	dx,ax			; Save index
	mov	ax,fs:[bx]		; Get LUT value
	sub	bx,dx			; Sub index
	stosb				; Put AH -> *lpDst++

	dec	cx
	jnz	CNV8to8L	
CNV8to8D:

; Sanity Check
	pop	si
	pop	di
	pop	fs
	pop	es
	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_Convert8to8ASM	ENDP

;
; void Convert8to8ASM2(LPTR lpSrc, LPTR lpDst, LPTR lpLut, int iCount);
;

	PUBLIC	_Convert8to8ASM2

_Convert8to8ASM2	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

	push	ds
	push	es
	push	fs
	push	di
	push	si

	lds	si, DWORD PTR[bp+ 6]	; lpSrc
	les	di, DWORD PTR[bp+10]	; lpDst
	lfs	bx, DWORD PTR[bp+14]	; lpLut
	mov	cx, WORD  PTR[bp+18]	; iCount

	cmp	cx, 0			; Anything to do?
	jle	CNV8to82D		; Nope, give up...
	cld				; Make sure we go the right way...
CNV8to82L:
	xor	ax,ax			; Clear AX
	lodsb				; Get *lpSrc++ -> AH
	shr	ax, 4			; Divide index by 16
	add	bx,ax			; Add index
	mov	dx,ax			; Save index
	mov	ax, fs:[bx]		; Get LUT value
	sub	bx,dx			; Sub index
	stosb				; Put AH -> *lpDst++

	dec	cx
	jnz	CNV8to82L	
CNV8to82D:
	pop	si
	pop	di
	pop	fs
	pop	es
	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_Convert8to8ASM2	ENDP

;
; long quick_lmul(long num1, long num2) = num1 * num2
;

	PUBLIC	_quick_lmul

_quick_lmul	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

	mov	edx, DWORD PTR[bp+ 6] ; num1
	mov	eax, DWORD PTR[bp+10] ; num2
	imul	eax, edx
	shld	edx, eax, 16

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_quick_lmul	ENDP

;
; long quick_lmul2(long num1, long num2, long num3, long num4) =
;	(num1 * num2) + (num3 * num4)
;

	PUBLIC	_quick_lmul2

_quick_lmul2	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

	mov	edx, DWORD PTR[bp+ 6] ; num1
	mov	eax, DWORD PTR[bp+10] ; num2
	imul	eax, edx
	mov	ebx, eax

	mov	edx, DWORD PTR[bp+14] ; num3
	mov	eax, DWORD PTR[bp+18] ; num4
	imul	eax, edx
	add	eax, ebx

	shld	edx, eax, 16

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_quick_lmul2	ENDP

;
; short quick_lmul4shr16(
;	long num1, long num2, long num3, long num4,
;	long num5, long num6, long num7, long num8) =
;    ((num1 * num2) + (num3 * num4) + (num5 * num6) + (num7 * num8)) >> 16
;

	PUBLIC	_quick_lmul4shr16

_quick_lmul4shr16	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

	mov	edx, DWORD PTR[bp+ 6] ; num1
	mov	eax, DWORD PTR[bp+10] ; num2
	imul	eax, edx
	mov	ebx, eax

	mov	edx, DWORD PTR[bp+14] ; num3
	mov	eax, DWORD PTR[bp+18] ; num4
	imul	eax, edx
	add	ebx, eax

	mov	edx, DWORD PTR[bp+22] ; num5
	mov	eax, DWORD PTR[bp+26] ; num6
	imul	eax, edx
	add	ebx, eax

	mov	edx, DWORD PTR[bp+30] ; num7
	mov	eax, DWORD PTR[bp+34] ; num8
	imul	eax, edx
	add	eax, ebx

	shr	eax, 16

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_quick_lmul4shr16	ENDP

;
; short quick_average8(LPTR lpBuf1, LPTR lpBuf2, long area1, long area2, long area3, long area4, LPTR lpDst) =
;
;	(lpBuf1[0] * area1 +
;	 lpBuf1[1] * area2 +
;	 lpBuf2[0] * area3 +
;	 lpBuf2[1] * area4) >> 16
;

	PUBLIC	_quick_average8

_quick_average8	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

	push	ds
	push	es
	push	fs

	lds	si, DWORD PTR[bp+ 6]	; lpBuf1
	les	di, DWORD PTR[bp+30]	; lpDst
	lfs	bx, DWORD PTR[bp+10]	; lpBuf2

	mov	edx, DWORD PTR[bp+14]	; area1
	xor	eax, eax
	mov     al,  ds:[si]		; lpBuf1[0]
	imul	eax, edx
	mov	ecx, eax

	mov	edx, DWORD PTR[bp+18]	; area2
	xor	eax, eax
	mov	al,  ds:[si+1]		; lpBuf1[1]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+22]	; area3
	xor	eax, eax
	mov	al,  fs:[bx]		; lpBuf2[0]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+26]	; area4
	xor	eax, eax
	mov	al,  fs:[bx+1]		; lpBuf2[1]
	imul	eax, edx
	add	eax, ecx

	shr	eax, 16			; >> 16
	
	stosb				; Write the value back

	pop	fs
	pop	es
	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_quick_average8	ENDP

;
; short quick_average24(LPTR lpBuf1, LPTR lpBuf2, long area1, long area2, long area3, long area4, LPTR lpDst) =
;
;	lpDst[0] = (
;		lpBuf1[0] * area1 +
;		lpBuf1[3] * area2 +
;		lpBuf2[0] * area3 +
;	 	lpBuf2[3] * area4) >> 16
;
;	lpDst[1] = (
;		lpBuf1[1] * area1 +
;		lpBuf1[4] * area2 +
;		lpBuf2[1] * area3 +
;	 	lpBuf2[4] * area4) >> 16
;
;	lpDst[2] = (
;		lpBuf1[2] * area1 +
;		lpBuf1[5] * area2 +
;		lpBuf2[2] * area3 +
;	 	lpBuf2[5] * area4) >> 16
;

	PUBLIC	_quick_average24

_quick_average24	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

	push	ds
	push	es
	push	fs

	lds	si, DWORD PTR[bp+ 6]	; lpBuf1
	les	di, DWORD PTR[bp+30]	; lpDst
	lfs	bx, DWORD PTR[bp+10]	; lpBuf2

; Do Red Channel

	mov	edx, DWORD PTR[bp+14]	; area1
	xor	eax, eax
	mov	al,  ds:[si]		; lpBuf1[0]
	imul	eax, edx
	mov	ecx, eax

	mov	edx, DWORD PTR[bp+18]	; area2
	xor	eax, eax
	mov	al,  ds:[si+3]		; lpBuf1[3]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+22]	; area3
	xor	eax, eax
	mov	al,  fs:[bx]		; lpBuf2[0]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+26]	; area4
	xor	eax, eax
	mov	al,  fs:[bx+3]		; lpBuf2[3]
	imul	eax, edx
	add	eax, ecx
	shr	eax, 16			; >> 16
	stosb				; Write the value back

; Do Green Channel

	mov	edx, DWORD PTR[bp+14]	; area1
	xor	eax, eax
	mov	al,  ds:[si+1]		; lpBuf1[1]
	imul	eax, edx
	mov	ecx, eax

	mov	edx, DWORD PTR[bp+18]	; area2
	xor	eax, eax
	mov	al,  ds:[si+4]		; lpBuf1[4]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+22]	; area3
	xor	eax, eax
	mov	al,  fs:[bx+1]		; lpBuf2[1]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+26]	; area4
	xor	eax, eax
	mov	al,  fs:[bx+4]		; lpBuf2[4]
	imul	eax, edx
	add	eax, ecx
	shr	eax, 16			; >> 16
	stosb				; Write the value back

; Do Blue Channel

	mov	edx, DWORD PTR[bp+14]	; area1
	xor	eax, eax
	mov	al,  ds:[si+2]		; lpBuf1[2]
	imul	eax, edx
	mov	ecx, eax

	mov	edx, DWORD PTR[bp+18]	; area2
	xor	eax, eax
	mov	al,  ds:[si+5]		; lpBuf1[5]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+22]	; area3
	xor	eax, eax
	mov	al,  fs:[bx+2]		; lpBuf2[2]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+26]	; area4
	xor	eax, eax
	mov	al,  fs:[bx+5]		; lpBuf2[5]
	imul	eax, edx
	add	eax, ecx
	shr	eax, 16			; >> 16
	stosb				; Write the value back

	pop	fs
	pop	es
	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_quick_average24	ENDP

;
; short quick_average32(LPTR lpBuf1, LPTR lpBuf2, long area1, long area2, long area3, long area4, LPTR lpDst) =
;
;	lpDst[0] = (
;		lpBuf1[0] * area1 +
;		lpBuf1[4] * area2 +
;		lpBuf2[0] * area3 +
;	 	lpBuf2[4] * area4) >> 16
;
;	lpDst[1] = (
;		lpBuf1[1] * area1 +
;		lpBuf1[5] * area2 +
;		lpBuf2[1] * area3 +
;	 	lpBuf2[5] * area4) >> 16
;
;	lpDst[2] = (
;		lpBuf1[2] * area1 +
;		lpBuf1[6] * area2 +
;		lpBuf2[2] * area3 +
;	 	lpBuf2[6] * area4) >> 16
;
;	lpDst[3] = (
;		lpBuf1[3] * area1 +
;		lpBuf1[7] * area2 +
;		lpBuf2[3] * area3 +
;	 	lpBuf2[7] * area4) >> 16
;

	PUBLIC	_quick_average32

_quick_average32	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

	push	ds
	push	es
	push	fs

	lds	si, DWORD PTR[bp+ 6]	; lpBuf1
	les	di, DWORD PTR[bp+30]	; lpDst
	lfs	bx, DWORD PTR[bp+10]	; lpBuf2

; Do Cyan Channel

	mov	edx, DWORD PTR[bp+14]	; area1
	xor	eax, eax
	mov	al,  ds:[si]		; lpBuf1[0]
	imul	eax, edx
	mov	ecx, eax

	mov	edx, DWORD PTR[bp+18]	; area2
	xor	eax, eax
	mov	al,  ds:[si+4]		; lpBuf1[4]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+22]	; area3
	xor	eax, eax
	mov	al,  fs:[bx]		; lpBuf2[0]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+26]	; area4
	xor	eax, eax
	mov	al,  fs:[bx+4]		; lpBuf2[4]
	imul	eax, edx
	add	eax, ecx
	shr	eax, 16			; >> 16
	stosb				; Write the value back

; Do Magenta Channel

	mov	edx, DWORD PTR[bp+14]	; area1
	xor	eax, eax
	mov	al,  ds:[si+1]		; lpBuf1[1]
	imul	eax, edx
	mov	ecx, eax

	mov	edx, DWORD PTR[bp+18]	; area2
	xor	eax, eax
	mov	al,  ds:[si+5]		; lpBuf1[5]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+22]	; area3
	xor	eax, eax
	mov	al,  fs:[bx+1]		; lpBuf2[1]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+26]	; area4
	xor	eax, eax
	mov	al,  fs:[bx+5]		; lpBuf2[5]
	imul	eax, edx
	add	eax, ecx
	shr	eax, 16			; >> 16
	stosb				; Write the value back

; Do Yellow Channel

	mov	edx, DWORD PTR[bp+14]	; area1
	xor	eax, eax
	mov	al,  ds:[si+2]		; lpBuf1[2]
	imul	eax, edx
	mov	ecx, eax

	mov	edx, DWORD PTR[bp+18]	; area2
	xor	eax, eax
	mov	al,  ds:[si+6]		; lpBuf1[6]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+22]	; area3
	xor	eax, eax
	mov	al,  fs:[bx+2]		; lpBuf2[2]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+26]	; area4
	xor	eax, eax
	mov	al,  fs:[bx+6]		; lpBuf2[6]
	imul	eax, edx
	add	eax, ecx
	shr	eax, 16			; >> 16
	stosb				; Write the value back

; Do Black Channel

	mov	edx, DWORD PTR[bp+14]	; area1
	xor	eax, eax
	mov	al,  ds:[si+3]		; lpBuf1[3]
	imul	eax, edx
	mov	ecx, eax

	mov	edx, DWORD PTR[bp+18]	; area2
	xor	eax, eax
	mov	al,  ds:[si+7]		; lpBuf1[7]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+22]	; area3
	xor	eax, eax
	mov	al,  fs:[bx+3]		; lpBuf2[3]
	imul	eax, edx
	add	ecx, eax

	mov	edx, DWORD PTR[bp+26]	; area4
	xor	eax, eax
	mov	al,  fs:[bx+7]		; lpBuf2[7]
	imul	eax, edx
	add	eax, ecx
	shr	eax, 16			; >> 16
	stosb				; Write the value back

	pop	fs
	pop	es
	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_quick_average32	ENDP

QMUL_TEXT ENDS

ENDIF
END

