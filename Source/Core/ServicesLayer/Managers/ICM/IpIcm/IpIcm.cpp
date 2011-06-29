//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IpIcm.cpp $
//  $Author: Cward $
//    $Date: 4/13/06 4:04a $
//
// Description:
//    Driver for the Altera IP module ICM
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IpIcm.cpp $
// 
// 2     4/13/06 4:04a Cward
// removed usage block
//
// 2     2/15/06 4:10p Bmeinke
// Removed the __USAGE block
//
// 1     12/21/03 6:28p Cward
//
// 2     10/24/03 1:19p Khykin
// Updated for machines without encoder phasing.
//
// 1     10/07/03 11:33a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/26/03 4:01p Khykin
// Updated with Brians latest changes.
//
// 1     6/20/03 3:36p Bmeinke
//
// 5     6/20/03 3:35p Bmeinke
// Updated to work in demo mode
//
// 4     4/30/03 2:12p Bmeinke
// FordChicago updates
//
// 3     3/18/03 6:01p Bmeinke
// Changed 'm_uspIP[ OPERATION_CONTROL] = EN_PERIODIC_DATA_SAMPLE |
// INTER_CNT_LOAD_TRIGGER" to 'm_uspIP[ OPERATION_CONTROL] =
// INTER_CNT_LOAD_TRIGGER' in AckIrq()
// Changed where AckIrq() is called in EnableIrq() (called at the end
// instead of the beginning)
//
// 2     3/10/03 7:20p Bmeinke
// Moved initialization of interrupt counters to EnableIrq()
// EnableIrq() now takes a samplePeriod argument to use for calculating
// the necessary interrupt counter values required to sample data with the
// period given by the samplePeriod argument
//
// 1     3/10/03 11:08a Bmeinke
// Initial checkin. Compiles and runs
//
//******************************************************************************

#include <unistd.h>
#include <sys/types.h>
//#include <sys/neutrino.h>
#include <vector>

#include "Pci40.h"
#include "IpIcm.h"
#include "IpIcmDefs.h"

const double IpIcm::m_refFreq = 8e6;

/**
 * Constructor
 */
IpIcm::IpIcm() : m_uspIP( NULL), m_ipInt( NULL), m_totalMeasureChannels( 0),
	m_ipCarrier( NULL), m_moduleIdx( -1)
{
}

/**
 * Destructor
 */
IpIcm::~IpIcm()
{
	DisableIrq();
}

/**
 * Initializes an IpIcm object using the IP module at index 'moduleIdx'
 * on the given IP carrier
 *
 * @param ipCarrier IP carrier board on which the IP module is located
 * @param moduleIdx The IP module index on the IP carrier
 * @param encoderPhasing
 *                  The phaseing to use for the encoders (single = 1, quadrature = 0)
 *                  It will default to quadrature.
 *
 * @return true if successfully initialized, false if an error occurred
 */
