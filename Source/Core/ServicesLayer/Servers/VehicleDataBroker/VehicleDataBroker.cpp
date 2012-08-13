//*************************************************************************
// Description:
//   This defines the VehicleDataBroker object.
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/VehicleDataBroker/VehicleDataBroker.cpp $
// 
// 2     2/07/08 9:58a Derickso
// Updated to support global use bypass part number list tag.
// 
// 1     11/01/07 2:58p Derickso
// Added capability to support a common module fault list.
// 
// 10    10/26/06 1:24p Rwiersem
// Changes for the 06102701 core release:
// 
// Now clear the XmlNodeMaps in the destructor.
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 9     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/07/06 11:01a Rwiersem
// Removed the old ReloadConfiguration() method, added the HandlePulse()
// method, and added the LoadAdditionalConfigurationItems() method.
//
// 8     7/20/05 9:31p Cward
// Added log calls to register method
//
// 7     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 6     5/09/05 5:00p Gpattison
// Re-arranged the Initialize() method to call the base class initialize
// as soon as possible
//
// 6     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/18/05 12:01p Gswope
// added try/catch around attempt to read build record
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     11/15/04 11:29a Bmeinke
// Added a m_useDefDriverNo data meber to say whether or not to use the
// DefaultDriverNumber from the config file when we reset the drive number
// on rolls down or if we should read the current driver number from the
// system
//
// 4     3/04/04 5:33p Cward
// Removed temporary code that was a work around, to get around a PLC
// problem for wheebase adjust
//
// 3     2/26/04 6:35p Cward
// Changes made for the Feb 26, 2004 iteration
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
//
// 3     10/24/03 2:01p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/18/03 7:53a Khykin
// Updated with Flatrock changes.
//
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 3:57p Khykin
// Initial checkin.
//
// 1     4/22/03 9:52a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:40  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:19a Khykin
// Initial Checkin
//
// 1     1/18/03 8:13a Khykin
// Initial Checkin From DCTC
//
// 21    10/22/02 3:54p Derickso
// Modified to read NextVehicleBuild tag from PlantHostInbound
// and publish VehicleBuild to subscribers.
// Updated LoadDataTags method to use macros out of BepTagDefs.h
//
// 20    9/05/02 3:28p Derickso
// Updated to support J2190ActiveVehicleData.
//
// 19    8/23/02 10:47a Dan.erickson
// Updated Initialize method to set the Default driver number.  Updated to
// handle driver number display processing.
//
// 18    7/29/02 8:11a Dan.erickson
// Added functionality to gather active data from the vehicle
// modules during testing.
//
// 17    6/13/02 3:02p Dan.erickson
// Added diagnostic messages.
//
// 16    6/03/02 1:06p Dan.erickson
// Modified UpdatePLC() to only write SET or CLEAR for boolean
// type items.
//
// 15    5/20/02 4:45p Dan.erickson
// Allow UpdateVehicleBuildTags to continue publishing all tags
// even if a failure is encountered.
// Modified return value of Publish().
//
// 14    4/25/02 4:47p Skypig13
// Updated with new changes
//
// 12    4/16/02 7:26a Dan.erickson
// Added code to set and get the vin display tag.
// Use ConvertStatusToResponse().
// In UpdateVehicleBuildTags(), write VINDisplay to the
// NamedDataBroker.
//
// 11    4/03/02 1:47p Dan.erickson
// Removed support for VinDisplay tag.  MachineDataBroker will
// now support this tag.
//
// 10    3/27/02 3:31p Dan.erickson
// Added support to invalidate the driver number when the
// retaining rolls are lowered.
// Added support to publish the vin to the GUI.
//
// 9     3/25/02 9:08a Dan.erickson
// Overloaded Initialize(const std::string&) method.
//
// 8     3/22/02 1:33p Dan.erickson
// Reworked data that gets sent to PLC.  The tags are now coming
// from the config file so they can be changed at any time.
//
// 7     2/18/02 1:09p Dan.erickson
// Reworked for new version of BepServer.
//
// 6     2/12/02 5:19p Dan.erickson
// Moved retrieval of StartTest tag and VehicleBuild tag to LoadDataTags
// method.
// Pass DataTags node to LoadDataTags method.
//
// 5     2/06/02 2:26p Admin
// Checking in for system testing.
//
// 4     1/16/02 4:24p Dan.erickson
// Modified Initialize method to get node name instead of the value.
//
// 3     1/11/02 5:00p Dan.erickson
// Added UpdatePLC method.
// Cleaned up code and used accessor methods to return data.
//
// 2     1/07/02 9:45a Dan.erickson
// Modified Initialize method to reflect new location of
// configuration data under the Setup node.
//
// 1     12/06/01 3:17p Dan.erickson
// Created.
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================

