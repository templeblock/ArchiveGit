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
*        slchrdos.h - IBM PC Extended ASCII character set defines.      *
*                     (code page 437)                                   *  
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the IBM PC
            Extended ASCII character set. It also defines some characters
            that do not exist in this character set but are essential for
            the multilingual language software products.

            Some nonessential characters are defined as P_FIL_#.

        Cautions:
            None.

        Creation Date:
            18-jul-89        


*-----------------------------------------------------------------------*/
#ifndef SLCHRDOS_H

#define SLCHRDOS_H


/* IBM PC CHARACTER SET DEFINES                                         */

/* P_C_... = PC CAPITAL ...                                             */
/* P_L_... = PC LOWER   ...                                             */

#define P_C_C_CEDILLA      128
#define P_L_U_UMLAUT       129
#define P_L_E_ACUTE        130
#define P_L_A_CIRCUMFLX    131
#define P_L_A_UMLAUT       132
#define P_L_A_GRAVE        133
#define P_L_A_ANGSTROM     134
#define P_L_C_CEDILLA      135
#define P_L_E_CIRCUMFLX    136
#define P_L_E_UMLAUT       137
#define P_L_E_GRAVE        138
#define P_L_I_UMLAUT       139
#define P_L_I_CIRCUMFLX    140
#define P_L_I_GRAVE        141
#define P_C_A_UMLAUT       142
#define P_C_A_ANGSTROM     143
#define P_C_E_ACUTE        144
#define P_L_AE_LIGATURE    145
#define P_C_AE_LIGATURE    146
#define P_L_O_CIRCUMFLX    147
#define P_L_O_UMLAUT       148
#define P_L_O_GRAVE        149
#define P_L_U_CIRCUMFLX    150
#define P_L_U_GRAVE        151
#define P_L_Y_UMLAUT       152
#define P_C_O_UMLAUT       153
#define P_C_U_UMLAUT       154
#define P_CENT             155
#define P_POUND            156
#define P_YEN              157

#define P_FIL_1            158   /* Spanish currency symbol (peseta)    */

#define P_FLORIN           159   /* Latin small letter script f substitutes */
                                 /* for the Dutch currency symbol (florin)  */

#define P_L_A_ACUTE        160
#define P_L_I_ACUTE        161
#define P_L_O_ACUTE        162
#define P_L_U_ACUTE        163
#define P_L_N_TILDE        164
#define P_C_N_TILDE        165
#define P_FEM_ORDINAL      166
#define P_MAS_ORDINAL      167
#define P_INV_QUESTION     168

#define P_FIL_2            169   /* A reversed not sign                 */

#define P_NOT_SIGN         170   
                                 
#define P_ONE_HALF         171
#define P_ONE_QUARTER      172
#define P_INV_EXCLAMATN    173

#define P_LEFT_GUILL       174
#define P_RIGHT_GUILL      175

/* Redefined for Inso product backward compatibility                    */
#define P_OPEN_GUILL       P_LEFT_GUILL
#define P_CLOS_GUILL       P_RIGHT_GUILL 

#define P_6_S_QUOTE        176   /* Light shade substitutes             */
                                 /* for 6-like single quote             */
#define P_9_S_QUOTE        177   /* Medium shade substitutes            */
                                 /* for 9-like single quote             */

/* Redefined for Inso product backward compatibility                    */
#define P_OPEN_S_QUOTE     P_6_S_QUOTE
#define P_CLOSE_S_QUOTE    P_9_S_QUOTE


#define P_66_D_QUOTE       178   /* Dark shade substitutes              */
                                 /* for 66-like double quote            */

#define P_99_D_QUOTE       179   /* Light vertical form substitutes     */
                                 /* for 99-like double quote            */

/* Redefined for Inso product backward compatibility                    */
#define P_OPEN_D_QUOTE     P_66_D_QUOTE
#define P_CLOSE_D_QUOTE    P_99_D_QUOTE

#define P_ELLIPSIS         180   /* Light vertical and left form        */
                                 /* substitutes for 3-dot ellipsis      */

#define P_M_DASH           181   /* Vertical single and left double form*/
                                 /* substitutes for em-dash             */

