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
#ifndef Gen3Message_h
#define Gen3Message_h
//-----------------------------------------------------------------------------
#include <map>
#include "BepSync.h"
#include "Gen3Interface.h"
//-----------------------------------------------------------------------------
typedef struct _LocationInfo
{
    int   index;   // Index of the message data
    float scale;   // Scaling to be applied to the data prior to inserting the data into the message.
    int   bit;     // Bit to stuff the data into if this is a bit based location.  If this is -1, it is not a bit based location.
    UINT8 mask;    // Mask to use to set/clear multiple bits
    bool  isParam; // Flag to indicate if the item is a parameter
    int   param;   // Parameter number if the item is a parameter
} LocationInfo;

typedef map<string, LocationInfo*>          DataLocationMap;
typedef DataLocationMap::iterator        DataLocationMapIter;
typedef DataLocationMap::const_iterator  DataLocationMapCIter;
typedef DataLocationMap::value_type      DataLocationMapVal;
//-----------------------------------------------------------------------------

class Gen3Message
{
public:
    /**
     * Calss Constructor.
     */
    Gen3Message();
    /**
     * Class destructor
     */
    virtual ~Gen3Message();
    /**
     * Initialize the Gen 3 message.
     * 
     * @param config Configuration data to use for initializing the Gen 3 message.
     */
    void Initialize(const XmlNode *config);
    /**
     * Clear the contents of the message string.
     */
    void ClearMessageContents(void);
    /**
     * Update the ICM data locations in the message.
     * 
     * @param forces    Current ICM force data.
     * @param speeds    Current ICM speed data.
     * @param distances Current ICM pulse count data.
     * @param rollerCount
     *                  Number of rollers on the machine.
     * 
     * @return Status of updating the ICM data locations in the message.
     */
    BEP_STATUS_TYPE UpdateIcmDataLocations(const float forces[], const float speeds[], 
                                           const float distances[], const UINT8 &rollerCount,
                                           const INT32 icmIndex);
    /**
     * Update a data item in the message.
     * 
     * @param dataItem  Item to be updated.
     * @param dataValue New value to store in the message.
     * 
     * @return Status of updating the data item.
     */
    BEP_STATUS_TYPE UpdateDataItem(const string &dataItem, const INT16 &dataValue);
    /**
     * Update a data item in the message.
     * 
     * @param dataItem  Item to be updated.
     * @param dataValue New value to store in the message.
     * 
     * @return Status of updating the data item.
     */
    BEP_STATUS_TYPE UpdateDataItem(const string &dataItem, const UINT8 &dataValue);
    /**
     * Update a data item in the message.
     * 
     * @param dataItem  Item to be updated.
     * @param dataValue New value to store in the message.
     * 
     * @return Status of updating the data item.
     */
    BEP_STATUS_TYPE UpdateDataItem(const string &dataItem, const float &dataValue);
    /**
     * Update a data item in the message.
     * The location information will be checked to determine if the item is a bit based piece of information.
     * 
     * @param dataItem  Item to be updated in the message.
     * @param dataValue Value to be updated in the message.
     * 
     * @return Status of updating the data item in the message.
     */
    BEP_STATUS_TYPE UpdateDataItem(const string &dataItem, const string &dataValue);
    /**
     * Get the raw message data.
     * 
     * @param message Message structure to populate with the current message.
     * 
     * @return Status of getting the current message data.
     */
    BEP_STATUS_TYPE GetMessage(SerialString_t &message);

private:
    /**
     * Find the index for the specified data location.  If the location is not supported or not found, -1 will be returned.
     * 
     * @param locationName
     *               Name of the data location to find.
     * 
     * @return Index for the data location or -1 if the location is not supported.
     */
    LocationInfo* GetLocationInfo(const string &locationName);
    /**
     * Populate the specified message location with the provided data value.
     * 
     * @param dataLocation
     *                  Data location to be updated.
     * @param dataValue Data value to place in the message.
    * 
     * @return Status of updating the message location.  If the location cannot be updated, BEP_STATUS_FAILURE 
     *         will be returned.  Otherwise, BEP_STATUS_SUCCESS will be returned.
     */
    BEP_STATUS_TYPE PopulateDataLocation(const LocationInfo *dataLocationInfo, const float &dataValue);
    /**
     * Populate the specified message location with the provided data value.
     * 
     * @param dataLocationInfo
     *                  Data location to be updated.
     * @param dataValue Data value to place in the message.
     * 
     * @return Status of updating the message location.  If the location cannot be updated, BEP_STATUS_FAILURE 
     *         will be returned.  Otherwise, BEP_STATUS_SUCCESS will be returned.
     */
    BEP_STATUS_TYPE PopulateDataLocation(const LocationInfo *dataLocationInfo, const UINT8 &dataValue);
    /**
     * Populate the specified message location with the provided data value.
     * 
     * @param dataLocationInfo
     *                  Data location to be updated.
     * @param dataValue Data value to place in the message.
     * 
     * @return Status of updating the message location.  If the location cannot be updated, BEP_STATUS_FAILURE 
     *         will be returned.  Otherwise, BEP_STATUS_SUCCESS will be returned.
     */
    BEP_STATUS_TYPE PopulateDataLocation(const LocationInfo *dataLocationInfo, const UINT32 &dataValue);
    /**
     * Populate the specified message location with the provided data value.
     * 
     * @param dataLocationInfo
     *                  Data location to be updated.
     * @param dataValue Data value to place in the message.
     * 
     * @return Status of updating the message location.  If the location cannot be updated, BEP_STATUS_FAILURE 
     *         will be returned.  Otherwise, BEP_STATUS_SUCCESS will be returned.
     */
    BEP_STATUS_TYPE PopulateDataLocation(const LocationInfo *dataLocationInfo, const INT16 &dataValue);
    /**
     * The Gen 3 message string to send to the DVT.
     */
    SerialString_t m_gen3Message;
    /**
     * Locations for the ICM data.
     */
    DataLocationMap m_dataLocations;
    /**
     * Mutex for accessing the raw message.
     */
    BepMutex m_msgMutex;
};

//-----------------------------------------------------------------------------
#endif //Gen3Message_h
