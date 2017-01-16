//******************************************************************************
// FILE:
//    $Header: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/ModuleInterfaces/Brake/FordABSModuleTemplate.cpp 1     2/07/11 4:45p Mmcnamar $
//
// FILE DESCRIPTION:
//  Module Interface for a Ford ABS Controller module.
//
//==============================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/ModuleInterfaces/Brake/FordABSModuleTemplate.cpp $
// 
// 1     2/07/11 4:45p Mmcnamar
// backup c drive source for cross compiler
// 
// 1     2/07/11 4:43p Mmcnamar
// backup c drive
// 
// 1     5/10/10 2:09p Mmcnamar
// initial check-in for new core.
// 
// 7     11/14/08 2:22p Mmcnamar
// Added "ModuleKeepAlive" to method GetInfo(); Added method
// SendModuleKeepAlive(); Updated Security Access so security seed values
// passed into sucurity access message in config file.
// 
// 6     6/16/08 1:47p Jwynia
// Updated to Read pedal travel DID
// 
// 5     5/05/08 2:44p Mmcnamar
// Updated for hybrids: ABS security; Read sensor DID's; Set/clear test
// status  DID.
// 
// 4     3/28/07 1:59p Mmcnamar
// Added SetBrakeSwitchStatusOn() for brake pedal check in
// ParkBrakePawlInit().
// 
// 3     2/24/06 1:17p Bmeinke
// Beautified
// 
// 2     6/30/05 8:19p Bmeinke
// Added ArmAbsRollTestSequence handler to GetInfo(string methodName,
// string &value)
// 
// 1     5/06/05 5:10p Bmeinke
// Moved back here from the Brake/Common directory to match core directory
// structure
// 
// 1     5/06/05 12:58p Bmeinke
// Moved to the Brake directory from the root ModuleTemplate directory
// 
// 2     5/06/05 12:57p Bmeinke
// Added GetInfo(string, SerialString_t) method
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 4     3/11/05 6:05p Bmeinke
// Added AllStoredCodeLogging() and NoStoredCodeLogging() for D219 Bosch
// ABS
// 
// 3     12/22/04 7:42p Bmeinke
// Added InitializeHook() method to initialize the keep alive and switch
// monitor protocol filters
// Removed the ActuateReduction() method
// Moved StopPumpMotor() to GenericABSModuleTemplate
// 
// 2     12/14/04 8:46p Bmeinke
// Overloaded GetInfo(string,string) to look for ReadBrakeSwitch requests
// and convert the bool response into an "On" or "Off" string
// 
// 1     9/30/04 2:42p Bmeinke
// Initial full core integration for Ford
// 
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "GenericABSModuleTemplate.cpp"
#include "FordABSModuleTemplate.h"


template <class ProtocolFilterType>
FordABSModuleTemplate<ProtocolFilterType>::FordABSModuleTemplate() : GenericABSModuleTemplate<ProtocolFilterType>()
{
}

template <class ProtocolFilterType>
FordABSModuleTemplate<ProtocolFilterType>::~FordABSModuleTemplate()
{
}

template <class ProtocolFilterType>
string FordABSModuleTemplate<ProtocolFilterType>::ModuleName()
{
    return("FordABSModuleTemplate");
}

