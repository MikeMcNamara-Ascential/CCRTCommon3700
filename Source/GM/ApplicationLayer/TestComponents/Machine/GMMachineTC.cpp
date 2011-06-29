//******************************************************************************
// FILE:
//    $Header: /CMake Core/Source/Ford/ApplicationLayer/TestComponents/Machine/FordMachineTC.cpp 7     7/12/07 12:30p Mmcnamar $
//
// FILE DESCRIPTION:
// Ford specific machine component
//
//==============================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CMake Core/Source/GM/ApplicationLayer/TestComponents/Machine/GMMachineTC.cpp $
// 
// 7     7/12/07 12:30p Mmcnamar
// Fixed after compile error.
// 
// 6     7/12/07 12:22p Mmcnamar
// Updated TestStepAxleRatio() for KTP.
// 
// 5     3/28/07 3:01p Mmcnamar
// Added parameter "AxleRatioSampleCount" in TestStepAxleratio(); Also
// added SystemWrite() call so axle ratio % can be passed to eCATS in the
// DCL engineering data message.
// 
// 4     12/27/06 1:07p Mmcnamar
// Updated for axle artio test: Added TestStepAxleRatio().
// 
// 3     6/15/06 5:54p Bmeinke
// Added a 250ms loop delay inside WaitForWheelbase()
// Added more logging to WaitForWheelbase()
// 
// 2     2/24/06 1:31p Bmeinke
// Beautified
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 3     12/22/04 8:17p Bmeinke
// Moved all park brake and park pawl methods (and supporting routines) to
// GenericTC
// 
// 2     12/14/04 8:53p Bmeinke
// Added a dummy FourByFourTest test step for U152 testing at BEP
// Changed if(ReadSubscribeData(GetDataTag("MachineType3600")) == "1") to
// if(ReadSubscribeData(GetDataTag("MachineType")) ==
// GetDataTag("MachineType3600")) in the ParkPawl and ParkBrakePawl test
// step methods
// Changed the direction specifiers in the ParkBrake / ParkPawl methods
// from Front/Rear to Forward/Reverse
// 
// 1     9/30/04 2:43p Bmeinke
// Initial full core integration for GM
// 
// 5     8/30/04 10:39a Bmeinke
// Added test steps to enable / disable motor loading. The enable motor
// loading test step will also (optionally) set motor loading constants.
// 
// 4     8/17/04 12:24p Bmeinke
// When checking for a required wheelbase position in
// TestStepWaitForWheelbase(), use the base class GetParameterInt()
// method. This is poossible because GetParameterInt() will return 0 if
// the parameter does not exist and 0 is not a valid wheelbase position.
// Hence, a value of 0 for a required wheelbase position means 'no
// wheelbase position required'.
// 
// 3     8/16/04 3:13p Bmeinke
// Modified TestStepWaitForWheelbase() to verify that the correct
// wheelbase position is selected iff a "VehicleWheelbasePosition"
// parameter is present in the config file
// 
// 2     7/21/04 3:23p Bmeinke
// Moved the wheelbase in position check from GMSystemMonitor to this
// component to get rid a test startup timing between us and eCATS
// 
// 1     4/12/04 1:20p Bmeinke
// Changing VSS repositories
// 
// 1     3/05/04 2:27p Bmeinke
// Latest release from March 03,2004 trip to Chicago
// 
// 1     8/14/03 7:37a Khykin
// Initial checkin after name conversion from TestComponent to TC.
// 
// 1     8/13/03 6:32a Khykin
// Updated with new directory structure changes.
// 
// 1     7/08/03 1:40p Derickso
// Restructured for new directory structure.
// 
// 1     6/24/03 7:08a Khykin
// Initial checkin.  These were done to add stack light functionality.
// 
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "GMMachineTC.h"

//-----------------------------------------------------------------------------
GMMachineTC::GMMachineTC() : MachineTC()
{
}

//-----------------------------------------------------------------------------
GMMachineTC::~GMMachineTC()
{
}

//-----------------------------------------------------------------------------
const string GMMachineTC::TestStepAxleRatio(const string &value)
{
    float frontSum = 0;
    float rearSum = 0;
    WHEELINFO currentSpeeds;
    for(INT16 index = 0; index < GetParameterInt("AxleRatioSamples"); index++)
    {   // Sum the front and rear wheel speeds
        if(BEP_STATUS_SUCCESS == GetWheelSpeeds(currentSpeeds))
        {
            frontSum = (currentSpeeds.lfWheel + currentSpeeds.rfWheel) / 2.0;   // Average front wheel speed
            rearSum  = (currentSpeeds.lrWheel + currentSpeeds.rrWheel) / 2.0;   // Average rear wheel speed
            // Wait a bit for the next sample
            BposSleep(GetParameterInt("AxleRatioSampleDelay"));
        }
        else
        {   // Could not read wheel speeds
            Log(LOG_ERRORS, "Could not read wheel speeds for axle ratio test");
        }
    }
    // Collected all samples, analyze the results
    float axleRatio = 
}
