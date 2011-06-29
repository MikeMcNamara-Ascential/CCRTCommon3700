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
#ifndef GMLossCompensation_h
#define GMLossCompensation_h
//-----------------------------------------------------------------------------
#include "LossCompensationTC.h"
#include "BepTimer.h"
#include "BepSync.h"
#include "IQnxDataServer.h"

//-----------------------------------------------------------------------------
#define LOSSCOMP_PULSE_CODE     SYSTEM_PULSE+13
#define NEW_COMMAND_PULSE       14

//-----------------------------------------------------------------------------
class GMLossCompensation : public LossCompensationTC
{
public:
    /** Class Constructor */
    GMLossCompensation();
    /** Class Destructor */
    virtual ~GMLossCompensation();
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
#endif //GMLossCompensation_h