template <class ProtocolFilterType>
bool FordABSModuleTemplate<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{
    // We need a keep alive, so initialize the keep alice protocol filter
    if ( m_keepAliveComm != NULL)
    {
        Log( LOG_DEV_DATA, "FordABSModuleTemplate::InitializeHook: Initializing the keep alive comm object\n");
        m_keepAliveComm->Initialize( configNode);
    }

    if ( m_switchMonitorComm != NULL)
    {
        Log( LOG_DEV_DATA, "FordABSModuleTemplate::InitializeHook: Initializing the switch monitor comm object\n");
        m_switchMonitorComm->Initialize( configNode);
    }
    return( GenericABSModuleTemplate<ProtocolFilterType>::InitializeHook(configNode));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, WheelSpeeds_t &value) throw(ModuleException)
{
    // set the initial value
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Entering FordABSModuleTemplate::GetInfo(%s,WheelSpeeds_t\n", methodName.c_str());

    // Any of these could throw module exception
    // Read the wheel speed sensors
    if (methodName == m_readSensorSpeeds)
    {
        retVal = ReadSensorSpeeds(value);
    }
    else if (methodName == "ReadSensorDIDs")   //MAM 3/18/08
    {
        retVal = ReadSensorDIDs(value);
    }
    else
    {   // No special method, try using the base class
        Log(LOG_DEV_DATA, "FordABSModuleTemplate::GetInfo(WheelSpeeds_t) calling GenericModuleTemplate::GetInfo(WheelSpeeds_t)\n");
        retVal = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit FordABSModuleTemplate::GetInfo(%s,WheelSpeeds_t\n", methodName.c_str());
    // Return the return value
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadSensorDIDs(WheelSpeeds_t &speeds)  throw(ModuleException)
{   
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    const char *tags[] = { "ReadLFSensorSpeed", "ReadRFSensorSpeed", "ReadLRSensorSpeed", "ReadRRSensorSpeed"};
    float sensorSpeeds[ 4];
    int ii;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter ReadSensorDIDs()\n");
    // Check to see that all our objects are in place
    // This throws a ModuleException if things aren't good
    CheckObjectsStatus();

    for ( ii=0; ii<4; ii++)
    {
        speeds[ ii] = 0;

        status = ReadSensorDID( tags[ ii], sensorSpeeds[ii]);

        if ( BEP_STATUS_SUCCESS == status)   speeds[ ii] = sensorSpeeds[ ii];
        else                                Log( LOG_ERRORS, "Error reading %s\n", tags[ii]);
    }

    for ( ii=0; ii<4; ii++)
    {
        Log( LOG_DEV_DATA, "\t%s = %.3f\n", tags[ii], speeds[ii]);
    }

    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit ReadSensorDIDs(), status=%d\n", status);
    // Return the status of the operation
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadSensorDID(const std::string &methodName, float &value)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    SerialString_t response;

    Log(LOG_FN_ENTRY, "Enter ReadSensorDID(%s)\n", methodName.c_str());

    // Default to no data
    value = 0.0;

    // Check to see that all our objects are in place
    // This throws a ModuleException if things aren't good
    CheckObjectsStatus();

    try
    {
        // Send the message and get the response
        status = m_protocolFilter->GetModuleData( methodName, response);
        if ( BEP_STATUS_SUCCESS == status)
        {
            //MAM 10/29/08 - debug
            //Log( LOG_DEV_DATA, "ReadSensorDID response: %2X %2X %2X %2X\n",
            //     response[0],response[1],response[2],response[3]);

            try
            {
                // Parse and interpret the response
                const std::string interpret( m_vehicleMessagesNode->getChild( methodName)->getChild("ReplyInterpretation")->getValue());
                value = ParseFloatResponse(interpret, response);                
            }
            catch ( BepException &parseErr)
            {
                Log( LOG_ERRORS, "Exception parsing %s response: %s\n", methodName.c_str(), parseErr.GetReason());
                status = BEP_STATUS_SOFTWARE;
            }
        }
    }
    catch ( BepException &err)
    {
        Log( LOG_ERRORS, "Exception reading %s: %s\n", methodName.c_str(), err.GetReason());
        status = BEP_STATUS_SOFTWARE;
    }
    Log(LOG_FN_ENTRY, "Exit ReadSensorDID: %s, value=%.3f, status=%d\n", 
        methodName.c_str(), value, status);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, string &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    Log( LOG_FN_ENTRY, "Enter FordABSModuleTemplate::GetInfo( %s, string)\n", methodName.c_str());

    if ( m_readBrakeSwitch == methodName)
    {
        bool    switchOn;

        status = GetInfo( methodName, switchOn);
        if ( status == BEP_STATUS_SUCCESS)
        {
            if ( switchOn == true)   value = "On";
            else                    value = "Off";
        }
        else
        {
            value = "Unknown";
        }
    }
    else if ( methodName == "ArmAbsRollTestSequence")
    {
        SerialString_t  response;
        SerialString_t::iterator    itr;
        status = ArmAbsRollTestSequence( response);

        for ( itr=response.begin(); itr!=response.end() && response.empty() == false; itr++)
        {
            value.push_back( (char)*itr);
        }
    }
    else
    {
        // No special method, try the base class
        status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);
    }
    Log( LOG_FN_ENTRY, "Exit FordABSModuleTemplate::GetInfo( %s, %s), status=%d\n", 
         methodName.c_str(), value.c_str(), status);

    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, SerialString_t &value) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    Log( LOG_FN_ENTRY, "Enter FordABSModuleTemplate::GetInfo( %s, SerialString_t)\n", methodName.c_str());

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    //MAM 3/14/08
    if (methodName == "RequestSecuritySeeds")
    {
        status = RequestSecuritySeeds( value);
    }
    else if (methodName == "RequestBendixSeeds1")
    {
        status = RequestBendixSeeds1( value);
    }
    else if (methodName == "RequestBendixSeeds3")
    {
        status = RequestBendixSeeds3( value);
    }
    else if (methodName == "RequestBendixSeeds7")
    {
        status = RequestBendixSeeds7( value);
    }
    else if (methodName == "RequestBendixSeeds")
    {
        status = RequestBendixSeeds( value);
    }
    else if (methodName == "RequestSecurityAccess")
    {
        status = RequestSecurityAccess( value);
    }
    else if (methodName == "RequestBendixAccess")
    {
        status = RequestBendixAccess( value);
    }
    else if (methodName == "RequestBendixAccess1")
    {
        status = RequestBendixAccess1( value);
    }
    else if (methodName == "RequestBendixAccess3")
    {
        status = RequestBendixAccess3( value);
    }
    else if (methodName == "RequestBendixAccess7")
    {
        status = RequestBendixAccess7( value);
    }
    else if (methodName == "ReadStatusDID")
    {
        status = ReadStatusDID( value);
    }
    else if (methodName == "ReadPedalTravel")
    {
        status = ReadPedalTravel( value);
    }
    else if (methodName == "ReadCurrentTireSizes")
    {
        status = ReadCurrentTireSizes( value);
    }
    else if (methodName == "ReadCurrentTireSizesEC80")
    {
        status = ReadCurrentTireSizesEC80( value);
    }
    else if (methodName == "ReadConfigBytes45")
    {
        status = ReadConfigBytes45( value);
    }
    else if (methodName == "ReadLampGndChkEC80")
    {
        status = ReadLampGndChkEC80( value);
    }
    else if (methodName == "ReadHrwCcvs")
    {
        status = ReadHrwCcvs( value);
    }
    else if (methodName == "ReadHrwCcvsEC80")
    {
        status = ReadHrwCcvsEC80( value);
    }
    else if (methodName == "ReadModulePartNumber")
    {
        status = ReadABSPartNumber( value);
    }
    else if (methodName == "ReadModuleSwId")
    {
        status = ReadABSSwId( value);
    }
    else if (methodName == "ReadModuleSn")
    {
        status = ReadABSSn( value);
    }
    else if (methodName == "ReadPartNumber")
    {
        status = ReadABSPartNumber( value);
    }
    else if (methodName == "ClearStatusDID")
    {
        status = ClearStatusDID( value);
    }
    else if (methodName == "SetStatusDIDPass")
    {
        status = SetStatusDIDPass( value);
    }
    else if (methodName == "WriteFingerprint")
    {
        status = WriteFingerprint( value);
    }
    //else if (methodName == "WriteTireSize")
    //{
    //    status = WriteTireSize( value);
    //}
    else if (methodName == "ProgramTireSizes")
    {
        status = ProgramTireSizes( value);
    }
    else if (methodName == "ProgramTireSizesEC80")
    {
        status = ProgramTireSizesEC80( value);
    }
    else if (methodName == "WheelAlignAddAxle")
    {
        status = WheelAlignAddAxle( value);
    }
    else if (methodName == "WheelAlignSDAxles")
    {
        status = WheelAlignSDAxles( value);
    }
    else if (methodName == "ProgramConfigBytes45")
    {
        status = ProgramConfigBytes45( value);
    }
    else if (methodName == "ProgramLampGndChkEC80")
    {
        status = ProgramLampGndChkEC80( value);
    }
    else if (methodName == "ProgramHrwCcvs")  //MAM 1/8/15
    {
        status = ProgramHrwCcvs( value);
    }
    else if (methodName == "ProgramHrwCcvsEC80")  //MAM 1/8/15
    {
        status = ProgramHrwCcvsEC80( value);
    }
    else
        status = m_protocolFilter->GetModuleData(methodName, value);

    Log( LOG_FN_ENTRY, "Exit FordABSModuleTemplate::GetInfo( %s, SerialString_t), status=%d\n", 
         methodName.c_str(), status);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, bool &value)  throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    Log( LOG_FN_ENTRY, "Enter FordABSModuleTemplate::GetInfo( %s, bool)\n", methodName.c_str());

    status = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value);

    Log( LOG_FN_ENTRY, "Exit FordABSModuleTemplate::GetInfo( %s, %d), status=%d\n", 
         methodName.c_str(), value, status);
    // No special method, try the base class
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t & value) throw(ModuleException)
{

    //MAM 1/9/15
    Log( LOG_FN_ENTRY, "Enter FordABSModuleTemplate::GetInfo( %s, FaultVector_t)\n", methodName.c_str());

    // No special method, try the base class
    return( GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));

    //MAM 1/6/15
    /*
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;

    if(value.size() != 0)
    {
        value.clear();
    }

    if(methodName == "ReadFaults") status = ReadFaults(value);

    // Return the status
    return status;
    */

}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, int &value) throw(ModuleException)
{
    // No special method, try the base class
    return( GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT8 &value) throw(ModuleException)
{
    // No special method, try the base class
    return( GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT16 &value) throw(ModuleException)
{
    // No special method, try the base class
    return( GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, float &value) throw(ModuleException)
{
    // No special method, try the base class
    return( GenericABSModuleTemplate<ProtocolFilterType>::GetInfo(methodName, value));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::GetInfo(string methodName) throw(ModuleException)
{
    // set the initial value
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    SerialString_t  response;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Entering FordABSModuleTemplate::GetInfo(%s)\n", methodName.c_str());

    if ( methodName == "ExecuteAbsRollTestSequence")
    {
        retVal = ExecuteAbsRollTestSequence( response);
    }
    else if ( methodName == "ArmAbsRollTestSequence")
    {
        retVal = ArmAbsRollTestSequence( response);
    }
    else if (methodName == "AllStoredCodeLogging")
    {
        retVal = AllStoredCodeLogging( response);
    }
    else if (methodName == "NoStoredCodeLogging")
    {
        retVal = NoStoredCodeLogging( response);
    }
    else if (methodName == "DisableTraction")  //MAM 1/8/15
    {
        retVal = DisableTraction( response);
    }
    else if (methodName == "EnableTraction")  //MAM 1/8/15
    {
        retVal = EnableTraction( response);
    }
    else if (methodName == "CycleValves1")  //MAM 1/8/15
    {
        retVal = CycleValves1( response);
    }
    else if (methodName == "CycleValves1Stop")  //MAM 3/31/15
    {
        retVal = CycleValves1Stop( response);
    }
    else if (methodName == "CycleValves2")  //MAM 1/8/15
    {
        retVal = CycleValves2( response);
    }
    else if (methodName == "CycleValves2Stop")  //MAM 3/31/15
    {
        retVal = CycleValves2Stop( response);
    }
    else if (methodName == "CycleValves3")  //MAM 1/8/15
    {
        retVal = CycleValves3( response);
    }
    else if (methodName == "CycleValves3Stop")  //MAM 3/31/15
    {
        retVal = CycleValves3Stop( response);
    }
    else if (methodName == "CycleValves4")  //MAM 1/8/15
    {
        retVal = CycleValves4( response);
    }
    else if (methodName == "CycleValves4Stop")  //MAM 3/31/15
    {
        retVal = CycleValves4Stop( response);
    }
    else if (methodName == "ModuleKeepAlive")  //MAM 10/30/08
    {
        retVal = SendModuleKeepAlive( response);
    }
    else if( "ActivateRBM" == methodName)   retVal = m_protocolFilter->GetModuleData( methodName, response);
    else if( "DeactivateRBM" == methodName)   retVal = m_protocolFilter->GetModuleData( methodName, response);
    //else if( "WriteFingerprint" == methodName) retVal = m_protocolFilter->GetModuleData( methodName, response);
    else
    {
        retVal = GenericABSModuleTemplate<ProtocolFilterType>::GetInfo( methodName);
    }

    Log(LOG_FN_ENTRY, "Exit FordABSModuleTemplate::GetInfo(%s): retVal = %d\n", methodName.c_str(), retVal);

    return( retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ExecuteAbsRollTestSequence( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("ExecuteAbsRollTestSequence", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ArmAbsRollTestSequence( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("ArmAbsRollTestSequence", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::AllStoredCodeLogging( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "AllStoredCodeLogging", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::NoStoredCodeLogging( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "NoStoredCodeLogging", response);

    return( status);
}

//MAM 1/8/14
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::DisableTraction( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "DisableTraction", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::EnableTraction( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "EnableTraction", response);

    return( status);
}

//MAM 1/9/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::CycleValves1( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "CycleValves1", response);

    return( status);
}
//MAM 3/31/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::CycleValves1Stop( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "CycleValves1Stop", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::CycleValves2( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "CycleValves2", response);

    return( status);
}

//MAM 3/31/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::CycleValves2Stop( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "CycleValves2Stop", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::CycleValves3( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "CycleValves3", response);

    return( status);
}

//MAM 3/31/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::CycleValves3Stop( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "CycleValves3Stop", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::CycleValves4( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "CycleValves4", response);

    return( status);
}

//MAM 3/31/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::CycleValves4Stop( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "CycleValves4Stop", response);

    return( status);
}

//MAM 10/30/08
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::SendModuleKeepAlive( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ModuleKeepAlive", response);

    return( status);
}

//MAM 3/14/08
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ClearStatusDID( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ClearStatusDID");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ClearStatusDID", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::SetStatusDIDPass( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate SetStatusDIDPass");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "SetStatusDIDPass", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadStatusDID( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadStatusDID");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadStatusDID", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadCurrentTireSizes( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadCurrentTireSizes");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadCurrentTireSizes", response);

    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::ReadCurrentTireSizes: front=%2X rear=%2X additional=%2X \n",response[7],response[8],response[9]);
    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ReadCurrentTireSizes: status = %d ",status);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadCurrentTireSizesEC80( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadCurrentTireSizesEC80");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadCurrentTireSizesEC80", response);

    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::ReadCurrentTireSizesEC80: steer=%2X drive=%2X additional=%2X \n",response[9],response[13],response[17]);
    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ReadCurrentTireSizesEC80: status = %d ",status);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadConfigBytes45( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadConfigBytes45");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadConfigBytes45", response);

    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::ReadConfigBytes45: byte4=%2X byte5=%2X \n",response[7],response[8]);
    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ReadConfigBytes45: status = %d ",status);

    return( status);
}

//mam 2/10/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadLampGndChkEC80( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadLampGndChkEC80");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadLampGndChkEC80", response);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ReadLampGndChkEC80: status = %d ",status);

    return( status);
}

//MAM 1/8/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadHrwCcvs( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadHrwCcvs");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadHrwCcvs", response);

    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::ReadHrwCcvs: byte4=%2X byte5=%2X byte5=%2X \n",response[7],response[8],response[9]);
    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ReadHrwCcvs: status = %d ",status);

    return( status);
}

