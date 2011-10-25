//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Include/Bosch8TC.h 3     5/23/06 11:28a Gswope $
//
// FILE DESCRIPTION:
//		ABS test component for the Bosch8 system.
//
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Include/Bosch8TC.h $
// 
// 3     5/23/06 11:28a Gswope
// 1) Added CMABSValveFiringTest test step and associated function. this test
//  fires the rear valves first. It is NOT being used at present, as there was
//  no improvement over the previous method.
// 2) Modified default values for initial speed deltas to a realistic value
//  This allows the read to be commented out to simplify debug of valve firing
// 
// 2     5/15/06 8:32a Gswope
// fixed cascade of fail logic in ABSValveFiringTest()
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 3     1/30/06 3:41p Gswope
// in EvaluateSensorCross(), modified second comparisons (two places) to 
// improve quality of evaluation
// 
// 2     3/02/05 11:17a Gswope
// Emergency request from HMMA to remove rear valve firing from ESP test
// 2005.02.28 ews removed at HMMA emergency request
// changed ESP valve firing to use only front wheels in method
// ESPValveFiringTest()
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 5     11/24/04 11:03a Bmeinke
// Updated the individual ABS test methods to report inlet/outlet valve
// actuation results as required per the HMMA QMS spec
// Updated the EvaluateSensorCross() method to report front sensor cross,
// rear sensor cross and front-to-rear sensor cross results as required
// per the HMMA QMS spec
// General beautification
// 
// 3     11/15/04 9:21a Bmeinke
// Added ESPSteps_t enum to define the steps required during an ESP valve
// firing test
// Allocate the m_ESPIndex array in the constructor and delete it in the
// destructor
// Added handlers for EvaluateSensorCross, ReadSpeedDeltas and
// StopESPPumpMotor inside CommandTestStep
// Added ReadSpeedDeltas() method
// Call UpdatePrompts()/RemovePrompts() at the beginning/end of the
// ESPValveFiringTest() method
// Changed the **ESPTest() methods into state machines using the new
// ESPTestSteps_t
// Removed the local redSpeeds and recSpeeds variables inside the
// **ABSTest() methods. Use the class variable m_absSpeedDeltas instead
// Added new EvaluateSensorCross() method to perform an integrated sensor
// cross check using the speed delta values returned during ABS valve
// actuation.
// Override StopPumpMotor() and BrakeSwitchTest() to call
// UpdatePrompts()/RemovePrompts() at the beginning.end of the test step.
// Added m_initSpeedDelta data member to record the initial decel rate at
// the end of base brake. Used as a reference for the sensor cross
// evaluation
// Added m_absSpeedDeltas array to hold the decel rates during ABS valve
// actuation. Used to evaluate sensor cross.
// 
// 3     11/10/04 1:35a Bmeinke
// Added defines (RED_DELTA_IDX and REC_DELTA_IDX) for the wheel speed
// delta array indices
// Need to allocate the m_ESPIndex array in the constructor and delete it
// in the destructor
// Fixed the m_ESPIndex[LFWHEEL} typo in RFESPTest()
// Changed the integrated sensor cross algorithm to use the speed deltas
// obtained during the recovery cycles instead of the ones received during
// the reduction cycle
// 
// 2     10/27/04 8:59p Bmeinke
// Added EvaluateSensorCross test step and method.
// 
// 1     10/27/04 5:40p Bmeinke
// 
// 2     10/26/04 1:54p Bmeinke
// Fixed the names of the parameters used for the SensorQuality test
// Declutch the machine at the end of the SensorQuality test (instead of
// commanding the motors to zero speed) to avoid huge force spikes
// In LFAbsTest(), RFAbsTest(), LRAbsTest() and RRAbsTest(), we need to do
// a signed comparison of the wheel speed deltas to properly evaluate
// sensor cross
// ALWAYS tag the end of the recovery- even if testResult is not testPass
// 
// 1     8/30/04 7:24p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
//
//*************************************************************************
#ifndef Bosch9TC_h
#define Bosch9TC_h

#include "Bosch8TC.h"
#include "ModuleException.h"


template <class ModuleType>
class Bosch9TC : public Bosch8TC<ModuleType>
{

public:
    /**
     * Constructor
     * Nothing special to do here.
     */
    Bosch9TC();

    /**
     * Destructor
     * Nothing special to do here.
     */
    virtual ~Bosch9TC();

};

#endif



