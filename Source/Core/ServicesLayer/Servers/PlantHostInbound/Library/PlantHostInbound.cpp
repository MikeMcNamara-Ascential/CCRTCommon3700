//******************************************************************************
// Description:
//   This defines the PlantHostInbound object.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/PlantHostInbound/Library/PlantHostInbound.cpp $
// 
// 1     11/01/07 3:40p Derickso
// Updated to initialize the prompt server comm object in the Register
// function.
// Updated printf statements to Log statements.
// Added catch statements for XmlException and BepException.
// 
// 13    2/22/07 11:14a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added an XmlNode parameter to AddDerivedBuildInfo().
// - Added an XmlNode parameter and boolean parameter to
// LoadVehicleBuildRecord().
// 
// - Added GetAbortPrompt(), GetAbortColor(), GetBlockPrompt(),
// GetShipPrompt(), and GetShipStatusColor().
// 
// - Added AreRetainersDown().
// - Added a boolean parameter to ReadBuildRecordFromBroadcast().
// - Added an XmlNode parameter and boolean parameter to
// TranslateBuildRecord().
// 
// - Added an XmlNode parameter and boolean parameter to
// LoadVehicleBuildFromFile().
// 
// - Added an XmlNode parameter and boolean parameter to
// LoadVehicleBuildFromBroadcast().
// 
// - Added TestStatusCheck().
// - Added an XmlNodeMap parameter to SetVehicleBuild().
// - Added an XmlNodeMap parameter to AddVehicleBuildItem().
// - Added an XmlNodeMap parameter to GetVehicleBuildItem().
// - Added an XmlNode parameter and boolean parameter to
// ClearVehicleBuild().
// 
// 12    1/18/07 3:02p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added a version of TranslateBuildRecord() that takes an additional
// XmlNodeMap parameter. 
// - Added TranslateBuildRecordHook().
// 
// 11    10/26/06 1:14p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 10    7/19/06 6:18p Cward
// Made the vehicle build file name configurable.
//
// 1     7/13/06 1:50p Rwiersem
// Made the part of the VIN that specifies the vehicle build file name
// configurable.  If the VIN is not in the vehicle build record, fill it
// in with the VIN used to specify the vehicle build file.
//
// 9     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/24/06 4:29p Rwiersem
// Added HandlePulse() and LoadAdditionalConfigurationItems() methods.
// These are used to reload the configuration without restarting the
// process.
//
// 8     4/13/06 4:26a Cward
// Literal code processing
// c.500 in TranslateBuildRecord() modified and factored conditions for type
// checking of literal code data to permit either previously defined label
// to be used in configuration files
//
// 7     2/15/06 8:12p Cward
// Updated translate broadcast string to work off section lengths provided
// in the braodcast string.
//
// 6     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 5     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 4     5/09/05 5:00p Gpattison
// Re-arranged the Initialize() method to call the base class initialize
// as soon as possible
//
// 4     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 3     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 2     3/02/05 1:30p Gswope
// changed printfs to Log(), added more logging
//
// 2     2/16/05 9:33p Cward
// Translate build record updated to allow for non numeric values as well
// as out of sequence build sub tags
//
// 3     11/24/04 11:54a Bmeinke
// Added GetInputVinLength() method to define the number of characters
// expected in a VIN from the scanner/pendant. Read from
// <DataTags>/<InputVinLength> value. Defaults to 17 (vinlength) if not
// defined in the config file.
//
// 1     7/21/04 10:18a Cward
//
// 3     3/04/04 5:17p Cward
// Added new methods and ivars
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     1/27/04 4:53p Cward
//
// 3     11/29/03 1:28p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 3     8/18/03 7:51a Khykin
// Updated with Flatrock changes.
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/24/03 7:58a Khykin
// Updating with Genes latest changes.
//
// 1     6/20/03 9:59a Khykin
// Initial checkin.
//
// 1     6/12/03 4:01p Khykin
// Initial checkin.
//
// 1     4/22/03 9:49a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:08  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:15a Khykin
// Initial Checkin
//
// 1     1/18/03 8:10a Khykin
// Initial Checkin From DCTC
//
// 14    10/22/02 3:48p Derickso
// Modified to subscribe for NextVIN tag.  When tag is published,
// server will load the vehicle build into NextVehicleBuild and
// publish the build data to any subscribers.
// Updated LoadDataTags() to use macros out of BepTagDefs.h
//
// 13    10/21/02 12:24p Derickso
// Remove ClearVehicleBuild() from destructor.  It is handled
// in ClearData().
// Modified to default client communications debugs to off.
//
// 12    8/23/02 9:39a Dan.erickson
// Changed USR_ROOT to FTP_ROOT.
// Updated UpdateInputServerState method.
//
// 11    8/14/02 12:12p Skypig13
// Removed compiler warnings
//
// 10    6/07/02 12:04p Dan.erickson
// Updated setting of InputServer state logic.
//
// 9     6/03/02 1:01p Dan.erickson
// Wheelbases are now kept in a seperate config file. The format
// of the file is generic and compatible with all manufacturers.
//
// 8     5/20/02 4:30p Dan.erickson
// Made files local to USR_ROOT.
//
// 7     5/10/02 7:39a Dan.erickson
// Added diagnostics to UpdateInputServerState() method.
//
// 6     5/06/02 3:49p Dan.erickson
// Corrected memory leak.
//
// 5     4/24/02 3:16p Dan.erickson
// Need to check XmlNode being returned from
// TranslateBuildRecord() for NULL before trying to invoke the
// Copy() method.
//
// 4     4/17/02 9:12a Dan.erickson
// Modified LoadVehicleBuildFromBroadcast() to check for a NULL
// vehicle build returned from TranslateBuildRecord().
//
// 3     4/16/02 2:44p Dan.erickson
// Added logic to update the VINReadStatus tag according to the
// state of the vehicle build record.
//
// 2     4/04/02 10:27a Dan.erickson
// Moved the call to AddDerivedinfo() so that it is called whether
// the build record is loaded from file or broadcast.
//
// 1     4/03/02 2:03p Dan.erickson
// Created.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "PlantHostInbound.h"
#include "INamedDataBroker.h"

PlantHostInbound::PlantHostInbound() : BepServer(),
m_broker(NULL), m_vehicleBuildFilePath("/VehicleData/BuildRecords/"),
m_updateVINReadStatus(true), m_invalidateBuildRecordOnRead(true), m_alwaysPerformTest(false)
{
}

