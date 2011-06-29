//******************************************************************************
//
// $Archive: /CMake Core/Source/Ford/ServicesLayer/Servers/SystemMonitor/FordSystemMonitor.h $
// $Author: Bmeinke $
//   $Date: 3/20/06 10:36a $
//
// Description:
//    System Monitor Task
//
//
//==============================================================================
//
//     Copyright © 2009- Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//
//******************************************************************************

#ifndef FOTON_SYSTEM_MONITOR_INCLUDED
#define FOTON_SYSTEM_MONITOR_INCLUDED

/*******************
 Forward Definitions
 ******************/
class FotonSystemMonitor;

				
/*******************
 include files
 ******************/
#include "SystemMonitor.h"

/**
 * Base 3700 extension of the control data
 */
class Base3700ControlData : public ControlData
{
public:	
	/**
	 * Allocate a new ControlData object. Required due to non-public
	 * constructor
	 *
	 * @return A new ControlData object
	 */
	static Base3700ControlData* Allocate()
	{
		return( new Base3700ControlData);
	}
	
	/**
	 * Destructor
	 */
	virtual ~Base3700ControlData()
	{
	}

	/**
	 * Assignment operator
	 * 
	 * @param rhs    Object to be copied
	 * 
	 * @return this
	 */
	Base3700ControlData& operator=( const Base3700ControlData& rhs)
	{
		Assign( rhs);

		return( *this);
	}
	
	/**
	 * Flag that is used to indicate if the rolls tester is ok to roll.
	 * @since Version 1.0
	 */
	bool 		okToRoll;

	/**
	 * Number of tests since the last loss compensation
	 */
	int			testsSinceLossComp;

	/**
	 * Bit indicating that our inter-loss comp cycle count has been exceeded
	 */
	bool		cycleLimitExceeded;

protected:

	/**
	 * Assigns the value of rhs to this
	 * 
	 * @param rhs    ControlData object to be copied
	 * 
	 * @return this
	 */
	virtual ControlData& Assign( const ControlData& rhs)
	{
		const Base3700ControlData *ctrl = (const Base3700ControlData*)(&rhs);
		this->okToRoll = ctrl->okToRoll;
		this->testsSinceLossComp = ctrl->testsSinceLossComp;
		this->cycleLimitExceeded = ctrl->cycleLimitExceeded;

		return( ControlData::Assign( rhs));
	}
	
	/**
	 * Constructor
	 */
	Base3700ControlData() : ControlData(),
		okToRoll( false), testsSinceLossComp(0), cycleLimitExceeded( false)
	{
	}
};


/**
 * Base 3700 System Monitor class.
 * Handles system events: Start Test push button, ABORT, VIN processing, etc
 *
 * @author Kevin Hykin
 * @version 1.0
 */
class FotonSystemMonitor : public SystemMonitor
{
public:
	/**
	 * Default constructor. Set debug to false, channel ID's to -1, etc
	 */
	FotonSystemMonitor();

	/**
	 * Constructor from command line args
	 *
	 * @param argc   Number of command line arguments
	 * @param argv   Array of command line args
	 */
	FotonSystemMonitor( int argc, char *argv[]);

	/**
	 * Class destructor
	 */
	virtual ~FotonSystemMonitor();

	/**
	 * method for component initialization.
	 *
	 * @param document Reference to a parsed configuration document.
	 * @since Version 1.0
	 */
	void Initialize(const XmlNode *document);
	
	/**
	 * Request to write a data tag
	 *
	 * @param tag    Tag to be written
	 * @param value  Data to be written
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 2.0
	 */
	virtual const std::string Write(const std::string &tag, const std::string &value);
	/**
	 * Request to write a data tag
	 *
	 * @param node    Data to be written
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 2.0
	 */
	virtual const std::string Write(const XmlNode *node);
    
    /**
	 * Request to Perform an action
	 *
	 * @param tag    Action to be performed
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 2.0
	 */
	virtual const std::string Command(const std::string &tag, const std::string &value);
	
    /**
	 * Request to Perform an action
	 *
	 * @param node    Action to be performed
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 2.0
	 */
	virtual const std::string Command(const XmlNode *node);
	
