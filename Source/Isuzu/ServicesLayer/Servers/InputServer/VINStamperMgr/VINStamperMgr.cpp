//*************************************************************************
// FILE DESCRIPTION:
//	The VINSTampermanager handles all data from the VIN Stamper
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/VINStamperMgr/VINStamperMgr.cpp $
//
// 1     2/1/16 4:06p kstutzma
// Created.
//
//*************************************************************************

#include "VINStamperMgr.h"
#include "GeneralUtilities.h"

VINStamperMgr::VINStamperMgr() : InputDeviceBase(),
m_openLogicalPort(true)
{   // Nothing to do here
}
VINStamperMgr::~VINStamperMgr()
{   // Nothing to do here
}

void VINStamperMgr::Initialize(const XmlNode *configNode)
{
    Log(LOG_DEV_DATA,"VINStamperMgr::Initialize()\n");

    // Load the additional configuration items needed by the VINStamperMgr
    LoadAdditionalConfigurationItems(configNode);
    Log(LOG_DEV_DATA, "LoadAdditionalConfigurationItems() - Complete"); 
    
    // Call the base class
    InputDeviceBase::Initialize(configNode);
    Log(LOG_DEV_DATA,"VINStamperMgr::Initialize() complete\n");
}

void VINStamperMgr::LoadAdditionalConfigurationItems(const XmlNode *configNode)
{
    const XmlNode *document = configNode->getChild("Setup");

    // if we should open the logical ports
    if(m_openLogicalPort == true)
    {
        // Get the serial channel config data
        m_stamperComm.Initialize(document->getChild("VinStamperCommunication"));
    }

    // Set the length of the data string
    SetInputDataLength(document->getChild("VinStamperModes/Normal/DataLength")->getValue());

    // Check if we should shorten the data string to the set length
    m_shortenData = (document->getChild("VinStamperModes/Normal/ShortenDataToLength")->getValue() == "Yes" ? true:false);

    // Set the length of the VIN
    m_vinLength = atoi(document->getChild("VinStamperModes/Normal/VINLength")->getValue().c_str());

    // Set the starting position of the VIN number
    m_vinStartPosition = atoi(document->getChild("VinStamperModes/Normal/VINPosition")->getValue().c_str());

}

const std::string VINStamperMgr::Publish(const XmlNode *node)
{   // Call the base class method to do it's thing
    return(BepServer::Publish(node));
}

void VINStamperMgr::Run(volatile bool *terminateFlag) /* =NULL */
{
    Log(LOG_DEV_DATA,"VINStamperMgr::Execute() begin\n");
    unsigned char data[128];
    INT32 byteCount;
    // Look for data until the InputServer tells us to stop
    while (GetStatus() != BEP_TERMINATE)
    {   // Wait for data
        Log(LOG_DEV_DATA,"Waiting for Data\n");
        memset(data, 0, sizeof(data));
        byteCount = m_stamperComm.ReadPort(data, 128, 500, 1000);
        if (byteCount > 0)
        {   // Process the data
            Log(LOG_DEV_DATA, "Found %d bytes\n", byteCount);
            ConvertDataToFile((char *)data, byteCount);
        }
    }
    Log(LOG_DEV_DATA, "VINStamperMgr::Execute() end\n");
}

void VINStamperMgr::ConvertDataToFile(char * data, int byteCount)
{
    Log(LOG_DEV_DATA,"VINStamperMgr::ConvertDataToFile() - Enter\n");
    char fileName[64];
    char command[150];
    char buffer[m_vinLength + 1];
    char shortData[GetInputDataLength()];
    FILE *stampFile = NULL;
    int writeStatus = -99;
    int i;
    int vinEnd = m_vinStartPosition-1+m_vinLength;

    //Check if the data is long enough to have the VIN in it
    if(byteCount >= m_vinStartPosition+m_vinLength)
    {
        // Extract the VIN number from the data
        strncpy(buffer,data+(m_vinStartPosition-1),m_vinLength);
        Log("Extracted vin from file: %s\n", buffer);
    }
    else
    {
        strncpy(buffer,"00000000000000000",m_vinLength);
        Log("Using default VIN\n");
    }
    buffer[m_vinLength] = '\0';
    //Name the file based on the extracted VIN number
    sprintf(fileName, "/tmp/%s.STP", buffer);
    stampFile = fopen(fileName, "w");

    //Write the data to the new .STP file
    if(NULL != stampFile)  
    {
        Log("Opened vin stamp file %s\n", fileName);
        if(m_shortenData)
        {   //Shorten the data to the data length
            Log("Shortening Vin Stamper Data\n");
            for(i = vinEnd; i < (GetInputDataLength() + vinEnd); i++)
            {
                shortData[i] = *data;
                data++;
            }
            if(i > 0)
                writeStatus = fprintf(stampFile, "%s", shortData);
        }
        else
        {
            Log(LOG_DEV_DATA, "Data: %s, Vin End Index %d, ByteCount: %d", data, vinEnd, byteCount);
            char tempVin[10];
            memset(tempVin,0,sizeof(tempVin));
            Log("Copying the rest of the data\n");
            strncpy(tempVin,data+(vinEnd),byteCount-vinEnd-1);
            Log("Writing the data to the file\n");
            writeStatus = fprintf(stampFile, "%s", tempVin);
        }
    }
    Log("Closing the file\n");
    fclose(stampFile);
    //Move the finished STP file to the STP directory
    sprintf(command, "mv %s /home/CCRT/Rewrite/TestResults/STP/",fileName);
    system(command);
    Log(LOG_DEV_DATA,"VINStamperMgr::ConvertDataToFile() - Exit\n");
}

inline const INT32& VINStamperMgr::GetInputDataLength(void)
{
    return m_inputDataLength;
}

inline void VINStamperMgr::SetInputDataLength(const INT32 &length)
{
    m_inputDataLength = length;
}

inline void VINStamperMgr::SetInputDataLength(const std::string &length)
{
    SetInputDataLength(atoi(length.c_str()));
}