PlantHostInbound::~PlantHostInbound()
{
    ClearData();
    ClearMaps();
    if (m_broker != NULL) delete m_broker;
    m_broker = NULL;
}

void PlantHostInbound::Initialize(const std::string &fileName)
{
    BepServer::Initialize(fileName);
}

void PlantHostInbound::Initialize(const XmlNode *document)
{
    // Initialize base server
    BepServer::Initialize(document);
    // Make sure the name being used is standard
    Log(LOG_FN_ENTRY,"PlantHostInbound::Initialize()\n");

    // Load the additional configuration items needed by plant host inbound
    LoadAdditionalConfigurationItems(document);

    // Invalidate the build record status
    SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
    Log(LOG_DEV_DATA, "Initialize - SetVehicleBuildRecordStatus: Invalid");
}

void PlantHostInbound::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    XmlNode *setupNode = const_cast<XmlNode *>(document->getChild("Setup"));

    // Load up the data tags we will be using
    LoadDataTags(setupNode);

    // Get the mappings needed for results and build strings
    try
    {
        const XmlNode *broadcastNode = setupNode->getChild("Broadcast");
        LoadDataMaps(broadcastNode);
    }
    catch (XmlException &e)
    {
        Log(LOG_ERRORS, "Could not find Broadcast node in config file or error loading maps!\n");
        Log(LOG_ERRORS, "LoadDataMaps XmlException - %s", e.GetReason());
    }

    // Get the Four Wheel Drive values for each type of transfer case
    try
    {
        SetFourWheelDriveValues(setupNode->getChild("DataTags")->getChild(FOUR_WHEEL_DRIVE_TAG));
    }
    catch (XmlException &XmlErr)
    {   // No values specified
        SetFourWheelDriveValues(NULL);
    }

    // Get the All Wheel Drive values for each type of transfer case
    try
    {
        SetAllWheelDriveValues(setupNode->getChild("DataTags")->getChild(ALL_WHEEL_DRIVE_TAG));
    }
    catch (XmlException &XmlErr)
    {   // No values specified
        SetAllWheelDriveValues(NULL);
    }

    // Get the wheel base configuration
    try
    {
        XmlParser parser;
        const XmlNode *wheelBaseConfig = parser.ReturnXMLDocument(getenv("FTP_ROOT") + setupNode->getChild("WheelbaseConfigFile")->getValue());
        // Set the Wheel base positions
        SetWheelBasePositions(wheelBaseConfig->getChild("VehicleConfig"));
        SetWheelbasePositionTag(setupNode->getChild("DataTags")->getChild("WheelbasePositionTag")->getValue());
    }
    catch (BepException &XmlErr)
    {   // No values specified
        SetWheelBasePositions(NULL);
        SetWheelbasePositionTag("WheelbasePositionInchesX10");
    }     

    Log(LOG_DEV_DATA, "WheelbasePositiontag m_wheelbasePositionTag: %s", m_wheelbasePositionTag.c_str());

    // Check if the build record should be invalidated when read
    try
    {
        StoreInvalidateBuildRecordOnReadFlag(setupNode->getChild("InvalidateBuildRecordOnRead")->getValue() == "True");
    }
    catch (XmlException &XmlErr)
    {   // Option not provided, default to true
        StoreInvalidateBuildRecordOnReadFlag(true);
    }

    // Check if the build record should be invalidated when read
    try
    {
        StoreAlwaysPerformTest(setupNode->getChild("AlwaysPerformTest")->getValue() == "True");

    }
    catch (XmlException &XmlErr)
    {  // Option not provided, default to true
        StoreAlwaysPerformTest(false);
    }
    Log(LOG_DEV_DATA, "CheckVal: AlwaysPerformTest: %s", AlwaysPerformTest() ? "True" : "False");
    // Set the VIN starting index used to create the vehicle build file name
    try
    {
        const XmlNode* node = setupNode->getChild("VehicleBuildFileVinStart");
        SetVehicleBuildFileVinStart(atoi(node->getValue().c_str()));
    }
    catch(XmlException &XmlErr)
    {
        SetVehicleBuildFileVinStart(uniqueVinStart);
    }

    // Set the number of VIN characters to use when creating the vehicle build
    // file name
    try
    {
        const XmlNode* node = setupNode->getChild("VehicleBuildFileVinLength");
        SetVehicleBuildFileVinLength(atoi(node->getValue().c_str()));
    }
    catch(XmlException &XmlErr)
    {
        SetVehicleBuildFileVinLength(minVinLength);
    }
}

void PlantHostInbound::Reset(void)
{   // Clear the maps
    ClearMaps();
    ClearData();
    // Clear the rest of the data
    BepServer::Reset();
}

const std::string PlantHostInbound::Register(void)
{   // Initialize the InputServer comm object
    m_inputServerComm.Initialize(INPUT_SERVER_NAME,"Client",false);
    m_broker = new INamedDataBroker();
    m_promptComm.Initialize(PROMPT_SERVER_NAME, DEFAULT_BUFFER_SIZE, DEFAULT_TIMEOUT,false); 
    Log(LOG_DEV_DATA, "PlantHostInbound::Register - Initialized prompt server communications\n");
    return(BepServer::Register());
}

const std::string PlantHostInbound::Read(const std::string &tag, const INT32 rate) /*=0*/
{
    Log(LOG_FN_ENTRY,"Enter PlantHostInbound::Read(%s)",tag.c_str());
    return(BepServer::Read(tag, rate));
}

const std::string PlantHostInbound::Read(const XmlNode *dataNode, const INT32 rate) /*=0*/
{
    Log(LOG_FN_ENTRY,"Enter PlantHostInbound::Read(Xml)");
    std::string result(""), response;
    if (dataNode->getName() == GetVehicleBuildTag())
    {   // Make sure we have a valid build record first
        Log(LOG_DEV_DATA,"PlantHostInbound::Read() Checking Vehicle Build");
        result = GetVehicleBuildRecordStatus(); // "Valid" or "Invalid"
        if (validStatus == result)
        {   // Send out the vehicle build record
            Log(LOG_DEV_DATA,"PlantHostInbound::Read() Getting Vehicle Build");
            result.erase();
            for (XmlNodeMapItr iter = m_vehicleBuild.begin(); iter != m_vehicleBuild.end(); iter++)
            {   // Build up the record to be passed back
                result += (*iter).second->ToString();
            }
            if (InvalidateBuildRecordOnRead())
            {   // Invalidate the build record since it was just read and update traffic light
                SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
                Log(LOG_DEV_DATA, "Read - SetVehicleBuildRecordStatus: Invalid");
                m_broker->Write(GetVinReadStatusTag(), NO_VIN, response, true);
            }
        }
    }
    else result = BepServer::Read(dataNode, rate);
    Log(LOG_FN_ENTRY,"Exit PlantHostInbound::Read() Returning: %s", result.c_str());
    return(result);
}