#include "VehicleDataBroker.h"
#include "GeneralUtilities.h"

//-----------------------------------------------------------------------------
VehicleDataBroker::VehicleDataBroker() : BepServer(), m_ndb(NULL), m_plantHostComm(NULL), m_undefinedFault(NULL)
{
}

//-----------------------------------------------------------------------------
VehicleDataBroker::~VehicleDataBroker()
{   // Delete the INdb object if it exists
    if(m_ndb != NULL) delete m_ndb;
    m_ndb = NULL;
    // Delete the plant host comm object
    if(m_plantHostComm != NULL) delete m_plantHostComm;
    m_plantHostComm = NULL;

    // clear and free the XmlNodeMaps
    m_dataToPLC.clear(true);

    if(m_undefinedFault != NULL)  delete m_undefinedFault;
    m_undefinedFault = NULL;
}

//-----------------------------------------------------------------------------
void VehicleDataBroker::Initialize(const std::string &fileName)
{
    BepServer::Initialize(fileName);
}

//-----------------------------------------------------------------------------
void VehicleDataBroker::Initialize(const XmlNode *document)
{
    // Initialize the base server
    BepServer::Initialize(document);
    Log("VehicleDataBroker::Initialize(%s)", document->ToString().c_str());

    // load the additional configuration items
    LoadAdditionalConfigurationItems(document);
}

//-----------------------------------------------------------------------------
void VehicleDataBroker::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    // Make sure the communication channel is set correctly
    std::string srvr(VEHICLE_DATA_BROKER_NAME);
    XmlNode *name = const_cast<XmlNode *>(document->getChild("Setup")->getChild("Communication")->getChild("Name"));
    if(name->getValue() != srvr)
    {   // Set the name properly
        Log("Attempt to use non-standard name: %s, using: %s instead\n", name->getValue().c_str(), srvr.c_str());
        name->setValue(srvr);
    }

    // Get the name of the plant host
    SetPlantHost(document->getChild("Setup")->getChild("PlantHost")->getValue());
    // Load the data tags
    LoadDataTags(document->getChild("Setup"));

    // Set the default driver number
    SetDefaultDriverNumber(DEFAULT_DRIVER_NUMBER);
    XmlNode defaultDriverNumber(GetDriverNumberTag(), GetDefaultDriverNumber());
    Write(&defaultDriverNumber);

    // Get the default background color for the driver number field
    SetDriverNumberBGColor(document->getChild("Setup/DataTags/DriverNumberDisplayBGColorDefault")->getValue());

    // Set the default driver number bg color
    XmlNode driverNumberBGColor(GetDriverNumberBGColorTag(), GetDriverNumberBGColor());

    // Determine if the default driver number should be used on rolls down
    try
    {
        const XmlNode *useDefDrvNo = document->getChild("Setup/DefaultDriverNumberOnRollsDown");
        m_useDefDriverNo = (bool)(useDefDrvNo->getValue() != "0");
    }
    catch( ...)
    {
        m_useDefDriverNo = false;
    }
    // Determine if common module fault list should be loaded
    try
    {
        LoadCommonModuleFaultList(document->getChild("Setup/CommonModuleFaultList"));
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No common module fault list defined - %s", excpt.GetReason());
        m_commonModuleFaultList.clear(true);
    }
    // Load common undefined fault node
    try
    {
        m_undefinedFault = document->getChild("Setup/CommonModuleFaultList/DefaultFaultNode/ModuleFault_xxxx")->Copy();
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No common undefined module fault defined - %s", excpt.GetReason());
        m_undefinedFault = new XmlNode("", "");
        m_undefinedFault->addChild("Code", "", ATTRIBUTE_NODE);
        m_undefinedFault->addChild("ItemCode", "EFLTxx", ATTRIBUTE_NODE);
    }
	// Store the item code length
	UINT8 length = 10;
	try
	{
		length = BposReadInt(document->getChild("Setup/CommonModuleFaultList/ItemCodeLength")->getValue().c_str());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "No Item Code Length defined, using 10 - %s", excpt.GetReason());
		length = 10;
	}
	ItemCodeLength(&length);
}

