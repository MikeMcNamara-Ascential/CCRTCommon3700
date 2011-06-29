//******************************************************************************
//
// $Archive: /Working/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComDefs.h $
// $Author: Cward $
//   $Date: 5/11/05 9:39p $
//
// Description:
//    Constants and data type definitions for the MultiCom Comm Proxy
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Working/Source/Core/ServicesLayer/Managers/CommProxy/MultiCom/MultiComDefs.h $
// 
// 1     5/11/05 9:39p Cward
// Initial Version
// 
// 1     4/26/05 10:02a Bmeinke
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for Ford
// 
// 2     4/23/04 4:57p Bmeinke
// Added additional module ID offsets for different types of messages
// Fixed the MC_HBCC_RCVD_MSSG and MC_HBCC_SENT_MSSG values
// Added MultiComTxStatus_t type for the status bits in the MultiCom TX
// status byte
// 
// 1     4/12/04 1:21p Bmeinke
// Changing VSS repositories
// 
// 1     4/12/04 10:34a Bmeinke
// 
//
//******************************************************************************

#ifndef _MULTICOM_DEFS_H_INCLUDED_
#define _MULTICOM_DEFS_H_INCLUDED_

#include "BepSync.h"
#include "BepDefs.h"	// mSEC_nseC

typedef uint8_t				ModuleId_t;

/**
 * Invalid module ID specifier
 */
#define	INVALID_MODULE_ID	0xFF

/**
 * Index of a module ID in transmit string to a module
 */
#define MODULE_TX_ID_INDEX		3

/**
 * Index of a module ID in a multicom TX ACK string
 */
#define MODULE_ACK_ID_INDEX		3

/**
 * Index of a module ID in a module response string 
 */
#define MODULE_RX_ID_INDEX		2

/**
 * Default reply timeout for module responses
 */
#define DEF_MOD_TIMEOUT		mSEC_nSEC( 500LL)

/**
 * Timeout for replies from the MultiCom box
 */
#define MULTI_COM_TIMEOUT	mSEC_nSEC( 5000LL)

/**
 * Timeout for ACKs from the MultiCom box
 */
#define ACK_TIMEOUT			mSEC_nSEC( 1500LL)

/**
 * String sent by the MultiCom module to acknowledge the 
 * receipt of a command from us
 */
#define ACK_STRING	{ 0x34, 0x00, 0x28 }

/**
 * Multicom TX message status values
 */
typedef enum
{
	/**
	 * No message sent
	 */
	MC_NO_MSSG_SENT		= 0x00,
	/**
	 * Message sent, waiting for ACK
	 */
	MC_MSSG_SENT_PEND	= 0x01,
	/**
	 * Message sent, ACK received from MultiCom
	 */
	MC_MSSG_SENT_ACK	= 0x02
	
} MultiComTxFlag_t;

/**
 * Type used to abstract condvar for MultiCom read flags
 */
typedef BepCondVar< bool>				MultiComRxCondVar;

/**
 * Type used to abstract condvar for MultiCom write flags
 */
typedef BepCondVar< MultiComTxFlag_t>	MultiComTxCondVar;

/**
 * Type used to abstract condvar for serial strings
 */
typedef BepCondVar< SerialString_t>		MultiComStringCondVar;

/**
 * Command codes used to communicates with the MultiCom module
 */
