	page ,132
	TITLE TEXMAP.ASM

;---------------------------------------------------------------------------;
;  texmap.asm
;
;  32 bit asm routine for drawing a bilinear texture mapped scanline
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

		externD		_ClipMinX
		externD		_ClipMaxX
		externD		_ClipMinY
		externD		_ClipMaxY

sEnd Data

sBegin CodeSeg
		assumes cs,CodeSeg
		assumes ds,Data
		assumes es,nothing

BailOut:
		jmp		EndOfScan
		

cProc ScanOutLine,<FAR, PASCAL, PUBLIC>,<>
		ParmD	StartX			; left X coordinate of destination scanline
		ParmD	EndX			; right X coordinate of destination scanline
		ParmD	SourceStartX	; left texture X fixedpoint coordinate
		ParmD	SourceStartY	; left texture Y fixedpoint coordinate
		ParmD	SourceEndX		; right texture X fixedpoint coordinate
		ParmD	SourceEndY		; right texture Y fixedpoint coordinate
		ParmD	pSource			; segment:offset of source pointer
		ParmD	SourceWidth		; source width in bytes

		ParmW	DestinationSegment
		ParmD	pDestination	; offset of destination pointer

		LocalD	SourceStepX		; fixedpoint X step in source
		LocalD	SourceStepY		; fixedpoint Y step in source
		LocalD	Errors			; temporary for double error accumulators
cBegin
		push	ds
		push	esi
		push	edi

		mov		ebx,[StartX]
		mov		ecx,[EndX]

		; trivial reject
		
		mov		esi,ecx
		sub		esi,ebx					; scanline width
		jz		BailOut					; nothing to draw
		jg		CorrectOrder

		xchg	ebx,ecx					; put'em in the right order
		neg		esi

CorrectOrder:

		cmp		ecx,[_ClipMinX]
		jle		BailOut
		cmp		ebx,[_ClipMaxX]
		jge		BailOut

		shl		esi,16					; make it fixedpoint

		; calculate source fixedpoint X step

		mov		eax,[SourceEndX]
		xor		edx,edx
		sub		eax,[SourceStartX]
		rol		eax,16
		movsx	edx,ax
		xor		ax,ax
		idiv	esi
		mov		[SourceStepX],eax
		mov		edi,eax

		; calculate source fixedpoint Y step

		mov		eax,[SourceEndY]
		xor		edx,edx
		sub		eax,[SourceStartY]
		rol		eax,16
		movsx	edx,ax
		xor		ax,ax
		idiv	esi
		mov		[SourceStepY],eax

; cbh!  ignoring half step into source - how does it look?

		cmp		ecx,[_ClipMaxX]
		jg		ClipRightEdge

RightEdgeClipped:

		cmp		ebx,[_ClipMinX]
		jl		ClipLeftEdge

LeftEdgeClipped:

		; ebx = StartX
		; ecx = EndX

		; get source and destination pointers

		xor		esi,esi
		lds		si,[pSource]

		mov		di,[DestinationSegment]
		mov		es,di
		mov		edi,[pDestination]
		add		edi,ebx					; add StartX to destination

		sub		ecx,ebx					; get width
		sub		ecx,1					; prestep for main loop and fill rule
		jle		BailOut					; nothing to draw!
		shl		ecx,16					; put it in ecx

		; calculate starting source offset
		
		mov		eax,[SourceStartY]
		mov		ebx,eax					; keep copy of 16.16
		and		eax,0FFFF0000h			; make an integer
		imul	[SourceWidth]			; 32.32 in edx:eax

		shrd	eax,ebx,16				; get fraction towards next scan in eax
		
		add		esi,edx					; add in Y offset

		mov		edx,[SourceStartX]
		shr		eax,1					; Y fractional step 15 bits in eax
		shrd	eax,edx,16				; eax = X step:Y Step (15 bits)
		sar		edx,16					; get integer
		add		esi,edx					; add to source offset

		; eax = HIWORD: X error, LOWORD 15 bit error
		; esi = source pointer
		; edi = destination pointer
		; ecx = HIWORD: counter

		mov		[Errors],eax	; save for later

		; calculate integer steps

		mov		eax,[SourceStepY]		; eax = integer Y step:fractional Y step
		mov		cx,[WORD PTR SourceWidth+2]

		or		eax,eax
		jns		short YStepPositive

		; SourceStepY is negative

		; fix the error term for Y stepping