const std::string PlantHostInbound::Write(const std::string &tag, const std::string &value)
{
    return(BepServer::Write(tag, value));
}

const std::string PlantHostInbound::Write(const XmlNode *dataNode)
{
    std::string result(BEP_ERROR_RESPONSE), response;
    result = BepServer::Write(dataNode);
    if ((dataNode->getName() == GetBuildRecordTag()) && (m_broker != NULL))
    {
        const std::string vinTag = GetVinReadStatusTag();
        // Update the traffic light widget to let driver know vin is being processed
        m_broker->Write(vinTag, PROCESSING_VIN, response, true);
        // Load the vehicle build record for the specified vin
        result = LoadVehicleBuildRecord(dataNode->getValue(), m_vehicleBuild, true);
    }
    return(result);
}

void PlantHostInbound::ReloadConfiguration(void)
{   // Clear the build map before we continue
    ClearMaps();
    BepServer::ReloadConfiguration();
}

const std::string PlantHostInbound::GetVehicleBuildSource(void)
{   // Check the config file to determine if we need to update the build record source
	XmlParser parser;
	try
	{
		const XmlNode *config = parser.ReturnXMLDocument(m_configFile);
		m_vehicleBuildRecordSource = config->getChild("Setup/VehicleBuildRecordSource")->getValue();
		Log(LOG_DEV_DATA, "Set build record source to %s", m_vehicleBuildRecordSource.c_str());
	}
	catch(...)
	{
		Log(LOG_DEV_DATA, "Error checking for updated build source");
	}
    return(m_vehicleBuildRecordSource);
}

const std::string PlantHostInbound::GetVehicleBuildFilePath(void)
{
    return(m_vehicleBuildFilePath);
}

const std::string PlantHostInbound::GetVehicleBuildTag(void)
{
    return(m_vehicleBuildTag);
}

const std::string PlantHostInbound::GetVehiclePresentTag(void)
{
    return(m_vehiclePresentTag);
}

const std::string PlantHostInbound::GetBuildRecordTag(void)
{
    return(m_buildRecordTag);
}

const std::string PlantHostInbound::GetVinTag(void)
{
    return(m_vinTag);
}

const std::string PlantHostInbound::GetNextVinTag(void)
{
    return(m_nextVinTag);
}

const std::string PlantHostInbound::GetVinDisplayTag(void)
{
    return(m_vinDisplayTag);
}

const std::string PlantHostInbound::GetVinReadStatusTag(void)
{
    return(m_vinReadStatusTag);
}

const std::string PlantHostInbound::GetRetainersDownTag(void)
{
    return(m_retainersDownTag);
}

const std::string PlantHostInbound::GetVehicleBuildRecordStatusTag(void)
{
    return(m_vehicleBuildStatusTag);
}

const std::string PlantHostInbound::GetInputServerStateTag(void)
{
    return(m_inputServerStateTag);
}

const std::string PlantHostInbound::GetVehicleBuildRecordStatus(void)
{
    return(m_vehicleBuildStatus);
}

const std::string PlantHostInbound::GetWheelbasePositionTag(void)
{
    return(m_wheelbasePositionTag);
}

void PlantHostInbound::AddDerivedBuildInfo(XmlNodeMap &buildData)
{   // No generic info to add to the build record
}

const std::string PlantHostInbound::HasAllWheelDrive(const std::string &transferCaseType)
{
    return(std::string("No"));
}

const std::string PlantHostInbound::HasFourWheelDrive(const std::string &transferCaseType)
{
    return(std::string("No"));
};

const std::string PlantHostInbound::Publish(const XmlNode *node)
{
    std::string result = BEP_ERROR_RESPONSE, response, loadResult;
    Log("PlantHostInbound::Publish(%s)\n", node->ToString().c_str());
    result = BepServer::Publish(node);
    // Only do special stuff if publish was successful
    if (result == BEP_SUCCESS_RESPONSE)
    {
        if (node->getName() == GetVehiclePresentTag())
        {   // Update the DataInputServer state
            UpdateInputServerState();
        }
        else if (node->getName() == GetNextVinTag())
        {   // Update the traffic light widget to let driver know vin is being processed
            m_broker->Write(GetVinReadStatusTag(), PROCESSING_VIN, response, true);
            // Load the vehicle build record for the specified vin
            loadResult = LoadVehicleBuildRecord(node->getValue(), m_vehicleBuild, true);
            if (loadResult == BEP_SUCCESS_RESPONSE)
            {   // Publish the vehicle build to any subscribers
                XmlNode nextVehicleBuild(GetVehicleBuildTag(), "");
                for (XmlNodeMapItr iter = m_vehicleBuild.begin(); iter != m_vehicleBuild.end(); iter++)
                {   // Add the build item to the node
                    nextVehicleBuild.addChild(iter->second->Copy());
                }
                // Write the next build data
                Write(&nextVehicleBuild);
                // Get rid of the node
                nextVehicleBuild.clear();
            }
        }
    }
    return(result);
}


const std::string PlantHostInbound::ReadBuildRecordFromBroadcast(const std::string &vin, std::string &buildRecord, const bool &updateStatus)
{   // No generic functionality to implement at this time
    return(BEP_NO_IMPLEMENTED_RESPONSE);
}


