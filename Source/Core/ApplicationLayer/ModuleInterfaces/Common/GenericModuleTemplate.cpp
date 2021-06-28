//******************************************************************************
// Description:
//  Genric Module Interface.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Common/GenericModuleTemplate.cpp $
// 
// 1     11/01/07 1:24p Derickso
// Updated to use ReadModuleData and CommandModule instead of
// ProtocolFilter::GetModuleData directly.
// 
// 18    10/16/07 8:35a Derickso
// Refactored functions to make use of ReadModuleData and be more
// efficient.
// 
// 17    10/09/07 1:56p Derickso
// Updated CommandMOdule to accept an optional IProtocolFilter to use
// instead of using m_protocolFilter.
// 
// 16    9/21/07 8:15a Mbhattac
// GenericModuleTemplate.cpp now supports vector<UINT16> as a parameter
// within ReadModuleData() and ParseVectorResponse().
//
// 15     9/18/07 11:18a Mbhattac
// Modified ReadModuleData to take in an IProtocolFilter Object. 
// Added ReadModuleData with vector<float> being supported as a parameter.
// ParseVectorResponse() now supports vector<float> as it was being called
// within the ReadModuleData() function which now needs vector<float>
// support.
// 
// 
// 14    2/22/07 8:38a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Now delete m_vehicleMessagesNode and m_replyInterpretationNode in the
// destructor.
// 
// 13    1/17/07 2:46p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initialize m_lastTxTime to 0 in the constructor.
// 
// - If an old m_replyInterpretationNode exists in Initialize() delete it
// before reading in the new one.
// 
// - If and old m_vehicleMessagesNode exist in Initialize() delete it
// before reading in the new one.
// 
// - Delete the keep alive and switch monitor threads in CloseCom().
// 
// - Added the ability to send different keep alive messages.  This is
// needed
// for ABS types that require different keep alive messages before and
// after
// valve firing.
// 
// - Call OpenCom() in StartSwitchMonitor().
// 
// - Added IsInitialized() and Uninitialize().
// 
// 12    10/25/06 2:57p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added a last transmit time to the ProtocolFilters
//     - Added OpenCom() and CloseCom()
// 
// 11    7/05/06 9:49p Cward
// Added support for reading data from a module for reporting. Also ran
// beautify.
//
// 10    4/13/06 3:13a Cward
// Fix clear faults so that a reply interpretation is not needed
//
// 9     3/30/06 1:46a Cward
// Updated to add the capability to enter diagnostic mode without a
// response
//
// 8     8/17/05 8:06p Cward
// Added another defaulted argument to all the Read Module Data methods
// and the Command Module method to optionally return the raw serial
// string.
//
// 7     7/20/05 8:35p Cward
// Log messages added for debugging
//
// 6     5/11/05 9:49p Cward
// Added Gpattiso changes from 5/09/05
//
// 5     5/09/05 5:00p Gpattison
// Added all necessary ReadModuleData() methods and ParseVector()
// methods to support new addition of Vectors in the
// Reply Interpreter class
//
// 5     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 4     4/29/05 5:29p Gswope
// Fixed memory leak with m_moduleInterpreter.  It was being created in the
// Constructor and the initialize method without deleting the memory first or
// checking if the value was NULL
//
// 4     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     2/16/05 9:09p Cward
// In Initialize calling InitialzeHook with persistent config node
// pointer. In clear faults better handling of good and status if
// exception occurs. commented out various default arguments. Added
// overloaded Read Module data. Added SerialArgs_t parameter to Command
// Module.
//
// 3     1/05/05 3:46p Bmeinke
// Pass m_configNode into InitializeHook() instead of pasing simply
// 'configNode' because configNode is a temporary variable
//
// 2     8/18/04 10:57p Cward
// Added multiple inclusion protection
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericModuleTemplate_cpp
#define GenericModuleTemplate_cpp

#include "GenericModuleTemplate.h"
#include "GenericCANModule.h"

template <class ProtocolFilterType>
GenericModuleTemplate<ProtocolFilterType>::GenericModuleTemplate(void) : IGenericModule(), ILogger(),
    m_initializeHookCalled(false), m_initializeCalled(false),
    m_configNode(NULL), m_replyInterpretationNode(NULL), // Don't delete these pointers on destruction
    m_vehicleMessagesNode(NULL), m_moduleKeepAlive(NULL), m_switchMonitor(NULL)
{   // Create the module interpreter
    m_moduleInterpreter = new ReplyInterpreter();
    // Define the getinfo strings
    m_readPartNumber       = "ReadPartNumber";
    m_unLockModule         = "UnLockModule";
    m_programVIN           = "ProgramVIN";
    m_readVIN              = "ReadVIN";
    m_readTraceData        = "ReadTraceData";
    m_readFaults           = "ReadFaults";
    m_startSwitchMonitor   = "StartSwitchMonitor";
    m_stopSwitchMonitor    = "StopSwitchMonitor";
    m_startModuleKeepAlive = "StartModuleKeepAlive";
    m_stopModuleKeepAlive  = "StopModuleKeepAlive";
    m_lockModule           = "LockModule";
    m_clearFaults          = "ClearFaults";
    m_enterDiagnosticMode  = "EnterDiagnosticMode";
    m_enterNormalMode      = "EnterNormalMode";
    m_exitDiagnosticMode   = "ExitDiagnosticMode";
    m_disableNormalMode    = "DisableNormalMode";
    m_enterDiagnosticMode  = "EnterDiagnosticMode";
    m_driveCurveRewriteDir = "";

    m_lastTxTime.SetValue(0LL);
    m_stopCommsFlag.SetValue(false);

    // Create new protocol filters
    m_switchMonitorComm = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
    m_keepAliveComm = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
    m_protocolFilter = new ProtocolFilterType(m_lastTxTime, &m_stopCommsFlag, &m_commsInUse);
}

