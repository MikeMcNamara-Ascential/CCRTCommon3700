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
#include "DsaServer.h"

//-----------------------------------------------------------------------------
DsaServer::DsaServer() : BepServer(), 
m_requestedService(-1),m_testInProgress(false), m_speedFile(-1), m_forceFile(-1), m_accelFile(-1),
m_dsaComm(NULL), m_dsaIncoming(NULL), m_dsaOutgoing(NULL)
{   // Nothing special to be done
}

//-----------------------------------------------------------------------------
DsaServer::~DsaServer()
{   // Ensure the DSaIn object has been cleaned up
    if(m_dsaIncoming != NULL)  delete m_dsaIncoming;
    m_dsaIncoming = NULL;
    // Close the roller data files
    if(m_speedFile != -1)
    {
        close(m_speedFile);
        m_speedFile = -1;
    }
    if(m_forceFile != -1)
    {
        close(m_forceFile);
        m_forceFile = -1;
    }
    if(m_accelFile != -1)
    {
        close(m_accelFile);
        m_accelFile = -1;
    }
}

//-----------------------------------------------------------------------------
void DsaServer::Initialize(const string &fileName)
{   // Call the base class to begin the intialization.
    BepServer::Initialize(fileName);
    // Store the message timeout
}

//-----------------------------------------------------------------------------
void DsaServer::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization procedure
    BepServer::Initialize(document);
    // Store a copy of the config
    m_config = document->Copy();
    // Create a new DSA Incoming object
    if(m_dsaIncoming == NULL)
    {
        m_dsaIncoming = new DsaIn(document, this);
    }
    // Create a new DSA Outgoing object
    if(m_dsaOutgoing == NULL)
    {
        m_dsaOutgoing = new DsaOut(document, this);
    }
    // Store the DSA Message Timeout
    long timeout = 0;
    try
    {
        timeout = BposReadInt(document->getChild("Setup/IncomingMessageData/MessageTimeout")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "No DSA message timeout provided, using 3 seconds - %s", excpt.GetReason());
        timeout = 3000;
    }
    DsaMessageTimeout(&timeout);
    Log(LOG_DEV_DATA, "Set DSA Message timeout to %d", timeout);
    // Store the service status translations
    try
    {
        m_serviceStatusList.DeepCopy(document->getChild("Setup/Parameters/ServiceProcessingStatus")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Service status translations not found: %s", excpt.GetReason());
        m_serviceStatusList.clear(true);
    }
    // Store the bit definition data
    StoreBitLocations(document->getChild("Setup/OutgoingMessageData/BitLocations"));
    Log(LOG_DEV_DATA, "Stored service process status list");
}

