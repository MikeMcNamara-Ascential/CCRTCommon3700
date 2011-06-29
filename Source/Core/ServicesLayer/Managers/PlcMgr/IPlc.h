//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/PlcMgr/IPlc.h $
//  $Author: Cward $
//    $Date: 8/18/04 11:32p $
//
// Description:
//    PLC board interface definition
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
//    $Log: /Core/ServicesLayer/Managers/PlcMgr/IPlc.h $
// 
// 2     8/18/04 11:32p Cward
// Added channel bit ivar. Added new setter/getter functions. updated
// constructor and copy constructors to initialize.
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
// 3     3/12/03 12:43a Bmeinke
// Initial machine testing for Ford Chicago
// No longer read the input/output channel count from the config file:
// they are passed as arguments to Open()
//
// 2     1/18/03 3:41p Bmeinke
// N oreadl changes, just moved a couple of lines around (with out
// re-ordering them)
//
// 1     12/11/02 9:43p Bmeinke
// Initial check-in
//
//******************************************************************************

#ifndef _IPLC_H_INCLUDED_
#define _IPLC_H_INCLUDED_

#include <stdint.h>		// uint32_t, etc
#include "ILogger.h"
#include "HwDataMap.h"	// for portDir_t
#include "BepXmlUtils.h"
/**
 * PLC interface object definition
 *
 * @author Brian Meinke
 * @version 1
 * @since 12/11/02
 */
class IPlc : public ILogger
{
public:
	/**
	 * Default constructor
	 */
	IPlc() : ILogger(), m_fromPlcCount( 0), m_toPlcCount( 0), m_channelBits( 16)
	{
	}

	/**
	 * Copy constructor
	 *
	 * @param copy   IPlc object to be copied into *this
	 */
	IPlc( const IPlc &copy) : ILogger( copy), m_fromPlcCount( 0), m_toPlcCount( 0), m_channelBits( 16)
	{
		m_fromPlcCount = copy.m_fromPlcCount;
		m_toPlcCount = copy.m_toPlcCount;
		m_channelBits = copy.m_channelBits;
	}

	/**
	 * Destructor
	 */
	virtual ~IPlc()
	{
	}

	/**
	 * Opens a DF1 serial PLC board
	 *
	 * @param inputCount Number of inputs from the PLC we need to read
	 * @param outputCount
	 *                   Number of outputs to the PLC we need to write
	 * @param demoMode   Optional demo mode flag. If this argument is true, the Open() will
	 *                   ALWAYS succeed
	 *
	 * @return true if board successfully opened
	 *         false if any errors occur
	 */
	virtual bool Open(uint32_t inputCount, uint32_t outputCount, bool demoMode=false) = 0;

	/**
	 * Initializes the underlying PLC object
	 *
	 * @param plcSetup XML Parent Node containing PLCBoard  configuration
	 */
	virtual void Initialize( const XmlNode *plcSetup) = 0;

	/**
	 * Initializes the underlying logger interface of the DF1 PLC
	 *
	 * @param logCfg Exisiting ILogger to use to configure our logger interface
	 */
	virtual void Initialize( const ILogger &logCfg)
	{
		SetLocalEcho( logCfg.GetLocalEcho());
		SetProcessName( logCfg.GetProcessName());
		SetVerboseMask( logCfg.GetVerboseMask());
		SetAdditionalLogFileName( logCfg.GetAdditionalLogFileName());
		if( logCfg.GetLogStatus() == true)			EnableLogger();
	}
	
	/**
	 * Reads the input channel configuration from the given node map
	 *
	 * @param inputNodes XmlNodeMap containing the setup nodes for the PLC inputs
	 */
	virtual void ReadInputConfiguration( const XmlNodeMap &inputNodes) = 0;

	/**
	 * Reads the output channel configuration from the given node map
	 *
	 * @param outputNodes
	 *               XmlNodeMap containing the setup nodes for the PLC outputs
	 */
	virtual void ReadOutputConfiguration( const XmlNodeMap &outputNodes) = 0;
	
	/**
	 * Closes the object.
	 */
	virtual void Close() = 0;

	/**
	 * Reads the channel at the given index
	 *
	 * @param channelIdx Channel index to read
	 * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
	 * @return Value of the channel at the given index
	 */
	virtual uint32_t ReadChannel( uint32_t channelIdx, portDir_t portDir=PORT_DIR_IN) = 0;

	/**
	 * Reads from specified port on the channel at the given index
	 *
	 * @param channelIdx Channel number to read
	 * @param portIdx    Port (bit) index to read to
	 * @param logicLvl   Logic level for the specified port (positive true=1)
	 * @param portDir    Port type to read (PORT_DIR_IN or PORT_DIR_OUT, default=PORT_DIR_IN)
	 * @return 1 if port is "true"
	 *         0 if port is "off"
	 */
	virtual uint32_t ReadPort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, portDir_t portDir=PORT_DIR_IN) = 0;

	/**
	 * Writes to the the channel at the given index
	 *
	 * @param channelIdx Channel number to write to
	 * @param channelVal Value to write to the given channel
	 */
	virtual void WriteChannel( uint32_t channelIdx, uint32_t channelVal) = 0;
	
	/**
	 * Writes to specified port on the channel at the given index
	 *
	 * @param channelIdx Channel number to write to
	 * @param portIdx    Port (bit) index to write to
	 * @param logicLvl   Logic level for the specified port (positive true=1)
	 * @param portVal    Value to write to the given port
	 */
	virtual void WritePort( uint32_t channelIdx, uint16_t portIdx, uint16_t logicLvl, uint32_t portVal) = 0;

	/**
	 * Returns the number of channels for the output buffer (to PLC)
	 *
	 * @return The number of channels for the output buffer (to PLC)
	 */
	inline uint32_t GetToPlcCount() const;
	
	/**
	 * Returns the number of channels for the input buffer (from PLC)
	 *
	 * @return The number of channels for the input buffer (from PLC)
	 */
	inline uint32_t GetFromPlcCount() const;

	/**
	 * Returns the number of bits per channel for the PLC
	 *
	 * @return The number of bits per channel for the PLC
	 */
	inline uint32_t GetBitsPerChannel() const;

protected:
	/**
	 * Number of words to read from the plc
	 */
	uint32_t	m_fromPlcCount;
	
	/**
	 * Number of words to write to the plc
	 */
	uint32_t	m_toPlcCount;

	/**
	 * Number of bits per channel for PLC
	 */
	uint32_t	m_channelBits;
};


/**
 * Returns the number of channels for the output buffer (to PLC)
 *
 * @return The number of channels for the output buffer (to PLC)
 */
inline uint32_t IPlc::GetToPlcCount() const		{ return( m_toPlcCount);	};

/**
 * Returns the number of channels for the input buffer (from PLC)
 *
 * @return The number of channels for the input buffer (from PLC)
 */
inline uint32_t IPlc::GetFromPlcCount() const	{ return( m_fromPlcCount);	};

/**
 * Returns the number of bits per channel for the PLC
 *
 * @return The number of bits per channel for the PLC
 */
inline uint32_t IPlc::GetBitsPerChannel() const	{ return( m_channelBits);	};


#endif // _IPLC_H_INCLUDED_

