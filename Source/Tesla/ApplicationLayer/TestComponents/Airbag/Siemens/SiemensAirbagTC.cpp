//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
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
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "SiemensAirbagTC.h"
#include "ModuleException.h"

//-----------------------------------------------------------------------------
template<class ModuleType>
SiemensAirbagTC<ModuleType>::SiemensAirbagTC() : GenericTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
SiemensAirbagTC<ModuleType>::~SiemensAirbagTC()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
const string SiemensAirbagTC<ModuleType>::CommandTestStep(const string &value)
{
    string result(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "SiemensAirbagTC::CommandTestStep(value: %s) - Step: %s - Enter", 
        value.c_str(), GetTestStepName().c_str());
    // Make sure system status will allow testing
    if(BEP_STATUS_SUCCESS != StatusCheck())
    {
        UpdateResult(StatusCheck(), result);
        Log(LOG_ERRORS, "SiemensAirbagTC::CommandTestStep: StatusCheck() failed: %s", result.c_str());
        SendTestResult(result, GetTestStepInfo("Description"));
    }
    // Link up with the module
    else if(!GetTestStepName().compare("PerformModuleLinkup"))   result = PerformModuleLinkup();
    // Check the airbag lamp status
    else if(!GetTestStepName().compare("CheckAirbagLamp"))       result = CheckAirbagLamp(value);
    // Check the resistance values
    else if(!GetTestStepName().compare("CheckResistanceValues")) result = CheckResistanceValues();
    // Call the base class to perform the test step
    else     result = GenericTCTemplate<ModuleType>::CommandTestStep(value);
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "SiemensAirbagTC::CommandTestStep(value: %s) - Step: %s - Exit: %s", 
        value.c_str(), GetTestStepName().c_str(), result.c_str());
    return result;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
