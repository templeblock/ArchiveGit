pIn_off		equ	[bp+6]
iCount		equ	[bp+8]
lpOut_off	equ	[bp+10]
lpOut_seg	equ	[bp+12]
labits		equ	[bp+14]

	.model medium
	.286
	.code
	
	public	_fla2con

_fla2con	proc	far
	push	bp
	mov	bp,sp
	push	di
	push	es
	push	si
;
;basic concept: copy(labits[*pIn++], lpOut, 8)
;
	mov	dx,iCount	;get the number of pixels
	shr	dx,3		;get bytes per line
	mov	bx,pIn_off	;load the offset of the input data
	mov	di,lpOut_off	;load the offset of the destination
	mov	ax,lpOut_seg	;load the segment of the destination
	mov	es,ax		;into es
nxtbit:	mov	al,[bx]		;get an input byte
	xor	ah,ah		;clear upper bits
	shl	ax,3		;*8, each lookup table entry is 8 bytes long
	mov	si,ax		;put it into the source reg
	add	si,labits	;add the starting address of the lookup table
	mov	cx,4		;copy 8 bytes
rep	movsw
	inc	bx		;point at the next input byte
	dec	dx		;decrement bytes per line count
	jnz	nxtbit		;continue until entire line done

	pop	si
	pop	es
	pop	di
	pop	bp
	ret
_fla2con	endp
	end
