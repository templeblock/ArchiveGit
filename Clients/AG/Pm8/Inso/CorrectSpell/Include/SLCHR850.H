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
*        slchr850.h - IBM PC Extended ASCII Latin 1 character set       *
*                     defines (code page 850)                           *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that name characters from code page
            850, the Latin-1 -like page of character codes in the IBM PC
            Extended ASCII character sets.  It also defines some characters
            that do not exist in cp850 character set but are essential for
            the multilingual language software products.  Those defines are
            mapped using available spots:
                Use the character whose value matches the ISO Latin-1 code if
                    it is one we don't need for multilingual product support
                If that spot is already taken, use the character code from
                    the Windows character set
                If that spot is already taken, arbitrarily map it to an
                    unsed (FIL) character.

            Some nonessential characters (graphics characters that aren't
            recycled to represent multilingual chars) are defined as 
            P8_FIL_#.

        Cautions:
            Do not confuse the character code values in code page 850 with 
            the code values in ISO Latin-1.  They are not the same, even 
            though some documentation calls code page 850 "Latin 1".  Code
            page 850 can print more ISO Latin-1 characters than code page
            437, the default PC character set.

        Creation Date:
            09-nov-93   ARH/JCC     Cloned from SLCHRDOS.H

        Change Record:

*-----------------------------------------------------------------------*/

#ifndef SLCHR850_H
#define SLCHR850_H


/* IBM PC CHARACTER SET DEFINES                                         */

/* P8_C_... = PC CAPITAL ...                                             */
/* P8_L_... = PC LOWER   ...                                             */

#define P8_C_C_CEDILLA      128
#define P8_L_U_UMLAUT       129
#define P8_L_E_ACUTE        130
#define P8_L_A_CIRCUMFLX    131
#define P8_L_A_UMLAUT       132
#define P8_L_A_GRAVE        133
#define P8_L_A_ANGSTROM     134
#define P8_L_C_CEDILLA      135
#define P8_L_E_CIRCUMFLX    136
#define P8_L_E_UMLAUT       137
#define P8_L_E_GRAVE        138
#define P8_L_I_UMLAUT       139
#define P8_L_I_CIRCUMFLX    140
#define P8_L_I_GRAVE        141
#define P8_C_A_UMLAUT       142
#define P8_C_A_ANGSTROM     143
#define P8_C_E_ACUTE        144
#define P8_L_AE_LIGATURE    145
#define P8_C_AE_LIGATURE    146
#define P8_L_O_CIRCUMFLX    147
#define P8_L_O_UMLAUT       148
#define P8_L_O_GRAVE        149
#define P8_L_U_CIRCUMFLX    150
#define P8_L_U_GRAVE        151
#define P8_L_Y_UMLAUT       152
#define P8_C_O_UMLAUT       153
#define P8_C_U_UMLAUT       154
#define P8_L_O_SLASH        155
#define P8_POUND            156
#define P8_C_O_SLASH        157
#define P8_MULTIPLICATION   158

#define P8_FLORIN           159 /* Latin small letter script f substitutes */
                                /* for the Dutch currency symbol (frorin)  */

#define P8_L_A_ACUTE        160
#define P8_L_I_ACUTE        161
#define P8_L_O_ACUTE        162
#define P8_L_U_ACUTE        163
#define P8_L_N_TILDE        164
#define P8_C_N_TILDE        165
#define P8_FEM_ORDINAL      166
#define P8_MAS_ORDINAL      167
#define P8_INV_QUESTION     168
#define P8_REG_TRADEMARK    169
#define P8_NOT_SIGN         170
#define P8_ONE_HALF         171
#define P8_ONE_QUARTER      172
#define P8_INV_EXCLAMATN    173

#define P8_LEFT_GUILL       174
#define P8_RIGHT_GUILL      175

/* Redefined for Inso product backward compatibility - some products
 * used OPEN_GUILL instead of LEFT_GUILL, CLOS_GUILL instead of
 * RIGHT_GUILL.
*/
#define P8_OPEN_GUILL       P8_LEFT_GUILL
#define P8_CLOS_GUILL       P8_RIGHT_GUILL

#define P8_6_S_QUOTE        176  /* Light shade substitutes             */
                                 /* for 6-like single quote             */
#define P8_9_S_QUOTE        177  /* Medium shade substitutes            */
                                 /* for 9-like single quote             */

/* Redefined for Inso product backward compatibility - some products
 * used OPEN_S_QUOTE instead of 6_S_QUOTE, CLOSE_S_QUOTE instead of
 * 9_S_QUOTE.
*/
#define P8_OPEN_S_QUOTE     P8_6_S_QUOTE
#define P8_CLOSE_S_QUOTE    P8_9_S_QUOTE


#define P8_66_D_QUOTE       178  /* Dark shade substitutes              */
                                 /* for 66-like double quote            */

#define P8_99_D_QUOTE       179  /* Light vertical form substitutes     */
                                 /* for 99-like double quote            */