//-----------------------------------------------------------------------------
const string DsaServer::Register(void)
{   // Call the base class to begin the registration process
    string result = BepServer::Register();
    // Create a new comm object to Get/Send DSA messages
    if(m_dsaComm == NULL)
    {
        m_dsaComm = new SerialChannel();
        m_dsaComm->Initialize(m_config->getChild("Setup/DsaCommunications"));
    }
    // Open the speed file
    m_speedFile = open(GetDataTag("CurrentSpeeds").c_str(), O_RDONLY);
    if(m_speedFile == -1)
    {   // Could not open the speed file
        Log(LOG_ERRORS, "Could not open %s - %s", GetDataTag("CurrentSpeeds").c_str(), strerror(errno));
    }
    // Open the force file
    m_forceFile = open(GetDataTag("CurrentForces").c_str(), O_RDONLY);
    if(m_forceFile == -1)
    {   // Could not open the force file
        Log(LOG_ERRORS, "Could not open %s - %s", GetDataTag("CurrentForces").c_str(), strerror(errno));
    }
    // Open the accel file
    m_accelFile = open(GetDataTag("CurrentAccels").c_str(), O_RDONLY);
    if(m_accelFile == -1)
    {   // Could not open the accel file
        Log(LOG_ERRORS, "Could not open %s - %s", GetDataTag("CurrentAccels").c_str(), strerror(errno));
    }
    // Store the machine number
    INT8 machNum = BposReadInt(ReadSubscribeData(GetDataTag("MachineNumber")).c_str());
    UpdateResponseData("MachineNumber", machNum);
    Log(LOG_DEV_DATA, "Set machine number: %d", machNum);
    // Store the zero speed data
    bool zeroSpeed = atob(ReadSubscribeData(GetDataTag("ZeroSpeed")).c_str());
    Log(LOG_DEV_DATA, "Vehicle at zero speed: %s", zeroSpeed ? "True" : "False");
    UpdateResponseDataBit("StatusInfo1", GetBitLocation("ZeroSpeed"), zeroSpeed ? 0x01 : 0x00);
    UpdateResponseDataBit("ZeroSpeedCheck", GetBitLocation("LfZeroSpeed"), zeroSpeed ? 0x01 : 0x00);
    UpdateResponseDataBit("ZeroSpeedCheck", GetBitLocation("RfZeroSpeed"), zeroSpeed ? 0x01 : 0x00);
    // Store the Retaining rolls state
    bool rollsDown = atob(ReadSubscribeData(GetDataTag("RollsDown")).c_str());
    Log(LOG_DEV_DATA, "Rolls down: %s", rollsDown ? "True" : "False");
    UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("LfRetRollDown"), rollsDown ? 0x01 : 0x00);
    UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("RfRetRollDown"), rollsDown ? 0x01 : 0x00);
    UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("LrRetRollDown"), rollsDown ? 0x01 : 0x00);
    UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("RrRetRollDown"), rollsDown ? 0x01 : 0x00);
    UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("RetRollsUp"), rollsDown ? 0x00 : 0x01);
    UpdateResponseDataBit("ExtendedUnitState2", GetBitLocation("ExhaustFlapUp"), rollsDown ? 0x00 : 0x01);
    // Set the initial vehicle present state
    bool vehiclePresent = atob(ReadSubscribeData(GetDataTag("VehiclePresent")).c_str());
    Log(LOG_DEV_DATA, "Vehicle present: %s", vehiclePresent ? "True" : "False");
    UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("FrontVehiclePresent"), vehiclePresent ? 0x01 : 0x00);
    UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("RearVehiclePresent"), vehiclePresent ? 0x01 : 0x00);
    // Return the registration result
    return result;
}

//-----------------------------------------------------------------------------
void DsaServer::Run(volatile bool *terminateFlag)/*=NULL*/
{
    SerialString_t message;
    // Wait for the DSA communication object to be initialized
    while(m_dsaComm == NULL)  BposSleep(1000);
    BposSleep(1000);
    while(BEP_TERMINATE != GetStatus())
    {   // Wait for a message from the MFT
        message.erase();
        INT32 byteCount = m_dsaComm->ReadPort(message, DsaMessageTimeout(), DsaMessageTimeout()/2);
        // Clear the comm port for the next incoming message
        m_dsaComm->ResetServer();
        if(byteCount > 0)
        {   // recieved a message, process it
            DsaSerialString_t dsaMessage;
            dsaMessage.assign(message.begin(), message.end());
            bool requestStat = m_dsaIncoming->ProcessMessage(dsaMessage);
            Log(LOG_DEV_DATA, "Processed request: %s", requestStat ? "Success" : "Error");
            // Send the response
            bool responseStat = m_dsaOutgoing->ProcessResponse();
            Log(LOG_DEV_DATA, "Processed response: %s", responseStat ? "Success" : "Error");
        }
        else
        {
            Log(LOG_ERRORS, "Timeout waiting for DSA message!");
            BposSleep(500);
        }
    }
}

