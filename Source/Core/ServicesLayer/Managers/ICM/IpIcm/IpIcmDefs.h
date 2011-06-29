//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IpIcmDefs.h $
//  $Author: Cward $
//    $Date: 4/13/06 4:05a $
//
// Description:
//    Constants and data definitions for the Altera IP ICM
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IpIcmDefs.h $
// 
// 2     4/13/06 4:05a Cward
// Increased the ISR_FIFO_SZ to 100 to allow larger sample delta values
//
// 2     3/20/06 10:52a Bmeinke
// Increased the ISR_FIFO_SZ to 100 to allow larger sample delta values
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:34a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 4:14p Khykin
// Initial checkin.
//
// 1     5/27/03 8:27a Khykin
// Initial checkin.
//
// 1     4/30/03 4:44p Bmeinke
// Migration to new personalized configuration management structure
//
// 3     3/10/03 7:25p Bmeinke
// Removed unused constants
//
// 2     2/25/03 6:33p Bmeinke
// Added defines fro FORWARD_DIR, REVERSE_DIR, PHASE_A_SELECT,
// PHASE_B_SELECT and SAMPLE_PERIOD
//
//******************************************************************************


#ifndef _IPICMDEFS_H_INCLUDED_
#define _IPICMDEFS_H_INCLUDED_


#include 	"Pci40.h"		// IpInformation_t
#include	"BepSync.h"

// physical constants
#define     KPH_MPH             	1.609344            // KPH per MPH
#define     KPA_PSI             	6.894757291         // KPA per PSI
#define     KGF_LBS             	0.4359237           // KG FORCE per LBS
#define     N_LBS               	4.4482216152        // NEWTONS per LBS
#define     DEG_GRAD            	0.9                 // [deg] per [grad]
#define     DEG_RAD             	57.2957795131       // [deg] per [rad]
#define		CM_INCH					2.54				// [cm/inch]
#define		INCH_FEET				12
#define		FEET_MILE				5280


#define		FORWARD_DIR					0
#define		REVERSE_DIR					1


#define		PHASE_A_SELECT				0x0001	// these can be or'd together
#define		PHASE_B_SELECT				0x0002

//#define		SAMPLE_PERIOD				0.02			// [sec]


// short offsets from IP base address
// READ
#define		DIFF_INPUT_LO				2
#define		DIFF_INPUT_HI				3
#define		TTL_IO						4
#define		INTERRUPT_VECTOR			9
#define		SYSTEM_STATUS				16
#define		TIME_LO						22
#define		TIME_HI						23
#define		CHAN_N_COUNT_LO				28
#define		CHAN_N_COUNT_HI				29
#define		CHAN_N_TIME_LO				30
#define		CHAN_N_TIME_HI				31
// WRITE
#define		DIFF_OUTPUT_LO				0
#define		DIFF_OUTPUT_HI				1
#define		TTL_IO						4
#define		DIFF_DIRECTION_LO			7
#define		DIFF_DIRECTION_HI			8
#define		INTERRUPT_VECTOR			9
#define		SYSTEM_CONFIGURATION		16
#define		OPERATION_CONTROL			17
#define		INTERR_COUNT_LO				22
#define		INTERR_COUNT_HI				23
#define		cHANNEL_SELECT				24
#define		CHANNEL_CONTROL				25
#define		CHAN_N_PHASE_DIV_LO			30
#define		CHAN_N_PHASE_DIV_HI			31



// IO Bits.  Direction: 0=input 1=output
#define		PHASE_A_0					0x00000001
#define		PHASE_A_1					0x00000002
#define		PHASE_A_2					0x00000004
#define		PHASE_A_3					0x00000008
#define		PHASE_B_0					0x00000010
#define		PHASE_B_1					0x00000020
#define		PHASE_B_2					0x00000040
#define		PHASE_B_3					0x00000080
#define		PHASE_C_0					0x00000100
#define		PHASE_C_1					0x00000200
#define		PHASE_C_2					0x00000400
#define		PHASE_C_3					0x00000800
#define		PHASE_C_4					0x00001000
#define		SYNC_IN						0x00010000
#define		SYNC_OUT					0x00020000



// System Configuration/Status register
#define		EN_SAM_TRIG_0				0x0001
#define		EN_SAM_TRIG_1				0x0002
#define		EN_SAM_TRIG_2				0x0004
#define		EN_SAM_TRIG_3				0x0008
#define		EN_PERIODIC_INTERRUPT		0x0010
#define		EN_PERIODIC_DATA_SAMPLE		0x0020
#define		EN_EXTERNAL_SYNC			0x0040
#define		EXTERN_SYNC_MASTER_SLAVE	0x0080
#define		IP_REF_CLOCK				0x0000
#define		EXTERN_REF_CLOCK			0x0100
#define		DIFF_IN_REF_CLOCK			0x0200
#define		SPARE_REF_CLOCK				0x0300


// Operation Control register bits
#define		LATCH_0_TRIGGER				0x0001
#define		LATCH_1_TRIGGER				0x0002
#define		LATCH_2_TRIGGER				0x0004
#define		LATCH_3_TRIGGER				0x0008
#define		INTER_CNT_LOAD_TRIGGER		0x0010
#define		TIME_LATCH_TRIGGER			0x0020



// CHANNEL
	// Channel select
#define		CHANNEL_0_SELECT			0
#define		CHANNEL_1_SELECT			1
#define		CHANNEL_2_SELECT			2
#define		CHANNEL_3_SELECT			3
#define		MAX_CHANNELS				4
#define		CHANNEL_SINGLE_PHASE		7
	// measure channels