bool IpIcm::Initialize( const Pci40 *ipCarrier, int moduleIdx, bool encoderPhasing /*= 0*/)
{
	bool		retVal = false;
	unsigned	modCount=0;

	Log( LOG_FN_ENTRY, "Enter IpIcm::Initialize( %d)\n", moduleIdx);

	// What carrier is our IP module on
	m_ipCarrier = (Pci40 *)ipCarrier;
	m_moduleIdx = moduleIdx;

	// ***************
	// Local Last
	// ***************
	// Init data
	InitializeIpIcmData();

	// if found this occurance
	if( -1 != (m_ipIcmData.handle = LocateIpIcm( m_moduleIdx)))
	{
		Log( LOG_DEV_DATA, "IP-ICM Module %d Located.  Handle: $%x\n", m_moduleIdx, m_ipIcmData.handle);

		// get information on this IP
		if( 0 == m_ipCarrier->GetIpInformation( m_ipIcmData.handle, m_ipIcmData.ipInfo))
		{
			Log( LOG_DEV_DATA, "IP Information Collected\n");
			Log( LOG_DEV_DATA, "\tType  = %d\n", m_ipIcmData.ipInfo.type);
			Log( LOG_DEV_DATA, "\tSpeed = %f [MHz]\n", m_ipIcmData.ipInfo.speed / 1e6);
			Log( LOG_DEV_DATA, "\tMfg   = $%x\n", m_ipIcmData.ipInfo.mfg);
			Log( LOG_DEV_DATA, "\tModel = $%x\n", m_ipIcmData.ipInfo.model);
			Log( LOG_DEV_DATA, "\tRevision = %d\n", m_ipIcmData.ipInfo.revision);
			Log( LOG_DEV_DATA, "\tSoftwareDriver = %x.%02x\n", m_ipIcmData.ipInfo.driver/0x100, m_ipIcmData.ipInfo.driver%0x100);

			// go initialize this module
			if( 0 == InitializeIpICM(encoderPhasing))
			{
				// another one ready
				modCount++;
				retVal = true;
			}
			else
			{
				m_ipIcmData.handle = -1;
				Log( LOG_ERRORS, "\tInitializeIpICM( %d) failed\n", m_moduleIdx);
			}
		}
		else
		{
			m_ipIcmData.handle = -1;								// not usable
			Log( LOG_ERRORS, "\tGetIpInformation( %d) failed\n", m_moduleIdx);
		}
	}

	// if it is not successful
	if( 0 >= modCount)
	{
		Log( LOG_ERRORS, "\tNo IP ICM Modules found\n");
		retVal = false;
	}

	Log( LOG_FN_ENTRY, "Exit IpIcm::Initialize()\n");

	return( retVal);
}

/**
 * Initializes our logger interface object
 *
 * @param loggerCfg The logger object whose configuration we want to copy
 */
void IpIcm::Initialize( const ILogger &loggerCfg)
{
	SetAdditionalLogFileName( loggerCfg.GetAdditionalLogFileName());
	SetProcessName( loggerCfg.GetProcessName());
	SetVerboseMask( loggerCfg.GetVerboseMask());
	SetDebug(loggerCfg.IsDebugOn());
	SetLocalEcho( loggerCfg.GetLocalEcho());
	if( loggerCfg.GetLogStatus())
	{
		EnableLogger();
	}
}

/**
 * Ip Icm process
 *
 * @return 0 if successful, -1 if an error occurred
 */
int IpIcm::Run( void)
{
	int	retVal=-1;

	Log( LOG_FN_ENTRY, "Enter IpIcm::Run()\n");

	if( 0 == SampleFirstReadings())		retVal = 0;
	else								Log( LOG_ERRORS, "\tError in SampleFirstReadings()\n");

	Log( LOG_FN_ENTRY, "Exit IpIcm::Run()\n");
	return( retVal);
}

/**
 * Triggers channel latches
 *
 * @param channel Channel to latch: -1 = All Channels (including Time)
 *                or a positive channel number
 *
 * @return 0 if successful, -1 if an error occurred
 */
int IpIcm::LatchChannelValues( int channel)
{
	int			retVal=-1;
	unsigned	chnlIdx, val;

	// if do all channels on this module
	if( -1 == channel)
	{
		val = LATCH_0_TRIGGER | LATCH_1_TRIGGER | LATCH_2_TRIGGER | LATCH_3_TRIGGER | TIME_LATCH_TRIGGER;
		TriggerLatches( val);
		retVal = 0;
	}
	else
	{
		// scan for a matching channel
		for( chnlIdx=0; chnlIdx<MAX_CHANNELS; chnlIdx++)
		{
			// if this is a match
			if( channel == m_ipIcmData.chnlData[ chnlIdx].channel)
			{
				if( 0 == channel)
				{
					TriggerLatches( LATCH_0_TRIGGER);
					retVal = 0;
				}
				else if( 1 == channel)
				{
					TriggerLatches( LATCH_1_TRIGGER);
					retVal = 0;
				}
				else if( 2 == channel)
				{
					TriggerLatches( LATCH_2_TRIGGER);
					retVal = 0;
				}
				else if( 3 == channel)
				{
					TriggerLatches( LATCH_3_TRIGGER);
					retVal = 0;
				}
			}
		}
	}

	//
	return( retVal);
}

