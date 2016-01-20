;	Static Name Aliases
;
	TITLE   sample.c
	NAME    sample

	.286p
	.287
SAMPLE_TEXT	SEGMENT  WORD PUBLIC 'CODE'
SAMPLE_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME  CS: SAMPLE_TEXT, DS: DGROUP, SS: DGROUP
EXTRN	__acrtused:ABS
EXTRN	__chkstk:FAR
EXTRN	__aFlmul:FAR
_BSS      SEGMENT
COMM NEAR	PLOCALHEAP:	BYTE:	 2
_BSS      ENDS
SAMPLE_TEXT      SEGMENT
	ASSUME	CS: SAMPLE_TEXT

;	/*********************************************************/
;	void sample8( input, inskip, output, outskip, count, rate )
;	/*********************************************************/
;	LPTR	input, output;
;	int	inskip, outskip;
;	int	count;
;	FIXED	rate;
;	{
;	FIXED	offset;

;	offset = rate>>1;
;	input += inskip;

;	offset += ((long)outskip * rate);
;	count -= outskip;
;	while ( --count >= 0 )
;		{
;		*output++ = *( input + HIWORD( offset ) );
;		offset += rate;
;		}
;	}

	PUBLIC	_sample8
_sample8	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	ASSUME DS: NOTHING
	mov	ax,6
	call	FAR PTR __chkstk
	push	si
; Line 19
;	input = 6
;	inskip = 10
;	output = 12
;	outskip = 16
;	count = 18
;	rate = 20
;	offset = -6
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	sar	dx,1
	rcr	ax,1
	mov	WORD PTR [bp-6],ax	;offset
	mov	WORD PTR [bp-4],dx

	mov	ax,WORD PTR [bp+10]	;inskip
	add	WORD PTR [bp+6],ax	;input
	push	WORD PTR [bp+22]
	push	WORD PTR [bp+20]	;rate
	mov	ax,WORD PTR [bp+16]	;outskip
	cwd	
	push	dx
	push	ax
	call	FAR PTR __aFlmul
	add	WORD PTR [bp-6],ax	;offset
	adc	WORD PTR [bp-4],dx

	mov	ax,WORD PTR [bp+16]	;outskip
	sub	WORD PTR [bp+18],ax	;count
	mov	cx,WORD PTR [bp+18]	;count
	lds	si,DWORD PTR [bp+6]	;input
	les	di,DWORD PTR [bp+12]	;output

	mov	bx,WORD PTR [bp-4]	;(HIWORD)offset
	mov	dx,WORD PTR [bp-6]	;(LOWORD)offset

	mov	ax,WORD PTR [bp+20]	;rate
	or	ax,ax
	jnz	nunity
	mov	ax,WORD PTR [bp+22]
	cmp	ax,1
	jnz	nunity
	jmp	short unity
; rate not 1:1
nunity:	inc	cx			;count++
	jmp	SHORT $L20002

unity:
	add	si,bx
	cld				;increment data pointers
	shr	cx,1
	rep	movsw
	jnc	fini8
	lodsb
	stosb
	jmp	short fini8
$FC4855:
	mov	al,[bx][si]
	stosb
	mov	ax,WORD PTR [bp+20]	;rate
	add	dx,ax			;offset
	adc	bx,WORD PTR [bp+22]	;hiword of offset
$L20002:
	loop	$FC4855
fini8:	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	
	nop	

_sample8	ENDP

; Line 43
	PUBLIC	_sample8MW
_sample8MW	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	ASSUME DS: NOTHING
	mov	ax,6
	call	FAR PTR __chkstk
	push	si
; Line 48
;	offset = -6
;	input = 6
;	inskip = 10
;	output = 12
;	outskip = 16
;	count = 18
;	rate = 20
;	mask = 24
; Line 51
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	sar	dx,1
	rcr	ax,1
	mov	WORD PTR [bp-6],ax	;offset
	mov	WORD PTR [bp-4],dx
; Line 52
	mov	ax,WORD PTR [bp+10]	;inskip
	add	WORD PTR [bp+6],ax	;input
; Line 54
	push	WORD PTR [bp+22]
	push	WORD PTR [bp+20]	;rate
	mov	ax,WORD PTR [bp+16]	;outskip
	cwd	
	push	dx
	push	ax
	call	FAR PTR __aFlmul
	add	WORD PTR [bp-6],ax	;offset
	adc	WORD PTR [bp-4],dx
