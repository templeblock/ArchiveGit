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
*    SLCH7WIN.H  MS-WINDOWS 3.1 GREEK CHARACTER SET DEFINES:            *
*                CODE PAGE (CP) 1253                                    *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the MS Windows
            Greek character set. It also defines some characters that do 
            not exist in this character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as W7_FIL_#.

         Cautions:
            None.

*-----------------------------------------------------------------------*/
#ifndef SLCH7WIN_H

#define SLCH7WIN_H


/* WINDOWS GREEK CHARACTER SET DEFINES                                   */

/* W7_C_... = Windows CAPITAL ...                                        */
/* W7_L_... = Windows LOWER   ...                                        */


#define W7_ONE_SUPER                    128
#define W7_ELLIPSIS                     129
#define W7_ONE_QUARTER                  130 
#define W7_THREE_FOURTHS                131  
#define W7_FIL_5                        132
#define W7_EN_DASH                      133 
#define W7_M_DASH                       134 
#define W7_6_S_QUOTE                    135 
#define W7_9_S_QUOTE                    136    
#define W7_MULTIPLICATION               137 
#define W7_DIVISION                     138 
#define W7_CENT                         139 
#define W7_YEN                          140 
#define W7_GEN_CURRENCY                 141 

#define W7_MACRON                       142
#define W7_66_D_QUOTE                   143 
#define W7_99_D_QUOTE                   144 
#define W7_LEFT_SINGLE_QUOTATION_MARK   145 
#define W7_RIGHT_SINGLE_QUOTATION_MARK  146    
#define W7_MICRO_SIGN                   147 
#define W7_FIL_19                       148
#define W7_FIL_20                       149 
#define W7_FIL_21                       150 
#define W7_FIL_22                       151 
#define W7_FIL_23                       152 
#define W7_TRADEMARK                    153 
#define W7_FIL_25                       154 
#define W7_FIL_26                       155 
#define W7_FIL_27                       156 
#define W7_FIL_28                       157 
#define W7_FIL_29                       158 
#define W7_FIL_30                       159 

#define W7_NBSP                         160
#define W7_ACUTE_ACCENT_DIAERESIS       161
#define W7_C_ALPHA_ACCENT               162
#define W7_POUND_SIGN                   163
#define W7_FIL_31                       164
#define W7_FIL_32                       165
#define W7_BROKEN_BAR                   166
#define W7_PARAGRAPH_SIGN               167
#define W7_DIAERESIS                    168
#define W7_COPYRIGHT_SIGN               169
#define W7_FIL_33                       170
#define W7_LEFT_ANGLE_QUOTATION_MARK    171
#define W7_NOT_SIGN                     172
#define W7_SOFT_HYPEN                   173  /* Soft hyphen */
#define W7_FIL_34                       174
#define W7_HORIZONTAL_BAR               175

#define W7_DEGREE_SIGN                  176
#define W7_PLUS_OR_MINUS_SIGN           177
#define W7_SUPER_SCRIPT_TWO             178
#define W7_SUPER_SCRIPT_THREE           179
#define W7_ACCENT_TONOS                 180
#define W7_FIL_35                       181
#define W7_FIL_36                       182
#define W7_MIDDLE_DOT                   183
#define W7_C_EPSILON_ACCENT             184
#define W7_C_ETA_ACCENT                 185
#define W7_C_IOTA_ACCENT                186
#define W7_RIGHT_ANGLE_QUOTATION_MARK   187
#define W7_C_OMICRON_ACCENT             188
#define W7_VULGAR_FRACTION_ONE_HALF     189
#define W7_C_UPSILON_ACCENT             190
#define W7_C_OMEGA_ACCENT               191

#define W7_L_IOTA_DIAERESIS_ACCENT      192
#define W7_C_ALPHA                      193
#define W7_C_BETA                       194
#define W7_C_GAMMA                      195
#define W7_C_DELTA                      196
#define W7_C_EPSILON                    197
#define W7_C_ZETA                       198
#define W7_C_ETA                        199
#define W7_C_THETA                      200
#define W7_C_IOTA                       201
#define W7_C_KAPPA                      202
#define W7_C_LAMDA                      203
#define W7_C_MU                         204       
#define W7_C_NU                         205
#define W7_C_XI                         206
#define W7_C_OMICRON                    207

#define W7_C_PI                         208
#define W7_C_RHO                        209
#define W7_FIL_37                       210
#define W7_C_SIGMA                      211
#define W7_C_TAU                        212
#define W7_C_UPSILON                    213
#define W7_C_PHI                        214
#define W7_C_CHI                        215
#define W7_C_PSI                        216
#define W7_C_OMEGA                      217
#define W7_C_IOTA_DIAERESIS             218
#define W7_C_UPSILON_DIAERESIS          219
#define W7_L_ALPHA_ACCENT               220
#define W7_L_EPSILON_ACCENT             221
#define W7_L_ETA_ACCENT                 222
#define W7_L_IOTA_ACCENT                223

#define W7_L_UPSILON_DIAERESIS_ACCENT   224
#define W7_L_ALPHA                      225
#define W7_L_BETA                       226
#define W7_L_GAMMA                      227
#define W7_L_DELTA                      228
#define W7_L_EPSILON                    229
#define W7_L_ZETA                       230
#define W7_L_ETA                        231
#define W7_L_THETA                      232
#define W7_L_IOTA                       233
#define W7_L_KAPPA                      234
#define W7_L_LAMDA                      235
#define W7_L_MU                         236
#define W7_L_NU                         237
#define W7_L_XI                         238
#define W7_L_OMICRON                    239

#define W7_L_PI                         240
#define W7_L_RHO                        241
#define W7_L_TERMINAL_SIGMA             242
#define W7_L_SIGMA                      243
#define W7_L_TAU                        244
#define W7_L_UPSILON                    245
#define W7_L_PHI                        246
#define W7_L_CHI                        247
#define W7_L_PSI                        248
#define W7_L_OMEGA                      249
#define W7_L_IOTA_DIAERESIS             250
#define W7_L_UPSILON_DIAERESIS          251
#define W7_L_OMICRON_ACCENT             252
#define W7_L_UPSILON_ACCENT             253
#define W7_L_OMEGA_ACCENT               254
#define W7_FIL_38                       255

#endif /* SLCH7WIN_H */

/* End of file - slch7win.h */
