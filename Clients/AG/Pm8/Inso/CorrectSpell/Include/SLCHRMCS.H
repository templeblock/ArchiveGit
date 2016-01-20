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
*        slchrmcs.h - The Inso Master Character Set defines *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:

            This file contains #defines based on the ISO Latin-1
            Character Set and the DEC Multinational Character Set.
            It also defines some characters that do not
            exist in these character sets but are essential for the 
            multilingual language software products.

            This is the Inso default character table. 
            Some nonessential characters are defined as D_FIL_#.


        Cautions:
            None.

        Creation Date:
            18-jul-89        Created by Inso Corporation

*-----------------------------------------------------------------------*/
#ifndef SLCHRMCS_H

#define SLCHRMCS_H


/* Default character set defines                                        */
/* D_C_...    Default CAPITAL ...                                       */
/* D_L_...    Default LOWER   ...                                       */


#define D_FIL_1            128

#define D_FIL_2            129

#define D_L9_S_QUOTE       130   /* Substitutes for low 9-like single quote*/

#define D_FLORIN           131   /* Substitutes for the Dutch currency     */
                                 /* symbol (florin)                        */

#define D_L99_D_QUOTE      132   /* Substitutes for low 99-like double quote*/

#define D_ELLIPSIS         133   /* Substitutes for ellipsis               */

#define D_FIL_3            134   
#define D_FIL_4            135   
#define D_FIL_5            136   
#define D_FIL_6            137   
#define D_FIL_7            138   

#define D_LT_S_GUILL       139   /* Substitutes for left pointing single   */
                                 /* guillemet                              */
               
#define D_MULTIPLICATION   140   /* Substitutes for 215 in the ISO Latin-1 */
                                 /* Character Set                          */

#define D_FIL_8            141
#define D_FIL_9            142
#define D_FIL_10           143 
#define D_FIL_11           144 

#define D_6_S_QUOTE        145   /* Substitutes for 6-like single quote    */
#define D_9_S_QUOTE        146   /* Substitutes for 9-like single quote    */
/* Redefined for Inso product backward compatibility                       */
#define D_OPEN_S_QUOTE     D_6_S_QUOTE
#define D_CLOSE_S_QUOTE    D_9_S_QUOTE           
                           
#define D_66_D_QUOTE       147   /* Substitutes for 66-like double quote   */
#define D_99_D_QUOTE       148   /* Substitutes for 99-like double quote   */
/* Redefined for Inso product backward compatibility                       */
#define D_OPEN_D_QUOTE     D_66_D_QUOTE
#define D_CLOSE_D_QUOTE    D_99_D_QUOTE            
                           

#define D_FIL_12           149
#define D_EN_DASH          150   /* Substitutes for en dash                */

#define D_M_DASH           151   /* Substitutes for em dash                */

#define D_FIL_13           152

#define D_TRADEMARK        153   /* Substitutes for the trademark symbol   */

#define D_FIL_14           154

#define D_RT_S_GUILL       155   /* Substitutes for right pointing single  */
                                 /* guillemet                              */

#define D_DIVISION         156   /* Substitutes for 247 (division) in the  */
                                 /* ISO Latin-1 Character Set              */

#define D_FIL_15           157
#define D_FIL_16           158

#define D_C_Y_ACUTE        159   /* Substitutes for 221 (Y acute) in the   */
                                 /* ISO Latin-1 Character Set              */

#define D_NBSP             160

#define D_INV_EXCLAMATN    161
#define D_CENT             162
#define D_POUND            163

#define D_UMLAUT           164   /* Substitutes for 168 (umlaut) in the ISO*/
                                 /* Latin-1 Character Set                  */

#define D_YEN              165 

#define D_BROKEN_VBAR      166
#define D_SECTION          167
#define D_GEN_CURRENCY     168   /* Substitutes for 164 (general currency) */
                                 /* in the ISO Latin-1 Character Set       */
#define D_COPYRIGHT        169
#define D_FEM_ORDINAL      170
#define D_LEFT_GUILL       171
/* Redefined for Inso product backward compatibility                       */
#define D_OPEN_GUILL       D_LEFT_GUILL

