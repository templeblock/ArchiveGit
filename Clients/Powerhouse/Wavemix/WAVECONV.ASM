        page    ,132
;-----------------------------Module-Header-----------------------------;
; Module Name:  WAVECONV.ASM
;
;   routines for converting PCM wave data around
;
; Copyright (c) 1984-1994 Microsoft Corporation
;
; Public Functions:
;
;       pcmSizeOutputData   - convert size of output wave data given input
;       pcmConvert          - convert from one format to another.
;
; Public Data:          none
;
; General Description:
;
; Restrictions:
;
;       this code converts from any PCM format (8/16 mono/stereo) at any
;       sample rate to any other.  Thus is is to general the most common
;       conversion is just a sample rate change, this should be handled
;       more optimaly but it is not.
;
;-----------------------------------------------------------------------;

;   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
;   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
;   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
;   PURPOSE.
; 
;   Copyright (c) 1993, 1994  Microsoft Corporation.  All Rights Reserved.
; 

        ?PLM=1                          ; Pascal calling convention
        ?WIN=0                          ; NO! Windows prolog/epilog code

        .xlist
        include cmacro32.inc            ; for real 32 bit code (USE32)
;;;;;;;;include cmacros.inc             ; for 16 bit USE16 code
        .list

;===========================================================================;
;   internal flags for format types
;===========================================================================;

WF_STEREO       = 00000010b
WF_16BIT        = 00000100b

;===========================================================================;
;   structurs and macros
;===========================================================================;


;
; shorter versions of these obnoxious things
;
dptr    equ     dword ptr
wptr    equ     word ptr
bptr    equ     byte ptr


;
; Useful for 386 stuff
;
EAXtoDXAX   macro
        shld    edx,eax,16      ; move HIWORD(eax) to dx
        endm

DXAXtoEAX   macro
        ror     eax,16          ; xchg HIWORD(eax) and LOWORD(eax)
        shrd    eax,edx,16      ; move LOWORD(edx) to HIWORD(eax)
        endm

LOBHIB struc
        lob     db  ?
        hib     db  ?
LOBHIB ends

LOHI struc
        lo      dw  ?
        hi      dw  ?
LOHI ends

;===========================================================================;
;   min/max/abs macros
;===========================================================================;

ABS_AX	macro
	cwd
	xor	ax,dx
	sub	ax,dx
        endm

;
; _SIGNED_ MIN() macro
;
MIN_AX  macro   REG
ifdif <REG>,<0>
        sub     ax,REG
endif
	cwd
	and	ax,dx
ifdif <REG>,<0>
        add     ax,REG
endif
	endm

;
; _SIGNED_ MAX() macro
;
MAX_AX  macro   REG
ifdif <REG>,<0>
        sub     ax,REG
endif
	cwd
	not	dx
        and     ax,dx
ifdif <REG>,<0>
        add     ax,REG
endif
        endm


ABS_EAX	macro
	cdq
	xor	eax, edx
	sub	eax, edx
        endm

;
; _SIGNED_ MIN() macro
;
MIN_EAX  macro REG
ifdif <REG>, <0>
        sub     eax, REG
endif
	cdq
	and	eax, edx
ifdif <REG>, <0>
        add     eax, REG
endif
	endm

;
; _SIGNED_ MAX() macro
;
MAX_EAX  macro REG
ifdif <REG>, <0>
        sub     eax, REG
endif
	cdq
	not	edx
        and     eax, edx
ifdif <REG>, <0>
        add     eax, REG
endif
        endm

;===========================================================================;
;   waveformat stuff from MMSYSTEM.INC
;===========================================================================;

; general waveform format structure (information common to all formats)
WAVEFORMAT struc
    wfmt_wFormatTag       dw    ?   ; format type
    wfmt_nChannels        dw    ?   ; number of channels (i.e. mono, stereo, etc.)
    wfmt_nSamplesPerSec   dd    ?   ; sample rate
    wfmt_nAvgBytesPerSec  dd    ?   ; for buffer estimation
    wfmt_nBlockAlign      dw    ?   ; block size of data
WAVEFORMAT ends

; flags for wFormatTag field of WAVEFORMAT
WAVE_FORMAT_PCM     equ    1

