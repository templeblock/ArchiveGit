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
*    SLCH7MCS.H  Greek Master character set defines                     *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines that are based upon the ISO-8859
            Greek Character Set. It also defines some characters that do 
            not exist in this character set but are essential for the 
            multilingual language software products.

            Some nonessential characters are defined as D7_FIL_#.

        Cautions:
            None.

*-----------------------------------------------------------------------*/
#ifndef SLCH7MCS_H

#define SLCH7MCS_H                             


/* DEFAULT GREEK CHARACTER SET DEFINES                                  */
/* The following definitions are used in Inso GREEK character       */
/* set conversions.                                                     */

/* D7_C -- GREEK UPPER CASE LETTER                                      */
/* D7_L -- GREEK LOWER CASE LETTER                                      */


#define D7_NBSP                                0x80    /* 128 */
#define D7_LEFT_SINGLE_QUOTATION_MARK          0x81    /* 129 */
#define D7_RIGHT_SINGLE_QUOTATION_MARK         0x82    /* 130 */
#define D7_POUND_SIGN                          0x83    /* 131 */
#define D7_ONE_SUPER                           0x84    /* 132 */
#define D7_ELLIPSIS                            0x85    /* 133 */
#define D7_BROKEN_BAR                          0x86    /* 134 */
#define D7_PARAGRAPH_SIGN                      0x87    /* 135 */
#define D7_DIAERESIS                           0x88    /* 136 */
#define D7_COPYRIGHT_SIGN                      0x89    /* 137 */
#define D7_ONE_QUARTER                         0x8A    /* 138 */
#define D7_LEFT_ANGLE_QUOTATION_MARK           0x8B    /* 139 */
#define D7_RIGHT_ANGLE_QUOTATION_MARK          0x8C    /* 140 */
#define D7_NOT_SIGN                            0x8D    /* 141 */
#define D7_SOFT_HYPEN                          0x8E    /* 142 */
#define D7_THREE_FOURTHS                       0x8F    /* 143 */
                                                       
#define D7_HORIZONTAL_BAR                      0x90    /* 144 */
#define D7_DEGREE_SIGN                         0x91    /* 145 */
#define D7_PLUS_OR_MINUS_SIGN                  0x92    /* 146 */
#define D7_SUPER_SCRIPT_TWO                    0x93    /* 147 */
#define D7_SUPER_SCRIPT_THREE                  0x94    /* 148 */
#define D7_VULGAR_FRACTION_ONE_HALF            0x95    /* 149 */
#define D7_ACCENT_TONOS                        0x96    /* 150 */
#define D7_DIAERESIS_AND_ACCENT                0x97    /* 151 */
#define D7_MIDDLE_DOT                          0x98    /* 152 */
#define D7_C_ALPHA                             0x99    /* 153 */
#define D7_C_ALPHA_ACCENT                      0x9A    /* 154 */
#define D7_C_BETA                              0x9B    /* 155 */
#define D7_C_GAMMA                             0x9C    /* 156 */
#define D7_C_DELTA                             0x9D    /* 157 */
#define D7_C_EPSILON                           0x9E    /* 158 */
#define D7_C_EPSILON_ACCENT                    0x9F    /* 159 */
                                                      
#define D7_C_ZETA                              0xA0    /* 160 */
#define D7_C_ETA                               0xA1    /* 161 */
#define D7_C_ETA_ACCENT                        0xA2    /* 162 */
#define D7_C_THETA                             0xA3    /* 163 */
#define D7_C_IOTA                              0xA4    /* 164 */
#define D7_C_IOTA_ACCENT                       0xA5    /* 165 */
#define D7_C_KAPPA                             0xA6    /* 166 */
#define D7_C_LAMDA                             0xA7    /* 167 */
#define D7_C_MU                                0xA8    /* 168 */
#define D7_C_NU                                0xA9    /* 169 */
#define D7_C_XI                                0xAA    /* 170 */
#define D7_C_OMICRON                           0xAB    /* 171 */
#define D7_C_OMICRON_ACCENT                    0xAC    /* 172 */
#define D7_C_PI                                0xAD    /* 173 */
#define D7_C_RHO                               0xAE    /* 174 */
#define D7_C_SIGMA                             0xAF    /* 175 */
                                                       
#define D7_C_TAU                               0xB0    /* 176 */
#define D7_C_UPSILON                           0xB1    /* 177 */
#define D7_C_UPSILON_ACCENT                    0xB2    /* 178 */
#define D7_C_PHI                               0xB3    /* 179 */
#define D7_C_CHI                               0xB4    /* 180 */
#define D7_C_PSI                               0xB5    /* 181 */
#define D7_C_OMEGA                             0xB6    /* 182 */
#define D7_C_OMEGA_ACCENT                      0xB7    /* 183 */
#define D7_FIL_5                               0xB8    /* 184 */
#define D7_L_ALPHA                             0xB9    /* 185 */
#define D7_L_ALPHA_ACCENT                      0xBA    /* 186 */
#define D7_L_BETA                              0xBB    /* 187 */
#define D7_L_GAMMA                             0xBC    /* 188 */
#define D7_L_DELTA                             0xBD    /* 189 */
#define D7_L_EPSILON                           0xBE    /* 190 */
#define D7_L_EPSILON_ACCENT                    0xBF    /* 191 */
                                                       