/**
 * Reads time and count from given channel
 *
 * @param channel Channel to read from
 * @param time    Pointer to where to store channel time value
 * @param count   Pointer to where to store channel count value
 *
 * @return 0 if successful, -1 if an error occurred
 */
int IpIcm::ReadChannelValues( int channel, uint32_t &time, uint32_t &count)
{
	int			retVal=-1;
	unsigned	chnlIdx;

	// scan for a matching channel
	chnlIdx = 0;
	while((chnlIdx < MAX_CHANNELS) && (0 != retVal))
	{
		// if this is the channel i am looking for
		if( channel == m_ipIcmData.chnlData[ chnlIdx].channel)
		{
			// valid
			retVal = 0;

			// select channel
			SelectChannel( channel);

			// read values
			count = ReadActiveChannelCount();
			time = ReadActiveChannelTime();
		}
		// next channel
		chnlIdx++;
	}

	//
	return( retVal);
}

/**
 * En/Dis-ables periodic sample
 *
 * @param state true to enable periodic sample, false to disable periodic sample
 */
void IpIcm::PeriodicSample( bool state)
{
	if( m_uspIP != NULL)
	{
		// turn on or off
		if( true == state)
		{
			m_uspIP[ SYSTEM_CONFIGURATION] |= EN_PERIODIC_DATA_SAMPLE;
		}
		else
		{
			m_uspIP[ SYSTEM_CONFIGURATION] &= ~EN_PERIODIC_DATA_SAMPLE;
		}
	}
}

/**
 * En/Dis-able selected phase for selected channel
 *
 * @param channel channel to change
 * @param phase   PHASE_A_SELECT or PHASE_B_SELECT (can be or'd together)
 * @param state   true to enable, false to disable
 *
 * @return 0 if successful, -1 if an error occurred
 */
int IpIcm::PhaseControl( int channel, uint32_t phase, bool state)
{
	int			retVal=-1;
	unsigned	chnlIdx;

	// scan for a matching channel
	chnlIdx = 0;
	while((chnlIdx < MAX_CHANNELS) && (0 != retVal) && (m_uspIP != NULL))
	{
		// if this is the channel i am looking for
		if( channel == m_ipIcmData.chnlData[ chnlIdx].channel)
		{
			if( PHASE_A_SELECT & phase)
			{
				if( true == state)
				{
					// ok
					retVal = 0;
					// set bit
					m_ipIcmData.chnlData[ chnlIdx].chnlCtrl |= MEASURE_PHASE_A_EN;
				}
				else if( false == state)
				{
					// ok
					retVal = 0;
					// clear bit
					m_ipIcmData.chnlData[ chnlIdx].chnlCtrl &= ~MEASURE_PHASE_A_EN;
				}
			}
			if( PHASE_B_SELECT & phase)
			{
				if( true == state)
				{
					// ok
					retVal = 0;
					// set bit
					m_ipIcmData.chnlData[ chnlIdx].chnlCtrl |= MEASURE_PHASE_B_EN;
				}
				else if( false == state)
				{
					// ok
					retVal = 0;
					// clear bit
					m_ipIcmData.chnlData[ chnlIdx].chnlCtrl &= ~MEASURE_PHASE_B_EN;
				}
			}

			// if good selection
			if( 0 == retVal)
			{
				// select channel
				SelectChannel( chnlIdx);
				// write out new state
				m_uspIP[ CHANNEL_CONTROL] = m_ipIcmData.chnlData[ chnlIdx].chnlCtrl;
			}
		}
		// next channel
		chnlIdx++;
	}

	//
	return( retVal);
}

/**
 * Reads speed and acceleration for given channel.  Assumes data has already been latched.
 *
 * @param channel desired channel
 * @param freq    reported speed
 * @param dist    accumulated encoder pulses
 * @param dir     REVERSE_DIR or FORWARD_DIR
 * @param time    Sample time [us]
 *
 * @return 0 if successful, -1 if an error occurred
 */
