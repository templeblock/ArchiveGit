;  _mixit(*lpDest,*pTemp[],channels,count)
;
; *lpDest = Destination buffer.
; *pTemp = Array of pointers to source data.
;      ****Assumed to be referenced off of DS (i.e. static data)****
; channels = #of channels with data.
; count = # of bytes to mix.
;
;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
; 
;   Copyright (c) 1993, 1994  Microsoft Corporation.  All Rights Reserved.
; 


	.MODEL small, pascal
MIXIT  PROTO  lpDest:DWORD, pTemp:WORD, channels:WORD, count:WORD        

_TEXT SEGMENT WORD PUBLIC 'CODE' 

	.386        
MIXIT  PROC  USES ds es fs gs esi edi bp ebx edx lpDest:DWORD,
		pTemp:WORD, channels:WORD, count:WORD
	LOCAL isum2:WORD, isum3:WORD, isum4:WORD, normalize:WORD, chan:WORD, pTemp3:WORD, dwShit:DWORD

		cld
	sub ebx,ebx
	sub ecx,ecx
	sub edx,edx
	sub edi,edi
	sub esi,esi
	mov dwShit,ebx

	mov ax,channels  
	cmp ax,2           
	jz  twosounds      ;special case for 2 sounds.
	cmp ax,1
	jz  onesound       ;special case for 1 sound.
	mov ax,channels
	cmp ax,3
	jz  threesounds    ;special case for 3 sounds.
			   ;Falls thru to case for 4 or more sounds.

	les di,lpDest      ;es:[edi] = pointer to lpDest.
	
	mov ax,channels
	dec ax
	mov cl,7
	shl ax,cl
	mov normalize,ax   ;Offset required to normalize data = (channels-1)*128

	mov dx,pTemp       ;Initialize array offset
	sub dx,4
	mov ax,channels    
	add ax,ax
	add ax,ax
	add ax,dx
	mov chan,ax        ;pTemp3 = pTemp+(4*channels)
	mov pTemp3,dx      ;pTemp3 is address of each array element in pTemp
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;This section mixes count/4 Dwords.
massmix:mov cx,count       
	and cl,0fch
	sub count,cx
	shr cx,2
	or  cx,cx          ;Find number of DWORDS to mix.
	jz  remain         ;If no DWORDS, do nothing.

	sub ah,ah
outer4: sub dx,dx          ;4 variables used to store sums.
	mov isum2,dx
	mov isum3,dx
	mov isum4,dx
	mov si,chan        ;Load address of pTemp[channels]
inner4: 
	;lfs bx,DWORD PTR ds:[si]  ;Load FS:BX from pTemp[channels]
	mov bx,WORD PTR ds:[si+2]
	mov	fs,bx
	mov bx,WORD PTR ds:[si]

	add WORD PTR ds:[si],4
	jc  sixty4k4              ;Check for segment overrun.
next4:  mov eax,DWORD PTR fs:[bx] ;Load DWORD of data.
	movzx bx,al        ;Get first byte.
	add dx,bx          ;Add to sum.
	mov bl,ah          ;Get Second byte...
	add isum2,bx
	ror eax,16
	mov bl,al
	add isum3,bx
	mov bl,ah
	add isum4,bx
	
	sub si,4           ;si = pTemp[channels-1]
	cmp pTemp3,si      ;Check to see if pTemp[0] was mixed.
	jnz  inner4        

	sub dx,normalize   ;Normalize dx=sum
	or  dh,dh          ;Check for >255 or <0
	jnz oops241        ;Invalid data.
	mov al,dl
back1:  mov dx,isum2       ;Normalize isum2...
	sub dx,normalize
	or  dh,dh
	jnz oops242
	mov ah,dl
back2:  ror eax,16
	mov dx,isum3
	sub dx,normalize
	or  dh,dh
	jnz oops243
	mov al,dl
back3:  mov dx,isum4
	sub dx,normalize
	or  dh,dh
	jnz oops244
	mov ah,dl
back4:  ror eax,16
	
	stosd              ;Store mixed DWORD
	loop outer4
	jmp remain

oops241:movsx  dx,dh            ;handle >255 or <0 case.
	not    dh
	mov    al,dh
	jmp    back1
oops242:movsx  dx,dh            ;handle >255 or <0 case.
	not    dh
	mov    ah,dh
	jmp    back2