; specific waveform format structure for PCM data
PCMWAVEFORMAT struc
    pcm_wf               db (SIZE WAVEFORMAT) dup (?)
    pcm_wBitsPerSample   dw    ?
PCMWAVEFORMAT ends

;===========================================================================;
;   segmentation
;===========================================================================;

ifndef SEGNAME
        SEGNAME equ <_TEXT32>
endif

createSeg %SEGNAME, CodeSeg, word, public, CODE


;===========================================================================;
;   Data Segment
;===========================================================================;

sBegin Data

sEnd Data


;===========================================================================;
;   code segment
;===========================================================================;

sBegin CodeSeg
        .386                            ; ain't no girly man code!
        assumes cs, CodeSeg
        assumes ds, Data
        assumes es, nothing

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   the following table is indexed by the wave format flags
;
;       x x x x 0
;       | | | |
;       | | | +------------ output is 1=stereo, 0=mono
;       | | +-------------- output is 1=16 bit, 0=8bit
;       | +---------------- input  is 1=stereo, 0=mono
;       +------------------ input  is 1=16 bit, 0=8bit
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        public ReadSample_Table
ReadSample_Table label dword
        dd      ReadSample_M08M08
        dd      ReadSample_S08M08
        dd      ReadSample_M16M08
        dd      ReadSample_S16M08

        dd      ReadSample_M08S08
        dd      ReadSample_S08S08
        dd      ReadSample_M16S08
        dd      ReadSample_S16S08

        dd      ReadSample_M08M16
        dd      ReadSample_S08M16
        dd      ReadSample_M16M16
        dd      ReadSample_S16M16

        dd      ReadSample_M08S16
        dd      ReadSample_S08S16
        dd      ReadSample_M16S16
        dd      ReadSample_S16S16


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   the following table is indexed by the wave format flags
;
;       x x 0
;       | |
;       | +------------ output is 1=stereo, 0=mono
;       +-------------- output is 1=16 bit, 0=8bit
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        public WriteSample_Table
WriteSample_Table label dword
        dd      WriteSample_M08
        dd      WriteSample_S08
        dd      WriteSample_M16
        dd      WriteSample_S16


;---------------------------Public-Routine------------------------------;
; pcmGetWaveFormat - determine if a PCMWAVEFORMAT is valid
;
; Entry:
;       DS:SI   -> PCMWAVEFORMAT
; Returns:
;       NC      - valid PCM wave format
;       DX:AX   - samples per second
;       BX      - wave format flags (WF_STEREO, WF_16BIT)
; Error Returns:
;       C       - invalid PCM wave format
; Registers Preserved:
;       BP,ES,DS,SI,DI
; Registers Destroyed:
;       AX,BX,CX,DX,FLAGS
; History:
;       Wed 04-Jan-1990 13:45:58 -by-  Todd Laney [ToddLa]
;	Created.
;-----------------------------------------------------------------------;

        assumes ds,nothing
        assumes es,nothing

        public pcmGetWaveFormat
pcmGetWaveFormat proc near

        xor     bx, bx                  ; set flags to zero

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   get and validate the FormatTag only PCM wave formats are valid
;   is allowed
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        cmp     [si].wfmt_wFormatTag,WAVE_FORMAT_PCM
        jne     short GetWaveFormatFail

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   get and validate the number of channels, only 1 or 2 channels
;   is allowed
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
        mov     dx,[si].wfmt_nChannels  ; get wfChannels

        or      dx,dx                   ; 0 is invalid
        jz      short GetWaveFormatFail

        cmp     dx,2                    ; > 2 is invalid
        ja      short GetWaveFormatFail

        and     dx,not 0001h
        or      bx,dx                   ; set WF_STEREO if wfChannels == 2
        errnz   WF_STEREO-2

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   get and validate the bits per sample, only 16 or 8 is valid
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     dx,[si].pcm_wBitsPerSample

        cmp     dx,8
        je      short GetWaveFormatSuccess

        cmp     dx,16
        jne     short GetWaveFormatFail

        or      bx,WF_16BIT             ; set the WF_16BIT flag
        errn$   GetWaveFormatSuccess

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   get the samples per second, any sample rate from 1 to FFFFFFFFh is
;   valid.
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

