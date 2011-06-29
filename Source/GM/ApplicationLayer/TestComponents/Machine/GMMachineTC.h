//******************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
// GM specific machine component
//
//==============================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef GMMachineTC_h
#define GMMachineTC_h

#include "MachineTC.h"
#include "IMotorController.h"

//==============================================================================
                
/**
 * GMMachineTC class that is responsible for performing
 * machine based responsibilities during a vehicle test.
 * 
 * @author Dan Erickson
 */
class GMMachineTC : public MachineTC
{
public:
	/**
	 * GM Machine Component Constructor.
	 */
    GMMachineTC();
	/**
	 * GM Machine Component Destructor.
	 */
    virtual ~GMMachineTC();

protected:
    /**
      * Run the axle ratio test.  We prompt the driver to accelerate to the axle
      * ratio speed.  Then we sample the four rolls and calculate the axle
      * ratio.
      *
      * @param value This is the value that was commanded by the test sequencer.
      * @return The result of the test.
      */
    virtual const string TestStepAxleRatio(const string &value);
};

#endif	// GMMachineTC_h
