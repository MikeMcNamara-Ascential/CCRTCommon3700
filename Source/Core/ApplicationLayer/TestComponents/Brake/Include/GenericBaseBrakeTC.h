//*************************************************************************
// FILE:
//    $Header: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Brake/Include/GenericBaseBrakeTC.h 1     11/01/07 2:12p Derickso $
//
// FILE DESCRIPTION:
// This file contains the functions necessary to perform a base brake test on
// the machine.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Brake/Include/GenericBaseBrakeTC.h $
// 
// 1     11/01/07 2:12p Derickso
// Updated comments to doxygen format.
// 
// 1     10/05/07 3:27p Derickso
// Updated header comments for doxygen format.
// 
// 3     10/25/06 3:51p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added Deactivate() and WarmInitialize().  These methods are used
// to          keep the test components running when a new drive curve
// starts.
// 
// 2     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 1     12/21/03 6:33p Cward
//
// 3     10/29/03 6:22a Derickso
// Added Initialize() method since GenericTC::Initialize()
// does not call InitializeHook().
//
// 2     9/22/03 5:32p Derickso
// Updated to support latest architecture and use BaseBrakeTools.
//
// 1     8/21/03 8:59a Derickso
// 05WK CCRT Project
//
// 1     8/14/03 7:46a Khykin
// Updated with new naming convention from TestComponent to TC.
//
// 2     8/12/03 2:42p Khykin
// Updated with new directory update changes.
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//    Revision 1.4  2003/06/05 21:43:04  tenharmsel
//    Made TestStepAccelerate virtual
//
//    Revision 1.3  2003/05/14 20:02:33  swieton
//    Pit conditionals working
//
//    Revision 1.1  2003/04/04 21:57:01  bereza
//    added
//
//
// 1     1/27/03 11:09a Khykin
// Initial Checkin
//
// 1     1/18/03 7:17a Khykin
// Initial Checkin
//
// 7     11/22/02 3:09p Khykin
// Adding logging information.
//
// 6     11/04/02 6:48a Khykin
// Updated for the addition of new methods to perform force target
// updates.
//
// 5     8/23/02 8:01a Kh
// Synchronizing the pit machine with VSS.
//
// 3     3/25/02 6:16p Kh
// Updating due to changes in the base classes.
//
// 2     3/06/02 1:55p Kh
// Updating for subclassing the DataArrayTC.
//
// 1     2/06/02 12:16a Kh
// Initial Checkin
//
//*************************************************************************

#ifndef GENERICBASEBRAKE_H
#define GENERICBASEBRAKE_H

#include "BaseBrakeTool.h"
#include "GenericTC.h"
#include <strstream.h>
#include <math.h>

using std::string;

/**
 * Define that indicates if the wheelspeeds should be read
 * using IPC or direct read.  This is primarily for component
 * debugging.
 *
 * Note: For Release set to 0
 * @since Version 1.0
 */
#define COMPONENT_TESTING   0

/**
 * GenericBaseBrakeTC component that is responsible for perfroming
 * a base brake test on a vehicle.  It is mostly a front end for the
 * Base Brake utility class, BaseBrakeTool.
 *
 * @author Kevin Hykin
 */
class GenericBaseBrakeTC : public GenericTC
{
    friend class BaseBrakeTool;
public:
    /**
     * GenericBaseBrakeTC constructor.
     * A new BaseBrakeTool object will be created in the constructor.
     */
    GenericBaseBrakeTC();
    /**
     * GenericBaseBrakeTC destructor.
     * The BaseBrakeTool object will be destroyed in the destructor.
     */
    virtual ~GenericBaseBrakeTC();
    /**
     * Initialize the base brake component.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The test component will be initialized and inform the system when it has completed initializing.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> GetProcessName() </li>
     *      <li> GenericTC::Initialize() </li>
     *      <li> InitializeHook() </li>
     *      <li> InitializationComplete() </li>
     *      </ul>
     *
     * @param config The test component configuration file to be used for initialization.
     */
    virtual void Initialize(const XmlNode *config);
    /**
     * Specific initialization steps for this component.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This function will perform specific initialization items for the GenericBaseBrake test component.  The force thread
     * will be launched during this phase of the initialization.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetComponentName() </li>
     *      <li> LaunchForceThread() </li>
     *      </ul>
     *
     * @param configFile Configuration file to use for initialization.
     */
    const void InitializeHook(const XmlNode *config);

    /**
     * Reinitialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Reinitialize using the already loaded configuration file. This is intended to be used when a component is 
     * activated and there have been no modifications to any configuration files used by the component.
     * Member variables that store status during the test will also be reset to initial values.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::WarmInitialize() </li>
     *      <li> GenericTC::WarmInitialize() </li>
     *      </ul>
     */
    virtual void WarmInitialize();

