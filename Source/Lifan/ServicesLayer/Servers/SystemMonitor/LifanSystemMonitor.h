//******************************************************************************
//
// $Archive$
// $Author$
//   $Date$
//
// Description:
//    Lifan System Monitor Task
//
//==============================================================================
//
//     Copyright 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log$
// 
//******************************************************************************

#ifndef LifanSystemMonitor_h
#define LifanSystemMonitor_h

/*******************
 Forward Definitions
 ******************/
class LifanSystemMonitor;

#include <fcntl.h>
#include "SystemMonitor.h"

/**
 * Pulse value used to indicate that it is time to update the test conditions
 * for the system.
 */
#define UPDATE_TEST_CONDITIONS_PULSE        40

/**
 * Addition control data needed for the China System Monitor
 */
class LifanControlData : public ControlData
{
public:

    /**
     * Class constructor
     */
     LifanControlData();

    /**
     * Class destructor
     */
    virtual ~LifanControlData();

    /**
     * Flag that tells if the roll drives are enabled
     */ 
    bool rollDrivesEnabled;
};

/**
 * Lifan System Monitor class.
 * Handles system events: Start Test push button, ABORT, VIN processing, etc
 *
 * @author Ross Wiersema
 */
class LifanSystemMonitor : public SystemMonitor
{
public:
    /**
     * Default constructor. Set debug to false, channel ID's to -1, etc
     */
    LifanSystemMonitor();

    /**
     * Constructor from command line args
     *
     * @param argc   Number of command line arguments
     * @param argv   Array of command line args
     */
    LifanSystemMonitor( int argc, char *argv[]);

    /**
     * Class destructor
     */
    virtual ~LifanSystemMonitor();

    /**
     * Process system changes until a terminate message is received.
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);

protected:
    /**
     * method for component initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    
    /**
     * Overloaded BepServer Publish method which provides the ability
     * to handle the fault level messages that were requested.
     *
     * @param node the published information to deal with.
     * @return the status of the operation.
     */
    virtual const std::string Publish(const XmlNode *node);

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         Success, Fail
     */
    virtual const std::string Register(void);

    /**
     * Reads current values of control data items. The m_subscribedData data is
     * updated asynchronously via subscription notifications from the servers to
     * whoim we are subscribed
     *
     * @param ctrl   Control Data structure to fill with values
     */
    virtual void GetCurrentControlData( ControlData *ctrl);
    
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
     * 
     * @return The status of the operation.
     */
    INT32 GetWheelSpeeds(float wheelSpeed[]);

    /**
     * Will hadle pulse if overriden.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * 
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
    
    /**
     * Perform a check to determine if the conditions are correct to start a
     * Vehicle test and or set the DriveAxle.
     * 
     * @param ctrl   Current value of control data needed for system control events
     */
    void UpdateTestConditions( ControlData *ctrl);

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
     * @return a ControlData object.
     */
    virtual ControlData* ControlAllocate() const;

    /**
     * Frees a ControlData structure previously allocated via ControlAllocate
     *
     * @param controlData
     *               ControlData structure previously allocated via ControlAllocate
     */
    virtual void ControlFree(ControlData* &controlData) const;

 private:
    /**
     * Location of the wheel speeds reported by the ICM.
     */
    string              m_ICMSpeedLocation;
    
    /**
     * The number of rollers that are being controlled.
     */
    INT32               m_rollerCount;

    /**
     * The current drive axle.  This is cleared out at the beginning
     * of every test and then is set based on which weels are the
     * drive wheels.
     */
    std::string         m_currentDriveAxle;
    
    /**
     * Timer object used to send the SystemMonitor a pulse after the
     * vehicle present filter time has elapsed.
     */
    BepTimer            m_updateTestConditionsTimer;

    /**
     * The number of mili-seconds to wait before updateing the test
     * conditions.
     */
    double              m_updateTestConditionsTime; 

    /**
     * Flag that is used to indicate that a vehicle test has been
     * started.  It is cleared when the retainers are lowered.
     */
    bool                m_startVehicleTestFlag;
    /**
     * this flag is used to prevent the prompt from changing to 
     * "press start button" at the end of a test.
     * (Koreans call this prompt "select test")
     */
    bool                m_testWasRun;

    /**
     * Flag used to tell if we should clear the test results when the
     * rolls go down.
     */ 
    bool                m_clearResultsOnRollsDown;
};

#endif    // LifanSystemMonitor_h

