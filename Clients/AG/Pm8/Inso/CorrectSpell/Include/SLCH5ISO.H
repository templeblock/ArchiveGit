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
*        slch5mcs.h - The ISO Latin-5 (Cyrillic) Character Set defines  *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:

            This file contains #defines based on the ISO Latin5/Cyrillic
            Character Set. It also defines some characters that do not
            exist in this character set but are essential for the 
            multilingual language software products.

            This is the character table used for the VAX platform and in 
            the absence of the character set for the Unix platform. 
            Some nonessential characters are defined as I5_FIL_#.


        Cautions:
            None.

        Creation Date:
            13-Jan-93       JMH  



*-----------------------------------------------------------------------*/
#ifndef SLCH5ISO_H

#define SLSH5ISO_H


/* ISO/IEC character set defines                                        */
/* I5_C_...        ISO LATIN-5 DEFAULT CAPITAL ...                      */
/* I5_L_...        ISO LATIN-5 DEFAULT LOWER   ...                      */

#define I5_PER_MILLE       128
#define I5_LT_S_GUILL      129  /* Unused; substitutes for left pointing    */
                                /* single guillemet                         */

#define I5_GEN_CURRENCY    130
#define I5_BROKEN_VBAR     131
#define I5_COPYRIGHT       132
#define I5_LEFT_GUILL      133
#define I5_NOT_SIGN        134
#define I5_REG_TRADEMARK   135
#define I5_DEGREE          136
#define I5_PLUS_MINUS      137
#define I5_MICRO           138
#define I5_PARAGRAPH       139
#define I5_MIDDLE_DOT      140
#define I5_RIGHT_GUILL     141
#define I5_L_GE_UPTURN     142
#define I5_FIL_2           143

#define I5_C_GE_UPTURN     144
#define I5_6_S_QUOTE       145
#define I5_9_S_QUOTE       146
#define I5_66_D_QUOTE      147
#define I5_99_D_QUOTE      148
#define I5_BULLET          149
#define I5_N_DASH          150
#define I5_M_DASH          151
#define I5_FIL_1           152
#define I5_TRADEMARK       153
#define I5_L9_S_QUOTE      154
#define I5_RT_S_GUILL      155  /* Unused; substitutes for right pointing   */
                                /* single guillemet                         */
#define I5_L99_D_QUOTE     156
#define I5_ELLIPSIS        157
#define I5_DAGGER          158
#define I5_D_DAGGER        159
                           
#define I5_NBSP            160
#define I5_C_IO            161
#define I5_C_DJE           162
#define I5_C_GJE           163
#define I5_C_E             164
#define I5_C_DZE           165
#define I5_C_I             166
#define I5_C_YI            167
#define I5_C_JE            168
#define I5_C_LJE           169
#define I5_C_NJE           170
#define I5_C_TSHE          171
#define I5_C_KJE           172
#define I5_SHY             173
#define I5_C_SHORT_U       174
#define I5_C_DZHE          175

#define I5_C_A             176
#define I5_C_BE            177
#define I5_C_VE            178
#define I5_C_GE            179
#define I5_C_DE            180
#define I5_C_IE            181
#define I5_C_ZHE           182
#define I5_C_ZE            183
#define I5_C_II            184
#define I5_C_SHORT_II      185
#define I5_C_KA            186
#define I5_C_EL            187
#define I5_C_EM            188
#define I5_C_EN            189
#define I5_C_O             190
#define I5_C_PE            191

#define I5_C_ER            192
#define I5_C_ES            193
#define I5_C_TE            194
#define I5_C_U             195
#define I5_C_EF            196
#define I5_C_KHA           197
#define I5_C_TSE           198
#define I5_C_CHE           199
#define I5_C_SHA           200
#define I5_C_SHCHA         201
#define I5_C_HARD_SIGN     202
#define I5_C_YERI          203
#define I5_C_SOFT_SIGN     204
#define I5_C_REVERSED_E    205
#define I5_C_IU            206
#define I5_C_IA            207

#define I5_L_A             208
#define I5_L_BE            209
#define I5_L_VE            210
#define I5_L_GE            211
#define I5_L_DE            212
#define I5_L_IE            213
#define I5_L_ZHE           214
#define I5_L_ZE            215
#define I5_L_II            216
#define I5_L_SHORT_II      217
#define I5_L_KA            218
#define I5_L_EL            219
#define I5_L_EM            220
#define I5_L_EN            221
#define I5_L_O             222
#define I5_L_PE            223

#define I5_L_ER            224
#define I5_L_ES            225
#define I5_L_TE            226
#define I5_L_U             227
#define I5_L_EF            228
#define I5_L_KHA           229
#define I5_L_TSE           230
#define I5_L_CHE           231
#define I5_L_SHA           232
#define I5_L_SHCHA         233
#define I5_L_HARD_SIGN     234
#define I5_L_YERI          235
#define I5_L_SOFT_SIGN     236
#define I5_L_REVERSED_E    237
#define I5_L_IU            238
#define I5_L_IA            239

#define I5_NUMERO          240
#define I5_L_IO            241
#define I5_L_DJE           242
#define I5_L_GJE           243
#define I5_L_E             244
#define I5_L_DZE           245
#define I5_L_I             246
#define I5_L_YI            247
#define I5_L_JE            248
#define I5_L_LJE           249
#define I5_L_NJE           250
#define I5_L_TSHE          251
#define I5_L_KJE           252
#define I5_SECTION         253
#define I5_L_SHORT_U       254
#define I5_L_DZHE          255


#endif /* SLCH5ISO_H */

/* End of file - slch5iso.h */
