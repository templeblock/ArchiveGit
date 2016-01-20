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
*        SLCHR869.H - IBM PC Extended ASCII GREEK Character Set         *
*                     defines (code page 869)                           *
*                                                                       *
*-----------------------------------------------------------------------*

    Description:

    This file contains #defines that name characters from code page
    869, the Greek code page of character codes in the IBM PC Extended
    ASCII character sets.  It also defines some characters that do not
    exist in CP869 character set but are essential for the multilingual
    language software products.
    
    Some nonessential characters (graphics characters that aren't recycled
    to represent multilingual chars) are defined as P7_FIL_#.

*-----------------------------------------------------------------------*/

#ifndef SLCHR869_H
#define SLCHR869_H


/* IBM PC CHARACTER SET DEFINES                                          */

/* P7_C_... = PC CAPITAL ...                                             */
/* P7_L_... = PC LOWER   ...                                             */

#define P7_FIL_1                          128
#define P7_FIL_2                          129
#define P7_FIL_3                          130
#define P7_FIL_4                          131
#define P7_FIL_5                          132
#define P7_FIL_6                          133
#define P7_C_ALPHA_ACCENT                 134
#define P7_FIL_7                          135
#define P7_MIDDLE_DOT                     136
#define P7_NOT_SIGN                       137
#define P7_BROKEN_BAR                     138
#define P7_LEFT_SINGLE_QUOTATION_MARK     139
#define P7_RIGHT_SINGLE_QUOTATION_MARK    140
#define P7_C_EPSILON_ACCENT               141
#define P7_HORIZONTAL_BAR                 142
#define P7_C_ETA_ACCENT                   143

#define P7_C_IOTA_ACCENT                  144
#define P7_C_IOTA_DIAERESIS               145
#define P7_C_OMICRON_ACCENT               146
#define P7_FIL_8                          147 
#define P7_FIL_9                          148 
#define P7_C_UPSILON_ACCENT               149
#define P7_C_UPSILON_DIAERESIS            150
#define P7_COPYRIGHT_SIGN                 151
#define P7_C_OMEGA_ACCENT                 152
#define P7_SUPER_SCRIPT_TWO               153
#define P7_SUPER_SCRIPT_THREE             154
#define P7_L_ALPHA_ACCENT                 155
#define P7_POUND_SIGN                     156
#define P7_L_EPSILON_ACCENT               157
#define P7_L_ETA_ACCENT                   158
#define P7_L_IOTA_ACCENT                  159 

#define P7_L_IOTA_DIAERESIS               160
#define P7_L_IOTA_DIAERESIS_ACCENT        161
#define P7_L_OMICRON_ACCENT               162
#define P7_L_UPSILON_ACCENT               163
#define P7_C_ALPHA                        164
#define P7_C_BETA                         165
#define P7_C_GAMMA                        166
#define P7_C_DELTA                        167
#define P7_C_EPSILON                      168
#define P7_C_ZETA                         169
#define P7_C_ETA                          170
#define P7_VULGAR_FRACTION_ONE_HALF       171
#define P7_C_THETA                        172
#define P7_C_IOTA                         173
#define P7_LEFT_ANGLE_QUOTATION_MARK      174
#define P7_RIGHT_ANGLE_QUOTATION_MARK     175

#define P7_FIL_10                         176
#define P7_FIL_11                         177
#define P7_FIL_12                         178 
#define P7_FIL_13                         179 
#define P7_FIL_14                         180  
#define P7_C_KAPPA                        181
#define P7_C_LAMDA                        182
#define P7_C_MU                           183
#define P7_C_NU                           184
#define P7_FIL_15                         185 
#define P7_FIL_16                         186 
#define P7_FIL_17                         187 
#define P7_FIL_18                         188  
#define P7_C_XI                           189 
#define P7_C_OMICRON                      190
#define P7_FIL_19                         191 
              
#define P7_FIL_20                         192 
#define P7_FIL_21                         193                                                        
#define P7_FIL_22                         194 
#define P7_FIL_23                         195
#define P7_FIL_24                         196 
#define P7_FIL_25                         197 
#define P7_C_PI                           198
#define P7_C_RHO                          199
#define P7_FIL_26                         200                      
#define P7_FIL_27                         201 
#define P7_FIL_28                         202 
#define P7_FIL_29                         203 
#define P7_FIL_30                         204 
#define P7_FIL_31                         205 
#define P7_FIL_32                         206 
#define P7_C_SIGMA                        207

#define P7_C_TAU                          208     
#define P7_C_UPSILON                      209
#define P7_C_PHI                          210
#define P7_C_CHI                          211
#define P7_C_PSI                          212
#define P7_C_OMEGA                        213
#define P7_L_ALPHA                        214
#define P7_L_BETA                         215
#define P7_L_GAMMA                        216
#define P7_FIL_33                         217 
#define P7_FIL_34                         218 
#define P7_FIL_35                         219 
#define P7_FIL_36                         220 
#define P7_L_DELTA                        221
#define P7_L_EPSILON                      222
#define P7_FIL_37                         223 
                                
#define P7_L_ZETA                         224
#define P7_L_ETA                          225
#define P7_L_THETA                        226
#define P7_L_IOTA                         227
#define P7_L_KAPPA                        228
#define P7_L_LAMDA                        229
#define P7_L_MU                           230
#define P7_L_NU                           231
#define P7_L_XI                           232
#define P7_L_OMICRON                      233
#define P7_L_PI                           234
#define P7_L_RHO                          235
#define P7_L_SIGMA                        236
#define P7_L_TERMINAL_SIGMA               237
#define P7_L_TAU                          238 
#define P7_ACCENT_TONOS                   239 

#define P7_SOFT_HYPEN                     240 
#define P7_PLUS_OR_MINUS_SIGN             241
#define P7_L_UPSILON                      242 
#define P7_L_PHI                          243
#define P7_L_CHI                          244
#define P7_PARAGRAPH_SIGN                 245
#define P7_L_PSI                          246 
#define P7_DIAERESIS_AND_ACCENT           247 
#define P7_DEGREE_SIGN                    248
#define P7_DIAERESIS                      249 
#define P7_L_OMEGA                        250
#define P7_L_UPSILON_DIAERESIS            251
#define P7_L_UPSILON_DIAERESIS_ACCENT     252
#define P7_L_OMEGA_ACCENT                 253
#define P7_FIL_38                         254 
#define P7_NBSP                           255 

#endif   

/* End of file - SLCHR869.H */
