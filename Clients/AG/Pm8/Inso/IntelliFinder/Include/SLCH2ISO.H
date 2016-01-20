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
*        slch2mcs.h - ISO Latin-2 Header File                           *
*                     ISO Latin-2 Character Set defines                 *
*                     (Central European)                                          *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:

            This file contains #defines based on the ISO Latin-2
            Character Set. It also defines some characters that do not
            exist in this character set but are essential for the
            multilingual language software products.

            This is the character table used for the VAX platform and in 
            the absence of the character set for the Unix platform. 
            Some nonessential characters are defined as I2_FIL_#.


        Cautions:
            None.

        Creation Date:
            13-jan-93   JMH

        Change Record:


*-----------------------------------------------------------------------*/
#ifndef SLCH2ISO_H

#define SLCH2ISO_H


/* ISO Latin-2 character set defines                                    */
/* I2_C_...       ISO LATIN-2 DEFAULT CAPITAL ...                       */
/* I2_L_...       ISO LATIN-2 DEFAULT LOWER   ...                       */


#define I2_FIL_1             128  /* HMC reserved.                       */
#define I2_FIL_2             129  /* HMC reserved.                       */

#define I2_L9_S_QUOTE        130  /* HMC reserved; substitutes for       */
                                  /* low 9-like single quote             */

#define I2_FIL_3             131  /* HMC reserved.                       */

#define I2_L99_D_QUOTE       132  /* HMC reserved; substitutes for       */
                                  /* low 99-like double quote            */

#define I2_ELLIPSIS          133  /* HMC reserved; substitutes for       */
                                  /* a three-point ellipsis              */

#define I2_DAGGER            134  /* HMC reserved; substitutes for       */
                                  /* dagger                              */

#define I2_D_DAGGER          135  /* HMC reserved; substitutes for       */
                                  /* double dagger                       */

#define I2_FIL_4             136  /* HMC reserved.                       */

#define I2_PER_MILLE         137  /* HMC reserved; substitutes for       */
                                  /* per mille sign                      */

#define I2_PLUS_MINUS        138  /* HMC reserved; substitutes for       */
                                  /* plus-minus sign                     */

#define I2_LT_S_GUILL        139  /* HMC reserved; substitutes for       */
                                  /* left pointing single guillement     */


#define I2_N_DASH            140  /* HMC reserved; substitutes for       */
                                  /* En dash                             */

#define I2_M_DASH            141  /* HMC reserved; substitutes for       */
                                  /* Em dash                             */

#define I2_TRADEMARK         142  /* HMC reserved; substitutes for       */
                                  /* Trademark                           */

#define I2_RT_S_GUILL        143  /* HMC reserved; substitutes for       */
                                  /* right pointing single guillemet     */

#define I2_MIDDLE_DOT        144  /* HMC reserved; substitutes for       */
                                  /* middle dot character                */                                 

#define I2_6_S_QUOTE         145  /* HMC reserved; substitutes for       */
                                  /* 6-like single quote                 */

#define I2_9_S_QUOTE         146  /* HMC reserved; substitutes for       */
                                  /* 9-like single quote                 */

#define I2_66_D_QUOTE        147  /* HMC reserved; substitutes for       */
                                  /* 66-like double quote                */

#define I2_99_D_QUOTE        148  /* HMC reserved; substitutes for       */
                                  /* 99-like double quote                */

#define I2_BULLET            149  /* HMC reserved; substitutes for bullet*/

#define I2_BROKEN_VBAR       150  /* HMC reserved; substitutes for       */
                                  /* broken vertical bar character       */                                 

#define I2_FIL_5             151  /* HMC reserved.                       */
#define I2_FIL_6             152  /* HMC reserved.                       */

#define I2_COPYRIGHT         153  /* HMC reserved; substitutes for       */
                                  /* copyright character                 */

#define I2_MICRO_SIGN        154  /* HMC reserved; substitutes for       */
                                  /* micro sign                          */

#define I2_LEFT_GUILL        155  /* HMC reserved; substitutes for       */
                                  /* left pointing guillemet             */

#define I2_NOT_SIGN          156  /* HMC reserved; substitutes for       */
                                  /* not-sign character                  */

#define I2_PARAGRAPH         157  /* HMC reserved; substitutes for       */
                                  /* paragraph symbol                    */

#define I2_REG_TRADEMARK     158  /* HMC reserved; substitutes for       */
                                  /* registered trademark symbol         */

