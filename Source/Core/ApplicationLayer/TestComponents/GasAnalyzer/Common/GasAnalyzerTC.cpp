//******************************************************************************
// Description:
//  Gas Analyzer Test Component.
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
#include "GasAnalyzerTC.h"

//-----------------------------------------------------------------------------
template<class AnalyzerType>
GasAnalyzerTC<AnalyzerType>::GasAnalyzerTC() : GenericTCTemplate<AnalyzerType>()
{	// Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class AnalyzerType>
GasAnalyzerTC<AnalyzerType>::~GasAnalyzerTC()
{	// Clear analyzer data items
	m_gasAnalyzerDataItems.clear(true);
}

//-----------------------------------------------------------------------------
template<class AnalyzerType>
void GasAnalyzerTC<AnalyzerType>::Initialize(const string &fileName)
{	// Call the base class to begin the intialization process
	GenericTCTemplate<AnalyzerType>::Initialize(fileName);
}

//-----------------------------------------------------------------------------
template<class AnalyzerType>
void GasAnalyzerTC<AnalyzerType>::Initialize(const XmlNode *config)
{	// Call the base class to begin the initialization
	GenericTCTemplate<AnalyzerType>::Initialize(config);
	// Save the data items to collect from the gas analyzer
	try
	{
		m_gasAnalyzerDataItems.DeepCopy(config->getChild("Setup/Parameters/GasAnalyzerData")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "No gas analyzer data specified for collection! - %s", excpt.GetReason());
		m_gasAnalyzerDataItems.clear(true);
	}
}

//-----------------------------------------------------------------------------
template<class AnalyzerType>
const string GasAnalyzerTC<AnalyzerType>::CommandTestStep(const string &value)
{
	string testResult(BEP_TESTING_STATUS);
	// Determine if the test step should be performed
	if(BEP_STATUS_SUCCESS == StatusCheck())
	{	// Perform the test step
		if(!GetTestStepName().compare("EnableMeasurementMode"))		 testResult = EnableMeasurementMode();
		else if(!GetTestStepName().compare("EnableStandbyMode"))	 testResult = EnableStandbyMode();
		else if(!GetTestStepName().compare("GatherAnalyzerData"))	 testResult = GatherAnalyzerData();
		else	 testResult	= GenericTCTemplate<AnalyzerType>::CommandTestStep(value);
	}
	else
	{	// Cannot continue testing
		Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s", ConvertStatusToResponse(StatusCheck()).c_str());
		testResult = testAbort;
	}
	// Return the result of the test step
	Log(LOG_FN_ENTRY, "GasAnalyzerTC::CommandTestStep() - exit: %s", testResult.c_str());
	return testResult;
}

//-----------------------------------------------------------------------------
template<class AnalyzerType>
string GasAnalyzerTC<AnalyzerType>::EnableMeasurementMode(void)
{
	string testResult(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "GasAnalyzerTC::EnableMeasurementMode() - Enter");
	// Command the analyzer to enter measurement mode
	BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("EnableMeasurementMode");
	Log(LOG_DEV_DATA, "Enabled analyzer measurement mode: %s", ConvertStatusToResponse(status).c_str());
	// Determine the result and report it
	testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
	SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "GasAnalyzerTC::EnableMeasurementMode() - Exit");
	return testResult;
}

//-----------------------------------------------------------------------------
template<class AnalyzerType>
string GasAnalyzerTC<AnalyzerType>::EnableStandbyMode(void)
{
	string testResult(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "GasAnalyzerTC::EnableStandbyMode() - Enter");
	// Command the analyzer to enter measurement mode
	BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("EnableStandbyMode");
	Log(LOG_DEV_DATA, "Enabled analyzer standby mode: %s", ConvertStatusToResponse(status).c_str());
	// Determine the result and report it
	testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
	SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "GasAnalyzerTC::EnableStandbyMode() - Exit");
	return testResult;
}

//-----------------------------------------------------------------------------
template<class AnalyzerType>
string GasAnalyzerTC<AnalyzerType>::GatherAnalyzerData(void)
{
	string testResult(BEP_TESTING_STATUS);
	int                 armSpeed=0;
	Log(LOG_FN_ENTRY, "GasAnalyzerTC::GatherAnalyzerData() - Enter");

	try
	{
		(void)GetTestStepParameter( "ArmSpeed", armSpeed);

		DisplayPrompt(1,"PositionGasProbe");
		BposSleep(2000);

		// wait for the specified speed
		std::string testStatus = WaitAndPromptForSpeed(armSpeed, armSpeed+5, 500);

		RemovePrompts();

		// if we reached the speed ok
		if(testStatus == testPass)
		{
			DisplayPrompt(2,"MaintainSpeed");
			DisplayPrompt(1,"PositionGasProbe");

			// Read the data from the analyzer
			vector<float> analyzerData;
			BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("AnalyzerData", analyzerData);
			if(BEP_STATUS_SUCCESS == status)
			{	// Good data from the analyzer, report all selected items
				char buff[32];
				for(XmlNodeMapItr iter = m_gasAnalyzerDataItems.begin(); iter != m_gasAnalyzerDataItems.end(); iter++)
				{	// Report the item
					SendSubtestResultWithDetail(iter->second->getName(), testPass, GetTestStepInfo("Description"), "0000",
												iter->second->getName(), 
												CreateMessage(buff, sizeof(buff), "%.4f", analyzerData[BposReadInt(iter->second->getValue().c_str())]),
												"");
					// insert logic here to check max values if we ever get them
					testResult = testPass;
				}
			}
			else
			{	// Good not read the data from the analyzer
				testResult = testFail;
				Log(LOG_ERRORS, "Could not read the data from the analyzer - status: %s", ConvertStatusToResponse(status).c_str());
			}
		}
		else
		{
			// did not reach req'd speed
			Log(LOG_DEV_DATA, "Did not reach target speed!");
			testResult = testFail;
		}
	}
	catch(BepException &err)
	{
		Log(LOG_ERRORS, "Gas Analyzer test exception: %s\n", err.GetReason());
		testResult = testSoftwareFail;
	}

	RemovePrompts();

	// check to make sure an abort did not happen, if so flag it
	if(CheckAbort())	testResult = testAbort;

	// Report the result and exit
	SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
	Log(LOG_FN_ENTRY, "GasAnalyzerTC::GatherAnalyzerData() - Enter");
	return testResult;
}