void PlantHostInbound::UpdateInputServerState()
{   // Use a semaphore to prevent race condition on InputServerState
    m_SemInputServerState.CriticalSectionEnter();
    Log(LOG_DEV_DATA,"Entered InputServerState Sem in UpdateInputServer()");
    // Check conditions of data and the machine
    bool retainersDown = (Read(GetRetainersDownTag()) == retainingRollsDown ? true : false);
    bool vehiclePresent = (Read(GetVehiclePresentTag()) == vehicleIsPresent ? true : false);
    bool validBuildRecord = (GetVehicleBuildRecordStatus() == validStatus ? true : false);
    std::string response;
    Log(LOG_DEV_DATA,"PlantHostInbound::UpdateInputServerState() -- retainersDown:%s, vehiclePresent:%s, validBuildRecord:%s\n",
        (retainersDown ? "True" : "False"),
        (vehiclePresent ? "True" : "False"),
        (validBuildRecord ? "True" : "False"));

    if (retainersDown && vehiclePresent && validBuildRecord)
    {   // Set the state to Test Result
        m_inputServerComm.Write(GetInputServerStateTag(), INPUT_SERVER_TEST_RESULT_STATE, response, true);
        Log(LOG_DEV_DATA,"Set InputServer state to %s\n", INPUT_SERVER_TEST_RESULT_STATE);
    }
    else if (retainersDown && vehiclePresent)
    {   // Set the state to Vin
        m_inputServerComm.Write(GetInputServerStateTag(), INPUT_SERVER_VIN_STATE, response, true);
        Log(LOG_DEV_DATA,"Set InputServer state to %s\n", INPUT_SERVER_VIN_STATE);
    }
    else if (!retainersDown || !vehiclePresent)
    {   // Set the state to Normal
        m_inputServerComm.Write(GetInputServerStateTag(), INPUT_SERVER_NORMAL_STATE, response, true);
        Log(LOG_DEV_DATA,"Set InputServer state to %s\n", INPUT_SERVER_NORMAL_STATE);
    }
    // Determine if we need to display the VIN
    if (retainersDown && validBuildRecord)
    {   // Display the full VIN on the GUI
        Log(LOG_DEV_DATA,"Displaying VIN on GUI");
        try
        {
            Log( LOG_DEV_DATA, "PlantHostInbound: vinDisplayTag: %s\n",GetVinDisplayTag().c_str());
            if (m_broker != NULL)
            {
                m_broker->Write(GetVinDisplayTag(), m_vehicleBuild.getNode(GetVinTag())->getValue(), response, true);
            }
        }
        catch(...)
        {

            Log( LOG_DEV_DATA, "PlantHostInbound: Problems setting the vin display");           
        }        
    }
    Log(LOG_DEV_DATA,"Looking at exiting this m_SemInputServerState.CriticalSectionExit()");
    m_SemInputServerState.CriticalSectionExit();
    Log(LOG_DEV_DATA,"Exited InputServerState Sem in UpdateInputServer()");
}

void PlantHostInbound::ClearData(void)
{   // Clear the vehicle build
    ClearVehicleBuild(m_vehicleBuild, true);
    ClearVehicleBuild(m_staticTesterBuild, false);
    BepServer::ClearData();
}

// for backward compatibility
void PlantHostInbound::TranslateBuildRecord(const std::string &buildRecord, XmlNodeMap &buildData, const bool &updateStatus)
{
    return(TranslateBuildRecord(buildRecord, buildData, updateStatus, m_buildRecordMap));
}

void PlantHostInbound::TranslateBuildRecord(const std::string &buildRecord, XmlNodeMap &buildData, 
                                            const bool &updateStatus, XmlNodeMap &specificMap)
{
    std::string tag, value;
    INT32 startIndex, length;
    Log(LOG_FN_ENTRY, "PlantHostInbound::TranslateBuildRecord(buildRecord:%s)\n", buildRecord.c_str());
    string nodeString;

    try
    {
        // Iterate through the map and pull out the build information one field at a time
        for(XmlNodeMapItr iter = specificMap.begin(); iter != specificMap.end(); iter++)
        {   // Get the name of the field and pull the field from the build record
            tag = iter->second->getName();
            startIndex = GetStartIndex(iter->second);
            length = atoi(iter->second->getAttribute(FIELD_LENGTH)->getValue().c_str());
            // Ensure the start index is valid
            if (IsStartIndexValid(iter->second, buildRecord.length()))
            {   // Start index is valid
                value = buildRecord.substr(startIndex, length);
                std::string fieldType(iter->second->getAttribute(FIELD_TYPE)->getValue());
                // was (*iter).second->getAttributes().getNode(FIELD_TYPE)->getValue());

                if(BEP_STATUS_SUCCESS != TranslateBuildRecordHook(&fieldType, tag, value))
                {
                    // node was not fully processed in the line above
                    Log(LOG_DEV_DATA,"PlantHostInbound::TranslateBuildRecord - PostHook: <%s>\n", value.c_str());
                    if((fieldType == TYPE_CODE) || (fieldType == TYPE_LITERAL_CODE))
                    {   // Look up the type of component
                        if(tag == PERFORM_ABORT_TEST)
                        {
                            try
                            {
                                // force value to always perform test
                                if (AlwaysPerformTest()) 
                                {
                                    value = "Perform";
                                    Log(LOG_DEV_DATA,"PlantHostInbound always performing test");
                                }
                                else
                                {
                                    value = iter->second->getChild(value)->getValue();
                                    Log(LOG_DEV_DATA,"Reading value from xml node:  value: <%s>", value.c_str());
                                }
                            }
                            catch (...)
                            {
                                value = "Abort";
                            }

                            Log(LOG_DEV_DATA,"Perform status: %s", value.c_str());
                        }
                        else
                        {
                            // if the value is empty/blank/space, tell it to grab the default value. if no default value, ignore. compare returns 0 on equal.
                            Log(LOG_DEV_DATA,"Value index[0]: <0x%02X>\n", value[0]);
                            
                            if (!value.compare(" "))
                            {
                                nodeString = "Default";
                                Log(LOG_DEV_DATA, "empty value. grabbing default value");
                            }
                            else
                            {
                                Log(LOG_DEV_DATA,"compare returned: %d", value.compare(" "));
                                nodeString = "Type" + value; 
                            }

                            Log(LOG_DEV_DATA,"PlantHostInbound::TranslateBuildRecord - nodeString: <%s>\n", nodeString.c_str());


                            try
                            {
                                const XmlNodeMap *translationMap = &iter->second->getChildren();
                                XmlNodeMapCItr valueIter = translationMap->find(nodeString);
                                if(valueIter != translationMap->end())
                                {
                                    value = valueIter->second->getValue();
                                }
                                else
                                {
                                    Log(LOG_ERRORS, "Could not find translation for Tag: <%s> - Type: <%s>", tag.c_str(), value.c_str());
                                    value = "Undefined";
                                }
                            }               
                            catch (...)
                            {
                                Log(LOG_ERRORS, "PlantHostInbound::TranslateBuildRecord - there was an xml fault");
                                value = "Undefined";
                            }

                            Log(LOG_DEV_DATA,"PlantHostInbound::TranslateBuildRecord - nodeString Value: <%s>\n", value.c_str());
                        }
                    }
                }

                if ("Undefined" != value)
                {   // Plug the info into the vehicle build node
                    
                    Log(LOG_DEV_DATA,"Adding tag: %s, value: %s\n", tag.c_str(), value.c_str());
                    AddVehicleBuildItem(tag, value, buildData);
                }
                else
                {
                    Log(LOG_DEV_DATA,"Not updating item in vehicle build. tag: %s, value: %s\n", tag.c_str(), value.c_str());
                }

            }
            else
            {   // Start index is invalid, do not do anything with this data item
                Log(LOG_ERRORS, "Data location for %s is invalid - StartIndex: %d, FieldLength: %d, MessageLength: %d",
                    tag.c_str(), startIndex, length, buildRecord.length());
            }
        }
    }
    catch (...)
    {
        Log(LOG_ERRORS, "PlantHostInbound::TranslateBuildRecord() - XML Exception trying to get data for %s in vehicle build map\n", tag.c_str());
    }
    Log(LOG_FN_ENTRY,"PlantHostInbound::TranslateBuildRecord() - Exit\n");
}

