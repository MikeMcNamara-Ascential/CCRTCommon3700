//*************************************************************************
// FILE:
//    $Header: /Core/ApplicationLayer/VehicleTest/Include/PhaseCommander.h 2     1/23/04 9:25a Cward $
//
// FILE DESCRIPTION:
// The PhaseCommander is responsible for allowing a master task to sequence
// individual test phases.  It will launch a ComponentManager and
// BackgroundComponentManager to launch components and command objectives.
// At the end of each Phase the sequencer will evaluate the oveall Pass or
// Failure of the test and either sequence the phase again or continue.
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
//    $Log: /Core/ApplicationLayer/VehicleTest/Include/PhaseCommander.h $
// 
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:34p Cward
//
// 3     10/24/03 11:06a Khykin
// Updated with latest changes from Flatrock.
//
// 1     10/07/03 11:21a Khykin
// Initial checkin
//
// 2     8/06/03 10:39a Khykin
// Updated with changes from Flatrock.
//
// 1     6/20/03 9:44a Khykin
// Initial checkin
//
// 1     6/20/03 9:19a Khykin
// Initial checkin.
//
//
//*************************************************************************
#ifndef PHASECOMMANDER_H
#define PHASECOMMANDER_H
//==============================================================================
using namespace std;

#include "TestSequencer.h"
#include "BepSemaphore.h"
#include "BepTagDefs.h"
#include <map>

/**
 * Phase result map for storing the phase results.
 * @since Version 1.0
 */
typedef map< std::string , std::string, less<string> >		PHASERESULTMAP;
/**
 * Phase result map iterator for iterating through the phase result map.
 * @since Version 1.0
 */
typedef PHASERESULTMAP::iterator							PHASERESULTMAPITR;
/**
 * Phase result map constant iterator.
 * @since Version 1.0
 */
typedef PHASERESULTMAP::const_iterator						PHASERESULTMAPCONSTITR;
/**
 * Phase result map value used to place a value in the phase result map.
 * @since Version 1.0
 */
typedef PHASERESULTMAP::value_type							PHASERESULTMAPVALUE;

/**
 * The PhaseCommander is responsible for setting up and
 * performing vehicle tests.  It will receive a drive
 * curve, launch the necessary components, and then
 * allow phases to be commanded by a test master.
 *
 * @author Kevin Hykin
 * @version 1.0
 */
class PhaseCommander : public TestSequencer
{
public:
	/**
	 * Constructor for the Test Sequencer.
	 * @since Version 1.0
	 */
	PhaseCommander();
	
	/**
	 * Destructor for the Test Sequencer.
	 * @since Version 1.0
	 */
	virtual ~PhaseCommander();
	
	/**
	 * Initialize the sequencer for running.
	 *
	 * @param config The configuration for initialization.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *config);
	
	/**
	 * Start the sequencing of the vehicle test.  This
	 * is called with the type of testing that should be
	 * performed.
	 *
	 * @param type   The type of test to sequence. Start, Continue,
	 *               Retest, ...
	 * @return Return the overall status of the test.
	 *         Pass, Fail, Abort, ...
	 * @since Version 1.0
	 */
	const std::string SequenceTest(const std::string type);
	
	/**
     * Method to reset the StopCurrentTest flag.
     */
    void StopCurrentTest_Reset(void);
	
	/**
	 * Method used to abort the current processing based
	 * on the needs of the component.
	 * @since Version 1.0
	 */
	virtual void Abort(void);
	
	/**
	 * Method used to terminate the component.  This performs
	 * any cleanup that the destructor would not and sets
	 * the state of the component to terminate.
	 * @since Version 1.0
	 */
	virtual void Terminate(void);

	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
	 *		   BEP_SUCCESS_RESPONSE == Registered
	 * @since Version 1.0
	 */
	virtual const std::string Register(void);
	
	/**
	 * Get the commanded phase.
	 *
	 * @return The phase that was commanded.
	 * @since Version 1.0
	 */
	std::string GetCommandedPhase(void);
	
	/**
	 * Set the next test phase to be executed.
	 *
	 * @param commandedPhase The test phase to be executed.
	 * @since Version 1.0
	 */
	void SetCommandedPhase(const std::string commandedPhase);
	
	/**
	 * Display the overall phase prompt based on the passed in status.
	 *
	 * @param testStatus The overall phase status.
	 * @since Version 1.0
	 */
	void DisplayOverallTestPhasePrompt(const std::string testStatus);

	/**
	 * Remove the overalll result from the GUI.
	 *
	 * @param testStatus The status of the test.
	 */
	void RemoveOverallTestPhasePrompt(const std::string testStatus);
	
protected:
	
	/**
	 * Determine if the commanded phase needs to be converted.  If so, perform the
	 * conversion and return the updated phase to be executed.
	 *
	 * @param commandedPhase The commanded phase.
	 * @return The phase to be executed.
	 * @since Version 1.0
	 */
	std::string ConvertForSpecialProcessing(const std::string &commandedPhase);

	/**
	 * Update the sequenced test phase result.
	 *
	 * @param phase  The phase that was sequenced.
	 * @param result The result that was returned.
	 */
	void UpdatePhaseResult(const std::string phase, const std::string result);

	/**
	 * Scan throught the list of tests that were executed and return the overall
	 * test result.
	 *
	 * @return The overall test result of the test phases that were sequenced.
	 */
	const std::string GetOverallPhaseTestResults(void);

	/**
	 * Clears the phase result map. This needs to happen at the end of
	 * the test after we have evaluated individual phase results to
	 * determine overall pass/fail
	 */
	void ClearPhaseResultMap(void);

	/**
	 * Special processing phases
	 * @since Version 1.0
	 */
	const XmlNode *m_specialProcessingPhases;
	
	/**
	 * Semaphore that is used to manage the signaling for a phase that is to be
	 * executed.
	 * @since Version 1.0
	 */
	BepSemaphore m_semCommandPhase;
	
	/**
	 * The phase that was commanded.
	 * @since Version 1.0
	 */
	string m_commandedPhase;

	/**
	 * The phase result map used to store the results of all of the sequenced test
	 * phases.
	 * @since Version 1.0
	 */
	PHASERESULTMAP	m_phaseResultMap;

};
#endif //PHASECOMMANDER_H

