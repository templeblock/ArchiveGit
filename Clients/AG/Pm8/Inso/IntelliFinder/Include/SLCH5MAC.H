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
*    slch5mac.h - Macintosh Latin-5 Character Set defines               *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the Macintosh
            Cyrillic Character Set. It also defines some characters that do 
            not exist in this character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as M5_FIL_#.

        Cautions:
            None.

        Creation Date:
            12-JAN-93   JMH Created by InfoSoft International, Inc.

*-----------------------------------------------------------------------*/
#ifndef SLCH5MAC_H

#define SLCH5MAC_H


/* MACINTOSH CYRILLIC CHARACTER SET DEFINES                             */
/* The following definitions are used in InfoSoft Latin5/Cyrillic character */
/* set conversions.                                                     */
/* M5_C -- Windows cyrillic capital                                     */
/* M5_L -- Windows cyrillic lower case                                  */

#define M5_C_A             128
#define M5_C_BE            129
#define M5_C_VE            130
#define M5_C_GE            131
#define M5_C_DE            132
#define M5_C_IE            133
#define M5_C_ZHE           134
#define M5_C_ZE            135
#define M5_C_II            136
#define M5_C_SHORT_II      137
#define M5_C_KA            138
#define M5_C_EL            139
#define M5_C_EM            140
#define M5_C_EN            141
#define M5_C_O             142
#define M5_C_PE            143

#define M5_C_ER            144
#define M5_C_ES            145
#define M5_C_TE            146
#define M5_C_U             147
#define M5_C_EF            148
#define M5_C_KHA           149
#define M5_C_TSE           150
#define M5_C_CHE           151
#define M5_C_SHA           152
#define M5_C_SHCHA         153
#define M5_C_HARD_SIGN     154
#define M5_C_YERI          155
#define M5_C_SOFT_SIGN     156
#define M5_C_REVERSED_E    157
#define M5_C_IU            158
#define M5_C_IA            159

#define M5_BROKEN_VBAR     160
#define M5_DEGREE          161
#define M5_L9_S_QUOTE      162
#define M5_L99_D_QUOTE     163
#define M5_SECTION         164
#define M5_BULLET          165
#define M5_PARAGRAPH       166
#define M5_C_I             167
#define M5_REG_TRADEMARK   168
#define M5_COPYRIGHT       169
#define M5_TRADEMARK       170
#define M5_C_DJE           171
#define M5_L_DJE           172
#define M5_L_I             173
#define M5_C_GJE           174
#define M5_L_GJE           175

#define M5_NBSP            176
#define M5_PLUS_MINUS      177
#define M5_LT_S_GUILL      178  /* Less than and equal sign substitutes     */
                                /* for left pointing single quillemet       */
#define M5_RT_S_GUILL      179  /* Greater than and equal sign substitutes  */
                                /* for right pointing single quillemet      */
#define M5_GEN_CURRENCY    180
#define M5_MICRO           181
#define M5_PER_MILLE       182
#define M5_C_JE            183
#define M5_C_E             184
#define M5_L_E             185
#define M5_C_YI            186
#define M5_L_YI            187
#define M5_C_LJE           188
#define M5_L_LJE           189
#define M5_C_NJE           190
#define M5_L_NJE           191

#define M5_L_JE            192
#define M5_C_DZE           193
#define M5_NOT_SIGN        194
#define M5_FIL_1           195
#define M5_D_DAGGER        196
#define M5_C_GE_UPTURN     197
#define M5_L_GE_UPTURN     198
#define M5_LEFT_GUILL      199
#define M5_RIGHT_GUILL     200
#define M5_ELLIPSIS        201
#define M5_SHY             202
#define M5_C_TSHE          203
#define M5_L_TSHE          204
#define M5_C_KJE           205
#define M5_L_KJE           206
#define M5_L_DZE           207

#define M5_N_DASH          208
#define M5_M_DASH          209
#define M5_66_D_QUOTE      210
#define M5_99_D_QUOTE      211
#define M5_6_S_QUOTE       212
#define M5_9_S_QUOTE       213
#define M5_DAGGER          214
#define M5_MIDDLE_DOT      215
#define M5_C_SHORT_U       216
#define M5_L_SHORT_U       217
#define M5_C_DZHE          218
#define M5_L_DZHE          219
#define M5_NUMERO          220
#define M5_C_IO            221
#define M5_L_IO            222
#define M5_L_IA            223

#define M5_L_A             224
#define M5_L_BE            225
#define M5_L_VE            226
#define M5_L_GE            227
#define M5_L_DE            228
#define M5_L_IE            229
#define M5_L_ZHE           230
#define M5_L_ZE            231
#define M5_L_II            232
#define M5_L_SHORT_II      233
#define M5_L_KA            234
#define M5_L_EL            235
#define M5_L_EM            236
#define M5_L_EN            237
#define M5_L_O             238
#define M5_L_PE            239

#define M5_L_ER            240
#define M5_L_ES            241
#define M5_L_TE            242
#define M5_L_U             243
#define M5_L_EF            244
#define M5_L_KHA           245
#define M5_L_TSE           246
#define M5_L_CHE           247
#define M5_L_SHA           248
#define M5_L_SHCHA         249
#define M5_L_HARD_SIGN     250
#define M5_L_YERI          251
#define M5_L_SOFT_SIGN     252
#define M5_L_REVERSED_E    253
#define M5_L_IU            254
#define M5_FIL_2           255

#endif /* SLCH5MAC_H */

/* end of file - slch5mac.h */


































































































































