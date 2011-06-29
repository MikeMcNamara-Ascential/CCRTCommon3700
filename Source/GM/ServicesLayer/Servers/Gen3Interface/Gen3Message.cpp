//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The  Gen 3 Message class handles setting data in the particular gen 3
//  message it is configured to handle.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//*************************************************************************
#include "Gen3Message.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Gen3Message::Gen3Message()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
Gen3Message::~Gen3Message()
{   // Clean out the location info
    for(DataLocationMapIter iter = m_dataLocations.begin(); iter != m_dataLocations.end(); iter++)
    {
        delete iter->second;
    }
}

//-----------------------------------------------------------------------------
void Gen3Message::Initialize(const XmlNode *config)
{   // Get the message length
    UINT8 data = 0;
    try
    {
        data = BposReadInt(config->getChild("Length")->getValue().c_str());
    }
    catch(XmlException &rxcpt)
    {   // Default to 5 bytes
        data = 5;
    }
    // Set up the message
    m_gen3Message.reserve(data);
    m_gen3Message.resize(data);
    // Save the data locations for the message
    try
    {
        const XmlNodeMap *locations = &config->getChild("DataLocations")->getChildren();
        for(XmlNodeMapCItr iter = locations->begin(); iter != locations->end(); iter++)
        {
            LocationInfo *info = new LocationInfo();
            info->index = BposReadInt(iter->second->getValue().c_str());
            const XmlNodeMap &attributes = iter->second->getAttributes();
            // Save the scale factor
            XmlNodeMapCItr scale = attributes.find("scaleFactor");
            if(scale != attributes.end()) info->scale = atof(scale->second->getValue().c_str());
            else  info->scale = 1;
            // Check if this is a bit based location and get the bit
            XmlNodeMapCItr bitLocation = attributes.find("bit");
            if(bitLocation != attributes.end()) info->bit = BposReadInt(bitLocation->second->getValue().c_str());
            else info->bit = -1;
            // Check if this is a bit based location and get the bit mask
            XmlNodeMapCItr mask = attributes.find("mask");
            if(mask != attributes.end()) info->mask = (UINT8)BposReadInt(mask->second->getValue().c_str());
            else info->mask = 0x00;
            // Check if this item is a parameter
            XmlNodeMapCItr param = attributes.find("parameter");
            if(param != attributes.end())
            {
                info->isParam = true;
                info->param = BposReadInt(param->second->getValue().c_str());
            }
            else
            {
                info->isParam = false;
                info->param = -1;
            }
            // Save the data
            m_dataLocations.insert( DataLocationMapVal(iter->second->getName(), info) );
        }
    }
    catch(XmlException &excpt)
    {
        m_dataLocations.clear();
    }
}

