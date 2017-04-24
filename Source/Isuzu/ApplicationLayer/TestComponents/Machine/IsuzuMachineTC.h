//*************************************************************************
// Description:
//   Rolls test machine component for performing machine based testing.
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Isuzu/ApplicationLayer/TestComponents/Machine/Include/IsuzuMachineTC.h $
// 
//
// 1     7/27/16 2:23a kstutzma
// Initial Checkin
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ISUZUMACHINECOMPONENT_H
#define ISUZUMACHINECOMPONENT_H

#include "MachineTC.h"

/**
 * IsuzuMachineTC class that is responsible for performing machine based 
 * responsibilities during a vehicle test.
 *
 */
class IsuzuMachineTC : public MachineTC
{

public:
    /**
     * <b>Description</b><br>
     * Machine component constructor.
     */
    IsuzuMachineTC ();
    /**
     * <b>Description</b><br>
     * Machine component destructor.
     */
    virtual ~IsuzuMachineTC ();

protected:

    /**
     * Execute the next test step.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * The next test step will be exectued.  The test step result will be returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GenericTC::CommandTestStep() </li>
     *      </ul>
     * 
     * @param value The value that was commanded by the test sequencer.
     * @return The result of the test step.
     */
    virtual const std::string CommandTestStep(const std::string &value);
    
private:

    std::string TransitionToRearAxle(void);
    std::string ReportSideSlipValue(void);
    std::string ReportSteeringWheelAngle(void);
    std::string AttachSteeringWheelAngleDevice(void);
};

#endif
