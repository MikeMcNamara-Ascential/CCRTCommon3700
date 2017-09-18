//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/ModuleServer/VehicleModuleVinReader/VehicleModuleVinReader.cpp 2     4/18/0 $
//
// FILE DESCRIPTION:
//	The VehicleModuleVinReader will read the VIN from a module
//  in the vehicle and send it to the WorkCellController so the build data 
//  can be retrieved.
//
//===========================================================================
// Copyright (c) 2005- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/ModuleServer/VehicleModuleVinReader/VehicleModuleVinReader.cpp $
// 
// 2     4/18/07 1:58p Jsemann
// 1.  LoadAdditionalConfigurationItems()(
//      a.  Added get CommModulePath and CommModuleFileName from config
// file
// 2.  LoadAdditionalConfigurationItems
//      a.  Added call to WaitForCommModule()
// 3.  WaitForCommModule() Added - needed to prevent system lockup on
// first      boot of new software install
// 4.  FindCommLink() Added
// 
// 1     11/13/06 3:54p Bbarrett
// Add Module VIN Reader.
// 
// 1     8/29/06 12:53p Jsemann
// Ported in from old ServicesLayer structure
// 
// 1     8/29/06 12:26p Jsemann
// 
// 1     3/03/06 12:32p Jsemann
// 3/3/06
// 
// 1     10/31/05 1:22p Jsemann
//==============================================================================
#include "VehicleModuleVinReader.h"
#include <sys/dir.h>

VehicleModuleVinReader::VehicleModuleVinReader() : InputDeviceBase() , m_vinModule(NULL)
{//Nothing to do here
}

VehicleModuleVinReader::~VehicleModuleVinReader()
{   // Get rid of the NDB comm object
    if (m_ndb != NULL)  delete m_ndb;
    m_ndb = NULL;
}

void VehicleModuleVinReader::Initialize(const XmlNode *configNode)
{   // Decide what type of protocol to talk
    InputDeviceBase::Initialize(configNode);
    Log(LOG_DEV_DATA, "VehicleModuleVinReader: Initializing VIN reader...");
    // load the additional configuration items needed by the Module VIN Reader
    LoadAdditionalConfigurationItems(configNode);
}

void
VehicleModuleVinReader::LoadAdditionalConfigurationItems(const XmlNode *configNode)
{
    string protocol;
    try
    {
        protocol = configNode->getChild("CommunicationProtocol")->getValue();
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Defaulting to KeywordProtocol2000 for comm protocol: %s", excpt.GetReason());
        protocol = "KeywordProtocol2000";
    }
    try
    {
        SetVinTag(configNode->getChild("Vin")->getValue());
    }
    catch (...)
    {
        SetVinTag("VIN");
    }
    try
    {
        SetPresenceTag(configNode->getChild("PresenceDetected")->getValue());
    }
    catch (...)
    {
        SetPresenceTag("WirelessCableConnected");
    }
    try
    {
        SetReaderModeTag(configNode->getChild("ReaderMode")->getValue());
    }
    catch (...)
    {
        SetReaderModeTag("ReadVIN");
    }
    try
    {
        SetCommModulePath(configNode->getChild("CommModulePath")->getValue());
    }
    catch (...)
    {
        SetCommModulePath("/home/CCRT/Rewrite/");
    }
    try
    {
        SetCommModuleFileName(configNode->getChild("CommModuleFileName")->getValue());
    }
    catch (...)
    {
        SetCommModuleFileName("GryphonCCan");
    }
	try
	{
		m_vinReadMessages.DeepCopy(configNode->getChild("VinReadMessages")->getChildren());
	}
	catch (XmlException &err)
	{	// Error loading vin read messages
		Log(LOG_ERRORS, "XmlException loading VinReadMessages - %s\n", err.what());
	}
    try
	{
		m_presenceMessages.DeepCopy(configNode->getChild("PresenceMessages")->getChildren());
	}
	catch (XmlException &err)
	{	// Error loading vin read messages
		Log(LOG_ERRORS, "XmlException loading PresenceMessages - %s\n", err.what());
	}
    // Create a new module with the given protocol filter
    if (!protocol.compare("KeywordProtocol2000"))
    {
        Log(LOG_DEV_DATA, "VehicleModuleVinReader: Creating object to read VIN from the module...");
        m_vinModule = new GenericModuleTemplate<KwpCanProtocolFilter>();
        Log(LOG_DEV_DATA, "VehicleModuleVinReader: Created object to read VIN from the module");
    }
    else
    {   // Unsupported module type
        Log(LOG_ERRORS, "Unsupported protocol: %s", protocol.c_str());
        m_vinModule = NULL;
    }
    // Get the configuration file to use
    if (m_vinModule)
    {
        string configFile;
        try
        {
            //wait for comm module link to be created
            WaitForCommModule();
            Log(LOG_DEV_DATA, "Loading module configuration file...");
            configFile = configNode->getChild("ModuleConfigFile")->getValue();
            XmlParser parser;
            const XmlNode* config = parser.ReturnXMLDocument(getenv("FTP_ROOT") + configFile);
            if (config != NULL)
            {   // Got the config file, initialize the interface
                Log(LOG_DEV_DATA, "Initializing module interface...");
                m_vinModule->Initialize(config);
                Log(LOG_DEV_DATA, "Initializing module interface - complete");
            }
            else
            {
                Log(LOG_ERRORS, "Could not get config file.  Deleting vin module interface!");
                delete m_vinModule;
                m_vinModule = NULL;
            }
        }
        catch (XmlException &excpt)
        {
            Log(LOG_ERRORS, "Could not get config file: %s", excpt.GetReason());
            delete m_vinModule;
            m_vinModule = NULL;
        }
    }
}