GetWaveFormatSuccess:

        mov     dx, [si].wfmt_nSamplesPerSec.hi
        mov     ax, [si].wfmt_nSamplesPerSec.lo

        mov     cx, dx
        or      cx, ax
        jz      short GetWaveFormatFail     ; == 0 invalid

GetWaveFormatSuccessExit:

        clc
        ret

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

GetWaveFormatFail:

        stc
        ret

pcmGetWaveFormat endp


;---------------------------Public-Routine------------------------------;
; pcmBytesToSamples - convert number of bytes to number of samples
;
;
;       Samples = Bytes / (NumChannels*BitsPerSample/8)
;
; Entry:
;       DX:AX   - number of bytes
;       BX      - wave format flags (WF_16BIT, WF_STEREO)
; Returns:
;       DX:AX   - number of samples
; Error Returns:
;       none
; Registers Preserved:
;       BP,ES,DS,SI,DI
; Registers Destroyed:
;       AX,BX,CX,DX,FLAGS
; History:
;       Wed 04-Jan-1990 13:45:58 -by-  Todd Laney [ToddLa]
;	Created.
;-----------------------------------------------------------------------;
        assumes ds, nothing
        assumes es, nothing

        public pcmBytesToSamples
pcmBytesToSamples proc near

        mov     cx,bx
        and     cx,WF_16BIT+WF_STEREO
        jz      short BytesToSamplesExit

        shr     dx,1                    ; divide by 2
        rcr     ax,1

        cmp     cx,WF_16BIT+WF_STEREO
        jne     short BytesToSamplesExit

        shr     dx,1                    ; divide by 4
        rcr     ax,1

BytesToSamplesExit:
        ret

pcmBytesToSamples endp


;---------------------------Public-Routine------------------------------;
; SamplesToBytes - convert number of samples to number of bytes
;
; Entry:
;       DX:AX   - number of samples
;       BX      - wave format flags (WF_16BIT, WF_STEREO)
; Returns:
;       DX:AX   - number of bytes
; Error Returns:
;       none
; Registers Preserved:
;       BP,ES,DS,SI,DI
; Registers Destroyed:
;       AX,BX,CX,DX,FLAGS
; History:
;       Wed 04-Jan-1990 13:45:58 -by-  Todd Laney [ToddLa]
;	Created.
;-----------------------------------------------------------------------;
        assumes ds,nothing
        assumes es,nothing

SamplesToBytes proc near

        mov     cx,bx
        and     cx,WF_16BIT+WF_STEREO
        jz      short SamplesToBytesExit

        shl     ax,1                    ; multiply by 2
        rcl     dx,1

        cmp     cx,WF_16BIT+WF_STEREO
        jne     short SamplesToBytesExit

        shl     ax,1                    ; multiply by 4
        rcl     dx,1

SamplesToBytesExit:
        ret

SamplesToBytes endp


;---------------------------Public-Routine------------------------------;
; pcmSizeOutputData - convert size of output wave data given input
;
;   the wave data must be PCM format with the following:
;
;       SamplesPerSecond:       1 - 0FFFFh
;       BitsPerSample           8 or 16
;       Channels                1 or 2
;
; Entry:
;       pwfDst  - PCMWAVEFORMAT for destination
;       pwfSrc  - PCMWAVEFORMAT for source data
;       dwSize  - number of bytes (of source data) to convert
;
; Returns:
;       DX:AX   - size in bytes of output wave data
; Error Returns:
;       DX:AX   - 0
; Registers Preserved:
;       BP,DS,SI,DI
; Registers Destroyed:
;       AX,BX,CX,DX,ES,FLAGS
; History:
;       Wed 04-Jan-1990 13:45:58 -by-  Todd Laney [ToddLa]
;	Created.
;-----------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

cProc   pcmSizeOutputData, <FAR, PUBLIC, PASCAL>, <ds, esi, edi>
        ParmD   pwfDst
        ParmD   pwfSrc
        ParmD   NumBytesSrc

        LocalW  spsSrc                  ; samples per second
        LocalW  spsDst

        LocalW  wfSrc                   ; wave format flags
        LocalW  wfDst