//-----------------------------------------------------------------------------
const std::string VehicleDataBroker::Read(const XmlNode *node, const INT32 rate /*= 0*/)
{
    std::string result;
    // Only handle special for reading of vehicle build record
    if(node->getName() == GetVehicleBuildTag())
    {   // Build the value from the child nodes
        Log("VehicleDataBroker::Read() - %s\n", node->ToString().c_str());
        XmlNodeMap buildRecord = m_data.getNode(GetVehicleBuildTag())->getChildren();
        for(XmlNodeMapItr iter = buildRecord.begin(); iter != buildRecord.end(); iter++)
            result += iter->second->ToString();
    }
    // Handle request for common module fault
    else if(node->getName() == COMMON_MODULE_FAULT_TAG)
    {
        Log(LOG_DEV_DATA, "Retrieving common fault for: %s", node->getValue().c_str());
        // Get the common fault node
        XmlNodeMapItr iter = m_commonModuleFaultList.find(node->getValue());
        if(iter != m_commonModuleFaultList.end())
        {   // Found the common fault, translate to string for return
            result = iter->second->ToString();
        }
        else
        {   // Common fault not found, use default fault node
            Log(LOG_DEV_DATA, "Could not find %s in common module fault list, creating default", node->getValue().c_str());
            string faultCode = node->getValue().substr(node->getValue().find_first_of("_") + 1);
#if 0
            m_undefinedFault->setName(node->getValue());
            m_undefinedFault->setValue(node->getValue() + ": Undefined fault");
            XmlNode *codeAttrib = const_cast<XmlNode *>(m_undefinedFault->getAttribute("Code"));
            codeAttrib->setValue(faultCode);
            XmlNode *itemAttrib = const_cast<XmlNode *>(m_undefinedFault->getAttribute("ItemCode"));
            itemAttrib->setValue(m_undefinedFault->getAttribute("ItemCode")->getValue() + faultCode);
            // Change to string
            result = m_undefinedFault->ToString();
#else
			XmlNode resultNode(node->getValue(), node->getValue() + ": Undefined Fault");
			resultNode.addChild("Code", faultCode, ATTRIBUTE_NODE);
			string itemCodeValue(m_undefinedFault->getAttribute("ItemCode")->getValue());
			while((itemCodeValue.length() + faultCode.length()) < ItemCodeLength())
			{
				itemCodeValue += "x";
			}
			resultNode.addChild("ItemCode", itemCodeValue + faultCode, ATTRIBUTE_NODE);
			result = resultNode.ToString();
#endif 
        }
    }
    else result = BepServer::Read(node);
    Log(LOG_DEV_DATA, "VehicleDataBroker::Read() returning: %s", result.c_str());
    return(result);
}