//MAM 2/10/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadHrwCcvsEC80( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadHrwCcvsEC80");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadHrwCcvsEC80", response);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ReadHrwCcvsEC80: status = %d ",status);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadABSPartNumber( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadABSPartNumber");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadPartNumber", response);

    //Log(LOG_FN_ENTRY, "FordABSModuleTemplate::ReadABSPartNumber: byte1=%2X byte2=%2X byte3=%2X byte4=%2X byte5=%2X byte6=%2X byte7=%2X byte8=%2X byte9=%2X byte10=%2X \n"
    //    ,response[7],response[8],response[9],response[10],response[11],response[12],response[13],response[14],response[15],response[16]);
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit FordABSModuleTemplate ReadABSPartNumber: status = %d ",status);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadABSSwId( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadABSSwId");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadSwId", response);

    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::ReadABSSwId: byte1=%2X byte2=%2X byte3=%2X byte4=%2X byte5=%2X byte6=%2X byte7=%2X byte8=%2X byte9=%2X byte10=%2X \n"
        ,response[7],response[8],response[9],response[10],response[11],response[12],response[13],response[14],response[15],response[16]);
    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit FordABSModuleTemplate ReadABSSwId: status = %d ",status);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadABSSn( SerialString_t &response) throw(ModuleException)
{

    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadABSSn");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadEcuSn", response);

    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::ReadABSSn: byte1=%2X byte2=%2X byte3=%2X byte4=%2X byte5=%2X byte6=%2X byte7=%2X byte8=%2X byte9=%2X byte10=%2X \n"
        ,response[7],response[8],response[9],response[10],response[11],response[12],response[13],response[14],response[15],response[16]);
    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit FordABSModuleTemplate ReadABSSn: status = %d ",status);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadPedalTravel( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate ReadPedalTravel");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "ReadPedalTravel", response);

    return( status);
}


