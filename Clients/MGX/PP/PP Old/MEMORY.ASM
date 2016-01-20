;®TP5¯®BT5¯®RFA®FC¯Page ®PN¯ of ®FP¯ -- beta ®VA$FI¯ -- 11:20 AM , October 25, 1988¯

.model	medium
.code

public	_ExtCount
;
; this routine checks for the amout of extended memory
;
; input: none
; output: returns the amount of extended memory
;
_ExtCount	proc	far
		mov	ah,88h
		int	15h
		ret
_ExtCount	endp

;public	_GetA000H
;
; this routine get the Windows A000H selector value
;
; input: none
; output: returns the selector to use for A000H
;
;extrn __A000H:ABS
;
;_GetA000H	proc	far
;		mov	ax,__A000H
;		ret
;_GetA000H	endp
;
;
;public	_GetAHINCR
;
; this routine get the Windows AHINCR increment for huge pointer math
;
; input: none
; output: returns the AH increment for huge math
;
;extrn __AHINCR:ABS
;
;_GetAHINCR	proc	far
;		mov	ax,__AHINCR
;		ret
;_GetAHINCR	endp
;
end