cBegin

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        lds     si, pwfDst
        call    pcmGetWaveFormat
        jc      short SizeWaveDataError
        mov     spsDst,ax
        mov     wfDst,bx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        lds     si, pwfSrc
        call    pcmGetWaveFormat
        jc      short SizeWaveDataError
        mov     spsSrc,ax
        mov     wfSrc,bx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax,NumBytesSrc.lo
        mov     dx,NumBytesSrc.hi
        mov     bx,wfSrc
        call    pcmBytesToSamples

        movzx   ecx, spsDst     ; lDenominator
        movzx   ebx, spsSrc     ; lNumerator
        DXAXtoEAX               ; lNumber
        
        imul    ebx             ; edx:eax = (lNumber * lNumerator)
        idiv    ecx             ; eax     = (lNumber * lNumerator) / lDenominator

        shld    edx, eax, 16    ; covert eax to dx:ax for 16 bit programs

        mov     bx,wfDst
        call    SamplesToBytes
        jmp     short SizeWaveDataExit

SizeWaveDataError:
        mov     ax,-1
        cwd

SizeWaveDataExit:

cEnd


;---------------------------------------------------------------------------;
;
;   DWORD FAR PASCAL pcmConvert(pwfDst, pbDst, pwfSrc, pbSrc, dwNumBytesSrc)
;
;   Description:
;       the wave data must be PCM format with the following:
;
;           SamplesPerSecond:       1 - 0FFFFFFFFh
;           BitsPerSample           8 or 16
;           Channels                1 or 2
;
;   Entry:
;       pwfDst  - PCMWAVEFORMAT for destination
;       pbDst   - destination data
;       pwfSrc  - PCMWAVEFORMAT for source data
;       pbSrc   - source data
;       dwSize  - number of bytes (of source data) to convert
;
;   Exit:
;
;   Uses:
;       Flags, AX, BX, CX, DX, ES
;
;   History:
;       11/16/92    cjp     [curtisp] 
;
;---------------------------------------------------------------------------;

        assumes ds, Data
        assumes es, nothing

        ALIGN   4

cProc pcmConvert, <FAR, PUBLIC, PASCAL>, <ds, esi, edi>
        ParmD   pwfDst
        ParmD   pbDst
        ParmD   pwfSrc
        ParmD   pbSrc
        ParmD   dwSrcLenBytes

        LocalD  dwSrcSamples            ; number of samples

        LocalD  spsSrc                  ; samples per second
        LocalD  spsDst

        LocalW  wfSrc                   ; wave format flags
        LocalW  wfDst

        LocalD  fnReadSample            ; function to read a sample
        LocalD  fnWriteSample           ; function to write a sample

        LocalD  dwCurSample
        LocalD  dwDecSample
cBegin

        cld                             ; just in case
        xor     esi, esi
        xor     edi, edi

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, dwSrcLenBytes.lo
        or      ax, dwSrcLenBytes.hi
        jz      short Conv_ExitV

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        lds     si, pwfDst
        call    pcmGetWaveFormat
        jnc     short Conv_NoError

        public Conv_Error
Conv_Error:

        mov     ax, -1

        public Conv_ExitV
Conv_ExitV:

        cwd
        jmp     Conv_Exit

        public Conv_NoError
Conv_NoError:

        mov     spsDst.lo, ax
        mov     spsDst.hi, dx
        mov     wfDst, bx
        movzx   ebx, bx
        mov     eax, WriteSample_Table[ebx]
        mov     fnWriteSample, eax

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        lds     si, pwfSrc
        call    pcmGetWaveFormat
        jc      Conv_Error

        mov     spsSrc.lo, ax
        mov     spsSrc.hi, dx
        mov     wfSrc, bx
        shl     bx, 2
        errnz WF_STEREO-2
        errnz WF_16BIT-4
        or      bx, wfDst
        movzx   ebx, bx
        mov     eax, ReadSample_Table[ebx]
        mov     fnReadSample, eax

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ax, dwSrcLenBytes.lo
        mov     dx, dwSrcLenBytes.hi
        mov     bx, wfSrc
        call    pcmBytesToSamples

        mov     dwSrcSamples.lo, ax
        mov     dwSrcSamples.hi, dx

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   all set to convert the wave data, load the pointers and either
;   do a major or a minor DDA
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        lds     si, pbSrc
        les     di, pbDst

        mov     ebx, spsSrc
        mov     eax, spsDst

        cmp     ebx, eax
        jbe     short Conv_DDAMajor


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   we have a minor axis DDA, ie source sps > dest sps
;
;       eax = spsDst
;       ebx = spsSrc
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        public Conv_DDAMinor
Conv_DDAMinor:

        shr     ebx, 1
        mov     dwCurSample, ebx        ; start at spsSrc/2
        mov     dwDecSample, eax        ; decrement by spsDst

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   register use:
;
;       ES:DI   -> dest buffer
;       DS:SI   -> source buffer
;       SS:BP   -> stack frame
;       EAX     - sample
;       EBX     - dwDecSample
;       ECX     - loop count
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ebx, dwDecSample
        mov     ecx, dwSrcSamples

        ALIGN   4

        public Conv_DDAMinorLoop
