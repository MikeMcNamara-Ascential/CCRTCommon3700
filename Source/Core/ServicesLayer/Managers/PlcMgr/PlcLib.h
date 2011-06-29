//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/PlcMgr/PlcLib.h $
//  $Author: Cward $
//    $Date: 12/21/03 6:28p $
//
// Description:
//    PLC Shared Library Interface definition and data types
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
//    $Log: /Core/ServicesLayer/Managers/PlcMgr/PlcLib.h $ 
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
// 1     4/30/03 4:44p Bmeinke
// Migration to new personalized configuration management structure
// 
// 3     1/18/03 3:32p Bmeinke
// Added typedefs fod the chared library PLC interface objects
// 
// 2     12/11/02 9:45p Bmeinke
// Added include of dlfcn.h (for dynamic library loading)
// 
// 1     12/11/02 9:43p Bmeinke
// Initial check-in
//
//******************************************************************************

#ifndef _PLC_LIB_H_INCLUDED_
#define _PLC_LIB_H_INCLUDED_

#include <stdint.h>
#include <pthread.h>
#include <termios.h>
#include <dlfcn.h>

#ifdef __cplusplus
extern "C" {
#endif

#define		PLC_INVALID_HANDLE	NULL

/**
 * Data type used to identify a particular PLC instance.
 * Returned from PlcOpen
 */
typedef struct _PLCHandle
{
	/**
	 * Mutext used to lock access to the PLC
	 */
	pthread_mutex_t	plcLock;

	/**
	 * Driver specfic PLC identifier
	 */
	uint32_t		plcId;
} PLCHandle;

typedef PLCHandle	*PlcHandle_t;

/**
 * Data type passed to (*PlcOpen)()
 */
typedef struct
{
	/**
	 * Size of the structure being passed to PlcOpen
	 */
	PlcHandle_t		plcHandle;
} PlcConfig_t;

/**
 * Data type passed to (*PlcOpen)() for a serial type PLC
 */
typedef struct
{
	/**
	 * Base data structure expected by PlcOpen
	 */
	PlcConfig_t		base;

	/**
	 * Communication settings for the serial port
	 */
	struct termios	portSettings;

	/**
	 * Handle to the communication device used to communicate with the PLC
	 */
	int			comDev;

} SerialPlcConfig_t;

/**
 * Data type passed to (*PlcOpen)() for a VME type PLC
 */
typedef struct
{
	/**
	 * Base data structure expected by PlcOpen
	 */
	PlcConfig_t		base;

	/**
	 * Base address of the PLC in VME Space
	 */
	uint32_t		plcAddress;

	/**
	 * Where to configure the PLC's inputs (our outputs) in VME space
	 */
	uint32_t		inputAddress;

	/**
	 * Where to configure the PLC's outputs (our inputs) in VME space
	 */
	uint32_t		outputAddress;
} VmePlcConfig_t;

/**
 * Data type passed to (*PlcOpen)() for an ethernet type PLC
 */
typedef struct
{
	/**
	 * Base data structure expected by PlcOpen
	 */
	PlcConfig_t		base;

	/**
	 * IP address of the PLC to communicate with
	 */
	char			ipAddress[ 16];
	
	/**
	 * Ehternet port to connect to in order to communicate with the PLC
	 */
	uint32_t		portNum;

} TcpPlcConfig_t;

typedef PlcHandle_t (*FPlcOpen)( PlcConfig_t *plcConfig);

typedef int (*FPlcClose)( PlcHandle_t *whichPlc);

typedef int (*FPlcRead)( PlcHandle_t, uint32_t *buff, uint32_t count);

typedef int (*FPlcWrite)( PlcHandle_t, const uint32_t *buff, uint32_t count);
/**
 * Function pointer holder for functions imported from a PLC shared library
 */
typedef struct
{
	PlcHandle_t (*plcOpen)( PlcConfig_t *plcConfig);

	int (*plcClose)( PlcHandle_t *whichPlc);

	int (*plcRead)( PlcHandle_t, uint32_t *buff, uint32_t count);

	int (*plcWrite)( PlcHandle_t, const uint32_t *buff, uint32_t count);

} PlcFuncs_t;

#ifdef __cplusplus
};
#endif

#endif // _PLC_LIB_H_INCLUDED_