//-----------------------------------------------------------------------------
const std::string VehicleDataBroker::Register(void)
{
    Log( LOG_FN_ENTRY, "Enter VehicleDataBroker::Register()\n");
    // Create a new NDB comm object
    if(m_ndb == NULL)
    {
        m_ndb = new INamedDataBroker(IsDebugOn());
    }
    // Create a new Plant host comm object
    if(m_plantHostComm == NULL)
    {
        m_plantHostComm = new IBepCommunication();
        m_plantHostComm->Initialize(GetPlantHost(),"Client",IsDebugOn(), 5000);
    }
    // Write a wheelbase position to the system
    string response;
    m_ndb->Write(WHEELBASE_INCHESX10, GetData(WHEELBASE_INCHESX10), response, true);
    Log( LOG_FN_ENTRY, "Exit VehicleDataBroker::Register()\n");

    return BepServer::Register();
}

//-----------------------------------------------------------------------------
const std::string VehicleDataBroker::Publish(const XmlNode *node)
{   // Only do something special for StartTestTag
    std::string result(BEP_ERROR_RESPONSE);
    Log("VehicleDataBroker::Publish(node: %s)\n", node->ToString().c_str());

    if((node->getName() == GetOverallTestTag()) && (node->getValue() != "Testing"))
    {   // Invalidate the driver number so it has to be entered for next test
        Log("Processing %s - Clearing DriverID information\n", node->ToString().c_str());
        XmlNode driverNumberNode(GetDriverNumberTag(), GetDefaultDriverNumber());
        Log("Writing %s to internal data\n", driverNumberNode.ToString().c_str());
        result = Write(&driverNumberNode);
        Log("Write result: %s\n", result.c_str());
        if(result != BEP_SUCCESS_RESPONSE)
        {
            Log("HEY! We Could Not Invalidate The Drivers Number!\n");
            result = BEP_SUCCESS_RESPONSE;
        }
    }
    else if(node->getName() == GetCableConnectTag())
    {   // Determine whether to idle or resume the threads
        Log("Vehicle cable %s\n", (node->getValue() == SET) ? "Connected" : "Disconnected");
    }
    else if((node->getName() == GetRetainersDownTag()) && (node->getValue() == ROLLS_DOWN))
    {   // Clear the Driver number background color
        std::string response;
        XmlNode driverNumberBGColorNode(GetDriverNumberBGColorTag(), GetDriverNumberBGColor());
        XmlNode driverNumberDisplayNode(GetDriverNumberDisplayTag(), "    ");

        if( m_useDefDriverNo)
        {
            driverNumberDisplayNode.setValue(GetDataTag("DefaultDriverNumber"));
        }
        else
        {
            driverNumberDisplayNode.setValue( Read(GetDriverNumberTag()));
        }
        Log("Setting the BG color to the default color: %s and Driver Number to %s",
            driverNumberBGColorNode.getValue().c_str(), driverNumberDisplayNode.getValue().c_str());
        m_ndb->Write(&driverNumberDisplayNode, response);
        m_ndb->Write(&driverNumberBGColorNode, response, true);
        Log("Response from INamedDataBroker write: %s\n", response.c_str());
    }
    else result = BepServer::Publish(node);
    return(result);
}

//-----------------------------------------------------------------------------
const std::string VehicleDataBroker::Command(const XmlNode *node)
{
    std::string result;

    Log( LOG_FN_ENTRY, "Enter VehicleDataBroker::Command( %s)\n", node->getName().c_str());

    if( node->getName() == GetDataTag("ReadLatestBuildData"))
    {
        try
        {
            result = ReadLatestVehicleBuildRecord( true);
            // Update the bypass part number option
            UpdateUseBypassPartNumberListTag();
        }
        catch(...)
        {
            result  = BEP_UNAVAILABLE_RESPONSE;
        }
        if(result != BEP_SUCCESS_RESPONSE)
            Log(LOG_ERRORS, "HEY! We Need A Vehicle Build!\n");
    }
    else
    {
        result = BepServer::Command( node);
    }

    Log( LOG_FN_ENTRY, "Exit VehicleDataBroker::Command( %s)\n", node->getName().c_str());

    return( result);
}