typedef enum
{
	MC_ERROR_RESPONSE			= 0x00,
	
	MC_GET_FIRMWARE_VER			= 0x00,
	MC_GET_SERIAL_NUMBER		= 0x01,
	MC_GET_STATUS				= 0x03,
	MC_GET_CLOCK				= 0x04,
	MC_SET_CLOCK				= 0x05,
	MC_RESET					= 0x09,

	MC_SCP_GET_FIRMWARE_VER		= 0x30,
	MC_SCP_NORMAL_MODE			= 0x31,
	MC_SCP_GET_STATUS			= 0x33,
	MC_SCP_SEND_MSSG			= 0x34,
	MC_SCB_READ_MSSG			= 0x35,
	MC_SCP_GET_RCV_BYTE_COUNT	= 0x36,
	MC_SCP_CLEAR_RCV_BUFF		= 0x37,
	MC_SCP_CLEAR_XMT_BUFF		= 0x3B,
	MC_SCP_GET_XMT_BYTE_COUNT	= 0x3A,

	MC_SCP_GET_BIT_RATE			= 0x46,
	MC_SCP_SET_BIT_RATE			= 0x47,
	MC_SCP_GET_INTER_MSSG_DELAY	= 0x48,
	MC_SCP_SET_INTER_MSSG_DELAY	= 0x49,
	MC_SCP_START_GROUP			= 0x4A,
	MC_SCP_STOP_GROUP			= 0x4B,
	MC_SCP_GET_GRP_COUNT		= 0x4C,
	MC_SCP_READ_GRP_MSSG		= 0x4D,
	MC_SCP_GET_GRP_CONTROL		= 0x4E,
	MC_SCP_CLEAR_GROUP_BUFF		= 0x4F,
	
	MC_SCP_DEF_GRP_MSSG			= 0x50,
	MC_SCP_DEF_GRP_CONTROL		= 0x51,
	MC_SCP_TERMINATION_ON		= 0x52,
	MC_SCP_TERMINATION_OFF		= 0x53,
	MC_SCP_READ_HBCC_REGISTERS	= 0x54,
	MC_SCP_WRITE_HBCC_REGISTERS	= 0x55,
	MC_SCP_READ_HBCC_RAM		= 0x56,
	MC_SCP_WRITE_HBCC_RAM		= 0x57,
	MC_SCP_GET_SMT_STATUS		= 0x58,
	MC_SCP_MONITOR_MODE			= 0x59,
	MC_SCP_AUTO_XMT_ON			= 0x5E,
	MC_SCP_AUTO_XMT_OFF			= 0x5F,

	MC_ISO_GET_FIRMWARE_VER		= 0x60,
	MC_ISO_NORMAL_MODE			= 0x61,
	MC_ISO_DEACTIVATE			= 0x62,
	MC_ISO_GET_STATUS			= 0x63,
	MC_ISO_SEND_MSSG			= 0x64,
	MC_ISO_READ_MSSG			= 0x65,
	MC_ISO_GET_RCV_BYTE_COUNT	= 0x66,
	MC_ISO_CLEAR_RCV_BUFF		= 0x67,
	MC_ISO_GET_XMT_BYTE_COUNT	= 0x6A,
	MC_ISO_CLEAR_XMT_BUFF		= 0x6B,

	MC_ISO_GET_BIT_RATE			= 0x76,
	MC_ISO_SET_BIT_RATE			= 0x77,
	MC_ISO_GET_INTER_MSSG_DELAY	= 0x78,
	MC_ISO_SET_INTER_MSSG_DELAY	= 0x79,
	MC_ISO_START_KAM			= 0x7A,
	MC_ISO_STOP_KAM				= 0x7B,
	MC_ISO_READ_KAM				= 0x7D,
	MC_ISO_GET_KAM_DELAY		= 0x7E,
	MC_ISO_CLEAR_KAM_BUFF		= 0x7F,

	MC_ISO_DEFINE_KAM			= 0x80,
	MC_ISO_DEFINE_KAM_DELAY		= 0x81,
	MC_ISO_ENABLE_LOOPBACK		= 0x82,
	MC_ISO_DISABLE_LOOPBACK		= 0x83,
	MC_ISO_GET_INTER_BYTE_DELAY	= 0x86,
	MC_ISO_SET_INTER_BYTE_DELAY	= 0x87,
	MC_ISO_GET_XMT_STATUS		= 0x88,
	MC_ISO_AUTO_XMT_ON			= 0x8E,
	MC_ISO_AUTO_XMT_OFF			= 0x8F,

	MC_485_GET_FIRMWARE_VER		= 0x90,
	MC_485_RETRANSMIT_LAST		= 0x91,
	MC_485_SET_REMOTE_BIT_RATE	= 0x92,
	MC_485_CLOSE_RELAY			= 0x93,
	MC_485_OPEN_RELAY			= 0x94,
	
	MC_HBCC_RCVD_MSSG			= 0xD0,
	MC_HBCC_SENT_MSSG			= 0xD1
	
} MultiComCommand_t;


/**
 * Status bit definitions in a Multicom TX acknowledgemnet packet ($D1).
 * The first 3 bits in the status byte indicate the number of data bytes 
 * to follow (excluding the CRC byte)
 */
typedef enum
{
	/**
	 * Transmit jabber watchdog
	 */
	MC_TX_STATUS_TX_JABBER		= 0x08,
	
	/**
	 * Network driver watchdog
	 */
	MC_TX_STATUS_NET_DRVR_WD	= 0x10,
	
	/**
	 * Loss of arbitration limit exceeded
	 */
	MC_TX_STATUS_LOSS_ARB		= 0x20,
	
	/**
	 * Transmit error limit exceeded
	 */
	MC_TX_STATUS_TX_ERR_LIMIT	= 0x40,
	
	/**
	 * Message transmitted OK
	 */
	MC_TX_STATUS_OK				= 0x80
	
} MultiComTxStatus_t;


#endif // _MULTICOM_DEFS_H_INCLUDED_

