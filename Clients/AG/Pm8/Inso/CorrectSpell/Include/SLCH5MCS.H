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
*        slch5mcs.h - The MCS Latin-5 Character Set defines             *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:

            This file contains #defines based on the ISO Latin5/Cyrillic
            Character Set. It also defines some characters that do not
            exist in this character set but are essential for the 
            multilingual language software products.

            This is the MCS default cyrillic character table. 
            Some nonessential characters are defined as D5_FIL_#.


        Cautions:
            None.

        Creation Date:
            09-dec-92        


*-----------------------------------------------------------------------*/
#ifndef SLCH5MCS_H

#define SLCH5MCS_H


/* DEC Multinational character set defines                              */
/* D5_C_...        HMC LATIN-5 DEFAULT CAPITAL ...                      */
/* D5_L_...        HMC LATIN-5 DEFAULT LOWER   ...                      */

#define D5_C_GE_UPTURN     128
#define D5_C_IO            129
#define D5_C_DJE           130
#define D5_C_GJE           131
#define D5_C_E             132
#define D5_C_DZE           133
#define D5_C_I             134
#define D5_C_YI            135
#define D5_C_JE            136
#define D5_C_LJE           137
#define D5_C_NJE           138
#define D5_C_TSHE          139
#define D5_C_KJE           140
#define D5_SHY             141
#define D5_C_SHORT_U       142
#define D5_C_DZHE          143

#define D5_NBSP            144
#define D5_6_S_QUOTE       145
#define D5_9_S_QUOTE       146
#define D5_66_D_QUOTE      147
#define D5_99_D_QUOTE      148
#define D5_BULLET          149
#define D5_N_DASH          150
#define D5_M_DASH          151
#define D5_FIL_1           152
#define D5_TRADEMARK       153
#define D5_L9_S_QUOTE      154
#define D5_RT_S_GUILL      155  /* Substitutes for                      */
                                /* right pointing single guillemet      */
#define D5_L99_D_QUOTE     156
#define D5_ELLIPSIS        157
#define D5_DAGGER          158
#define D5_D_DAGGER        159
                           
#define D5_L_GE_UPTURN     160
#define D5_L_IO            161
#define D5_L_DJE           162
#define D5_L_GJE           163
#define D5_L_E             164
#define D5_L_DZE           165
#define D5_L_I             166
#define D5_L_YI            167
#define D5_L_JE            168
#define D5_L_LJE           169
#define D5_L_NJE           170
#define D5_L_TSHE          171
#define D5_L_KJE           172
#define D5_SECTION         173
#define D5_L_SHORT_U       174
#define D5_L_DZHE          175

#define D5_PER_MILLE       176
#define D5_LT_S_GUILL      177  /* Substitutes for                      */
                                /* left pointing single guillemet       */
#define D5_GEN_CURRENCY    178
#define D5_BROKEN_VBAR     179
#define D5_COPYRIGHT       180
#define D5_LEFT_GUILL      181
#define D5_NOT_SIGN        182
#define D5_REG_TRADEMARK   183
#define D5_DEGREE          184
#define D5_PLUS_MINUS      185
#define D5_MICRO           186
#define D5_PARAGRAPH       187
#define D5_MIDDLE_DOT      188
#define D5_RIGHT_GUILL     189
#define D5_NUMERO          190
#define D5_FIL_2           191

#define D5_C_A             192
#define D5_C_BE            193
#define D5_C_VE            194
#define D5_C_GE            195
#define D5_C_DE            196
#define D5_C_IE            197
#define D5_C_ZHE           198
#define D5_C_ZE            199
#define D5_C_II            200
#define D5_C_SHORT_II      201
#define D5_C_KA            202
#define D5_C_EL            203
#define D5_C_EM            204
#define D5_C_EN            205
#define D5_C_O             206
#define D5_C_PE            207

#define D5_C_ER            208
#define D5_C_ES            209
#define D5_C_TE            210
#define D5_C_U             211
#define D5_C_EF            212
#define D5_C_KHA           213
#define D5_C_TSE           214
#define D5_C_CHE           215
#define D5_C_SHA           216
#define D5_C_SHCHA         217
#define D5_C_HARD_SIGN     218
#define D5_C_YERI          219
#define D5_C_SOFT_SIGN     220
#define D5_C_REVERSED_E    221
#define D5_C_IU            222
#define D5_C_IA            223

#define D5_L_A             224
#define D5_L_BE            225
#define D5_L_VE            226
#define D5_L_GE            227
#define D5_L_DE            228
#define D5_L_IE            229
#define D5_L_ZHE           230
#define D5_L_ZE            231
#define D5_L_II            232
#define D5_L_SHORT_II      233
#define D5_L_KA            234
#define D5_L_EL            235
#define D5_L_EM            236
#define D5_L_EN            237
#define D5_L_O             238
#define D5_L_PE            239

#define D5_L_ER            240
#define D5_L_ES            241
#define D5_L_TE            242
#define D5_L_U             243
#define D5_L_EF            244
#define D5_L_KHA           245
#define D5_L_TSE           246
#define D5_L_CHE           247
#define D5_L_SHA           248
#define D5_L_SHCHA         249
#define D5_L_HARD_SIGN     250
#define D5_L_YERI          251
#define D5_L_SOFT_SIGN     252
#define D5_L_REVERSED_E    253
#define D5_L_IU            254
#define D5_L_IA            255

#define D5_HIGH_VALUE      255   /* Extra define for end of set            */


#endif /* SLCH5MCS_H */

/* End of file - slch5mcs.h */