//-----------------------------------------------------------------------------
const string DsaServer::Publish(const XmlNode *node)
{   // Call the base cass to handle the publish stuff
    string result = BepServer::Publish(node);
    // Handle specifics
    INT8 data = 0xFF;
    if(node->getName() == GetDataTag("AllFaults"))
    {   // Determine if the faults are active
        data = (node->getChildren().size() > 0) ? 0x01 : 0x00;
        UpdateResponseDataBit("ErrorStatus2", GetBitLocation("MachineFault"), data);
    }
    else if(node->getName() == GetDataTag("ZeroSpeed"))
    {   // Determine if we are at zero speed
        data = (atob(node->getValue().c_str())) ? 0x01 : 0x00;
        UpdateResponseDataBit("StatusInfo1", GetBitLocation("ZeroSpeed"), data);
        UpdateResponseDataBit("ZeroSpeedCheck", GetBitLocation("LfZeroSpeed"), data);
        UpdateResponseDataBit("ZeroSpeedCheck", GetBitLocation("RfZeroSpeed"), data);
    }
    else if(node->getName() == GetDataTag("PowerSense"))
    {   // Determine if we have lost or regained power sense
        bool powerSense = atob(node->getValue().c_str());
        bool eStop = atob(ReadSubscribeData(GetDataTag("EStop")).c_str());
        data = (powerSense && !eStop) ? 0x00 : 0x01;
        UpdateResponseDataBit("StatusInfo2", GetBitLocation("EStop"), data);
    }
    else if(node->getName() == GetDataTag("EStop"))
    {   // Determine if the E-Stop button has been pressed
        bool eStop = atob(node->getValue().c_str());
        bool powerSense = atob(ReadSubscribeData(GetDataTag("PowerSense")).c_str());
        data = (eStop || !powerSense) ? 0x01 : 0x00;
        UpdateResponseDataBit("StatusInfo2", GetBitLocation("EStop"), data);
    }
    else if(node->getName() == GetDataTag("Calibrate"))
    {   // Determine if a loss verification sequence is in process
        data = atob(node->getValue().c_str()) ? 0x01 : 0x00;
        UpdateResponseDataBit("StatusInfo2", GetBitLocation("CalibrationMode"), data);
    }
    else if(node->getName() == GetDataTag("ReadyForTest"))
    {   // Determine if the system is ready for testing
        data = atob(node->getValue().c_str()) ? 0x01 : 0x00;
        UpdateResponseDataBit("StatusInfo2", GetBitLocation("ReadyForTest"), data);
    }
    else if(node->getName() == GetDataTag("RollsDown"))
    {   // Set rolls up/down state
        INT8 rollsDown  = atob(node->getValue().c_str()) ? 0x01 : 0x00;
        INT8 rollsUp = atob(node->getValue().c_str()) ? 0x00 : 0x01;
        UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("LfRetRollDown"), rollsDown);
        UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("RfRetRollDown"), rollsDown);
        UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("LrRetRollDown"), rollsDown);
        UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("RrRetRollDown"), rollsDown);
        UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("RetRollsUp"), rollsUp);
        UpdateResponseDataBit("ExtendedUnitState2", GetBitLocation("ExhaustFlapUp"), rollsUp);
    }
    else if(node->getName() == GetDataTag("VehiclePresent"))
    {   // Set the vehicle present state
        data = atob(node->getValue().c_str()) ? 0x01 : 0x00;
        UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("FrontVehiclePresent"), data);
        UpdateResponseDataBit("ExtendedUnitState1", GetBitLocation("RearVehiclePresent"), data);
    }
    return result;
}

//-----------------------------------------------------------------------------
const UINT8& DsaServer::RequestedService(const UINT8 *requestedService) /*= NULL*/
{
    if(requestedService != NULL)  m_requestedService = *requestedService;
    return m_requestedService;
}

//-----------------------------------------------------------------------------
void DsaServer::UpdateServiceStatus(const string &status)
{
    m_currentServiceStatus = status;
    // Set the service status in the response
    INT8 servStat = GetServiceStatus(m_currentServiceStatus);
    m_dsaOutgoing->UpdateMessageData("FunctionResult", servStat);
}

//-----------------------------------------------------------------------------
void DsaServer::UpdateResponseData(const string &dataByte, INT8 data)
{
    m_dsaOutgoing->UpdateMessageData(dataByte, data);
}

//-----------------------------------------------------------------------------
void DsaServer::UpdateResponseDataBit(const string &byte, const UINT8 &bit, INT8 value)
{
    m_dsaOutgoing->UpdateDataBit(byte, bit, value);
}

