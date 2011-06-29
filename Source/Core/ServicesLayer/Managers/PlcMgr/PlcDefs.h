//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/PlcMgr/PlcDefs.h $
// $Author:: Cward                                                            $
//   $Date:: 12/21/03 6:28p                                                   $
//
// Description:
//
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
//     $Log: /Core/ServicesLayer/Managers/PlcMgr/PlcDefs.h $
// 
// 1     12/21/03 6:28p Cward
// 
// 1     10/07/03 11:33a Khykin
// Initial checkin
// 
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:26p Khykin
// Initial checkin
// 
// 2     6/20/03 8:50a Khykin
// Updated for Ford changes.
// 
// 1     4/30/03 4:42p Bmeinke
// Migration to new personalized configuration management structure
// 
// 1     6/21/01 1:44p Brian.meinke
// PLC Resource Manager Definitions
//
//******************************************************************************

#ifndef _PLCDEFS_H_
#define _PLCDEFS_H_

//------------------------------------------------------------------------------
// SYSTEM HEADER FILES INCLUSIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// USER DEFINED HEADER FILE INCLUSIONS
//------------------------------------------------------------------------------
#include "BepDefs.h"

//------------------------------------------------------------------------------
// GLOBAL DEFINITIONS
//------------------------------------------------------------------------------

/**
 * Path name to adopt under the /dev directory
 */
#define 	PLC_PRFX_ROOT			"PLC"

/**
 * Path to adopt for the PLC input data items (relative to PLC_PRFX_ROOT)
 */
#define		PLC_INPUT_PATH			"Inputs"

/**
 * Path to adopt for the PLC output data items (relative to PLC_PRFX_ROOT)
 */
#define		PLC_OUTPUT_PATH			"Outputs"

/**
 * Default VME scan period (in milliseconds)
 */
#define 	CHANNEL_TIME			20

/**
 * Default name to register with the OS
 */
#define 	PLC_MGR_NAME			"mgr/PLC"

/**
 * Config file XML node tag to read VME scan period from
 */
#define		CFG_CHNL_TIME_TAG		XML_T("ChannelTime")

/**
 * Config file XML node tag to read channel count from
 */
#define		CFG_CHNL_COUNT_TAG		XML_T("ChannelCount")

/**
 * Config file XML node tag for Input configuration node
 */
#define		CFG_INPUT_TAG			XML_T("Inputs")

/**
 * Config file XML node tag for Input configuration node
 */
#define		CFG_OUTPUT_TAG			XML_T("Outputs")

/**
 * Config file XML node tag for input/output buffer addresses
 */
#define		CFG_IO_ADDRESS_TAG		XML_T("Address")

/*******************************************************************************
  PLCConfigData type
	Holds PLC configuration information
*******************************************************************************/
typedef struct _PLCConfigData
{
	UINT32	m_baseAddress;		// Base VME address
	UINT32	m_count;			// Item Count for this Board
	UINT8	m_intVec;			// VME Interrupt vector
	UINT8	m_intMask;			// Interrupt mask
	UINT8	m_irqLevel;			// VME Interrupt level
} PLCConfigData;

/*******************************************************************************
  PLCSysCfg type
	Holds config data for a system of PLC boards
*******************************************************************************/
typedef struct _PLCSysCfg
{
	UINT32			m_boardCount;	// Number of AIN's in system
	UINT32			m_scanRate;		// Loop scan rate (ms)
	PLCConfigData	*m_cfgData;		// Config per board
} PLCSysCfg;

#endif		// _PLCDEFS_H_