#define I2_RIGHT_GUILL       159  /* HMC reserved; substitutes for       */
                                  /* right pointing guillemet            */

#define I2_NBSP              160
#define I2_C_A_OGONEK        161
#define I2_BREVE             162
#define I2_C_L_SLASH         163
#define I2_GEN_CURRENCY      164
#define I2_C_L_HACEK         165
#define I2_C_S_ACUTE         166
#define I2_SECTION           167
#define I2_UMLAUT            168
#define I2_C_S_HACEK         169
#define I2_C_S_CEDILLA       170
#define I2_C_T_HACEK         171
#define I2_C_Z_ACUTE         172
#define I2_SHY               173
#define I2_C_Z_HACEK         174
#define I2_C_Z_DOT           175

#define I2_DEGREE            176
#define I2_L_A_OGONEK        177
#define I2_OGONEK            178
#define I2_L_L_SLASH         179
#define I2_ACUTE             180
#define I2_L_L_HACEK         181 
#define I2_L_S_ACUTE         182
#define I2_HACEK             183
#define I2_CEDILLA           184
#define I2_L_S_HACEK         185                                             
#define I2_L_S_CEDILLA       186                                             
#define I2_L_T_HACEK         187
#define I2_L_Z_ACUTE         188
#define I2_DOUBLE_ACUTE      189
#define I2_L_Z_HACEK         190
#define I2_L_Z_DOT           191

#define I2_C_R_ACUTE         192                                              
#define I2_C_A_ACUTE         193
#define I2_C_A_CIRCUMFLX     194
#define I2_C_A_BREVE         195
#define I2_C_A_UMLAUT        196
#define I2_C_L_ACUTE         197
#define I2_C_C_ACUTE         198
#define I2_C_C_CEDILLA       199
#define I2_C_C_HACEK         200
#define I2_C_E_ACUTE         201
#define I2_C_E_OGONEK        202
#define I2_C_E_UMLAUT        203
#define I2_C_E_HACEK         204
#define I2_C_I_ACUTE         205
#define I2_C_I_CIRCUMFLX     206
#define I2_C_D_HACEK         207

#define I2_C_D_BAR           208
#define I2_C_N_ACUTE         209                                              
#define I2_C_N_HACEK         210                                              
#define I2_C_O_ACUTE         211
#define I2_C_O_CIRCUMFLX     212
#define I2_C_O_DOUBLE_ACUTE  213
#define I2_C_O_UMLAUT        214
#define I2_MULTIPLICATION    215
#define I2_C_R_HACEK         216
#define I2_C_U_RING          217
#define I2_C_U_ACUTE         218
#define I2_C_U_DOUBLE_ACUTE  219
#define I2_C_U_UMLAUT        220
#define I2_C_Y_ACUTE         221
#define I2_C_T_CEDILLA       222
#define I2_L_S_SHARP         223                                              

#define I2_L_R_ACUTE         224                                              
#define I2_L_A_ACUTE         225
#define I2_L_A_CIRCUMFLX     226
#define I2_L_A_BREVE         227
#define I2_L_A_UMLAUT        228
#define I2_L_L_ACUTE         229
#define I2_L_C_ACUTE         230
#define I2_L_C_CEDILLA       231
#define I2_L_C_HACEK         232
#define I2_L_E_ACUTE         233
#define I2_L_E_OGONEK        234
#define I2_L_E_UMLAUT        235
#define I2_L_E_HACEK         236
#define I2_L_I_ACUTE         237
#define I2_L_I_CIRCUMFLX     238
#define I2_L_D_HACEK         239

#define I2_L_D_BAR           240
#define I2_L_N_ACUTE         241                                              
#define I2_L_N_HACEK         242                                              
#define I2_L_O_ACUTE         243
#define I2_L_O_CIRCUMFLX     244
#define I2_L_O_DOUBLE_ACUTE  245
#define I2_L_O_UMLAUT        246
#define I2_DIVISION          247
#define I2_L_R_HACEK         248
#define I2_L_U_RING          249
#define I2_L_U_ACUTE         250
#define I2_L_U_DOUBLE_ACUTE  251
#define I2_L_U_UMLAUT        252
#define I2_L_Y_ACUTE         253
#define I2_L_T_CEDILLA       254
#define I2_SPACING_DOT_ABOVE 255
                              
                                                                           
#endif /* SLCH2ISO_H */

/* End of file - slch2iso.h */



 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
