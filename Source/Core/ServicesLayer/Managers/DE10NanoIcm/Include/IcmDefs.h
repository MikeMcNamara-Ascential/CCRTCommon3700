//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/Include/IcmDefs.h $
// $Author:: Cward                                                            $
//   $Date:: 4/13/06 4:02a                                                    $
//
// Description:
//	 Data type definitions for use with the ICM (BEP 7280)
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/Include/IcmDefs.h $
// 
// 2     4/13/06 4:02a Cward
// Cleanup
//
// 2     2/15/06 4:08p Bmeinke
// Removed the $Revision: 2 $ and $Date: 4/13/06 4:02a $ strings from the __USAGE block
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:34a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:11p Khykin
// Initial checkin.
//
// 2     5/19/03 11:18a Khykin
// Updated with Brians latest changes.
//
// 1     4/30/03 4:42p Bmeinke
// Migration to new personalized configuration management structure
//
// 16    3/12/03 7:39p Bmeinke
// Moved IcmSubChannelIdx_t back to the individual ICM manager header
// files due to sub-channel index differences
//
// 15    3/10/03 6:37p Bmeinke
// Moved  ICM_PULSE_CODE, ICM_TIMER_PULSE, AsciiLogFile_t,
// IcmChannelIdx_t, IcmSubChannelIdx_t and IcmArrayIndex_t from IcmMgr.h
// to this file for use with the IpIcmMgr
//
// 14    10/15/02 6:51p Bmeinke
// Added MACH_3600_ID_NAME define: Tag name of the node used to define the
// data tag for the 3600 machine ID bit
//
// 13    10/15/02 3:34p Bmeinke
// Added define for MACH_3600_ID_NAME
//
// 12    8/23/02 11:21a Kh
// Updating for logging changes.
//
// 11    4/26/02 12:34a Bmeinke
// Added Tag name for Overspeed parameter
//
// 10    2/27/02 9:22p Bmeinke
// Added defines SAMPLE_DELTA_NAME, ACCUM_TARGET_NAME and
// PRESCALE_MODE_NAME for reading associated data from config file
//
// 9     2/27/02 2:40p Bmeinke
// Added CFG_TAG_LIST_TAG to define the node which holds the mappings of
// data tag names we will use
// Added tags to define the nodes which will map the names for system data
// used by the ICM into system data tags
//
// 8     2/01/02 10:27p Brian.meinke
// Added XML node tag definitions for log file configuration
//
// 7     11/30/01 5:13p Brian.meinke
// Added support for logging array data to an ASCII file for FTP access by
// the GUI
//
// 6     10/09/01 4:08p Brian.meinke
// Changed case of Channel prefix tags to mixed case
//
// 5     6/11/01 8:57a Brian.meinke
// Removed references to m_logger and replaced with *this
//
// 4     5/07/01 5:49p Brian.meinke
// Changed define names fromt ICM_* to ICM7280_*
//
// 3     4/24/01 4:42p Brian.meinke
// Changed registered name from mgr/IcmMgr to mgr/ICM
//
// 2     4/13/01 10:20a Brian.meinke
// Added min/max addresses range for board autodetection
//
// 1     4/10/01 10:46a Admin
//
// 1     3/20/01 4:18p Brian.meinke
// Data type definitions for use with the ICM (BEP 7280)
//
//******************************************************************************

#ifndef _ICM_DEFS_H_
#define _ICM_DEFS_H_

#include <stdio.h>		// FILE
#include <string>		// std::string


/**
 * Pulse code to send for ICM pulses
 */
#define	ICM_PULSE_CODE	SYSTEM_PULSE+1

/**
 * Pulse value to send when the scan timer expires
 */
#define	ICM_TIMER_PULSE	0

/**
 * Structure to hold data used for creating ascii data log files
 */
typedef struct _AsciiLogFile_t
{
	/**
	 * Name of the log file to save the data to
	 */
	std::string		logFileName;

	/**
	 * Name of the file link to create at the end of the test for the log file
	 */
	std::string		logFileLink;

	/**
	 * Handle to the open log file
	 */
	FILE		*logFile;

} AsciiLogFile_t;


/**
 * Enumeration of ICM channel indices
 */
typedef enum
{
	LF_IDX = 0,
	RF_IDX = 1,
	LR_IDX = 2,
	RR_IDX = 3,
	LT_IDX = 4,
	RT_IDX = 5,
	ICM_CHANNEL_COUNT
} IcmChannelIdx_t;

/**
 * Enumeration used as index for array data
 */
typedef enum
{
	DIST_ARRAY		= 0,
	SPEED_ARRAY		= 1,
	ACCEL_ARRAY		= 2,
	FORCE_ARRAY		= 3,
	FREQ_ARRAY		= 4,
	ICM_ARRAY_COUNT
} IcmArrayIndex_t;