; Line 55
	mov	ax,WORD PTR [bp+16]	;outskip
	sub	WORD PTR [bp+18],ax	;count
; Line 64
	jmp	SHORT $L20003
$FC4867:
; Line 65
; Line 66
	les	bx,DWORD PTR [bp+24]	;mask
	inc	WORD PTR [bp+24]	;mask
	cmp	BYTE PTR es:[bx],0
	je	$I4869
; Line 67
	mov	bx,WORD PTR [bp-4]
	les	si,DWORD PTR [bp+6]	;input
	mov	al,es:[bx][si]
	les	bx,DWORD PTR [bp+12]	;output
	inc	WORD PTR [bp+12]	;output
	mov	BYTE PTR es:[bx],al
; Line 68
	jmp	SHORT $I4870
$I4869:
	mov	ax,WORD PTR [bp+12]	;output
	mov	dx,WORD PTR [bp+14]
	inc	WORD PTR [bp+12]	;output
$I4870:
; Line 69
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	add	WORD PTR [bp-6],ax	;offset
	adc	WORD PTR [bp-4],dx
; Line 70
$L20003:
	dec	WORD PTR [bp+18]	;count
	jns	$FC4867
; Line 71
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	

_sample8MW	ENDP
; Line 77
	PUBLIC	_sample16
_sample16	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	ASSUME DS: NOTHING
	mov	ax,6
	call	FAR PTR __chkstk
	push	si
; Line 81
;	input = 6
;	inskip = 10
;	output = 12
;	outskip = 16
;	count = 18
;	rate = 20
;	offset = -6
; Line 84
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	sar	dx,1
	rcr	ax,1
	mov	WORD PTR [bp-6],ax	;offset
	mov	WORD PTR [bp-4],dx
; Line 85
	mov	ax,WORD PTR [bp+10]	;inskip
	shl	ax,1
	add	WORD PTR [bp+6],ax	;input
; Line 87
	push	WORD PTR [bp+22]
	push	WORD PTR [bp+20]	;rate
	mov	ax,WORD PTR [bp+16]	;outskip
	cwd	
	push	dx
	push	ax
	call	FAR PTR __aFlmul
	add	WORD PTR [bp-6],ax	;offset
	adc	WORD PTR [bp-4],dx
; Line 88
	mov	ax,WORD PTR [bp+16]	;outskip
	sub	WORD PTR [bp+18],ax	;count
; Line 97
	jmp	SHORT $L20004
$FC4880:
; Line 98
; Line 99
	mov	bx,WORD PTR [bp-4]
	shl	bx,1
	les	si,DWORD PTR [bp+6]	;input
	mov	ax,es:[bx][si]
	les	bx,DWORD PTR [bp+12]	;output
	add	WORD PTR [bp+12],2	;output
	mov	WORD PTR es:[bx],ax
; Line 100
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	add	WORD PTR [bp-6],ax	;offset
	adc	WORD PTR [bp-4],dx
; Line 101
$L20004:
	dec	WORD PTR [bp+18]	;count
	jns	$FC4880
; Line 102
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	

_sample16	ENDP
; Line 108
	PUBLIC	_sample16MW
_sample16MW	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	ASSUME DS: NOTHING
	mov	ax,6
	call	FAR PTR __chkstk
	push	si
; Line 113
;	input = 6
;	inskip = 10
;	output = 12
;	outskip = 16
;	count = 18
;	rate = 20
;	mask = 24
;	offset = -6
; Line 116
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	sar	dx,1
	rcr	ax,1
	mov	WORD PTR [bp-6],ax	;offset
	mov	WORD PTR [bp-4],dx
; Line 117
	mov	ax,WORD PTR [bp+10]	;inskip
	shl	ax,1
	add	WORD PTR [bp+6],ax	;input
; Line 119
	push	WORD PTR [bp+22]
	push	WORD PTR [bp+20]	;rate
	mov	ax,WORD PTR [bp+16]	;outskip
	cwd	
	push	dx
	push	ax
	call	FAR PTR __aFlmul
	add	WORD PTR [bp-6],ax	;offset
	adc	WORD PTR [bp-4],dx
