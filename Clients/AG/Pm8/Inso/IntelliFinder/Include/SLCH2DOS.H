/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
*        slch2dos.h - IBM PC Extended ASCII character set defines.      *
*                     Slavic (Latin-2)                                  *  
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the IBM PC
            Extended ASCII character set for Slavic (Latin-2). It also 
            defines some characters that do not exist in this character 
            set but are essential for the multilingual language software 
            products.

            Some nonessential characters are defined as P2_FIL_#.

        Cautions:
            None.

        Creation Date:
            08-dec-92   PJA

        Change Record:


*-----------------------------------------------------------------------*/
#ifndef SLCH2DOS_H

#define SLCH2DOS_H


/* IBM PC CHARACTER SET DEFINES SLAVIC (LATIN II)                       */

/* P2_C_... = PC CAPITAL ...                                            */
/* P2_L_... = PC LOWER   ...                                            */

#define P2_C_C_CEDILLA        128
#define P2_L_U_UMLAUT         129
#define P2_L_E_ACUTE          130
#define P2_L_A_CIRCUMFLX      131
#define P2_L_A_UMLAUT         132
#define P2_L_U_RING           133
#define P2_L_C_ACUTE          134
#define P2_L_C_CEDILLA        135
#define P2_L_L_SLASH          136
#define P2_L_E_UMLAUT         137
#define P2_C_O_DOUBLE_ACUTE   138
#define P2_L_O_DOUBLE_ACUTE   139
#define P2_L_I_CIRCUMFLX      140
#define P2_C_Z_ACUTE          141
#define P2_C_A_UMLAUT         142
#define P2_C_C_ACUTE          143

#define P2_C_E_ACUTE          144
#define P2_C_L_ACUTE          145
#define P2_L_L_ACUTE          146
#define P2_L_O_CIRCUMFLX      147
#define P2_L_O_UMLAUT         148
#define P2_C_L_HACEK          149
#define P2_L_L_HACEK          150
#define P2_C_S_ACUTE          151
#define P2_L_S_ACUTE          152
#define P2_C_O_UMLAUT         153
#define P2_C_U_UMLAUT         154
#define P2_C_T_HACEK          155
#define P2_L_T_HACEK          156
#define P2_C_L_SLASH          157
#define P2_MULTIPLICATION     158
#define P2_L_C_HACEK          159

#define P2_L_A_ACUTE          160
#define P2_L_I_ACUTE          161
#define P2_L_O_ACUTE          162
#define P2_L_U_ACUTE          163
#define P2_C_A_OGONEK         164
#define P2_L_A_OGONEK         165
#define P2_C_Z_HACEK          166
#define P2_L_Z_HACEK          167
#define P2_C_E_OGONEK         168
#define P2_L_E_OGONEK         169
#define P2_NOT_SIGN           170   /* Alt-170 appears as a blank spot in   */
                                    /* Microsoft MSDOS v5.0 user's guide and*/
                                    /* reference book for Slavic (Latin II) */
                                    /* but appears as a NOT SIGN character  */
                                    /* in all other tables. This should be  */
                                    /* verified as soon as anyone has the   */
                                    /* chance                               */

#define P2_L_Z_ACUTE          171
#define P2_C_C_HACEK          172
#define P2_L_S_CEDILLA        173
#define P2_LEFT_GUILL         174
#define P2_RIGHT_GUILL        175

#define P2_LT_S_GUILL         176  /* Graphic character; substitutes for    */
                                   /* left single guillemet                 */

#define P2_RT_S_GUILL         177  /* Graphic character; substitutes for    */
                                   /* right single guillemet                */

#define P2_L9_S_QUOTE         178  /* Graphic character; substitutes for    */
                                   /* low 9-like single quote               */
#define P2_L99_D_QUOTE        179  /* Graphic character; substitutes for    */
                                   /* low 99-like double quote              */
#define P2_ELLIPSIS           180  /* Graphic character; substitutes for    */
                                   /* a three-point ellipsis                */

#define P2_C_A_ACUTE          181

#define P2_C_A_CIRCUMFLX      182 
#define P2_C_E_HACEK          183 
#define P2_C_S_CEDILLA        184

#define P2_6_S_QUOTE          185  /* Graphic character; substitutes for    */
                                   /* 6-like single quote                   */
