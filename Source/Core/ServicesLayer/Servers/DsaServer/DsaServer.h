//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The DSA Server will interface to the Multi-Function Tester (MFT) using the DSA Protocol.
//  Messages will be received from the MFT and processed.  A response will be generated to
//  transmit back to the MFT.
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
#ifndef DsaServer_h
#define DsaServer_h
//-----------------------------------------------------------------------------
#include "BepServer.h"
#include "SerialChannel.h"
#include "DsaIn.h"
#include "DsaOut.h"
//-----------------------------------------------------------------------------

const string NoService           = "NoService";
const string ServiceRequestAck   = "ServiceRequestAck";
const string ServiceCompletePass = "ServiceCompletePass";
const string ServiceCompleteFail = "ServiceCompleteFail";
const string ServiceInProgress   = "ServiceInProgress";

const bool TestInProgress = true;
const bool TestNotInProgress = false;

const float MphToKph      = 1.609344;
const float PoundToNewton = 4.44822162;

typedef basic_string<INT8>	DsaSerialString_t;

class DsaIn;
class DsaOut;

/**
 * DSA Protocol Server.
 * This class is intended to interface with external test equipment using the DSA protocol.
 * The repsponse received from the test equipment will be processed.  An acknowledgement 
 * message will be generated and returned to the external test equipment.
 * 
 * @author Dan Erickson
 * @see BepServer
 * @see SerialChannel
 * @since February 18, 2008
 */
