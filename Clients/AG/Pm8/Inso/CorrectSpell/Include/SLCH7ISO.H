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
*    SLCH7ISO.H  Greek Master Character Set defines                     *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the ISO-8859
            Greek Character Set. It also defines some characters that do 
            not exist in this character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as I7_FIL_#.

        Cautions:
            None.

        Creation Date:
            22-NOV-94   RMA 

*-----------------------------------------------------------------------*/
#ifndef SLCH7ISO_H

#define SLCH7ISO_H                             

/* ISO/IEC GREEK CHARACTER SET DEFINES */

/* The following definitions are used in the MCS GREEK character      */
/* set conversions.                                                   */

/* I7_C -- GRE UPPER CASE LETTER                                      */
/* I7_L -- GRE LOWER CASE LETTER                                      */
              
#define I7_ONE_SUPER                           0x80
#define I7_ELLIPSIS                            0x81
#define I7_ONE_QUARTER                         0x82
#define I7_THREE_FOURTHS                       0x83
#define I7_FIL_5                               0x84
#define I7_TRADEMARK                           0x85
#define I7_EN_DASH                             0x86
#define I7_M_DASH                              0x87
#define I7_6_S_QUOTE                           0x88
#define I7_9_S_QUOTE                           0x89
#define I7_66_D_QUOTE                          0x8A
#define I7_99_D_QUOTE                          0x8B
#define I7_MULTIPLICATION                      0x8C
#define I7_DIVISION                            0x8D
#define I7_CENT                                0x8E
#define I7_YEN                                 0x8F

#define I7_GEN_CURRENCY                        0x90
#define I7_MACRON                              0x91
#define I7_MICRO_SIGN                          0x92
#define I7_FIL_20                              0x93
#define I7_FIL_21                              0x94
#define I7_FIL_22                              0x95
#define I7_FIL_23                              0x96
#define I7_FIL_24                              0x97
#define I7_FIL_25                              0x98
#define I7_FIL_26                              0x99
#define I7_FIL_27                              0x9A
#define I7_FIL_28                              0x9B
#define I7_FIL_29                              0x9C
#define I7_FIL_30                              0x9D
#define I7_FIL_31                              0x9E
#define I7_FIL_32                              0x9F
 
#define I7_NBSP                                0xA0
#define I7_LEFT_SINGLE_QUOTATION_MARK          0xA1
#define I7_RIGHT_SINGLE_QUOTATION_MARK         0xA2
#define I7_POUND_SIGN                          0xA3
#define I7_FIL_33                              0xA4
#define I7_FIL_34                              0xA5
#define I7_BROKEN_BAR                          0xA6
#define I7_PARAGRAPH_SIGN                      0xA7
#define I7_DIAERESIS                           0xA8
#define I7_COPYRIGHT_SIGN                      0xA9
#define I7_FIL_35                              0xAA
#define I7_LEFT_ANGLE_QUOTATION_MARK           0xAB
#define I7_NOT_SIGN                            0xAC
#define I7_SOFT_HYPEN                          0xAD
#define I7_FIL_36                              0xAE
#define I7_HORIZONTAL_BAR                      0xAF

#define I7_DEGREE_SIGN                         0xB0
#define I7_PLUS_OR_MINUS_SIGN                  0xB1
#define I7_SUPER_SCRIPT_TWO                    0xB2
#define I7_SUPER_SCRIPT_THREE                  0xB3
#define I7_ACCENT_TONOS                        0xB4
#define I7_DIAERESIS_AND_ACCENT                0xB5
#define I7_C_ALPHA_ACCENT                      0xB6
#define I7_MIDDLE_DOT                          0xB7
#define I7_C_EPSILON_ACCENT                    0xB8
#define I7_C_ETA_ACCENT                        0xB9
#define I7_C_IOTA_ACCENT                       0xBA
#define I7_RIGHT_ANGLE_QUOTATION_MARK          0xBB
#define I7_C_OMICRON_ACCENT                    0xBC
#define I7_VULGAR_FRACTION_ONE_HALF            0xBD
#define I7_C_UPSILON_ACCENT                    0xBE
#define I7_C_OMEGA_ACCENT                      0xBF

#define I7_L_IOTA_ACCENT_DIAERESIS             0xC0
#define I7_C_ALPHA                             0xC1
#define I7_C_BETA                              0xC2
#define I7_C_GAMMA                             0xC3
#define I7_C_DELTA                             0xC4
#define I7_C_EPSILON                           0xC5
#define I7_C_ZETA                              0xC6
#define I7_C_ETA                               0xC7
#define I7_C_THETA                             0xC8
#define I7_C_IOTA                              0xC9
#define I7_C_KAPPA                             0xCA
#define I7_C_LAMDA                             0xCB
#define I7_C_MU                                0xCC
#define I7_C_NU                                0xCD
#define I7_C_XI                                0xCE
#define I7_C_OMICRON                           0xCF

#define I7_C_PI                                0xD0
#define I7_C_RHO                               0xD1
#define I7_FIL_37                              0xD2
#define I7_C_SIGMA                             0xD3
#define I7_C_TAU                               0xD4
#define I7_C_UPSILON                           0xD5
#define I7_C_PHI                               0xD6
#define I7_C_CHI                               0xD7
#define I7_C_PSI                               0xD8
#define I7_C_OMEGA                             0xD9
#define I7_C_IOTA_DIAERESIS                    0xDA
#define I7_C_UPSILON_DIAERESIS                 0xDB
#define I7_L_ALPHA_ACCENT                      0xDC
#define I7_L_EPSILON_ACCENT                    0xDD
#define I7_L_ETA_ACCENT                        0xDE
#define I7_L_IOTA_ACCENT                       0xDF

#define I7_L_UPSILON_DIAERESIS_ACCENT          0xE0
#define I7_L_ALPHA                             0xE1
#define I7_L_BETA                              0xE2
#define I7_L_GAMMA                             0xE3
#define I7_L_DELTA                             0xE4
#define I7_L_EPSILON                           0xE5
#define I7_L_ZETA                              0xE6
#define I7_L_ETA                               0xE7
#define I7_L_THETA                             0xE8
#define I7_L_IOTA                              0xE9
#define I7_L_KAPPA                             0xEA
#define I7_L_LAMDA                             0xEB
#define I7_L_MU                                0xEC
#define I7_L_NU                                0xED
#define I7_L_XI                                0xEE
#define I7_L_OMICRON                           0xEF

#define I7_L_PI                                0xF0
#define I7_L_RHO                               0XF1
#define I7_L_TERMINAL_SIGMA                    0xF2
#define I7_L_SIGMA                             0xF3
#define I7_L_TAU                               0xF4
#define I7_L_UPSILON                           0xF5
#define I7_L_PHI                               0xF6
#define I7_L_CHI                               0xF7
#define I7_L_PSI                               0xF8
#define I7_L_OMEGA                             0xF9
#define I7_L_IOTA_DIAERESIS                    0xFA
#define I7_L_UPSILON_DIAERESIS                 0xFB
#define I7_L_OMICRON_ACCENT                    0xFC
#define I7_L_UPSILON_ACCENT                    0xFD
#define I7_L_OMEGA_ACCENT                      0xFE
#define I7_FIL_38                              0xFF

#endif /* SLCH7ISO_H */

/* END OF FILE - SLCH7ISO.H */
