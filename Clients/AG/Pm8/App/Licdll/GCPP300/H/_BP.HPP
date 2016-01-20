//*
//* _BP.HPP
//*
//* Contains:
//* 	Additional declarations used for the CompuServe B+ transfers.
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*
//*

#ifndef _BP_HPP
#define _BP_HPP

#define BPLUS_DEFAULT_PACKET_SIZE		1031
#define BPLUS_MASK_LO_RANGE             0x01
#define BPLUS_MASK_HI_RANGE             0x10

/*
 * CompuServe B+ quoting levels and mask
 */
#define BPLUS_QUOTING_LEVEL_0     0       /* No qouting level */
#define BPLUS_QUOTING_LEVEL_1     1
#define BPLUS_QUOTING_LEVEL_2     2
#define BPLUS_QUOTING_LEVEL_3     3
#define BPLUS_QUOTING_MASK        4


typedef enum {
    BPLUS_GET_DLE,
    BPLUS_DLE_SEEN,
    BPLUS_DLE_B_SEEN,
    BPLUS_GET_DATA,
    BPLUS_GET_CHECKSUM,
    BPLUS_GET_CRC_16,
    BPLUS_GET_CRC_32,
    BPLUS_VERIFY_CRC,
    BPLUS_VERIFY_CHECKSUM,
    BPLUS_VERIFY_PACKET,
    BPLUS_SEND_NAK,
    BPLUS_SEND_ACK,
    BPLUS_SEND_ENQ,
    BPLUS_RESEND_PACKETS
} BPLUS_ACTION;


#define NORMAL_MODE      0
#define ESCAPE_SEQ_MODE  1
#define BPLUS_LOW_RANGE  7
#define BPLUS_HIGH_RANGE 11
#define BPLUS_PLUS_PACKET_SIZE  18

#endif	//* #ifdef( _BP_HPP )