const std::string PlantHostInbound::LoadVehicleBuildRecord(const std::string &vin, 
                                                           XmlNodeMap &buildData, 
                                                           const bool updateStatus)
{   // Clear the vehicle build record before reloading it
    Log(LOG_FN_ENTRY,"Enter PlantHostInbound::LoadVehicleBuildRecord(%s)\n",vin.c_str());
    std::string status(BEP_ERROR_RESPONSE), response, buildRecord;
    // Invalidate vehicle build since we are about to clear it and reload with a new one
    if(updateStatus)
    { 
       SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
       Log(LOG_DEV_DATA, "LoadVehicleBuildRecord - SetVehicleBuildRecordStatus: Invalid");
    }

    ClearVehicleBuild(buildData, updateStatus);
    Log(LOG_DEV_DATA, "LoadVehicleBuildRecord - ClearVehicleBuild updateStatus: %s", updateStatus ? "True" : "False");

    std::string buildDataStr = "";
    std::string source = GetVehicleBuildSource();
    if (SOURCE_FILE == source)
    {   // Get the vehicle build record from file
        status = LoadVehicleBuildFromFile(vin, buildData, updateStatus);
    }
    else if (SOURCE_BROADCAST == source)
    {   // Request the vehicle build record from broadcast
        status = LoadVehicleBuildFromBroadcast(vin, buildData, updateStatus);

        Log(LOG_FN_ENTRY,"PlantHostInbound::LoadVehicleBuildFromBroadcast - Status: %s\n", status.c_str());
    }
    // If the vehicle build was retrieved, store it.
    if (status == BEP_SUCCESS_RESPONSE)
    {   // Add information derived from the vehicle build information to the vehicle build record
        Log(LOG_FN_ENTRY,"Calling AddDerivedBuildInfo\n");
        AddDerivedBuildInfo(buildData);
        if (updateStatus)
        {   // Set the vehicle build status to valid
            Log(LOG_DEV_DATA, "Setting vehicle build status to valid");
            SetVehicleBuildRecordStatus(validStatus);
            m_broker->Write(GetVinReadStatusTag(), READY_TO_TEST, response, true);
        }
        else
        {
              Log(LOG_DEV_DATA, "Not setting vehicle build status to valid");
        }
        Log(LOG_DEV_DATA, "PlantHostInbound::LoadVehicleBuildRecord - Loaded vehicleBuild with:\n");
        for(XmlNodeMapItr iter = buildData.begin(); iter != buildData.end(); iter++)
            Log(LOG_DEV_DATA,"\t%s\n", (*iter).second->ToString().c_str());
        Log(LOG_DEV_DATA, "PlantHostInbound::LoadVehicleBuildRecord - Display complete");
    }
    else
    {   // Log that an error occurred loading vehicle build data
        Log(LOG_DEV_DATA, "PlantHostInbound::LoadVehicleBuildRecord - Error occurred loading vehicle build data");
        m_broker->Write(GetVinReadStatusTag(), NO_VIN, response, true);
        Log(LOG_ERRORS,"Error Loading VehicleBuild from: %s for data: %s\n", source.c_str(), vin.c_str());
    }
    // Update the Input Server State if needed
    if(updateStatus)
    {   // Update the DataInputServer state
        Log(LOG_DEV_DATA, "Updating the input server state");
        UpdateInputServerState();
    }


    Log(LOG_FN_ENTRY,"PlantHostInbound::LoadVehicleBuildRecord() returning %s\n", status.c_str());
    return(status);
}

const std::string PlantHostInbound::LoadVehicleBuildFromFile(const std::string &vin, 
                                                             XmlNodeMap &buildData, 
                                                             const bool &updateStatus)
{
    XmlParser parser;
    std::string requestedVin = vin, status, testStatus;
    try
    {
        if (vin.length() > minVinLength)
        {
            Log(LOG_DEV_DATA,"VIN Start Index: %d, VIN length: %d",
                VehicleBuildFileVinStart(),VehicleBuildFileVinLength());

            requestedVin = vin.substr(VehicleBuildFileVinStart(),
                                      VehicleBuildFileVinLength());
        }

        std::string buildRecordFile = GetVehicleBuildFilePath() + "Vin" + requestedVin + ".xml";
        // Get the vehicle build record from file
        Log("Retrieving build record from %s\n", buildRecordFile.c_str());
        SetVehicleBuild(parser.ReturnXMLDocument(buildRecordFile)->getChild(VEHICLE_BUILD_TAG), buildData);
         
        Log(LOG_DEV_DATA,"Retrieved build record:");
        for(XmlNodeMapItr iter = buildData.begin(); iter != buildData.end(); iter++)
        {
            Log(LOG_DEV_DATA, "\t%s", iter->second->ToString().c_str());
        }
        TestStatusCheck(buildData);
        Log(LOG_DEV_DATA, "Checked test status");
        testStatus = GetVehicleBuildItem(PERFORM_ABORT_TEST, buildData);
        Log(LOG_DEV_DATA, "Test Status: %s", testStatus.c_str());
        if(ABORT_STATUS == testStatus)
        {
            Log(LOG_DEV_DATA, "PlantHostInbound::LoadVehicleBuildFromFile() - testStatus:%s\n", testStatus.c_str());
            status = BEP_ABORT_RESPONSE;
        }
        else
        {
            Log(LOG_DEV_DATA, "PlantHostInbound::LoadVehicleBuildFromFile() - testStatus:%s\n", testStatus.c_str());
            status = BEP_SUCCESS_RESPONSE;
            // if the VIN node is empty, use the passed in VIN
            if(buildData.getNode("VIN")->getValue() == "")
            {
                Log(LOG_DEV_DATA,"<VIN> was blank, using passed in VIN");
                XmlNode *node = const_cast<XmlNode *> (buildData.getNode("VIN"));
                node->setValue(vin);
            }
        }
    }
    catch(XmlException &excpt)
    {
        status = BEP_UNAVAILABLE_RESPONSE;
        Log(LOG_ERRORS, "XmlException: Error loading vehicle build data from file: %s", excpt.GetReason());
        ClearVehicleBuild(buildData, updateStatus);
        Log(LOG_DEV_DATA, "LoadVehicleBuildFromFile1 - ClearVehicleBuild updateStatus: %s", updateStatus ? "True" : "False");
    }
    catch(BepException &excpt)
    {
        status = BEP_UNAVAILABLE_RESPONSE;
        Log(LOG_ERRORS, "BepException: Error loading vehicle build data from file: %s", excpt.GetReason());
        ClearVehicleBuild(buildData, updateStatus);
        Log(LOG_DEV_DATA, "LoadVehicleBuildFromFile2 - ClearVehicleBuild updateStatus: %s", updateStatus ? "True" : "False");
    }
    catch (...)
    {       // There was an error getting the build record
        status = BEP_UNAVAILABLE_RESPONSE;
        Log("Error getting vehicle build from file\n");
        ClearVehicleBuild(buildData, updateStatus);
        Log(LOG_DEV_DATA, "LoadVehicleBuildFromFile3 - ClearVehicleBuild updateStatus: %s", updateStatus ? "True" : "False");
    }
    if (IsDebugOn()) printf("PlantHostInbound::LoadVehicleBuildFromFile() complete, returning: %s\n", status.c_str());
    return(status);
}

