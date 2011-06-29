//******************************************************************************
// Description:
// This class extends the functionality of GenericABSTCTemplate for Ford
// specific ABS functionality
//
//==============================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "GMBrakeTC.h"
#include "DataAnalysis.h"
#include <algorithm>

//-----------------------------------------------------------------------------
template<class ModuleType>
GMBrakeTC<ModuleType>::GMBrakeTC() : GenericABSTCTemplate<ModuleType>(), 
    m_parkBrakeStartIndex(0), m_parkBrakeEndIndex(0), 
    m_tractionControlStartIndex(0), m_tractionControlEndIndex(0), m_dragTestStartIndex(0)
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
GMBrakeTC<ModuleType>::~GMBrakeTC()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
const string GMBrakeTC<ModuleType>::CommandTestStep(const string &value)
{
    string testResult(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "GMBrakeTC::CommandTestStep(%s) - Enter", value.c_str());
    // Make sure brake testing status is always set to testing
    m_baseBrakeTool->SetBrakeTestingStatus(BEP_TESTING_RESPONSE);
    // Make sure it is ok to perform the test step
    if(BEP_STATUS_SUCCESS == StatusCheck())
    {
        if(!GetTestStepName().compare("StartAbsValveFiring"))             testResult = StartAbsValveFiringTest();
        else if(!GetTestStepName().compare("StopAbsValveFiring"))         testResult = StopAbsValveFiring();
        else if(!GetTestStepName().compare("StartParkBrakeTest"))         testResult = StartParkBrakeTest();
        else if(!GetTestStepName().compare("StopParkBrakeTest"))          testResult = StopParkBrakeTest();
        else if(!GetTestStepName().compare("StartTractionControlTest"))   testResult = StartTractionControlTest();
        else if(!GetTestStepName().compare("StopTractionControlTest"))    testResult = StopTractionControlTest();
        else if(!GetTestStepName().compare("StartBaseBrakeTest"))         testResult = StartBrakeTest();
        else if(!GetTestStepName().compare("StopBaseBrakeTest"))          testResult = StopBrakeTest();
        else if(!GetTestStepName().compare("StartMaxBrakeForceTest"))     testResult = StartMaxBrakeForceTest();
        else if(!GetTestStepName().compare("StopMaxBrakeForceTest"))      testResult = StopMaxBrakeForceTest();
        else if(!GetTestStepName().compare("DragTest"))                
        {   // Store the drag test start index
            m_dragTestStartIndex = TagArray("TagDragTestStart");
            testResult = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
        }
        else  testResult = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
    }
    else
    {   // System is not ready to perform the test
        testResult = testSkip;
        Log(LOG_ERRORS, "System status preventing test: %s", ConvertStatusToResponse(StatusCheck()).c_str());
    }
    Log(LOG_FN_ENTRY, "GMBrakeTC::CommandTestStep(%s) - returning: %s", value.c_str(), testResult.c_str());
    return testResult;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StartBrakeTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartBrakeTest() - Enter");
    // Set the starting marker
    m_baseBrakeTool->SetBrakeStart(TagArray("BaseBrakeStart"));
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartBrakeTest() - Exit");
    return testPass;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StopBrakeTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopBrakeTest() - Enter");
    // Set the starting marker
    m_baseBrakeTool->SetBrakeEnd(TagArray("BaseBrakeEnd"));
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopBrakeTest() - Exit");
    return testPass;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StartAbsValveFiringTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartAbsValveFiringTest() - Enter");
    // Set the markers for the start of ABS testing
    INT32 startIndex = TagArray("StartValveFiring");
    // Store the start index.  All wheels use the same index since we have no idea of firing order
    for(UINT8 index = 0; index < GetRollerCount(); index++)
    {
        m_reduxRecovIndex[index].reductionStart = startIndex;
        m_reduxRecovIndex[index].recoveryStart = startIndex;
    }
    // Set the ABS start Index
    m_absStartIndex = startIndex;
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartAbsValveFiringTest() - Exit: Pass");
    return testPass;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StopAbsValveFiring(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopAbsValveFiring() - Enter");
    // Tag the data array with the end tag
    INT32 endIndex = TagArray("StopValveFiring");
    // Store the stop index.  All wheels use the same index since the firing sequence is unknown
    for(UINT8 index = 0; index < GetRollerCount(); index++)
    {
        m_reduxRecovIndex[index].reductionEnd = endIndex;
        m_reduxRecovIndex[index].recoveryEnd = endIndex;
    }
    // Set the ABS end index
    m_absEndIndex = endIndex;
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopAbsValveFiring() - Exit: Pass");
    return testPass;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StartParkBrakeTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartParkBrakeTest() - Enter");
    // Tag the start of the park brake test
    m_parkBrakeStartIndex = TagArray("StartParkBrakeTest");
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartParkBrakeTest() - Exit");
    return testPass;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StopParkBrakeTest(void)
{
    string testResult(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopParkBrakeTest() - Enter");
    // Tag the end of the test
    m_parkBrakeEndIndex = TagArray("StopParkBrakeTest");
    // Find the max force for each wheel
    WHEELDATAARRAY forces;  // Array to hold the roller forces from the test
    float maxParkBrakeForce[GetRollerCount()];  // Max
    for(UINT8 index = 0; index < GetRollerCount(); index++)  maxParkBrakeForce[index] = 0;
    BEP_STATUS_TYPE status = ConvertIntToBepStatus(ReadDataArrays(GetParameter("IcmForceArray"), m_parkBrakeStartIndex, 
                                                                  m_parkBrakeEndIndex, forces));
    if(BEP_STATUS_SUCCESS == status)
    {
        DataAnalysis analyze;
        for(UINT8 index = 0; index < GetRollerCount(); index++)
        {
            const DATAARRAY &rollerForces = forces[index];
            vector<float>::const_iterator maxIter = max_element(rollerForces.begin(), rollerForces.end());
            vector<float>::const_iterator minIter = min_element(rollerForces.begin(), rollerForces.end());
            maxParkBrakeForce[index] = *maxIter;
            // Report the stats
            char buff[8];
            SendSubtestResultWithDetail(rollerName[index]+"ParkBrakeStats", testPass, "Park Brake Stats", "0000",
                                        "MinParkBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", *minIter), unitsLBF,
                                        "MaxParkBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", *maxIter), unitsLBF,
                                        "MinParkBrakeIndex", CreateMessage(buff, sizeof(buff), "%d", distance(rollerForces.begin(), minIter)+m_parkBrakeStartIndex), "",
                                        "MaxParkBrakeIndex", CreateMessage(buff, sizeof(buff), "%d", distance(rollerForces.begin(), maxIter)+m_parkBrakeStartIndex), "",
                                        "ParkBrakeAverage", CreateMessage(buff, sizeof(buff), "%.2f", (*maxIter + *minIter) / 2.0), unitsLBF);
            Log(LOG_DEV_DATA, "%s max park brake force: %3.7f", rollerName[index].c_str(), maxParkBrakeForce[index]);
        }
        testResult = testPass;
    }
    else
    {   // Could not get force data
        Log(LOG_ERRORS, "Could not read force data: %s", ConvertStatusToResponse(status).c_str());
        testResult = testFail;
    }
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopParkBrakeTest() - Exit");
    return testResult;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::PerformMaxBrakeTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::PerformMaxBrakeTest() - Enter");
    string testResult(BEP_TESTING_STATUS);
    INT32 startIndex = TagArray("StartMaxBrakeTest");
    BposSleep(GetParameterInt("MaxBrakeTestDelay"));
    INT32 stopIndex = TagArray("StopMaxBrakeTest");
    BrakeDataStats stats[GetRollerCount()];
    // Analyze the max brake data
    char buff[8];
    INT32 status = BEP_STATUS_SUCCESS;
    float average = 0.0;
    for(UINT32 wheel = LFWHEEL; (wheel < GetRollerCount()) && (BEP_STATUS_SUCCESS == status); wheel++)
    {
        status = m_baseBrakeTool->AverageForces(wheel, average, stats[wheel]);
        // Report the test data stats
        SendSubtestResultWithDetail(rollerName[wheel]+"MaxBrakeStats", testPass, "", "0000",
                                    "MinBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].minValue), unitsLBF,
                                    "MaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].maxValue), unitsLBF,
                                    "MinBrakeIndex", CreateMessage(buff, sizeof(buff), "%d", stats[wheel].minValueIndex+startIndex), "",
                                    "MaxBrakeIndex", CreateMessage(buff, sizeof(buff), "%d", stats[wheel].maxValueIndex+startIndex), "",
                                    "Average", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].averageValue), unitsLBF);
    }
    Log(LOG_FN_ENTRY, "GMBrakeTC::PerformMaxBrakeTest() - Exit");
    return testResult;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StartTractionControlTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartTractionControlTest() - Enter");
    // Tag the start of the test
    m_tractionControlStartIndex = TagArray("StartTractionControlTest");
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartTractionControlTest() - Exit");
    return testPass;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StopTractionControlTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopTractionControlTest() - Enter");
    // Tag the end of the test
    m_tractionControlEndIndex = TagArray("StopTractionControlTest");
    // Get the test stats for the test
    // Read in the data from the brake force array
    INT32 testStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"), 
                                                                   m_tractionControlStartIndex, 
                                                                   m_tractionControlEndIndex);
    if(BEP_STATUS_SUCCESS == testStatus)
    {
        // Average the forces
        BrakeDataStats stats[GetRollerCount()];
        INT32 avgStat = BEP_STATUS_SUCCESS;
        float average = 0.0;
        char buff[8];
        for(UINT32 wheel = LFWHEEL; (wheel < GetRollerCount()) && (BEP_STATUS_SUCCESS == avgStat); wheel++)
        {
            avgStat = m_baseBrakeTool->AverageForces(wheel, average, stats[wheel]);
            // Report the test data stats
            SendSubtestResultWithDetail(rollerName[wheel]+"TractionControlStats", testPass, "", "0000",
                                        "MinReduction", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].minValue), unitsLBF,
                                        "MaxRecovery", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].maxValue), unitsLBF,
                                        "MinReductionIndex", CreateMessage(buff, sizeof(buff), "%d", stats[wheel].minValueIndex+m_dragTestStartIndex), "",
                                        "MaxRecoveryIndex", CreateMessage(buff, sizeof(buff), "%d", stats[wheel].maxValueIndex+m_dragTestStartIndex), "",
                                        "Average", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].averageValue), unitsLBF);
			// Report the individual values for display
			SystemWrite(rollerName[wheel]+"TractionMinReductionValue", stats[wheel].minValue);
			SystemWrite(rollerName[wheel]+"TractionMaxRecoveryValue", stats[wheel].maxValue);
			SystemWrite(rollerName[wheel]+"TractionAverageValue", stats[wheel].averageValue);
			SystemWrite(rollerName[wheel]+"TractionMinReductionIndex", stats[wheel].minValueIndex+m_dragTestStartIndex);
			SystemWrite(rollerName[wheel]+"TractionMaxRecoveryIndex", stats[wheel].maxValueIndex+m_dragTestStartIndex);
        }
    }
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopTractionControlTest() - Exit");
    return testPass;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StartMaxBrakeForceTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartMaxBrakeForceTest() - Enter");
    m_maxBrakeForceStartIndex = TagArray("StartMaxBrakeTest");
    Log(LOG_FN_ENTRY, "GMBrakeTC::StartMaxBrakeForceTest() - Exit");
    return testPass;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string GMBrakeTC<ModuleType>::StopMaxBrakeForceTest(void)
{
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopMaxBrakeForceTest() - Enter");
    // Tag the array with the end of test
    m_maxBrakeForceEndIndex = TagArray("EndMaxBrakeTest");
    // Read the data from the brake force array
    INT32 testStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"),
                                                                   m_maxBrakeForceStartIndex,
                                                                   m_maxBrakeForceEndIndex);
    // If data retrieved, calculate the stats
    if(BEP_STATUS_SUCCESS == testStatus)
    {   // Average the forces
        BrakeDataStats stats[GetRollerCount()];
        INT32 avgStat = BEP_STATUS_SUCCESS;
        float average = 0.0;
        char buff[16];
        for(UINT32 wheel = LFWHEEL; (wheel < GetRollerCount()) && (BEP_STATUS_SUCCESS == avgStat); wheel++)
        {
            avgStat = m_baseBrakeTool->AverageForces(wheel, average, stats[wheel]);
            // Report the test data stats
            SendSubtestResultWithDetail(rollerName[wheel]+"MaxBrakeStats", testPass, "", "",
                                        "MinBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].minValue), unitsLBF,
                                        "MaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].maxValue), unitsLBF,
                                        "Average", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].averageValue), unitsLBF,
                                        "MinBrakeIndex", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].minValueIndex+m_maxBrakeForceStartIndex), "",
                                        "MaxBrakeIndex", CreateMessage(buff, sizeof(buff), "%.2f", stats[wheel].maxValueIndex+m_maxBrakeForceStartIndex), "");
        }
    }
    Log(LOG_FN_ENTRY, "GMBrakeTC::StopMaxBrakeForceTest() - Exit");
    return testPass;
}
