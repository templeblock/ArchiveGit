;***************************************************************************
;*
;*      Filename: compbin.asm
;* 
;*      Description: This file contains the assembly laguage module for
;*              CCITT Group 3 FAX data compression routine.
;* 
;*      Creation Date:  3/2/1987
;* 
;*      Modification History:
;*       
;* 
;*
;*     This source code is provided courtesy of Hewlett-Packard     **
;*     Company, Greeley Division.                                   **          			                                
;*     Licensed by Hewlett-Packard and Microsoft to developers for  **									
;*     use with Microsoft Windows.  Please consult your license     **					
;*     agreement concerning your rights to modify and distribute.   **								
;*								    **
;*     Copyright Hewlett-Packard Company, 1987.			    **
;********************************************************************/

;******************************************************************************
;*                                   COMPBIN                                  *
;******************************************************************************
;*                                                                            *
;* Status = compbin(InputBuffer,OutputBuffer,ScratchBuffer,LineCount,         *
;*              BytesPerLine,ImageWidth,LinesPerBuffer,BufferSize,WhiteValue, *
;*              File);                                                        *
;*                                                                            *
;*     int Status Zero if the operation was completed with no errors,         *
;*              negative if an error occured                                  *
;*      char *InputBuffer Pointer to a buffer holding the data to be          *
;*              compressed                                                    *
;*      char *OutputBuffer Pointer to a buffer for holding the compressed     *
;*              data as it is written to the file                             *
;*      struct record *ScratchBuffer Pointer to a buffer to hold the runs of  *
;*              bits as they areecoded from the data. This buffer must be a   *
;*              maximum of 4 * bits per line of the image bytes in size       *
;*      int LineCount Number of lines to comppress into the file from the     *
;*              input buffer                                                  *
;*      int BytesPerLine Number of bytes of data in a line                    *
;*      int ImageWidth Number of pixels in a line of the image                *
;*      int LinesPerBuffer Number of lines in the input buffer                *
;*      int BufferSize Number of bytes in the output buffer                   *
;*      int WhiteValue This is used to determine what value is written in the *
;*              input buffer to represent white                               *
;*                      0: 0 is white, 1 is black                             *
;*                      1: 1 is white, 0 is black                             *
;*      int File DOS File handle for the file in which to write the           *
;*              compressed data. The file pointer for the file should point   *
;*              the location to write the first byte of data for the          *
;*              compressed line                                               *
;*                                                                            *
;******************************************************************************
;*                                                                            *
;*              This routine is used to compress data with the TIFF standard  *
;*      adaptation of CCITT Group 3 FAX Encoding (TIFF Compression method 2)  *
;*      The data is compressed from the input buffer into the current         *
;*      location of the file. LineCount lines are compressed and the data     *
;*      wrriten into the file                                                 *
;*                                                                            *
;******************************************************************************

;********************************************************************
;*   COMPBIN.asm - assembly code for COMPBIN procedure for the 
;*                  RGCOMPRESSDATA library call
;********************************************************************

?PLM = 0
?WIN = 0
memH          EQU  1
                        
.xlist
include c:\include\cmacros.inc
.list

UNCOMPRESSABLEFILE  EQU  -404
ACCESSDENIED        EQU  -405
NODISCSPACE         EQU  -406

COLORW         EQU  0ffh
COLORB         EQU  000h
CCITTPHOTOVAL  EQU  01h

extrn     _DosWrite:far
extrn     _blktable:word
extrn     _whttable:word

sBegin  DATA
bits_in   dw   0
old       db   0
bytes_out DW   0
obuff     DD   ?
outptr    DW   ?
MAX_BYTES DW   ?
out_bytes DD   0
DstHandle DW   ?
sEnd    DATA

sBegin  CODE

assumes CS,CODE
assumes DS,DATA

;---------------------------------------------------
;*** int compbin( in_buff, out_buff, rec,
;                 num_lines, bytes_line, bits_line, linesperbuffer,
;                 bufsize, Photometric, DHandle );
;
;       returns the status
;
cProc compbin,<PUBLIC,FAR>,<SI,DI,ES,DS>
          parmD in_buff
          parmD out_buff
          parmD rec
          parmW num_lines
          parmW bytes_line
          parmW bits_line
          parmW linesperbuffer
          parmW bufsize
          parmW Photometric
          parmW DHandle
          LocalW in_bytes_l        ; total number of bytes read
          LocalW in_bytes_h
          LocalW inptr
          LocalW row               ; number of lines left to compress
          LocalW linesin
