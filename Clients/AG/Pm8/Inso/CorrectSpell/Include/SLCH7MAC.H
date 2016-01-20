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
*    SLCH7MAC.H  MACINTOSH GREEK CHARACTER SET DEFINES                  *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the 
            MACINTOSH GREEK CHARACTER SET 100069.
            It also defines some characters that do not exist in this
            character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as MC_FIL_#.

        Cautions:
            None.

        Creation Date:
            22-NOV-94   RMA 

*-----------------------------------------------------------------------*/
#ifndef SLCH7MAC_H

#define SLCH7MAC_H                             


/* GREEK MACINTOSH CHARACTER SET DEFINES                               */
/* The following definitions are used in the MCS GREEK character       */
/* set conversions for Macintosh Platform.                             */
/* M7_C -- GREEK UPPER CASE LETTER                                     */
/* M7_L -- GREEK LOWER CASE LETTER                                     */



#define M7_NBSP                                128
#define M7_ONE_SUPER                           129
#define M7_ELLIPSIS                            130
#define M7_ONE_QUARTER                         131
#define M7_THREE_FOURTHS                       132
#define M7_FIL_5                               133
#define M7_FIL_6                               134
#define M7_EN_DASH                             135
#define M7_M_DASH                              136
#define M7_DIVISION                            137
#define M7_6_S_QUOTE                           138
#define M7_9_S_QUOTE                           139
#define M7_66_D_QUOTE                          140
#define M7_99_D_QUOTE                          141
#define M7_MULTIPLICATION                      142
#define M7_CENT                                143
#define M7_YEN                                 144
#define M7_GEN_CURRENCY                        145

#define M7_MACRON                              146
#define M7_MICRO_SIGN                          147
#define M7_FIL_20                              148
#define M7_FIL_21                              149
#define M7_FIL_22                              150
#define M7_FIL_23                              151
#define M7_FIL_24                              152
#define M7_FIL_25                              153
#define M7_FIL_26                              154
#define M7_FIL_27                              155
#define M7_FIL_28                              156
#define M7_FIL_29                              157
#define M7_FIL_30                              158
#define M7_FIL_31                              159


#define M7_FIL_32                              0xA0
#define M7_LEFT_SINGLE_QUOTATION_MARK          0xA1
#define M7_RIGHT_SINGLE_QUOTATION_MARK         0xA2
#define M7_POUND_SIGN                          0xA3
#define M7_FIL_33                              0xA4
#define M7_FIL_34                              0xA5
#define M7_BROKEN_BAR                          0xA6
#define M7_PARAGRAPH_SIGN                      0xA7
#define M7_DIAERESIS                           0xA8
#define M7_COPYRIGHT_SIGN                      0xA9
#define M7_TRADEMARK                           0xAA
#define M7_LEFT_ANGLE_QUOTATION_MARK           0xAB
#define M7_NOT_SIGN                            0xAC
#define M7_SOFT_HYPEN                          0xAD
#define M7_FIL_36                              0xAE
#define M7_HORIZONTAL_BAR                      0xAF

#define M7_DEGREE_SIGN                         0xB0
#define M7_PLUS_OR_MINUS_SIGN                  0xB1
#define M7_SUPER_SCRIPT_TWO                    0xB2
#define M7_SUPER_SCRIPT_THREE                  0xB3
#define M7_ACCENT_TONOS                        0xB4
#define M7_DIAERESIS_AND_ACCENT                0xB5
#define M7_C_ALPHA_ACCENT                      0xB6
#define M7_MIDDLE_DOT                          0xB7
#define M7_C_EPSILON_ACCENT                    0xB8
#define M7_C_ETA_ACCENT                        0xB9
#define M7_C_IOTA_ACCENT                       0xBA
#define M7_RIGHT_ANGLE_QUOTATION_MARK          0xBB
#define M7_C_OMICRON_ACCENT                    0xBC
#define M7_VULGAR_FRACTION_ONE_HALF            0xBD
#define M7_C_UPSILON_ACCENT                    0xBE
#define M7_C_OMEGA_ACCENT                      0xBF

#define M7_L_IOTA_DIAERESIS_ACCENT             0xC0
#define M7_C_ALPHA                             0xC1
#define M7_C_BETA                              0xC2
#define M7_C_GAMMA                             0xC3
#define M7_C_DELTA                             0xC4
#define M7_C_EPSILON                           0xC5
#define M7_C_ZETA                              0xC6
#define M7_C_ETA                               0xC7
#define M7_C_THETA                             0xC8
#define M7_C_IOTA                              0xC9
#define M7_C_KAPPA                             0xCA
#define M7_C_LAMDA                             0xCB
#define M7_C_MU                                0xCC
#define M7_C_NU                                0xCD
#define M7_C_XI                                0xCE
#define M7_C_OMICRON                           0xCF

#define M7_C_PI                                0xD0
#define M7_C_RHO                               0xD1
#define M7_FIL_37                              0xD2
#define M7_C_SIGMA                             0xD3
#define M7_C_TAU                               0xD4
#define M7_C_UPSILON                           0xD5
#define M7_C_PHI                               0xD6
#define M7_C_CHI                               0xD7
#define M7_C_PSI                               0xD8
#define M7_C_OMEGA                             0xD9
#define M7_C_IOTA_DIAERESIS                    0xDA
#define M7_C_UPSILON_DIAERESIS                 0xDB
#define M7_L_ALPHA_ACCENT                      0xDC
#define M7_L_EPSILON_ACCENT                    0xDD
#define M7_L_ETA_ACCENT                        0xDE
#define M7_L_IOTA_ACCENT                       0xDF

#define M7_L_UPSILON_DIAERESIS_ACCENT          0xE0
#define M7_L_ALPHA                             0xE1
#define M7_L_BETA                              0xE2
#define M7_L_GAMMA                             0xE3
#define M7_L_DELTA                             0xE4
#define M7_L_EPSILON                           0xE5
#define M7_L_ZETA                              0xE6
#define M7_L_ETA                               0xE7
#define M7_L_THETA                             0xE8
#define M7_L_IOTA                              0xE9
#define M7_L_KAPPA                             0xEA
#define M7_L_LAMDA                             0xEB
#define M7_L_MU                                0xEC
#define M7_L_NU                                0xED
#define M7_L_XI                                0xEE
#define M7_L_OMICRON                           0xEF

#define M7_L_PI                                0xF0
#define M7_L_RHO                               0xF1
#define M7_L_TERMINAL_SIGMA                    0xF2
#define M7_L_SIGMA                             0xF3
#define M7_L_TAU                               0xF4
#define M7_L_UPSILON                           0xF5
#define M7_L_PHI                               0xF6
#define M7_L_CHI                               0xF7
#define M7_L_PSI                               0xF8
#define M7_L_OMEGA                             0xF9
#define M7_L_IOTA_DIAERESIS                    0xFA
#define M7_L_UPSILON_DIAERESIS                 0xFB
#define M7_L_OMICRON_ACCENT                    0xFC
#define M7_L_UPSILON_ACCENT                    0xFD
#define M7_L_OMEGA_ACCENT                      0xFE
#define M7_FIL_38                              0xFF


#endif /* SLCH7MAC_H */

/* END OF FILE - SLCH7MAC.H */