template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestSecuritySeeds( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate RequestSecuritySeeds");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "RequestSecuritySeeds", response);
    //MAM 10/29/08
    //Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestSecuritySeeds: seed1=%2X seed2=%2X seed3=%2X \n",response[1],response[2],response[3]);
    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestSecuritySeeds: seed1=%2X seed2=%2X seed3=%2X \n",response[2],response[3],response[4]);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestSecuritySeeds: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestBendixSeeds( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate RequestBendixSeeds");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    //status = m_protocolFilter->GetModuleData( "RequestSecuritySeeds", response);
    status = m_protocolFilter->GetModuleData( "RequestBendixSeeds", response);
    //MAM 10/29/08
    //Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestSecuritySeeds: seed1=%2X seed2=%2X seed3=%2X \n",response[1],response[2],response[3]);
    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestBendixSeeds: seed1=%2X seed2=%2X \n",response[6],response[7]);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestBendixSeeds: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestBendixSeeds1( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate RequestBendixSeeds1");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    //status = m_protocolFilter->GetModuleData( "RequestSecuritySeeds", response);
    status = m_protocolFilter->GetModuleData( "RequestBendixSeeds1", response);
    //MAM 10/29/08
    //Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestSecuritySeeds: seed1=%2X seed2=%2X seed3=%2X \n",response[1],response[2],response[3]);
    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestBendixSeeds1: seed1=%2X seed2=%2X \n",response[6],response[7]);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestBendixSeeds1: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestBendixSeeds3( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate RequestBendixSeeds3");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    //status = m_protocolFilter->GetModuleData( "RequestSecuritySeeds", response);
    status = m_protocolFilter->GetModuleData( "RequestBendixSeeds3", response);
    //MAM 10/29/08
    //Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestSecuritySeeds: seed1=%2X seed2=%2X seed3=%2X \n",response[1],response[2],response[3]);
    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestBendixSeeds3: seed1=%2X seed2=%2X \n",response[6],response[7]);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestBendixSeeds3: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestBendixSeeds7( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate RequestBendixSeeds7");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    //status = m_protocolFilter->GetModuleData( "RequestSecuritySeeds", response);
    status = m_protocolFilter->GetModuleData( "RequestBendixSeeds7", response);
    //MAM 10/29/08
    //Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestSecuritySeeds: seed1=%2X seed2=%2X seed3=%2X \n",response[1],response[2],response[3]);
    Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestBendixSeeds7: seed1=%2X seed2=%2X \n",response[6],response[7]);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestBendixSeeds7: status = %d ",status);
    return( status);
}