const std::string VehicleModuleVinReader::Publish(const XmlNode *node)
{
    if (!node->getName().compare(GetDataTag("CableConnect")) && !node->getValue().compare(cableConnected))
    {   // Check if a test is in progress
        string response, tag, testInProgress;
        m_ndb->Read("TestInProgress", response, true);
        m_ndb->GetNext(tag, testInProgress, response);
        if (testInProgress.compare("1"))
        {   // Test is not in progress, allow VIN to be scanned from module
            Log(LOG_DEV_DATA, "Cable was just connected, signaling to managers");
            m_cableConnected.Signal(cableConnected);
            BposSleep(500);
            m_cableConnected.Signal(cableNotConnected);
        }
    }
    else if (!node->getName().compare(GetDataTag("VehiclePresent")))
    {   // Check if a test is in progress
        m_vehPresentSignal = atob(node->getValue().c_str());
    }
    // Call the base class method to do it's thing
    return(BepServer::Publish(node));
}

void VehicleModuleVinReader::Run(volatile bool *terminateFlag) /* =NULL */
{
    Log(LOG_DEV_DATA,"VehicleModuleVinReader::Run() begin\n");
    if (!m_readerModeTag.compare("CheckPresence")) 
    {
        if (m_ndb == NULL)  m_ndb = new INamedDataBroker(IsDebugOn());
        while (GetStatus() != BEP_TERMINATE)
        {   
            if(m_vehPresentSignal)
            {
                // Check for presence of vehicle comm
                CheckPresence();
            }
            else
            {
                Log(LOG_DEV_DATA,"Waiting for vehicle present before checking for vehicle module.");
                XmlNode commPresence(GetPresenceTag(), "0");
                string response;
                m_inputServerComm->Write(&commPresence,response,true);
            }
            //Allow time for presence to change
            BposSleep(500);
        }
    }
    else
    {
        m_cableConnected.Acquire();
        if (m_ndb == NULL)  m_ndb = new INamedDataBroker(IsDebugOn());
        while (GetStatus() != BEP_TERMINATE)
        {   // Wait for the cable to be plugged in
            Log(LOG_DEV_DATA,"Waiting for cable to be connected\n");
            m_cableConnected.Wait(cableConnected);
            Log(LOG_DEV_DATA, "Vehicle cable connected, reading vin from module.");
            // The cable has been connected, read the VIN from the module
            ReadVIN();
            //Allow time for m_cableConnected to become false
            BposSleep(500);
        }
        // Release the cond var and return
        m_cableConnected.Release();
    }
    
}

