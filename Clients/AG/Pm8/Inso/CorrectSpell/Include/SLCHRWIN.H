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
*    slchrwin.h - Microsoft Windows Latin-1 Character Set defines       *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the MS Windows
            Latin-1 Character Set. It also defines some characters that do 
            not exist in this character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as W_FIL_#.

            The Microsoft Windows Latin-1 Character Set is the ANSI 
            Character Set (code page 1252).

         Cautions:
            None.

        Creation Date:
            02-oct-91   JCC  Split off from slchrdos.h


*-----------------------------------------------------------------------*/
#ifndef SLCHRWIN_H

#define SLCHRWIN_H


/* WINDOWS CHARACTER SET DEFINES                                        */

/* W_C_... = Windows CAPITAL ...                                        */
/* W_L_... = Windows LOWER   ...                                        */

/* 
   When running under WINDOWS, there are only a few characters that have a 
   different ASCII value from the default DEC Multinational Character Set.  
   To enter the extended ASCII, hold down the ALT key and using the numeric 
   keypad to the right of your keyboard, type 0 and then the 3-digit number.
*/


#define W_FIL_1            128   /* Not supported by Windows            */
#define W_FIL_2            129   /* Not supported by Windows            */

#define W_L9_S_QUOTE       130   /* low 9-like single quote             */
                                 /* in the Windows Latin-1 Character Set*/

#define W_FLORIN           131   /* Latin small letter script f substitutes*/
                                 /* for the Dutch currency symbol (florin) */

#define W_L99_D_QUOTE      132   /* low 99-like double quote            */
                                 /* in the Windows Latin-1 Character Set*/

#define W_ELLIPSIS         133   /* a three-point ellipsis              */
                                 /* in the Windows Latin-1 Character Set*/

#define W_FIL_3            134   /* Dagger                              */
#define W_FIL_4            135   /* Double dagger                       */
#define W_FIL_5            136   /* Modifier letter circumflex          */
#define W_PER_MILLE        137   /* Per mille sign                      */
#define W_FIL_6            138   /* Latin capital letter S hacek        */

#define W_LT_S_GUILL       139   /* Left pointing single guillemet      */
               
#define W_C_OE_LIGATURE    140   /* OE ligature                         */
                                 /* in the Windows Latin-1 Character Set*/

#define W_FIL_7            141   /* Not supported by Windows            */
#define W_FIL_8            142   /* Not supported by Windows            */
#define W_FIL_9            143   /* Not supported by Windows            */
#define W_FIL_10           144   /* Not supported by Windows            */

#define W_6_S_QUOTE        145   /* 6-like single quote                 */
#define W_9_S_QUOTE        146   /* 9-like single quote                 */
/* Redefined for Inso product backward compatibility                    */
#define W_OPEN_S_QUOTE     W_6_S_QUOTE
#define W_CLOSE_S_QUOTE    W_9_S_QUOTE

#define W_66_D_QUOTE       147   /* 66-like double quote                */
                                 /* in the Windows Latin-1 Character Set*/
#define W_99_D_QUOTE       148   /* 99-like double quote                */
                                 /* in the Windows Latin-1 Character Set*/
/* Redefined for Inso product backward compatibility                    */
#define W_OPEN_D_QUOTE     W_66_D_QUOTE
#define W_CLOSE_D_QUOTE    W_99_D_QUOTE            

#define W_FIL_11           149   /* Bullet                              */

#define W_EN_DASH          150   /* En dash                             */
                                 /* in the Windows Latin-1 Character Set*/

#define W_M_DASH           151   /* em dash                             */
                                 /* in the Windows Latin-1 Character Set*/

#define W_FIL_12           152   /* Spacing tilde                       */
#define W_TRADEMARK        153   /* Trademark                           */
#define W_FIL_13           154   /* Latin small letter s hacek          */

#define W_RT_S_GUILL       155   /* Right pointing single guillemet     */

#define W_L_OE_LIGATURE    156   /* oe ligature                         */
                                 /* in the Windows Latin-1 Character Set*/

#define W_FIL_14           157   /* Not supported by Windows            */
#define W_FIL_15           158   /* Not supported by Windows            */