template <class ProtocolFilterType>
GenericModuleTemplate<ProtocolFilterType>::~GenericModuleTemplate(void)
{
    if(m_moduleInterpreter != NULL)
    {
        delete m_moduleInterpreter;
        m_moduleInterpreter = NULL;
    }
    if(m_vehicleMessagesNode != NULL)
    {
        delete m_vehicleMessagesNode;
        m_vehicleMessagesNode = NULL;
    }
    if(m_replyInterpretationNode != NULL)
    {
        delete m_replyInterpretationNode;
        m_replyInterpretationNode = NULL;
    }
    // Get rid of the keep alive thread
    if(m_moduleKeepAlive != NULL)
    {   // Stop sending the keep alive message
        m_moduleKeepAlive->StopMonitor();
        m_moduleKeepAlive->WaitUntilDone();
        if(m_moduleKeepAlive != NULL)
            delete m_moduleKeepAlive;
        m_moduleKeepAlive = NULL;
    }
    // Get rid of the switch monitor thread
    if(m_switchMonitor != NULL)
    {   // Stop reading switches
        m_switchMonitor->StopMonitor();
        m_switchMonitor->WaitUntilDone();
        // Get rid of the thread
        if(m_switchMonitor != NULL)
            delete m_switchMonitor;
        m_switchMonitor = NULL;
    }
    // Get rid of the config node
    if(m_configNode != NULL)  delete m_configNode;
    m_configNode = NULL;
    // Get rid of all protocol filters
    if(m_switchMonitorComm != NULL) delete m_switchMonitorComm;
    if(m_keepAliveComm != NULL)     delete m_keepAliveComm;
    if(m_protocolFilter != NULL)    delete m_protocolFilter;
}

template <class ProtocolFilterType>
bool GenericModuleTemplate<ProtocolFilterType>::Initialize (const XmlNode *configNode)
{
    ILogger::Initialize(configNode);
    // Log the function entry
    Log(LOG_FN_ENTRY, "GenericModuleTemplate - %s: Entering Initialize\n", ModuleName().c_str());

    // Our return status
    bool ourReturn = true;
    // Set the initializeCalled flag to true
    m_initializeCalled = true;
    if( NULL != m_configNode)   delete m_configNode;
    m_configNode = configNode->Copy();
    // GETTING XML NODES
    try
    {
        if( NULL != m_replyInterpretationNode)
        {
            delete m_replyInterpretationNode;
        }
        // Grab the ReplyInterpretations node
        m_replyInterpretationNode = configNode->getChild("SerialMessages/ReplyInterpretations")->Copy();
    }
    catch(XmlException &ex)
    {
        Log(LOG_ERRORS, "%s: XmlError: %s", ModuleName().c_str(), ex.GetReason());
        // set the pointer to null, which we can check for later
        m_replyInterpretationNode = NULL;
        // Set our return value
        ourReturn = false;
    }
    try
    {
        if( NULL != m_vehicleMessagesNode)
        {
            delete m_vehicleMessagesNode;
        }
        // Grab the VehicleMessages node
        m_vehicleMessagesNode = configNode->getChild("SerialMessages/VehicleMessages")->Copy();
    }
    catch(XmlException &ex)
    { // Log this
        Log(LOG_ERRORS, "%s: XmlError: %s", ModuleName().c_str(), ex.GetReason());
        // set the pointer to null, which we can check for later
        m_vehicleMessagesNode = NULL;
        // Set our return value
        ourReturn = false;
    }
    // Call our children's initialize hook
    Log(LOG_FN_ENTRY, "GenericModuleTemplate - %s: Entering InitializeHook\n", ModuleName().c_str());
    bool theirReturn;
    theirReturn = InitializeHook(m_configNode);
    Log(LOG_FN_ENTRY, "GenericModuleTemplate - %s: Finished InitializeHook\n", ModuleName().c_str());
    // Return the summary return status
    return(ourReturn && theirReturn);
}

