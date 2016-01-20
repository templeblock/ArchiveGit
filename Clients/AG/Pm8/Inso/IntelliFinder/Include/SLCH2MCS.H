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
*        slch2mcs.h - Master Character Set 2 Header File                *
*                     The InfoSoft Latin-2 Character Set defines*
*                     (Slavic)                                          *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:

            This file contains #defines based on the ISO Latin-2
            Character Set. Character points from 144 to 159 and
            character points from 160 to 175 are swapped for easier case
            conversion, i.e., 32 points apart. It also defines some
            characters that do not exist in this character set but are
            essential for the multilingual language software products.

            This is the InfoSoft default character table. 
            Some nonessential characters are defined as D2_FIL_#.


        Cautions:
            None.

        Creation Date:
            08-dec-92   PJA

        Change Record:


*-----------------------------------------------------------------------*/
#ifndef SLCH2MCS_H

#define SLCH2MCS_H


/* InfoSoft Latin-2 character set defines                                   */
/* D2_C_...       HMC LATIN-2 DEFAULT CAPITAL ...                       */
/* D2_L_...       HMC LATIN-2 DEFAULT LOWER   ...                       */


#define D2_FIL_1             128  /* HMC reserved.                       */
#define D2_FIL_2             129  /* HMC reserved.                       */

#define D2_L9_S_QUOTE        130  /* HMC reserved; substitutes for       */
                                  /* low 9-like single quote             */

#define D2_FIL_3             131  /* HMC reserved.                       */

#define D2_L99_D_QUOTE       132  /* HMC reserved; substitutes for       */
                                  /* low 99-like double quote            */

#define D2_ELLIPSIS          133  /* HMC reserved; substitutes for       */
                                  /* a three-point ellipsis              */

#define D2_DAGGER            134  /* HMC reserved; substitutes for       */
                                  /* dagger                              */

#define D2_D_DAGGER          135  /* HMC reserved; substitutes for       */
                                  /* double dagger                       */

#define D2_FIL_4             136  /* HMC reserved.                       */

#define D2_PER_MILLE         137  /* HMC reserved; substitutes for       */
                                  /* per mille sign                      */

#define D2_PLUS_MINUS        138  /* HMC reserved; substitutes for       */
                                  /* plus-minus sign                     */

#define D2_LT_S_GUILL        139  /* HMC reserved; substitutes for       */
                                  /* left pointing single guillement     */

#define D2_N_DASH            140  /* HMC reserved; substitutes for       */
                                  /* En dash                             */

#define D2_M_DASH            141  /* HMC reserved; substitutes for       */
                                  /* Em dash                             */

#define D2_TRADEMARK         142  /* HMC reserved; substitutes for       */
                                  /* Trademark                           */

#define D2_RT_S_GUILL        143  /* HMC reserved; substitutes for       */
                                  /* right pointing single guillemet     */

#define D2_NBSP              144
#define D2_C_A_OGONEK        145
#define D2_BREVE             146
#define D2_C_L_SLASH         147
#define D2_GEN_CURRENCY      148
#define D2_C_L_HACEK         149
#define D2_C_S_ACUTE         150
#define D2_SECTION           151
#define D2_UMLAUT            152
#define D2_C_S_HACEK         153
#define D2_C_S_CEDILLA       154
#define D2_C_T_HACEK         155
#define D2_C_Z_ACUTE         156
#define D2_SHY               157
#define D2_C_Z_HACEK         158
#define D2_C_Z_DOT           159

#define D2_MIDDLE_DOT        160  /* HMC reserved; substitutes for       */
                                  /* middle dot character                */                                 

#define D2_6_S_QUOTE         161  /* HMC reserved; substitutes for       */
                                  /* 6-like single quote                 */

#define D2_9_S_QUOTE         162  /* HMC reserved; substitutes for       */
                                  /* 9-like single quote                 */

#define D2_66_D_QUOTE        163  /* HMC reserved; substitutes for       */
                                  /* 66-like double quote                */

#define D2_99_D_QUOTE        164  /* HMC reserved; substitutes for       */
                                  /* 99-like double quote                */

#define D2_BULLET            165  /* HMC reserved; substitutes for bullet*/

#define D2_BROKEN_VBAR       166  /* HMC reserved; substitutes for       */
                                  /* broken vertical bar character       */                                 

#define D2_FIL_5             167  /* HMC reserved.                       */
#define D2_FIL_6             168  /* HMC reserved.                       */

#define D2_COPYRIGHT         169  /* HMC reserved; substitutes for       */
                                  /* copyright character                 */

