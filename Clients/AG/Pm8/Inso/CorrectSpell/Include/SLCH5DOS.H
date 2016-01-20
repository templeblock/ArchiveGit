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
*        slch5dos.h - IBM PC Cyrillic character set defines.            *
*                     (code page 866)                                   *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the IBM PC
            Cyrillic character set. It also defines some characters
            that do not exist in this character set but are essential for
            the multilingual language software products.

            Some nonessential characters are defined as P5_FIL_#, or, like 
            most of the DOS graphics characters, remapped onto more 
            useful characters.

        Cautions:
            None.

        Creation Date:
            9-dec-92        


*-----------------------------------------------------------------------*/
#ifndef SLCH5DOS_H

#define SLCH5DOS_H


/* IBM PC CHARACTER SET DEFINES                                         */

/* P5_C_... = PC CYRILLIC CAPITAL ...                                   */
/* P5_L_... = PC CYRILLIC LOWER ...                                     */

/* The following definitions are used in MCS Latin5/Cyrillic character  */
/* set conversions.                                                     */

#define P5_C_A             128
#define P5_C_BE            129
#define P5_C_VE            130
#define P5_C_GE            131
#define P5_C_DE            132
#define P5_C_IE            133
#define P5_C_ZHE           134
#define P5_C_ZE            135
#define P5_C_II            136
#define P5_C_SHORT_II      137
#define P5_C_KA            138
#define P5_C_EL            139
#define P5_C_EM            140
#define P5_C_EN            141
#define P5_C_O             142
#define P5_C_PE            143

#define P5_C_ER            144
#define P5_C_ES            145
#define P5_C_TE            146
#define P5_C_U             147
#define P5_C_EF            148
#define P5_C_KHA           149
#define P5_C_TSE           150
#define P5_C_CHE           151
#define P5_C_SHA           152
#define P5_C_SHCHA         153
#define P5_C_HARD_SIGN     154
#define P5_C_YERI          155
#define P5_C_SOFT_SIGN     156
#define P5_C_REVERSED_E    157
#define P5_C_IU            158
#define P5_C_IA            159

#define P5_L_A             160
#define P5_L_BE            161
#define P5_L_VE            162
#define P5_L_GE            163
#define P5_L_DE            164
#define P5_L_IE            165
#define P5_L_ZHE           166
#define P5_L_ZE            167
#define P5_L_II            168
#define P5_L_SHORT_II      169
#define P5_L_KA            170
#define P5_L_EL            171
#define P5_L_EM            172
#define P5_L_EN            173
#define P5_L_O             174
#define P5_L_PE            175

#define P5_C_DJE           176
#define P5_C_GJE           177
#define P5_L9_S_QUOTE      178
#define P5_L_GJE           179
#define P5_L99_D_QUOTE     180
#define P5_ELLIPSIS        181
#define P5_DAGGER          182
#define P5_D_DAGGER        183
#define P5_PLUS_MINUS      184
#define P5_PER_MILLE       185
#define P5_C_LJE           186
#define P5_LT_S_GUILL      187  /* Graphic character substitutes for    */
                                /* left pointing single guillemet       */
#define P5_C_NJE           188
#define P5_C_KJE           189
#define P5_C_TSHE          190
#define P5_C_DZHE          191

#define P5_L_DJE           192        
#define P5_6_S_QUOTE       193        
#define P5_9_S_QUOTE       194        
#define P5_66_D_QUOTE      195        
#define P5_99_D_QUOTE      196        
#define P5_C_DZE           197        
#define P5_N_DASH          198        
#define P5_M_DASH          199        
#define P5_FIL_1           200        
#define P5_TRADEMARK       201        
#define P5_L_LJE           202        
#define P5_RT_S_GUILL      203  /* Graphic character substitutes for    */
                                /* right pointing single guillemet      */
#define P5_L_NJE           204        
#define P5_L_KJE           205        
#define P5_L_TSHE          206        
#define P5_L_DZHE          207        
#define P5_RIGHT_GUILL     208      
#define P5_C_I             209      
#define P5_L_I             210         
#define P5_C_JE            211         
#define P5_FIL_2           212      
#define P5_C_GE_UPTURN     213       
#define P5_BROKEN_VBAR     214      
#define P5_SECTION         215      
#define P5_L_GE_UPTURN     216      
#define P5_COPYRIGHT       217            
#define P5_MICRO           218      
#define P5_LEFT_GUILL      219      
#define P5_NOT_SIGN        220       
#define P5_SHY             221       
#define P5_REG_TRADEMARK   222       
#define P5_PARAGRAPH       223      

#define P5_L_ER            224
#define P5_L_ES            225
#define P5_L_TE            226
#define P5_L_U             227
#define P5_L_EF            228
#define P5_L_KHA           229
#define P5_L_TSE           230
#define P5_L_CHE           231
#define P5_L_SHA           232
#define P5_L_SHCHA         233
#define P5_L_HARD_SIGN     234
#define P5_L_YERI          235
#define P5_L_SOFT_SIGN     236
#define P5_L_REVERSED_E    237
#define P5_L_IU            238
#define P5_L_IA            239

#define P5_C_IO            240
#define P5_L_IO            241
#define P5_C_E             242
#define P5_L_E             243
#define P5_C_YI            244
#define P5_L_YI            245
#define P5_C_SHORT_U       246
#define P5_L_SHORT_U       247
#define P5_DEGREE          248
#define P5_BULLET          249
#define P5_MIDDLE_DOT      250
#define P5_L_JE            251
#define P5_NUMERO          252
#define P5_GEN_CURRENCY    253
#define P5_L_DZE           254
#define P5_NBSP            255

 
#endif /* SLCH5DOS_H */

/* End of file - slch5dos.h */