Conv_DDAMinorLoop:

        call    [fnReadSample]

        dec     ecx
        jz      short Conv_Done

        sub     dwCurSample, ebx
        jnc     Conv_DDAMinorLoop

        call    [fnWriteSample]

        mov     eax, spsSrc
        add     dwCurSample, eax
        jmp     short Conv_DDAMinorLoop



;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   we have a major axis DDA, ie source sps <= dest sps
;       eax = spsDst
;       ebx = spsSrc
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        public Conv_DDAMajor
Conv_DDAMajor:

        shr     eax, 1
        mov     dwCurSample, eax        ; start at spsDst/2
        mov     dwDecSample, ebx        ; decrement by spsSrc

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   register use:
;
;       ES:DI   -> dest buffer
;       DS:SI   -> source buffer
;       SS:BP   -> stack frame
;       EAX     - sample
;       EBX     - dwDecSample
;       ECX     - loop count
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        mov     ebx, dwDecSample
        mov     ecx, dwSrcSamples


        ALIGN   4

        public Conv_DDAMajorLoop
Conv_DDAMajorLoop:

        call    [fnReadSample]

@@:     call    [fnWriteSample]
        sub     dwCurSample, ebx
        jnc     @B

        mov     eax, spsDst
        add     dwCurSample, eax

        dec     ecx
        jnz     Conv_DDAMajorLoop
        errn$ Conv_Done


;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;
;
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        public Conv_Done
Conv_Done:

        mov     ax, dwSrcLenBytes.lo
        mov     dx, dwSrcLenBytes.hi

        public Conv_Exit
Conv_Exit:

cEnd


;---------------------------Public-Routine------------------------------;
; ReadSample - read a sample from wave data
;
; Entry:
;       DS:SI   -- > wave data
; Returns:
;       DX:AX   - sample
;       DS:SI   - advanced to next sample
; Error Returns:
;       none
; Registers Preserved:
;       BP,ES,CX,BX,DI
; Registers Destroyed:
;       AX,DX,DS,SI,FLAGS
; History:
;       Wed 04-July-1992 13:45:58 -by-  Curtis Palmer [CurtisP]
;	Created.
;-----------------------------------------------------------------------;

        assumes ds, nothing
        assumes es, nothing

ReadSample proc near

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   read a Mono 8 bit sample, from all formats
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        ALIGN   4

        public ReadSample_M08M08
ReadSample_M08M08:
        mov     al, bptr ds:[esi]
        inc     esi
        ret

        ALIGN   4

        public ReadSample_M08S08
ReadSample_M08S08:
        mov     ax, wptr ds:[esi]
        add     esi, 2
        add     al, ah
        rcr     al, 1
        ret

        ALIGN   4

        public ReadSample_M08M16
ReadSample_M08M16:
        mov     ax, wptr ds:[esi]
        add     esi, 2
        mov     al, ah
        add     al, 80h
        ret

        ALIGN   4

        public ReadSample_M08S16
ReadSample_M08S16:
        mov     eax, dptr ds:[esi]
        add     esi, 4
        mov     dx, ax
	add     dx, 08000h
        ror     eax, 16
	add  	ax, 08000h
        add     ax, dx
        rcr     ax, 1
        mov     al, ah
        ret

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   read a Stereo 8 bit sample, from all formats
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        ALIGN   4

        public ReadSample_S08M08