#define D2_MICRO_SIGN        170  /* HMC reserved; substitutes for       */
                                  /* micro sign                          */

#define D2_LEFT_GUILL        171  /* HMC reserved; substitutes for       */
                                  /* left pointing guillemet             */

#define D2_NOT_SIGN          172  /* HMC reserved; substitutes for       */
                                  /* not-sign character                  */

#define D2_PARAGRAPH         173  /* HMC reserved; substitutes for       */
                                  /* paragraph symbol                    */

#define D2_REG_TRADEMARK     174  /* HMC reserved; substitutes for       */
                                  /* registered trademark symbol         */

#define D2_RIGHT_GUILL       175  /* HMC reserved; substitutes for       */
                                  /* right pointing guillemet            */

#define D2_DEGREE            176
#define D2_L_A_OGONEK        177
#define D2_OGONEK            178
#define D2_L_L_SLASH         179
#define D2_ACUTE             180
#define D2_L_L_HACEK         181 
#define D2_L_S_ACUTE         182
#define D2_HACEK             183
#define D2_CEDILLA           184
#define D2_L_S_HACEK         185                                             
#define D2_L_S_CEDILLA       186                                             
#define D2_L_T_HACEK         187
#define D2_L_Z_ACUTE         188
#define D2_DOUBLE_ACUTE      189
#define D2_L_Z_HACEK         190
#define D2_L_Z_DOT           191

#define D2_C_R_ACUTE         192                                              
#define D2_C_A_ACUTE         193
#define D2_C_A_CIRCUMFLX     194
#define D2_C_A_BREVE         195
#define D2_C_A_UMLAUT        196
#define D2_C_L_ACUTE         197
#define D2_C_C_ACUTE         198
#define D2_C_C_CEDILLA       199
#define D2_C_C_HACEK         200
#define D2_C_E_ACUTE         201
#define D2_C_E_OGONEK        202
#define D2_C_E_UMLAUT        203
#define D2_C_E_HACEK         204
#define D2_C_I_ACUTE         205
#define D2_C_I_CIRCUMFLX     206
#define D2_C_D_HACEK         207

#define D2_C_D_BAR           208
#define D2_C_N_ACUTE         209                                              
#define D2_C_N_HACEK         210                                              
#define D2_C_O_ACUTE         211
#define D2_C_O_CIRCUMFLX     212
#define D2_C_O_DOUBLE_ACUTE  213
#define D2_C_O_UMLAUT        214
#define D2_MULTIPLICATION    215
#define D2_C_R_HACEK         216
#define D2_C_U_RING          217
#define D2_C_U_ACUTE         218
#define D2_C_U_DOUBLE_ACUTE  219
#define D2_C_U_UMLAUT        220
#define D2_C_Y_ACUTE         221
#define D2_C_T_CEDILLA       222
#define D2_L_S_SHARP         223                                              

#define D2_L_R_ACUTE         224                                              
#define D2_L_A_ACUTE         225
#define D2_L_A_CIRCUMFLX     226
#define D2_L_A_BREVE         227
#define D2_L_A_UMLAUT        228
#define D2_L_L_ACUTE         229
#define D2_L_C_ACUTE         230
#define D2_L_C_CEDILLA       231
#define D2_L_C_HACEK         232
#define D2_L_E_ACUTE         233
#define D2_L_E_OGONEK        234
#define D2_L_E_UMLAUT        235
#define D2_L_E_HACEK         236
#define D2_L_I_ACUTE         237
#define D2_L_I_CIRCUMFLX     238
#define D2_L_D_HACEK         239

#define D2_L_D_BAR           240
#define D2_L_N_ACUTE         241                                              
#define D2_L_N_HACEK         242                                              
#define D2_L_O_ACUTE         243
#define D2_L_O_CIRCUMFLX     244
#define D2_L_O_DOUBLE_ACUTE  245
#define D2_L_O_UMLAUT        246
#define D2_DIVISION          247
#define D2_L_R_HACEK         248
#define D2_L_U_RING          249
#define D2_L_U_ACUTE         250
#define D2_L_U_DOUBLE_ACUTE  251
#define D2_L_U_UMLAUT        252
#define D2_L_Y_ACUTE         253
#define D2_L_T_CEDILLA       254
#define D2_SPACING_DOT_ABOVE 255
                              
                                                                           
#define D2_HIGH_VALUE        255  /* Extra define for end of set          */

#endif /* SLCH2MCS_H */

/* End of file - slch2mcs.h */