int IpIcm::ReadWheelValues( int channel, float &freq, int &dist, uint32_t &dir, uint32_t &time)
{
	int			retVal=-1;
	unsigned	chnlIdx;
	unsigned	deltaTime, deltaCount, validCountBits;

	// scan for a matching channel
	chnlIdx = 0;
	while((chnlIdx < MAX_CHANNELS) && (0 != retVal))
	{
		// if this is the channel i am looking for
		if( channel == m_ipIcmData.chnlData[ chnlIdx].channel)
		{
			// success
			retVal = 0;

			// select channel
			SelectChannel( chnlIdx);

			// these are all the valid count bits
			validCountBits = (VALID_COUNT_BITS << 16) | 0x0ffff;

			// time shift data
			{
				m_ipIcmData.chnlData[ chnlIdx].count1 = m_ipIcmData.chnlData[ chnlIdx].count0;
				m_ipIcmData.chnlData[ chnlIdx].time1  = m_ipIcmData.chnlData[ chnlIdx].time0;
			}

			// determine frequency
			{
				// read new measurement data
				m_ipIcmData.chnlData[ chnlIdx].count0 = ReadActiveChannelCount();
				m_ipIcmData.chnlData[ chnlIdx].time0  = ReadActiveChannelTime();

				// calculate changes
				deltaCount = m_ipIcmData.chnlData[ chnlIdx].count0 - m_ipIcmData.chnlData[ chnlIdx].count1;
				deltaTime  = m_ipIcmData.chnlData[ chnlIdx].time0  - m_ipIcmData.chnlData[ chnlIdx].time1;
				// sign adjust for missing bits
				if( deltaCount & ((validCountBits + 1) >> 1))
				{
					// negative.  set upper bits
					deltaCount |= ~validCountBits;
				}
				else
				{
					// positive.  clear upper bits
					deltaCount &= validCountBits;
				}

				// calculate results
				if( 0 != deltaTime)
				{	// report frequency
					freq = ((float)((int)deltaCount)) / (((float)deltaTime) / IpIcm::m_refFreq);
				}
				else
				{	// report no frequency
					freq = 0;
				}
			}

			// update distance
			{
				// update accumulated distance
				m_ipIcmData.chnlData[ chnlIdx].accCount += (int)deltaCount;
				// report accumulated count
				dist = m_ipIcmData.chnlData[ chnlIdx].accCount;
			}

			// read direction
			{
				if( 0 != ReadActiveChannelDirection())
				{	// forward
					m_ipIcmData.chnlData[ chnlIdx].direction = FORWARD_DIR;
				}
				else
				{	// reverse
					m_ipIcmData.chnlData[ chnlIdx].direction = REVERSE_DIR;
				}
				// report direction
				dir = m_ipIcmData.chnlData[ chnlIdx].direction;
			}

			// read time in micro seconds
			{
				time = (uint32_t)(((float)ReadIpIcmFreeRunningCounter() / IpIcm::m_refFreq) * 1e6);
			}
		}
		// next channel
		chnlIdx++;
	}

	//
	return( retVal);
}

/**
 * Clears distance accumulator for given channel
 *
 * @param channel Channel to clear (-1=all)
 *
 * @return 0 if successful, -1 if an error occurred
 */
int IpIcm::ClearDistance( int channel)
{
	int			retVal=-1;
	int			iChan=0;
	unsigned	chnlIdx;

	chnlIdx = 0;
	while((chnlIdx < MAX_CHANNELS) && (0 != retVal))
	{
		if( -1 == channel)
		{
			// success
			retVal = 0;
			m_ipIcmData.chnlData[ chnlIdx].accCount = 0;
		}
		else if( iChan == channel)
		{
			// success
			retVal = 0;
			m_ipIcmData.chnlData[ chnlIdx].accCount = 0;
		}

		// another checked
		iChan++;
	}

	//
	return( retVal);
}

/**
 * Selects measure channel for reading
 *
 * @param channel Measure channel to select
 */
void IpIcm::SelectChannel( int channel)
{
	if( m_uspIP != NULL)
	{
		m_uspIP[ cHANNEL_SELECT] = channel;
	}
}

/**
 * Reads latched time value
 *
 * @return Time Value
 */
unsigned IpIcm::ReadIpIcmFreeRunningCounter( void)
{
	if( m_uspIP != NULL)
	{
		return( (m_uspIP[ TIME_HI] << 16) | m_uspIP[ TIME_LO]);
	}
	else
	{
		return( 0);
	}
}

