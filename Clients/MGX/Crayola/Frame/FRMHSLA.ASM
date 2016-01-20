;       Static Name Aliases
;
IFNDEF C_CODE
	TITLE   hslasm.c
	NAME    HSLASM

	.386p
HSLASM_TEXT     SEGMENT  USE16 WORD PUBLIC 'CODE'
HSLASM_TEXT     ENDS
_DATA   SEGMENT  USE16 WORD PUBLIC 'DATA'
_DATA   ENDS
CONST   SEGMENT  USE16 WORD PUBLIC 'CONST'
CONST   ENDS
_BSS    SEGMENT  USE16 WORD PUBLIC 'BSS'
_BSS    ENDS
DGROUP  GROUP   CONST, _BSS, _DATA
	ASSUME  CS: HSLASM_TEXT, DS: DGROUP, SS: DGROUP
EXTRN   __acrtused:ABS
EXTRN   __chkstk:FAR
_BSS      SEGMENT
COMM NEAR       PLOCALHEAP:     BYTE:    2
_BSS      ENDS
HSLASM_TEXT      SEGMENT
	ASSUME  CS: HSLASM_TEXT
; Line 1489
; Line 29
	PUBLIC  _RGBtoHSL
_RGBtoHSL       PROC FAR
	push    ds
	pop     ax
	nop     
	inc     bp
	push    bp
	mov     bp,sp
	push    ds
	mov     ds,ax
	ASSUME DS: NOTHING
	mov     ax,16
	call    FAR PTR __chkstk
	push    di
	push    si
; Line 31
;       R = 6
;       G = 8
;       B = 10
;       lpHSL = 12
;       cMax = -14
;       cMin = -10
;       sum = -8
;       dif = -6
;       H = -12
;       L = -16
;       S = -4
; Line 38
	mov     bl,BYTE PTR [bp+6]      ;bl=R
	mov     bh,BYTE PTR [bp+8]      ;bh=G
	mov     ch,BYTE PTR [bp+10]     ;ch=B

	mov     dh,bl                   ;R
	sub     dh,bh                   ;R-G
	sbb     cl,cl
	not     cl
	and     dh,cl
	add     dh,bh                   ;G
	sub     dh,ch                   ;G-B
	sbb     cl,cl
	not     cl
	and     dh,cl
	add     dh,ch                   ;dh=cMax
; Line 39
	mov     dl,bl                   ;R
	sub     dl,bh                   ;G
	sbb     cl,cl
	and     dl,cl
	add     dl,bh                   ;G
	sub     dl,ch                   ;B
	sbb     cl,cl
	and     dl,cl
	add     dl,ch                   ;dl=cMin
; Line 40
	mov     al,dl                   ;cMin
	sub     ah,ah
	mov     si,ax                   ;si=cMin (WORD)
	mov     al,dh                   ;cMax
	mov     di,ax                   ;di=cMax (WORD)
	add     ax,si                   ;ax = sum = cMin + cMax
; Line 41
	sub     di,si                   ;di = dif = cMax-cMin
; Line 42
	mov     si,ax                   ;si = sum
	shr     ax,1
	mov     BYTE PTR [bp-16],al     ;L
; Line 44
	cmp     di,0                    ;dif
	jne     $JCC109
	jmp     $I5980
$JCC109:
; Line 45
; Line 47
	cmp     si,255                  ;sum
	jbe     $I5981
; Line 48
	mov     ax,510
	sub     ax,si                   ;sum = (2*RGBMAX)-sum
	mov     si,ax                   ;si=sum
; Line 49
$I5981:
	mov     cl,dh                   ; save cMax
	imul    ax,di,255               ;dif
	sub     dx,dx
	div     si                      ;sum
	mov     BYTE PTR [bp-4],al      ;S
	mov     dh,cl                   ; restore cMax
; Line 52
	mov     al,dh                   ;cMax
	cmp     bl,al                   ;R
	jne     $I5982
	mov     al,bh                   ;G
	sub     ah,ah
	mov     cl,ch                   ;B
	sub     ch,ch
	sub     ax,cx
	mov     cx,ax
	imul    ax,cx,42
	cwd     
	idiv    di                      ;dif
	jmp     SHORT $L20013