//-----------------------------------------------------------------------------
void DsaServer::UpdateRollerData(void)
{
    static INT32 rollerCount = BposReadInt(GetDataTag("RollerCount").c_str());
    static INT32 dataSize = (sizeof(float) * rollerCount);
    // Update the wheel speed data
    UpdateSpeedData(rollerCount, dataSize);
    // Update the acceleration data
    UpdateAccelData(rollerCount, dataSize);
    // Update the forces
    UpdateForceData(rollerCount, dataSize);
}

//-----------------------------------------------------------------------------
const string& DsaServer::CurrentServiceStatus(void)
{
    return m_currentServiceStatus;
}

//-----------------------------------------------------------------------------
const UINT8 DsaServer::GetBitLocation(string bitDescription)
{
    UINT8 location = 0x00;
    // Attempt to find the bit definition
    XmlNodeMapItr itr = m_bitDefinitions.find(bitDescription);
    if(itr != m_bitDefinitions.end())
    {   // Found the bit, get the location value
        location = BposReadInt(itr->second->getValue().c_str());
    }
    else
    {   // Requested bit not defined
        Log(LOG_ERRORS, "Bit %s not defined!  Using bit 0", bitDescription.c_str());
        location = 0x00;
    }
    return location;
}

//-----------------------------------------------------------------------------
const bool& DsaServer::TestInProgress(const bool *testInProgress) /*= NULL*/
{
    if(testInProgress != NULL)  m_testInProgress = *testInProgress;
    return m_testInProgress;
}

//-----------------------------------------------------------------------------
bool DsaServer::SendResponse(DsaSerialString_t &response)
{
    return(BEP_STATUS_SUCCESS == m_dsaComm->Send(response));
}

//-----------------------------------------------------------------------------
INT8 DsaServer::GetServiceStatus(const string &status)
{   // Look up the status value
    INT8 retStatus = 0;
    XmlNodeMapItr iter = m_serviceStatusList.find(status);
    if(iter != m_serviceStatusList.end())
    {   // Get the status value
        retStatus = BposReadInt(iter->second->getValue().c_str());
    }
    else
    {   // Unsupported status, return -9
        Log(LOG_ERRORS, "Unsupported service status: %s", status.c_str());
        retStatus = -9;
    }
    Log(LOG_FN_ENTRY, "GetServiceStatus(%s) returning %d", status.c_str(), retStatus);
    return retStatus;
}

//-----------------------------------------------------------------------------
const long& DsaServer::DsaMessageTimeout(const long *timeout) /*= NULL*/
{
    if(timeout != NULL)  m_messageTimeout = *timeout;
    return m_messageTimeout;
}

//-----------------------------------------------------------------------------
void DsaServer::StoreBitLocations(const XmlNode *bitLocations)
{
    try
    {
        m_bitDefinitions.DeepCopy(bitLocations->getChildren());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "DsaIn::StoreBitLocations - bit definitions not supplied! - %s", excpt.GetReason());
        m_bitDefinitions.clear(true);
    }
}

//-----------------------------------------------------------------------------
INT32 DsaServer::ConvertWheelSpeed(const float &speed)
{
    return(INT32)(speed * 100.0 * MphToKph);
}

//-----------------------------------------------------------------------------
void DsaServer::UpdateSpeedData(const INT32 &rollerCount, const INT32 &dataSize)
{
    if(m_speedFile != -1)
    {   // Get the current wheel speeds
        float wheelSpeeds[rollerCount];
        lseek(m_speedFile, 0, SEEK_SET);
        UINT32 bytes = read(m_speedFile, (void *)wheelSpeeds, dataSize);
        if(bytes == (UINT32)dataSize)
        {   // Stuff the speeds into the response message
            UpdateResponseData("LfSpeedHi", ((ConvertWheelSpeed(wheelSpeeds[0]) & 0xFF00) >> 8));
            UpdateResponseData("LfSpeedLo", (ConvertWheelSpeed(wheelSpeeds[0]) & 0x00FF));
            UpdateResponseData("RfSpeedHi", ((ConvertWheelSpeed(wheelSpeeds[1]) & 0xFF00) >> 8));
            UpdateResponseData("RfSpeedLo", (ConvertWheelSpeed(wheelSpeeds[1]) & 0x00FF));
            UpdateResponseData("LrSpeedHi", ((ConvertWheelSpeed(wheelSpeeds[2]) & 0xFF00) >> 8));
            UpdateResponseData("LrSpeedLo", (ConvertWheelSpeed(wheelSpeeds[2]) & 0x00FF));
            UpdateResponseData("RrSpeedHi", ((ConvertWheelSpeed(wheelSpeeds[3]) & 0xFF00) >> 8));
            UpdateResponseData("RrSpeedLo", (ConvertWheelSpeed(wheelSpeeds[3]) & 0x00FF));
        }
        else
        {
            Log(LOG_ERRORS, "Could not read all roller speeds!");
        }
    }
    else
    {
        Log(LOG_ERRORS, "Speed file not open, not updating wheel speed data!");
    }
}