; cbh! will also need to fix X stepping!!!!!

		mov		eax,[Errors]
		sub		ax,08000h				; compensate for decreasing Y
		neg		ax
		mov		[Errors],eax

		mov		eax,[SourceStepY]		; gross!!!! cbh!

		neg		eax						; flip it
		mov		ebx,eax					; keep fraction in bx
		and		eax,0ffff0000h
		imul	[SourceWidth]			; edx = integer, eax = 0
		neg		edx						; negate integer Y step
		neg		cx						; negate Y overflow step
		jmp		short DoXStep

YStepPositive:

		mov		ebx,eax					; bx = fractional part
		and		eax,0ffff0000h
		imul	[SourceWidth]			; edx = integer, eax = 0

DoXStep:

		shrd	eax,ebx,17				; fraction in eax (15 bits)
		and		eax,07fff0000h			; chop off possible high bit

		mov		ebx,[SourceStepX]

		or		ebx,ebx
		js		XStepIsNegative

		shrd	eax,ebx,16				; eax = X fraction:Y fraction (15 bits)
		shr		ebx,16					; get integer

		add		ebx,edx					; collect integer steps

		movsx	edx,cx					; get Y overflow step

		; eax = fractional steps
		; edx = Y integer step
		; ebx = X + Y integer step
		; ecx = HIWORD: counter
		; esi = source pointer
		; edi = destination pointer

		push 	ebp						; save base pointer

		mov		ebp,[Errors]

; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
; %%%%%%%%%%%%%%%%%%%%% Can't access local variables %%%%%%%%%%%%%%%%%%%%%%%
		
align 4

MainLoop_XPositive:

		; eax = HIWORD: X fractional step, LOWORD Y >> 1 fractional step
		; ebx = X + Y integer step
		; ecx = HIWORD: pixel count, LOWORD: scratch for pixel transfer
		; edx = Y step
		; ebp = HIWORD: X error accumulator, LOWORD: Y >> 1 error accumulator
		; ds:esi = source pointer
		; es:edi = destination pointer

		mov		cl,[esi]				; get byte from source
		mov		es:[edi],cl				; store it to destination

		add		esi,ebx					; add in X and Y base steps
		add		ebp,eax					; accumulate X and Y error
		adc		esi,0					; advance in X if necessary
		test	ebp,08000h				; test for Y overflow
		jz		short @f

		; Y overflowed, advance an extra step

		and		ebp,NOT 08000h			; clear overflow bit
		add		esi,edx					; add in Y step

@@:

		add		edi,1					; increment destination pointer
		sub		ecx,010000h				; decrement counter
		jnc		MainLoop_XPositive

		pop		ebp

; %%%%%%%%%%%%%%%%%%%%%% Okay to access local variables %%%%%%%%%%%%%%%%%%%%
; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		jmp		EndOfScan


XStepIsNegative:

		neg		ebx						; convert SourceStepX to positive
		shrd	eax,ebx,16				; eax = X fraction:Y fraction (15 bits)
		shr		ebx,16					; get integer
		neg		ebx						; convert back to negative

		add		ebx,edx					; collect integer steps

		movsx	edx,cx					; get Y overflow step

		; eax = fractional steps
		; edx = Y integer step
		; ebx = X + Y integer step
		; ecx = HIWORD: counter
		; esi = source pointer
		; edi = destination pointer

		push 	ebp						; save base pointer

		mov		ebp,[Errors]

; cbh! sorta hack-like

		rol		ebp,16					; get x error in bp
		sub		bp,0FFFFh				; subtract 65535
		dec		bp						; make it 65536
		neg		bp						; get distance to rollover
		rol		ebp,16					; put it back
		

; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
; %%%%%%%%%%%%%%%%%%%%% Can't access local variables %%%%%%%%%%%%%%%%%%%%%%%
		
align 4

MainLoop_XNegative:

		; eax = HIWORD: X fractional step, LOWORD Y >> 1 fractional step
		; ebx = X + Y integer step
		; ecx = HIWORD: pixel count, LOWORD: scratch for pixel transfer
		; edx = Y step
		; ebp = HIWORD: X error accumulator, LOWORD: Y >> 1 error accumulator
		; ds:esi = source pointer
		; es:edi = destination pointer

		mov		cl,[esi]				; get byte from source
		mov		es:[edi],cl				; store it to destination

		add		esi,ebx					; add in X and Y base steps
		add		ebp,eax					; accumulate X and Y error
		sbb		esi,0					; advance in X if necessary
		test	ebp,08000h				; test for Y overflow
		jz		short @f

		; Y overflowed, advance an extra step

		and		ebp,NOT 08000h			; clear overflow bit
		add		esi,edx					; add in Y step

@@:

		add		edi,1					; increment destination pointer
		sub		ecx,010000h				; decrement counter
		jnc		MainLoop_XNegative

		pop		ebp

; %%%%%%%%%%%%%%%%%%%%%% Okay to access local variables %%%%%%%%%%%%%%%%%%%%
; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


EndOfScan:

		pop 	edi
		pop 	esi
		pop 	ds

cEnd

ClipRightEdge:

		mov		ecx,[_ClipMaxX]
		jmp		RightEdgeClipped

ClipLeftEdge:

		; eax has SourceStepY
		; edi has SourceStepX
		; ebx has StartX

		sub		ebx,[_ClipMinX]
		neg		ebx

		imul	ebx
		
		add		[SourceStartY],eax

		mov		eax,edi
		imul	ebx
		add		[SourceStartX],eax

		mov		ebx,[_ClipMinX]

		jmp		LeftEdgeClipped



;***************************************************************************
;
;	Transparent version begins here - it's almost exactly
;	the same, and I'm sure the two version could share
;	some code, but for now...
;

cProc ScanOutTransparentLine,<FAR, PASCAL, PUBLIC>,<>
		ParmD	StartX			; left X coordinate of destination scanline
		ParmD	EndX			; right X coordinate of destination scanline
		ParmD	SourceStartX	; left texture X fixedpoint coordinate
		ParmD	SourceStartY	; left texture Y fixedpoint coordinate
		ParmD	SourceEndX		; right texture X fixedpoint coordinate
		ParmD	SourceEndY		; right texture Y fixedpoint coordinate
		ParmD	pSource			; segment:offset of source pointer
		ParmD	SourceWidth		; source width in bytes

		ParmW	DestinationSegment
		ParmD	pDestination	; offset of destination pointer
		parmB   iTransparent            ;transparent index color

		LocalD	SourceStepX		; fixedpoint X step in source
		LocalD	SourceStepY		; fixedpoint Y step in source
		LocalD	Errors			; temporary for double error accumulators
cBegin
		push	ds
		push	esi
		push	edi

		mov		ebx,[StartX]
		mov		ecx,[EndX]

		; trivial reject
		
		mov		esi,ecx
		sub		esi,ebx					; scanline width
		jz		BailOut					; nothing to draw
		jg		TCorrectOrder

		xchg	ebx,ecx					; put'em in the right order
		neg		esi

TCorrectOrder:

		cmp		ecx,[_ClipMinX]
		jle		BailOut
		cmp		ebx,[_ClipMaxX]
		jge		BailOut

		shl		esi,16					; make it fixedpoint

		; calculate source fixedpoint X step

		mov		eax,[SourceEndX]
		xor		edx,edx
		sub		eax,[SourceStartX]
		rol		eax,16
		movsx	edx,ax
		xor		ax,ax
		idiv	esi
		mov		[SourceStepX],eax
		mov		edi,eax

		; calculate source fixedpoint Y step

		mov		eax,[SourceEndY]
		xor		edx,edx
		sub		eax,[SourceStartY]
		rol		eax,16
		movsx	edx,ax
		xor		ax,ax
		idiv	esi
		mov		[SourceStepY],eax

