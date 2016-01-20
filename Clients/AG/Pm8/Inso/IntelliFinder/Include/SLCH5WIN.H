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
*    slchrwin.h - Microsoft Windows Latin-5 Character Set defines       *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the MS Windows
            Cyrillic Character Set. It also defines some characters that do 
            not exist in this character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as W5_FIL_#.

        Cautions:
            None.

        Creation Date:
            09-dec-92   Created by InfoSoft International, Inc.

        Change Record:

*-----------------------------------------------------------------------*/
#ifndef SLCH5WIN_H

#define SLCH5WIN_H


/* WINDOWS CYRILLIC CHARACTER SET DEFINES                               */
/* The following definitions are used in InfoSoft Latin5/Cyrillic character */
/* set conversions.                                                     */
/* W5_C -- Windows cyrillic capital                                     */
/* W5_L -- Windows cyrillic lower case                                  */

#define W5_C_DJE           128
#define W5_C_GJE           129
#define W5_L9_S_QUOTE      130
#define W5_L_GJE           131
#define W5_L99_D_QUOTE     132
#define W5_ELLIPSIS        133
#define W5_DAGGER          134
#define W5_D_DAGGER        135
#define W5_FIL_1           136
#define W5_PER_MILLE       137
#define W5_C_LJE           138
#define W5_LT_S_GUILL      139  /* Left pointing single guillemet       */
#define W5_C_NJE           140
#define W5_C_KJE           141
#define W5_C_TSHE          142
#define W5_C_DZHE          143

#define W5_L_DJE           144
#define W5_6_S_QUOTE       145
#define W5_9_S_QUOTE       146
#define W5_66_D_QUOTE      147
#define W5_99_D_QUOTE      148
#define W5_BULLET          149
#define W5_N_DASH          150
#define W5_M_DASH          151
#define W5_FIL_2           152
#define W5_TRADEMARK       153
#define W5_L_LJE           154
#define W5_RT_S_GUILL      155  /* Right pointing single guillemet      */
#define W5_L_NJE           156
#define W5_L_KJE           157
#define W5_L_TSHE          158
#define W5_L_DZHE          159

#define W5_NBSP            160
#define W5_C_SHORT_U       161
#define W5_L_SHORT_U       162
#define W5_C_JE            163
#define W5_GEN_CURRENCY    164
#define W5_C_GE_UPTURN     165
#define W5_BROKEN_VBAR     166
#define W5_SECTION         167
#define W5_C_IO            168
#define W5_COPYRIGHT       169
#define W5_C_E             170
#define W5_LEFT_GUILL      171
#define W5_NOT_SIGN        172
#define W5_SHY             173
#define W5_REG_TRADEMARK   174
#define W5_C_YI            175

#define W5_DEGREE          176
#define W5_PLUS_MINUS      177
#define W5_C_I             178
#define W5_L_I             179
#define W5_L_GE_UPTURN     180
#define W5_MICRO           181
#define W5_PARAGRAPH       182
#define W5_MIDDLE_DOT      183
#define W5_L_IO            184
#define W5_NUMERO          185
#define W5_L_E             186
#define W5_RIGHT_GUILL     187
#define W5_L_JE            188
#define W5_C_DZE           189
#define W5_L_DZE           190
#define W5_L_YI            191

#define W5_C_A             192
#define W5_C_BE            193
#define W5_C_VE            194
#define W5_C_GE            195
#define W5_C_DE            196
#define W5_C_IE            197
#define W5_C_ZHE           198
#define W5_C_ZE            199
#define W5_C_II            200
#define W5_C_SHORT_II      201
#define W5_C_KA            202
#define W5_C_EL            203
#define W5_C_EM            204
#define W5_C_EN            205
#define W5_C_O             206
#define W5_C_PE            207

#define W5_C_ER            208
#define W5_C_ES            209
#define W5_C_TE            210
#define W5_C_U             211
#define W5_C_EF            212
#define W5_C_KHA           213
#define W5_C_TSE           214
#define W5_C_CHE           215
#define W5_C_SHA           216
#define W5_C_SHCHA         217
#define W5_C_HARD_SIGN     218
#define W5_C_YERI          219
#define W5_C_SOFT_SIGN     220
#define W5_C_REVERSED_E    221
#define W5_C_IU            222
#define W5_C_IA            223

#define W5_L_A             224
#define W5_L_BE            225
#define W5_L_VE            226
#define W5_L_GE            227
#define W5_L_DE            228
#define W5_L_IE            229
#define W5_L_ZHE           230
#define W5_L_ZE            231
#define W5_L_II            232
#define W5_L_SHORT_II      233
#define W5_L_KA            234
#define W5_L_EL            235
#define W5_L_EM            236
#define W5_L_EN            237
#define W5_L_O             238
#define W5_L_PE            239

#define W5_L_ER            240
#define W5_L_ES            241
#define W5_L_TE            242
#define W5_L_U             243
#define W5_L_EF            244
#define W5_L_KHA           245
#define W5_L_TSE           246
#define W5_L_CHE           247
#define W5_L_SHA           248
#define W5_L_SHCHA         249
#define W5_L_HARD_SIGN     250
#define W5_L_YERI          251
#define W5_L_SOFT_SIGN     252
#define W5_L_REVERSED_E    253
#define W5_L_IU            254
#define W5_L_IA            255

#endif /* SLCHRWIN_H */

/* end of file - slchrwin.h */