//-----------------------------------------------------------------------------
void DsaServer::UpdateAccelData(const INT32 &rollerCount, const INT32 &dataSize)
{
    if(m_accelFile != -1)
    {   // Get the current accelerations
        float accels[rollerCount];
        lseek(m_accelFile, 0, SEEK_SET);
        UINT32 bytes = read(m_accelFile, (void *)accels, dataSize);
        if(bytes == (UINT32)dataSize)
        {   // Stuff the accelerations into the response message
            UpdateResponseData("LfAccelHi", (((INT32)(accels[0] * 10) & 0xFF00) >> 8));
            UpdateResponseData("LfAccelLo",  ((INT32)(accels[0] * 10) & 0x00FF));
            UpdateResponseData("RfAccelHi", (((INT32)(accels[1] * 10) & 0xFF00) >> 8));
            UpdateResponseData("RfAccelLo",  ((INT32)(accels[1] * 10) & 0x00FF));
            UpdateResponseData("LrAccelHi", (((INT32)(accels[2] * 10) & 0xFF00) >> 8));
            UpdateResponseData("LrAccelLo",  ((INT32)(accels[2] * 10) & 0x00FF));
            UpdateResponseData("RrAccelHi", (((INT32)(accels[3] * 10) & 0xFF00) >> 8));
            UpdateResponseData("RrAccelLo",  ((INT32)(accels[3] * 10) & 0x00FF));
        }
        else
        {
            Log(LOG_ERRORS, "Could not read accelerations!");
        }
    }
    else
    {
        Log(LOG_ERRORS, "Acceleration file not open, not updating wheel acceleration data!");
    }
}

//-----------------------------------------------------------------------------
void DsaServer::UpdateForceData(const INT32 &rollerCount, const INT32 &dataSize)
{
    if(m_forceFile != -1)
    {   // Get the current accelerations
        float forces[rollerCount];
        lseek(m_forceFile, 0, SEEK_SET);
        UINT32 bytes = read(m_forceFile, (void *)forces, dataSize);
        if(bytes == (UINT32)dataSize)
        {   // Stuff the accelerations into the response message
            UpdateResponseData("LfForceHi", (((INT32)(forces[0] * PoundToNewton) & 0xFF00) >> 8));
            UpdateResponseData("LfForceLo",  ((INT32)(forces[0] * PoundToNewton) & 0x00FF));
            UpdateResponseData("RfForceHi", (((INT32)(forces[1] * PoundToNewton) & 0xFF00) >> 8));
            UpdateResponseData("RfForceLo",  ((INT32)(forces[1] * PoundToNewton) & 0x00FF));
            UpdateResponseData("LrForceHi", (((INT32)(forces[2] * PoundToNewton) & 0xFF00) >> 8));
            UpdateResponseData("LrForceLo",  ((INT32)(forces[2] * PoundToNewton) & 0x00FF));
            UpdateResponseData("RrForceHi", (((INT32)(forces[3] * PoundToNewton) & 0xFF00) >> 8));
            UpdateResponseData("RrForceLo",  ((INT32)(forces[3] * PoundToNewton) & 0x00FF));
        }
        else
        {
            Log(LOG_ERRORS, "Could not read forces!");
        }
    }
    else
    {
        Log(LOG_ERRORS, "Force file not open, not updating wheel force data!");
    }
}