void SiemensAirbagTC<ModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to start the initialization
    GenericTCTemplate<ModuleType>::InitializeHook(config);
    // Get the list of resistance values to check
    try
    { 
        m_resistanceData.DeepCopy(config->getChild("Setup/Parameters/AirbagResistanceData")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Resistance data not prvided: %s", excpt.GetReason());
        m_resistanceData.clear(true);
    }
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string SiemensAirbagTC<ModuleType>::PerformModuleLinkup(void)
{
    string testResult(BEP_TESTING_STATUS);
    // Log the entry and determine if the test step should be performed
    Log(LOG_FN_ENTRY, "SiemensAirbagTC::PerformModuleLinkup() - Enter");
    if(!ShortCircuitTestStep())
    {   // Command the module to perform a low baud init sequence
        string resultCode("0000");
        string description(GetTestStepInfo("Description"));
        if(CheckCableConnect() || GetParameterBool("DeveloperTest"))
        {
            BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
            try
            {
                UINT8 retry = 3;
                while(retry-- && (status != BEP_STATUS_SUCCESS))
                {
                    status = m_vehicleModule.PerformModuleLinkup();
                    if(status != BEP_STATUS_SUCCESS)
                    {   //prompt driver to restart
                        DisplayTimedPrompt("KeyOff", "1", 0, 10000);
                        DisplayTimedPrompt("StartVehicle", "1", 0, 6000);
                        status = m_vehicleModule.PerformModuleLinkup();
                    }
                }
                Log(LOG_ERRORS, "Module linkup status: %s", ConvertStatusToResponse(status).c_str());
                testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
                resultCode = !testResult.compare(testPass) ? resultCode : GetFaultCode("ModuleLinkupFailure");
                description = !testResult.compare(testPass) ? description : GetFaultDescription("ModuleLinkupFailure");
            }
            catch(ModuleException &excpt)
            {
                Log(LOG_ERRORS, "ModuleException in SiemensAirbagTC::PerformModuleLinkup() - %s", excpt.GetReason());
                testResult = testSoftwareFail;
                resultCode = GetFaultCode("SoftwareFailure");
                description = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {   // Timeout waiting for the cable to be connected
            Log(LOG_ERRORS, "Timeout waiting for the operator to connect the cable");
            testResult = testTimeout;
            resultCode = GetFaultCode("CableConnectTimeout");
            description = GetFaultDescription("CableConnectTimeout");
        }
        // Report the test result
        SendTestResult(testResult, description, resultCode);
    }
    else
    {   // Need to skip the test
        Log(LOG_FN_ENTRY, "Skipping PerformModuleLinkup");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "SiemensAirbagTC::PerformModuleLinkup() - Exit");
    return testResult;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string SiemensAirbagTC<ModuleType>::CheckAirbagLamp(const string &state)
{
    string testResult(BEP_TESTING_STATUS);
    // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "SiemensAirbagTC::CheckAirbagLamp() - Enter");
    if(!ShortCircuitTestStep())
    {   // Prompt the operator to pass or fail the airbag lamp
        testResult = OperatorPassFail("AirbagLamp"+state+"Result");
        Log(LOG_ERRORS, "Airbag lamp status: %s", testResult.c_str());
        // Report the result
        SendTestResult(testResult, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "SiemensAirbagTC::CheckAirbagLamp() - Exit");
    return testResult;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string SiemensAirbagTC<ModuleType>::CheckResistanceValues(void)
{
    string testResult(BEP_TESTING_STATUS);
    string resultCode("0000");
    // Log the entry and check if this step should be perofmred
    Log(LOG_FN_ENTRY, "SiemensAirbagTC::CheckResistanceValues() - Enter");
    if(!ShortCircuitTestStep())
    {   // Read the resistance values from the module
        vector<float> resistanceValues;
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData(GetDataTag("ResistanceValuesMessage"), resistanceValues);
        if(BEP_STATUS_SUCCESS == status)
        {   // Make sure the data we read is the same amount as what we expect
            if(resistanceValues.size() == m_resistanceData.size())
            {   // Check each of the values read from the module
                testResult = testPass;   // Assume pass until one of the values is out of limits
                resultCode = "0000";
                UINT8 index = 0;
                for(XmlNodeMapItr iter = m_resistanceData.begin(); iter != m_resistanceData.end(); iter++)
                {
                    if((resistanceValues[index] < atof(iter->second->getAttribute("Min")->getValue().c_str())) || 
                       (resistanceValues[index] > atof(iter->second->getAttribute("Max")->getValue().c_str())))
                    {   // Failed
                        Log(LOG_ERRORS, "%s resistance value out of limits: %.2f [%s %s]",
                            iter->second->getName().c_str(), resistanceValues[index], 
                            iter->second->getAttribute("Min")->getValue().c_str(),
                            iter->second->getAttribute("Max")->getValue().c_str());
                        testResult = testFail;
                        resultCode = GetFaultCode("ResistanceOutOfLimits");
                    }
                    Log(LOG_DEV_DATA, "%s resistance value: %.2f [%s %s] - %s",
                        iter->second->getName().c_str(), resistanceValues[index], 
                        iter->second->getAttribute("Min")->getValue().c_str(),
                        iter->second->getAttribute("Max")->getValue().c_str(), testResult.c_str());
                    // Report the resistance value
                    char buff[32];
                    SendSubtestResultWithDetail("ResistanceValue"+iter->second->getName(), testResult, GetTestStepInfo("Description"), resultCode,
                                                "MeasuredValue", CreateMessage(buff, sizeof(buff), "%.2f", resistanceValues[index]), "ohms",
                                                "MinValue", iter->second->getAttribute("Min")->getValue(), "ohms",
                                                "MaxValue", iter->second->getAttribute("Max")->getValue(), "ohms");
                }
            }
            else
            {
                Log(LOG_ERRORS, "Amount of data read from the module is different than expected - module items: %d, expected: %d",
                    resistanceValues.size(), m_resistanceData.size());
                testResult = testSoftwareFail;
            }
        }
        else
        {   // Could not read resistance data from the module
            Log(LOG_ERRORS, "Could not read airbag resistance data from the module: %s", ConvertStatusToResponse(status).c_str());
            testResult = testFail;
        }
        // Report the result
        SendTestResult(testResult, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Check Resistance Values");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "SiemensAirbagTC::CheckResistanceValues() - Exit");
    return testResult;
}