    /**
     * Deactivate the test component.
     * <p><b>Category:</b> Utility
     * Indicate that the component should de-activate itself. This should
     * happen at the end of a test when the component is no longer needed
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::Deactivate() </li>
     *      <li> GenericTC::Deactivate() </li>
     *      </ul>
     */
    virtual void Deactivate();
    
    /**
     * Reset the test component to run a vehicle test or retest.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The component will be reset to a test not started state in preparation for a retest or a new vehicle test sequence.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::Reset() </li>
     *      <li> GenericTC::Reset() </li>
     *      </ul>
     */
    void Reset(void);

    /**
     * Sequence a test step.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     *
     * <b>Test Step Functions Called:</b>
     *      <ul>
     *      <li> TestStepInitialize() </li>
     *      <li> TestStepAccelerate() </li>
     *      <li> TestStepCoast() </li>
     *      <li> TestStepDrag() </li>
     *      <li> TestStepAnalyzeDrag() </li>
     *      <li> TestStepBaseBrake() </li>
     *      <li> TestStepAnalyzeBaseBrake() </li>
     *      <li> TestStepBalance() </li>
     *      <li> TestStepBrakeToStop() </li>
     *      <li> TestStepFinishUp() </li>
     *      <li> ExcessiveBrakeForceCheck() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StatusCheck() </li>
     *      <li> GetComponentName() </li>
     *      <li> GetTestStepName() </li>
     *      <li> GenericTC::CommandTestStep() </li>
     *      </ul>
     *
     * @param value    This is the value that was commanded by the test sequencer.
     * @return  The result of the test step that was executed.
     */
    virtual const std::string CommandTestStep(const std::string &value);

protected:
    /**
     * Initialize the test component.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step is responsible for validating that the machine is ready to perform a brake test sequence.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepInitialize()
     *      </ul>
     *
     * @return The test result of the initialization test step.
     */
    const string TestStepInitialize(void);
    /**
     * Accelerate to the brake test speed.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step is responsible for prompting the operator to achieve the brake test arm speed.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepAccelerate() </li>
     *      </ul>
     *
     * @return The result of the acceleration test step.
     */
    virtual const string TestStepAccelerate(void);
    /**
     * Sample the drag forces.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step is responsible for sampling the drag forces of the vehicle.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepDrag() </li>
     *      </ul>
     *
     * @return The result of sampling the drag forces.
     */
    const string TestStepDrag(void);
    /**
     * Analyze the sampled drag forces.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The sampled drag forces will be analyzed to ensure they are within limits.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepAnalyzeDrag() </li>
     *      </ul>
     *
     * @return The result of analyzing the drag forces.
     */
    const string TestStepAnalyzeDrag(void);
    /**
     * Perform the base brake test.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Perform the base brake test.  The brake forces will be sampled during this test step.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepBaseBrake() </li>
     *      </ul>
     *
     * @return The result of sampling the base brake forces.
     */
    const string TestStepBaseBrake(void);
    /**
     * Analyze the base brake force samples.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step will analyze the results from the base brake test.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepAnalyzeBaseBrake() </li>
     *      </ul>
     *
     * @return The result of analyzing the base brake forces.
     */
    const string TestStepAnalyzeBaseBrake(void);
    /**
     * Calculate the brake balance.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The brake balances will be calculated and reported to the system.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepBalance() </li>
     *      </ul>
     *
     * @return The result of calculating the brake balances.
     */
    const string TestStepBalance(void);
    /**
     * Prompt the operator to brake to stop.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * This test step will prompt the driver to brake to stop and wait until zerospeed is achieved or a timeout occurs.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepBrakeToStop() </li>
     *      </ul>
     *
     * @return The result of waiting for the operator to stop the vehicle.
     */
    const string TestStepBrakeToStop(void);
    /**
     * Finish up the brake testing.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * Complete the brake testing and report all results.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepFinishUp() </li>
     *      </ul>
     *
     * @return The result of completing the test sequence.
     */
    const string TestStepFinishUp(void);
    /**
     * Engage the machine.
     * <br><p>
     * <b>Category:</b> Test Step
     * <br><p>
     * <b>Description:</b><br>
     * If the machine has been disengaged from the motors, engage them.
     * <br><p>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BaseBrakeTool::TestStepEngage() </li>
     *      </ul>
     *
     * @return The result of completing the test sequence.
     */
    const string TestStepEngageMachine(void);
    /**
     * <b>Description:</b>
     * Method used to abort the current processing based on the needs of the 
     * component.
     */
    virtual void Abort(void);

    /**
     * The Base Brake utility object that contains most of the funtionality
     * for base brake testing
     */
    BaseBrakeTool *m_baseBrakeTool;
};

#endif  // GENERICBASEBRAKE_H