template <class ProtocolFilterType>
bool GenericModuleTemplate<ProtocolFilterType>::OpenCom()
{
    IProtocolFilter *protocolFilter = (IProtocolFilter*)m_protocolFilter;
    bool portOpened = false;
    if( NULL != protocolFilter)
    {
        portOpened = protocolFilter->OpenCom();
		StopVehicleComms(false);
    }
    return( portOpened);
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::CloseCom()
{
    // Get rid of the keep alive thread
    if(m_moduleKeepAlive != NULL)
    {   // Stop sending the keep alive message
        m_moduleKeepAlive->StopMonitor();
        m_moduleKeepAlive->WaitUntilDone();
        if(m_moduleKeepAlive != NULL)
            delete m_moduleKeepAlive;
        m_moduleKeepAlive = NULL;
    }
    // Get rid of the switch monitor thread
    if(m_switchMonitor != NULL)
    {   // Stop reading switches
        m_switchMonitor->StopMonitor();
        m_switchMonitor->WaitUntilDone();
        // Get rid of the thread
        if(m_switchMonitor != NULL)
            delete m_switchMonitor;
        m_switchMonitor = NULL;
    }
    if( NULL != m_protocolFilter)       m_protocolFilter->CloseCom();
    if( NULL != m_keepAliveComm)        m_keepAliveComm->CloseCom();
    if( NULL != m_switchMonitorComm)    m_switchMonitorComm->CloseCom();
}

template <class ProtocolFilterType>
inline string GenericModuleTemplate<ProtocolFilterType>::ModuleName(void)
{
    return("GenericModuleTemplate");
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::EnterDiagnosticMode(void)
{
    Log(LOG_FN_ENTRY, "GenericModuleTemplate::EnterDiagnosticMode\n");
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    Log(LOG_DEV_DATA, "Objects are OK\n");
    // Return the status of Commanding the module to enter diagnostic mode.
    return(CommandModule("EnterDiagnosticMode"));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ExitDiagnosticMode(void)
{   // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Return the status of commanding the module to exit diagnostic mode
    return(CommandModule("ExitDiagnosticMode"));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ClearFaults(void)
{
    bool good = false;
    // Command the module to clear faults and get the result
    BEP_STATUS_TYPE status = ReadModuleData("ClearFaults", good);
    if(BEP_STATUS_SOFTWARE == status)
    {   // if we get the software fail status, it is because there is no reply interpretation node. For this function 
        // (Clear Faults) this is normally acceptable, so set the status to pass.
        status = BEP_STATUS_SUCCESS;
    }
    else if(BEP_STATUS_SUCCESS == status)
    {   // Check if it worked
        status = (good ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE);
    }
    else
    {
        Log(LOG_ERRORS, "Error Clearing faults from %s", ModuleName().c_str());
    }
    // Return the status and log the exit
    Log(LOG_FN_ENTRY, "GenericModuleTemplate::ClearFaults() - Exit: %s", ConvertStatusToResponse(status).c_str());
    return(status);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadPartNumber(string &partNumber)
{
    return(ReadModuleData(m_readPartNumber, partNumber));
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadVIN(string &vin)
{
    return(ReadModuleData(m_readVIN, vin));
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadTraceData(string &traceabilityData)
{
    return(ReadModuleData(m_readTraceData, traceabilityData));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadSwitches(IProtocolFilter *filter)
{   // this is a no-op for GenericModuleTemplate
    Log(LOG_ERRORS, "No generic method for reading switches - all modules are different\n");
    return(BEP_STATUS_FAILURE);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::StartModuleKeepAlive(void)
throw(ModuleException)
{
    return(StartSpecialKeepAlive("ModuleKeepAlive"));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::StartSpecialKeepAlive(std::string messageId)
throw(ModuleException)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // ensure backward compatibility
    if(messageId == "") messageId = "ModuleKeepAlive";

    // Check to see that all our objects are in place
    CheckObjectsStatus();

    // Determine if a keep alive thread should be created
    if(m_moduleKeepAlive == NULL)
    {
        Log(LOG_FN_ENTRY, "Creating SendModuleKeepAlive\n");

        m_keepAliveComm->OpenCom();

        // Create a new keep alive thread
        string keepAliveTime = m_configNode->getChild("Setup/ModuleKeepAliveTime")->getValue();
        bool keepAliveResponseRequired = m_configNode->getChild("Setup/KeepAliveResponseRequired")->getValue() == "Yes";

        Log(LOG_DEV_DATA, "Setting keep alive time to: %d seconds", BposReadInt(keepAliveTime.c_str()));
        m_moduleKeepAlive = new SendModuleKeepAlive(m_keepAliveComm,
                                                    messageId,
                                                    BposReadInt(keepAliveTime.c_str()),
                                                    keepAliveResponseRequired);

        // Start the thread running
        m_moduleKeepAlive->Resume(false);
        status = BEP_STATUS_SUCCESS;
    }
    // Completed, return test result
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::StopModuleKeepAlive(void)
{   // Stop the keep alive thread if needed
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    // Determine if a keep alive thread is running
    if(m_moduleKeepAlive != NULL)
    {   // Stop sending the keep alive message
        m_moduleKeepAlive->StopMonitor();
        m_moduleKeepAlive->WaitUntilDone();
        if(m_moduleKeepAlive != NULL)
            delete m_moduleKeepAlive;
    }
    // Set the poitner to NULL so we can restart later if needed
    Log(LOG_DEV_DATA, "Keep alive thread complete, setting pointer to NULL\n");
    m_moduleKeepAlive = NULL;
    // Completed, set test result
    Log(LOG_DEV_DATA, "StopModuleKeepAlive() complete!\n");
    status = BEP_STATUS_SUCCESS;
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::StartSwitchMonitor(void)
throw(ModuleException)
{   // Create a new keep alive thread if needed
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Determine if a switch monitor exists
    if(m_switchMonitor == NULL)
    {   // Create a new switch monitor thread
        string switchMonitorDelay = m_configNode->getChild("Setup/SwitchMonitorDelay")->getValue();

        m_switchMonitorComm->OpenCom();
        m_switchMonitor = new SwitchMonitor(this, m_switchMonitorComm, BposReadInt(switchMonitorDelay.c_str()));
        // Start the thread running
        m_switchMonitor->Resume(false);
        status = BEP_STATUS_SUCCESS;
    }
    // Completed, return test result
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::StopSwitchMonitor(void)
{   // Stop the keep alive thread if needed
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    // Determine if a switch monitor thread was created
    if(m_switchMonitor != NULL)
    {   // Stop reading switches
        m_switchMonitor->StopMonitor();
        m_switchMonitor->WaitUntilDone();
        if(m_switchMonitor != NULL)
            delete m_switchMonitor;
    }
    // Set the pointer to NULL so it can be restarted later
    Log(LOG_DEV_DATA, "Switch monitor thread complete, setting pointer to NULL\n");
    m_switchMonitor = NULL;
    // Completed, set test result
    Log(LOG_DEV_DATA, "StopSwitchMonitor() complete!\n");
    status = BEP_STATUS_SUCCESS;
    return(status);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ProgramVIN(string vin)
{   // Let the developer know we can't do this.
    Log(LOG_ERRORS, "No generic method for programming the VIN - all modules are different\n");
    // We cant' do this here, do it in child classes
    return(BEP_STATUS_SOFTWARE);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::UnlockModuleSecurity()
{
    Log(LOG_ERRORS, "WARNING: Need to implement this function before using!");
    return(BEP_STATUS_SOFTWARE);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadConsecutiveMemoryAddresses(const string &serialMessage,
                                                                                          const UINT16 &address,
                                                                                          const UINT16 &numberOfAddressBytes,
                                                                                          vector<SerialString_t> &moduleResponses,
                                                                                          const UINT16 &numberOfReads, /* = 1*/
                                                                                          const UINT16 &addressIncrement) /* = 1*/
{
    SerialString_t response;                    // Used to store the individual module read responses
    SerialArgs_t arguments;                     // Used to store the address arguments needed for call to GetModuleData()
    UINT16 count           = 0;                 // used for do-while loop control
    UINT16 localAddress    = address;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;  // used to return status of method
    // Log Method entry
    Log(LOG_FN_ENTRY, "Enter GenericModuleTemplate::ReadConsecutiveMemoryAddresses()\n");
    // Clear the module response vector
    moduleResponses.clear();
    // Read the addresses from the module
    do
    {   // Place address bytes it correct order in the arguments vector
        // NOTE: Breaks have been intentionally left out so the correct address bytes
        //       will be masked out.  This eliminates the need for a loop to accomplish
        //       the same result.
        arguments.clear();
        switch(numberOfAddressBytes)
        {
        case 2:
            arguments.push_back((UINT8)((localAddress & 0xFF00) >> 8));
        case 1:
            arguments.push_back((UINT8)(localAddress & 0x00FF));
            // Read module data
            status = m_protocolFilter->GetModuleData(serialMessage, response, &arguments);
            if(status == BEP_STATUS_SUCCESS)
            {   // place response into moduleReponses vector
                moduleResponses.push_back(response);
            }
            else
            {   // Module read failure
                Log(LOG_ERRORS,"Error in ReadConsecutiveMemoryAddresses - Failed while reading from module, status = %s\n",
                    ConvertStatusToResponse(status).c_str());
            }
            // increment least significant byte of address by addressIncrement
            localAddress += addressIncrement;
            break;

        default:
            // Incorrect value for numberOfAddressBytes
            Log(LOG_ERRORS,"Error in ReadConsecutiveMemoryAddresses - numberOfAddressBytes Out of Range, value = %d, range = 1-4\n",
                numberOfAddressBytes);
            status = BEP_STATUS_FAILURE;
        }
        // Keep looking while good module comms and more addresses to read
    } while((status == BEP_STATUS_SUCCESS) && (++count < numberOfReads));
    // return status of method
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleRegister(const UINT16 &moduleRegister,
                                                                              UINT16 &registerContents)
{
    SerialArgs_t registerArg;
    registerArg.push_back((moduleRegister & 0xFF00) >> 8);
    registerArg.push_back(moduleRegister & 0x00FF);
    return(ReadModuleData("ReadModuleRegister", registerContents, &registerArg));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, string & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // Read the part number from the module
    if(methodName == m_readPartNumber)       retVal = ReadPartNumber(value);
    // Unlock the module
    else if(methodName == m_unLockModule)    retVal = UnLockModule(value);
    // Program the vin into the module
    else if(methodName == m_programVIN)      retVal = ProgramVIN(value);
    // Read the VIN from the module
    else if(methodName == m_readVIN)         retVal = ReadVIN(value);
    // Read tracebility data from the module
    else if(methodName == m_readTraceData)   retVal = ReadTraceData(value);
    // No method to get requested information
    else
    {
        Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
        throw ModuleException(ModuleName() + "::GetInfo(string): No method named: " + methodName);
    }
    // Return the status
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<string> & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal;
    string got;

    if(value.size() != (unsigned int)0)
    {
        value.empty();
    }

    retVal = GetInfo(methodName, got);

    if(retVal == BEP_STATUS_SUCCESS)
    {
        value.push_back(got);
    }

    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, bool &value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // No generic bool data to get from the module
    Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
    // Throw an exception
    throw ModuleException(ModuleName() + "::GetInfo(bool): No method named: " + methodName);
    // Return the status
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<bool> & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal;
    bool got;

    if(value.size() != (unsigned int)0)
    {
        value.empty();
    }

    retVal = GetInfo(methodName, got);

    if(retVal == BEP_STATUS_SUCCESS)
    {
        value.push_back(got);
    }

    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, int & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // No generic integer data to get from a module
    Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
    // Throw an exception because there is nothing to do
    throw ModuleException(ModuleName() + "::GetInfo(int): No method named: " + methodName);
    // Return the status
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<INT32> & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal;
    int got;

    if(value.size() != (unsigned int)0)
    {
        value.empty();
    }

    retVal = GetInfo(methodName, got);

    if(retVal == BEP_STATUS_SUCCESS)
    {
        value.push_back(got);
    }

    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, FaultVector_t & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // Read faults from the module
    if(methodName == m_readFaults)
    {   // Read the module faults
        retVal = ReadFaults(value);
    }
	else if(methodName == "UDSReadFaults")
	{
		GenericCANModule faultReader(m_protocolFilter);
		faultReader.Initialize(m_configNode);
		retVal = faultReader.ReadFaults(value);
	}
    else
    {   // no generic method available
        Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
        // Thorw an exception since no generic method is available
        throw ModuleException(ModuleName() + "::GetInfo(FaultVector_t): No method named: " + methodName);
    }
    // Return the status
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, float &value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // No generic float data available from the module
    Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
    // Throw an exception since no generic method available
    throw ModuleException(ModuleName() + "::GetInfo(float): No method named: " + methodName);
    // Return the status
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<float> & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal;
    float got;

    if(value.size() != (unsigned int)0)
    {
        value.empty();
    }

    retVal = GetInfo(methodName, got);

    if(retVal == BEP_STATUS_SUCCESS)
    {
        value.push_back(got);
    }

    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, WheelSpeeds_t & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // No generic method available for reading wheel speeds from the module
    Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
    // Thorw an exception since no generic method available
    throw ModuleException(ModuleName() + "::GetInfo(WheelSpeeds_t): No method named: " + methodName);
    // Return the status
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<WheelSpeeds_t> & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal;
    WheelSpeeds_t got;

    if(value.size() != (unsigned int)0)
    {
        value.empty();
    }

    retVal = GetInfo(methodName, got);

    if(retVal == BEP_STATUS_SUCCESS)
    {
        value.push_back(got);
    }

    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName)
throw(ModuleException)
{   // The return value
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // Start the switch monitor
    if(methodName == m_startSwitchMonitor)        retVal = StartSwitchMonitor();
    // Stop the switch monitor
    else if(methodName == m_stopSwitchMonitor)    retVal = StopSwitchMonitor();
    // Start the module keep alive thread
    else if(methodName == m_startModuleKeepAlive) retVal = StartModuleKeepAlive();
    // Stop the module keep alive thread
    else if(methodName == m_stopModuleKeepAlive)  retVal = StopModuleKeepAlive();
    // Lock the mdule
    else if(methodName == m_lockModule)           retVal = LockModule();
    // Clear module faults
    else if(methodName == m_clearFaults)          retVal = ClearFaults();
    // Command module to enter diagnostic mode
    else if(methodName == m_enterDiagnosticMode)  retVal = EnterDiagnosticMode();
    // Command the module to exit diagnostic mode
    else if(methodName == m_exitDiagnosticMode)   retVal = ExitDiagnosticMode();
    // No generic method available
    else
    {   // No generic method available
        Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
        // Throw an exception since no method is available
        throw ModuleException(ModuleName() + "::GetInfo(): No method named: " + methodName);
    }
    // Return the return value
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT8 & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // No generic method available
    Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
    // Throw an exception since no method is available
    throw ModuleException(ModuleName() + "::GetInfo(UINT8): No method named: " + methodName);
    // Return the status
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<UINT8> & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal;
    UINT8 got;

    if(value.size() != (unsigned int)0)
    {
        value.empty();
    }

    retVal = GetInfo(methodName, got);

    if(retVal == BEP_STATUS_SUCCESS)
    {
        value.push_back(got);
    }

    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, UINT16 & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal = BEP_STATUS_SOFTWARE;
    // No generic method available
    Log(LOG_DEV_DATA, "%s: GetInfo: No method named: %s", ModuleName().c_str(), methodName.c_str());
    // Throw an exception since no method is available
    throw ModuleException(ModuleName() + "::GetInfo(UINT16): No method named: " + methodName);
    // Return the status
    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::GetInfo(string methodName, vector<UINT16> & value)
throw(ModuleException)
{
    BEP_STATUS_TYPE retVal;
    UINT16 got;

    if(value.size() != (unsigned int)0)
    {
        value.empty();
    }

    retVal = GetInfo(methodName, got);

    if(retVal == BEP_STATUS_SUCCESS)
    {
        value.push_back(got);
    }

    return(retVal);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReceiveBroadcastData(const string messageTag,
																				  string &data,
																				  long timeout,
																				  SerialString_t *rawData, /* = NULL */
																				  IProtocolFilter *commObject)/* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetBusBroadcastMessage(messageTag, timeout, moduleResponse);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            data = ParseStringResponse(GetReplyInterpretationTag(messageTag), moduleResponse);
            Log(LOG_DEV_DATA, "%s: %s\n", messageTag.c_str(), data.c_str());
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReceiveBroadcastData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          string &data,
                                                                          SerialArgs_t *args,/* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject)/* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            data = ParseStringResponse(GetReplyInterpretationTag(messageTag), moduleResponse);
            Log(LOG_DEV_DATA, "%s: %s\n", messageTag.c_str(), data.c_str());
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          bool &data,
                                                                          SerialArgs_t *args,/* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL*/ 
                                                                          IProtocolFilter *commObject) /* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            data = ParseBooleanResponse(GetReplyInterpretationTag(messageTag), moduleResponse);
            Log(LOG_DEV_DATA, "%s: %d\n", messageTag.c_str(), data);
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          float &data,
                                                                          SerialArgs_t *args,/* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject) /*= NULL*/
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            data = ParseFloatResponse(GetReplyInterpretationTag(messageTag), moduleResponse);
            Log(LOG_DEV_DATA, "%s: %.2f\n", messageTag.c_str(), data);
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          INT32 &data,
                                                                          SerialArgs_t *args,/* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject) /* = NULL*/
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            data = ParseSignedIntegerResponse(GetReplyInterpretationTag(messageTag), moduleResponse);
            Log(LOG_DEV_DATA, "%s: %d\n", messageTag.c_str(), data);
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          UINT8 &data,
                                                                          SerialArgs_t *args,/* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL*/
                                                                          IProtocolFilter *commObject) /*= NULL*/
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            data = ParseUnsignedByteResponse(GetReplyInterpretationTag(messageTag), moduleResponse);
            Log(LOG_DEV_DATA, "%s: 0x%02X\n", messageTag.c_str(), data);
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          UINT16 &data,
                                                                          SerialArgs_t *args,/* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL*/
                                                                          IProtocolFilter *commObject) /* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            data = ParseUnsignedShortResponse(GetReplyInterpretationTag(messageTag), moduleResponse);
            Log(LOG_DEV_DATA, "%s: 0x%04X\n", messageTag.c_str(), data);
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          UINT32 &data,
                                                                          SerialArgs_t *args,/* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL*/
                                                                          IProtocolFilter *commObject) /* = NULL*/ 
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            data = ParseUnsignedIntegerResponse(GetReplyInterpretationTag(messageTag), moduleResponse);
            Log(LOG_DEV_DATA, "%s: 0x%08X\n", messageTag.c_str(), data);
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          vector<bool> &data,
                                                                          SerialArgs_t *args, /* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject) /* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    vector<bool>::iterator iter;
    INT32 count = 0;

    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            ParseVectorResponse(GetReplyInterpretationTag(messageTag), moduleResponse, data);
            for(count = 0,iter = data.begin(); iter != data.end(); count++,iter++)
            {
                Log(LOG_DEV_DATA, "%s: Vector Response %03d - %s\n", messageTag.c_str(), count, *iter ? "True" : "False");
            }
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          vector<string> &data,
                                                                          SerialArgs_t *args, /* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject ) /* = NULL*/
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    vector<string>::iterator iter;
    INT32 count = 0;

    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            ParseVectorResponse(GetReplyInterpretationTag(messageTag), moduleResponse, data);
            for(count = 0,iter = data.begin(); iter != data.end(); count++,iter++)
            {
                Log(LOG_DEV_DATA, "%s: Vector Response %03d - %s\n", messageTag.c_str(), count, (*iter).c_str());
            }
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}
template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          vector<UINT8> &data,
                                                                          SerialArgs_t *args, /* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject) /* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    vector<UINT8>::iterator iter;
    INT32 count = 0;

    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            ParseVectorResponse(GetReplyInterpretationTag(messageTag), moduleResponse, data);
            for(count = 0,iter = data.begin(); iter != data.end(); count++,iter++)
            {
                Log(LOG_DEV_DATA, "%s: Vector Response %03d - 0x%02X\n", messageTag.c_str(), count, *iter);
            }
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          vector<UINT32> &data,
                                                                          SerialArgs_t *args, /* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject ) /* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    vector<UINT32>::iterator iter;
    INT32 count = 0;

    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            ParseVectorResponse(GetReplyInterpretationTag(messageTag), moduleResponse, data);
            for(count = 0,iter = data.begin(); iter != data.end(); count++,iter++)
            {
                Log(LOG_DEV_DATA, "%s: Vector Response %03d - 0x%08X\n", messageTag.c_str(), count, *iter);
            }
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

//------------------------------------------------------------------------------------------------------
//mbhattac added 

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          vector<UINT16> &data,
                                                                          SerialArgs_t *args, /* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject ) /* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    vector<UINT16>::iterator iter;
    INT32 count = 0;

    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            ParseVectorResponse(GetReplyInterpretationTag(messageTag), moduleResponse, data);
            for(count = 0,iter = data.begin(); iter != data.end(); count++,iter++)
            {
                Log(LOG_DEV_DATA, "%s: Vector Response %03d - 0x%08X\n", messageTag.c_str(), count, *iter);
            }
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          vector<INT32> &data,
                                                                          SerialArgs_t *args, /* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject ) /* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    vector<INT32>::iterator iter;
    INT32 count = 0;

    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            ParseVectorResponse(GetReplyInterpretationTag(messageTag), moduleResponse, data);
            for(count = 0,iter = data.begin(); iter != data.end(); count++,iter++)
            {
                Log(LOG_DEV_DATA, "%s: Vector Response %03d - 0x%08X\n", messageTag.c_str(), count, *iter);
            }
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          vector<double> &data,
                                                                          SerialArgs_t *args, /* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject ) /* = NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    vector<double>::iterator iter;
    INT32 count = 0;

    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            ParseVectorResponse(GetReplyInterpretationTag(messageTag), moduleResponse, data);
            for(count = 0,iter = data.begin(); iter != data.end(); count++,iter++)
            {
                Log(LOG_DEV_DATA, "%s: Vector Response %03d - %.4e\n", messageTag.c_str(), count, *iter);
            }
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadModuleData(const string messageTag,
                                                                          vector<float> &data,
                                                                          SerialArgs_t *args, /* = NULL*/
                                                                          SerialString_t *rawData, /* = NULL */
                                                                          IProtocolFilter *commObject) /*= NULL */
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    vector<float>::iterator iter;
    INT32 count = 0;

    // Make sure all neccessary objects have been created
    CheckObjectsStatus();
    // Read the data from the module
    // Checks to see if whether a comm Object is passed
    // Otherwise use m_protocolFilter
    IProtocolFilter *moduleComm = (commObject == NULL) ? m_protocolFilter : commObject;
    status = moduleComm->GetModuleData(messageTag, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
    }
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == status)
    {   // Good read, attempt to extract the data
        try
        {   // Extract the data from the response
            ParseVectorResponse(GetReplyInterpretationTag(messageTag), moduleResponse, data);
            for(count = 0,iter = data.begin(); iter != data.end(); count++,iter++)
            {
                Log(LOG_DEV_DATA, "%s: Vector Response %03d - %.4e\n", messageTag.c_str(), count, *iter);
            }
        }
        catch(XmlException &err)
        {   // Log the reason for the exception
            Log(LOG_ERRORS, "%s: XmlException in ReadModuleData(%s): %s\n", ModuleName().c_str(),
                messageTag.c_str(), err.GetReason());
            // Set the status to software error
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading the data from the module
        Log(LOG_ERRORS, "Error with %s message - status: %s\n", messageTag.c_str(),
            ConvertStatusToResponse(status).c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::CommandModule(const string command, 
                                                                         SerialArgs_t *args /*= NULL*/, 
                                                                         SerialString_t *rawData /*= NULL*/, 
                                                                         IProtocolFilter *commObject /*= NULL*/)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    SerialString_t moduleResponse;
    Log(LOG_DEV_DATA, "GenericModuleTemplate::CommandModule - is commObject NULL? ... %s", (commObject == NULL) ? "yes" : "no");
    IProtocolFilter *comm = (commObject == NULL) ? m_protocolFilter : commObject;
    // Command the module
    status = comm->GetModuleData(command, moduleResponse, args);
    // Check if the module response should be returned to the caller
    if(rawData != NULL)
    {
        *rawData = moduleResponse;
        Log(LOG_DEV_DATA, "module response being returned to the caller");
    }
    // Check the status of the command
    if(BEP_STATUS_SUCCESS == status)
        Log(LOG_DEV_DATA, "Command module: %s - Success\n", command.c_str());
    else
        Log(LOG_ERRORS, "Error commanding module: %s - status: %s\n", command.c_str(), ConvertStatusToResponse(status).c_str());
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::LockModule(void)
{   // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Return the status of locking the module
    return(CommandModule("LockModule"));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::UnLockModule(string key)
{   // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Return the status of unlocking the module
    return(CommandModule("UnLockModule"));
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadFaults(FaultVector_t &faultCodes)
{
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Read the faults from the module
    status = m_protocolFilter->GetModuleData("ReadFaults", response);
    // Check the status of the read
    if(status == BEP_STATUS_SUCCESS)
    { // Get the reply interpretation from the Xml
        const XmlNode *replyInterpNode = m_vehicleMessagesNode->getChild("ReadFaults/ReplyInterpretation");
        // Extract the faults from the module
        try
        {
            faultCodes.push_back(FaultCode_t(ParseStringResponse(replyInterpNode->getValue(), response)));
        }
        catch(XmlException &ex)
        {
            // Return a software failure and log
            Log(LOG_ERRORS, (ModuleName() + ": ReadFaults: " + ex.GetReason()).c_str());
            status = BEP_STATUS_SOFTWARE;
        }
    }
    else
    {   // Error reading faults from the module
        Log(LOG_ERRORS, "Error read faults from %s\n", ModuleName().c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadSoftwareInfo(void)
{
    Log(LOG_ERRORS, "No generic method for reading software info\n");
    // Return the status
    return(BEP_STATUS_SOFTWARE);
}

template <class ProtocolFilterType>
inline BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::SetMecCounter(void)
{
    Log(LOG_ERRORS, "No generic method for setting the MEC counter\n");
    // Return the status
    return(BEP_STATUS_SOFTWARE);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ResumeNormalOps(void)
{   // Check to see that all our objects are in place
    CheckObjectsStatus();
    // Return the status of commanding the module to resume normal operations.
    return(CommandModule("ResumeNormalOps"));
}

template <class ProtocolFilterType>
bool GenericModuleTemplate<ProtocolFilterType>::IsIgnitionOn(void)
{
    bool ignitionOn = false;
    // If a response is sent by the module, the ignition must be on
    if(BEP_STATUS_SUCCESS == CommandModule("ReadIgnitionStatus"))
    {   // Ignition is in the On or Run position
        ignitionOn = true;
        Log(LOG_DEV_DATA, "Ignition is in the On/Run position\n");
    }
    else
    {   // Ignition is not on
        ignitionOn = false;
        Log(LOG_DEV_DATA, "Ignition is NOT in the On/Run position\n");
    }
    // Return the status
    return(ignitionOn);
}

template <class ProtocolFilterType>
bool GenericModuleTemplate<ProtocolFilterType>::IsEngineRunning(const INT32 &minimumEngineRPM)
{
    bool engineRunning = false;
    INT32 engineRPM = 0;
    // Read the engine rpm from the module
    if(ReadModuleData("ReadEngineRPMActual", engineRPM) == BEP_STATUS_SUCCESS)
    {   // Good read, evaluate the data
        Log(LOG_DEV_DATA, "Read engine RPM from the module: %d, minimum: %d\n", engineRPM, minimumEngineRPM);
        // Determine if the RPM is sufficient
        engineRunning = engineRPM > minimumEngineRPM;
    }
    else
    {   // Error reading engine rpm, default to engine off
        Log(LOG_ERRORS, "Error reading the engine rpm\n");
        engineRunning = false;
    }
    // Return the status
    return(engineRunning);
}

template <class ProtocolFilterType>
bool GenericModuleTemplate<ProtocolFilterType>::InitializeHook(const XmlNode *configNode)
{   // Initialize com object
    m_protocolFilter->Initialize(configNode);
    // Flag that the InializeHook() method is complete
    m_initializeHookCalled = true;
    // Return the status
    return(m_initializeHookCalled);
}

template <class ProtocolFilterType>
bool GenericModuleTemplate<ProtocolFilterType>::IsInitialized() const
{   
    // Return the status
    return(m_initializeHookCalled);
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::Uninitialize()
{   
    m_initializeHookCalled = false;

    CloseCom();
}

template <class ProtocolFilterType>
string GenericModuleTemplate<ProtocolFilterType>::ParseStringResponse(string interpTag, SerialString_t rawResponse)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseStringResponse( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeStr();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        returnMe = m_moduleInterpreter->strEvaluate(rawResponse);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
    // Return the return value
    return(returnMe);
}

template <class ProtocolFilterType>
bool GenericModuleTemplate<ProtocolFilterType>::ParseBooleanResponse(string interpTag, SerialString_t rawResponse)   throw(XmlException, ModuleException)
{   // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    bool returnMe = false;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // Check if the config is set up for reading the value
    bool goodSetup = m_moduleInterpreter->isInterpretTypeBool();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        returnMe = m_moduleInterpreter->boolEvaluate(rawResponse);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a boolean value");
    }
    // Return the return value
    return(returnMe);
}

template <class ProtocolFilterType>
UINT32 GenericModuleTemplate<ProtocolFilterType>::ParseUnsignedIntegerResponse(string interpTag, SerialString_t rawResponse)
throw(XmlException, ModuleException)
{   
    Log(LOG_DEV_DATA, "Entering GenericModuleTemplate::ParseUnsignedIntegerResponse");
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL)  throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    UINT32 returnMe = 0;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // Check if the config is set up for reading the value
    bool goodSetup = m_moduleInterpreter->isInterpretTypeUnsignedInt();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        returnMe = m_moduleInterpreter->intUnsignedEvaluate(rawResponse);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect an integer value");
    }
    // Return the return value
    return(returnMe);
}

template <class ProtocolFilterType>
inline UINT8 GenericModuleTemplate<ProtocolFilterType>::ParseUnsignedByteResponse(string interpTag,
                                                                                  SerialString_t rawResponse)
throw(XmlException, ModuleException)
{
    Log(LOG_DEV_DATA,"Entering GenericModuleTemplate::ParseUnsignedByteResponse");
    return((UINT8) ParseUnsignedIntegerResponse(interpTag, rawResponse));
}

template <class ProtocolFilterType>
inline UINT16 GenericModuleTemplate<ProtocolFilterType>::ParseUnsignedShortResponse(string interpTag,
                                                                                    SerialString_t rawResponse)
throw(XmlException, ModuleException)
{
    return((UINT16) ParseUnsignedIntegerResponse(interpTag, rawResponse));
}

template <class ProtocolFilterType>
INT32 GenericModuleTemplate<ProtocolFilterType>::ParseSignedIntegerResponse(string interpTag, SerialString_t rawResponse)
throw(XmlException, ModuleException)
{   // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL)  throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    INT32 returnMe = 0;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode(interpNode);
    // Check if the config is set up for reading the value
    bool goodSetup = m_moduleInterpreter->isInterpretTypeSignedInt();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        returnMe = m_moduleInterpreter->intSignedEvaluate(rawResponse);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect an integer value");
    }
    // Return the return value
    return(returnMe);
}

template <class ProtocolFilterType>
inline INT8 GenericModuleTemplate<ProtocolFilterType>::ParseSignedByteResponse(string interpTag,
                                                                               SerialString_t rawResponse)
throw(XmlException, ModuleException)
{
    return((INT8) ParseSignedIntegerResponse(interpTag, rawResponse));
}

template <class ProtocolFilterType>
inline INT16 GenericModuleTemplate<ProtocolFilterType>::ParseSignedShortResponse(string interpTag,
                                                                                 SerialString_t rawResponse)
throw(XmlException, ModuleException)
{
    return((INT16) ParseSignedIntegerResponse(interpTag, rawResponse));
}

template <class ProtocolFilterType>
double GenericModuleTemplate<ProtocolFilterType>::ParseFloatResponse(string interpTag, SerialString_t rawResponse)
throw(XmlException, ModuleException)
{   // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    double returnMe = 0;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild(interpTag);
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode(interpNode);
    // Check if the config is set up for reading the value
    bool goodSetup = m_moduleInterpreter->isInterpretTypeFlt();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        returnMe = m_moduleInterpreter->fltEvaluate(rawResponse);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a float value");
    }
    // Return the return value
    return(returnMe);
}

template <class ProtocolFilterType>
UINT16 GenericModuleTemplate<ProtocolFilterType>::ParseEnumeratedResponse(string interpTag, SerialString_t rawResponse)
throw(XmlException, ModuleException)
{   // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    UINT16 returnMe = 0;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // Check if the config is set up for reading the value
    bool goodSetup = m_moduleInterpreter->isInterpretTypeEnum();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        returnMe = m_moduleInterpreter->enumEvaluate(rawResponse);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect an enumerated value");
    }
    // Return the return value
    return(returnMe);
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::ParseVectorResponse(string interpTag, SerialString_t rawResponse,
                                                                    vector<bool> &data)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseVectorResponse - for booleans ( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeVector();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        m_moduleInterpreter->vectorEvaluate(rawResponse,data);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::ParseVectorResponse(string interpTag, SerialString_t rawResponse,
                                                                    vector<string> &data)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseVectorResponse - for strings ( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeVector();
    // If we're set to go..
    if(goodSetup)
    {
        m_moduleInterpreter->vectorEvaluate(rawResponse,data);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::ParseVectorResponse(string interpTag, SerialString_t rawResponse,
                                                                    vector<UINT8> &data)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseVectorResponse - for unsigned characters ( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeVector();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        m_moduleInterpreter->vectorEvaluate(rawResponse,data);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::ParseVectorResponse(string interpTag, SerialString_t rawResponse,
                                                                    vector<UINT32> &data)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseVectorResponse - for unsigned integers ( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeVector();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        m_moduleInterpreter->vectorEvaluate(rawResponse,data);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::ParseVectorResponse(string interpTag, SerialString_t rawResponse,
                                                                    vector<UINT16> &data)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseVectorResponse - for unsigned integers ( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeVector();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        m_moduleInterpreter->vectorEvaluate(rawResponse,data);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::ParseVectorResponse(string interpTag, SerialString_t rawResponse,
                                                                    vector<INT32> &data)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseVectorResponse - for integers ( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeVector();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        m_moduleInterpreter->vectorEvaluate(rawResponse,data);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::ParseVectorResponse(string interpTag, SerialString_t rawResponse,
                                                                    vector<double> &data)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseVectorResponse - for floating points ( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeVector();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        m_moduleInterpreter->vectorEvaluate(rawResponse,data);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::ParseVectorResponse(string interpTag, SerialString_t rawResponse,
                                                                    vector<float> &data)
throw(XmlException, ModuleException)
{   // Log the function entry
    Log(LOG_FN_ENTRY, "ENTER: ParseVectorResponse - for floating points ( %s )\n", interpTag.c_str());
    // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL) throw ModuleException("*m_replyInterpretationNode is null");
    // The return value
    string returnMe;
    // Get the XML node for interpretations
    const XmlNode *interpNode = m_replyInterpretationNode->getChild( interpTag );
    // Store the interpretation node
    m_moduleInterpreter->setConfigNode( interpNode );
    // We're expecting a string, this throws an XML Exception
    bool goodSetup = m_moduleInterpreter->isInterpretTypeVector();
    // If we're set to go..
    if(goodSetup)
    {   // Interpret it
        m_moduleInterpreter->vectorEvaluate(rawResponse,data);
    }
    else
    {   // The XML was not set up to handle the desired
        // interpretation
        // So throw a module exception with a nice message
        throw XmlException("Xml config for " + interpTag + " reply interpretation was not" +
                           " set to expect a string value");
    }
}

template <class ProtocolFilterType>
string GenericModuleTemplate<ProtocolFilterType>::GetReplyInterpretationTag(const string &messageTag)
{
    string replyInterpretationTag = BEP_ERROR_RESPONSE;
    // Build the node name to look for
    string replyInterpretationNodeName = messageTag + "/ReplyInterpretation";
    // Get the reply interpretation name to use
    replyInterpretationTag = m_vehicleMessagesNode->getChild(replyInterpretationNodeName)->getValue();
    // Return the reply interpretation tag
    return(replyInterpretationTag);
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::CheckObjectsStatus(void) throw(ModuleException)
{   // Check to see that we have a reply interp node
    if(m_replyInterpretationNode == NULL)
    {   // No reply interpretation availbale
        throw ModuleException(ModuleName() + ": Object Null: m_replyInterpretationNode" );
    }
    // Check to see that we have a VehicleMessages node
    if(m_vehicleMessagesNode == NULL)
    {   // No vehicle messages provided
        throw ModuleException(ModuleName() + ": Object Null: m_vehicleMessagesNode" );
    }
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::EnterDiagnosticModeNoResponse(void)
{
    BEP_STATUS_TYPE status;
    Log(LOG_FN_ENTRY, "GenericModuleTemplate::EnterDiagnosticModeNoResponse\n");
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    Log(LOG_DEV_DATA, "Objects are OK\n");
    // Command the module to enter diagnostic mode
    Log(LOG_DEV_DATA, "GenericModuleTemplate - Calling SendMessage\n");
    status = m_protocolFilter->SendMessage("EnterDiagnosticMode");
    // Return the status of the operation
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::SendKeepAliveNoResponse(void)
{
    BEP_STATUS_TYPE status;
    Log(LOG_FN_ENTRY, "GenericModuleTemplate::SendKeepAliveNoResponse\n");
    // Check to see that all our objects are in place
    CheckObjectsStatus();
    Log(LOG_DEV_DATA, "Objects are OK\n");
    // Command the module to enter diagnostic mode
    Log(LOG_DEV_DATA, "GenericModuleTemplate - Calling SendMessage\n");
    status = m_protocolFilter->SendMessage("ModuleKeepAlive");
    // Return the status of the operation
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::ReadInfoToReport(XmlNodeMap *moduleDataItems, const string &messageTag)
{
    // Response
    SerialString_t response(255, 0);
    BEP_STATUS_TYPE status;
    // Read data from the module
    status = m_protocolFilter->GetModuleData(messageTag, response);
    // Check the status of the operation
    if(status == BEP_STATUS_SUCCESS)
    {   // Attempt to parse the data from response
        for(XmlNodeMapItr iter = moduleDataItems->begin();
           (iter != moduleDataItems->end());
           iter++)
        {   // Parse out data
            Log(LOG_DEV_DATA, "Iterate through data items\n");
            try
            {
                iter->second->setValue(ParseStringResponse(iter->second->getName(), response));
            }
            catch(XmlException &excpt)
            {
                iter->second->setValue("Error");
            }
            Log(LOG_DEV_DATA, "Set data: %s - Value: %s\n",
                iter->second->getName().c_str(), iter->second->getValue().c_str());
        }
    }
    else
    {
        Log(LOG_ERRORS, "Error Reading Data from %s\n", ModuleName().c_str());
    }
    // Return the status
    return(status);
}

template <class ProtocolFilterType>
BEP_STATUS_TYPE GenericModuleTemplate<ProtocolFilterType>::PerformPowerOnReset(void)
{	
    return CommandModule("PerformPowerOnReset");
}

template <class ProtocolFilterType>
void GenericModuleTemplate<ProtocolFilterType>::StopVehicleComms(const bool &stopComms)
{//Signal Protocol filters to stop/resume communications
    m_protocolFilter->SetStopCommsFlag(stopComms);
}

//--------------------------------------------------------------------------
#endif //GENERICMODULETemplate_cpp