void VehicleModuleVinReader::ReadVIN(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if (m_vinModule)
    {   // Read the VIN from the module
        string vin;
        try 
        {
            for (XmlNodeMapItr iter = m_vinReadMessages.begin();
                (iter != m_vinReadMessages.end()) && (BEP_STATUS_SUCCESS != status);
                iter++)
            {//iterate through vin read messages - if status success exit
                status = m_vinModule->ReadModuleData(iter->second->getValue().c_str(),vin);
            }
        }
        catch (...)
        {
        status = m_vinModule->ReadVIN(vin);
        }

        if (BEP_STATUS_SUCCESS == status)
        {   // VIN read from the vehicle, publish it to the world
            Log(LOG_DEV_DATA, "Read vin from the module: %s", vin.c_str());
            if (vin.length() >= 17)
            {
                XmlNode vinData(GetVinTag(), vin);
                XmlNode vinDisplayData(VINDISPLAY_DATA_TAG, vin);
                string response;
                m_inputServerComm->Write(&vinData,response,true);
                //update display here
                m_inputServerComm->Write(&vinDisplayData,response,true);
            }
            else
            {
                Log(LOG_ERRORS, "Invalid VIN read from module: %s", vin.c_str());
            }
        }
        else
        {   // Could not read VIN from the module
            Log(LOG_ERRORS, "Failed to read VIN from the vehicle - status: %s", ConvertStatusToResponse(status).c_str());
        }
    }
    else
    {   // VIN module has not been created, cannot read VIN
        Log(LOG_ERRORS, "VIN module reader has not been instantiated, not reading VIN!");
    }
}

void VehicleModuleVinReader::CheckPresence(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if (m_vinModule)
    {   // Communicate with the module
        string resp;
        try 
        {
            for (XmlNodeMapItr iter = m_presenceMessages.begin();
                (iter != m_presenceMessages.end()) && (BEP_STATUS_SUCCESS != status);
                iter++)
            {//iterate through presence messages - if status success exit
                Log(LOG_DEV_DATA, "Checking for presence of vehicle module.");
                status = m_vinModule->ReadModuleData(iter->second->getValue().c_str(),resp);
            }
        }
        catch (...)
        {
        status = BEP_STATUS_FAILURE;
        }

        if (BEP_STATUS_SUCCESS == status)
        {   // Heard response from the vehicle, publish it to the world
            Log(LOG_DEV_DATA, "Communication established with the vehicle module. Cable must be connected.");
            XmlNode commPresence(GetPresenceTag(), "1");
            string response;
            m_inputServerComm->Write(&commPresence,response,true);
        }
        else
        {   // Could not talk to the module
            Log(LOG_ERRORS, "Failed to establish communication with vehicle module - status: %s. Cable must not be connected.", ConvertStatusToResponse(status).c_str());
            XmlNode commPresence(GetPresenceTag(), "0");
            string response;
            m_inputServerComm->Write(&commPresence,response,true);
        }
    }
    else
    {   // Module has not been created, cannot talk to the module
        Log(LOG_ERRORS, "Module reader has not been instantiated, not checking vehicle comm!");
    }
}

void VehicleModuleVinReader::WaitForCommModule(void)
{
    bool commLinkFound = false;
    do
    {   // check if link to communications module exists
        if ((commLinkFound = FindCommLink()))
        {
            Log(LOG_DEV_DATA, "Communication Module link created: VehicleModuleVinReader ready to start\n");
        }
        else
        {
            Log(LOG_ERRORS, "Link to communication module not yet created\n");
        }
        BposSleep(500);
        // Keep looking until link is created and we are not terminated
    } while (GetStatus() != BEP_TERMINATE && !commLinkFound);
}

bool VehicleModuleVinReader::FindCommLink(void)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    bool fileFound = false;
    if ((dir = opendir(m_commModulePath.c_str())) != NULL)
    {   // Directory is open, loop through and get all the files
        for (;;)
        {
            entry = readdir(dir);
            if (entry == NULL)
            {
                break;
            }
            else
            {   // print the name of the file
                string file(entry->d_name);
                Log(LOG_DEV_DATA, "File Name:%s Looking for: %s\n",file.c_str(), m_commModuleFileName.c_str());
                // determine if the file should be added to the list
                if (file.find(m_commModuleFileName) != file.npos)
                {//file found
                    fileFound = true;
                    break;
                }
            }
        }
        closedir(dir);
    }
    else
    {
        Log(LOG_ERRORS, "Could not open directory\n");
    }
    return fileFound;
}
