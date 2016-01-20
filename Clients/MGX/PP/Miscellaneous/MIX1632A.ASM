.386

ENSURE_32SEG macro
    xor eax,eax                 ;; Test to see if in a 32 bit segment
    mov ah,80h                  ;; When executed as 16 bit code, the 
    add eax,eax                 ;; ADD overflows, setting the carry flag.
    jnc @F                      ;; As 32 bit code, carry flag doesn't get set.

    ;; Use DPMI to change the 16 bit segment descriptor to 32 bits
    ;; Note that this code must be done with DB's, since putting in the
    ;; actual instructions would cause the assembler to generate the
    ;; instructions as if they were running in a 32 bit segment.
    db 057h                     ;; push    di
    db 083h,0ECh,008h           ;; sub     sp,8
    db 08Ch,0CBh                ;; mov     bx,cs
    db 08Ch,0D0h                ;; mov     ax,ss
    db 08Eh,0C0h                ;; mov     es,ax
    db 08Bh,0FCh                ;; mov     di,sp
    db 0B8h,00Bh,000h           ;; mov     ax,000Bh
    db 0CDh,031h                ;; int     31h
    db 026h,080h,04Dh,006h,040h ;; or      byte ptr es:[di+6],40h
    db 0B8h,00Ch,000h           ;; mov     ax,000Ch
    db 0CDh,031h                ;; int     31h
    add     sp,8                ;; db 083h,0C4h,008h
    pop     di                  ;; db 05Fh
@@:
endm

MYSEG32 segment PARA PUBLIC USE32 'CODE'

public memcpy32
memcpy32:

    ENSURE_32SEG

_memcpy32 proc pascal \
    destSeg:WORD, destOffset:DWORD, srcSeg:WORD, srcOffset:DWORD, cbCopy:DWORD
    
    movzx   ebp, bp     ;; Zero out high word of EBP

    push    ds
    push    esi
    push    edi

    mov     ds, [srcSeg]
    mov     esi,[srcOffset]
    mov     es, [destSeg]
    mov     edi,[destOffset]
    mov     ecx, [cbCopy]

    push    ecx                 ;; Save original ECX value
    shr     ecx, 2              ;; divide by 4 (we're moving DWORDs)
    cld                         ;; make sure we move in right direction
    rep     movsd
    pop     ecx
    and     ecx, 3              ;; Get bottom 2 bits of cbCopy and move
    rep     movsb               ;; with rep movsb

    pop     edi
    pop     esi
    pop     ds

    pop     ebp

    db      66h
    retf    14h
_memcpy32 endp

MYSEG32 ends
end
