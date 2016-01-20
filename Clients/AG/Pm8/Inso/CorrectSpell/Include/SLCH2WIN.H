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
*    slch2win.h - Microsoft Windows Latin-2 Character Set defines       *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the MS Windows
            Latin-2 Character Set. It also defines some characters that do 
            not exist in this character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as W2_FIL_#.

         Cautions:
            None.

        Creation Date:
            08-dec-92   PJA  


*-----------------------------------------------------------------------*/
#ifndef SLCH2WIN_H

#define SLCH2WIN_H


/* WINDOWS LATIN-2 CHARACTER SET DEFINES                                        */

/* W2_C_... = Windows CAPITAL ...                                        */
/* W2_L_... = Windows LOWER   ...                                        */


#define W2_FIL_1             128
#define W2_FIL_2             129

#define W2_L9_S_QUOTE        130  /* low 9-like single quote             */
                                  /* in the Windows Latin-2 Character Set*/

#define W2_FIL_3             131  

#define W2_L99_D_QUOTE       132  /* low 99-like double quote            */
                                  /* in the Windows Latin-2 Character Set*/

#define W2_ELLIPSIS          133  /* a three-point ellipsis              */
                                  /* in the Windows Latin-2 Character Set*/

#define W2_DAGGER            134  /* Dagger                              */

#define W2_D_DAGGER          135  /* Double dagger                       */

#define W2_FIL_4             136
#define W2_PER_MILLE         137  /* Per mille sign                      */
#define W2_C_S_HACEK         138
#define W2_LT_S_GUILL        139  /* Left pointing single guillemet      */

#define W2_C_S_ACUTE         140
#define W2_C_T_HACEK         141
#define W2_C_Z_HACEK         142
#define W2_C_Z_ACUTE         143

#define W2_FIL_5             144

#define W2_6_S_QUOTE         145  /* 6-like single quote                 */
#define W2_9_S_QUOTE         146  /* 9-like single quote                 */

#define W2_66_D_QUOTE        147  /* 66-like double quote                */
                                  /* in the Windows Latin-2 Character Set*/
#define W2_99_D_QUOTE        148  /* 99-like double quote                */
                                  /* in the Windows Latin-2 Character Set*/

#define W2_BULLET            149  /* Bullet                              */
#define W2_N_DASH            150  /* En dash                             */
                                  /* in the Windows Latin-2 Character Set*/

#define W2_M_DASH            151  /* em dash                             */
                                  /* in the Windows Latin-2 Character Set*/

#define W2_FIL_6             152  /* Not supported by Windows            */

#define W2_TRADEMARK         153  /* Trademark                           */
#define W2_L_S_HACEK         154
#define W2_RT_S_GUILL        155  /* Right pointing single guillemet     */

#define W2_L_S_ACUTE         156

#define W2_L_T_HACEK         157
#define W2_L_Z_HACEK         158

#define W2_L_Z_ACUTE         159

#define W2_NBSP              160
#define W2_HACEK             161
#define W2_BREVE             162
#define W2_C_L_SLASH         163
#define W2_GEN_CURRENCY      164
#define W2_C_A_OGONEK        165
#define W2_BROKEN_VBAR       166
#define W2_SECTION           167
#define W2_UMLAUT            168
#define W2_COPYRIGHT         169
#define W2_C_S_CEDILLA       170
#define W2_LEFT_GUILL        171
#define W2_NOT_SIGN          172
#define W2_SHY               173  /* Soft hyphen                         */
#define W2_REG_TRADEMARK     174
#define W2_C_Z_DOT           175


#define W2_DEGREE            176
#define W2_PLUS_MINUS        177
#define W2_OGONEK            178
#define W2_L_L_SLASH         179
#define W2_ACUTE             180
#define W2_MICRO_SIGN        181
#define W2_PARAGRAPH         182
#define W2_MIDDLE_DOT        183
#define W2_CEDILLA           184
#define W2_L_A_OGONEK        185
#define W2_L_S_CEDILLA       186
#define W2_RIGHT_GUILL       187
#define W2_C_L_HACEK         188
#define W2_DOUBLE_ACUTE      189
#define W2_L_L_HACEK         190
#define W2_L_Z_DOT           191

#define W2_C_R_ACUTE         192
#define W2_C_A_ACUTE         193
#define W2_C_A_CIRCUMFLX     194
#define W2_C_A_BREVE         195
#define W2_C_A_UMLAUT        196
#define W2_C_L_ACUTE         197
#define W2_C_C_ACUTE         198
#define W2_C_C_CEDILLA       199
#define W2_C_C_HACEK         200
#define W2_C_E_ACUTE         201
#define W2_C_E_OGONEK        202
#define W2_C_E_UMLAUT        203
#define W2_C_E_HACEK         204       
#define W2_C_I_ACUTE         205
#define W2_C_I_CIRCUMFLX     206
#define W2_C_D_HACEK         207

#define W2_C_D_BAR           208
#define W2_C_N_ACUTE         209
#define W2_C_N_HACEK         210
#define W2_C_O_ACUTE         211
#define W2_C_O_CIRCUMFLX     212
#define W2_C_O_DOUBLE_ACUTE  213
#define W2_C_O_UMLAUT        214
#define W2_MULTIPLICATION    215
#define W2_C_R_HACEK         216
#define W2_C_U_RING          217
#define W2_C_U_ACUTE         218
#define W2_C_U_DOUBLE_ACUTE  219
#define W2_C_U_UMLAUT        220
#define W2_C_Y_ACUTE         221
#define W2_C_T_CEDILLA       222
#define W2_L_S_SHARP         223

#define W2_L_R_ACUTE         224
#define W2_L_A_ACUTE         225
#define W2_L_A_CIRCUMFLX     226
#define W2_L_A_BREVE         227
#define W2_L_A_UMLAUT        228
#define W2_L_L_ACUTE         229
#define W2_L_C_ACUTE         230
#define W2_L_C_CEDILLA       231
#define W2_L_C_HACEK         232
#define W2_L_E_ACUTE         233
#define W2_L_E_OGONEK        234
#define W2_L_E_UMLAUT        235
#define W2_L_E_HACEK         236
#define W2_L_I_ACUTE         237
#define W2_L_I_CIRCUMFLX     238
#define W2_L_D_HACEK         239

#define W2_L_D_BAR           240
#define W2_L_N_ACUTE         241
#define W2_L_N_HACEK         242
#define W2_L_O_ACUTE         243
#define W2_L_O_CIRCUMFLX     244
#define W2_L_O_DOUBLE_ACUTE  245
#define W2_L_O_UMLAUT        246
#define W2_DIVISION          247
#define W2_L_R_HACEK         248
#define W2_L_U_RING          249
#define W2_L_U_ACUTE         250
#define W2_L_U_DOUBLE_ACUTE  251
#define W2_L_U_UMLAUT        252
#define W2_L_Y_ACUTE         253
#define W2_L_T_CEDILLA       254
#define W2_SPACING_DOT_ABOVE 255


#endif /* SLCH2WIN_H */

/* End of file - slch2win.h */
