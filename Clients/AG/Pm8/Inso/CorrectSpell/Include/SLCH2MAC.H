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
*        slch2mac.h - MAC Extended ASCII character set defines.         *
*                     Slavic (Latin-2)                                  *  
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the MACINTOSH
            Extended ASCII character set for Slavic (Latin-2). It also 
            defines some characters that do not exist in this character 
            set but are essential for the multilingual language software 
            products.

            Some nonessential characters are defined as M2_FIL_#.

        Cautions:
            None.

        Creation Date:
            12-jan-93   PJA

        Change Record:


*-----------------------------------------------------------------------*/
#ifndef SLCH2MAC_H

#define SLCH2MAC_H


/* MACINTOSH CHARACTER SET DEFINES SLAVIC (LATIN II)                     */

/* M2_C_... = MAC CAPITAL ...                                            */
/* M2_L_... = MAC LOWER   ...                                            */

#define M2_C_A_UMLAUT         128
#define M2_C_A_CIRCUMFLX      129  /* character substitutes for		    */
                                   /*			A-circumflex        */
#define M2_L_A_CIRCUMFLX      130  /* character substitutes for		    */
                                   /*			a-circumflex        */
#define M2_C_E_ACUTE          131
#define M2_C_A_OGONEK         132
#define M2_C_O_UMLAUT         133
#define M2_C_U_UMLAUT         134
#define M2_L_A_ACUTE          135
#define M2_L_A_OGONEK         136
#define M2_C_C_HACEK          137
#define M2_L_A_UMLAUT         138
#define M2_L_C_HACEK          139
#define M2_C_C_ACUTE          140
#define M2_L_C_ACUTE          141
#define M2_L_E_ACUTE          142
#define M2_C_Z_ACUTE          143

#define M2_L_Z_ACUTE          144
#define M2_C_D_HACEK          145
#define M2_L_I_ACUTE          146
#define M2_L_D_HACEK          147
#define M2_C_C_CEDILLA        148  /* character substitutes for		    */
                                   /*			C-cedilla     	    */
#define M2_C_I_CIRCUMFLX      149  /* character substitutes for		    */
                                   /*			I-circumflex  	    */
#define M2_L_C_CEDILLA        150  /* character substitutes for		    */
                                   /*			c-cedilla     	    */
#define M2_L_O_ACUTE          151
#define M2_MULTIPLICATION     152  /* character substitutes for		    */
                                   /*		multiplication sign   	    */
#define M2_L_O_CIRCUMFLX      153
#define M2_L_O_UMLAUT         154
#define M2_L_I_CIRCUMFLX      155  /* character substitutes for		    */
                                   /*			i-circumflex  	    */
#define M2_L_U_ACUTE          156
#define M2_C_E_HACEK          157
#define M2_L_E_HACEK          158
#define M2_L_U_UMLAUT         159

#define M2_DAGGER             160  /* character substitutes for	dagger      */
#define M2_DEGREE             161
#define M2_C_E_OGONEK         162
#define M2_OGONEK             163  /* character substitutes for	ogonek 	    */
#define M2_SECTION            164
#define M2_BULLET             165
#define M2_SPACING_DOT_ABOVE  166  /* character substitutes for		    */
                                   /*			spacing dot above   */
#define M2_L_S_SHARP          167
#define M2_REG_TRADEMARK      168
#define M2_COPYRIGHT          169
#define M2_TRADEMARK          170
#define M2_L_E_OGONEK         171
#define M2_ELLIPSIS           172  /* character substitutes for	ellipsis    */
#define M2_C_S_CEDILLA        173
#define M2_N_DASH             174  /* character substitutes for	en dash     */
#define M2_M_DASH             175  /* character substitutes for	em dash     */

#define M2_BROKEN_VBAR        176  /* character substitutes for	broken vbar */
#define M2_C_A_BREVE          177  /* character substitutes for		    */
                                   /*			A-breve             */
#define M2_L_A_BREVE          178  /* character substitutes for		    */
                                   /*			a-breve             */
#define M2_MICRO_SIGN         179  /* character substitutes for	micro sign  */
#define M2_PARAGRAPH          180  /* character substitutes for	paragraph   */
#define M2_MIDDLE_DOT         181  /* character substitutes for	middle dot  */
#define M2_C_E_UMLAUT         182  /* character substitutes for		    */
                                   /*			E-umlaut            */
