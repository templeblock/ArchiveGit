PAGE 80,132
TITLE - MEASM.ASM - Assembly routines for 3D engine
;;;;.286P
;==============================================================================
; COMMAND LINE ASSEMBLY
;	MASM /B63 /Z /D_Mx FILENAME;
;
;  WHERE Mx SPECIFIES MODEL (l OR c) FOR LARGE OR COMPACT MODELS
;==============================================================================
	INCLUDE		ET.MAC

	PUBLIC		_SetPalette
	PUBLIC		_SetPalette2
	PUBLIC		_SetVGAmode
	PUBLIC		_inkey

_TEXT	segment byte public 'CODE'
DGROUP	group	_DATA,_BSS
		assume	cs:_TEXT,ds:DGROUP
_TEXT	ends
_DATA	segment word public 'DATA'
_d@		label	byte
_DATA	ends
_BSS	segment word public 'BSS'
_b@		label	byte
_BSS	ends


_TEXT	SEGMENT byte public 'CODE'
		ASSUME cs:_TEXT

;==============================================================================
; void SetPalette(unsigned char far *PalBuf);
;==============================================================================
SPbuf	equ	[bp+ABASE]

PBEGIN	_SetPalette
	push	bp
	mov		bp,sp
	push	ds
	push	si

	lds		si,dword ptr SPbuf
	mov		cx,256
	xor		bx,bx
	cld
	mov		dx,3C6H
	mov		al,0FFH
	out		dx,al
	mov		dx,3C8H
sp010:
	mov		al,bl
	out		dx,al
	inc		dx
	lodsb
	out		dx,al
	lodsb
	out		dx,al
	lodsb
	out		dx,al
	dec		dx
	inc		bx
	loop	sp010

	pop		si
	pop		ds
	pop		bp
	ret
_SetPalette endp


;==============================================================================
; void SetPalette2(unsigned char far *PalBuf);
;==============================================================================
SP2buf	equ	[bp+ABASE]

PBEGIN	_SetPalette2
	push	bp
	mov		bp,sp
	push	ds
	push	si

	lds		si,dword ptr SP2buf
	mov		cx,256	;246	;240	;jcl
	mov		bx,0	;10		;mov	bx,16	;jcl
	cld
	mov		dx,3C6H
	mov		al,0FFH
	out		dx,al
	mov		dx,3C8H
sp210:
	mov		al,bl
	out		dx,al
	inc		dx
	lodsb
	out		dx,al
	lodsb
	out		dx,al
	lodsb
	out		dx,al
	dec		dx
	inc		bx
	loop	sp210

	pop		si
	pop		ds
	pop		bp
	ret
_SetPalette2 endp



;==============================================================================
; void SetVGAmode(void);
;==============================================================================
PBEGIN	_SetVGAmode
	push	bp
	mov		ax,13h
	int		10h		; Set 320x200x256
	pop		bp
	ret
_SetVGAmode endp

;==============================================================================
;
;==============================================================================
PBEGIN	_inkey
	mov		ah,1		;see if key available
	int		16h
	jz		ink080		;nope
	xor		ax,ax
	int		16h
	jmp		short ink090

ink080:
	xor		ax,ax
ink090:
	ret
_inkey	endp

_TEXT	ENDS
	END

