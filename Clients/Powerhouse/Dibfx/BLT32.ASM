    page ,132
    TITLE COPYBITS.ASM
;---------------------------------------------------------------------------;
;  COPYBITS.ASM
;
;  32 bit asm routines for doing a "sprite" bitblt
;
;	Copy32Bytes	- a faster hmemcpy
;	CopyBits	- dib to dib blts
;	CopyBitsT	- dib to dib blts with a transparent color
;
;---------------------------------------------------------------------------;

;
; 32 bit code segment version of COPYBITS.ASM
; General technology courtesy of Todd Laney
;
; NOTE! cmacro32.inc NEEDS MASM 5.1 (or compatible)
; you would think MASM 6 would be compatible but it isn't
;
    .xlist
    include cmacro32.inc
    .list

;
; NOTE!!!! because we are generating USE32 code this must NOT
; be located in a code segment with USE16 code, so we put it into
; it's own little segment....
;

ifndef SEGNAME
    SEGNAME equ <BLT_TEXT32>
endif

createSeg %SEGNAME, CodeSeg, word, use32, CODE

sBegin Data
sEnd Data

sBegin CodeSeg
        assumes cs,CodeSeg
        assumes ds,nothing
        assumes es,nothing

cProc Copy32Bytes,<FAR, PASCAL, PUBLIC>,<>
	ParmW   DestSelector    ; dest 48bit pointer
	ParmD   DestOffset
	ParmW	SrcSelector
	ParmD	SrcOffset	; source 48bit pointer
	ParmD	cBytes		; number of bytes
cBegin
	push ds
	push esi
	push edi

	mov ecx, cBytes		; how many bytes are we copying
        mov eax,ecx		;
        shr ecx,2		; how many dwords are we copying
        and eax,11b		; how many bytes left over

	mov ds, SrcSelector	
	mov esi, SrcOffset	; DS:[ESI] point to source

	mov es, DestSelector    ; es:[edi] point to dest
	mov edi, DestOffset


	cld			; movs goes forward through memory

	rep movs dword ptr es:[edi], dword ptr ds:[esi]
	mov ecx, eax
	rep movs byte ptr es:[edi], byte ptr ds:[esi]

	pop edi
	pop esi
	pop ds
cEnd

;-----------------------------------------------------------------------------;
;
; CopyBits
;
; 
;-----------------------------------------------------------------------------;

cProc CopyBits,<FAR, PASCAL, PUBLIC>,<>
	ParmW	selDst
	ParmD	offDst
	ParmD	pitchDst
	ParmW	selSrc
	ParmD	offSrc
	ParmD	pitchSrc
	ParmD	dwWidth
	ParmD	dwHeight

cBegin
        push ds
        push esi
        push edi

	mov ecx, dwWidth	; ecx counts pixels
        mov edx, dwHeight	; edx counts lines

	mov esi, offSrc
	mov ds, selSrc

	mov edi, offDst
	mov es, selDst
	
	sub pitchSrc, ecx	; bias pitch
	sub pitchDst, ecx	;

        mov ebx,ecx		;
        shr ebx,2		; number of dwords per line

        mov eax,ecx		;
        and eax,11b		; number of bytes remaining

        align 4

cdb_loop:
        mov ecx, ebx
        rep movs dword ptr es:[edi], dword ptr ds:[esi]
        mov ecx,eax
        rep movs byte ptr es:[edi], byte ptr ds:[esi]

        add esi, pitchSrc
        add edi, pitchDst
        dec edx                 ; line counter
        jnz short cdb_loop

cdb_nomore:
        pop edi
        pop esi
        pop ds
cEnd


sEnd CodeSeg
end