#define D7_L_ZETA                              0xC0    /* 192 */
#define D7_L_ETA                               0xC1    /* 193 */
#define D7_L_ETA_ACCENT                        0xC2    /* 194 */
#define D7_L_THETA                             0xC3    /* 195 */
#define D7_L_IOTA                              0xC4    /* 196 */
#define D7_L_IOTA_ACCENT                       0xC5    /* 197 */
#define D7_L_KAPPA                             0xC6    /* 198 */
#define D7_L_LAMDA                             0xC7    /* 199 */
#define D7_L_MU                                0xC8    /* 200 */
#define D7_L_NU                                0xC9    /* 201 */
#define D7_L_XI                                0xCA    /* 202 */
#define D7_L_OMICRON                           0xCB    /* 203 */
#define D7_L_OMICRON_ACCENT                    0xCC    /* 204 */
#define D7_L_PI                                0xCD    /* 205 */
#define D7_L_RHO                               0xCE    /* 206 */
#define D7_L_SIGMA                             0xCF    /* 207 */
                                                       
#define D7_L_TAU                               0xD0    /* 208 */
#define D7_L_UPSILON                           0xD1    /* 209 */
#define D7_L_UPSILON_ACCENT                    0xD2    /* 210 */
#define D7_L_PHI                               0xD3    /* 211 */
#define D7_L_CHI                               0xD4    /* 212 */
#define D7_L_PSI                               0xD5    /* 213 */
#define D7_L_OMEGA                             0xD6    /* 214 */
#define D7_L_OMEGA_ACCENT                      0xD7    /* 215 */
#define D7_TRADEMARK                           0xD8    /* 216 */
#define D7_C_IOTA_DIAERESIS                    0xD9    /* 217 */
#define D7_EN_DASH                             0xDA    /* 218 */
#define D7_C_UPSILON_DIAERESIS                 0xDB    /* 219 */
#define D7_M_DASH                              0xDC    /* 220 */
#define D7_6_S_QUOTE                           0xDD    /* 221 */
#define D7_9_S_QUOTE                           0xDE    /* 222 */
#define D7_66_D_QUOTE                          0xDF    /* 223 */
                                                      
#define D7_99_D_QUOTE                          0xE0    /* 224 */
#define D7_MULTIPLICATION                      0xE1    /* 225 */
#define D7_DIVISION                            0xE2    /* 226 */
#define D7_CENT                                0xE3    /* 227 */
#define D7_YEN                                 0xE4    /* 228 */
#define D7_GEN_CURRENCY                        0xE5    /* 229 */
#define D7_MACRON                              0xE6    /* 230 */
#define D7_MICRO_SIGN                          0xE7    /* 231 */
#define D7_FIL_20                              0xE8    /* 232 */
#define D7_FIL_21                              0xE9    /* 233 */
#define D7_FIL_22                              0xEA    /* 234 */
#define D7_FIL_23                              0xEB    /* 235 */
#define D7_FIL_24                              0xEC    /* 236 */
#define D7_FIL_25                              0xED    /* 237 */
#define D7_FIL_26                              0xEE    /* 238 */
#define D7_FIL_27                              0xEF    /* 239 */
                                                       
#define D7_FIL_28                              0xF0    /* 240 */
#define D7_FIL_29                              0xF1    /* 241 */
#define D7_FIL_30                              0xF2    /* 242 */
#define D7_FIL_31                              0xF3    /* 243 */
#define D7_FIL_32                              0xF4    /* 244 */
#define D7_FIL_33                              0xF5    /* 245 */
#define D7_FIL_34                              0xF6    /* 246 */
#define D7_FIL_35                              0xF7    /* 247 */
#define D7_FIL_36                              0xF8    /* 248 */
#define D7_L_IOTA_DIAERESIS                    0xF9    /* 249 */
#define D7_L_IOTA_ACCENT_DIAERESIS             0xFA    /* 250 */
#define D7_L_UPSILON_DIAERESIS                 0xFB    /* 251 */
#define D7_L_UPSILON_DIAERESIS_ACCENT          0xFC    /* 252 */
#define D7_L_TERMINAL_SIGMA                    0xFD    /* 253 */
#define D7_FIL_37                              0xFE    /* 254 */
#define D7_FIL_38                              0xFF    /* 255 */


#endif /* SLCH7MCS_H */

/* END OF FILE - SLCH7MCS.H */
