page ,132
TITLE CLEAR32.ASM

;---------------------------------------------------------------------------;
;  clear32.asm
;
;  Provides fast 32-bit memory clears.
;
;  Copyright (c) 1994 Microsoft Corporation
;---------------------------------------------------------------------------;

.xlist
include cmacro32.inc
.list

;
; NOTE!!!! because we are generating USE32 code this must NOT
; be located in a code segment with USE16 code, so we put it into
; it's own little segment....
;

ifndef SEGNAME
	SEGNAME equ <_TEXT32>
endif

createSeg %SEGNAME, CodeSeg, word, use32, CODE

sBegin Data
sEnd Data

sBegin CodeSeg
		assumes cs,CodeSeg
		assumes ds,nothing
		assumes es,nothing

cProc ClearMem32,<FAR, PASCAL, PUBLIC>,<>
	ParmW selDst
	ParmD offDst
	ParmD cBytes
	ParmB value

cBegin
	push es
	push edi
                

	; Create a 32-bit word filled with 4 vals
	xor ebx, ebx
	mov bl, value
	mov bh,bl
	mov eax,ebx
	shl eax,16
	or  eax,ebx

	mov ecx, cBytes		; how many bytes are we clearing
	mov ebx, ecx
	shr ecx, 2		; how many dwords are we clearing
	and ebx, 11b		; how many bytes left over

	mov es, selDst
	mov edi, offDst

	cld

	rep stos dword ptr es:[edi]
	mov ecx, ebx
	rep stos byte ptr es:[edi]

	pop edi
	pop es
cEnd

sEnd CodeSeg
end
