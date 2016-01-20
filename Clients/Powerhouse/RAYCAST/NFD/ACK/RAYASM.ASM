PAGE 80,132
TITLE - RAYASM.ASM - Assembly routine for 3D engine
.386P
	OPTION		OLDSTRUCTS
;==============================================================================
; COMMAND LINE ASSEMBLY
;	MASM /B63 /Z /D_ML FILENAME;
;				OR
;	ML /D_ML /H64 /c FILENAME.ASM
;
;  WHERE ML SPECIFIES LARGE MEMORY MODEL
;==============================================================================
	INCLUDE		ET.MAC

	PUBLIC		?Ray@@ZAIPEI0HHJJ@Z

_TEXT	segment byte public use16 'CODE'
_TEXT	ends

_DATA	segment word public use16 'DATA'
_DATA	ends

_CONST	segment word public use16 'CONST'
_CONST	ends

_BSS	segment word public use16 'BSS'
_BSS	ends

CONST	segment word public use16 'CONST'
CONST	ENDS

_BSS	segment
PUBLIC  ?LastWallHit@@3IE
?LastWallHit@@3IE	DW 01H DUP (?)
_BSS	ends

CONST	segment
CONST	ENDS

DGROUP	group	CONST,_DATA,_BSS
	assume	ds: DGROUP,ss: DGROUP
EXTRN	?LastX1@@3HE:WORD
EXTRN	?LastY1@@3HE:WORD
EXTRN	?iLastX@@3HE:WORD
EXTRN	?iLastY@@3HE:WORD
EXTRN	?xMapPosn@@3HE:WORD
EXTRN	?yMapPosn@@3HE:WORD

_TEXT	SEGMENT
	ASSUME cs:_TEXT

DCxGrid	equ	[bp+ABASE]
DCyGrid	equ	[bp+ABASE+4]
DCx	equ	[bp+ABASE+8]
DCy	equ	[bp+ABASE+10]
DCdx	equ	[bp+ABASE+12]
DCdy	equ	[bp+ABASE+16]

DCxhi	equ	[bp-2]
DCxl	equ	[bp-4]
DCyhi	equ	[bp-6]
DCyl	equ	[bp-8]

PBEGIN	?Ray@@ZAIPEI0HHJJ@Z
	push		bp
	mov		bp,sp
	sub		sp,8
	push		ds
	push		si
	push		di

	xor		eax,eax
	mov		edi,eax
	mov		esi,eax

	lfs		di,DWORD PTR DCxGrid
	lgs		si,DWORD PTR DCyGrid

	mov		DCxl,ax		; Clear low word
	mov		cx,DCx		; Initial x
	mov		DCxhi,cx

	mov		DCyl,ax		; Clear low word
	mov		dx,DCy		; Initial y
	mov		DCyhi,dx

$020:
	mov		ebx,DCdx	; x += dx
	add		DCxl,ebx
	jl		$040		; if x < 0
	mov		cx,DCxhi
	cmp		cx,1000h	; if x >= GRID_MAX
	jge		$040

	mov		ebx,DCdy	; y += dy
	add		DCyl,ebx
	jl		$040		; if y < 0
	mov		dx,DCyhi
	cmp		dx,1000h	; if y >= GRID_MAX
	jge		$040

	test		cx,3fh		; Only on a squares boundary
	jnz		$030

	mov		ax,dx		; ax = (y & 0xfc0) + (x >> 6)
	and		ax,0fc0h
	mov		bx,cx
	shr		bx,6
	or		ax,bx
	shl		ax,1		; Accessing words

	mov		bx,fs:[edi][eax]	; Color = xGrid[ax]
	or		bl,bl
	jz		$030

	mov		?LastWallHit@@3IE,bx
	shr		ax,1
	mov		?xMapPosn@@3HE,ax
	mov		?iLastX@@3HE,cx
	mov		?LastY1@@3HE,dx
	mov		ax,1
	jmp		$100

$030:	test		dx,3fh		; Only on a squares boundary
	jnz		$020

	mov		ax,dx		; ax = (y & 0xfc0) + (x >> 6)
	and		ax,0fc0h
	mov		bx,cx
	shr		bx,6
	or		ax,bx
	shl		ax,1		; Accessing words

	mov		bx,gs:[esi][eax]	; Color = yGrid[ax]
	or		bl,bl
	jz		$020

	mov		?LastWallHit@@3IE,bx
	shr		ax,1
	mov		?yMapPosn@@3HE,ax
	mov		?iLastY@@3HE,dx
	mov		?LastX1@@3HE,cx
	mov		ax,-1
	jmp		$100
	
$040:					; Didn't find anything
	xor		ax,ax

$100:	pop		di
	pop		si
	pop		ds
	mov		sp,bp
	pop		bp
	ret
?Ray@@ZAIPEI0HHJJ@Z	endp

_TEXT	ENDS
	END

