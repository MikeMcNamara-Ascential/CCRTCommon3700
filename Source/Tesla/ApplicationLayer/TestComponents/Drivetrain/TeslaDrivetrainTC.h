//******************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
// Tesla specific machine component
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

#ifndef TeslaDrivetrainTC_h
#define TeslaDrivetrainTC_h

#include "GenericTC.h"

//==============================================================================
                
/**
 * TeslaDrivetrainTC class that is responsible for performing
 * machine based responsibilities during a vehicle test.
 * 
 * @author Dan Erickson
 */
class TeslaDrivetrainTC : public GenericTC
{
public:
	/**
	 * GM Drivetrain Component Constructor.
	 */
    TeslaDrivetrainTC();
	/**
	 * GM Drivetrain Component Destructor.
	 */
    virtual ~TeslaDrivetrainTC();
    /**
     * Initialize the test component.
     * <p><b>Category:</b> Utility
     * <p><b>Description</b><br>
     * Initialization method to set up the MachineTC component.  The base class will be called to begin the
     * initialization.  The maximum boost pressure will then be saved off to a member variable.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericTC::Initialize() </li>
     *      <li> InitializationComplete() </li>
     *      </ul>
     * 
     * @param config Configuration data to use for initializing the test component.
     */
    void Initialize(const XmlNode *config);
    /**
     * Sequence the next test step.
     * 
     * @param value  Test step value to be used during the test step.
     * 
     * @return Result of the test.
     */
    virtual const string CommandTestStep(const string &value);

protected:
    /**
     * Setup the machine for drive profile testing.
     * The drive axle will be switched so the driven axle of the vehicle can be loaded.  Torque will also be
     * applied to the driven axle.
     * 
     * @return Result of setting up the machine for drive profile testing.
     */
    const string SetupMachineForDriveProfileTest(void);
    /**
     * Reset the machine to initial conditions.
     * The drive axle will be reset to the original drive axle.  The motors will also be placed back into 
     * boost mode.
     * 
     * @return Result of restoring the initial machine conditions.
     */
    const string ResetMachineToInitialCondition(void);
    /**
     * Perform cruise testing.
     * The operator will be instructed to achieve the target speed if the vehicle is not in the correct
     * speed range.  once the proper speed range is achieved, the operator will be instructed to excercise
     * the speed control.
     * 
     * @return Result of performing the speed control test.
     */
    const string Cruise(void);
    /**
     * Instruct the operator to brake to the indicated speed range.
     * 
     * @return Result of braking to the desired speed range.
     */
    const string BrakeToSpeed(void);
    /**
     * The operator will be instructed to achieve the target speed if the vehicle is not in the correct
     * speed range.  The operator will then be instruected to shift the transmission to reverse and
     * verify the reverse warning lamp has been illuminated.
     * 
     * @return Result of verifying the reverse warning lamp.
     */
    const string ReverseWarning(void);
    /**
     * Perform a vehicle check out cycle for the operator.
     * The operator will be prompted to accelerate to the target speed and then brake to stop.
     * This sequence will be repeated multiple times.
     * 
     * @return Result of vehicle checkout sequence.
     */
    const string VehicleCheckOutCycle(void);

private:
    /**
     * Get/Set the original drive axle of the vehicle.
     * 
     * @param driveAxle Drive axle to store.
     * 
     * @return Drive axle of the vehicle.
     */
    const string& OriginalDriveAxle(const string *driveAxle = NULL);
    /** The original drive axle of the vehicle so it can be restored once testing is compelted */
    string m_originalDriveAxle;
};

#endif	// TeslaDrivetrainTC_h