#define ICM_PRFX_ROOT				"Icm"

/**
 * Default VME scan period (in milliseconds)
 */
#define SCAN_TIME					20		// 20 ms default scan time

/**
 * Default name to register with the OS
 */
#define ICM_NAME					"mgr/ICM"

/**
 * Number of channels per ICM board
 */
#define ICM_CHNL_BRD				4

/**
 * Accurate value for pi (used for ICM caclulations)
 */
#define	ICM_PI						3.14159265358979323846

/**
 * Config file XML node tag for VME scanner priority
 */
#define	CFG_SCAN_PRIO_TAG			XML_T( "ScannerPriority")

/**
 * Config file XML node tag for VME scanner period
 */
#define	CFG_SCAN_PERIOD_TAG			XML_T( "ScanPeriod")

/**
 * Config file XML node tag for Left Front channel configuration
 */
#define	LF_CHNL_TAG					"Lf"

/**
 * Config file XML node tag for Right Front channel configuration
 */
#define	RF_CHNL_TAG					"Rf"

/**
 * Config file XML node tag for Left Rear channel configuration
 */
#define	LR_CHNL_TAG					"Lr"

/**
 * Config file XML node tag for Right Rear  channel configuration
 */
#define	RR_CHNL_TAG					"Rr"

/**
 * Config file XML node tag for Left Tandem channel configuration
 */
#define	LT_CHNL_TAG					"Lt"

/**
 * Config file XML node tag for Right Tandem channel configuration
 */
#define	RT_CHNL_TAG					"Rt"

/**
 * Config file XML node tag for roll factor value
 */
#define	CFG_ROLL_FACTOR_TAG			XML_T( "RollFactor")

/**
 * Config file XML node tag for encoder pulses per revolution  value
 */
#define	CFG_ENC_PULSES_TAG			XML_T( "EncoderPulses")

/**
 * Config file XML node tag for roll diameter value
 */
#define	CFG_ROLL_DIAM_TAG			XML_T( "RollDiameter")

/**
 * Config file XML node tag for machine loss value
 */
#define	CFG_MACH_LOSS_TAG			XML_T( "MachineLoss")

/**
 * Config file XML node tag for channel index value
 */
#define	CFG_CHNL_IDX_TAG			XML_T( "channelIndex")

/**
 * Config file XML node tag for board index value
 */
#define	CFG_BOARD_IDX_TAG			XML_T( "boardIdx")

/**
 * XML node tag of config node containing the name of the log file to store
 * ASCII ICM data in
 * @since Nov 30, 2001
 */
#define CFG_ICM_LOG_TAG				XML_T( "DataLogFile")

/**
 * XML node tag of config node containing the name of the log file to store
 * ASCII ICM data in
 * @since Nov 30, 2001
 */
#define CFG_LOG_LNK_TAG				XML_T( "DataLogLink")

/**
 * XML node tag of config node containing the name of the data tag to use
 * when subscribing for the start of test event
 * @since Nov 30, 2001
 */
#define CFG_START_TEST_TAG			XML_T( "StartTestTag")

/**
 * XML node tag of config node containing the name of the data tag to use
 * when subscribing for the test in progress test event
 * @since Nov 30, 2001
 */
#define CFG_TESTING_TEST_TAG		XML_T( "TestInProgress")

/**
 * XML node tag of config node containing the name of the data tag to use
 * when subscribing for the end of test event
 * @since Nov 30, 2001
 */
#define CFG_END_TEST_TAG			XML_T( "EndTestTag")

/**
 * XML node tag of the data logging configuration parent node
 * @since Dec 12, 2001
 */
#define CFG_DATA_LOG_TAG			XML_T( "DataLogFiles")

/**
 * XML node tag of the tag list configuration parent node
 * @since Dec 12, 2001
 */
#define CFG_TAG_LIST_TAG			XML_T( "TagNames")

/**
 * XML node tag of the data logging configuration node used to configure
 * data logging for distance data
 * @since Dec 12, 2001
 */
#define CFG_DIST_LOG_TAG			XML_T( "Distance")

/**
 * XML node tag of the data logging configuration node used to configure
 * data logging for distance data
 * @since Dec 12, 2001
 */
#define CFG_SPEED_LOG_TAG			XML_T( "Speed")

/**
 * XML node tag of the data logging configuration node used to configure
 * data logging for brake force data
 * @since Dec 12, 2001
 */
#define CFG_FORCE_LOG_TAG			XML_T( "BrakeForce")

/**
 * XML node tag of the data logging configuration node used to configure
 * data logging for acceleration data
 * @since Dec 12, 2001
 */
#define CFG_ACCEL_LOG_TAG			XML_T( "Acceleration")

/**
 * XML node tag of the data logging configuration node used to configure
 * data logging for frequency data
 * @since Dec 12, 2001
 */
