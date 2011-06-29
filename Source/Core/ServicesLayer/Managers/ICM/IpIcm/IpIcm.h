//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/ICM/IpIcm/IpIcm.h $
//  $Author: Cward $
//    $Date: 12/21/03 6:28p $
//
// Description:
//    IP ICM Implementation
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
//    $Log: /Core/ServicesLayer/Managers/ICM/IpIcm/IpIcm.h $ 
// 
// 1     12/21/03 6:28p Cward
// 
// 2     10/24/03 1:20p Khykin
// Updated to work with machines without encoder phasing.
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
// 1     6/12/03 4:14p Khykin
// Initial checkin.
// 
// 2     5/27/03 8:23a Khykin
// Adding latest changes.
// 
// 1     4/30/03 4:43p Bmeinke
// Migration to new personalized configuration management structure
// 
// 4     3/10/03 7:24p Bmeinke
// EnableIrq() now takes a samplePeriod argument to use for calculating
// the necessary interrupt counter values required to sample data with the
// period given by the samplePeriod argument
// 
// 3     3/10/03 6:39p Bmeinke
// Moved method bodies to new IpIcm.cpp file
// 
// 2     2/25/03 6:32p Bmeinke
// Changed class to only manage a single IP ICM
// Added Initialize() method to perform initialization
//
//******************************************************************************

class IpIcm;

#ifndef _IP_ICM_INCLUDED_
#define _IP_ICM_INCLUDED_

#include <vector>

#include "Pci40.h"		// GreenSpring IP carrier card
#include "ILogger.h"	// Logger interface
#include "IpIcmDefs.h"	// Constants

#define		IP_ICM_MANUFACTURER				0x0b
#define		IP_ICM_MODEL					0x5a

typedef vector< IpIcm*>						IpIcmList_t;
typedef IpIcmList_t::iterator				IpIcmListItr_t;
typedef IpIcmList_t::const_iterator			IpIcmListCItr_t;

/**
 * Altera IP ICM object
 * 
 * @author Brian Meinke
 * @version 1
 * @since Feb 27, 2003
 */
class IpIcm : public ILogger
{
public:

	/**
	 * reference frequency
	 */
	const static double	m_refFreq;

	/**
	 * Constructor
	 */
	IpIcm();

	/**
	 * Destructor
	 */
	~IpIcm();

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
    bool Initialize( const Pci40 *ipCarrier, int moduleIdx, bool encoderPhasing = 0);

	/**
	 * Initializes our logger interface object
	 * 
	 * @param loggerCfg The logger object whose configuration we want to copy
	 */
	void Initialize( const ILogger &loggerCfg);

	/**
	 * Ip Icm process
	 * 
	 * @return 0 if successful, -1 if an error occurred
	 */
	int Run( void);

	/**
	 * Triggers channel latches
	 * 
	 * @param channel Channel to latch: -1 = All Channels (including Time)
	 *                or a positive channel number
	 * 
	 * @return 0 if successful, -1 if an error occurred
	 */
	int LatchChannelValues( int channel);
	
	/**
	 * Reads time and count from given channel
	 * 
	 * @param channel Channel to read from
	 * @param time    Pointer to where to store channel time value
	 * @param count   Pointer to where to store channel count value
	 * 
	 * @return 0 if successful, -1 if an error occurred
	 */
	int ReadChannelValues( int channel, uint32_t &time, uint32_t &count);
	
	/**
	 * En/Dis-ables periodic sample
	 * 
	 * @param state true to enable periodic sample, false to disable periodic sample
	 */
	void PeriodicSample( bool state);
	
	/**
	 * En/Dis-able selected phase for selected channel
	 * 
	 * @param channel channel to change
	 * @param phase   PHASE_A_SELECT or PHASE_B_SELECT (can be or'd together)
	 * @param state   true to enable, false to disable
	 * 
	 * @return 0 if successful, -1 if an error occurred
	 */
	int PhaseControl( int channel, uint32_t phase, bool state);
	
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
	int ReadWheelValues( int channel, float &freq, int &dist, uint32_t &dir, uint32_t &time);
	