ReadSample_S08M08:
        mov     al, bptr ds:[esi]
        inc     esi
        mov     ah, al
        ret

        ALIGN   4

        public ReadSample_S08S08
ReadSample_S08S08:
        mov     ax, wptr ds:[esi]
        add     esi, 2
        ret

        ALIGN   4

        public ReadSample_S08M16
ReadSample_S08M16:
        mov     ax, wptr ds:[esi]
        add     esi, 2
        mov     al, ah
        add     al, 80h
        mov     ah, al
        ret

        ALIGN   4

        public ReadSample_S08S16
ReadSample_S08S16:
        mov     eax, dptr ds:[esi]
        add     esi, 4
        mov     dx, ax
        ror     eax, 16
        mov     al, ah
        mov     ah, dh
        xor     ax, 8080h
        ret

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   read a Mono 16 bit sample, from all formats
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        ALIGN   4

        public ReadSample_M16M08
ReadSample_M16M08:
        mov     al, bptr ds:[esi]
        inc     esi
        sub     al, 80h
        mov     ah, al
        xor     al, al
        ret

        ALIGN   4

        public ReadSample_M16S08
ReadSample_M16S08:
        mov     ax, wptr ds:[esi]
        add     esi, 2
        xor     ax, 8080h
        add     ah, al
        rcr     ah, 1
        xor     al, al
        ret

        ALIGN   4

        public ReadSample_M16M16
ReadSample_M16M16:
        mov     ax, wptr ds:[esi]
        add     esi, 2
        ret

        ALIGN   4

        public ReadSample_M16S16
ReadSample_M16S16:
        mov     eax, dptr ds:[esi]
        add     esi, 4
        mov     dx, ax
	add     dx, 08000h
        ror     eax, 16
	add  	ax, 08000h
        add     ax, dx
        rcr     ax, 1
	sub	ax, 08000h
        ret

;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;
;   read a Stereo 16 bit sample, from all formats
;- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -;

        ALIGN   4

        public ReadSample_S16M08
ReadSample_S16M08:
        mov     al, bptr ds:[esi]
        inc     esi
        sub     al, 80h
        mov     ah, al
        xor     al, al
        mov     dx, ax
        ret

        ALIGN   4

        public ReadSample_S16S08
ReadSample_S16S08:
        mov     ax, wptr ds:[esi]
        add     esi, 2
        xor     ax, 8080h
        mov     dh, ah
        mov     ah, al
        xor     al, al
        xor     dl, dl
        ret

        ALIGN   4

        public ReadSample_S16M16
ReadSample_S16M16:
        mov     ax, wptr ds:[esi]
        add     esi, 2
        mov     dx, ax
        ret

        ALIGN   4

        public ReadSample_S16S16
ReadSample_S16S16:
        mov     eax, dptr ds:[esi]
        add     esi, 4
        mov     dx, ax
        ror     eax, 16
        xchg    dx, ax
        ret

ReadSample endp


;---------------------------Public-Routine------------------------------;
; WriteSample - write a sample to wave data
;
; Entry:
;       DX:AX   - sample
;       ES:DI   -- > wave data
; Returns:
;       ES:DI   - advanced to next sample
; Error Returns:
;       none
; Registers Preserved:
;       AX,BX,CX,DX,BP,DS,SI
; Registers Destroyed:
;       ES,DI,FLAGS
; History:
;       Wed 04-July-1992 13:45:58 -by-  Curtis Palmer [CurtisP]
;	Created.
;-----------------------------------------------------------------------;

        assumes ds,nothing
        assumes es,nothing

WriteSample proc near

        ALIGN   4

        public WriteSample_M08
WriteSample_M08:
        mov     bptr es:[edi], al
        inc     edi
        ret

        ALIGN   4

        public WriteSample_S08
WriteSample_S08:

        public WriteSample_M16
WriteSample_M16:
        mov     wptr es:[edi], ax
        add     edi, 2
        ret

        ALIGN   4

        public WriteSample_S16
WriteSample_S16:
        ror     eax, 16
        shrd    eax, edx, 16
        mov     dptr es:[edi], eax
        add     edi, 4
        ret

WriteSample endp

sEnd CodeSeg

        end