cBegin
          mov  ax,OFF_in_buff
          mov  inptr,ax
          mov  ax,num_lines
          mov  linesin,ax
          imul bytes_line
          mov  WORD PTR in_bytes_l,ax
          mov  WORD PTR in_bytes_h,dx
          mov  ax,OFF_in_buff      ;     inptr = in_buff;
          mov  inptr,ax
          mov  WORD PTR out_bytes,0
          mov  WORD PTR out_bytes+2,0
          mov  bytes_out,0
          mov  bits_in,0
          mov  old,0
          mov  ax,OFF_out_buff
          mov  WORD PTR obuff,ax
          mov  outptr,ax
          mov  ax,SEG_out_buff
          mov  WORD PTR obuff+2,ax
          mov  ax,DHandle
          mov  DstHandle,ax
          mov  ax,bufsize
          mov  MAX_BYTES,ax

; check the photometric interpretation to see if the current definition of
; WHITE is a 1 and BLACK is a 0
          cmp  WORD PTR Photometric,CCITTPHOTOVAL
          je   photoOK
          push ds
          mov  ax,bytes_line       ; get number of bytes to invert
          imul num_lines
          mov  cx,ax
          lds  si,in_buff          ; get address of the input buffer
invert:
          not  BYTE PTR [si]       ; invert the bits
          inc  si
          loop invert
          pop  ds
photoOK:

          jmp  CheckWhile

Whileloop:
DOLINE:
;**************************************************
;*** begin get_col_chgs()
;**************************************************
;
;    register bx = cp
;    register di = run
;    register es = segment of input line
;    register si = offset of input line
;    register dx = bits_used
;    register ah = color
;    register al = byte being tested
          mov  bx,OFF_rec          ; cp = rec;
          mov  ah,COLORW           ; color = COLORW;
          sub  di,di               ; run = 0;
          mov  dx,bits_line        ; bits_used = bits_line;
          mov  es,SEG_in_buff
          mov  si,inptr            ; setup SI register with input line
          jmp  $L20010
$WC143:
          cmp  dx,8                ;   while(( XX ) && (bits_used >= 8 ))
          jl   $WB144              ;     {
          add  di,8                ;     run += 8;
          sub  dx,8                ;     bits_used -= 8;
          inc  si                  ;     line++;
$L20008:                           ;     }
          mov  al,es:[si]          ;   while((*line == bytemask) && ( XX ))
          cmp  ah,al               ;bytemask
          je   $WC143
$WB144:
          cmp  dx,0                ;  if(bits_used == 0)
          jne  $I145
$L20009:                           ;     {
          push ds
          mov  ds,SEG_rec
          mov  [bx+2],ah           ;     cp->color = color;
          mov  [bx],di             ;     cp->position = run;
          pop  ds
          jmp  SHORT $WB142        ;     break;
$I145:                             ;     }
          mov  cx,8                ; set up CX for bit testing
          cmp  ah,COLORW           ; is the color COLORW ?
          je   White
          jmp  SHORT Black
SetWhite:
          dec  dx                  ;     bits_used--;
          dec  cx
          push ds
          mov  ds,SEG_rec
          mov  [bx+2],ah           ;     cp->color = color;
          mov  ah,COLORW                ;     color = COLORW
          mov  [bx],di             ;     cp->position = run;
          pop  ds
          add  bx,4                ;     cp++;
          mov  di,1                ;     run = 1;
          jcxz $FC147              ; finished last bit of byte
          cmp  dx,0
          je   $FC147
White:
          rcl  al,1
          jnc  SetBlack            ; color has changed to COLORB
          inc  di                  ;     run++;
          dec  dx                  ;     bits_used--;
          jz   $FC147
          loop White               ; test next bit
          jmp  SHORT $FC147
SetBlack:
          dec  dx                  ;     bits_used--;
          dec  cx
          push ds
          mov  ds,SEG_rec
          mov  [bx+2],ah           ;     cp->color = color;
          mov  ah,COLORB           ;     color = COLORB
          mov  [bx],di             ;     cp->postion = run;
          pop  ds
          add  bx,4                ;     cp++;
          mov  di,1                ;     run = 1;
          jcxz $FC147              ; finished last bit of byte
          cmp  dx,0
          je   $FC147
Black:
          rcl  al,1
          jc   SetWhite            ; color has changed to COLORW
          inc  di                  ;     run++;
          dec  dx                  ;     bits_used--;
          jz   $FC147
          loop Black
$FC147:
          inc  si                  ;   line++;
          cmp  dx,0                ;   if(bits_used == 0)
          je   $L20009