; cbh!  ignoring half step into source - how does it look?

		cmp		ecx,[_ClipMaxX]
		jg		TClipRightEdge

TRightEdgeClipped:

		cmp		ebx,[_ClipMinX]
		jl		TClipLeftEdge

TLeftEdgeClipped:

		; ebx = StartX
		; ecx = EndX

		; get source and destination pointers

		xor		esi,esi
		lds		si,[pSource]

		mov		di,[DestinationSegment]
		mov		es,di
		mov		edi,[pDestination]
		add		edi,ebx					; add StartX to destination

		sub		ecx,ebx					; get width
		sub		ecx,1					; prestep for main loop and fill rule
		jle		BailOut					; nothing to draw!
		shl		ecx,16					; put it in ecx

		; calculate starting source offset
		
		mov		eax,[SourceStartY]
		mov		ebx,eax					; keep copy of 16.16
		and		eax,0FFFF0000h			; make an integer
		imul	[SourceWidth]			; 32.32 in edx:eax

		shrd	eax,ebx,16				; get fraction towards next scan in eax
		
		add		esi,edx					; add in Y offset

		mov		edx,[SourceStartX]
		shr		eax,1					; Y fractional step 15 bits in eax
		shrd	eax,edx,16				; eax = X step:Y Step (15 bits)
		sar		edx,16					; get integer
		add		esi,edx					; add to source offset

		; eax = HIWORD: X error, LOWORD 15 bit error
		; esi = source pointer
		; edi = destination pointer
		; ecx = HIWORD: counter

		mov		[Errors],eax	; save for later

		; calculate integer steps

		mov		eax,[SourceStepY]		; eax = integer Y step:fractional Y step
		mov		cx,[WORD PTR SourceWidth+2]

		or		eax,eax
		jns		short TYStepPositive

		; SourceStepY is negative

		; fix the error term for Y stepping

; cbh! will also need to fix X stepping!!!!!

		mov		eax,[Errors]
		sub		ax,08000h				; compensate for decreasing Y
		neg		ax
		mov		[Errors],eax

		mov		eax,[SourceStepY]		; gross!!!! cbh!

		neg		eax						; flip it
		mov		ebx,eax					; keep fraction in bx
		and		eax,0ffff0000h
		imul	[SourceWidth]			; edx = integer, eax = 0
		neg		edx						; negate integer Y step
		neg		cx						; negate Y overflow step
		jmp		short TDoXStep

TYStepPositive:

		mov		ebx,eax					; bx = fractional part
		and		eax,0ffff0000h
		imul	[SourceWidth]			; edx = integer, eax = 0

TDoXStep:

		shrd	eax,ebx,17				; fraction in eax (15 bits)
		and		eax,07fff0000h			; chop off possible high bit

		mov		ebx,[SourceStepX]

		or		ebx,ebx
		js		TXStepIsNegative

		shrd	eax,ebx,16				; eax = X fraction:Y fraction (15 bits)
		shr		ebx,16					; get integer

		add		ebx,edx					; collect integer steps

		movsx	edx,cx					; get Y overflow step

		; eax = fractional steps
		; edx = Y integer step
		; ebx = X + Y integer step
		; ecx = HIWORD: counter
		; esi = source pointer
		; edi = destination pointer

		mov	ch, iTransparent

		push 	ebp						; save base pointer

		mov		ebp,[Errors]

; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
; %%%%%%%%%%%%%%%%%%%%% Can't access local variables %%%%%%%%%%%%%%%%%%%%%%%
		
align 4

