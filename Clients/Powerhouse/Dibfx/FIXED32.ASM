;----------------------------Module-Header------------------------------;
; Module Name: FIXED32.ASM
;
; Fixed-point multiply and divide operations
;
; AUTHOR: Checker
;
; JonBl - Removed C++ implementation stuff
;
;-----------------------------------------------------------------------;
?PLM=1
?WIN=0

include cmacro32.inc

sBegin	Data
sEnd	Data

ifndef SEGNAME
    SEGNAME equ <_TEXT32>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE

sBegin  CodeSeg
	.386
	assumes cs,CodeSeg
	assumes ds,nothing
	assumes es,nothing

;-----------------------------------------------------------------------
;  Fixed point math routines for clipping calculations
;-----------------------------------------------------------------------

align 4

cProc FixedMultiply, <FAR,PASCAL,PUBLIC>,<>

	parmD	Multiplicand
	parmD	Multiplier

cBegin
	mov	eax,Multiplicand
	imul	Multiplier
	add	eax,8000h		; round result
	adc	edx,0
	shrd	eax,edx,16
cEnd

align 4

cProc FixedDivide, <FAR,PASCAL,PUBLIC>,<>

	parmD	Dividend
	parmD	Divisor

cBegin
	mov		eax,Dividend
	mov		ecx,Divisor

	rol		eax,10h			;fix dividend
	movsx	edx,ax
	xor		ax,ax
	
	idiv	ecx
	shld edx,eax,16
cEnd

sEnd    CodeSeg

end