class DsaServer : public BepServer
{
public:
    /**
     * Class constructor.
     */
    DsaServer();
    /**
     * Class destructor.
     */
    virtual ~DsaServer();
    /**
     * <b>Description:</b>
     * Method for initialization passing in the file name.
     *
     * @param fileName Name of the XML configuration file to be used to 
     *                 initialize.
     */
    virtual void Initialize(const string &fileName);
    /**
     * <b>Description:</b>
     * Method for initialization passing an XML node.
     *
     * <b>Configuration Items:</b>
     *     <ul>
     *     <li> Setup/Debug </li>
     *     <li> Setup/DataTags </li>
     *     </ul>
     * 
     * @param document Pointer to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * <b>Description:</b>
     * Method that is called to command the component to perform any system 
     * registration functions necessary.
     *
     * @return BEP_SUCCESS_RESPONSE if the registration was successful
     *         BEP_FAILURE_RESPONSE if the registration was not successful
     */
    virtual const string Register(void);
    /**
     * <b>Description:</b>
     * Main run method.  This method runs until the status is BEP_TERMINATE.
     *
     * @param terminateFlag Flag used to terminate the system.  (this is unused)
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    /**
     * <b>Description:</b>
     * Write the requested data item from the server.
     *
     * @param node  The data tag and value to write.
     * 
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const string Publish(const XmlNode *node);
    /**
     * The requested service ID will be stored.  The service ID wil be used to determine how to
     * respond to the MFT.
     * 
     * @param requestedService
     *               Requested service ID.
     */
    const UINT8& RequestedService(const UINT8 *requestedService = NULL);
    /**
     * Update the service processing status.
     * The status of processing the service will be updated for transmitting to the MFT.
     * 
     * @param status Current processing status of the requested service.
     */
    void UpdateServiceStatus(const string &status);
    /**
     * Update the specified data byte in the response message.
     * 
     * @param dataByte Data byte location to be updated.
     * @param data     Data value to be stored in the response message.
     */
    void UpdateResponseData(const string &dataByte, INT8 data);
    /**
     * Update a data bit in the response data.
     * 
     * @param byte   Response byte to be updated.
     * @param bit    Data bit to be updated.
     * @param value  Value to store in the data byte.
     */
    void UpdateResponseDataBit(const string &byte, const UINT8 &bit, INT8 value);
    /**
     * Update the roller data in the response message.
     */
    void UpdateRollerData(void);
    /**
     * Get the status of the currently requested service.
     * 
     * @return Status of the currently requested service.
     */
    const string& CurrentServiceStatus(void);
    /**
     * Get the location of the specified bit.
     * 
     * @param bitDescription
     *               Description of the bit location.
     * 
     * @return Location of the requested bit.
     */
    const UINT8 GetBitLocation(string bitDescription);
    /**
     * Get/Set the flag indicating if a test is 
     * 
     * @param testInProgress
     * 
     * @return 
     */
    const bool& TestInProgress(const bool *testInProgress = NULL);
    /**
     * Send the response message to the MFT.
     * 
     * @param response Message to send to the MFT.
     * 
     * @return Flag indicating if the message transmit was successful.
     */
    bool SendResponse(DsaSerialString_t &response);

protected:
    /**
     * Get the status value for the requested service status.
     * 
     * @param status Service status to get value for.
     * 
     * @return Service status value to send to the MFT.
     */
    INT8 GetServiceStatus(const string &status);

private:
    /**
     * Get/Set the timeout for DSA messages.
     * 
     * @param timeout Timeout for DSA messages.
     * 
     * @return Timeout for DSA messages.
     */
    const long& DsaMessageTimeout(const long *timeout = NULL);
    /**
     * Store the config data indicating where the specific bits are located in certain bytes.
     * 
     * @param bitLocations
     *               Bit locations/definitions.
     */
    void StoreBitLocations(const XmlNode *bitLocations);
    /**
     * Convert the provided wheel speed from mph to 1/100 kph.
     * 
     * @param speed  Wheel speed to convert.
     * 
     * @return Converted wheel speed.
     */
    INT32 ConvertWheelSpeed(const float &speed);
    /**
     * Load the speed data into the response message.
     * 
     * @param rollerCount
     *                 Number of rollers currently supported.
     * @param dataSize Size of the data expected to be read from the data file.
     */
    void UpdateSpeedData(const INT32 &rollerCount, const INT32 &dataSize);
    /**
     * Load the acceleration data into the response message.
     * 
     * @param rollerCount
     *                 Number of rollers currently supported.
     * @param dataSize Size of the data expected to be read from the data file.
     */
    void UpdateAccelData(const INT32 &rollerCount, const INT32 &dataSize);
    /**
     * Load the force data into the response message.
     * 
     * @param rollerCount
     *                 Number of rollers currently supported.
     * @param dataSize Size of the data expected to be read from the data file.
     */
    void UpdateForceData(const INT32 &rollerCount, const INT32 &dataSize);
    /**
     * The service ID that was requested by the MFT.
     */
    INT8 m_requestedService;
    /**
     * Timeout for sending/receiveing messages between test station and MFT.
     */
    long m_messageTimeout;
    /**
     * Flag indicating if a test is currently in progress.
     */
    bool m_testInProgress;
    /**
     * File descriptor for the speed file.
     */
    INT32 m_speedFile;
    /**
     * File descriptor for the force file.
     */
    INT32 m_forceFile;
    /**
     * File descriptor for the current accelerations file.
     */
    INT32 m_accelFile;
    /**
     * Copy of the configuration data used to setup the class.
     */
    XmlNode *m_config;
    /**
     * List of valid service status values.
     */
    XmlNodeMap m_serviceStatusList;
    /**
     * Definitions of bit locations.
     */
    XmlNodeMap m_bitDefinitions;
    /**
     * Status of the current requested service.
     */
    string m_currentServiceStatus;
    /**
     * Communication object to use for getting/sending DSA messages.
     * 
     * @see SerialChannel
     */
    SerialChannel *m_dsaComm;
    /**
     * Object to process and handle the incoming DSA message traffic.
     * 
     * @see DSaIn
     */
    DsaIn *m_dsaIncoming;
    /**
     * Object to handle the processing of the outgoing DSA message.
     * 
     * @see DsaOut
     */
    DsaOut *m_dsaOutgoing;
};

//-----------------------------------------------------------------------------
#endif  // DsaServer_h
