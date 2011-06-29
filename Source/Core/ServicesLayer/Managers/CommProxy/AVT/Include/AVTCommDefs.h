//******************************************************************************
// Description:
//    Constants and data type definitions for AVT box communication
//
//==============================================================================
//
// Copyright © 1999 Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/AVT/Include/AVTCommDefs.h $
// 
// 3     7/20/05 9:08p Cward
// support for refactored message object setup
//
// 2     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     1/17/05 10:04a Bmeinke
// Added AvtCanErrorBits_t enum to define the bit definitions for CAN
// errors
//
// 1     12/21/03 6:28p Cward
//
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 4:05p Khykin
// Initial checkin.
//
// 1     4/22/03 9:40a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//
// 1     11/15/02 3:03p Bmeinke
// Constants and data type definitions for AVT box communications
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================


#ifndef _AVTCOMMDEFS_H_INCLUDED_
#define _AVTCOMMDEFS_H_INCLUDED_

/**
 * Data bytes sent from the AVT box when it ACK's our commands
 */
#define AVT_ACK_STRING			{ 0x01, 0x60 }

/**
 * AVT Command Header byte values:
 *  AVT_PACKET_VAILD
 *  AVT_RESET
 *  AVT_REQ_FILTER_TBL
 *  AVT_CLEAR_FILTER_TBL
 *  AVT_ADD_FILTER_TBL_ENTRY
 *  AVT_WRITE_HIP7030A_RAM
 *  AVT_READ_HIP7030A_RAM
 *  AVT_WRITE_REG1
 *  AVT_READ_REG1
 *  AVT_REQ_VERSION
 *  AVT_REQ_OPER_MODE
 *  AVT_SWITCH_PWM_MODE
 *  AVT_HARDWARE_RESTART
 *
 * @see AVT_PACKET_VAILD
 * @see AVT_RESET
 * @see AVT_REQ_FILTER_TBL
 * @see AVT_CLEAR_FILTER_TBL
 * @see AVT_ADD_FILTER_TBL_ENTRY
 * @see AVT_WRITE_HIP7030A_RAM
 * @see AVT_READ_HIP7030A_RAM
 * @see AVT_WRITE_REG1
 * @see AVT_READ_REG1
 * @see AVT_REQ_VERSION
 * @see AVT_REQ_OPER_MODE
 * @see AVT_SWITCH_PWM_MODE
 * @see AVT_HARDWARE_RESTART
 */
typedef enum
{
	/**
	 * This is a valid packet for transmission
	 */
	AVT_PACKET_VAILD		= 0x00,
	/**
	 * Reserved
	 */
	AVT_RESERVED_1			= 0x10,
	/**
	 * Reset command. Must be followed by one of the following:
	 *     0x01: Reset HBCC device
	 *     0x02: Reset the SCC device
	 *     0x04: Reset FIFO #1
	 *     0x05: Reset FIFO #2
	 */
	AVT_RESET				= 0x21,
	/**
	 * Request current filter byte table entries
	 */
	AVT_REQ_FILTER_TBL		= 0x30,
	/**
	 * Turn off message filtering; clear filter byte table
	 */
	AVT_CLEAR_FILTER_TBL	= 0x31,
	/**
	 * Add a new filter table entry
	 */
	AVT_ADD_FILTER_TBL_ENTRY= 0x32,
	/**
	 * Reserved
	 */
	AVT_RESERVED_4			= 0x40,
	/**
	 * Reserved
	 */
	AVT_RESERVED_5			= 0x50,
	/**
	 * Reserved
	 */
	AVT_RESERVED_6			= 0x60,
	/**
	 * HIP703A0 write RAM
	 */
	AVT_WRITE_HIP7030A_RAM	= 0x72,
	/**
	 * HIP703A0 read RAM
	 */
	AVT_READ_HIP7030A_RAM	= 0x80,
	/**
	 * Hardware register #1 write
	 */
	AVT_WRITE_REG1			= 0x91,
	/**
	 * Hardware register #1 read
	 */
	AVT_READ_REG1			= 0xA1,
	/**
	 * Request AVT firmware version
	 */
	AVT_REQ_VERSION			= 0xB0,
	/**
	 * Reserved
	 */
	AVT_RESERVED_C			= 0xC0,
	/**
	 * Request current operational mode
	 */
	AVT_REQ_OPER_MODE		= 0xD0,
	/**
	 * Switch to PWM mode; must be followed by 0xCC
	 */
	AVT_SWITCH_PWM_MODE		= 0xE1,
	/**
	 * Hardware unit restart command; must be followed by 0xA5
	 */
	AVT_HARDWARE_RESTART	= 0xF1

} AVTCommandByte_t;


