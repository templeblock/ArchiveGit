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
*    slchrmac.h - Macintosh Extended ASCII character set defines.       *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines based on the Macintosh Extended  
            Character Set. It also defines some characters that do not
            exist in this character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as M_FIL_#.


        Cautions:
            None.

        Creation Date:
            02-aug-89        


*-----------------------------------------------------------------------*/
#ifndef SLCHRMAC_H

#define SLCHRMAC_H


/* MACINTOSH CHARACTER SET DEFINES                                      */

/* M_C_... = MACINTOSH CAPITAL ...                                      */
/* M_L_... = MACINTOSH LOWER   ...                                      */


#define M_C_A_UMLAUT       128    
#define M_C_A_ANGSTROM     129    
#define M_C_C_CEDILLA      130    
#define M_C_E_ACUTE        131    
#define M_C_N_TILDE        132
#define M_C_O_UMLAUT       133
#define M_C_U_UMLAUT       134
#define M_L_A_ACUTE        135
#define M_L_A_GRAVE        136
#define M_L_A_CIRCUMFLX    137
#define M_L_A_UMLAUT       138
#define M_L_A_TILDE        139
#define M_L_A_ANGSTROM     140
#define M_L_C_CEDILLA      141
#define M_L_E_ACUTE        142
#define M_L_E_GRAVE        143
#define M_L_E_CIRCUMFLX    144
#define M_L_E_UMLAUT       145
#define M_L_I_ACUTE        146
#define M_L_I_GRAVE        147
#define M_L_I_CIRCUMFLX    148
#define M_L_I_UMLAUT       149
#define M_L_N_TILDE        150
#define M_L_O_ACUTE        151
#define M_L_O_GRAVE        152
#define M_L_O_CIRCUMFLX    153
#define M_L_O_UMLAUT       154
#define M_L_O_TILDE        155
#define M_L_U_ACUTE        156
#define M_L_U_GRAVE        157
#define M_L_U_CIRCUMFLX    158
#define M_L_U_UMLAUT       159

#define M_FIL_1            160      /* A dagger                            */

#define M_DEGREE           161
#define M_CENT             162
#define M_POUND            163
#define M_SECTION          164
#define M_MIDDLE_DOT       165      /* A bullet substitutes for            */
                                    /* 183 in the DEC Multinational        */
                                    /* Character Set                       */
#define M_PARAGRAPH        166
#define M_L_S_SHARP        167
#define M_REG_TRADEMARK    168
#define M_COPYRIGHT        169

#define M_TRADEMARK        170      /* Trademark                           */

#define M_ACUTE            171
#define M_UMLAUT           172

#define M_FIL_2            173      /* Not-equal character                 */

#define M_C_AE_LIGATURE    174
#define M_C_O_SLASH        175

#define M_TWO_SUPER        176      /* Infinity character substitutes for  */
                                    /* 178 in the DEC Multinational        */
                                    /* Character Set                       */
#define M_PLUS_MINUS       177   

#define M_THREE_SUPER      178      /* Less-equal character substitutes for*/
                                    /* 179 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_FIL_3            179      /* Greater-equal character             */

#define M_YEN              180
#define M_MICRO_SIGN       181

#define M_ONE_SUPER        182      /* Partial-differential character      */
                                    /* substitutes for 185 in the DEC      */
                                    /* Character Set                       */

#define M_ONE_QUARTER      183      /* N-ary summation substitutes         */
                                    /* 188 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_ONE_HALF         184      /* N-ary product character substitutes */
                                    /* for 189 in the DEC Multinational    */
                                    /* Character Set                       */

#define M_THREE_FOURTHS    185      /* Lowercase pi character substitutes  */
                                    /* for 190 in the ISO Latin-1 Character*/
                                    /* set                                 */

#define M_FIL_4            186      /* Integral character                  */

#define M_FEM_ORDINAL      187
#define M_MAS_ORDINAL      188

#define M_FIL_5            189      /* OHM                                 */

#define M_L_AE_LIGATURE    190
#define M_L_O_SLASH        191
#define M_INV_QUESTION     192
#define M_INV_EXCLAMATN    193
#define M_NOT_SIGN         194

#define M_FIL_6            195      /* Square root character               */

#define M_FLORIN           196      /* Latin small letter script f substitues */
                                    /* for the Dutch currency symbol (florin) */

#define M_FIL_7            197      /* Nearly-equal character              */
#define M_FIL_8            198      /* Increment character                 */

#define M_LEFT_GUILL       199
#define M_RIGHT_GUILL      200
/* Redefined for Inso product backward compatibility                       */
#define M_OPEN_GUILL       M_LEFT_GUILL
#define M_CLOS_GUILL       M_RIGHT_GUILL 

#define M_ELLIPSIS         201
#define M_NBSP             202
#define M_C_A_GRAVE        203
#define M_C_A_TILDE        204
#define M_C_O_TILDE        205
#define M_C_OE_LIGATURE    206
#define M_L_OE_LIGATURE    207             
#define M_SHY              208      /* Shorter than 45 in Standard         */
                                    /* ASCII Character Set                 */

                                    /* The Unicode Standard has it defined */
                                    /* as hyphen, non-breaking hyphen,     */
                                    /* figure dash, and en dash            */