$I5982:
; Line 54
	cmp     bh,dh                   ;g,cMax
	jne     $I5984
	mov     al,ch                   ;B
	sub     ah,ah
	mov     cl,bl                   ;R
	sub     ch,ch
	sub     ax,cx
	mov     cx,ax
	imul    ax,cx,42
	cwd     
	idiv    di                      ;dif
	add     ax,84
	jmp     SHORT $L20013
$I5984:
; Line 56
;       mov     al,dh                   ;cMax
;       cmp     BYTE PTR [bp+10],al     ;B
;       jne     $I5986
	mov     al,bl                   ;R
	sub     ah,ah
	mov     cl,bh                   ;G
	sub     ch,ch
	sub     ax,cx
	mov     cx,ax
	imul    ax,cx,42
	cwd     
	idiv    di                      ;dif
	add     ax,168
$L20013:
; Line 58
$I5986:
	cmp     ax,0                    ;H
	jge     $I5987
	add     ax,252                  ;H
; Line 59
$I5987:
	cmp     ax,252                  ;H
	jle     $I5989
	sub     ax,252                  ;H
; Line 60
; Line 61
	jmp     SHORT $I5989
$I5980:
; Line 62
	mov     BYTE PTR [bp-4],0       ;S
; Line 63
	sub     ax,ax                   ;H
; Line 64
$I5989:
; Line 66
	les     bx,DWORD PTR [bp+12]    ;lpHSL
	mov     BYTE PTR es:[bx],al
; Line 67
	mov     al,BYTE PTR [bp-4]      ;S
	mov     BYTE PTR es:[bx+1],al
; Line 68
	mov     al,BYTE PTR [bp-16]     ;L
	mov     BYTE PTR es:[bx+2],al
; Line 69
	pop     si
	pop     di
	sub     bp,2
	mov     sp,bp
	pop     ds
	ASSUME DS: DGROUP
	pop     bp
	dec     bp
	ret     
	nop     

_RGBtoHSL       ENDP
; Line 75
	PUBLIC  _HSLtoRGB
_HSLtoRGB       PROC FAR
	push    ds
	pop     ax
	nop     
	inc     bp
	push    bp
	mov     bp,sp
	push    ds
	mov     ds,ax
	ASSUME DS: NOTHING
	mov     ax,16
	call    FAR PTR __chkstk
	push    di
	push    si
; Line 77
;       hue = 6
;       sat = 8
;       lum = 10
;       lpRGB = 12
;       R = -8
;       G = -10
;       B = -6
;       iHue = -12
;       n = -14
;       n1 = -16
;       n2 = -4
; Line 81
	mov     bh,BYTE PTR[bp+8]       ;bh = sat
	cmp     bh,0                    ;sat
	jne     $I6002
; Line 82
; Line 83
	les     bx,DWORD PTR [bp+12]    ;lpRGB
	mov     al,BYTE PTR [bp+10]     ;lum
	mov     BYTE PTR es:[bx+2],al
	mov     BYTE PTR es:[bx+1],al
	mov     BYTE PTR es:[bx],al
; Line 85
	jmp     $EX5994
$I6002:
	mov     bl,BYTE PTR [bp+6]      ;bl = hue
	cmp     bl,252                  ;hue
	jbe     $I6003
	sub     bl,252                  ;hue
; Line 93
$I6003:
	mov     cl,BYTE PTR [bp+10]     ;cl=lum
	mov     al,cl                   ;lum
	sub     ah,ah
	mov     si,ax                   ;si=n1
	mov     di,ax                   ;di=n2
; Line 94
	cmp     cl,127                  ; lum
	jbe     $I6004
; Line 95
	mov     al,255
	sub     al,cl                   ;lum
	mov     cl,al                   ;lum
; Line 96
$I6004:
	mov     al,bh                   ;sat
	sub     ah,ah
	sub     ch,ch
	mul     cx
	sub     dx,dx
	mov     cx,255
	div     cx                      ;ax=n
; Line 97
	sub     si,ax                   ;n1
	cmp     si,1                    ;n1
	jne     $I6005
	dec     si                      ;n1
; Line 98
$I6005:
	add     di,ax                   ;n2
	cmp     di,254                  ;n2
	jne     $I6006
	inc     di                      ;n2
; Line 100
$I6006:
	mov     al,bl                   ;hue
	sub     bh,bh                   ;bx=iHue
; Line 101
	cmp     bx,42
	jge     $I6007
; Line 102
; Line 103
	mov     ax,di                   ;n2
	mov     BYTE PTR [bp-8],al      ;R
