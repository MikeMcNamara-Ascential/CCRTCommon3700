//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/PhaseCommander/PhaseCommander.cpp 3     5/08/06 7:18p Cward $
//
// FILE DESCRIPTION:
// The PhaseCommander is responsible for commanding the individual phases of the provided
// drive curve.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/VehicleTest/PhaseCommander/PhaseCommander.cpp $
// 
// 3     5/08/06 7:18p Cward
// Changed printf statements to Log statements.
//
// 2     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:33p Cward
//
// 4     10/24/03 11:08a Khykin
// Updated with latest changes.
//
// 1     10/07/03 11:20a Khykin
// Initial checkin
//
// 2     8/06/03 3:08p Khykin
// Updated with latest changes from Flatrock.
//
// 2     6/24/03 7:10a Khykin
// Updated to better handle a component sig seg.
//
// 1     6/20/03 9:46a Khykin
// Initial checkin
//
// 1     6/20/03 9:22a Khykin
// Initial checkin.
//
//
//*************************************************************************
#include "PhaseCommander.h"

PhaseCommander::PhaseCommander() : TestSequencer(),
	m_semCommandPhase(0, false, "CommandPhase")
{
}

PhaseCommander::~PhaseCommander()
{	// if a drive curve was loaded
	Log(LOG_FN_ENTRY, "~PhaseCommander Terminating\n");
}

void PhaseCommander::Initialize(const XmlNode *configFile)
{	// initialize the base component
	TestSequencer::Initialize(configFile);
	// get the list of phases that will be processed specially
	m_specialProcessingPhases = configFile->getChild("SpecialProcessingPhases");
};

const std::string PhaseCommander::Register(void)
{
	Log(LOG_FN_ENTRY, "Registering the PhaseCommander\n");
	string status = TestSequencer::Register();
	return(status);
}

