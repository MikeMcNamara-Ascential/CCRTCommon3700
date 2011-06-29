//*************************************************************************
// Description:
// This file contains the functions necessary to perform a loss compensation
// test on the rolls tester.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//=============================================================================
#ifndef PhotonLossCompensation_h
#define PhotonLossCompensation_h
//-----------------------------------------------------------------------------
#include "LossCompensationTC.h"
#include "BepTimer.h"
#include "BepSync.h"
#include "IQnxDataServer.h"

//-----------------------------------------------------------------------------
#define LOSSCOMP_PULSE_CODE     SYSTEM_PULSE+13
#define NEW_COMMAND_PULSE       14

//-----------------------------------------------------------------------------
class PhotonLossCompensation : public LossCompensationTC
{
public:
    /** Class Constructor */
    PhotonLossCompensation();
    /** Class Destructor */
    virtual ~PhotonLossCompensation();
    /**
     * Initialize the test component using the given file.
     * 
     * @param fileName File to use for initializing the test component.
     */
    virtual void Initialize(const string &fileName);
    /**
     * Initialize the test component.
     * 
     * @param config Configuration data for initializing the test component.
     */
    virtual void Initialize(const XmlNode *config);
    /**
     * <b>Description:</b>
     * Handle the specified pulse.  This method simply calls
     * BepComponent::HandlePulse().
     *
     * @param code  The pulse code to process.
     * @param value The pulse value to process.
     *
     * @return BEP_STATUS_SUCCESS if the pulse was successfully handled.
     *         BEP_STATUS_FAILURE if the pulse was not handled.
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
    /**
     * Register the component with the system.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Command the component to perform any system registration functions necessary.  The component testing status will
     * be set to TEST_STATUS_IN_PROGRESS.  The base class will then be called to complete system registration.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateTestStatus() </li>
     *      <li> BepServer::Register() </li>
     *      </ul>
     *
     * @return BEP_SUCCESS_RESPONSE if the registration was successful.
     *         BEP_FAILURE_RESPONSE if the registration was not successful.
     */
    virtual const string Register(void);
    /**
     * Process an incoming command.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Process an incoming command.  Any commands other than objectives will be passed to the base class for processing.
     * Commands will only be processed if the component status is not set to BEP_DEACTIVATE_STATUS.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetStatus() </li>
     *      <li> SetCommand() </li>
     *      <li> BepServer::Command() </li>
     *      </ul>
     * 
     * @param temp   The command to be processed.
     * 
     * @return Result of processing the command.  BEP_UNAVAILABLE_RESPONSE will be returned if the current status is
     *         BEP_DEACTIVATE_STATUS.
     */
    virtual const string Command(const XmlNode *temp);

protected:
    /**
     * Initialize the test conditions.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step is responsible for validating that the machine is ready to perform a loss compensation cycle.
     * If any condition is not met, the operator will be instructed to take the appropriate actions to place the
     * machine in the correct state to perform the loss compensation sequence.
     * <p>
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> LossCompensationSwitchStatus - System tag for reading the state of the loss compensation switch. </li>
     *      <li> VehiclePresent - System tag to use for determining if a vehicle is currently on the machine. </li>
     *      <li> Zerospeed - System tag for determining of the rollers are at zero speed. </li>
     *      <li> RaiseRetainers - System tag for determining if the retaining devices are in position. </li>
     *      <li> StartLossCompensation - System tag for determining of a loss compensation cycle has started. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> VehiclePresentPromptBox - Prompt box in which to display the vehicle present prompt. </li>
     *      <li> VehiclePresentPrompt - Prompt tag to use for informing the operator a vehicle is on the machine. </li>
     *      <li> ZerospeedPromptBox - Prompt box in which to display the zero speed prompt. </li>
     *      <li> ZerospeedPrompt - Prompt tag to use for informing the operator the rollers are not at zero speed. </li>
     *      <li> RaiseRetainersPromptBox - Prompt box in which to display the raise retainers prompt. </li>
     *      <li> RaiseRetainersPrompt - Prompt tag for instructing the operator to raise the retaining rolls. </li>
     *      <li> StartLossCompensationBox - Prompt box in which to display the start loss compensation prompt. </li>
     *      <li> StartLossCompensationPrompt - Prompt tag for instructing the operator to start the loss 
     *                                         compensation sequence. </li>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> LossCompStatusCheck() </li>
     *      <li> TimeRemaining() </li>
     *      <li> CheckAbort() </li>
     *      <li> ReadSubscribeData() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> Engage() </li>
     *      <li> UpdateResult() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
     * @return The result of ensuring the machine is in the correct state to perform the loss compensation sequence.
     */
    virtual const string TestStepInitialize(void);
    /**
     * Finish up the loss compensation test sequence.
     * <p><b>Category:</b> Test Step
     * <p><b>Description:</b><br>
     * This test step is responsible for setting the machine configuration for normal operation.  It also determines 
     * the overall result of the test sequence.
     * <p>
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SendTestResult() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> SendOverallResult() </li>
     *      <li> SetStatus() </li>
     *      <li> UpdatePrompts() </li>
     *      <li> RemovePrompts() </li>
     *      <li> Disengage() </li>
     *      <li> TimeRemaining() </li>
     *      <li> SystemCommand() </li>
     *      <li> GetDataTag() </li>
     *      </ul>
     * 
     * @return The result of performing the finish up processing.
     */
    virtual const string TestStepFinishUp(void);

private:
    /**
     * Execute the specified command.
     * 
     * @param command Command to be executed.
     * 
     * @return Result of executing the command.
     */
    INT32 ExecuteCommand(const XmlNode *command);
    /** Next objective to execute */
    XmlNode *m_nextCommand;
    /** Timer to check for new incoming commands */
    BepTimer m_newCommandTimer;
    /** Mutex to coordinate access to the next command  variable */
    BepMutex m_nextCommandMutex;
    /** Interface to the QNX data server */
    IQnxDataServer m_qnxDataServer;
};


//-----------------------------------------------------------------------------
#endif //PhotonLossCompensation_h