#define P_C_Y_ACUTE        182   /* Vertical double and left single form*/
                                 /* substitutes                         */
                                 /* for 221 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_L_Y_ACUTE        183   /* Down double and left single form    */
                                 /* substitutes                         */
                                 /* for 253 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_THREE_FOURTHS    184   /* Down single and left double form    */
                                 /* substitutes for 190 in the ISO      */
                                 /* Latin-1 Character Set               */

#define P_GEN_CURRENCY     185   /* Double vertical and left form       */
                                 /* substitutes for 164 in the ISO      */
                                 /* Latin-1 Character Set               */

#define P_FIL_3            186   /* Double vertical form                */

#define P_L9_S_QUOTE       187   /* Double down and left form           */
                                 /* substitutes                         */
                                 /* for low 9-like single quote         */

#define P_FIL_4            188   /* Double up and left form             */

#define P_L99_D_QUOTE      189   /* Up double and left single form      */
                                 /* substitutes                         */
                                 /* for low 99-like double quote        */

#define P_UMLAUT           190   /* Up single and left double form      */
                                 /* substitutes for 168 in the ISO      */
                                 /* Latin-1 Character Set               */

#define P_BROKEN_VBAR      191   /* Light down and left form substitutes*/
                                 /* for 166 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_C_I_GRAVE        192   /* Light up and right form substitutes */
                                 /* for 204 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_I_ACUTE        193   /* Light up and horizontal form        */
                                 /* substitutes                         */
                                 /* for 205 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_I_CIRCUMFLX    194   /* Light down and horizontal form      */
                                 /* substitutes                         */
                                 /* for 206 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_I_UMLAUT       195   /* Light vertical and right form       */
                                 /* substitutes                         */
                                 /* for 207 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_O_GRAVE        196   /* Light horizontal form substitutes   */
                                 /* for 210 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_O_ACUTE        197   /* Light vertical and horizontal form  */
                                 /* substitutes                         */
                                 /* for 211 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_O_CIRCUMFLX    198   /* Vertical single and right double    */
                                 /* form substitutes                    */
                                 /* for 212 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_O_TILDE        199   /* Vertical double and right single    */
                                 /* form substitutes                    */
                                 /* for 213 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_OE_LIGATURE    200   /* Double up and right form substitutes*/
                                 /* for OE-ligature                     */

#define P_C_U_GRAVE        201   /* Double down and right form          */
                                 /* substitutes                         */
                                 /* for 217 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_U_ACUTE        202   /* Double up and horizontal form       */
                                 /* substitutes                         */
                                 /* for 218 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_U_CIRCUMFLX    203   /* Double down and horizontal form     */
                                 /* substitutes                         */
                                 /* for 219 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_Y_UMLAUT       204   /* Double vertical and right form      */
                                 /* substitutes for IJ ligature         */

#define P_L_A_TILDE        205   /* Double horizontal form substitutes  */
                                 /* for 227 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_L_O_TILDE        206   /* Double vertical and horizontal form */
                                 /* substitutes                         */
                                 /* for 245 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_L_OE_LIGATURE    207   /* Up single and horizontal double     */
                                 /* substitutes for oe-ligature         */

#define P_C_A_GRAVE        208   /* Up double and horizontal single form*/
                                 /* substitutes                         */
                                 /* for 192 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_A_ACUTE        209   /* Down single and horizontal double   */
                                 /* form substitutes                    */
                                 /* for 193 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_A_CIRCUMFLX    210   /* Down double and horizontal single   */
                                 /* form substitutes                    */
                                 /* for 194 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_A_TILDE        211   /* Up double and right single form     */
                                 /* substitutes                         */
                                 /* for 195 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_E_GRAVE        212   /* Up single and right double form     */
                                 /* substitutes                         */
                                 /* for 200 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_E_CIRCUMFLX    213   /* Down single and right double form   */
                                 /* substitutes                         */
                                 /* for 202 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_C_E_UMLAUT       214   /* Down double and right single form   */
                                 /* substitutes                         */
                                 /* for 203 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_SHY              215   /* Vertical double and horizontal      */
                                 /* single form substitutes             */
                                 /* for 173 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_REG_TRADEMARK    216   /* Vertical single and horizontal      */
                                 /* double form substitutes             */
                                 /* for 174 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_MACRON           217   /* Light up and left form substitutes  */
                                 /* for 175 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_ACUTE            218   /* Light down and right form           */
                                 /* substitutes for 180 in the ISO      */
                                 /* Latin-1 Character Set               */