//-----------------------------------------------------------------------------
void Gen3Message::ClearMessageContents(void)
{
    if(m_msgMutex.Acquire() == EOK)
    {
        m_gen3Message.assign(m_gen3Message.size(), 0x00);
        m_msgMutex.Release();
    }
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::UpdateIcmDataLocations(const float forces[], const float speeds[], 
                                                    const float distances[], const UINT8 &rollerCount,
                                                    const INT32 icmIndex)
{
    static const string rollerId[] = {"Lf", "Rf", "Lr", "Rr", "Lt", "Rt"};
    // Update icm data for each wheel
    LocationInfo *info = NULL;
    for(UINT8 roller = 0; roller < rollerCount; roller++)
    {   // Get the info for the force data and update
        info = GetLocationInfo(rollerId[roller]+"Force");
        if(info != NULL) PopulateDataLocation(info, forces[roller]);
        // Get the info for the speed data and update
        info = GetLocationInfo(rollerId[roller]+"Speed");
        if(info != NULL) PopulateDataLocation(info, speeds[roller]);
        // Get the info for the distance data and update
        info = GetLocationInfo(rollerId[roller]+"Distance");
        if(info != NULL) PopulateDataLocation(info, distances[roller]);
    }
    // Store the icm index
    info = GetLocationInfo("TagIndex");
    if(info != NULL)  PopulateDataLocation(info, (INT16)icmIndex);
    return BEP_STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::UpdateDataItem(const string &dataItem, const INT16 &dataValue)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    LocationInfo *info = GetLocationInfo(dataItem);
    if(info != NULL) 
    {
        status = PopulateDataLocation(info, dataValue);
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::UpdateDataItem(const string &dataItem, const UINT8 &dataValue)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    LocationInfo *info = GetLocationInfo(dataItem);
    if(info != NULL) 
    {
        status = PopulateDataLocation(info, dataValue);
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::UpdateDataItem(const string &dataItem, const float &dataValue)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    LocationInfo *info = GetLocationInfo(dataItem);
    if(info != NULL) 
    {
        status = PopulateDataLocation(info, dataValue);
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::UpdateDataItem(const string &dataItem, const string &dataValue)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Get the location information
    LocationInfo *info = GetLocationInfo(dataItem);
    if(info != NULL)
    {   // Convert the data to an integer
        INT16 data = BposReadInt(dataValue.c_str());
        status = PopulateDataLocation(info, data);
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::GetMessage(SerialString_t &message)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if(EOK == m_msgMutex.Acquire())
    {   // Mutex acquired, copy the message
        message.assign(m_gen3Message.begin(), m_gen3Message.end());
        m_msgMutex.Release();   // Relase the mutex since we are done with the message.
        status = BEP_STATUS_SUCCESS;
    }
    else
    {
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
LocationInfo* Gen3Message::GetLocationInfo(const string &locationName)
{
    LocationInfo *info = NULL;
    DataLocationMapIter iter = m_dataLocations.find(locationName);
    if(iter != m_dataLocations.end())
    {
        info = iter->second;
    }
    else
    {
        info = NULL;
    }
    return info;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::PopulateDataLocation(const LocationInfo *dataLocationInfo, const float &dataValue)
{   // Scale the data
    INT16 data = (INT16)(dataValue * dataLocationInfo->scale);
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if(EOK == m_msgMutex.Acquire())
    {   // Add the data to the message locations
        m_gen3Message[dataLocationInfo->index] = ((data & 0xFF00) >> 8);
        m_gen3Message[dataLocationInfo->index + 1] = (data & 0x00FF);
        m_msgMutex.Release();
        status = BEP_STATUS_SUCCESS;
    }
    else
    {
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::PopulateDataLocation(const LocationInfo *dataLocationInfo, const UINT8 &dataValue)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if(EOK == m_msgMutex.Acquire())
    {
        m_gen3Message[dataLocationInfo->index] = (UINT8)(dataValue * dataLocationInfo->scale);
        m_msgMutex.Release();
        status = BEP_STATUS_SUCCESS;
    }
    else
    {
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::PopulateDataLocation(const LocationInfo *dataLocationInfo, const UINT32 &dataValue)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if(EOK == m_msgMutex.Acquire())
    {   // Break the data into 4 bytes
        m_gen3Message[dataLocationInfo->index]   = (UINT8)((dataValue & 0xFF000000) >> 24);
        m_gen3Message[dataLocationInfo->index+1] = (UINT8)((dataValue & 0x00FF0000) >> 16);
        m_gen3Message[dataLocationInfo->index+2] = (UINT8)((dataValue & 0x0000FF00) >> 8);
        m_gen3Message[dataLocationInfo->index+3] =  (UINT8)(dataValue & 0x000000FF);
        m_msgMutex.Release();
        status = BEP_STATUS_SUCCESS;
    }
    else
    {
        status = BEP_STATUS_FAILURE;
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE Gen3Message::PopulateDataLocation(const LocationInfo *dataLocationInfo, const INT16 &dataValue)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    if(EOK == m_msgMutex.Acquire())
    {   // Determine if this is a bit update
        if((dataLocationInfo->bit == -1) && (dataLocationInfo->mask == 0x00))
        {   // Not a bit update, just assign the value
            INT16 data = (INT16)(dataValue * dataLocationInfo->scale);
            m_gen3Message[dataLocationInfo->index] = ((data & 0xFF00) >> 8);
            m_gen3Message[dataLocationInfo->index + 1] = (data & 0x00FF);
        }
        else
        {   // This is a bit update, make a mask
            UINT8 mask;
            if(dataLocationInfo->bit != -1)          mask = 0x01 << dataLocationInfo->bit;
            else if(dataLocationInfo->mask != 0x00)  mask = dataLocationInfo->mask;
            else                                     mask = 0x00;
            // Determine if the bit should be cleared or set
            if(dataValue)   m_gen3Message[dataLocationInfo->index] |= mask;   // Set the bits
            else            m_gen3Message[dataLocationInfo->index] &= ~mask;  // Clear the bits
        }
        m_msgMutex.Release();
        status = BEP_STATUS_SUCCESS;
    }
    else
    {
        status = BEP_STATUS_FAILURE;
    }
    return status;
}