const std::string PhaseCommander::SequenceTest(const std::string type)
{
	Log(LOG_FN_ENTRY, "PhaseCommander All Set Up\n");

	// before the test starts, get a reference to the overall pass/fail and set it to fail
	XmlNode *result = const_cast<XmlNode *> (m_driveCurve->getAttributes().getNode("Result"));
	std::string testStatus="Pass";			// set the overall test status to pass
	m_backgroundObjectiveStatus = "Pass";	// set the background test status to pass
	std::string response;					// response buffer
	SetStatus(BEP_TESTING_STATUS);			// set the test status to testing

	// set the overall status of the test to Fail
	result->setValue("Fail");
	// clear out the failure string
	SetFailureString("");
	string commandedPhase;
	string phaseStatus;			// the status of the current phase

	try
	{
		// Clear previous phase results
		ClearPhaseResultMap();

		// sit here until the test is over
		do
		{   // get the commanded phase
			commandedPhase = GetCommandedPhase();

			if((GetStatus() != BEP_ABORT_STATUS) && (GetStatus() != BEP_TERMINATE_STATUS))
			{
                // if a phase was commanded
                if(!commandedPhase.empty())
                {
                    // set the status of the phase processing to in progress
    				RemoveOverallTestPhasePrompt(phaseStatus);
    				phaseStatus = TEST_STATUS_IN_PROGRESS;
                    Log(LOG_FN_ENTRY, "Write The Phase Status Tag(%s, %s)\n", PHASE_STATUS_TAG, phaseStatus.c_str());
    				m_ndb->Write(PHASE_STATUS_TAG, phaseStatus, response, true);
                    Log(LOG_FN_ENTRY, "Write The Phase Status Tag(%s, %s) done\n", PHASE_STATUS_TAG, phaseStatus.c_str());
    				try
    				{   // look up the phase
                        if(m_driveCurve != NULL)
                        {
                            Log(LOG_FN_ENTRY, "Getting The Commanded Phase: %s\n", commandedPhase.c_str());
                            m_currentPhase = m_driveCurve->getChildren().getNode(commandedPhase);
                            Log(LOG_DEV_DATA, "Got The Commanded Phase\n");
        					// walk through the phases until complete
        					Log(LOG_DEV_DATA, "Current Test Phase: %s\n", m_currentPhase->ToString().c_str());

        					if(ResetPhase(false))				// reset the phase in the drive curve
        					{
        						// Re-set the overall test status to pass when sequencing a new phase
        						testStatus = BEP_PASS_RESPONSE;
        						// Sequence this phase and get the phase result
                                if(SequencePhase(false))	// if there were any foreground tests sequenced
                                {
                                    Log(LOG_DEV_DATA, "Processing The Results\n");
                                    phaseStatus = ProcessResults();
                                }
        						Log(LOG_DEV_DATA, "Current Test Status: %s\n", phaseStatus.c_str());

        						UpdateCurrentStatus(phaseStatus, testStatus);
        						Log(LOG_DEV_DATA, "Phase %s, Result: %s, Overall: %s\n", m_currentPhase->getName().c_str(),
        								phaseStatus.c_str(), testStatus.c_str());
        					}
        					else
        					{
        						Log( LOG_ERRORS, "\tERROR: ResetPhase() failed\n");
        					}
                        }
                        else
                            throw BepException("\tError: Drive Curve Is Null\n");

        			}
    				catch(BepException &e)
    				{
    					Log(LOG_ERRORS, "PhaseCommander: Phase: %s, Reason: %s\n", m_currentPhase->getName().c_str(), e.what());
    					// update the failure string
    					char buffer[128];		// buffer to create messages with
    					memset(buffer, 0, sizeof(buffer));
    					SetFailureString(CreateMessage(buffer, sizeof(buffer), "PhaseCommander: Phase: %s, Reason: %s\n", commandedPhase.c_str(), e.what()));
    					SetStatus(BEP_SOFTWAREFAIL_STATUS);		// set the component status to a software fail
    					testStatus = BEP_SOFTWAREFAIL_STATUS;	// set the sequence status to software fail
    				}
    				catch(...)
    				{
    					Log(LOG_ERRORS, "TestSequencer: Unknown Exception\n");
    					// update the failure string
    					char buffer[128];		// buffer to create messages with
    					memset(buffer, 0, sizeof(buffer));
    					SetFailureString(CreateMessage(buffer, sizeof(buffer), "PhaseCommander: Software Failure Occurred Sequencing Phase: %s\n", commandedPhase.c_str()));
    					SetStatus(BEP_SOFTWAREFAIL_STATUS);		// set the component status to a software fail
    					testStatus = BEP_SOFTWAREFAIL_STATUS;	// set the sequence status to software fail
    				}

    				// check if software failure occurred, if so, abort
    				if(testStatus == BEP_SOFTWAREFAIL_STATUS)
    				{
    					Log(LOG_ERRORS, "Software Failure Occurred Sequencing The Test\n");
                        m_compManager.AbortAll();				// if an issue detected, abort all of the test
    					SetStatus(BEP_SOFTWAREFAIL_STATUS);		// set the component status to a software fail
    				}
    				// else if a fatal failure occurred
    				else if(testStatus == BEP_FATALFAIL_STATUS)
    				{
    					Log(LOG_ERRORS, "Fatal failure detected, aborting test\n");
    					m_compManager.AbortAll();				// if an issue detected, abort all of the test
    					SetStatus(BEP_FATALFAIL_STATUS);		// set the component status to a software fail
    				}
    				// else if an Abort occurred, reset the status
    				else if(GetStatus() == BEP_ABORT_RESPONSE)
    				{
    					Log(LOG_ERRORS, "Aborting The TestComponents\n");
    					m_compManager.AbortAll();				// if an issue detected, abort all of the test
    					Log(LOG_ERRORS, "Resetting The Abort Status\n");
    					m_ndb->Write(ABORT_DATA_TAG, "0", response, true);
    				}

    				// update the phase status
    				if((phaseStatus != BEP_PASS_STATUS) && (phaseStatus != BEP_ABORT_STATUS))
    					phaseStatus = BEP_FAIL_STATUS;

    				// update the prompt to reflect the phase status
    				DisplayOverallTestPhasePrompt(phaseStatus);

    				// update the overall test phase status with the results of the current phase
    				UpdatePhaseResult(commandedPhase, phaseStatus);

    				// translate the command for special processing if configured for it
    				commandedPhase = ConvertForSpecialProcessing(commandedPhase);
    				// if the test is not complete, execute the phase
    				if(commandedPhase == BEP_COMPLETE_STATUS)
    				{	// set the status to complete and set phase pass
    					Log(LOG_DEV_DATA, "Test Complete Commanded\n");
    					SetStatus(BEP_COMPLETE_STATUS);
    					// get the results of the overall test
    					testStatus = GetOverallPhaseTestResults();
    					// update the GUI overall test status
    					DisplayOverallTestPrompt(testStatus);
    				}
                }
                else
                    Log(LOG_ERRORS, "ERROR: Blank command received\n");

				// reset the test phase statuses
                StopCurrentTest_Reset();
            }
			else
			{	// if in the middle of a test, flag the abort
				testStatus = BEP_ABORT_STATUS;
				SetStatus( testStatus);
				// update the prompt to reflect the phase status
				DisplayOverallTestPhasePrompt(testStatus);
				// update the GUI overall test status
				DisplayOverallTestPrompt(testStatus);
				Log(LOG_ERRORS, "PhaseCommander Aborted or Terminated: %s\n", GetStatus().c_str());
			}
		// while a test is in progress
		} while((GetStatus() != BEP_COMPLETE_STATUS) && (GetStatus() != BEP_SOFTWAREFAIL_STATUS) &&
				(GetStatus() != BEP_ABORT_STATUS) && (GetStatus() != BEP_TERMINATE_STATUS));

		// set the overall result value
		result->setValue(testStatus);
	}
	// if there is an exception after the phase is commanded, handle the error
	catch(BepException &e)
	{   // if the test is in progress, set the error
		if(m_ndb->Read(PHASE_STATUS_TAG, response, true) == BEP_STATUS_SUCCESS)
		{
			XmlNode node(PHASE_STATUS_TAG, XML_T(""));
			m_ndb->GetNext(node, response);
			if(node.getValue() == TEST_STATUS_IN_PROGRESS)
				m_ndb->Write(PHASE_STATUS_TAG, TEST_STATUS_ERROR, response, true);
		}
		// update the GUI overall test status to abort
		DisplayOverallTestPrompt(BEP_ABORT_STATUS);
		// set the result to abort
		result->setValue(BEP_ABORT_STATUS);
		// update the failure string
		char buffer[256];
		memset(buffer, 0, sizeof(256));
		SetFailureString(CreateMessage(buffer, sizeof(buffer),
						"PhaseCommander Aborted Error: %s, Failure String: %s",
						e.what(), GetFailureString().c_str()));
		// set the test status to software fail
		testStatus = BEP_SOFTWAREFAIL_STATUS;
	}

	// reset the abort status at the end of the phase
	m_ndb->Write(ABORT_DATA_TAG, "0", response, true);

	return(testStatus);
};