oops243:movsx  dx,dh            ;handle >255 or <0 case.
	not    dh
	mov    al,dh
	jmp    back3
oops244:movsx  dx,dh            ;handle >255 or <0 case.
	not    dh
	mov    ah,dh
	jmp    back4


sixty4k4: add WORD PTR [si+2],8  ;Advance segment to next segment.
	jmp  next4

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;                
;This mix the odd few bytes not mixed by massmix.
remain: mov cx,count            ;mix remaining bytes
	or  cx,cx
	jz  done3
	
	sub ah,ah
outer5: xor dx,dx
	mov si,chan
	inc dwShit
inner5: lfs bx,DWORD PTR ds:[si]
	add WORD PTR ds:[si],1
	jc  sixty4k5
next5:  mov al,BYTE PTR fs:[bx]  ;Do it one byte at a time.
	add dx,ax
	sub si,4
	cmp pTemp3,si
	jnz inner5
	mov ax,dx
	sub ax,normalize
	or  ah,ah
	jnz oops25
	stosb
	loop outer5
	jmp done3
oops25: movsx  ax,ah            ;handle >255 or <0 case.
	movzx  ax,ah
	not  al
	stosb
	loop outer5
	jmp  done3



sixty4k5: add WORD PTR [si+2],8  ;End of segment, incr segment reg to next segment.
	jmp  next5



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Special case for two sounds. 
		
twosounds: les di,lpDest        ;es:[di] = lpDest
	mov bx,pTemp
	mov ax,ds
	mov gs,ax
	lfs si,gs:[bx]          ;gs:[bx] = pTemp
	mov dx,si               ;fs:dx = pTemp[0]
	lds si,gs:[bx+4]        ;ds:si = pTemp[1]
	mov bx,dx               ;fs:bx = pTemp[0]
	mov cx,count
	
	sub dh,dh
	sub ah,ah
outer2: lodsb ds:[esi]
	mov dl,fs:[ebx]
	inc ebx                 ;No worries about segment overrun using ebx
	add ax,dx
	sub ax,128              ;Normalize data.
	or  ah,ah               ;Check for invalid result.
	jnz oops3
	stosb es:[edi]          ;Store result.
	loop outer2
	jmp done3
oops3:  movsx  ax,ah            ;handle >255 or <0.
	movzx  ax,ah
	not  al
	stosb
	loop outer2
	jmp  done3


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;special case for three sounds.                        
		
threesounds: les di,lpDest
	mov bx,pTemp
	lfs si,ds:[bx]          ;ds:[bx] = pTemp
	mov dx,si               ;fs:[dx] = pTemp[0]
	lgs si,ds:[bx+8]
	mov ax,si               ;gs:[ax] = pTemp[2]
	lds si,ds:[bx+4]        ;ds:[si] = pTemp[1]
	mov bx,dx               ;fs:[bx] = pTemp[0]
	mov cx,count            ;
	
	ror edx,16              ;Save bp
	mov dx,bp              
	ror edx,16
	sub ebp,ebp
	mov bp,ax               ;gs:[ebp] = pTemp[2]
	sub dh,dh
	sub ah,ah

outer23: lodsb ds:[esi]         
	mov dl,fs:[ebx]
	inc ebx
	add ax,dx
	mov dl,gs:[ebp]
	inc ebp
	add ax,dx               ;normalize data.
	sub ax,256
	or  ah,ah
	jnz oops33
	stosb es:[edi]
	loop outer23
	ror edx,16
	mov bp,dx
	jmp done3
oops33:  movsx  ax,ah           ;handle >255 or <0.
	movzx  ax,ah
	not  al
	stosb
	loop outer23
	ror edx,16
	mov bp,dx
	jmp  done3


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Special case for one sound - just a memcopy.
onesound: 
	mov cx,count
	shr cx,2
	mov bx,pTemp
	mov ax,ds
	mov gs,ax
	
	lds si,gs:[bx]
	les di,lpDest
	rep movsd es:[edi],ds:[esi]  ;Copy count/4 dwords.
	mov cx,count
	and cx,03h
	rep movsb es:[edi],ds:[esi]  ;Copy remaining bytes.
	jmp done3


done3:        
	ret
MIXIT  ENDP
_TEXT   ENDS
	END