const std::string PlantHostInbound::LoadVehicleBuildFromBroadcast(const std::string &vin, 
                                                                  XmlNodeMap &buildData, 
                                                                  const bool &updateStatus)
{   // Read the build record from broadcast
    std::string buildRecord;
    std::string status = ReadBuildRecordFromBroadcast(vin, buildRecord, updateStatus);
    if (status == BEP_SUCCESS_RESPONSE)
    {   // Parse the vehicle build record into the components spoecified in the map
        Log(LOG_DEV_DATA,"Processing successful read from broadcast. Overall length: %d\n",buildRecord.length());
        TranslateBuildRecord(buildRecord, buildData, updateStatus);
    }
    else if ((status == BEP_NO_IMPLEMENTED_RESPONSE) && (vin.length() > vinLength))
    {   // The build record was passed into us, just parse it
        TranslateBuildRecord(vin, buildData, updateStatus);
        status = BEP_SUCCESS_RESPONSE;
    }
    // Make sure the build info is good
    if(buildData.empty())
    { 
        Log(LOG_DEV_DATA,"vin.length(): %d   vinLength: %d\n", vin.length(), vinLength);
        Log(LOG_DEV_DATA,"Some of the build data is empty. Failing build record\n");
        status = BEP_FAILURE_RESPONSE;
    }
    Log(LOG_DEV_DATA,"LoadVehicleBuildFromBroadcast - Exit status: %s", status.c_str());
    return(status);
}

void PlantHostInbound::TestStatusCheck(XmlNodeMap &buildData)
{
    std::string response, testStatus = GetVehicleBuildItem(PERFORM_ABORT_TEST, buildData);
    Log(LOG_DEV_DATA, "PlantHostInbound::TestStatusCheck() - testStatus:%s\n", testStatus.c_str());
    if(PERFORM_STATUS != testStatus)
    {   // Do not allow testing.
        if(AreRetainersDown())
        {   // Display a prompt for the driver for why we cannot test this vehicle
            m_promptComm.DisplayPrompt(1, GetBlockPrompt(), response);
            m_promptComm.SetBackgroundColor(1, GetAbortColor(), response);
            if(testStatus == ABORT_STATUS || testStatus == SHIP_STATUS)
            {   // Vehicle is blocked - Abort Status
                m_broker->Write("PfsBlocked", "1", response, true);
                if(ABORT_STATUS == testStatus)
                {
                    m_promptComm.DisplayPrompt(2, GetAbortPrompt(), response);
                    m_promptComm.SetBackgroundColor(2, GetAbortColor(), response);
                    BposSleep(atoi(GetDataTag("AbortStatusBgDelay").c_str()));
                }
                else
                {
                    m_promptComm.DisplayPrompt(2, GetShipPrompt(), response);
                    m_promptComm.SetBackgroundColor(2, GetShipStatusColor(), response);
                }

                if(m_printOnBlock)
                {   // Create a test result file in order to have the GUI print a test report
                    //   indicating the block
                    // Do not allow testing.
                    // Add information derived from the vehicle build information to the vehicle build record
                    AddDerivedBuildInfo(buildData);

                    Log("PlantHostInbound::TestStatusCheck - Loaded vehicleBuild with:\n");

                    for(XmlNodeMapItr iter = buildData.begin(); iter != buildData.end(); iter++)
                        Log("\t%s\n", (*iter).second->ToString().c_str());

                    // Publish the vehicle build to any subscribers
                    XmlNode abortVehicleBuild("VehicleBuild", "");
                    for(XmlNodeMapItr iter = buildData.begin(); iter != buildData.end(); iter++)
                    {   // Add the build item to the node
                        abortVehicleBuild.addChild(iter->second->Copy());
                    }
                    // Write the next build data
                    m_broker->Write(&abortVehicleBuild,response,true);
                    // Wait for build record to get to TestResultServer
                    BposSleep(2000);
                    // Get rid of the node
                    abortVehicleBuild.clear();
                    m_broker->Write("OverallTest", BEP_ABORT_STATUS, response, true);
                }
            }
            else
            {   // Ok to test, clear the color from the prompt box
                Log("PlantHostInbound::TestStatusCheck - Conditions OK to test");
                m_promptComm.SetBackgroundColor(2, "White", response);
                m_broker->Write("PfsBlocked", "0", response, true);
            }
        }
        else
        {
            Log("PlantHostInbound::TestStatusCheck - The retainers are up");

        }
    }
    else
    {   // Test is a go.  Remove any block status messaging.
        m_promptComm.SetBackgroundColor(1, "White", response);
        m_promptComm.RemovePrompt(1, GetBlockPrompt(), response, 1);
        m_promptComm.SetBackgroundColor(2, "White", response);
        m_promptComm.RemovePrompt(2, GetAbortPrompt(), response, 1);
    }
}