void PhaseCommander::StopCurrentTest_Reset(void)
{
    std::string response;
    // reset the test phase statuses
    m_ndb->Write(STOP_CURRENT_TEST_TAG, "0", response, true);
}

std::string PhaseCommander::GetCommandedPhase(void)
{
	Log(LOG_DEV_DATA, "Waiting for a phase to be commanded\n");
	string commandedPhase;
	// block until a phase is commanded
	m_semCommandPhase.CriticalSectionEnter();
	// get the commanded phase
	commandedPhase = m_commandedPhase;
	Log(LOG_DEV_DATA, "Received new phase to command: %s\n", commandedPhase.c_str());

	return(commandedPhase);
}

void PhaseCommander::SetCommandedPhase(const std::string commandedPhase)
{
	Log(LOG_DEV_DATA, "Setting the newly commanded phase: %s\n", commandedPhase.c_str());
	// set the commanded phase
	m_commandedPhase = commandedPhase;
	// signal the phase commander to execute the phase
	m_semCommandPhase.CriticalSectionExit();
}

std::string PhaseCommander::ConvertForSpecialProcessing(const std::string &commandedPhase)
{
	std::string newPhase;

	try
	{   // look in the special processing node for the commanded test
		// if it exists, update the phase to be executed
		newPhase = m_specialProcessingPhases->getChild(commandedPhase)->getValue();
	}
	catch(...)
	{	// else command the phase that was commanded
		newPhase = commandedPhase;
	}

	return(newPhase);
}

