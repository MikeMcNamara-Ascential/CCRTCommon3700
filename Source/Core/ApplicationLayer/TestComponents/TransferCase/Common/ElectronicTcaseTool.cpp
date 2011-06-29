//******************************************************************************
// Description:
//  Electronic version of Tool Kit for Transfer Case Test Component.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/TransferCase/Common/ElectronicTcaseTool.cpp $
// 
// 2     10/30/07 2:01p Rwiersem
// Changes for the 07103101 core release:
// 
// - Moved single lines that went off the screen to multiple lines.
// 
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ElectronicTcaseTool.h"

template <class TestComponent>
ElectronicTcaseTool<TestComponent>::ElectronicTcaseTool(TestComponent *tc) : TcaseTool(tc)
{
    if (tc == NULL)
        throw BepException();
    m_tcaseComponent = tc;
}

template <class TestComponent>
ElectronicTcaseTool<TestComponent>::~ElectronicTcaseTool()
{  // Nothing to do here
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::ShiftTransmission ( const string transGear )
{
    //Log Transmission verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::ShiftTransmission - Called\n");
    return(m_tcaseComponent->ShiftTransmission(transGear));
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::TurnKeyOnOff ( const string keyPosition )
{
    //Log Turn Key ON/Off verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::TurnKeyOnOff - Called\n");
    return(m_tcaseComponent->TurnKeyOnOff(keyPosition));
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::EngineOnOff ( const string engineRun )
{
    //Log Engine Key ON/Off verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::CheckThrottlePosition - Called\n");
    return(m_tcaseComponent->EngineOnOff(engineRun));
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::CheckThrottlePosition ( const string throttlePosition )
{
    //Log Throttle ON/Off verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::CheckThrottlePosition - Called\n");
    return(m_tcaseComponent->CheckThrottlePosition(throttlePosition));
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::ShiftTcase ( void )
{
    string testResult(testFail);        //Default test to fail
    string shiftMethod (m_tcaseComponent->GetParameter(m_tcaseComponent->GetTestStepName()+"ShiftMethod"));

    //Log Tcase shift method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "%s: Tcase Use < %s > to shift\n", m_tcaseComponent->GetTestStepName().c_str(), shiftMethod.c_str());
    //check how to shift
    if (shiftMethod == "Command")
    {
        //Read cluster message to verify shift happened
        testResult = m_tcaseComponent-> CommandShiftTcase();
    }
    else if (shiftMethod == "Manual")
    {
        //check to see what is the prompt hold time
        testResult = TcaseTool::ShiftTcase();
    }
    else
    {   //Undefined shift method
        m_tcaseComponent->Log(LOG_ERRORS, "%s: Undefined Shift Tcase method < %s >\n", 
                              m_tcaseComponent->GetTestStepName().c_str(), shiftMethod.c_str());
        testResult = testFail;
    }
    m_tcaseComponent->Log(LOG_FN_ENTRY, "%s: TcaseTool::ShiftTcase use < %s > to shift - EXIT\n", 
                          m_tcaseComponent->GetTestStepName().c_str(), shiftMethod.c_str());
    return(testResult);
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::VerifyTcaseShift ( const string &verifyMethod )
{
    string testResult(testFail);        //Default test to fail

    //Log Tcase verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::VerifyTcaseShift Use < %s > to Verify\n", verifyMethod.c_str());

    //Verify Tcase shifted
    if (verifyMethod == "Cluster")
    {
        //Read cluster message to verify shift happened
        testResult = m_tcaseComponent->TcaseCheckCluster();
    }
    else if (verifyMethod == "Driver")
    {
        //Prompt Driver to use pendant and verify shift happened
        testResult = TcaseCheckDriver();
    }
    else if (verifyMethod == "Gear")
    {
        //Read Tcase message to verify gear was seen
        testResult = m_tcaseComponent->TcaseCheckGear();
    }
    else if (verifyMethod == "Selector")
    {
        //Read Tcase switch message to verify switch position
        testResult = m_tcaseComponent->TcaseCheckSwitch();
    }
    else if (verifyMethod == "Prompt")
    {   //prompt shift tcase
        testResult = TcasePromptDriver();
    }
    else
    {   //Undefined verification method
        m_tcaseComponent->Log(LOG_ERRORS, "ElectronicTcaseTool::VerifyTcaseShift: Undefined Tcase Check type < %s >\n", verifyMethod.c_str());
        testResult = testFail;
    }
    return(testResult);
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::CommandShiftTcase ( void )
{
    //Log Tcase verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::CommandShiftTcase - Called\n");
    return(m_tcaseComponent->CommandShiftTcase());
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::TcaseCheckCluster ( void )
{
    //Log Tcase verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::TcaseCheckCluster - Called\n");
    return(m_tcaseComponent->TcaseCheckCluster());
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::TcaseCheckGear ( void )
{
    //Log Tcase verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::TcaseCheckGear - Called\n");
    return(m_tcaseComponent->TcaseCheckGear());
}

template <class TestComponent>
string ElectronicTcaseTool<TestComponent>::TcaseCheckSwitch ( void )
{
    //Log Tcase verify method
    m_tcaseComponent->Log(LOG_FN_ENTRY, "ElectronicTcaseTool::TcaseCheckSwitch - Called\n");
    return(m_tcaseComponent->TcaseCheckSwitch());
}
