;®TP5¯®BT5¯®RFA®FC¯Page ®PN¯ of ®FP¯ -- beta ®VA$FI¯ -- 11:20 AM , October 25, 1988¯
	TITLE	TOOLS

include cmacros.inc

sBegin	CODE

assumes CS,CODE
assumes DS,NOTHING


;
; lstrlen: Same as strlen except for taking long ptrs
;
cProc	_lstrlen,<PUBLIC,FAR>
;	parmD	pStr
cBegin	nogen
	mov	bx,sp
	push	di
	les	di,ss:[bx+4]
	cld
	xor	ax,ax			; get zero in ax
	mov	cx,-1			; at most 64 k to move
	repnz	scasb			; look for end
	mov	ax,cx
	neg	ax
	dec	ax
	dec	ax
	pop	di
	ret	4
cEnd	nogen

lstrsetup:
	pop	dx
	mov	bx,sp

	push	ds
	push	si
	push	di

	lds	si,ss:[bx+4]
	les	di,ss:[bx+8]
	regptr	dssi,ds,si
	regptr	esdi,es,di
	cld
	jmp	dx

lstrfinish  proc    far
	pop	di
	pop	si
	pop	ds
	ret	8
lstrfinish  endp


;
;lstrcpy: strcpy with long pointers
;
cProc	_lstrcpy,<PUBLIC,FAR>
;	parmD	pDst			; [bx+8]
;	parmD	pSrc			; [bx+4]

cBegin	nogen
	call	lstrsetup
lcp1:	lodsb
	stosb
	or	al,al
	jnz	lcp1
	mov	ax,di			; point at last byte copied
	dec	ax
	mov	dx,es			; and its segment
	jmp	lstrfinish
cEnd	nogen


;
;lstrcat: Same as strcat except with long ptrs.
;
cProc	_lstrcat,<PUBLIC,FAR>
;   parmD   pDst
;   parmD   pSrc

cBegin	nogen
	call	lstrsetup
	xor	ax,ax			; get zero in ax
	mov	cx,-1			; at most 64 k to look
	repnz	scasb			; look for end
	dec	di			; Point at null byte
	jmp	lcp1			; jump to lstrcpy loop
cEnd	nogen

sEnd	CODE

end
