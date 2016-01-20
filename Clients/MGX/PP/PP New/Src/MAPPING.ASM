;	Static Name Aliases
;
	TITLE   mapping.asm
	.386p
	.387

_DATA	SEGMENT  USE16 WORD PUBLIC 'DATA'
_DATA	ENDS

CONST	SEGMENT  USE16 WORD PUBLIC 'CONST'
CONST	ENDS

_BSS	SEGMENT  USE16 WORD PUBLIC 'BSS'
_BSS	ENDS

DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME CS: MAPPING_TEXT, DS: DGROUP, SS: DGROUP

MAPPING_TEXT SEGMENT  USE16 WORD PUBLIC 'CODE'


IFDEF UNUSED
/************************************************************************/
static void MapLine8(LPTR lpMLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData = lpMLut[ *lpData ];
		*lpData++;
	}
}
ENDIF

;
;static void MapLine8(LPTR lpMLut, LPTR lpData, int count)
;
	PUBLIC	_MapLine8

_MapLine8	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

; Sanity Certify
;	push	ds
	push	es
	push	fs
	push	di
	push	si

	lds	si, DWORD PTR[bp+ 6]	; lpMLut
	les	di, DWORD PTR[bp+10]	; lpData
	mov	cx, WORD  PTR[bp+14]	; iCount

	cmp	cx, 0			; Anything to do?
	jle	Done8			; Nope, give up...
	cld					; Make sure we go the right way...
Loop8:
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get *lpData -> BX
	xor bl,255		; Get 255 - *lpData
	mov al,ds:[bx+si]	; Get lpMLut[255-*lpData] -> AL
	xor al,255		; Get 255-lpMLut[255-*lpData]
	stosb			; Put AL -> *lpDst++
	dec	cx
	jnz	Loop8
Done8:

; Sanity Check
	pop	si
	pop	di
	pop	fs
	pop	es
;	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_MapLine8	ENDP


IFDEF UNUSED
/************************************************************************/
static void MapLine8by2(LPTR lpMLut, LPTR lpLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData = lpLut[ 255-lpMLut[255-*lpData] ];
		*lpData++;
	}
}
ENDIF


;
;void MapLine8by2(LPTR lpMLut, LPTR lpLut, LPTR lpData, int count)
;
	PUBLIC	_MapLine8by2

_MapLine8by2	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

; Sanity Certify
;	push	ds
	push	es
	push	fs
	push	di
	push	si

	lds	si, DWORD PTR[bp+ 6]	; lpMLut
	lfs	bx, DWORD PTR[bp+10]	; lpLut
	les	di, DWORD PTR[bp+14]	; lpData
	mov	cx, WORD  PTR[bp+18]	; iCount

	cmp	cx, 0			; Anything to do?
	jle	Done82			; Nope, give up...
	add bx, 255			; get &lpLut[255]
	add si, 255			; get &lpMLut[255]
	mov dx, bx			; store bx
	cld					; Make sure we go the right way...
Loop82:
						; does : *lpData = lpLut[ 255-lpMLut[255-*lpData] ];
	xor ax, ax			; clear AX
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get (-*lpData) -> BX
	neg bx				 
	mov	al,ds:[bx+si]	; Get -lpMLut[255-BX] -> AL
	neg ax				
	mov	bx, dx			; retore BX	with &lpLut[255]
	add bx, ax			; get lpLut[255-AL]
	mov al,fs:[bx]		
	stosb				; Put AL -> *lpData++
	dec	cx
	jnz	Loop82	
Done82:

; Sanity Check
	pop	si
	pop	di
	pop	fs
	pop	es
;	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_MapLine8by2	ENDP



IFDEF UNUSED
// WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
/************************************************************************/
void MapLine24(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData   = lpRLut[ 255-lpMLut[255-*lpData]   ];
		*lpData++;
		*lpData   = lpGLut[ 255-lpMLut[255-*lpData]   ];
		*lpData++;
		*lpData   = lpBLut[ 255-lpMLut[255-*lpData]   ];
		*lpData++;
	}
}
ENDIF

; WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
;
; void MapLine24(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpData, int count)
;
	PUBLIC	_MapLine24

_MapLine24	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

