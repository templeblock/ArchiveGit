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
*        slchrunx.h - UNIX ISO Latin-1 Character Set                    *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:

            This file contains #defines based on the ISO Latin-1
            Character Set and other characters that are not in this
            set but are useful in processing multilingual language software
            products.

            The ISO Latin-1 Character Set is close to The DEC Multinational
            Character Set. Some characters in this ISO character set are
            reserved by DEC. See SLCHRMCS.H for details.

        Cautions:
            None.

        Creation Date:
            28-jan-91        Created by Inso Corporation


*-----------------------------------------------------------------------*/
#ifndef SLCHRUNX_H

#define SLCHRUNX_H


/* UNIX CHARACTER SET DEFINES                                           */

/* U_C_... = UNIX CAPITAL ...                                           */
/* U_L_... = UNIX LOWER   ...                                           */

#define U_FIL_1            128      /* Dummy character                  */
#define U_FIL_2            129      /* Dummy character                  */

#define U_L9_S_QUOTE       130      /* Dummy character substitutes      */
                                    /* for low 9-like single quote      */

#define U_FLORIN           131      /* Dummy character substitutes for   */
                                    /* the Dutch currency symbol (florin)*/

#define U_L99_D_QUOTE      132      /* Dummy character substitutes      */
                                    /* low 99-like double quote         */

#define U_ELLIPSIS         133      /* Dummy character substitutes      */
                                    /* for ellipses                     */

#define U_FIL_3            134      /* Dummy character                  */
#define U_FIL_4            135      /* Dummy character                  */
#define U_FIL_5            136      /* Dummy character                  */
#define U_FIL_6            137      /* Dummy character                  */
#define U_FIL_7            138      /* Dummy character                  */

#define U_LT_S_GUILL       139      /* Dummy character substitutes for  */
                                    /* left pointing single guillemet   */

#define U_C_OE_LIGATURE    140      /* Dummy character substitutes      */
                                    /* for OE-ligature                  */

#define U_FIL_8            141      /* Dummy character                  */
#define U_FIL_9            142      /* Dummy character                  */
#define U_FIL_10           143      /* Dummy character                  */
#define U_FIL_11           144      /* Dummy character                  */

#define U_6_S_QUOTE        145      /* Dummy character substitutes      */
                                    /* for 6-like single quote          */
#define U_9_S_QUOTE        146      /* Dummy character substitutes      */
                                    /* for 9-like single quote          */
/* Redefined for Inso product backward compatibility                    */
#define U_OPEN_S_QUOTE     U_6_S_QUOTE
#define U_CLOSE_S_QUOTE    U_9_S_QUOTE           
                           
#define U_66_D_QUOTE       147      /* Dummy character substitutes      */
                                    /* for 66-like double quote         */
#define U_99_D_QUOTE       148      /* Dummy character substitutes      */
                                    /* for 99-like double quote         */
/* Redefined for Inso product backward compatibility                    */
#define U_OPEN_D_QUOTE     U_66_D_QUOTE
#define U_CLOSE_D_QUOTE    U_99_D_QUOTE            
                           

#define U_FIL_12           149      /* Dummy character substitutes      */

#define U_EN_DASH          150      /* Dummy character substitutes      */
                                    /* for en dash                      */

#define U_M_DASH           151      /* Dummy character substitutes      */
                                    /* for em dash                      */

#define U_FIL_13           152      /* Dummy character substitutes for  */

#define U_TRADEMARK        153      /* Dummy character; substitute for  */   
                                    /* the trademark symbol             */

#define U_FIL_14           154      /* Dummy character                  */

#define U_RT_S_GUILL       155      /* Dummy character substitutes for  */
                                    /* right pointing single guillemet  */

#define U_L_OE_LIGATURE    156      /* Dummy character                  */
                                    /* for oe-ligature                  */

#define U_FIL_15           157      /* Dummy character                  */
#define U_FIL_16           158      /* Dummy character                  */

#define U_C_Y_UMLAUT       159      /* Dummy character                  */
                                    /* for Y umlaut                     */

