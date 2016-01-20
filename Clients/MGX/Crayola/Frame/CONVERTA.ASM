IFNDEF C_CODE
	.286
	.model medium

	.data
EXTRN	_Matrix:BYTE
	.code

EXTRN	__chkstk:FAR

;
;void Convert24to24( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, lpBlt )
;LPRGB lpPixel;
;int iCount;
;LPINT lpError;
;LPTR lpOutput;
;int xDiva, yDiva;
;LPBLT lpBlt;
;
	PUBLIC	_Convert24to24
_Convert24to24	PROC FAR
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

_Convert24to24	ENDP

;
;void Convert8to24( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, lpBlt )
;LPTR lpPixel;
;int iCount;
;LPINT lpError;
;LPTR lpOutput;
;int xDiva, yDiva;
;LPBLT lpBlt;
;
	PUBLIC	_Convert8to24
_Convert8to24	PROC FAR
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

_Convert8to24	ENDP

;
;void Convert24to4( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, lpBlt )
;LPRGB lpPixel;
;int iCount;
;LPINT lpError;
;LPTR lpOutput;
;int xDiva, yDiva;
;LPBLT lpBlt;
;
	PUBLIC	_Convert24to4
_Convert24to4	PROC FAR
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

	add	ax,OFFSET _Matrix	;lpMatrix + ((yDiva&15)*16)
	mov	dx,ds			;lpMatrix segment
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

_Convert24to4	ENDP


;void Convert24to8( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, lpBlt )
;LPRGB lpPixel;
;int iCount;
;LPINT lpError;
;LPTR lpOutput;
;int xDiva, yDiva;
;LPBLT lpBlt;
;
	PUBLIC	_Convert24to8
_Convert24to8	PROC FAR
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

;		p = 10;
	mov	dl,10		; p = 10

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

_Convert24to8	ENDP

ENDIF
END