/**
 * Reads Count value from active channel
 *
 * @return Channel Count Value
 */
unsigned IpIcm::ReadActiveChannelCount( void)
{
	if( m_uspIP != NULL)
	{
		return( ((m_uspIP[ CHAN_N_COUNT_HI] & VALID_COUNT_BITS) << 16) | m_uspIP[ CHAN_N_COUNT_LO]);
	}
	else
	{
		return( 0);
	}
}

/**
 * Reads Time value from active channel
 *
 * @return Channel Time Value
 */
unsigned IpIcm::ReadActiveChannelTime( void)
{
	if( m_uspIP != NULL)
	{
		return( (m_uspIP[ CHAN_N_TIME_HI] << 16) | m_uspIP[ CHAN_N_TIME_LO]);
	}
	else
	{
		return( 0);
	}
}

/**
 * Reads Time value from active channel
 *
 * @return 0 if reverse, != 0 if forward
 */
unsigned IpIcm::ReadActiveChannelDirection( void)
{
	if( m_uspIP != NULL)
	{
		return( m_uspIP[ CHAN_N_COUNT_HI] & DIRECTION_COUNT_BIT);
	}
	else
	{
		return( 0);
	}
}

/**
 * Returns the IP module's index on the carrier card
 *
 * @return The IP module's index on the carrier card
 */
int IpIcm::ModuleIndex() const
{
	return( m_moduleIdx);
}

/**
 * Returns the modules index on the IP carrier (0=IP slot 0, etc)
 *
 * @return The modules index on the IP carrier (0=IP slot 0, etc)
 */
inline int IpIcm::IpIndex() const
{
	return( m_ipIcmData.handle & 0x00FF);
}

/**
 * Checks to see if this IP module is generating an interrupt
 *
 * @return trus if the IP is generating an interrupt, false if not
 */
inline bool IpIcm::IsIrqActive() const
{
	bool	retVal = false;
	if( m_uspIP != NULL)
	{
		uint16_t	scr = m_uspIP[ OPERATION_CONTROL];

		if( scr & 0x8000)	retVal = true;
		else				retVal = false;
	}

	return( retVal);
}

/**
 * Acknowledges the interrupt generated by this module
 */
inline void IpIcm::AckIrq()
{
	uint16_t	junk;

	if( (m_ipInt != NULL) && (m_uspIP != NULL))
	{
		// Read the interrupt vector
		junk = m_ipInt[ 0];
		junk = m_ipInt[ 1];

		m_uspIP[ OPERATION_CONTROL] = INTER_CNT_LOAD_TRIGGER;
	}
}

/**
 * Enables the IP interrupt generation functionality
 *
 * @param irqVector Interrupt vector which the IP will be programmed to generate. Must
 *                  be unique among all IP modules on a carrier
 * @param samplePeriod
 *                  Number of milliseconds between consecutive data samples. Used to
 *                  calculate the interrupt latch counters
 */
inline void IpIcm::EnableIrq( uint8_t irqVector, uint32_t samplePeriod)
{
	unsigned	uBits=0;
	float		fSamplePeriod = (float)samplePeriod / 1000.0;

	Log( LOG_FN_ENTRY, "Enter IpIcm::EnableIrq( $%02hhX, %d)\n", irqVector, samplePeriod);

	if( m_uspIP != NULL)
	{
		// set up interrupt vector
		m_uspIP[ INTERRUPT_VECTOR] = irqVector;

		// turn off the interrupt
		uBits = m_uspIP[ SYSTEM_CONFIGURATION];
		uBits &= ~(EN_PERIODIC_DATA_SAMPLE | EN_PERIODIC_INTERRUPT | EN_SAM_TRIG_0 | EN_SAM_TRIG_1 | EN_SAM_TRIG_2 | EN_SAM_TRIG_3);
		m_uspIP[ SYSTEM_CONFIGURATION] = uBits;

		// set up interrupt vector
		m_uspIP[ INTERRUPT_VECTOR] = irqVector;
		// turn on the interrupt
		uBits = m_uspIP[ SYSTEM_CONFIGURATION];
		// if vector then enable interrupt
		if( irqVector)
		{
			// set up periodic sample/interrupt source
			uBits = ((unsigned)((IpIcm::m_refFreq * fSamplePeriod) + 0.5)) - 1;
			m_uspIP[ INTERR_COUNT_LO] = uBits & 0x0ffff;
			m_uspIP[ INTERR_COUNT_HI] = (uBits >> 16) & 0x07fff;
			Log( LOG_DEV_DATA, "Sampler: %d  = $%x%04x\n", uBits, (uBits >> 16) & 0x07fff, uBits & 0x0ffff);

			// set up system configuration register
			uBits |= (EN_PERIODIC_DATA_SAMPLE | EN_PERIODIC_INTERRUPT);
		}

		// write bits
		m_uspIP[ SYSTEM_CONFIGURATION] = uBits;

		// issue IACK to try and clear any old interrupts
		AckIrq();
	}

	Log( LOG_FN_ENTRY, "Enter IpIcm::EnableIrq( $%02hhX, %f)\n", irqVector, fSamplePeriod);
}