; Line 120
	mov	ax,WORD PTR [bp+16]	;outskip
	sub	WORD PTR [bp+18],ax	;count
; Line 129
	jmp	SHORT $L20005
$FC4892:
; Line 130
; Line 131
	les	bx,DWORD PTR [bp+24]	;mask
	inc	WORD PTR [bp+24]	;mask
	cmp	BYTE PTR es:[bx],0
	je	$I4894
; Line 132
	mov	bx,WORD PTR [bp-4]
	shl	bx,1
	les	si,DWORD PTR [bp+6]	;input
	mov	ax,es:[bx][si]
	les	bx,DWORD PTR [bp+12]	;output
	add	WORD PTR [bp+12],2	;output
	mov	WORD PTR es:[bx],ax
; Line 133
	jmp	SHORT $I4895
	nop	
$I4894:
	add	WORD PTR [bp+12],2	;output
$I4895:
; Line 134
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	add	WORD PTR [bp-6],ax	;offset
	adc	WORD PTR [bp-4],dx
; Line 135
$L20005:
	dec	WORD PTR [bp+18]	;count
	jns	$FC4892
; Line 136
	pop	si
	sub	bp,2
	mov	sp,bp
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	
	nop	

_sample16MW	ENDP

;	/************************************************************/
;	void sample24( input, inskip, output, outskip, count, rate )
;	/************************************************************/
;	LPTR	input, output;
;	int	inskip, outskip;
;	int	count;
;	FIXED	rate;
;	{
;	FIXED	offset;
;	LPTR pixel;
;	
;	offset = rate>>1;
;	input += ((long)inskip*3L);
;	offset += ((long)outskip * rate);
;	count -= outskip;
;	while ( --count >= 0 )
;		{
;		pixel = input + (3L * HIWORD(offset));
;		*output++ = *pixel++;
;		*output++ = *pixel++;
;		*output++ = *pixel;
;		offset += rate;
;		}
;	}

	PUBLIC	_sample24
_sample24	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	ASSUME DS: NOTHING
	mov	ax,10
	call	FAR PTR __chkstk
; Line 146
;	input = 6
;	inskip = 10
;	output = 12
;	outskip = 16
;	count = 18
;	rate = 20
;	offset = -10
;	pixel = -6
; Line 150
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	sar	dx,1
	rcr	ax,1
	mov	WORD PTR [bp-10],ax	;offset
	mov	WORD PTR [bp-8],dx
; Line 151
	mov	ax,WORD PTR [bp+10]	;inskip
	mov	cx,ax
	shl	ax,1
	add	ax,cx
	add	WORD PTR [bp+6],ax	;input
; Line 152
	push	WORD PTR [bp+22]
	push	WORD PTR [bp+20]	;rate
	mov	ax,WORD PTR [bp+16]	;outskip
	cwd	
	push	dx
	push	ax
	call	FAR PTR __aFlmul
	add	WORD PTR [bp-10],ax	;offset
	adc	dx,0
; Line 153
	mov	ax,WORD PTR [bp+16]	;outskip
	sub	WORD PTR [bp+18],ax	;count
	mov	cx,WORD PTR [bp+18]	;count
	lds	si,DWORD PTR [bp+6]	;input
	les	di,DWORD PTR [bp+12]	;output
; Line 162
	mov	ax,WORD PTR [bp+20]	;rate
	or	ax,ax
	jnz	nun24
	mov	ax,WORD PTR [bp+22]
	cmp	ax,1
	jnz	nun24
	jmp	short un24
; rate not 1:1
nun24:	inc	cx			;count++
	jmp	SHORT $L20006
	nop	

un24:
	mov	bx,dx			;(HIWORD)offset
	add	bx,bx			;(HIWORD)offset*2
	add	bx,dx			;(HIWORD)offset*3
	add	si,bx
	mov	ax,cx
	add	ax,ax
	add	cx,ax			;3 bytes per pixel for 24 bit
	cld				;increment data pointers
	shr	cx,1
	rep	movsw
	jnc	fini24
	lodsb
	stosb
	jmp	short fini24

