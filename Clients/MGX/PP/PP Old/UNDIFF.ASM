;****************************************************************************
;*
;*        Name:  UndifferenceLZW
;*
;*        extern void FAR PASCAL UndifferenceLZW (LPVOID lpBuffer,
;*                                                WORD   wBytes,
;*                                                WORD   wBitsPerPixel,
;*                                                BOOL   bTriplets);
;*
;*     Purpose:  Translate a horizontally differenced scan line
;*  Parameters:  lpBuffer       - buffer containing scan ine
;*               wBytes         - # of data bytes
;*               wBitsPerPixel  - must be 4 or 8
;*               bTriplets      - TRUE means triplet (Planar Config 1 RGB)
;*     Returns:  translated scanline and updated difference state
;*       Notes:  
;*
;****************************************************************************

cProc   UndifferenceLZW, <FAR,PUBLIC>, <SI,DI>

        ParmD   lpBuffer
        ParmW   wBytes
        ParmW   wBitsPerPixel
        ParmW   bTriplets

cBegin  UndifferenceLZW

        LES     DI,lpBuffer             ; ES:DI => buffer
        MOV     CX,wBytes               ; CX    => # of bytes to process
        JCXZ    undif_end

        XOR     AL,AL
        CMP     wBitsPerPixel,4
        JE      undif_4bit
        CMP     bTriplets,0
        JNZ     undif_triplets

undif_8bit:
        ADD     AL,ES:[DI]
        STOSB
        LOOP    undif_8bit
        JMP     short undif_end

undif_triplets:
        XOR     DX,DX
        MOV     AX,CX
        MOV     CX,3
        DIV     CX
        MOV     CX,AX
        XOR     BX,BX
        XOR     DX,DX

undif_triplets_loop:
        MOV     AX,ES:[DI]
        ADD     AL,BL
        ADD     AH,BH
        MOV     BX,AX
        STOSW
        MOV     AL,ES:[DI]
        ADD     AL,DL
        MOV     DL,AL
        STOSB
        LOOP    undif_triplets_loop
        JMP     short undif_end

;****************************************************************************
;*  4-bit loop:
;*
;*     (Assumes the previous nibble value is loaded in the upper half of AL)
;*
;*  Notation:   "a" is the previous 4-bit value
;*              "b" is the next 4-bit value
;*              "c" is the 4-bit value after "b"
;*
;*  The loop reads in the byte value  ( b | c ) and outputs ( a+b | a+b+c ).
;*
;*
;*  This loop-code is faster for 386/486, but code used is better for 286's.
;*
;*        ADD     AL,ES:[DI]              ; AX = ( _  | _ | ab | c )
;*        MOV     AH,AL                   ;      ( ab | c | ab | c )
;*        SHL     AH,4                    ;      ( c  | 0 | ab | c )
;*        AND     AX,0F0F0h               ;      ( c  | 0 | ab | 0 )
;*        ADD     AH,AL                   ;      ( abc| 0 | ab | 0 )
;*        SHR     AH,4                    ; AX = ( 0  |abc| ab | 0 )
;*        ADD     AL,AH                   ;      ( 0  |abc| ab |abc)
;*        STOSB                           ; Store AL
;*        SHL     AL,4
;*
;****************************************************************************

undif_4bit:
        ADD     AL,ES:[DI]              ; AX = ( _ | _ |ab | c )
        MOV     AH,AL                   ;      (ab | c |ab | c )

        XOR     BL,BL                   ; BX = ( _ | _ | 0 | 0 )
        MOV     BH,AH                   ;      (ab | c | 0 | 0 )
        SHR     BX,4                    ;      ( 0 |ab | c | 0 )

        ADD     AH,BH                   ; AX = ( _ |abc|ab | c )
        AND     AX,0FF0h                ;      ( 0 |abc|ab | 0 )

        ADD     BL,AL                   ; BX = ( 0 |ab |abc| 0 )

        OR      AL,AH                   ; AX = ( 0 |abc|ab |abc)
        STOSB
        MOV     AL,BL                   ; AX = ( 0 | _ |abc| 0 )
        LOOP    undif_4bit              ; Loop for each byte of scanline

undif_end:

cEnd    UndifferenceLZW