#define D_NOT_SIGN         172
#define D_SHY              173
#define D_REG_TRADEMARK    174
#define D_MACRON           175
#define D_DEGREE           176                                             
#define D_PLUS_MINUS       177                                             
#define D_TWO_SUPER        178
#define D_THREE_SUPER      179
#define D_ACUTE            180
#define D_MICRO_SIGN       181                                             
#define D_PARAGRAPH        182                                             
#define D_MIDDLE_DOT       183
#define D_CEDILLA          184
#define D_ONE_SUPER        185                                             
#define D_MAS_ORDINAL      186                                             

#define D_RIGHT_GUILL      187
/* Redefined for Inso product backward compatibility                        */
#define D_CLOS_GUILL       D_RIGHT_GUILL 

#define D_ONE_QUARTER      188
#define D_ONE_HALF         189
#define D_THREE_FOURTHS    190
#define D_INV_QUESTION     191                                              
#define D_C_A_GRAVE        192                                              
#define D_C_A_ACUTE        193
#define D_C_A_CIRCUMFLX    194
#define D_C_A_TILDE        195
#define D_C_A_UMLAUT       196
#define D_C_A_ANGSTROM     197
#define D_C_AE_LIGATURE    198
#define D_C_C_CEDILLA      199
#define D_C_E_GRAVE        200
#define D_C_E_ACUTE        201
#define D_C_E_CIRCUMFLX    202
#define D_C_E_UMLAUT       203
#define D_C_I_GRAVE        204
#define D_C_I_ACUTE        205
#define D_C_I_CIRCUMFLX    206
#define D_C_I_UMLAUT       207
#define D_C_EDH            208
#define D_C_N_TILDE        209                                              
#define D_C_O_GRAVE        210                                              
#define D_C_O_ACUTE        211
#define D_C_O_CIRCUMFLX    212
#define D_C_O_TILDE        213
#define D_C_O_UMLAUT       214
#define D_C_OE_LIGATURE    215   /* Substitutes for 215 (OE ligature) in   */
                                 /* the DEC Multinational Character Set    */
#define D_C_O_SLASH        216
#define D_C_U_GRAVE        217
#define D_C_U_ACUTE        218
#define D_C_U_CIRCUMFLX    219
#define D_C_U_UMLAUT       220
#define D_C_Y_UMLAUT       221   /* Substitutes for 221 (Y umlaut) in      */
                                 /* the DEC Multinational Character Set    */
#define D_C_THORN          222
#define D_L_S_SHARP        223                                              
#define D_L_A_GRAVE        224                                              
#define D_L_A_ACUTE        225
#define D_L_A_CIRCUMFLX    226
#define D_L_A_TILDE        227
#define D_L_A_UMLAUT       228
#define D_L_A_ANGSTROM     229
#define D_L_AE_LIGATURE    230
#define D_L_C_CEDILLA      231
#define D_L_E_GRAVE        232
#define D_L_E_ACUTE        233
#define D_L_E_CIRCUMFLX    234
#define D_L_E_UMLAUT       235
#define D_L_I_GRAVE        236
#define D_L_I_ACUTE        237
#define D_L_I_CIRCUMFLX    238
#define D_L_I_UMLAUT       239
#define D_L_EDH            240
#define D_L_N_TILDE        241                                              
#define D_L_O_GRAVE        242                                              
#define D_L_O_ACUTE        243
#define D_L_O_CIRCUMFLX    244
#define D_L_O_TILDE        245
#define D_L_O_UMLAUT       246
#define D_L_OE_LIGATURE    247   /* Substitutes for 247 (oe ligature) in   */
                                 /* the DEC Multinational Character Set    */
#define D_L_O_SLASH        248
#define D_L_U_GRAVE        249
#define D_L_U_ACUTE        250
#define D_L_U_CIRCUMFLX    251
#define D_L_U_UMLAUT       252
#define D_L_Y_UMLAUT       253   /* Substitutes for 255 (y umlaut) in the  */
                                 /* ISO Latin-1 Character Set              */
#define D_L_THORN          254
#define D_L_Y_ACUTE        255   /* Substitutes for 253 (y acute) in the   */
                                 /* ISO Latin-1 Character Set              */
                                                                           
#define D_HIGH_VALUE       255   /* Extra define for end of set            */

#endif /* SLCHRMCS_H */

/* End of file - slchrmcs.h */