$L20010:
          cmp  dx,0                ; while(bits_used > 0)
          jle  $JCC512             ;   {
          jmp  $L20008
$JCC512:
$WB142:
          mov  WORD PTR inptr,si   ; save new position in in_buff
          dec  linesin             ; reduce number of lines to do
          dec  row
;**********************************************
;* END OF GET_LINE
;**********************************************
;    register DX = j
          mov  dx,0                ; for (j = 0, cp=rec[0]; j < bits_line; cp++)
          les  di,rec
          jmp  SHORT $L20007       ;   {
$F905:
          push WORD PTR es:[di+2]     ;   encode(cp->position, cp->color);
          push WORD PTR es:[di]
          call encode
          jnc  noerror
          jmp  compbin_abort
noerror:
          add  sp,4
          mov  ax,es:[di]             ;   j += cp->position;
          add  dx,ax
          add  di,4                ;   }
$L20007:
          mov  ax,bits_line
          cmp  dx,ax
          jl   $F905
          xor  ax,ax
          push ax
          push ax
          call pack_bits           ;   pack_bits(0,0);
          jc   compbin_abort
          add  sp,4                ;   }
CheckWhile:
          cmp  linesin,0
          je   Done             
          jmp  Whileloop
Done:
          cmp  bytes_out,0         ; if (bytes_out != 0)
          je   $I910
          push bytes_out           ;save bytes count
          push bytes_out           ; out_bytes += DosWrite( DstHandle,
          push SEG_out_buff        ;                   out_buff, bytes_out );
          push OFF_out_buff
          push DHandle
          call _DosWrite           ;     DosWrite(DstHandle, out_buff,
          jc   compbin_abort
          add  sp,8                ;              bytes_out);
          pop  dx
          cmp  ax,dx
          jne  error2
          cwd  
          add  WORD PTR out_bytes,ax
          adc  WORD PTR out_bytes+2,dx
$I910:
          mov  ax,WORD PTR out_bytes+2       ; check for file expansion
          cmp  ax,WORD PTR in_bytes_h
          mov  ax,0
          jl   exit
          jg   expandedfile
          mov  ax,WORD PTR out_bytes
          cmp  ax,WORD PTR in_bytes_l
          mov  ax,0
          jb   exit
expandedfile:
          mov  ax,UNCOMPRESSABLEFILE
          jmp  SHORT exit
error2:
          mov  ax,NODISCSPACE
exit:
compbin_abort:
cEnd



;--------------------------------------------------
;*** int encode(run, color)
;         int run, color;
;
;       returns nothing

cProc encode,<NEAR>,<DX,DI,ES>
          parmW run
          parmW color
cBegin
;    register si = low
;    register di = high
          mov  di,run              ; run
