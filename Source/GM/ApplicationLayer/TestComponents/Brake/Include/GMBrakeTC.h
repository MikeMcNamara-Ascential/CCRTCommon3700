//******************************************************************************
// Description:
// This class extends the functionality of GenericABSTCTemplate for GM
// specific ABS functionality
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GMBrakeTc_h
#define GMBrakeTc_h
//-----------------------------------------------------------------------------
#include "GenericABSTCTemplate.cpp"

//-----------------------------------------------------------------------------
template<class ModuleType>
class GMBrakeTC : public GenericABSTCTemplate<ModuleType>
{
public:
    /** Constructor */
	GMBrakeTC();
    /** Destructor */
    virtual ~GMBrakeTC();
	/**
	 * This is the method that will handle the execution of each
	 * test step that has been specified by the system for each
	 * test objective that is passed to it.  This must be overloaded
	 * to execute the component tests.
	 * 
	 * @param value This is the value that was commanded by
	 * the test sequencer.
	 * @return The status of the overall objective.
	 * @since Version 1.0
	 */
	virtual const string CommandTestStep(const string &value);

protected:
    /**
     * Begin performing the base brake test.
     * 
     * @return Result of starting the base brake test.
     */
    string StartBrakeTest(void);
    /**
     * Stop testing the base brake test.
     * 
     * @return Result of stopping the base brake test.
     */
    string StopBrakeTest(void);
    /**
     * Begin the valve firing portion of the ABS test.
     * 
     * @return Result of starting the ABS valve firing portion of the test.
     */
    string StartAbsValveFiringTest(void);
    /**
     * Stop the ABS valve firing sequence.
     * The ICM data array will be tagged where the sequence ends.
     * 
     * @return Result of stopping the valve firing sequence.
     */
    string StopAbsValveFiring(void);
    /**
     * Start the parking brake test.
     * 
     * @return Result of starting the parking brake test.
     */
    string StartParkBrakeTest(void);
    /**
     * Stop the park brake test sequence.  The roller forces will be analyzed to find the maximum force on each roller during
     * the park brake testing.
     * 
     * @return Result of stopping the park brake test.
     */
    string StopParkBrakeTest(void);
    /**
     * Perform the max brake force test.
     * 
     * @return Result of performing the brake force test.
     */
    string PerformMaxBrakeTest(void);
    /**
     * Begin the traction control test.
     * The ICM data array will be tagged with the start of the test and the index will be saved in a member variable for
     * later analysis.
     * 
     * @return Result of tagging the start of the traction control test.
     */
    string StartTractionControlTest(void);
    /**
     * End the traction control testing.  The ICM force array will be tagged with the end of the traction control test.
     * The end index will be used in later analysis of the traction control forces.
     * 
     * @return Result of ending the traction control test.
     */
    string StopTractionControlTest(void);
    /**
     * Start the max brake force teting.  The ICM array will be tagged with the start of the test.
     * 
     * @return Result of starting the max brake force test.
     */
    string StartMaxBrakeForceTest(void);
    /**
     * End the max brake force test.
     * The ICM data array will be tagged with the end of the test.  The forces will be averaged and reported to the 
     * test result server.
     * 
     * @return Result of ending the max brake force test.
     */
    string StopMaxBrakeForceTest(void);

private:
    /** Index of the start of the park brake test */
    INT32 m_parkBrakeStartIndex;
    /** Index of the end of the park brake test */
    INT32 m_parkBrakeEndIndex;
    /** Index of the start of the traction control test */
    INT32 m_tractionControlStartIndex;
    /** Index of the end of the traction control test */
    INT32 m_tractionControlEndIndex;
    /** Index of the start of the max brake force test */
    INT32 m_maxBrakeForceStartIndex;
    /** Index of the end of the max brake force test */
    INT32 m_maxBrakeForceEndIndex;
    /** Index of the start of the drag test */
    INT32 m_dragTestStartIndex;
};

#endif // GMBrakeTc_h