#define M_M_DASH           209

#define M_66_D_QUOTE       210
#define M_99_D_QUOTE       211
/* Redefined for Inso product backward compatibility                       */
#define M_OPEN_D_QUOTE     M_66_D_QUOTE
#define M_CLOSE_D_QUOTE    M_99_D_QUOTE

#define M_6_S_QUOTE        212
#define M_9_S_QUOTE        213
/* Redefined for Inso product backward compatibility                       */
#define M_OPEN_S_QUOTE     M_6_S_QUOTE
#define M_CLOSE_S_QUOTE    M_9_S_QUOTE

#define M_DIVISION         214

#define M_FIL_9            215      /* Lozenge character                   */

#define M_L_Y_UMLAUT       216

#define M_C_Y_UMLAUT       217      /* Dummy character substitutes for     */
                                    /* Y umlaut                            */

#define M_FIL_10           218      /* Dummy character                     */

#define M_GEN_CURRENCY     219      /* Dummy character substitutes         */
                                    /* for 164 in the ISO Latin-1 Character*/
                                    /* Set                                 */

#define M_FIL_11           220      /* Dummy character                     */
#define M_FIL_12           221      /* Dummy character                     */
#define M_FIL_13           222      /* Dummy character                     */

#define M_MULTIPLICATION   223      /* Dummy character substitutes for     */
                                    /* 215 in the ISO Latin-1 Character    */
                                    /* Set                                 */

#define M_FIL_14           224      /* Dummy character                     */
#define M_EN_DASH          225      /* Dummy character substitutes for     */
                                    /* en dash                             */

#define M_FIL_15           226      /* Dummy character                     */
#define M_FIL_16           227      /* Dummy character                     */

#define M_LT_S_GUILL       228      /* Dummy character substitutes for     */
                                    /* left pointing single guillemet      */

#define M_C_A_CIRCUMFLX    229      /* Dummy character for                 */
                                    /* 194 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_E_CIRCUMFLX    230      /* Dummy character substitutes for     */
                                    /* 202 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_A_ACUTE        231      /* Dummy character substitutes         */
                                    /* for 193 in the DEC Multinational    */
                                    /* Character Set                       */

#define M_C_E_UMLAUT       232      /* Dummy character substitutes         */
                                    /* for 203 in the DEC Multinational    */
                                    /* Character Set                       */

#define M_C_E_GRAVE        233      /* Dummy character substitutes         */
                                    /* for 200 in the DEC Multinational    */
                                    /* Character Set                       */

#define M_C_I_ACUTE        234      /* Dummy character substitutes for     */
                                    /* 205 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_I_CIRCUMFLX    235      /* Dummy character substitutes for     */
                                    /* 206 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_I_UMLAUT       236      /* Dummy character substitutes for     */
                                    /* 207 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_I_GRAVE        237      /* Dummy character substitutes for     */
                                    /* 204 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_O_ACUTE        238      /* Dummy character substitutes for     */
                                    /* 211 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_O_CIRCUMFLX    239      /* Dummy character substitutes for     */
                                    /* 212 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_RT_S_GUILL       240      /* Dummy character substitutes for     */
                                    /* right pointing single guillemet     */

#define M_C_O_GRAVE        241      /* Dummy character substitutes for     */
                                    /* 210 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_U_ACUTE        242      /* Dummy character substitutes for     */
                                    /* 218 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_U_CIRCUMFLX    243      /* Dummy character substitutes for     */
                                    /* 219 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_U_GRAVE        244      /* Dummy character substitutes for     */
                                    /* 217 in the DEC Multinational        */
                                    /* Character Set                       */

#define M_C_EDH            245      /* Dummy character substitutes for     */
                                    /* 208 in the ISO Latin-1 Character set*/

#define M_C_Y_ACUTE        246      /* Dummy character substitutes for     */
                                    /* 221 in the ISO Latin-1 Character set*/

#define M_L_THORN          247      /* Dummy character substitutes for     */
                                    /* 222 in the ISO Latin-1 Character set*/

#define M_MACRON           248      /* Dummy character substitutes for     */
                                    /* 175 in the ISO Latin-1 Character set*/

#define M_L_EDH            249      /* Dummy character substitutes for     */
                                    /* 240 in the ISO Latin-1 Character set*/

#define M_L_Y_ACUTE        250      /* Dummy character substitutes for     */
                                    /* 253 in the ISO Latin-1 Character set*/

#define M_C_THORN          251      /* Dummy character substitutes for     */
                                    /* 254 in the ISO Latin-1 Character set*/

#define M_CEDILLA          252      /* Dummy character substitutes         */
                                    /* for 184 in The Latin-1 Character set*/

#define M_L9_S_QUOTE       253      /* Dummy character substitutes for     */
                                    /* for low open single quote           */

#define M_L99_D_QUOTE      254      /* Dummy character substitutes for     */
                                    /* for low open double quote           */

#define M_BROKEN_VBAR      255      /* Dummy character substitutes for     */
                                    /* 166 in the ISO Latin-1 Character Set*/


#endif /* SLCHRMAC_H */

/* End of file - slchrmac.h */