#define M2_L_D_BAR            183
#define M2_L_L_SLASH          184
#define M2_FIL_1              185
#define M2_FIL_2              186
#define M2_C_L_HACEK          187
#define M2_L_L_HACEK          188
#define M2_C_L_ACUTE          189
#define M2_L_L_ACUTE          190
#define M2_L_E_UMLAUT         191  /* character substitutes for		    */
                                   /*			e-umlaut            */

#define M2_PLUS_MINUS         192  /* character substitutes for		    */
                                   /*			plus-minus sign     */
#define M2_C_N_ACUTE          193
#define M2_NOT_SIGN           194
#define M2_L_S_CEDILLA        195  /* character substitutes for		    */
                                   /*			s-cedilla     	    */
#define M2_L_N_ACUTE          196
#define M2_C_N_HACEK          197
#define M2_D_DAGGER           198  /* character substitutes for		    */
                                   /*			double dagger 	    */
#define M2_LEFT_GUILL         199
#define M2_RIGHT_GUILL        200
#define M2_GEN_CURRENCY       201  /* character substitutes for		    */
                                   /*			gen currency symbol */
#define M2_NBSP               202
#define M2_L_N_HACEK          203
#define M2_C_O_DOUBLE_ACUTE   204
#define M2_C_T_CEDILLA        205  /* character substitutes for		    */
                                   /*			T-cedilla           */
#define M2_L_O_DOUBLE_ACUTE   206
#define M2_PER_MILLE          207  /* character substitutes for		    */
                                   /*			per mille sign      */

#define M2_SHY                208
#define M2_ACUTE              209  /* character substitutes for	acute       */
#define M2_66_D_QUOTE         210
#define M2_99_D_QUOTE         211
#define M2_6_S_QUOTE          212
#define M2_9_S_QUOTE          213
#define M2_DIVISION           214
#define M2_L_T_CEDILLA        215  /* character substitutes for		    */
                                   /*			t-cedilla           */
#define M2_DOUBLE_ACUTE       216  /* character substitutes for		    */
                                   /*			double acute        */
#define M2_C_R_ACUTE          217
#define M2_L_R_ACUTE          218
#define M2_C_R_HACEK          219
#define M2_LT_S_GUILL         220  /* Left pointing single guillemet        */
#define M2_RT_S_GUILL         221  /* Right pointing single guillemet       */
#define M2_L_R_HACEK          222
#define M2_BREVE              223  /* character substitutes for	breve       */

#define M2_CEDILLA            224  /* character substitutes for	cedilla     */
#define M2_C_S_HACEK          225
#define M2_L9_S_QUOTE         226
#define M2_L99_D_QUOTE        227
#define M2_L_S_HACEK          228
#define M2_C_S_ACUTE          229
#define M2_L_S_ACUTE          230
#define M2_C_A_ACUTE          231
#define M2_C_T_HACEK          232
#define M2_L_T_HACEK          233
#define M2_C_I_ACUTE          234
#define M2_C_Z_HACEK          235
#define M2_L_Z_HACEK          236
#define M2_FIL_3              237
#define M2_C_O_ACUTE          238
#define M2_C_O_CIRCUMFLX      239

#define M2_FIL_4              240
#define M2_C_U_RING           241
#define M2_C_U_ACUTE          242
#define M2_L_U_RING           243
#define M2_C_U_DOUBLE_ACUTE   244
#define M2_L_U_DOUBLE_ACUTE   245
#define M2_FIL_5              246
#define M2_FIL_6              247
#define M2_C_Y_ACUTE          248
#define M2_L_Y_ACUTE          249
#define M2_C_D_BAR            250
#define M2_C_Z_DOT            251
#define M2_C_L_SLASH          252  /* character substitutes for		    */
                                   /*			L-slash             */
#define M2_L_Z_DOT            253
#define M2_UMLAUT             254  /* character substitutes for	umlaut      */
#define M2_HACEK              255  /* character substitutes for	hacek  	    */


#endif /* SLCH2MAC_H */

/* End of file - slch2mac.h */



  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