/* Redefined for Inso product backward compatibility - some products
 * used OPEN_D_QUOTE instead of 66_D_QUOTE, CLOSE_D_QUOTE instead of
 * 99_D_QUOTE.
*/
#define P8_OPEN_D_QUOTE     P8_66_D_QUOTE
#define P8_CLOSE_D_QUOTE    P8_99_D_QUOTE

#define P8_ELLIPSIS         180  /* Light vertical and left form        */
                                 /* substitutes for 3-dot ellipsis      */

#define P8_C_A_ACUTE        181
#define P8_C_A_CIRCUMFLX    182
#define P8_C_A_GRAVE        183
#define P8_COPYRIGHT        184

#define P8_M_DASH           185  /* Substitutes for em dash             */

#define P8_FIL_1            186  /* Double vertical form                */

#define P8_L9_S_QUOTE       187  /* Double down and left form           */
                                 /* substitutes                         */
                                 /* for low 9-like single quote         */

#define P8_FIL_2            188  /* Double up and left form             */

#define P8_CENT             189
#define P8_YEN              190

#define P8_TRADEMARK        191 /* Light down and left form substitutes */
                                /* for the trademark sign               */
#define P8_L_OE_LIGATURE    192 /* Light up and right form substitutes  */
                                /* for the oe ligature                  */
#define P8_L99_D_QUOTE      193 /* Light up and horizontal form         */
                                /* substitutes for the lower 99-like    */
                                /* double quote                         */

#define P8_FIL_3            194 /* Light down and horizontal form       */
#define P8_FIL_4            195 /* Light vertical and right form        */
#define P8_FIL_5            196 /* Light horizontal form                */
#define P8_FIL_6            197 /* Light vertical and horizontal form   */

#define P8_L_A_TILDE        198
#define P8_C_A_TILDE        199

#define P8_C_OE_LIGATURE    200  /* Double up and right form substitutes*/
                                 /* for OE-ligature                     */

#define P8_FIL_7            201 /* Double down and right form           */
#define P8_FIL_8            202 /* Double up and horizontal form        */
#define P8_FIL_9            203 /* Double down and horizontal form      */

#define P8_C_Y_UMLAUT       204 /* Double vertical and right form       */
                                /* substitutes for Y umlaut             */

#define P8_FIL_10           205 /* Double horizontal form               */
#define P8_FIL_11           206 /* Double vertical and horizontal form  */

#define P8_GEN_CURRENCY     207
#define P8_L_EDH            208
#define P8_C_EDH            209
#define P8_C_E_CIRCUMFLX    210
#define P8_C_E_UMLAUT       211
#define P8_C_E_GRAVE        212
#define P8_FIL_12           213 /* Lower Case Dotless I                 */
#define P8_C_I_ACUTE        214
#define P8_C_I_CIRCUMFLX    215
#define P8_C_I_UMLAUT       216

#define P8_FIL_13           217 /* Light up and left form               */
#define P8_FIL_14           218 /* Light down and right form            */
#define P8_FIL_15           219 /* Full block                           */
#define P8_FIL_16           220 /* Lower half block                     */
                 
#define P8_BROKEN_VBAR      221
#define P8_C_I_GRAVE        222

#define P8_EN_DASH          223 /* Upper half block substitutes for     */
                                /* en dash                              */

#define P8_C_O_ACUTE        224
#define P8_L_S_SHARP        225
#define P8_C_O_CIRCUMFLX    226
#define P8_C_O_GRAVE        227
#define P8_L_O_TILDE        228
#define P8_C_O_TILDE        229
#define P8_MICRO_SIGN       230
#define P8_L_THORN          231
#define P8_C_THORN          232
#define P8_C_U_ACUTE        233
#define P8_C_U_CIRCUMFLX    234
#define P8_C_U_GRAVE        235
#define P8_L_Y_ACUTE        236
#define P8_C_Y_ACUTE        237
#define P8_MACRON           238 /* Spacing Macron                       */
#define P8_ACUTE            239
#define P8_SHY              240
#define P8_PLUS_MINUS       241

#define P8_LT_S_GUILL       242 /* Spacing double underscore substitutes*/
                                /* for left pointing single guillemet   */

#define P8_THREE_FOURTHS    243
#define P8_PARAGRAPH        244
#define P8_SECTION          245
#define P8_DIVISION         246
#define P8_CEDILLA          247 /* Spacing Cedilla                      */
#define P8_DEGREE           248
#define P8_UMLAUT           249 /* Spacing Umlaut                       */
#define P8_MIDDLE_DOT       250
#define P8_ONE_SUPER        251
#define P8_THREE_SUPER      252
#define P8_TWO_SUPER        253

#define P8_RT_S_GUILL       254 /* Black square substitutes for         */
                                /* right pointing single guillemet      */

#define P8_NBSP             255  /* A space substitutes                 */
                                 /* for 160 in the ISO Latin-1 Character*/
                                 /* Set                                 */


#endif  /* SLCHR850_H */

/* End of file - slchr850.h */