	/**
	 * Clears distance accumulator for given channel
	 * 
	 * @param channel Channel to clear (-1=all)
	 * 
	 * @return 0 if successful, -1 if an error occurred
	 */
	int ClearDistance( int channel);

	/**
	 * Selects measure channel for reading
	 * 
	 * @param channel Measure channel to select
	 */
	void SelectChannel( int channel);
	
	/**
	 * Reads latched time value
	 * 
	 * @return Time Value
	 */
	unsigned ReadIpIcmFreeRunningCounter( void);
	
	/**
	 * Reads Count value from active channel
	 * 
	 * @return Channel Count Value
	 */
	unsigned ReadActiveChannelCount( void);
	
	/**
	 * Reads Time value from active channel
	 * 
	 * @return Channel Time Value
	 */
	unsigned ReadActiveChannelTime( void);
	
	/**
	 * Reads Time value from active channel
	 * 
	 * @return 0 if reverse, != 0 if forward
	 */
	unsigned ReadActiveChannelDirection( void);
	
	/**
	 * Returns the IP module's index on the carrier card
	 * 
	 * @return The IP module's index on the carrier card
	 */
	int ModuleIndex() const;

	/**
	 * Returns the modules index on the IP carrier (0=IP slot 0, etc)
	 * 
	 * @return The modules index on the IP carrier (0=IP slot 0, etc)
	 */
	inline int IpIndex() const;

	/**
	 * Checks to see if this IP module is generating an interrupt
	 * 
	 * @return trus if the IP is generating an interrupt, false if not
	 */
	inline bool IsIrqActive() const;
	
	/**
	 * Acknowledges the interrupt generated by this module
	 */
	inline void AckIrq();

	/**
	 * Enables the IP interrupt generation functionality
	 * 
	 * @param irqVector Interrupt vector which the IP will be programmed to generate. Must
	 *                  be unique among all IP modules on a carrier
	 * @param samplePeriod
	 *                  Number of milliseconds between consecutive data samples. Used to
	 *                  calculate the interrupt latch counters
	 */
	inline void EnableIrq( uint8_t irqVector, uint32_t samplePeriod);

	/**
	 * Disables the IP interrupt generation functionality
	 */
	inline void DisableIrq();

protected:

	/**
	 * Initialize local data
	 */
	void InitializeIpIcmData( void);
	
	/**
	 * Gets a handle to the IP ICM for reference, if it exists.
	 * 
	 * @param moduleIdx Occurance of module to find
	 * 
	 * @return -1 if an error occurred; any other value represents a handle by 
	 *         which to reference this IP module
	 */
	int LocateIpIcm( unsigned moduleIdx);
	
    /**
     * Performs default setup on the specified IP ICM Module
     * 
     * @param encoderPhasing
     *               The type of encoder phasing to use. (single = 0, qudarature = 1).
     * 
     * @return 0 if successful, -1 if an error occurred
     */
    int InitializeIpICM(bool encoderPhasing);
	
	/**
	 * Initializes data structure with readings from the IP ICM
	 * 
	 * @return 0 if successful, -1 if an error occurred
	 */
	int SampleFirstReadings( void);
	
	/**
	 * Triggers internal latches to latch current data for reading
	 * 
	 * @param latches Operation/Control latches to activate
	 */
	void TriggerLatches( int latches);
	
	/**
	 * Returns direction from active channel
	 * 
	 * @return FORWARD_DIR if forward direction, REVERSE_DIR if reverse direction
	 */
	unsigned ReadActiveDirection( void);

private:

	volatile uint16_t	*m_uspIP;

	volatile uint16_t	*m_ipInt;

	/**
	 * total channels counted
	 */
	int					m_totalMeasureChannels;

	/**
	 * Data pertinent for each ICM module present
	 */
	IpIcmData_t			m_ipIcmData;

	/**
	 * Carrier card on which this IP module resides
	 */
	Pci40				*m_ipCarrier;

	/**
	 * The module index of our ICM IP module on the IP carriere card
	 */
	int					m_moduleIdx;

};


#endif // _IP_ICM_INCLUDED_