CheckTooBig:
                cmp     di,2560                 ; check for max CCITT code word
                jle     NotTooBig
                sub     di,2560                 ; subtract the MAX CCITT run length
          mov  dx,color
                cmp  dl,COLORB           ; if(color == COLORB)
          jne  $DC100              ;   {
          push WORD PTR _blktable[414]
          push WORD PTR _blktable[412]
          call pack_bits
          jc   encode_abort
          add  sp,4
                jmp     CheckTooBig
$DC100:
          push WORD PTR _whttable[414]
          push WORD PTR _whttable[412]
          call pack_bits
          jc   encode_abort
          add  sp,4
                jmp     CheckTooBig
NotTooBig:
          mov  si,di               ; low
          mov  cl,6
          shr  di,cl
          and  si,03fh             ; low = run & 0x003f;
          mov  dx,color
          cmp  dl,COLORB           ; if(color == COLORB)
          jne  $I157               ;   {
          or   di,di               ;   if(high)
          je   $I158
          shl  di,1                ;     pack_bits(blktable[63+high].nbits,
          shl  di,1                ;         blktable[63+high].bits);
          push WORD PTR _blktable[di+254]
          push WORD PTR _blktable[di+252]
          call pack_bits
          jc   encode_abort
          add  sp,4
$I158:
          shl  si,1                ;     pack_bits(blktable[low].nbits,
          shl  si,1                ;         blktable[low].bits);
          push WORD PTR _blktable[si+2]
          push WORD PTR _blktable[si]
          jmp  SHORT $L20012
$I157:
          or   di,di               ;   if(high)
          je   $I160               ;     {
          shl  di,1                ;     pack_bits(whttable[63+high].nbits,
          shl  di,1                ;         whttable[63+high].bits);
          push WORD PTR _whttable[di+254]
          push WORD PTR _whttable[di+252]
          call pack_bits
          jc   encode_abort
          add  sp,4
$I160:
          shl  si,1                ;     pack_bits(whttable[low].nbits,
          shl  si,1                ;         whttable[low].bits);
          push WORD PTR _whttable[si+2]
          push WORD PTR _whttable[si]
$L20012:
          call pack_bits
          jc   encode_abort
          add  sp,4
encode_abort:
cEnd


;--------------------------------------------------
;*** int pack_bits(nbits, bits)
;         int nbits,bits;
;
;       returns nothing

cProc pack_bits,<NEAR>,<SI,DI>
          parmW nbits
          parmW bits
cBegin
          mov  si,nbits            ; number of code word bits - nbits
          mov  ax,bits             ; code word bits - bits
          cmp  si,0                ; if(nbits == 0)
          jne  $L20014
$JCC668:                           ;   {
          cmp  bits_in,0           ;   if(!bits_in)
          je   $EX387
$JCC678:                           ;     {
          mov  ah,old
          call put_line            ;     put_line(old);
          jc   pack_bits_abort
          mov  bits_in,0           ;     bits_in = 0;
          jmp  SHORT $EX387        ;     }
$L20014:
          or   si,si               ; while(nbits > 0)
          jle  $JCC857
$WC397:
          cmp  bits_in,0           ; if(!bits_in)
          jne  $I399
          cmp  si,8                ;   if(nbits >= 8)
          jl   $WC398              ;     {
          call put_line            ;     put_line(HI(bits));   pass AH
          jc   pack_bits_abort
          sub  si,8                ;     nbits -= 8;
          xchg al,ah               ;     HI(bits) = LOW(bits);
                                   ;     }
                                   ;   else
$WC398:                            ;     {
          mov  byte ptr old,ah     ;     old = HI(bits);
          mov  word ptr bits_in,si ;     bits_in = nbits;
          xor  si,si               ;     nbits = 0;
          jmp  SHORT $L20014       ;     }
                                   ;  else
$I399:                             ;     {
          mov  cx,8
          sub  cx,bits_in          ;     bits_to_fill = 8 - bits_in;
          cmp  si,cx               ;     if(nbits < bits_to_fill)
          jg   $I400               ;       {
          mov  cx,bits_in
          shr  ax,cl
          or   old,ah              ;       old |= nbits >> bits_in;
          add  bits_in,si          ;       bits_in += nbits;
          sub  si,si               ;       nbits = 0;
          jmp  SHORT $L20014       ;       }
                                   ;     else     
$I400:                             ;       {
          mov  bx,ax               ;       temp = HI(bits);
          sub  si,cx               ;       nbits -= bits_to_fill;
          shl  ax,cl               ;       bits <<= bits_to_fill;
          mov  cx,bits_in
          shr  bx,cl
          or   old,bh              ;       old |= temp >> bits_in;
          mov  bits_in,0           ;       bits_in = 0;
          push ax                  ; save bits in AX register
          mov  ah,old
          call put_line            ;       put_line(old);
          jc   pack_bits_abort
          pop  ax                  ; restore bits in AX register
          jmp  SHORT $L20014       ;       }
$JCC857:                           ;     }
$EX387:
pack_bits_abort:
cEnd


;--------------------------------------------------
;*** put_line()
;                   input:    ah - byte to be put
;       output:     returns nothing

cProc put_line,<NEAR>,<>
cBegin
          push bp
          mov  bp,sp
          mov  es,WORD PTR obuff+2
          mov  bx,outptr          ; *outptr++ = old;   old in AH register
          inc  WORD PTR outptr
          mov  es:[bx],ah
          inc  bytes_out          ; bytes_out++;
          mov  bx,MAX_BYTES       ; if(bytes_out == MAX_BYTES)
          cmp  bytes_out,bx
          jne  $I168              ;   {
          push ax
          push bytes_out          ;   out_bytes += DosWrite(DstHandle
          push bytes_out          ;   out_bytes += DosWrite(DstHandle
          push WORD PTR obuff+2   ;segment
          push WORD PTR obuff     ;offset   ,out_buff,bytes_out);
          push DstHandle
          call _DosWrite
          jc   put_line_abort
          add  sp,8
          pop  dx
          cmp  ax,dx
          je   countOK
          mov  ax,NODISCSPACE      ; out of disc space
          stc
          jmp  put_line_abort
countOK:
          xor  dx,dx               ; zero out dx reg for dword add to follow    
          add  WORD PTR out_bytes,ax
          adc  WORD PTR out_bytes+2,dx
          mov  bytes_out,0         ;   bytes_out = 0;
          mov  ax,WORD PTR obuff   ;   outptr = out_buff;
          mov  WORD PTR outptr,ax  ;   }
          pop  ax
$I168:
          clc
exit1:
put_line_abort:
          mov  sp,bp
          pop  bp
cEnd

sEnd    CODE

end