//-----------------------------------------------------------------------------
const std::string VehicleDataBroker::ReadLatestVehicleBuildRecord(bool publishAfterRead)
{
    std::string tag, value, response, result(BEP_ERROR_RESPONSE);
    XmlParser parser;
    // Initialize the WCC Comm Object and Connect to WorkCellController object
    Log("VehicleDataBroker::ReadLatestVehicleBuildRecord( %d)\n", publishAfterRead);
    // Read the vehicle build info
    if((m_plantHostComm->Read(GetNextVehicleBuildTag(), response, true)) == BEP_STATUS_SUCCESS)
    {   // Get the response from the Read method
        Log("Read vehicle build data from %s - response:%s, length: %d\n", GetPlantHost().c_str(), response.c_str(), response.length());
        // if the response is not invalid, process the information
        if(response.compare("Invalid"))
        {
            m_plantHostComm->GetNext(tag, value, response);
            // Parse the response into an Xml document
            parser.Parse(response.c_str(), response.length(), true, BEP_XML_ROOT);
            XmlNode *vehicleBuildRecord = const_cast<XmlNode *>(parser.getDocElement()->getChild(GetNextVehicleBuildTag()));
            // Update the name of the vehicle build node
            vehicleBuildRecord->setName(GetVehicleBuildTag());
            Log("Vehicle build Record: %s\n", vehicleBuildRecord->ToString().c_str());
            // Save the new vehicle build record
            if((result = UpdateVehicleBuildRecord(vehicleBuildRecord, publishAfterRead)) == BEP_SUCCESS_RESPONSE)
            {   // Update the tags we publish to all subscribers
                result = UpdateVehicleBuildTags(vehicleBuildRecord, publishAfterRead);
            }
        }
        else
            Log(LOG_ERRORS, "ERROR: StartTest received without a valid Vehicle Build\n");
    }
    else Log("Error reading vehicle build information\n");

    Log("VehicleDataBroker::ReadLatestVehicleBuildRecord(%d) done, returning %s\n", publishAfterRead, result.c_str());
    return(result);
}

//-----------------------------------------------------------------------------
const std::string VehicleDataBroker::UpdateVehicleBuildRecord(const XmlNode *vehicleBuildRecord, bool publishRecord)
{
    std::string result;

#if 0
    /********************************************************************************
        Calling m_data.delNode() will cause a leak because XmlNodeMap::delNode()
        DOES NOT de-allocate any memory. The calls to Write()/SetData() below will
        perform the necessary memory de-allocation and update the m_data node map
    ********************************************************************************/
    try
    {   // Remove the old vehicle build record
        m_data.delNode(GetVehicleBuildTag());
    }
    catch(...)
    {   // Do nothing.  May not have stored a vehicle build record yet
    }
#endif

    // Add and publish the new record
    if( publishRecord)  result = Write(vehicleBuildRecord);
    // Just update our build data
    else                result = SetData( vehicleBuildRecord);

    return(result);
}

//-----------------------------------------------------------------------------
const std::string  VehicleDataBroker::UpdateVehicleBuildTags(const XmlNode *vehicleBuildRecord, bool publishTags)
{   // Get the child nodes from the build record
    XmlNodeMap buildInfo = vehicleBuildRecord->getChildren();
    // Write each tag and publish
    std::string result(BEP_SUCCESS_RESPONSE), response;
    if(buildInfo.size())
    {
        for(XmlNodeMapItr iter = buildInfo.begin(); iter != buildInfo.end(); iter++)
        {
            Log("Updating Data: %s, %s\n", (*iter).second->getName().c_str(), (*iter).second->getValue().c_str());
            if( publishTags)    
            {
                // If the data tag is not equal to the SecondarySelectionDataTag, publish it.
                // Publishing the SecondarySelectionDataTag here will cause the build record to endlessly reload
                if ((*iter).second->getName().compare(GetDataTag("SecondarySelectionDataTag"))) 
                {
                    result = Write((*iter).second);
                }
                else
                    Log("Not writing tag %s because it is already published", (*iter).second->getName().c_str());
            }
            else                
            {
                result = SetData( iter->second);
            }
        }
    }
    else
        Log("There Is No Data In The Build Record\n");

    if( publishTags)
    {
        // Update the PLC for the new drive configuration
        if(result == BEP_SUCCESS_RESPONSE) result = UpdatePLC(vehicleBuildRecord);
        if(result == BEP_SUCCESS_RESPONSE)
        {   // Write the VINDisplay tag
            Log("Updating The VINDisplay with: %s\n", buildInfo.getNode(GetVinTag())->getValue().c_str());
            result = ConvertStatusToResponse(m_ndb->Write(GetVinDisplayTag(), buildInfo.getNode(GetVinTag())->getValue(), response, true));
        }
    }
    else
    {
        Log( LOG_DEV_DATA, "Not publishing build tags this time\n");
    }
    return(result);
}