void PlantHostInbound::SetVehicleBuild(const XmlNode *buildRecord, XmlNodeMap &buildData)
{
    buildData.DeepCopy(buildRecord->getChildren());
}

void PlantHostInbound::AddVehicleBuildItem(const std::string &item, const std::string &value, XmlNodeMap &buildData)
{
    XmlNode buildItem(item, value);
    buildData.addNode(buildItem.Copy());
}

inline const std::string PlantHostInbound::GetVehicleBuildItem(const std::string &item, const XmlNodeMap &buildData)
{
    std::string result = BEP_UNAVAILABLE_RESPONSE;
    if(buildData.find(item) != buildData.end())
        result = buildData.getNode(item)->getValue();
    return(result);
}

void PlantHostInbound::SetBuildRecordMap(const XmlNode *buildMap)
{
    try
    {
        m_buildRecordMap.DeepCopy(buildMap->getChildren());
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException in PlantHostInbound::SetBuildRecordMap() - %s", excpt.GetReason());
    }
}

void PlantHostInbound::SetVehicleBuildSource(const std::string &source)
{   // Make sure the source is valid
    if ((source != SOURCE_BROADCAST) && (source != SOURCE_FILE))
    {   // The source was invalid, default to SOURCE_FILE
        Log(LOG_ERRORS, "Invalid vehicle build record source: %s, using %s as source\n", source.c_str(), SOURCE_FILE);
        m_vehicleBuildRecordSource = SOURCE_FILE;
    }
    else
    {   // Valid source provided
        m_vehicleBuildRecordSource = source;
    }
}

void PlantHostInbound::SetWheelBasePositions(const XmlNode *wheelBasePositions)
{
    if (wheelBasePositions != NULL) m_wheelBasePositions.DeepCopy(wheelBasePositions->getChildren());
    else m_wheelBasePositions.clear(true);
}

void PlantHostInbound::SetAllWheelDriveValues(const XmlNode *allWheelDriveValues)
{
    if (allWheelDriveValues != NULL) m_allWheelDrive.DeepCopy(allWheelDriveValues->getChildren());
    else m_allWheelDrive.clear(true);
}

void PlantHostInbound::SetFourWheelDriveValues(const XmlNode *fourWheelDriveValues)
{
    if (fourWheelDriveValues != NULL) m_fourWheelDrive.DeepCopy(fourWheelDriveValues->getChildren());
    else m_fourWheelDrive.clear(true);
}

void PlantHostInbound::SetVehicleBuildFilePath(const std::string &filePath)
{
    m_vehicleBuildFilePath = filePath;
}

void PlantHostInbound::SetVehicleBuildTag(const std::string &tag)
{
    m_vehicleBuildTag = tag;
}

void PlantHostInbound::SetVehiclePresentTag(const std::string &tag)
{
    m_vehiclePresentTag = tag;
}

void PlantHostInbound::SetBuildRecordTag(const std::string &tag)
{
    m_buildRecordTag = tag;
}

void PlantHostInbound::SetVinTag(const std::string &tag)
{
    m_vinTag = tag;
}

void PlantHostInbound::SetNextVinTag(const std::string &tag)
{
    m_nextVinTag = tag;
}

void PlantHostInbound::SetVinDisplayTag(const std::string   &tag)
{
    m_vinDisplayTag = tag;
}

void PlantHostInbound::SetVinReadStatusTag(const std::string &tag)
{
    m_vinReadStatusTag = tag;
}

void PlantHostInbound::SetRetainersDownTag(const std::string &tag)
{
    m_retainersDownTag = tag;
}

void PlantHostInbound::SetVehicleBuildRecordStatusTag(const std::string &tag)
{
    m_vehicleBuildStatusTag = tag;
}

void PlantHostInbound::SetInputServerStateTag(const std::string &tag)
{
    m_inputServerStateTag = tag;
}

void PlantHostInbound::SetVehicleBuildRecordStatus(const std::string &status)
{
    m_vehicleBuildStatus = status;
    // Update the value so we can publish to everyone
	Log(LOG_DEV_DATA, "SetVehicleBuildRecordStatus - setting %s to %s",
		GetVehicleBuildRecordStatusTag().c_str(), m_vehicleBuildStatus.c_str());
    Write(GetVehicleBuildRecordStatusTag(), m_vehicleBuildStatus);
}

void PlantHostInbound::SetWheelbasePositionTag(const std::string &tag)
{
    m_wheelbasePositionTag = tag;
}

void PlantHostInbound::ClearVehicleBuild(XmlNodeMap &buildData, const bool &updateStatus)
{   // Set status first so build record can not be read while it is being cleared
    if(updateStatus)         SetVehicleBuildRecordStatus(BEP_INVALID_RESPONSE);
    if(!buildData.empty())   buildData.clear(true);
}

void PlantHostInbound::ClearMaps(void)
{
    if (!m_buildRecordMap.empty())     m_buildRecordMap.clear(true);
    if(!m_staticTesterBuild.empty())  m_staticTesterBuild.clear(true);
    if (!m_wheelBasePositions.empty()) m_wheelBasePositions.clear(true);
    if (!m_fourWheelDrive.empty())     m_fourWheelDrive.clear(true);
    if (!m_allWheelDrive.empty())      m_allWheelDrive.clear(true);
}

const std::string PlantHostInbound::GetWheelBasePosition(const std::string &selectData)
{   // Return wheelbase position 0
    return("0");
}

void PlantHostInbound::LoadDataMaps(const XmlNode *broadCastNode)
{
    try
    {
        XmlParser parser;
        SetBuildRecordMap(parser.ReturnXMLDocument(getenv("FTP_ROOT") + broadCastNode->getChild("BuildRecordMap")->getValue()));
    }
    catch (XmlException &e)
    {
        Log(LOG_ERRORS, "PlantHostInbound::LoadDataMaps() XmlException: %s\n", e.what());
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "PlantHostInbound::LoadDataMaps() BepException: %s\n", e.what());
    }
}