#define U_NBSP             160      
#define U_INV_EXCLAMATN    161      
#define U_CENT             162      
#define U_POUND            163      
#define U_GEN_CURRENCY     164                                         
#define U_YEN              165      
#define U_BROKEN_VBAR      166      
#define U_SECTION          167
#define U_UMLAUT           168
#define U_COPYRIGHT        169
#define U_FEM_ORDINAL      170

#define U_LEFT_GUILL       171
/* Redefined for Inso product backward compatibility                        */
#define U_OPEN_GUILL       U_LEFT_GUILL

#define U_NOT_SIGN         172
#define U_SHY              173
#define U_REG_TRADEMARK    174
#define U_MACRON           175
#define U_DEGREE           176
#define U_PLUS_MINUS       177
#define U_TWO_SUPER        178
#define U_THREE_SUPER      179
#define U_ACUTE            180
#define U_MICRO_SIGN       181
#define U_PARAGRAPH        182
#define U_MIDDLE_DOT       183
#define U_CEDILLA          184
#define U_ONE_SUPER        185
#define U_MAS_ORDINAL      186

#define U_RIGHT_GUILL      187
/* Redefined for Inso product backward compatibility                        */
#define U_CLOS_GUILL       U_RIGHT_GUILL 

#define U_ONE_QUARTER      188
#define U_ONE_HALF         189
#define U_THREE_FOURTHS    190
#define U_INV_QUESTION     191
#define U_C_A_GRAVE        192
#define U_C_A_ACUTE        193
#define U_C_A_CIRCUMFLX    194
#define U_C_A_TILDE        195
#define U_C_A_UMLAUT       196
#define U_C_A_ANGSTROM     197
#define U_C_AE_LIGATURE    198
#define U_C_C_CEDILLA      199
#define U_C_E_GRAVE        200
#define U_C_E_ACUTE        201
#define U_C_E_CIRCUMFLX    202
#define U_C_E_UMLAUT       203
#define U_C_I_GRAVE        204       
#define U_C_I_ACUTE        205
#define U_C_I_CIRCUMFLX    206
#define U_C_I_UMLAUT       207
#define U_C_EDH            208
#define U_C_N_TILDE        209
#define U_C_O_GRAVE        210
#define U_C_O_ACUTE        211
#define U_C_O_CIRCUMFLX    212
#define U_C_O_TILDE        213
#define U_C_O_UMLAUT       214
#define U_MULTIPLICATION   215
#define U_C_O_SLASH        216
#define U_C_U_GRAVE        217
#define U_C_U_ACUTE        218
#define U_C_U_CIRCUMFLX    219
#define U_C_U_UMLAUT       220
#define U_C_Y_ACUTE        221
#define U_C_THORN          222
#define U_L_S_SHARP        223      /* sharp s                             */
#define U_L_A_GRAVE        224
#define U_L_A_ACUTE        225
#define U_L_A_CIRCUMFLX    226
#define U_L_A_TILDE        227
#define U_L_A_UMLAUT       228      /* a diaeresis                         */
#define U_L_A_ANGSTROM     229      /* a ring                              */
#define U_L_AE_LIGATURE    230
#define U_L_C_CEDILLA      231
#define U_L_E_GRAVE        232
#define U_L_E_ACUTE        233
#define U_L_E_CIRCUMFLX    234
#define U_L_E_UMLAUT       235
#define U_L_I_GRAVE        236
#define U_L_I_ACUTE        237
#define U_L_I_CIRCUMFLX    238
#define U_L_I_UMLAUT       239
#define U_L_EDH            240
#define U_L_N_TILDE        241
#define U_L_O_GRAVE        242
#define U_L_O_ACUTE        243
#define U_L_O_CIRCUMFLX    244
#define U_L_O_TILDE        245
#define U_L_O_UMLAUT       246
#define U_DIVISION         247
#define U_L_O_SLASH        248
#define U_L_U_GRAVE        249
#define U_L_U_ACUTE        250
#define U_L_U_CIRCUMFLX    251
#define U_L_U_UMLAUT       252
#define U_L_Y_ACUTE        253
#define U_L_THORN          254
#define U_L_Y_UMLAUT       255


#endif /* SLCHRUNX_H */

/* End of file - slchrunx.h */