//-----------------------------------------------------------------------------
const std::string VehicleDataBroker::UpdatePLC(const XmlNode *vehicleBuildRecord)
{
    std::string response, tag, value;
    INT32 status = BEP_STATUS_SUCCESS;
    // Write all the data to the PLC
    string logMessage;
    char buf[128];
    for(XmlNodeMapItr iter = m_dataToPLC.begin() ; iter != m_dataToPLC.end(); iter++)
    {
        logMessage += CreateMessage(buf, sizeof(buf), "\t%s\n", iter->second->ToString().c_str());
    }
    Log(LOG_DEV_DATA, "PLC tags to write:\n%s", logMessage.c_str());
    for(XmlNodeMapItr iter = m_dataToPLC.begin(); (iter != m_dataToPLC.end()) && (status == BEP_STATUS_SUCCESS); iter++)
    {   // Get the data from the build record
        tag.erase();
        tag = (*iter).second->getName();
        if(vehicleBuildRecord->getChildren().find(tag) != vehicleBuildRecord->getChildren().end())
        {
            Log(LOG_DEV_DATA, "VehicleDataBroker::UpdatePLC() - Writing tag:%s, value:%s", tag.c_str(),
                vehicleBuildRecord->getChild(tag)->getValue().c_str());
            if(iter->second->getAttributes().getNode("DataType")->getValue() == booleanType)
            {   // Translate a Yes/No into the appropriate SET/CLEAR value
                value = (vehicleBuildRecord->getChild(tag)->getValue() == "Yes" ? SET : CLEAR);
            }
            else
            {   // This is not a boolean value, just send to PLC as is
                value = vehicleBuildRecord->getChild(tag)->getValue();
            }
            status = m_ndb->Write(tag, value, response, true);
			Log(LOG_DEV_DATA, "VehicleDataBroker::UpdatePLC() - Wrote tag:%s, value:%s - status: %s", 
				tag.c_str(), vehicleBuildRecord->getChild(tag)->getValue().c_str(), ConvertStatusToResponse(status).c_str());
        }
        else
        {
            Log(LOG_DEV_DATA, "%s not in the vehicle build data. Not sending to PLC", tag.c_str());
        }
    }
    // Return the status
    return(ConvertStatusToResponse(status));
}