$FC4906:
	mov	bx,dx			;(HIWORD)offset
	add	bx,bx			;(HIWORD)offset*2
	add	bx,dx			;(HIWORD)offset*3
	mov	al,[bx][si]
	stosb
	inc	bx
	mov	al,[bx][si]
	stosb
	inc	bx
	mov	al,[bx][si]
	stosb
	inc	bx
	mov	ax,WORD PTR [bp+20]	;(LOWORD)rate
	add	WORD PTR [bp-10],ax	;(LOWORD)offset
	mov	ax,WORD PTR [bp+22]	;(HIWORD)rate
	adc	dx,ax			;(HIWORD)offset

$L20006:
	loop	$FC4906

fini24:	sub	bp,2
	mov	sp,bp
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	

_sample24	ENDP

; Line 176
	PUBLIC	_sample24MW
_sample24MW	PROC FAR
	push	ds
	pop	ax
	nop	
	inc	bp
	push	bp
	mov	bp,sp
	push	ds
	mov	ds,ax
	ASSUME DS: NOTHING
	mov	ax,10
	call	FAR PTR __chkstk
; Line 181
;	input = 6
;	inskip = 10
;	output = 12
;	outskip = 16
;	count = 18
;	rate = 20
;	mask = 24
;	offset = -10
;	pixel = -6
; Line 185
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	sar	dx,1
	rcr	ax,1
	mov	WORD PTR [bp-10],ax	;offset
	mov	WORD PTR [bp-8],dx
; Line 186
	mov	ax,WORD PTR [bp+10]	;inskip
	mov	cx,ax
	shl	ax,1
	add	ax,cx
	add	WORD PTR [bp+6],ax	;input
; Line 188
	push	WORD PTR [bp+22]
	push	WORD PTR [bp+20]	;rate
	mov	ax,WORD PTR [bp+16]	;outskip
	cwd	
	push	dx
	push	ax
	call	FAR PTR __aFlmul
	add	WORD PTR [bp-10],ax	;offset
	adc	WORD PTR [bp-8],dx
; Line 189
	mov	ax,WORD PTR [bp+16]	;outskip
	sub	WORD PTR [bp+18],ax	;count
; Line 198
	jmp	SHORT $L20007
	nop	
$FC4919:
; Line 199
; Line 200
	les	bx,DWORD PTR [bp+24]	;mask
	inc	WORD PTR [bp+24]	;mask
	cmp	BYTE PTR es:[bx],0
	je	$I4921
; Line 201
; Line 202
	mov	ax,WORD PTR [bp-8]
	mov	cx,ax
	shl	ax,1
	add	ax,cx
	add	ax,WORD PTR [bp+6]	;input
	mov	dx,WORD PTR [bp+8]
	mov	WORD PTR [bp-6],ax	;pixel
	mov	WORD PTR [bp-4],dx
; Line 203
	les	bx,DWORD PTR [bp-6]	;pixel
	inc	WORD PTR [bp-6]	;pixel
	mov	al,BYTE PTR es:[bx]
	les	bx,DWORD PTR [bp+12]	;output
	inc	WORD PTR [bp+12]	;output
	mov	BYTE PTR es:[bx],al
; Line 204
	les	bx,DWORD PTR [bp-6]	;pixel
	inc	WORD PTR [bp-6]	;pixel
	mov	al,BYTE PTR es:[bx]
	les	bx,DWORD PTR [bp+12]	;output
	inc	WORD PTR [bp+12]	;output
	mov	BYTE PTR es:[bx],al
; Line 205
	les	bx,DWORD PTR [bp-6]	;pixel
	mov	al,BYTE PTR es:[bx]
	les	bx,DWORD PTR [bp+12]	;output
	inc	WORD PTR [bp+12]	;output
	mov	BYTE PTR es:[bx],al
; Line 206
; Line 207
	jmp	SHORT $I4922
$I4921:
; Line 208
	add	WORD PTR [bp+12],3	;output
$I4922:
; Line 209
	mov	ax,WORD PTR [bp+20]	;rate
	mov	dx,WORD PTR [bp+22]
	add	WORD PTR [bp-10],ax	;offset
	adc	WORD PTR [bp-8],dx
; Line 210
$L20007:
	dec	WORD PTR [bp+18]	;count
	jns	$FC4919
; Line 211
	sub	bp,2
	mov	sp,bp
	pop	ds
	ASSUME DS: DGROUP
	pop	bp
	dec	bp
	ret	

_sample24MW	ENDP
SAMPLE_TEXT	ENDS
END