#define P2_9_S_QUOTE          186  /* Graphic character; substitutes for    */
                                   /* 9-like single quote                   */

#define P2_66_D_QUOTE         187  /* Graphic character; substitutes for    */
                                   /* 66-like double quote                  */
#define P2_99_D_QUOTE         188  /* Graphic character; substitutes for    */
                                   /* 99-like double quote                  */

#define P2_C_Z_DOT            189
#define P2_L_Z_DOT            190

#define P2_N_DASH             191  /* Graphic character; substitutes for    */
                                   /* En dash                               */
#define P2_M_DASH             192  /* Graphic character; substitutes for    */
                                   /* em dash                               */
#define P2_TRADEMARK          193  /* Graphic character; substitutes for    */
                                   /* Trademark                             */
#define P2_BROKEN_VBAR        194  /* Graphic character; substitutes for    */
                                   /* broken vertical bar character         */
#define P2_COPYRIGHT          195  /* Graphic character; substitutes for    */
                                   /* copyright symbol                      */
#define P2_FIL_1              196  /* Forms light horizontal                */
#define P2_REG_TRADEMARK      197  /* Graphic character; substitutes for    */
                                   /* registered trademark symbol           */
#define P2_C_A_BREVE          198
#define P2_L_A_BREVE          199   

#define P2_MICRO_SIGN         200  /* Graphic character; substitutes for    */
                                   /* micro sign                            */
#define P2_PARAGRAPH          201  /* Graphic character; substitutes for    */
                                   /* paragraph symbol                      */
#define P2_MIDDLE_DOT         202  /* Graphic character; substitutes for    */
                                   /* spacing dot above character           */
#define P2_PLUS_MINUS         203  /* Graphic character; substitutes for    */
                                   /* plus-minus sign                       */
#define P2_FIL_2              204
#define P2_DAGGER             205  /* Graphic character; substitutes for    */
                                   /* dagger                                */
#define P2_D_DAGGER           206  /* Graphic character; substitutes for    */
                                   /* double dagger                         */

#define P2_GEN_CURRENCY       207  
#define P2_L_D_BAR            208
#define P2_C_D_BAR            209  
#define P2_C_D_HACEK          210
#define P2_C_E_UMLAUT         211
#define P2_L_D_HACEK          212
#define P2_C_N_HACEK          213
#define P2_C_I_ACUTE          214
#define P2_C_I_CIRCUMFLX      215
#define P2_L_E_HACEK          216

#define P2_FIL_3              217
#define P2_FIL_4              218
#define P2_FIL_5              219
#define P2_FIL_6              220

#define P2_C_T_CEDILLA        221
#define P2_C_U_RING           222 

#define P2_PER_MILLE          223  /* Graphic character; substitutes for    */
                                   /* per mille sign                        */ 

#define P2_C_O_ACUTE          224
#define P2_L_S_SHARP          225
#define P2_C_O_CIRCUMFLX      226
#define P2_C_N_ACUTE          227  
#define P2_L_N_ACUTE          228  
#define P2_L_N_HACEK          229  
#define P2_C_S_HACEK          230                                          
#define P2_L_S_HACEK          231  
#define P2_C_R_ACUTE          232  
#define P2_C_U_ACUTE          233  
#define P2_L_R_ACUTE          234  
#define P2_C_U_DOUBLE_ACUTE   235  
#define P2_L_Y_ACUTE          236  
#define P2_C_Y_ACUTE          237  
#define P2_L_T_CEDILLA        238  

#define P2_ACUTE              239
#define P2_SHY                240

#define P2_DOUBLE_ACUTE       241
#define P2_OGONEK             242
#define P2_HACEK              243
#define P2_BREVE              244
#define P2_SECTION            245
#define P2_DIVISION           246
#define P2_CEDILLA            247
#define P2_DEGREE             248                                          
#define P2_UMLAUT             249
#define P2_SPACING_DOT_ABOVE  250
#define P2_L_U_DOUBLE_ACUTE   251
#define P2_C_R_HACEK          252
#define P2_L_R_HACEK          253

#define P2_BULLET             254  /* Graphic character; substitutes for    */
                                   /* bullet                                */

#define P2_NBSP               255

#endif /* SLCH2DOS_H */

/* End of file - slch2dos.h */