//-----------------------------------------------------------------------------
void VehicleDataBroker::LoadDataTags(const XmlNode *dataTags)
{
    try
    {   // Get the data tags
        SetVinTag(dataTags->getChild("DataTags/VIN")->getValue());
        SetVinDisplayTag(dataTags->getChild("DataTags/VINDisplay")->getValue());
        SetDriverNumberTag(dataTags->getChild("DataTags/DriverNumber")->getValue());
        SetDriverNumberDisplayTag(dataTags->getChild("DataTags/DriverNumberDisplay")->getValue());
        SetStartTestTag(dataTags->getChild("DataTags/StartTest")->getValue());
        SetVehicleBuildTag(dataTags->getChild("DataTags/VehicleBuild")->getValue());
        SetNextVehicleBuildTag(dataTags->getChild("DataTags/NextVehicleBuild")->getValue());
        SetRetainersDownTag(dataTags->getChild("DataTags/RollsDownElevUp")->getValue());
        SetCableConnectTag(dataTags->getChild("DataTags/CableConnect")->getValue());
        SetDriverNumberBGColorTag(dataTags->getChild("DataTags/DriverNumberBGColor")->getValue());
        SetOverallTestTag(dataTags->getChild("DataTags/OverallTest")->getValue());
        SetPlcData(dataTags->getChild("DataToPLC"));
    }
    catch(...)
    {   // Load default values
        Log(LOG_ERRORS, "SOMETHING IS TERRIBLY WRONG LOADING DATA TAGS FOR VehicleDataBroker!\n");
        SetVinTag(VIN_DATA_TAG);
        SetVinDisplayTag(VINDISPLAY_DATA_TAG);
        SetDriverNumberTag(DRIVER_NUMBER_DATA_TAG);
        SetDriverNumberDisplayTag(DRIVER_NUMBER_DISPLAY_DATA_TAG);
        SetStartTestTag(START_TEST_DATA_TAG);
        SetVehicleBuildTag(VEHICLE_BUILD_TAG);
        SetNextVehicleBuildTag(NEXT_VEHICLE_BUILD_TAG);
        SetRetainersDownTag(ROLLS_DOWN_DATA_TAG);
        SetCableConnectTag(CBL_CONNECT_DATA_TAG);
        SetDriverNumberBGColorTag(DRIVER_NUMBER_DISPLAY_BGCOLOR_DATA_TAG);
        SetOverallTestTag("OverallTest");
    }
}

//-----------------------------------------------------------------------------
inline IBepCommunication* VehicleDataBroker::PlantHostComm(void)
{
    return m_plantHostComm;
}

