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
// 
//******************************************************************************


#ifndef _IPICMDEFS_H_INCLUDED_
#define _IPICMDEFS_H_INCLUDED_


#include 	"DE10Nano.h"		// IpInformation_t
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

//#define		SAMPLE_PERIOD				0.02			// [sec]

// CHANNEL
	// Channel select
#define		CHANNEL_0_SELECT			0
#define		CHANNEL_1_SELECT			1
#define		CHANNEL_2_SELECT			2
#define		CHANNEL_3_SELECT			3
#define		MAX_CHANNELS				4


#define		FORWARD_DIR					0
#define		REVERSE_DIR					1

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
	float				freq0;
	// old values
	unsigned			count1;
	unsigned			time1;
	float				freq1;

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
	//void				*baseAddress;

	//  read of module information so software knows how to run
	//DE10NanoInfo_t		nanoInfo;

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