/**
 * Disables the IP interrupt generation functionality
 */
inline void IpIcm::DisableIrq()
{
	unsigned	uBits=0;

	Log( LOG_FN_ENTRY, "Enter IpIcm::DisableIrq()\n");

	if( m_uspIP != NULL)
	{
		// turn off the interrupt
		uBits = m_uspIP[ SYSTEM_CONFIGURATION];
		uBits &= ~(EN_PERIODIC_DATA_SAMPLE | EN_PERIODIC_INTERRUPT);
		m_uspIP[ SYSTEM_CONFIGURATION] = uBits;

		// set up interrupt vector
		m_uspIP[ INTERRUPT_VECTOR] = 0;

		AckIrq();
	}

	Log( LOG_FN_ENTRY, "Exit IpIcm::DisableIrq()\n");
}

/**
 * Initialize local data
 */
void IpIcm::InitializeIpIcmData( void)
{
	unsigned	uChan;

	Log( LOG_FN_ENTRY, "Enter IpIcm::InitializeIpIcmData()\n");

	// none counted yet
	m_totalMeasureChannels = 0;

	// does not exist
	m_ipIcmData.handle = -1;

	// no pointer
	m_ipIcmData.baseAddress = 0;

	// wipe out data
	for( uChan=0; uChan<MAX_CHANNELS; uChan++)
	{
		// this is my local channel number
		m_ipIcmData.chnlData[ uChan].channel = -1;
		// old values
		m_ipIcmData.chnlData[ uChan].count1 = 0;
		m_ipIcmData.chnlData[ uChan].time1 = 0;
		// new values
		m_ipIcmData.chnlData[ uChan].count0 = 0;
		m_ipIcmData.chnlData[ uChan].time0 = 0;
		// direction of rotation
		m_ipIcmData.chnlData[ uChan].direction = FORWARD_DIR;
		// accumulated count for distance
		m_ipIcmData.chnlData[ uChan].accCount = 0;
		// channel control variable (write only)
		m_ipIcmData.chnlData[ uChan].chnlCtrl = 0;
	}

	Log( LOG_FN_ENTRY, "Exit IpIcm::InitializeIpIcmData()\n");
}

/**
 * Gets a handle to the IP ICM for reference, if it exists.
 *
 * @param moduleIdx Occurance of module to find
 *
 * @return -1 if an error occurred; any other value represents a handle by
 *         which to reference this IP module
 */
int IpIcm::LocateIpIcm( unsigned moduleIdx)
{
	int		retVal;

	Log( LOG_FN_ENTRY, "Enter IpIcm::LocateIpIcm()\n");

	if( -1 == (retVal = m_ipCarrier->LocateIP( IP_ICM_MANUFACTURER, IP_ICM_MODEL, moduleIdx)))
	{
		Log( LOG_DEV_DATA, "INFO: LocateIP( $%x, $%x, %d) failed\n", IP_ICM_MANUFACTURER, IP_ICM_MODEL, moduleIdx);
	}

	Log( LOG_FN_ENTRY, "Exit IpIcm::LocateIpIcm()\n");
	return( retVal);
}