void PlantHostInbound::LoadDataTags(const XmlNode *setupNode)
{
    try
    {
        SetVehicleBuildFilePath(getenv("FTP_ROOT") + setupNode->getChild("VehicleBuildFilePath")->getValue());
        SetVehicleBuildSource(setupNode->getChild("VehicleBuildRecordSource")->getValue());
        const XmlNode *dataTags = setupNode->getChild("DataTags");
        SetInputServerStateTag(dataTags->getChild("InputServerState")->getValue());
        SetVehicleBuildTag(dataTags->getChild("VehicleBuild")->getValue());
        SetBuildRecordTag(dataTags->getChild("BuildRecord")->getValue());
        SetVinTag(dataTags->getChild("Vin")->getValue());
        SetNextVinTag(dataTags->getChild("NextVIN")->getValue());
        SetVinDisplayTag(dataTags->getChild("VINDisplay")->getValue());
        SetVinReadStatusTag(dataTags->getChild("VINReadStatus")->getValue());
        SetVehiclePresentTag(dataTags->getChild("VehiclePresent")->getValue());
        SetRetainersDownTag(dataTags->getChild("RollsDownElevUp")->getValue());
        SetVehicleBuildRecordStatusTag(dataTags->getChild("VehicleBuildRecordStatus")->getValue());
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "XmlException in PlantHostInbound::LoadDataTags() - %s", excpt.GetReason());
        SetInputServerStateTag(INPUT_SERVER_STATE);
        SetVehicleBuildFilePath(getenv("FTP_ROOT") + std::string("/VehicleData/BuildRecords/"));
        SetVehicleBuildSource("File");
        SetVehicleBuildTag(NEXT_VEHICLE_BUILD_TAG);
        SetBuildRecordTag(BUILD_RECORD_TAG);
        SetVinTag(VIN_DATA_TAG);
        SetNextVinTag(NEXT_VIN_DATA_TAG);
        SetVinDisplayTag(VINDISPLAY_DATA_TAG);
        SetVinReadStatusTag(VIN_READ_STATUS_TAG);
        SetVehiclePresentTag(VEH_PRESENT_DATA_TAG);
        SetRetainersDownTag(ROLLS_DOWN_DATA_TAG);
        SetVehicleBuildRecordStatusTag("VehicleBuildRecordStatus");
    }
    catch (...)
    {
        printf("SOMETHING IS TERRIBLY WRONG LOADING DATA TAGS IN PlantHostInbound\n");
        SetInputServerStateTag(INPUT_SERVER_STATE);
        SetVehicleBuildFilePath(getenv("FTP_ROOT") + std::string("/VehicleData/BuildRecords/"));
        SetVehicleBuildSource("File");
        SetVehicleBuildTag(NEXT_VEHICLE_BUILD_TAG);
        SetBuildRecordTag(BUILD_RECORD_TAG);
        SetVinTag(VIN_DATA_TAG);
        SetNextVinTag(NEXT_VIN_DATA_TAG);
        SetVinDisplayTag(VINDISPLAY_DATA_TAG);
        SetVinReadStatusTag(VIN_READ_STATUS_TAG);
        SetVehiclePresentTag(VEH_PRESENT_DATA_TAG);
        SetRetainersDownTag(ROLLS_DOWN_DATA_TAG);
        SetVehicleBuildRecordStatusTag("VehicleBuildRecordStatus");
    }
}

int PlantHostInbound::GetInputVinLength()
{
    std::string vinLenTagVal;
    int inVinLen = 0;

    // Check if the InputVinLength is defined in the config file
    vinLenTagVal = GetDataTag("InputVinLength");
    // If is defined, so use the custom value
    if (vinLenTagVal.empty() == false)
    {
        inVinLen = atoi(vinLenTagVal.c_str());
    }

    if (0 == inVinLen)
    {
        // Not defined in config file, so use the default value
        inVinLen = vinLength;
    }

    return( inVinLen);
}

INT32 PlantHostInbound::GetStartIndex(const XmlNode *node)
{
    INT32 startIndex = 0;
    try
    {
        startIndex = BposReadInt(node->getAttribute(START_INDEX)->getValue().c_str());
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Could not get start index for %s - %s", node->getName().c_str(), excpt.GetReason());
        startIndex = 0;
    }
    return(startIndex);
}

bool PlantHostInbound::IsStartIndexValid(const XmlNode *field, const INT32 &messageLength)
{   // Determine if the end index is within the message string
    INT32 startIndex = GetStartIndex(field);
    INT32 length = BposReadInt(field->getAttribute(FIELD_LENGTH)->getValue().c_str());
    return (messageLength > (startIndex + length));
}

const bool& PlantHostInbound::InvalidateBuildRecordOnRead(void)
{
    return(m_invalidateBuildRecordOnRead);
}

const bool& PlantHostInbound::AlwaysPerformTest(void)
{
    return(m_alwaysPerformTest);
}

void PlantHostInbound::StoreInvalidateBuildRecordOnReadFlag(const bool &invalidateOnRead)
{
    m_invalidateBuildRecordOnRead = invalidateOnRead;
}

void PlantHostInbound::StoreAlwaysPerformTest(const bool &alwaysPerformTest)
{
    m_alwaysPerformTest = alwaysPerformTest;
}

UINT16 PlantHostInbound::VehicleBuildFileVinStart(void)
{
    return(m_vehicleBuildFileVinStart);
}

void PlantHostInbound::SetVehicleBuildFileVinStart(UINT16 startIndex)
{
    m_vehicleBuildFileVinStart = startIndex;
}

UINT16 PlantHostInbound::VehicleBuildFileVinLength(void)
{
    return(m_vehicleBuildFileVinLength);
}

void PlantHostInbound::SetVehicleBuildFileVinLength(UINT16 length)
{
    m_vehicleBuildFileVinLength = length;
}

/**
 * this is a stub that just returns NA so that the base code works without
 * overriding this method
 * 
 * @return always BEP_STATUS_NA
 */
BEP_STATUS_TYPE PlantHostInbound::TranslateBuildRecordHook(std::string *fieldType, std::string &tag, std::string &value)
{
    // avoid potential compiler warnings
    (void) fieldType; 
    (void) tag; 
    (void) value;
    Log(LOG_FN_ENTRY, "PlantHostInbound::::TranslateBuildRecordHook() done. Returning BEP_STATUS_NA\n");
    return(BEP_STATUS_NA);
}

inline const std::string PlantHostInbound::GetAbortPrompt(void)
{
    return m_abortStatusPrompt;
}

inline const string& PlantHostInbound::GetAbortColor(void)
{
    return m_abortStatusColor;
}

inline const std::string PlantHostInbound::GetBlockPrompt(void)
{
    return m_testBlockPrompt;
}

inline const std::string PlantHostInbound::GetShipPrompt(void)
{
    return m_shipStatusPrompt;
}

inline const string& PlantHostInbound::GetShipStatusColor(void)
{
    return m_shipStatusColor;
}

inline const bool PlantHostInbound::AreRetainersDown(void)
{
    return(Read(m_retainersDownTag) == retainingRollsDown ? true : false);
}
