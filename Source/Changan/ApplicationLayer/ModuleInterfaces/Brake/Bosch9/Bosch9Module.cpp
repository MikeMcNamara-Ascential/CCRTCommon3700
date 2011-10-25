//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch9Module.h 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//	Module Interface for Bosch 9 ABS controller.
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
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch9Module.h $
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

#include "Bosch9Module.h"

template <class ProtocolFilterType>
Bosch9Module<ProtocolFilterType>::Bosch9Module(void) : Bosch8Module<ProtocolFilterType>()
{
}

template <class ProtocolFilterType>
Bosch9Module<ProtocolFilterType>::~Bosch9Module()
{
}


template <class ProtocolFilterType>
BEP_STATUS_TYPE Bosch9Module<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    SerialString_t response_dtcCount(255, 0);
    BEP_STATUS_TYPE status;
    UINT16     dtcCount=0, dtcIdx=0;
    UINT32    dtcVal;
    char    dtcStr[16];

    // Check to see that all our objects are in place
    CheckObjectsStatus();
    
    status = m_protocolFilter->GetModuleData("ReadNumberOfFaults", response_dtcCount);

    if (status == BEP_STATUS_SUCCESS)
    {
        // Extract the number of DTCs from the positive response...
        // e.g.    response_dtcCount = 0x59,0x01,0xFB,0x00,0x??,0x??
        dtcCount = ((response_dtcCount[4] << 8) & 0xFF00) | response_dtcCount[5];
        Log( LOG_DEV_DATA, "Module is reporting %d DTCs\n", dtcCount);

        //since reading the number of faults was successful let read the faults themselves.
        status = m_protocolFilter->GetModuleData("ReadFaults", response);
    }

    if (status == BEP_STATUS_SUCCESS)
    {
        // Extract the DTCs from the respnse   
        //  e.g.   rosponse = 0x59,0x02,0xMM (0x??,0x??,0x??) 0x??, (0x??,0x??,0x??) 0x?? ...

        //Chomp off the first 2 bytes (they are the positive response and the subfunction mask
        response.erase( 0,2);

        for ( dtcIdx=0; (dtcIdx<dtcCount) && (response.size()>1); dtcIdx++)
        {
            //Chomp off the first byte (it is either the status availability mask or the previous DTC's status
            response.erase( 0,1);  
            dtcVal = ((response[0] << 16) & 0xFF0000) | ((response[1] << 8) & 0xFF00) | response[2];
            Log( LOG_DEV_DATA, "\tDTC %d: 0x%06X\n", dtcIdx, dtcVal);
            // Erase the bytes for this DTC
            response.erase( 0,3);

            // Add the DTC to the fault list
            std::string faultStr( CreateMessage(dtcStr, 16, "%04X", dtcVal));
            FaultCode_t fault(faultStr);
            faultCodes.push_back( fault);
        }

        // If we did not receive the full DTC list from the module
        if ( dtcIdx < dtcCount)
        {
            // Log a message
            Log( LOG_ERRORS, "Did not receive full list of DTCs: %d of %d DTCs received\n",
                 dtcIdx, dtcCount);
        }

        return(BEP_STATUS_SUCCESS);
    }
    else
    {
        return(status);
    }
}