/**
 * AVT devctl command ids:
 *  AVT_DEV_CMD_PACKET_VAILD
 *  AVT_DEV_CMD_REQ_FILTER_TBL
 *  AVT_DEV_CMD_CLEAR_FILTER_TBL
 *  AVT_DEV_CMD_ADD_FILTER_TBL_ENTRY
 *  AVT_DEV_CMD_WRITE_HIP7030A_RAM
 *  AVT_DEV_CMD_READ_HIP7030A_RAM
 *  AVT_DEV_CMD_WRITE_REG1
 *  AVT_DEV_CMD_READ_REG1
 *  AVT_DEV_CMD_REQ_VERSION
 *  AVT_DEV_CMD_REQ_OPER_MODE
 *  AVT_DEV_CMD_SWITCH_PWM_MODE
 *  AVT_DEV_CMD_HARDWARE_RESTART
 *
 * @see AVT_DEV_CMD_PACKET_VAILD
 * @see AVT_DEV_CMD_REQ_FILTER_TBL
 * @see AVT_DEV_CMD_CLEAR_FILTER_TBL
 * @see AVT_DEV_CMD_ADD_FILTER_TBL_ENTRY
 * @see AVT_DEV_CMD_WRITE_HIP7030A_RAM
 * @see AVT_DEV_CMD_READ_HIP7030A_RAM
 * @see AVT_DEV_CMD_WRITE_REG1
 * @see AVT_DEV_CMD_READ_REG1
 * @see AVT_DEV_CMD_REQ_VERSION
 * @see AVT_DEV_CMD_REQ_OPER_MODE
 * @see AVT_DEV_CMD_SWITCH_PWM_MODE
 * @see AVT_DEV_CMD_HARDWARE_RESTART
 */
typedef enum
{
	/**
	 * This is a valid packet for transmission
	 */
	AVT_DEV_CMD_PACKET_VAILD = LOG_PORT_DEVCTL_NEXT_AVAIL,
	/**
	 * Reset command. Must be followed by one of the following:
	 *     0x01: Reset HBCC device
	 *     0x02: Reset the SCC device
	 *     0x04: Reset FIFO #1
	 *     0x05: Reset FIFO #2
	 */
	AVT_DEV_CMD_RESET,
	/**
	 * Request current filter byte table entries
	 */
	AVT_DEV_CMD_REQ_FILTER_TBL,
	/**
	 * Turn off message filtering; clear filter byte table
	 */
	AVT_DEV_CMD_CLEAR_FILTER_TBL,
	/**
	 * Add a new filter table entry
	 */
	AVT_DEV_CMD_ADD_FILTER_TBL_ENTRY,
	/**
	 * HIP703A0 write RAM
	 */
	AVT_DEV_CMD_WRITE_HIP7030A_RAM,
	/**
	 * HIP703A0 read RAM
	 */
	AVT_DEV_CMD_READ_HIP7030A_RAM,
	/**
	 * Hardware register #1 write
	 */
	AVT_DEV_CMD_WRITE_REG1,
	/**
	 * Hardware register #1 read
	 */
	AVT_DEV_CMD_READ_REG1,
	/**
	 * Request AVT firmware version
	 */
	AVT_DEV_CMD_REQ_VERSION,
	/**
	 * Request current operational mode
	 */
	AVT_DEV_CMD_REQ_OPER_MODE,
	/**
	 * Switch to PWM mode; must be followed by 0xCC
	 */
	AVT_DEV_CMD_SWITCH_PWM_MODE,
	/**
	 * Hardware unit restart command; must be followed by 0xA5
	 */
	AVT_DEV_CMD_HARDWARE_RESTART

} AVTDevCmd_t;


/**
 * Header bit definitions for message status
 */
typedef enum
{
	AVT_STAT_BAD_CRC		=	0x01,
	AVT_STAT_NUM_BITS		=	0x02,
	AVT_STAT_BREAK			=	0x04,
	AVT_STAT_IFR			=	0x08,
	AVT_STAT_ARBITRATION	=	0x10,
	AVT_STAT_SUCCESS		=	0x20,
	AVT_STAT_DEVICE			=	0x40,
	AVT_STAT_BAD_MSSG		=	0x80

} AvtHeaderStatus_t;

/**
 * Error bit definitions for response $23 $24 (taken ffrom AVT document "can_03c.pdf")
 */
typedef enum
{
	AVT_CAN_LEC0	=	0x01,
	AVT_CAN_LEC1	=	0x02,
	AVT_CAN_LEC2	=	0x04,
	AVT_CAN_TXOK	=	0x08,
	AVT_CAN_RXOK	=	0x10,
	AVT_CAN_WAKE	=	0x20,
	AVT_CAN_WARN	=	0x40,
	AVT_CAN_BOFF	=	0x80

} AvtCanErrorBits_t;

/**
 * Definitions for the different types of message objects:
 * 	AVT_CAN_RX_OBJECT = 0x01
 * 	AVT_CAN_TX_OBJECT = 0x10
 */
typedef enum
{
	AVT_CAN_RX_OBJECT = 0x01,
	AVT_CAN_TX_OBJECT = 0x10
} AvtCanMessageObj_t;

#endif // _AVTCOMMDEFS_H_INCLUDED_