#define W_C_Y_UMLAUT       159   /* Y umlaut                            */
                                 /* in the Windows Latin-1 Character Set*/

#define W_NBSP             160
#define W_INV_EXCLAMATN    161
#define W_CENT             162
#define W_POUND            163
#define W_GEN_CURRENCY     164
#define W_YEN              165
#define W_BROKEN_VBAR      166
#define W_SECTION          167
#define W_UMLAUT           168
#define W_COPYRIGHT        169
#define W_FEM_ORDINAL      170

#define W_LEFT_GUILL       171
/* Redefined for Inso product backward compatibility                        */
#define W_OPEN_GUILL       W_LEFT_GUILL

#define W_NOT_SIGN         172
#define W_SHY              173
#define W_REG_TRADEMARK    174
#define W_MACRON           175
#define W_DEGREE           176
#define W_PLUS_MINUS       177
#define W_TWO_SUPER        178
#define W_THREE_SUPER      179
#define W_ACUTE            180
#define W_MICRO_SIGN       181
#define W_PARAGRAPH        182
#define W_MIDDLE_DOT       183
#define W_CEDILLA          184
#define W_ONE_SUPER        185
#define W_MAS_ORDINAL      186

#define W_RIGHT_GUILL      187
/* Redefined for Inso product backward compatibility                        */
#define W_CLOS_GUILL       W_RIGHT_GUILL 

#define W_ONE_QUARTER      188
#define W_ONE_HALF         189
#define W_THREE_FOURTHS    190
#define W_INV_QUESTION     191
#define W_C_A_GRAVE        192
#define W_C_A_ACUTE        193
#define W_C_A_CIRCUMFLX    194
#define W_C_A_TILDE        195
#define W_C_A_UMLAUT       196
#define W_C_A_ANGSTROM     197
#define W_C_AE_LIGATURE    198
#define W_C_C_CEDILLA      199
#define W_C_E_GRAVE        200
#define W_C_E_ACUTE        201
#define W_C_E_CIRCUMFLX    202
#define W_C_E_UMLAUT       203
#define W_C_I_GRAVE        204       
#define W_C_I_ACUTE        205
#define W_C_I_CIRCUMFLX    206
#define W_C_I_UMLAUT       207
#define W_C_EDH            208
#define W_C_N_TILDE        209
#define W_C_O_GRAVE        210
#define W_C_O_ACUTE        211
#define W_C_O_CIRCUMFLX    212
#define W_C_O_TILDE        213
#define W_C_O_UMLAUT       214
#define W_MULTIPLICATION   215
#define W_C_O_SLASH        216
#define W_C_U_GRAVE        217
#define W_C_U_ACUTE        218
#define W_C_U_CIRCUMFLX    219
#define W_C_U_UMLAUT       220
#define W_C_Y_ACUTE        221
#define W_C_THORN          222
#define W_L_S_SHARP        223
#define W_L_A_GRAVE        224
#define W_L_A_ACUTE        225
#define W_L_A_CIRCUMFLX    226
#define W_L_A_TILDE        227
#define W_L_A_UMLAUT       228
#define W_L_A_ANGSTROM     229
#define W_L_AE_LIGATURE    230
#define W_L_C_CEDILLA      231
#define W_L_E_GRAVE        232
#define W_L_E_ACUTE        233
#define W_L_E_CIRCUMFLX    234
#define W_L_E_UMLAUT       235
#define W_L_I_GRAVE        236
#define W_L_I_ACUTE        237
#define W_L_I_CIRCUMFLX    238
#define W_L_I_UMLAUT       239
#define W_L_EDH            240
#define W_L_N_TILDE        241
#define W_L_O_GRAVE        242
#define W_L_O_ACUTE        243
#define W_L_O_CIRCUMFLX    244
#define W_L_O_TILDE        245
#define W_L_O_UMLAUT       246
#define W_DIVISION         247
#define W_L_O_SLASH        248
#define W_L_U_GRAVE        249
#define W_L_U_ACUTE        250
#define W_L_U_CIRCUMFLX    251
#define W_L_U_UMLAUT       252
#define W_L_Y_ACUTE        253
#define W_L_THORN          254
#define W_L_Y_UMLAUT       255

#endif /* SLCHRWIN_H */

/* end of file - slchrwin.h */