//-----------------------------------------------------------------------------
void VehicleDataBroker::LoadCommonModuleFaultList(const XmlNode *faultListData)
{   // Get the name of the fault file
    string fileName;
    bool loadFaultData = true;
    try
    {
        fileName = getenv("USR_ROOT") + faultListData->getChild("FaultFile")->getValue();
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No fault list file defined, not loading common module faults: %s", excpt.GetReason());
        loadFaultData = false;
    }
    if(loadFaultData)
    {
        const XmlNodeMap *lineFields = NULL;
        // Get the line format data
        try
        {
            lineFields = &(faultListData->getChild("LineFormat")->getAttributes());
        }
        catch(XmlException &excpt)
        {
            Log(LOG_ERRORS, "No line format defined, not using common module faults: %s", excpt.GetReason());
            loadFaultData = false;
        }
        if(loadFaultData)
        {
            string dataLineIndicator;
            bool allowCommentLines = true;
            try
            {
                dataLineIndicator = faultListData->getChild("DataLine")->getValue();
            }
            catch(XmlException &excpt)
            {   // No data line indicator defined, default to all lines are data lines
                Log(LOG_ERRORS, "No data line indicator defined, all lines are data lines: %s", excpt.GetReason());
                allowCommentLines = false;
            }
            // Get the maximum line length
            INT32 lineLength = 80;   // Defaul value if not specified
            try
            {
                lineLength = BposReadInt(faultListData->getChild("MaxLineLength")->getValue().c_str());
            }
            catch(XmlException &excpt)
            {   // No line length defined, default to 80 characters
                Log(LOG_ERRORS, "No line length defined, defaulting to 80 characters per line: %s", excpt.GetReason());
            }
            // Get the field separator
            string fieldSep = "\t";  // Default
            try
            {
                fieldSep = faultListData->getChild("FieldSeparator")->getValue();
            }
            catch(XmlException &excpt)
            {   // No field separator defined, default to tab character
                Log(LOG_ERRORS, "No field separator defined, defaulting to tab character: %s", excpt.GetReason());
            }
            FILE *faultFile = NULL;
            // Open the fault file
            if((faultFile = fopen(fileName.c_str(), "r")) != NULL)
            {   // Get each line from the file
                char buffer[lineLength];
                while(fgets(buffer, sizeof(buffer), faultFile) != NULL)
                {   // Process the current line if it is a data line
                    string line(buffer);
                    // Check if this is a comment line
                    bool commentLine = false;
                    if(allowCommentLines)
                    {   // Check if this is a data line
                        commentLine = dataLineIndicator.compare(line.substr(0,dataLineIndicator.size()));
                    }
                    // If the current line is not a comment line, process it
                    if(!commentLine)
                    {   // Parse all field out and place into a vector, removing leading spaces from all fields
                        vector<string> fields;
                        char *currentField = strtok(buffer, fieldSep.c_str());
                        fields.push_back(currentField);
                        while((currentField = strtok(NULL, fieldSep.c_str())) != NULL)
                        {   // Trim off any leading whitespace
                            string temp(currentField);
                            fields.push_back(temp.substr(temp.find_first_not_of(" ")));
                        }
                        // Convert the data into valid fault nodes
                        XmlNode faultNode("", "");
                        UINT32 fieldIndex = 0;
                        for(XmlNodeMapCItr iter = lineFields->begin(); (iter != lineFields->end()) && (fieldIndex < fields.size()); iter++, fieldIndex++)
                        {   // Decode the current field
                            Log(LOG_DEV_DATA, "Processing field: %s with value: %s", iter->second->getValue().c_str(), fields[fieldIndex].c_str());
                            if(!iter->second->getValue().compare("Code"))
                            {   // Process the Code field - Set the node name and add the Code attribute
                                // Remove 0x from fault code if it is present
                                string fc = fields[fieldIndex].substr(2,fields[fieldIndex].length() - 2);
                                INT32 charIndex = 0;
                                while(!isspace(fc[charIndex++]));
                                fc = fc.substr(0,charIndex-1);
                                faultNode.setName("ModuleFault_" + fc);
                                faultNode.addChild(DTC_CODE, fc, ATTRIBUTE_NODE);
                            }
                            else if(!iter->second->getValue().compare("Description"))
                            {   // Process the fault description field
                                faultNode.setValue(fields[fieldIndex]);
                            }
                            else
                            {   // This is an attribute to the current fault code
                                string attribValue = fields[fieldIndex];
                                // Remove any leading spaces
                                while(isspace(attribValue[0]))  attribValue = attribValue.substr(1, attribValue.npos);
                                // Remove any trailing spaces
                                INT32 charIndex = 0;
                                while(!isspace(attribValue[charIndex++]));
                                attribValue = attribValue.substr(0,charIndex-1);
                                // Add the attribute to the node
                                faultNode.addChild(iter->second->getValue(), attribValue, ATTRIBUTE_NODE);
                            }
                        }
                        // Add the current node to the common fault list
                        Log(LOG_DEV_DATA, "Adding new fault node to common list: %s", faultNode.ToString().c_str());
                        m_commonModuleFaultList.addNode(faultNode.Copy());
                    }
                }
                // Close the file
                fclose(faultFile);
            }
            else
            {   // Could not open the file
                Log(LOG_ERRORS, "Could not open %s - %s", fileName.c_str(), strerror(errno));
            }
        }
    }
}

//-----------------------------------------------------------------------------
void VehicleDataBroker::UpdateUseBypassPartNumberListTag()
{   // Retrieve the config file data
    XmlParser parser;
    string useBypassListPath("Setup/TagListTagListTagListTagListTagList/PublishTags/");
    string useBypassListTag("UseBypassPartNumberList");
    XmlNode *useBypassList = NULL;
    try
    {
        useBypassList = const_cast<XmlNode *>(parser.ReturnXMLDocument(m_configFile)->getChild(useBypassListPath + useBypassListTag));
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "WARNING: Could not find %s, setting use bypas list to False", useBypassListTag.c_str());
    }
    // Publish the information to the system
    if(useBypassList != NULL)
    {
        Write(useBypassList);
    }
}

//-----------------------------------------------------------------------------
const UINT8& VehicleDataBroker::ItemCodeLength(const UINT8 *length /*= NULL*/)
{
	if(length != NULL)   m_itemCodeLength = *length;
	return m_itemCodeLength;
}
