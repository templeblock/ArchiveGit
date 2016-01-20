


.MODEL SMALL,C

.CODE

EXTRN Source:DWORD
EXTRN Dest:DWORD

PUBLIC ShowPage32
ShowPage32 PROC FAR C

.386

    push ds
    cld

    les di,Dest
    lds si,Source
    mov cx,16000
    rep movsd
    pop ds
    ret

ShowPage32 ENDP

END