; Sanity Certify
;	push	ds
	push	es
	push	fs
	push	di
	push	si

	; WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
	lds	si, DWORD PTR[bp+ 6]	; &lpMLut[255] -> ds:si
	add si, 255			
	lfs	bx, DWORD PTR[bp+10]	; &lpRLut[255] -> fs:dx
	add bx, 255
	mov dx, bx	
	lfs	bx, DWORD PTR[bp+14]	; &lpGLut[255] -> fs:ax
	add bx, 255
	mov ax, bx					 
	lfs	bx, DWORD PTR[bp+18]	; &lpBLut[255] -> fs:bp
	add bx, 255
	les	di, DWORD PTR[bp+22]	; lpData
	mov	cx, WORD  PTR[bp+26]	; iCount
	mov bp, bx					; waited to use bp till done with vars		 

	cmp	cx, 0			; Anything to do?
	jle	Done24			; Nope, give up...
	cld					; Make sure we go the right way...
Loop24:
	
	; *lpData = lpRLut[ 255-lpMLut[255-*lpData] ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get (-*lpData) -> BX
	neg bx				 
	mov	bl,ds:[bx+si]	; Get -lpMLut[255-BX] -> BL
	and bx, 00ffh
	neg bx				
	add bx, dx			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	mov es:[di], bl		; Put BL -> *lpData++
	inc di

	; *lpData = lpGLut[ 255-lpMLut[255-*lpData] ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get (-*lpData) -> BX
	neg bx				 
	mov	bl,ds:[bx+si]	; Get -lpMLut[255-BX] -> BL
	and bx, 00ffh
	neg bx				
	add bx, ax			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	mov es:[di], bl		; Put BL -> *lpData++
	inc di

	; *lpData = lpBLut[ 255-lpMLut[255-*lpData] ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get (-*lpData) -> BX
	neg bx				 
	mov	bl,ds:[bx+si]	; Get -lpMLut[255-BX] -> BL
	and bx, 00ffh
	neg bx				
	add bx, bp			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	mov es:[di], bl		; Put BL -> *lpData++
	inc di

	dec	cx
	jnz	Loop24	
Done24:

; Sanity Check
	pop	si
	pop	di
	pop	fs
	pop	es
;	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_MapLine24	ENDP


IFDEF UNUSED
// WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
/************************************************************************/
void MapLine32(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData   = 255 - lpRLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = 255 - lpGLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = 255 - lpBLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = lpKLut[*lpData];
		*lpData++;
	}
}
ENDIF

; WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
;
; void MapLine32(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpKLut, LPTR lpData, int count)
;
	PUBLIC	_MapLine32

_MapLine32	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

; Sanity Certify
;	push	ds
	push	es
	push	fs
	push	di
	push	si

	; WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
	lds	si, DWORD PTR[bp+ 6]	; lpMLut -> ds:si
	lfs	bx, DWORD PTR[bp+10]	; &lpRLut[255] -> fs:dx
	add bx, 255
	mov dx, bx	
	lfs	bx, DWORD PTR[bp+14]	; &lpGLut[255] -> fs:ax[HI WORD]
	add bx, 255
	mov ax, bx					 
	
	lfs	bx, DWORD PTR[bp+18]	; &lpBLut[255] -> fs:ax[LO WORD]
	add bx, 255
	rol eax, 16					; shift lpGLut to HI WORD of ax
	mov ax, bx					 
	
	lfs	bx, DWORD PTR[bp+22]	; &lpKLut[0] -> fs:bp
	
	les	di, DWORD PTR[bp+26]	; lpData
	mov	cx, WORD  PTR[bp+30]	; iCount
	mov bp, bx					; waited to use bp			 

	cmp	cx, 0			; Anything to do?
	jle	Done32			; Nope, give up...
	cld					; Make sure we go the right way...
Loop32:
	
	; *lpData = 255 - lpRLut[ 255-lpMLut[*lpData] ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get *lpData -> BX
	mov	bl,ds:[bx+si]	; Get -lpMLut[BX] -> AL
	neg bx				
	add bx, dx			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	not bl				; get 255-al
	mov es:[di], bl		; Put BL -> *lpData++
	inc di

	; *lpData = 255 - lpGLut[ 255-lpMLut[*lpData] ];
	rol eax, 16			; shift lpGLut to LO WORD of ax
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get *lpData -> BX
	mov	bl,ds:[bx+si]	; Get -lpMLut[BX] -> AL
	neg bx				
	add bx, ax			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	not bl				; get 255-al
	mov es:[di], bl		; Put BL -> *lpData++
	inc di
	rol eax, 16			; shift lpGLut back to HI WORD of ax

	; *lpData = 255 - lpBLut[ 255-lpMLut[*lpData] ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get *lpData -> BX
	mov	bl,ds:[bx+si]	; Get -lpMLut[BX] -> AL
	neg bx				
	add bx, ax			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	not bl				; get 255-al
	mov es:[di], bl		; Put BL -> *lpData++
	inc di

	; *lpData   = lpKLut[lpMLut[*lpData]   ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get (*lpData) -> BX
	and bx, 00ffh
	add bx, bp			; get lpLut[BL]
	mov bl,fs:[bx]		
	mov es:[di], bl		; Put BL -> *lpData++
	inc di
	
	dec	cx
	jnz	Loop32	
Done32:

; Sanity Check
	pop	si
	pop	di
	pop	fs
	pop	es
;	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_MapLine32	ENDP

IFDEF UNUSED
// WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
/************************************************************************/
void MapLine32MToK(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpData, int count)
/************************************************************************/
{
	while (count--) 
	{
		*lpData   = 255 - lpRLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = 255 - lpGLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = 255 - lpBLut[ 255-lpMLut[*lpData] ];
		*lpData++;
		*lpData   = lpKLut[lpMLut[*lpData]   ];
		*lpData++;
	}
}
ENDIF

; WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
;
; void MapLine32MToK(LPTR lpMLut,LPTR lpRLut,LPTR lpGLut, LPTR lpBLut, LPTR lpKLut, LPTR lpData, int count)
;
	PUBLIC	_MapLine32MToK

_MapLine32MToK	PROC FAR	;  COMDAT
	mov	ax,ds
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax

; Sanity Certify
;	push	ds
	push	es
	push	fs
	push	di
	push	si

	; WARNING ASSUMES lpRLut, lpGLut & lpBLut are in same segment
	lds	si, DWORD PTR[bp+ 6]	; lpMLut -> ds:si
	lfs	bx, DWORD PTR[bp+10]	; &lpRLut[255] -> fs:dx
	add bx, 255
	mov dx, bx	
	lfs	bx, DWORD PTR[bp+14]	; &lpGLut[255] -> fs:ax[HI WORD]
	add bx, 255
	mov ax, bx					 
	
	lfs	bx, DWORD PTR[bp+18]	; &lpBLut[255] -> fs:ax[LO WORD]
	add bx, 255
	rol eax, 16					; shift lpGLut to HI WORD of ax
	mov ax, bx					 
	
	lfs	bx, DWORD PTR[bp+22]	; &lpKLut[0] -> fs:bp
	
	les	di, DWORD PTR[bp+26]	; lpData
	mov	cx, WORD  PTR[bp+30]	; iCount
	mov bp, bx					; waited to use bp			 

	cmp	cx, 0			; Anything to do?
	jle	Done32MTK			; Nope, give up...
	cld					; Make sure we go the right way...
Loop32MTK:
	
	; *lpData = 255 - lpRLut[ 255-lpMLut[*lpData] ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get *lpData -> BX
	mov	bl,ds:[bx+si]	; Get -lpMLut[BX] -> AL
	neg bx				
	add bx, dx			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	not bl				; get 255-al
	mov es:[di], bl		; Put BL -> *lpData++
	inc di

	; *lpData = 255 - lpGLut[ 255-lpMLut[*lpData] ];
	rol eax, 16			; shift lpGLut to LO WORD of ax
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get *lpData -> BX
	mov	bl,ds:[bx+si]	; Get -lpMLut[BX] -> AL
	neg bx				
	add bx, ax			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	not bl				; get 255-al
	mov es:[di], bl		; Put BL -> *lpData++
	inc di
	rol eax, 16			; shift lpGLut back to HI WORD of ax

	; *lpData = 255 - lpBLut[ 255-lpMLut[*lpData] ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get *lpData -> BX
	mov	bl,ds:[bx+si]	; Get -lpMLut[BX] -> AL
	neg bx				
	add bx, ax			; get lpLut[255-AL]
	mov bl,fs:[bx]		
	not bl				; get 255-al
	mov es:[di], bl		; Put BL -> *lpData++
	inc di

	; *lpData   = lpKLut[*lpData ];
	xor bx, bx			; clear BX
	mov bl,es:[di]		; Get (*lpData) -> BX
	mov	bl,ds:[bx+si]	; Get lpMLut[BX] -> BL
	and bx, 00ffh
	add bx, bp			; get lpLut[BL]
	mov bl,fs:[bx]		
	mov es:[di], bl		; Put BL -> *lpData++
	inc di
	
	dec	cx
	jnz	Loop32MTK	
Done32MTK:

; Sanity Check
	pop	si
	pop	di
	pop	fs
	pop	es
;	pop	ds

	pop	ds
	ASSUME  DS: DGROUP
	pop	bp
	dec	bp
	ret	

_MapLine32MToK	ENDP

MAPPING_TEXT ENDS

END