void PhaseCommander::Abort(void)
{
	Log(LOG_FN_ENTRY, "PhaseCommander::Abort\n");
	// call the base class abort
	TestSequencer::Abort();
	// reset the commanded phase
	m_commandedPhase="";
	// unblock the phase commander
	m_semCommandPhase.CriticalSectionExit();
	Log(LOG_FN_ENTRY, "PhaseCommander::Abort done\n");
};

void PhaseCommander::Terminate(void)
{
	Log(LOG_FN_ENTRY, "PhaseCommander::Terminate\n");
	// call the base class terminate
	TestSequencer::Terminate();
	// reset the commanded phase
	m_commandedPhase="";
	// unblock the phase commander
	m_semCommandPhase.CriticalSectionExit();
	Log(LOG_FN_ENTRY, "PhaseCommander::Terminate done\n");
};

void PhaseCommander::DisplayOverallTestPhasePrompt(const std::string testStatus)
{
	Log(LOG_FN_ENTRY, "DisplayOverallTestPhasePrompt(%s)\n", testStatus.c_str());
	std::string testResult("PhaseFail");	// default to test failed
	std::string bgColor("red");				// and a red background

	if(testStatus == "Pass")
	{
		testResult = "PhasePass";
		bgColor = "green";
	}

	std::string msgResponse;
	m_prompt->DisplayPrompt(1, "Blank", msgResponse);
	m_prompt->SetBackgroundColor(1, "white", msgResponse);
	m_prompt->DisplayPrompt(2, testResult, msgResponse);
	m_prompt->SetBackgroundColor(2, bgColor, msgResponse);

	// write the phase status
	m_ndb->Write(PHASE_STATUS_TAG, testStatus, msgResponse, true);
}

void PhaseCommander::RemoveOverallTestPhasePrompt(const std::string testStatus)
{
	Log(LOG_FN_ENTRY, "RemoveOverallTestPrompt(%s)\n", testStatus.c_str());

	std::string msgResponse;
	// if a prompt was not previously displayed, blank it
	if(testStatus.empty())  m_prompt->DisplayPrompt(2, "Blank", msgResponse);
	// else remove the displayed prompt
	else                    m_prompt->RemovePrompt(2, testStatus, msgResponse);
	m_prompt->SetBackgroundColor(2, "white", msgResponse);
	Log(LOG_FN_ENTRY, "RemoveOverallTestPrompt(%s) done\n", testStatus.c_str());
}


void PhaseCommander::UpdatePhaseResult(const std::string phase, const std::string result)
{
	PHASERESULTMAPITR	itr;

	// Must overwrite an existing phase with the same phase tag
	if( (itr=m_phaseResultMap.find( phase)) != m_phaseResultMap.end())
	{
		m_phaseResultMap.erase( itr);
	}
	// insert the result into the map
	m_phaseResultMap.insert(PHASERESULTMAPVALUE(phase, result));
}

const std::string PhaseCommander::GetOverallPhaseTestResults(void)
{
	std::string overallResult = BEP_PASS_STATUS;

	// if there were any phases sequenced
	if(m_phaseResultMap.size())
	{   // walk throught the phase map
		for(PHASERESULTMAPITR iter = m_phaseResultMap.begin(); iter != m_phaseResultMap.end(); iter++)
		{   // update the overall result
			UpdateCurrentStatus((*iter).second, overallResult);
		}
	}

	// return the overall result
	return(overallResult);
}

void PhaseCommander::ClearPhaseResultMap(void)
{
	Log(LOG_FN_ENTRY, "PhaseCommander::ClearPhaseResultMap()\n");
	m_phaseResultMap.clear();
}