//MAM 3/12/08
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestSecurityAccess( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    //SerialString_t  requestSecurity;
    //MAM 10/28/08 - Changed so we pass in the security access bytes to the variables in the config file msg.
    SerialArgs_t    requestSecurity;

    Log(LOG_FN_ENTRY, "Enter RequestSecurityAccess");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    //Assemble the security access message
    /*
    requestSecurity.push_back(0x07);
    requestSecurity.push_back(0x60);
    requestSecurity.push_back(0x27);
    requestSecurity.push_back(0x02);
    requestSecurity.push_back(response[1]);
    requestSecurity.push_back(response[2]);
    requestSecurity.push_back(response[3]);
      */
    requestSecurity.clear();
    requestSecurity.push_back((UINT8)response[2]);
    requestSecurity.push_back((UINT8)response[3]);
    requestSecurity.push_back((UINT8)response[4]);

    //Log(LOG_FN_ENTRY, "FordABSModuleTemplate::RequestSecurityAccess: Sending requestSecurity message\n");
    //Log(LOG_FN_ENTRY, "%2X %2X %2X %2X %2X %2X %2X \n", requestSecurity[0],requestSecurity[1],requestSecurity[2],
    //    requestSecurity[3],requestSecurity[4],requestSecurity[5],requestSecurity[6]);

    //status = m_protocolFilter->SendMessage( requestSecurity);
    //MAM 10/28/08 - calling GetModuleData() allows us to evaluate the response
    status = m_protocolFilter->GetModuleData( "RequestSecurityAccess",response,&requestSecurity);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestSecurityAccess: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestBendixAccess( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    requestSecurity;

    Log(LOG_FN_ENTRY, "Enter RequestBendixAccess");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    requestSecurity.clear();
    requestSecurity.push_back((UINT8)response[2]);
    requestSecurity.push_back((UINT8)response[3]);
    requestSecurity.push_back((UINT8)response[4]);

    //status = m_protocolFilter->GetModuleData( "RequestSecurityAccess",response,&requestSecurity);
    status = m_protocolFilter->GetModuleData( "RequestBendixAccess",response,&requestSecurity);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestBendixAccess: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestBendixAccess1( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    requestSecurity;

    Log(LOG_FN_ENTRY, "Enter RequestBendixAccess1");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    requestSecurity.clear();
    requestSecurity.push_back((UINT8)response[2]);
    requestSecurity.push_back((UINT8)response[3]);
    requestSecurity.push_back((UINT8)response[4]);

    //status = m_protocolFilter->GetModuleData( "RequestSecurityAccess",response,&requestSecurity);
    status = m_protocolFilter->GetModuleData( "RequestBendixAccess1",response,&requestSecurity);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestBendixAccess1: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestBendixAccess3( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    requestSecurity;

    Log(LOG_FN_ENTRY, "Enter RequestBendixAccess3");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    requestSecurity.clear();
    requestSecurity.push_back((UINT8)response[2]);
    requestSecurity.push_back((UINT8)response[3]);
    requestSecurity.push_back((UINT8)response[4]);

    //status = m_protocolFilter->GetModuleData( "RequestSecurityAccess",response,&requestSecurity);
    status = m_protocolFilter->GetModuleData( "RequestBendixAccess3",response,&requestSecurity);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestBendixAccess3: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::RequestBendixAccess7( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    requestSecurity;

    Log(LOG_FN_ENTRY, "Enter RequestBendixAccess7");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    requestSecurity.clear();
    requestSecurity.push_back((UINT8)response[2]);
    requestSecurity.push_back((UINT8)response[3]);
    requestSecurity.push_back((UINT8)response[4]);

    //status = m_protocolFilter->GetModuleData( "RequestSecurityAccess",response,&requestSecurity);
    status = m_protocolFilter->GetModuleData( "RequestBendixAccess7",response,&requestSecurity);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit RequestBendixAccess7: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::SetBrakeSwitchStatusOn(void)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering SetBrakeSwitchStatusOn", ModuleName().c_str());
    m_sawBrakeSwitchOn = true;
    m_sawBrakeSwitchOff = false;
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting SetBrakeSwitchStatusOn", ModuleName().c_str());
    // Return the status of the operation
    return BEP_STATUS_SUCCESS;
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::WriteFingerprint( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status;

    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter FordABSModuleTemplate WriteFingerprint");

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData( "WriteFingerprint", response);

    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ProgramTireSizes( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    tireSizes;

    Log(LOG_FN_ENTRY, "Enter ProgramTireSizes");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    tireSizes.clear();
    tireSizes.push_back((UINT8)response[0]);
    tireSizes.push_back((UINT8)response[1]);
    tireSizes.push_back((UINT8)response[2]);

    status = m_protocolFilter->GetModuleData( "ProgramTireSizes",response,&tireSizes);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ProgramTireSizes: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ProgramTireSizesEC80( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    configBytes;

    Log(LOG_FN_ENTRY, "Enter ProgramTireSizesEC80 ");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    //copy all data bytes into config bytes
    Log(LOG_FN_ENTRY, "ProgramTireSizesEC80 - Updating config bytes");
    configBytes.clear();
    configBytes.push_back((UINT8)response[0]);
    configBytes.push_back((UINT8)response[1]);
    configBytes.push_back((UINT8)response[2]);
    configBytes.push_back((UINT8)response[3]);
    configBytes.push_back((UINT8)response[4]);
    configBytes.push_back((UINT8)response[5]);
    configBytes.push_back((UINT8)response[6]);
    configBytes.push_back((UINT8)response[7]);
    configBytes.push_back((UINT8)response[8]);
    configBytes.push_back((UINT8)response[9]);
    configBytes.push_back((UINT8)response[10]);
    configBytes.push_back((UINT8)response[11]);
    configBytes.push_back((UINT8)response[12]);
    configBytes.push_back((UINT8)response[13]);
    /*
    for (int ii=0; ii<14; ii++)
    {
        configBytes[ii] = (UINT8)response[ii];
    }
      */
    Log(LOG_FN_ENTRY, "ProgramTireSizesEC80 - Sending message to module ");
    status = m_protocolFilter->GetModuleData( "ProgramTireSizesEC80",response,&configBytes);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ProgramTireSizesEC80: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::WheelAlignAddAxle( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    tireSizes;

    Log(LOG_FN_ENTRY, "Enter WheelAlignAddAxle");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    tireSizes.clear();

    tireSizes.push_back((UINT8)response[0]);
    tireSizes.push_back((UINT8)response[1]);

    status = m_protocolFilter->GetModuleData( "WheelAlignAddAxle",response,&tireSizes);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit WheelAlignAddAxle: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::WheelAlignSDAxles( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    tireSizes;

    Log(LOG_FN_ENTRY, "Enter WheelAlignSDAxles");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    tireSizes.clear();

    tireSizes.push_back((UINT8)response[0]);
    tireSizes.push_back((UINT8)response[1]);
    tireSizes.push_back((UINT8)response[2]);
    tireSizes.push_back((UINT8)response[3]);

    status = m_protocolFilter->GetModuleData( "WheelAlignSDAxles",response,&tireSizes);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit WheelAlignSDAxles: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ProgramConfigBytes45( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    configBytes;

    Log(LOG_FN_ENTRY, "Enter ProgramConfigBytes45");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    configBytes.clear();
    configBytes.push_back((UINT8)response[0]);
    configBytes.push_back((UINT8)response[1] & 0xF7);  //clear byte 5 bit 3

    status = m_protocolFilter->GetModuleData( "ProgramConfigBytes45",response,&configBytes);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ProgramConfigBytes45: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ProgramLampGndChkEC80( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    configBytes;

    Log(LOG_FN_ENTRY, "Enter ProgramLampGndChkEC80");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    configBytes.clear();
    //copy all data bytes into config bytes
    for (int ii=0; ii<60; ii++)
    {
        configBytes[ii] = response[ii];
    }

    status = m_protocolFilter->GetModuleData( "ProgramLampGndChkEC80",response,&configBytes);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ProgramLampGndChkEC80: status = %d ",status);
    return( status);
}

//MAM 1/8/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ProgramHrwCcvs( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    configBytes;

    Log(LOG_FN_ENTRY, "Enter ProgramHrwCcvs");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    configBytes.clear();
    configBytes.push_back((UINT8)response[0]);
    configBytes.push_back((UINT8)response[1]);
    configBytes.push_back((UINT8)response[2]);

    status = m_protocolFilter->GetModuleData( "ProgramHrwCcvs",response,&configBytes);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ProgramHrwCcvs: status = %d ",status);
    return( status);
}

//MAM 2/10/15
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ProgramHrwCcvsEC80( SerialString_t &response) throw(ModuleException)
{
    BEP_STATUS_TYPE status; 
    SerialArgs_t    configBytes;

    Log(LOG_FN_ENTRY, "Enter ProgramHrwCcvsEC80");
    // Check to see that all our objects are in place
    CheckObjectsStatus();

    configBytes.clear();    
    //copy all data bytes into config bytes
    for (int ii=0; ii<60; ii++)
    {
        configBytes[ii] = response[ii];
    }

    status = m_protocolFilter->GetModuleData( "ProgramHrwCcvsEC80",response,&configBytes);

    // Log the function entry
    Log(LOG_FN_ENTRY, "Exit ProgramHrwCcvsEC80: status = %d ",status);
    return( status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    uint8_t     dtcCount=0, dtcIdx=0;
    uint16_t    dtcVal;
    char    dtcStr[ 16];

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    status = m_protocolFilter->GetModuleData("ReadFaults", response);

    if (status == BEP_STATUS_SUCCESS)
    {
        // The first byte after the ID is a DTC count
        //if( response.size() >= 2)
        //first 7 bytes are header bytes
        if ( response.size() >= 8)
        {
            // Erase the first byte
            //response.erase( 0, 1);
            // Read the number of DTC's to follow
            //dtcCount = response[ 0];

            //no count byte so we erase header bytes and divide remaining # bytes by 4
            //because each DTC has 3 bytes abd a status byte
            response.erase( 0, 7);
            dtcCount =  response.size()/4;
        }
        Log( LOG_DEV_DATA, "FordABSModuleTemplate - Module is reporting %d DTC's\n", dtcCount);

        // Extract the DTCs from the respnse
        for ( dtcIdx=0; (dtcIdx<dtcCount) && (response.size()>1); dtcIdx++)
        {
            // Erase the first byte  - high byte = $00 (always?)
            response.erase( 0, 1);

            // DTC are sent as [ HI,LO]
            dtcVal = response[ 0];
            dtcVal = ((dtcVal << 8) & 0xFF00) | response[1];
            Log( LOG_DEV_DATA, "\tDTC %d: $%04X\n", dtcIdx, dtcVal);

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
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ReadSensorSpeeds(WheelSpeeds_t &moduleSpeeds) throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	SerialString_t moduleMessage;
    Log(LOG_DEV_DATA,"Enter FordABSModuleTemplate::ReadSensorSpeeds()\n");
	// Read the message from the buffer
    SerialString_t msg;
    std::string temp = "ReadSensorSpeeds";
    status = m_protocolFilter->GetBusBroadcastMessage(temp,(long)500, msg);

	if(status == BEP_STATUS_SUCCESS)
	{   // Valid message, extract the data
        Log(LOG_DEV_DATA,"Message is valid, parsing speed data\n");
        status = ParseSensorSpeeds( msg,moduleSpeeds);
        int i;
        for(i = 0; i < 4; i++)
            Log(LOG_DEV_DATA, "Wheel Idx:%d, ModuleSensorSpeed:%g\n",i, moduleSpeeds[i]);
	}
	else
	{   // Could not find a matching message, give up on it
		char buff[256];
		string logMessage;
		for(UINT8 index = 0; index < msg.length(); index++)
		{
			logMessage += CreateMessage(buff, sizeof(buff), " $%02X", msg[index]);
		}
		Log(LOG_ERRORS, "No matching message entry for %s", logMessage.c_str());
        status = BEP_STATUS_FAILURE;
	}
	return status;
}

//-----------------------------------------------------------------------------
template <class ProtocolFilterType>
BEP_STATUS_TYPE FordABSModuleTemplate<ProtocolFilterType>::ParseSensorSpeeds(const SerialString_t &response, WheelSpeeds_t &speeds)  throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // Log the function entry
    Log(LOG_FN_ENTRY, "%s: Entering ParseSensorSpeeds", ModuleName().c_str());

    try
    {
        // Get the reply interpretation from the Xml
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for Front Axle Average Speed", ModuleName().c_str());
        const XmlNode *replyInterpNodeFrontAvg = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretation");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for LF", ModuleName().c_str());
        const XmlNode *replyInterpNodeLF = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationLF");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for RF", ModuleName().c_str());
        const XmlNode *replyInterpNodeRF = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationRF");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for LR", ModuleName().c_str());
        const XmlNode *replyInterpNodeLR = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationLR");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Getting reply interpretation for RR", ModuleName().c_str());
        const XmlNode *replyInterpNodeRR = m_vehicleMessagesNode->getChild("ReadSensorSpeeds/ReplyInterpretationRR");
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing LF Response", ModuleName().c_str());

        // Try to parse the response into something readable
        float frontAvgSpeed = ParseFloatResponse(replyInterpNodeFrontAvg->getValue(), response);
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing Front Average Response", ModuleName().c_str());
        speeds[LFWHEEL] = ParseFloatResponse(replyInterpNodeLF->getValue(), response) + frontAvgSpeed;
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing RF Response", ModuleName().c_str());
        speeds[RFWHEEL] = ParseFloatResponse(replyInterpNodeRF->getValue(), response) + frontAvgSpeed;
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing LR Response", ModuleName().c_str());
        speeds[LRWHEEL] = ParseFloatResponse(replyInterpNodeLR->getValue(), response) + frontAvgSpeed;
        Log(LOG_DEV_DATA, "%s: ParseSensorSpeeds: Parsing RR Response", ModuleName().c_str());
        speeds[RRWHEEL] = ParseFloatResponse(replyInterpNodeRR->getValue(), response) + frontAvgSpeed;
        // Log the wheel speed sensors read from the module
        Log(LOG_DEV_DATA, "Parsed Sensor Speeds: \n\tAvg = %g\n\tLF = %g\n\tRF = %g\n\tLR = %g\n\tRR = %g\n",
            frontAvgSpeed, speeds[LFWHEEL], speeds[RFWHEEL], speeds[LRWHEEL], speeds[RRWHEEL]);
    }
    catch (XmlException &ex)
    {   // Log it
        Log(LOG_ERRORS, "%s: XmlError: %s", ModuleName().c_str(), ex.GetReason());
        // set status to a software error
        status = BEP_STATUS_SOFTWARE;
    }
    
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s: Exiting ParseSensorSpeeds", ModuleName().c_str());
    // Return the status of the operation
    return status;
}