/**
 * Performs default setup on the specified IP ICM Module
 *
 * @param encoderPhasing
 *               The type of encoder phasing to use. (single = 1, qudarature = 0).
 *
 * @return 0 if successful, -1 if an error occurred
 */
int IpIcm::InitializeIpICM(bool encoderPhasing)
{
	int			retVal=-1;
	int			iChan, iCurrentChannel;
	unsigned	uBits;

	Log( LOG_FN_ENTRY, "Enter IpIcm::InitializeIpICM()\n");

	// if this one exists
	if( 0 <= m_ipIcmData.handle)
	{
		// get base address to reference this module
		if( 0 != (m_ipIcmData.baseAddress = m_ipCarrier->GetIpBaseIOAddress( m_ipIcmData.handle)))
		{
			m_uspIP = (volatile uint16_t*)m_ipIcmData.baseAddress;
			m_ipInt = (volatile uint16_t*)((char*)m_ipIcmData.baseAddress + 0x200);

			// turn off ttl io
			m_uspIP[ TTL_IO] = 0x00;

			// turn off all differential outputs and output overrides
			m_uspIP[ DIFF_OUTPUT_LO] = 0;
			m_uspIP[ DIFF_OUTPUT_HI] = 0;

			// set up differential direction bits
			// assume all inputs
			uBits = 0;
			//  select outputs
			uBits |= PHASE_C_0;
			uBits |= PHASE_C_1;
			uBits |= PHASE_C_2;
			uBits |= PHASE_C_3;
			uBits |= PHASE_C_4;
			uBits |= SYNC_OUT;
			// set up 20 direction bits
			m_uspIP[ DIFF_DIRECTION_LO] = uBits & 0x0ffff;
			m_uspIP[ DIFF_DIRECTION_HI] = (uBits >> 16) & 0x0f;

			// set up interrupt vector
			m_uspIP[ INTERRUPT_VECTOR] = 0;

			// control bits
			// all off
			TriggerLatches( 0);

			// initialize all channels
			// measure channels
			// update the individual channels first
			for( iChan=0; iChan<MAX_CHANNELS; iChan++)
			{
				// external reference
				m_ipIcmData.chnlData[ iChan].channel = m_totalMeasureChannels++;
				Log( LOG_DEV_DATA, "Valid Wheel Channel %d\n", m_ipIcmData.chnlData[ iChan].channel);

				// select
				switch( iChan)
				{
				case 0:		iCurrentChannel = CHANNEL_0_SELECT;		break;
				case 1:		iCurrentChannel = CHANNEL_1_SELECT;		break;
				case 2:		iCurrentChannel = CHANNEL_2_SELECT;		break;
				case 3:		iCurrentChannel = CHANNEL_3_SELECT;		break;
				default:	iCurrentChannel = -1;					break;
				}

				// if valid channel
				if( 0 <= iCurrentChannel)
				{
					// select channel
					SelectChannel( iCurrentChannel);

					// select input and output phases
					uBits = 0;
					uBits |= MEASURE_IN_0;												// phase a
					uBits |= MEASURE_OUT_0;												// gnd
					// switch based on if the machine is a dual phase or single phase
                    // if it is single phase
                    if(encoderPhasing)
                    {
                        uBits |= MEASURE_PHASE_A_EN;
                    }
                    // else it is quadrature
                    else
                    {
                        uBits |= MEASURE_PHASE_A_EN;
                        uBits |= MEASURE_PHASE_B_EN;
                    }
					m_ipIcmData.chnlData[ iCurrentChannel].chnlCtrl = uBits;	// update structure
					m_uspIP[ CHANNEL_CONTROL] = uBits;									// send

					// set up divide
					uBits = ((unsigned)2) - 1;
					m_uspIP[ CHAN_N_PHASE_DIV_LO] = uBits & 0x0ffff;
					m_uspIP[ CHAN_N_PHASE_DIV_HI] = (uBits >> 16) & 0x07fff;
				}
			}

			// single phase channel
			// select channel
			SelectChannel( CHANNEL_SINGLE_PHASE);

			// select input and output phases
			uBits = 0;
			uBits |= SINGLE_PHASE_IN_0;						// chan 0
			uBits |= SINGLE_PHASE_OUT_0;					// gnd
			m_uspIP[ CHANNEL_CONTROL] = uBits;

			// set up divide
			uBits = ((unsigned)2) - 1;
			m_uspIP[ CHAN_N_PHASE_DIV_LO] = uBits & 0x0ffff;
			m_uspIP[ CHAN_N_PHASE_DIV_HI] = (uBits >> 16) & 0x07fff;

			// set up system configuration register
			uBits = 0;
			uBits |= EN_SAM_TRIG_0;
			uBits |= EN_SAM_TRIG_1;
			uBits |= EN_SAM_TRIG_2;
			uBits |= EN_SAM_TRIG_3;
			//uBits |= EN_PERIODIC_INTERRUPT;
//			uBits |= (EN_PERIODIC_INTERRUPT | EN_PERIODIC_DATA_SAMPLE);
			uBits |= (EN_SAM_TRIG_0 | EN_SAM_TRIG_1 | EN_SAM_TRIG_2 | EN_SAM_TRIG_3);

			uBits |= IP_REF_CLOCK;
			// write bits
			m_uspIP[ SYSTEM_CONFIGURATION] = uBits;

			// at least one is on line
			retVal = 0;
		}
		else
		{
			// can not exist
			m_ipIcmData.handle = -1;
			Log( LOG_ERRORS, "\tGetIpBaseIOAddress( %d) failed\n", m_moduleIdx);
		}
	}

	Log( LOG_FN_ENTRY, "Exit IpIcm::InitializeIpICM()\n");
	return( retVal);
}