#define		MEASURE_IN_0				0x0000			// Phase A
#define		MEASURE_IN_1				0x0001			// Phase B
#define		MEASURE_OUT_0				0x0000			// GND
#define		MEASURE_OUT_1				0x0010			// Input Phase
#define		MEASURE_OUT_2				0x0020			// Divided Pulse
#define		MEASURE_OUT_3				0x0030			// Divided Pulse / 2 (50%)
#define		MEASURE_PHASE_A_EN			0x0100
#define		MEASURE_PHASE_B_EN			0x0200
#define		MEASURE_PHASE_FORCE_LOAD	0x0400
	// Single Phase Channel
#define		SINGLE_PHASE_IN_0			0x0000			// chan 0 out phase
#define		SINGLE_PHASE_IN_1			0x0001			// chan 1 out phase
#define		SINGLE_PHASE_IN_2			0x0002			// chan 2 out phase
#define		SINGLE_PHASE_IN_3			0x0003			// chan 3 out phase
#define		SINGLE_PHASE_OUT_0			0x0000			// GND
#define		SINGLE_PHASE_OUT_1			0x0010			// Input Phase
#define		SINGLE_PHASE_OUT_2			0x0020			// Divided Pulse
#define		SINGLE_PHASE_OUT_3			0x0030			// Divided Pulse / 2 (50%)
#define		SINGLE_PHASE_SPARE_A		0x0100
#define		SINGLE_PHASE_SPARE_B		0x0200
#define		SINGLE_PHASE_FORCE_LOAD		0x0400



// Channel N Count Register
#define		VALID_COUNT_BITS			0x0FFF
#define		PHASE_A_COUNT_BIT			0x1000
#define		PHASE_B_COUNT_BIT			0x2000
#define		DIRECTION_COUNT_BIT			0x4000


/**
 * Number of samples to maintain in our sample FIFO
 */
#define 	ISR_FIFO_SZ					100

//
typedef struct
{
	// cross reference channel ID
	int					channel;

	// new values
	unsigned			count0;
	unsigned			time0;
	// old values
	unsigned			count1;
	unsigned			time1;

	// direction of rotation
	unsigned			direction;						// 1=Reverse 0=Forward

	// accumulated count for distance
	int					accCount;

	// channel control variable (write only)
	unsigned			chnlCtrl;

} ChannelData_t;



typedef struct
{
	// how to reference this module
	int					handle;

	// where is this module
	void				*baseAddress;

	//  read of module information so software knows how to run
	IpInformation_t		ipInfo;

	// data from this module
	ChannelData_t		chnlData[ MAX_CHANNELS];

} IpIcmData_t;

/**
 * List of data associated with each ICM channel
 */
typedef struct
{
	/**
	 * Time at which the count value was latched
	 */
	uint32_t	time;

	/**
	 * Number of encoder pulses received
	 */
	uint32_t	count;

	/**
	 * Direction of the roller attached to this channel
	 * (FORWARD_FIR or REVERSE_DIR)
	 */
	uint32_t	direction;

	/**
	 * Distance that the attached roller has travelled
	 */
	float/*int32_t*/		distance;

	/**
	 * Frequency of the encoder signal
	 */
	float		freq;

	/**
	 * Wheel speed
	 */
	float		speed;

	/**
	 * Acceleration
	 */
	float		accel;

	/**
	 * Brake force
	 */
	float		force;

} IcmChannelData_t;

typedef struct
{
	IcmChannelData_t		channel[4];
} IcmData_t;

/**
 * Data structure used to hold an array of ICM channel samples. The ISR
 * will fill in the 'time' and 'count' values for each of the channels
 * sampled, filling in at the 'head' position. Data is processed by
 * the user level threads beginning from the 'tail' position
 */
typedef struct _IcmSampleList_t
{
	/**
	 * Index where the ISR will put the next sample
	 */
	volatile uint32_t	head;

	/**
	 * Index where we will read the next sample from
	 */
	volatile uint32_t	tail;

	/**
	 * Array of samples from each of the IP ICMs
	 */
	IcmChannelData_t	isrSamples[ MAX_CHANNELS][ ISR_FIFO_SZ];

	/**
	 * Structure used to hold the running average of the last 'n' samples
	 */
	IcmChannelData_t	avgValues[ MAX_CHANNELS];

} IcmSamples_t;

typedef std::vector<IcmSamples_t>	IsrSampleFifo_t;
class IsrSampleFifo : public IsrSampleFifo_t
{
public:
	inline int Lock()	{	return( m_lock.Acquire());	}

	inline int Unlock()	{	return( m_lock.Release());	}

private:
	BepMutex	m_lock;
};

/**
 * Constants used to convert frequency into speed. acceleration, etc
 */
typedef struct
{
	/**
	 * Constant used to convert frequency into distance
	 */
	float	distance;
	/**
	 * Constant used to convert frequency into speed
	 */
	float	speed;
	/**
	 * Constant used to convert frequency difference into acceleration
	 */
	float	accel;
	/**
	 * Constant used to convert frequency difference into brake force
	 */
	float	force;

} IcmChannelConst_t;

/**
 * Data structure used to hold the constants needed to convert frequency
 * into ICM data values
 */
typedef struct
{
	/**
	 * Multiplier constants
	 */
	IcmChannelConst_t	scale[ MAX_CHANNELS];

	/**
	 * Channel offset values
	 */
	IcmChannelConst_t	offset[ MAX_CHANNELS];

} IcmChannelConv_t;

typedef std::vector< IcmChannelConv_t>		ConversionFactors_t;

#endif // _IPICMDEFS_H_INCLUDED_

