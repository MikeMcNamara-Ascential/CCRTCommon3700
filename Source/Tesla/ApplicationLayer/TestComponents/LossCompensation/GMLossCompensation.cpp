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
#include "GMLossCompensation.h"

//-----------------------------------------------------------------------------
GMLossCompensation::GMLossCompensation() : LossCompensationTC(), m_nextCommand(NULL)
{   // Nothing special to do
}

//-----------------------------------------------------------------------------
GMLossCompensation::~GMLossCompensation()
{   // Get rid of the next command if it has been stored
    if(m_nextCommand != NULL)  delete m_nextCommand;
    m_nextCommand = NULL;
}

//-----------------------------------------------------------------------------
void GMLossCompensation::Initialize(const string &fileName)
{
    GenericTC::Initialize(fileName);
}

//-----------------------------------------------------------------------------
void GMLossCompensation::Initialize(const XmlNode *config)
{   // Call the base class to begin the initialization
    LossCompensationTC::Initialize(config);
    Log(LOG_FN_ENTRY, "GMLossCompensation::Initialize() - Enter");
    // Initialize the timer
    UINT64 timerRate = GetParameterInt("NewCommandCheckDelay");
    // Start the timer
    m_newCommandTimer.SetPulseCode(LOSSCOMP_PULSE_CODE);
    m_newCommandTimer.SetPulseValue(NEW_COMMAND_PULSE);
    m_newCommandTimer.Initialize(GetProcessName(), mSEC_nSEC(timerRate), mSEC_nSEC(timerRate));
    m_newCommandTimer.Stop();
    Log(LOG_FN_ENTRY, "GMLossCompensation::Initialize() - Exit");
}

//-----------------------------------------------------------------------------
const INT32 GMLossCompensation::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 retVal = BEP_STATUS_ERROR;
    Log(LOG_DETAILED_DATA, "GMLossCompensation::HandlePulse(code: %d, value: %d) - Enter", code, value);
    switch(code)
    {
    case LOSSCOMP_PULSE_CODE:
        switch(value)
        {
        case NEW_COMMAND_PULSE:  //Check for a new command
            if(m_nextCommand != NULL)
            {   // Make sure we are the only one accessing the next command
                if(EOK == m_nextCommandMutex.Acquire())
                {
                    XmlNode *cmd = m_nextCommand->Copy();
                    delete m_nextCommand;
                    m_nextCommand = NULL;
                    m_nextCommandMutex.Release();
                    retVal = ExecuteCommand(cmd);
                }
                else
                {   // Could not access the next command
                    retVal = BEP_STATUS_FAILURE;
                }
            }
            else
            {   // No command to execute, just return success
                retVal = BEP_STATUS_SUCCESS;
            }
        }
        break;

    default:   // Call the base class to handle the pulse
        retVal = BepServer::HandlePulse(code, value);
        break;
    }
    Log(LOG_DETAILED_DATA, "GMLossCompensation::HandlePulse(code: %d, value: %d) - Exit: %d", code, value, retVal);
    return retVal;
}

//-----------------------------------------------------------------------------
const string GMLossCompensation::Register(void)
{   // Call the base class to begin the registration
    Log(LOG_FN_ENTRY, "GMLossCompensation::Register() - Enter");
    string result = LossCompensationTC::Register();
    // Start the timer
    m_newCommandTimer.Start();
    // Connect to the QNX data server
    Log(LOG_DEV_DATA, "Connected to QNX Data Server: %s", ConvertStatusToResponse(m_qnxDataServer.ConnectToQnxDataServer()).c_str());
    Log(LOG_FN_ENTRY, "GMLossCompensation::Register() - Exit");
    return result;
}

//-----------------------------------------------------------------------------
const string GMLossCompensation::Command(const XmlNode *temp)
{
    Log(LOG_FN_ENTRY, "GMLossCompensation::Command() - Enter");
    // Add the command to the next command
    string result = BEP_ERROR_RESPONSE;
    if(EOK == m_nextCommandMutex.Acquire())
    {   // Get rid of the existing command if there is one saved
        if(m_nextCommand != NULL)  delete m_nextCommand;
        // Store the new item
        m_nextCommand = temp->Copy();
        m_nextCommandMutex.Release();
        result = BEP_SUCCESS_RESPONSE;
    }
    else
    {
        Log(LOG_ERRORS, "Could not store new command!");
        result = BEP_FAILURE_RESPONSE;
    }
    Log(LOG_FN_ENTRY, "GMLossCompensation::Command() - Exit");
    return result;
}

//-----------------------------------------------------------------------------
const string GMLossCompensation::TestStepFinishUp(void)
{   // Wait for the rollers to stop
    CheckZeroSpeed();
    // Call the base class to do the finish up processing
    string result = LossCompensationTC::TestStepFinishUp();
    // Clear the Loss comp in progress tag
    Log(LOG_DEV_DATA, "Cleared LossCompInProgress flag in qnx data server: %s", 
        ConvertStatusToResponse(m_qnxDataServer.UpdateQnxDataServerVariable("LossCompInProgress", "0")).c_str());
    return result;
}

//-----------------------------------------------------------------------------
inline INT32 GMLossCompensation::ExecuteCommand(const XmlNode *command)
{   // Reset the status to test in progress
    ResetOverallResult();
    // Reinitialize the component
    WarmInitialize();
    // Execute the command
    return !GenericTC::Command(command).compare(BEP_SUCCESS_RESPONSE) ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
}