    /**
     * Overloaded BepServer Publish method which provides the ability
     * to handle the fault level messages that were requested.
     *
     * @param node the published information to deal with.
     * @return the status of the operation.
     */
    virtual const std::string Publish(const XmlNode *node);

    /**
	 * Reads current values of control data items. The m_subscribedData data is
	 * updated asynchronously via subscription notifications from the servers to
	 * whoim we are subscribed
	 *
	 * @param ctrl   Control Data structure to fill with values
	 */
	virtual void GetCurrentControlData( ControlData *ctrl);
	
	/**
	 * Monitors for system events such as ABORT, CAL, SIM, START_TEST, etc
	 *
	 * @param ctrl   Current value of control data needed for system control events
	 */
	virtual void UpdateControl( ControlData *ctrl);

    /**
     * Update the VehicleTest with the commanded phase if capable of performing a test.
     * 
    * @param ctrl   Current value of control data needed for system control events
     */
    virtual void UpdateCommandPhase( ControlData *ctrl);
	

protected:
    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         Success, Fail
     */
    virtual const string Register(void);

	/**
	 * Changes state of Drivers Monitor and put up some prompts
	 *
	 * @param ctrl Current value of control data needed for system control events
	 */
	virtual void UpdateDriverMonitor(ControlData *ctrl);

	/**
	 * Check to determine if the FACTS test is good to go and publish the OkToRoll tag
	 * to indicate the status.
	 * 
	 * @param ctrl Current value of control data needed for system control events
	 */
	void CheckOkToRoll( ControlData *ctrl);

#if 0
    /**
	 * Test ABORT logic
	 *
	 * @param ctrl Current value of control data needed for system control events
	 */
	virtual void CheckAbort( ControlData *ctrl);
#endif 
    /**
     * Testing control logic
     *
     * @param ctrl Current value of control data needed for system control events
     */
    virtual void CheckTesting( ControlData *ctrl);

    /**
	 * Read the current wheel speeds from the system.
	 * 
	 * @param wheelSpeed The wheel speed array to populate with the current speeds.
	 * @return The status of the operation.
	 */
    INT32 GetWheelSpeeds(float wheelSpeed[]);

	/**
	 * Perform a check to determine if the conditions are correct to start a
	 * Vehicle test and or set the DriveAxle.
	 * 
	 * @param ctrl   Current value of control data needed for system control events
	 */
	virtual void UpdateDriveAxle( ControlData *ctrl);

    /**
     * Set the number of rollers that the machine is equipped with.
     * 
     * @param rollerCount
     *               The number of rollers that the machine is equipped with.
     */
    void SetRollerCount( INT32 rollerCount);
    
    /**
     * Get the number of rollers that the machine is equipped with.
     * 
     * @return The number of rollers that the machine has.
     */
    INT32 GetRollerCount( void);

	/**
	 * Allocates a new ControlData structure to be used to maintain the 
	 * current state of the system
	 * 
	 * @return 
	 */
	virtual ControlData* ControlAllocate() const;

	/**
	 * Frees a ControlData structure previously allocated via ControlAllocate
	 * 
	 * @param controlData
	 *               ControlData structure previously allocated via ControlAllocate
	 */
	virtual void ControlFree(ControlData* &controlData) const;
	
    /**
     * The last commanded phase that was sent.
     * @since Version 2.0
     */
    std::string m_commandedPhase;
    
    /**
     * Location of the wheel speeds reported by the ICM.
     * @version 1.2
     */
    string              m_ICMSpeedLocation;
  	
    /**
	 * The number of rollers that are being controlled.
	 * @since Version 1.0
	 */
	INT32               m_rollerCount;

    /**
     * The current drive axle.  This is cleared out at the beginning
     * of every test and then is set based on which weels are the
     * drive wheels.
     */
    std::string         m_currentDriveAxle;

	/**
	 * Maximum number of tests between consecutive loss compensation cycles
	 */
	int					m_testsBetweenLossComp;

private:
    /** Flag indicating if a test start condition has been satisfied */
    bool                m_startTestArmed;
};

#endif	// FOTON_SYSTEM_MONITOR_INCLUDED