TMainLoop_XPositive:

		; eax = HIWORD: X fractional step, LOWORD Y >> 1 fractional step
		; ebx = X + Y integer step
		; ecx = HIWORD: pixel count, LOWORD: scratch for pixel transfer
		; edx = Y step
		; ebp = HIWORD: X error accumulator, LOWORD: Y >> 1 error accumulator
		; ds:esi = source pointer
		; es:edi = destination pointer

		mov		cl,[esi]				; get byte from source
	        cmp		cl, ch
	        jz		short XPos_Transparent
		mov		es:[edi],cl				; store it to destination
XPos_Transparent:

		add		esi,ebx					; add in X and Y base steps
		add		ebp,eax					; accumulate X and Y error
		adc		esi,0					; advance in X if necessary
		test	ebp,08000h				; test for Y overflow
		jz		short @f

		; Y overflowed, advance an extra step

		and		ebp,NOT 08000h			; clear overflow bit
		add		esi,edx					; add in Y step

@@:

		add		edi,1					; increment destination pointer
		sub		ecx,010000h				; decrement counter
		jnc		TMainLoop_XPositive

		pop		ebp

; %%%%%%%%%%%%%%%%%%%%%% Okay to access local variables %%%%%%%%%%%%%%%%%%%%
; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		jmp		EndOfScan


TXStepIsNegative:

		neg		ebx						; convert SourceStepX to positive
		shrd	eax,ebx,16				; eax = X fraction:Y fraction (15 bits)
		shr		ebx,16					; get integer
		neg		ebx						; convert back to negative

		add		ebx,edx					; collect integer steps

		movsx	edx,cx					; get Y overflow step

		; eax = fractional steps
		; edx = Y integer step
		; ebx = X + Y integer step
		; ecx = HIWORD: counter
		; esi = source pointer
		; edi = destination pointer

		mov		ch,iTransparent

		push 	ebp						; save base pointer

		mov		ebp,[Errors]

; cbh! sorta hack-like

		rol		ebp,16					; get x error in bp
		sub		bp,0FFFFh				; subtract 65535
		dec		bp						; make it 65536
		neg		bp						; get distance to rollover
		rol		ebp,16					; put it back
		

; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
; %%%%%%%%%%%%%%%%%%%%% Can't access local variables %%%%%%%%%%%%%%%%%%%%%%%
		
align 4

TMainLoop_XNegative:

		; eax = HIWORD: X fractional step, LOWORD Y >> 1 fractional step
		; ebx = X + Y integer step
		; ecx = HIWORD: pixel count, LOWORD: scratch for pixel transfer
		; edx = Y step
		; ebp = HIWORD: X error accumulator, LOWORD: Y >> 1 error accumulator
		; ds:esi = source pointer
		; es:edi = destination pointer

		mov		cl,[esi]				; get byte from source
	        cmp		cl, ch
	        jz		short XNeg_Transparent
		mov		es:[edi],cl				; store it to destination
XNeg_Transparent:

		add		esi,ebx					; add in X and Y base steps
		add		ebp,eax					; accumulate X and Y error
		sbb		esi,0					; advance in X if necessary
		test	ebp,08000h				; test for Y overflow
		jz		short @f

		; Y overflowed, advance an extra step

		and		ebp,NOT 08000h			; clear overflow bit
		add		esi,edx					; add in Y step

@@:

		add		edi,1					; increment destination pointer
		sub		ecx,010000h				; decrement counter
		jnc		TMainLoop_XNegative

		pop		ebp

; %%%%%%%%%%%%%%%%%%%%%% Okay to access local variables %%%%%%%%%%%%%%%%%%%%
; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


TEndOfScan:

		pop 	edi
		pop 	esi
		pop 	ds

cEnd

TClipRightEdge:

		mov		ecx,[_ClipMaxX]
		jmp		TRightEdgeClipped

TClipLeftEdge:

		; eax has SourceStepY
		; edi has SourceStepX
		; ebx has StartX

		sub		ebx,[_ClipMinX]
		neg		ebx

		imul	ebx
		
		add		[SourceStartY],eax

		mov		eax,edi
		imul	ebx
		add		[SourceStartX],eax

		mov		ebx,[_ClipMinX]

		jmp		TLeftEdgeClipped

sEnd CodeSeg
end


