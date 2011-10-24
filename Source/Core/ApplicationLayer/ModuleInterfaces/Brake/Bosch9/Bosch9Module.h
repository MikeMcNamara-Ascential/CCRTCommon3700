//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch9Module.h 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//	Module Interface for Bosch 8 ABS controller.
//  Taken from Bosch 8 ABS Module controller.  
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
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch8Module.h $
// 
// 1     10/24/11 dmazur
// Using the current Bosch 8 module source this Bosch 9 was created to inherit 
// the Bosch 8 funcionality.
//
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 4     11/15/04 8:52a Bmeinke
// Added a handler for ReadSpeedDeltas inside GetInfo(WheelSpeeds_t)
// Added the new ESP mehod handlers to the generic GetInfo() method
// 
// 1     10/27/04 5:40p Bmeinke
// 
// 3     10/26/04 12:42p Bmeinke
// ALWAYS put 2 WheelSpeed_t entries into the vector inside GetInfo(
// StartSensorQualityTest)
// Inside StartSensorQualityTest(), we do not get a message for "Starting"
// , so don't look for one.
// Inside StartSensorQualityTest(), consider "failed" to meane "not
// passed"
// Parse the module reply inside ReadFaults() the "hard" way (i.e. no
// response interpretaion: too difficult according to Dan E)
// 
// 2     10/15/04 2:02p Bmeinke
// Modified ReadFaults to manually parse the list of faults
// 
// 1     8/30/04 7:23p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
// 
// 1     8/09/04 9:17a Bmeinke
// Removed the RunPumpMotor() method (already provided in
// GenericABSModuleTemplate.h)
// Added Log( FN_ENTRY, ...) calls to EnterDiagnosticMode()
// 
// 1     12/22/03 12:34p Rwiersem
// Initial version.
// 
//*************************************************************************
#ifndef BOSCH9MODULE_H
#define BOSCH9MODULE_H

#include "../Bosch8/Bosch8Module.cpp"

/**
 * Module interface for the Bosch9 ABS module.
 * This class contains all the methods neccessary to send commands and
 * retrieve data from the Bosch9 module.
 * 
 * @author Ross Wiersema
 * @see KoreaABSModuleTemplate
 * @see GenericABSModuleTemplate
 * @see GenericModule
 * @since 19 December 2003
 */
template <class ProtocolFilterType>
class Bosch9Module : public Bosch8Module<ProtocolFilterType>
{
public:

    /**
     * Class constructor.
     * All communication objects will be initialized in this constructor.
     * 
     * @since 19 December 2003
     */
    Bosch9Module(void);

    /**
     * Class destructor.
     * All communication objects will be destroyed in this destructor.
     * @since 19 December 2003
     */
    virtual ~Bosch9Module();

 
private:

    /** 
     * Method to read the faults from the module
     * @param faultCodes Vector to store fault codes fromt the module
     * @return The status of the operation
     */ 
    virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);

};

#endif