#define CFG_FREQ_LOG_TAG			XML_T( "Frequency")

/**
 * XML node tag of node attribute containing the name of the an ascii data log file
 * @since Dec 12, 2001
 */
#define CFG_LOG_NAME_TAG			XML_T( "name")

/**
 * XML node tag of node attribute containing the name of the link for an ascii
 * data log file
 * @since Dec 12, 2001
 */
#define CFG_LOG_LINK_TAG			XML_T( "link")

/**
 * Board auto detection string
 */
#define 	ICM7280_ID_STRING   	"VMEID BEP P/N 7280A"

/**
 * Lowest valid VME address for the ICM board
 */
#define		ICM7280_MIN_ADDRESS		0xFFFF0000

/**
 * Address at which to stop searching for an ICM card at
 */
#define		ICM7280_MAX_ADDRESS		0xFFFF4000

/**
 * Address increment used when searching for an ICM card in VME space
 */
#define		ICM7280_INC_ADDRESS		0x00000400

/**
 * Tag name of the node used to define the data tag for the left front channel
 */
#define 	LF_CHANNEL_NAME			"LfChannelTag"

/**
 * Tag name of the node used to define the data tag for the right front channel
 */
#define 	RF_CHANNEL_NAME			"RfChannelTag"

/**
 * Tag name of the node used to define the data tag for the left rear channel
 */
#define 	LR_CHANNEL_NAME			"LrChannelTag"

/**
 * Tag name of the node used to define the data tag for the right rear channel
 */
#define 	RR_CHANNEL_NAME			"RrChannelTag"

/**
 * Tag name of the node used to define the data tag for the left tandem channel
 */
#define 	LT_CHANNEL_NAME			"LtChannelTag"

/**
 * Tag name of the node used to define the data tag for the right tandem channel
 */
#define 	RT_CHANNEL_NAME			"RtChannelTag"

/**
 * Tag name of the node used to define the data tag for zerospeed
 */
#define 	ZEROSPEED_NAME			"ZerospeedTag"

/**
 * Tag name of the node used to define the data tag for overspeed
 */
#define 	OVERSPEED_NAME			"OverspeedTag"

/**
 * Tag name of the node used to define the data tag for the testing test event
 */
#define 	TESTING_TEST_NAME		"TestingTestTag"

/**
 * Tag name of the node used to define the data tag for the array index
 */
#define 	TAG_IDX_NAME			"TagIndexTag"

/**
 * Tag name of the node used to define the data tag for the number of rollers
 */
#define		ROLLER_COUNT_NAME		"RollerCountTag"

/**
 * Tag name of the node used to define the data tag for writing zerospeed to
 * the PLC
 */
#define		ZEROSPEED_LATCH_NAME	"ZerospeedLatchTag"

/**
 * Tag name of the node used to define the data tag for LF roll direction
 */
#define		LF_ROLL_DIR_NAME		"LfRollDirTag"

/**
 * Tag name of the node used to define the data tag for RF roll direction
 */
#define		RF_ROLL_DIR_NAME		"RfRollDirTag"

/**
 * Tag name of the node used to define the data tag for LR roll direction
 */
#define		LR_ROLL_DIR_NAME		"LrRollDirTag"

/**
 * Tag name of the node used to define the data tag for RR roll direction
 */
#define		RR_ROLL_DIR_NAME		"RrRollDirTag"

/**
 * Tag name of the node used to define the data tag for LT roll direction
 */
#define		LT_ROLL_DIR_NAME		"LtRollDirTag"

/**
 * Tag name of the node used to define the data tag for RT roll direction
 */
#define		RT_ROLL_DIR_NAME		"RtRollDirTag"

/**
 * Tag name of the node used to define the data tag for sample delta used
 * for calculating accel/force
 */
#define		SAMPLE_DELTA_NAME		"SampleDelta"

/**
 * Tag name of the node used to define the data tag for accumulator register
 */
#define		ACCUM_TARGET_NAME		"AccumulatorTarget"

/**
 * Tag name of the node used to define the data tag for prescale mode
 */
#define		PRESCALE_MODE_NAME		"PrescaleMode"

/**
 * Tag name of the node used to define the data tag for the
 * 3600 machine ID bit
 */
#define		MACH_3600_ID_NAME		"Machine3600IdTag"
// Tag names for roller factors.
#define     LF_ROLL_FACTOR_NAME     "LfRollFactorTag"
#define     RF_ROLL_FACTOR_NAME     "RfRollFactorTag"
#define     LR_ROLL_FACTOR_NAME     "LrRollFactorTag"
#define     RR_ROLL_FACTOR_NAME     "RrRollFactorTag"
#define     LT_ROLL_FACTOR_NAME     "LtRollFactorTag"
#define     RT_ROLL_FACTOR_NAME     "RtRollFactorTag"

#endif	// _ICM_DEFS_H_