; Line 104
	mov     ax,di                   ;n2
	sub     ax,si                   ;n1
	imul    bx                      ;iHue
	cwd     
	mov     cx,42
	idiv    cx
	mov     cx,si
	add     al,cl                   ;n1
$L20020:
	mov     BYTE PTR [bp-10],al     ;G
; Line 105
	mov     ax,si                   ;n1
	jmp     $L20015
	nop     
$I6007:
; Line 108
	cmp     bx,84                   ;iHue
	jge     $I6009
; Line 109
; Line 110
	sub     bx,84                   ;iHue
; Line 111
	mov     cx,bx                   ;iHue
	neg     cx
	mov     ax,di                   ;n2
	sub     ax,si                   ;n1
	imul    cx
	cwd     
	mov     cx,42
	idiv    cx
	mov     cx,si
	add     al,cl                   ;n1
	mov     BYTE PTR [bp-8],al      ;R
; Line 112
	mov     ax,di                   ;n2
	jmp     SHORT $L20020
$I6009:
; Line 116
	cmp     bx,126                  ;iHue
	jge     $I6011
; Line 117
; Line 118
	sub     bx,84                   ;iHue
; Line 119
	mov     ax,si                   ;n1
	mov     BYTE PTR [bp-8],al      ;R
; Line 120
	mov     ax,di                   ;n2
	mov     BYTE PTR [bp-10],al     ;G
; Line 121
	mov     ax,di                   ;n2
	sub     ax,si                   ;n1
	imul    bx                      ;iHue
	jmp     SHORT $L20021
	nop     
$I6011:
; Line 124
	cmp     bx,168                  ;iHue
	jge     $I6013
; Line 125
; Line 126
	sub     bx,168                  ;iHue
; Line 127
	mov     ax,si                   ;n1
	mov     BYTE PTR [bp-8],al      ;R
; Line 128
	mov     cx,bx                   ;iHue
	neg     cx
	mov     ax,di                   ;n2
	sub     ax,si                   ;n1
	imul    cx
	cwd     
	mov     cx,42
	idiv    cx
	mov     cx,si
	add     al,cl                   ;n1
$L20022:
	mov     BYTE PTR [bp-10],al     ;G
; Line 129
	mov     ax,di                   ;n2
	jmp     SHORT $L20015
$I6013:
; Line 132
	cmp     bx,210                  ;iHue
	jge     $I6015
; Line 133
; Line 134
	sub     bx,168                  ;iHue
; Line 135
	mov     ax,di                   ;n2
	sub     ax,si                   ;n1
	imul    bx                      ;iHue
	cwd     
	mov     cx,42
	idiv    cx
	mov     cx,si
	add     al,cl                   ;n1
	mov     BYTE PTR [bp-8],al      ;R
; Line 136
	mov     ax,si                   ;n1
	jmp     SHORT $L20022
$I6015:
; Line 141
; Line 142
	sub     bx,252                  ;iHue
; Line 143
	mov     ax,di                   ;n2
	mov     BYTE PTR [bp-8],al      ;R
; Line 144
	mov     ax,si                   ;n1
	mov     BYTE PTR [bp-10],al     ;G
; Line 145
	mov     cx,bx                   ;iHue
	neg     cx
	mov     ax,di                   ;n2
	sub     ax,si                   ;n1
	imul    cx
$L20021:
	cwd     
	mov     cx,42
	idiv    cx
	mov     cx,si
	add     al,cl                   ;n1
$L20015:
	mov     BYTE PTR [bp-6],al      ;B
; Line 146
; Line 149
	les     bx,DWORD PTR [bp+12]    ;lpRGB
	mov     al,BYTE PTR [bp-8]      ;R
	mov     BYTE PTR es:[bx],al
; Line 150
	mov     al,BYTE PTR [bp-10]     ;G
	mov     BYTE PTR es:[bx+1],al
; Line 151
	mov     al,BYTE PTR [bp-6]      ;B
	mov     BYTE PTR es:[bx+2],al
; Line 152
$EX5994:
	pop     si
	pop     di
	sub     bp,2
	mov     sp,bp
	pop     ds
	ASSUME DS: DGROUP
	pop     bp
	dec     bp
	ret     

_HSLtoRGB       ENDP
HSLASM_TEXT     ENDS
ENDIF
END