/**
 * Initializes data structure with readings from the IP ICM
 *
 * @return 0 if successful, -1 if an error occurred
 */
int IpIcm::SampleFirstReadings( void)
{
	int			retVal=0;
	unsigned	chnlIdx;

	Log( LOG_FN_ENTRY, "Enter IpIcm::SampleFirstReadings()\n");

	if( m_uspIP != NULL)
	{
		// sample all channels
		TriggerLatches( LATCH_0_TRIGGER | LATCH_1_TRIGGER | LATCH_2_TRIGGER | LATCH_3_TRIGGER | TIME_LATCH_TRIGGER);

		// sample all channels
		for( chnlIdx=0; chnlIdx<MAX_CHANNELS; chnlIdx++)
		{
			// select channel
			SelectChannel( chnlIdx);

			// get values
			m_ipIcmData.chnlData[ chnlIdx].count1 = ReadActiveChannelCount();
			m_ipIcmData.chnlData[ chnlIdx].time1 = ReadActiveChannelTime();
			// copy values so the same and valid
			m_ipIcmData.chnlData[ chnlIdx].count0 = m_ipIcmData.chnlData[ chnlIdx].count1;
			m_ipIcmData.chnlData[ chnlIdx].time0 = m_ipIcmData.chnlData[ chnlIdx].time1;
			// direction of rotation
			m_ipIcmData.chnlData[ chnlIdx].direction = ReadActiveDirection();
			// accumulated count for distance
			m_ipIcmData.chnlData[ chnlIdx].accCount = 0;
		}
	}

	Log( LOG_FN_ENTRY, "Exit IpIcm::SampleFirstReadings()\n");
	return( retVal);
}

/**
 * Triggers internal latches to latch current data for reading
 *
 * @param latches Operation/Control latches to activate
 */
void IpIcm::TriggerLatches( int latches)
{
	if( m_uspIP != NULL)
	{
		m_uspIP[ OPERATION_CONTROL] = latches;
	}
}

/**
 * Returns direction from active channel
 *
 * @return FORWARD_DIR if forward direction, REVERSE_DIR if reverse direction
 */
unsigned IpIcm::ReadActiveDirection( void)
{
	unsigned	uRet = FORWARD_DIR;

	if( m_uspIP != NULL)
	{
		if( 0 != (m_uspIP[ CHAN_N_COUNT_HI] & DIRECTION_COUNT_BIT))
		{
			uRet = FORWARD_DIR;
		}
		else
		{
			uRet = REVERSE_DIR;
		}
	}

	return( uRet);
}