#define P_C_THORN          219   /* Full block substitutes for          */
                                 /* for 222 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_C_EDH            220   /* Lower half block substitutes        */
                                 /* for 208 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_L_EDH            221   /* Left half block substitutes         */
                                 /* for 240 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_CEDILLA          222   /* Right half block substitutes        */
                                 /* 184 in the ISO Latin-1 Character Set*/

#define P_EN_DASH          223   /* Upper half block substitutes for    */
                                 /* en dash                             */


#define P_FIL_5            224   /* Greek lowercase alpha               */

#define P_L_S_SHARP        225

#define P_L_THORN          226   /* Greek uppercase gamma substitutes   */
                                 /* for 254 in the ISO Latin-1 Character*/
                                 /* Set                                 */

#define P_FIL_6            227   /* Greek lowercase pi                  */
#define P_FIL_7            228   /* Greek uppercase sigma               */
#define P_FIL_8            229   /* Greek lowercase sigma               */

#define P_MICRO_SIGN       230                                          

#define P_FIL_9            231   /* Greek lowercase tau                 */
#define P_FIL_10           232   /* Greek uppercase phi                 */

#define P_C_O_SLASH        233   /* Greek uppercase theta substitutes   */
                                 /* for 216 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_FIL_11           234   /* Greek uppercase omega               */
#define P_FIL_12           235   /* Greek lowercase delta               */
#define P_FIL_13           236   /* Math infinity symbol                */

#define P_L_O_SLASH        237   /* Greek lowercase phi substitutes     */
                                 /* for 248 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_FIL_14           238   /* Greek lowercase epsilon             */
#define P_FIL_15           239   /* A math symbol: intersection         */
#define P_FIL_16           240   /* A math symbol: identical with       */

#define P_PLUS_MINUS       241

#define P_PARAGRAPH        242   /* A math symbol, greater than or      */
                                 /* equal to, substitutes for the       */
                                 /* paragraph symbol -- 182 in the ISO  */
                                 /* Latin-1 Character Set               */

#define P_SECTION          243   /* A math symbol, less than or         */
                                 /* equal to, substitutes for the       */
                                 /* section symbol -- 167 in the ISO    */
                                 /* Latin-1 Character Set               */

#define P_LT_S_GUILL       244   /* A math symbol: top part of          */
                                 /* integral substitutes for            */
                                 /* left pointing single guillemet      */

#define P_TRADEMARK        245   /* A math symbol, bottom part of       */
                                 /* integral, substitutes for the trade-*/
                                 /* mark symbol                         */

#define P_DIVISION         246

#define P_RT_S_GUILL       247   /* A math symbol: nearly equal to      */
                                 /* substitutes for right pointing      */
                                 /* single guillemet                    */

#define P_DEGREE           248                                          

#define P_MULTIPLICATION   249   /* A bullet operator substitutes       */
                                 /* for 215 in the ISO Latin-1 Character*/
                                 /* Set                                 */
#define P_MIDDLE_DOT       250                                          

#define P_COPYRIGHT        251   /* A math symbol: square root;         */
                                 /* for 169 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_ONE_SUPER        252   /* Superscript Latin small letter n    */
                                 /* substitutes for 185 in the ISO      */
                                 /* Latin-1 Character Set               */
#define P_TWO_SUPER        253

#define P_THREE_SUPER      254   /* Black square substitutes            */
                                 /* for 179 in the ISO Latin-1          */
                                 /* Character Set                       */

#define P_NBSP             255   /* A space substitutes                 */
                                 /* for 160 in the ISO Latin-1 Character*/
                                 /* Set                                 */


#endif /* SLCHRDOS_H */

/* End of file - slchrdos.h */
