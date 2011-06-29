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
#include "DsaIn.h"
#include <time.h>

//-----------------------------------------------------------------------------
DsaIn::DsaIn(const XmlNode *config, DsaServer *server) : 
    m_server(server), m_wheelbaseAdjusting (false), m_machineComponent(NULL)
{   // Store the config data
    StoreMessageData(config);
}

//-----------------------------------------------------------------------------
DsaIn::~DsaIn()
{   // Nothing special to be done
}

//-----------------------------------------------------------------------------
bool DsaIn::ProcessMessage(DsaSerialString_t &message)
{
    bool messageValid = false;
    BEP_STATUS_TYPE serviceStatus = BEP_STATUS_ERROR;
    // Verify the checksum is valid
    if(ValidateChecksum(message))
    {   // Ensure the message ID is correct
        if(message[MessageIdByte()] == ExpectedId())
        {   
            Server()->UpdateResponseDataBit("ErrorStatus1", Server()->GetBitLocation("MessageID"), 0x00);
            // Determine if the MFT fault bit is set
            if(message[StatusByte()] & atoh(m_parameters.getNode("MftFaultBit")->getValue().c_str()))
            {   // Fault bit is set, echo to the MFT
                Server()->UpdateResponseDataBit("StatusInfo2", Server()->GetBitLocation("MftStatus"), 0x01);
            }
            else
            {
                Server()->UpdateResponseDataBit("StatusInfo2", Server()->GetBitLocation("MftStatus"), 0x00);
            }
            // Echo back the motor mode
            Server()->UpdateResponseData("MotorModeAck", message[MotorModeByte()]);
            // Echo back the continuous MFT number
            Server()->UpdateResponseData("ContinuousMftNumber", message[ContinuousMftNumberByte()]);
            // Ensure the requested service is valid
            char buff[16];
            string serviceRequestTag(CreateMessage(buff, sizeof(buff), "Service%02d", message[ServiceByte()]));
            XmlNodeMapItr iter = m_validServiceRequests.find(serviceRequestTag);
            if(iter != m_validServiceRequests.end())
            {   // Clear the service request status bit
                Server()->UpdateResponseDataBit("ErrorStatus1", Server()->GetBitLocation("ServiceRequest"), 0x00);
                // Update the toggle byte
                INT8 toggleData = message[ToggleByte()]+1;
                Server()->Log(LOG_DEV_DATA, "DsaIn: Updating toggle byte %02X => %02X", message[ToggleByte()], toggleData);
                Server()->UpdateResponseData("ToggleByte", toggleData);
                // Store the current service request and verify the request is allowable
                if(RequestedServiceAllowed(message[ServiceByte()]))
                {   // Store the requested service
                    UINT8 srvc = (UINT8)message[ServiceByte()];
                    Server()->RequestedService(&srvc);
                    // Ensure service request error bit is cleared
                    Server()->UpdateResponseDataBit("ErrorStatus2", Server()->GetBitLocation("ServiceRequestError"), 0x00);
                    messageValid = true;
                    string response;
                    INT32 wheelbase;
                    WheelSpeeds rollData;
                    // Process the messsage - Determine the requested service
                    switch(message[ServiceByte()])
                    {
                    case ServiceAbort:
                        // Must clear the service request error with this request
                        Server()->UpdateResponseDataBit("ErrorStatus2", Server()->GetBitLocation("ServiceRequestError"), 0x00);
                        if(Server()->CurrentServiceStatus() == ServiceInProgress)
                        {   // Need to abort the current service
                            m_ndb.Write(ABORT_DATA_TAG, "1", response, true);
                            Server()->UpdateServiceStatus(NoService);
                        }
                        else if((Server()->CurrentServiceStatus() == ServiceCompletePass) ||
                                (Server()->CurrentServiceStatus() == ServiceCompleteFail))
                        {   // Acknowledge no service in progress
                            Server()->UpdateServiceStatus(NoService);
                        }
                        else
                        {   // Acknowledge no service in progress
                            Server()->UpdateServiceStatus(NoService);
                        }
                        break;

                    case ServiceStartTest:
                        // Inform server the service is in progress
                        Server()->UpdateServiceStatus(ServiceInProgress);
                        // Update test in progress status
                        Server()->TestInProgress(&TestInProgress);
                        // Set motors to free-roll state (torque 0)
                        serviceStatus = MachineControl("DisengageMachine");
                        // Inform the server the service is completed
                        Server()->UpdateServiceStatus((BEP_STATUS_SUCCESS == serviceStatus) ? 
                                                      ServiceCompletePass : ServiceCompleteFail);
                        break;

                    case ServiceEndTest:
                        // Inform the server the service is in progress
                        Server()->UpdateServiceStatus(ServiceInProgress);
                        // Update test in progress status
                        Server()->TestInProgress(&TestNotInProgress);
                        // Inform the server the service is completed
                        Server()->UpdateServiceStatus(ServiceCompletePass);
                        break;

                    case ServiceControlDev:
                        // Inform the server the service is in progress
                        Server()->UpdateServiceStatus(ServiceInProgress);
                        // Set the requested machine state
                        rollData = ExtractRequestedRollerData(message);
                        serviceStatus = ProcessControlRequests(message[MachineStatusByte()],
                                                               message[MotorModeByte()], rollData);
                        // Inform server the service is complete
                        Server()->UpdateServiceStatus((BEP_STATUS_SUCCESS == serviceStatus) ? 
                                                      ServiceCompletePass : ServiceCompleteFail);
                        break;

                    case ServiceWheelbase:
                        // Get the wheelbase position from the message
                        wheelbase = (message[ParameterByte()] << 8) | message[ParameterByte()+1];
                        // Inform the server the service is starting
                        Server()->UpdateServiceStatus(ServiceInProgress);
                        // Process the service
                        serviceStatus = AdjustWheelbase(wheelbase);
                        break;

                    case ServiceDefaultState:
                        // Inform the server the service is starting
                        Server()->UpdateServiceStatus(ServiceInProgress);
                        // Go to the home position
                        serviceStatus = ConvertIntToBepStatus(m_ndb.Write(RAISE_ROLLS, "0", response, true));
                        // Inform server the service is complete
                        Server()->UpdateServiceStatus((BEP_STATUS_SUCCESS == serviceStatus) ? 
                                                      ServiceCompletePass : ServiceCompleteFail);
                        break;

                    case ServiceDataLogging:
                        // Inform the server the service is starting
                        Server()->UpdateServiceStatus(ServiceInProgress);
                        // Command the machine component to log data
                        serviceStatus = MachineControl("LogRollerData");
                        break;

                    case ServiceHeightResults:
                        // Inform the server the service is starting
                        Server()->UpdateServiceStatus(ServiceInProgress);
                        // Command the machine component to transfer the height measurements
                        serviceStatus = MachineControl("HeightMeasurement");
                        break;

                    case ServiceStorePaNumber:
                        // Inform the server the service is starting
                        Server()->UpdateServiceStatus(ServiceInProgress);
                        // Store the PA number
                        serviceStatus = StorePaNumber(message);
                        // Inform server the service is complete
                        Server()->UpdateServiceStatus((BEP_STATUS_SUCCESS == serviceStatus) ? 
                                                      ServiceCompletePass : ServiceCompleteFail);
                        break;

                    case ServiceStartPowerTest:
                        break;

                    case ServiceStopPowerTest:
                        break;

                    default:
                        Server()->Log(LOG_ERRORS, "DsaIn::ProcessMessage - Unsupported service request: %02X (%d)", 
                                      message[ServiceByte()], message[ServiceByte()]);
                        messageValid = false;
                        break;
                    }
                }
                else
                {   // Requested service is not allowed at this time
                    Server()->UpdateResponseDataBit("ErrorStatus2", Server()->GetBitLocation("ServiceRequestError"), 0x01);
                    Server()->Log(LOG_ERRORS, "DsaIn::ProcessMessage - Requested Service not allowed at this time: %02X (%d)",
                                  message[ServiceByte()], message[ServiceByte()]);
                }
            }
            else
            {   // Invlaid service request
                Server()->UpdateResponseDataBit("ErrorStatus1", Server()->GetBitLocation("ServiceRequest"), 0x01);
                Server()->Log(LOG_ERRORS, "DsaIn::ProcessMessage - Unsupported service request: %02X (%d)", 
                              message[ServiceByte()], message[ServiceByte()]);
            }
        }
        else
        {   // Invalid message ID, do not process the message.
            Server()->UpdateResponseDataBit("ErrorStatus1", Server()->GetBitLocation("MessageID"), 0x01);
            Server()->Log(LOG_ERRORS, "DsaIn::ProcessMessage - Message ID (%02X) is incorrect, expected %02X",
                          message[MessageIdByte()], ExpectedId());
        }
    }
    else
    {   // Checksum is invalid, not processing message
        Server()->Log(LOG_ERRORS, "DsaIn::ProcessMessage - Message checksum is invalid, not processing message.");
    }
    // Return the result
    return messageValid;
}

//-----------------------------------------------------------------------------
void DsaIn::StoreMessageData(const XmlNode *config)
{
    const XmlNode *messageData = config->getChild("Setup/IncomingMessageData");
    // Store the expected Message ID
    UINT8 expectedID = 0x00;
    try
    {
        expectedID = atoh(messageData->getChild("ExpectedMessageId")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Expected Message Id was not listed, defaulting to 0x02 - %s", excpt.GetReason());
        expectedID = 0x02;
    }
    ExpectedId(&expectedID);
    // Store the number of PA bytes
    UINT8 numPaBytes = 0;
    try
    {
        numPaBytes = BposReadInt(messageData->getChild("NumberOfPaBytes")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Number of PA bytes not listed, default to 6 - %s", excpt.GetReason());
        numPaBytes = 28;
    }
    NumberOfPaBytes(&numPaBytes);
    // Store the byte location data
    StoreByteLocations(messageData->getChild("ByteLocations"));
    // Store the valid motor modes
    try
    {
        m_validMotorModes.DeepCopy(messageData->getChild("ValidMotorModes")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Valid motor Modes not listed! - %s", excpt.GetReason());
        m_validMotorModes.clear(true);
    }
    // Store the valid service requests
    try
    {
        m_validServiceRequests.DeepCopy(messageData->getChild("ValidServiceRequests")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Valid Service Requests not listed! - %s", excpt.GetReason());
        m_validServiceRequests.clear(true);
    }
    // Store the parameters
    try
    {
        m_parameters.DeepCopy(config->getChild("Setup/Parameters")->getChildren());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Parameters not listed! - %s", excpt.GetReason());
        m_parameters.clear(true);
    }
    // Create a new Machine component communication object
    m_machineComponent = new IBepCommunication();
    m_machineComponent->Initialize(m_parameters.getNode("MachineComponent")->getValue(), "Client");
}

//-----------------------------------------------------------------------------
void DsaIn::StoreByteLocations(const XmlNode *byteLocations)
{
    // Store the message ID byte
    UINT8 msgIdByte = 0;
    try
    {
        msgIdByte = BposReadInt(byteLocations->getChild("MessageIdByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Message ID byte not listed, default to 0 - %s", excpt.GetReason());
        msgIdByte = 0;
    }
    MessageIdByte(&msgIdByte);
    // Store the requested service byte location
    UINT8 srvcByte = 0;
    try
    {
        srvcByte = BposReadInt(byteLocations->getChild("RequestedServiceByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Requested Servive byte not listed, default to 1 - %s", excpt.GetReason());
        srvcByte = 1;
    }
    ServiceByte(&srvcByte);
    // Store the status information byte
    UINT8 statusInfoByte = 0;
    try
    {
        statusInfoByte = BposReadInt(byteLocations->getChild("StatusInfoByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Status Information byte not listed, default to 2 - %s", excpt.GetReason());
        statusInfoByte = 2;
    }
    StatusByte(&statusInfoByte);
    // Store the Toggle byte location
    UINT8 toggleByte = 0;
    try
    {
        toggleByte = BposReadInt(byteLocations->getChild("ToggleByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Toggle byte not listed, default to 3 - %s", excpt.GetReason());
        toggleByte = 3;
    }
    ToggleByte(&toggleByte);
    // Store the desired machine status byte
    UINT8 machStatusByte = 0;
    try
    {
        machStatusByte = BposReadInt(byteLocations->getChild("RquestedMachineStatusByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Requested Machine Status byte not listed, default to 4 - %s", excpt.GetReason());
        machStatusByte = 4;
    }
    MachineStatusByte(&machStatusByte);
    // Store the requested motor mode byte
    UINT8 motorModeByte = 0;
    try
    {
        motorModeByte = BposReadInt(byteLocations->getChild("RequestedMotorModeByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Requested Motor Mode byte not listed, default to 5 - %s", excpt.GetReason());
        motorModeByte = 5;
    }
    MotorModeByte(&motorModeByte);
    // Store the Left Front Target Speed byte
    UINT8 lfSpeedByte = 0;
    try
    {
        lfSpeedByte = BposReadInt(byteLocations->getChild("LfTargetSpeedByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Left Front Speed byte not listed, default to 6 - %s", excpt.GetReason());
        lfSpeedByte = 6;
    }
    LfTargetSpeedByte(&lfSpeedByte);
    // Store the Left Front Target Revolutions byte
    UINT8 lfRevsByte = 0;
    try
    {
        lfRevsByte = BposReadInt(byteLocations->getChild("LfTargetRevsByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Left Front Target Revolutions byte not listed, default to 8 - %s", excpt.GetReason());
        lfRevsByte = 8;
    }
    LfTargetRevsByte(&lfRevsByte);
    // Store the Right Front Target Speed byte
    UINT8 rfSpeedByte = 0;
    try
    {
        rfSpeedByte = BposReadInt(byteLocations->getChild("RfTargetSpeedByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Right Front Speed byte not listed, default to 6 - %s", excpt.GetReason());
        rfSpeedByte = 28;
    }
    RfTargetSpeedByte(&rfSpeedByte);
    // Store the Right Front Target Revolutions byte
    UINT8 rfRevsByte = 0;
    try
    {
        rfRevsByte = BposReadInt(byteLocations->getChild("RfTargetRevsByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Right Front Target Revolutions byte not listed, default to 8 - %s", excpt.GetReason());
        rfRevsByte = 30;
    }
    RfTargetRevsByte(&rfRevsByte);
    // Store the Left Rear Target Speed byte
    UINT8 lrSpeedByte = 0;
    try
    {
        lrSpeedByte = BposReadInt(byteLocations->getChild("LrTargetSpeedByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Left Rear Speed byte not listed, default to 6 - %s", excpt.GetReason());
        lrSpeedByte = 32;
    }
    LrTargetSpeedByte(&lrSpeedByte);
    // Store the Left Rear Target Revolutions byte
    UINT8 lrRevsByte = 0;
    try
    {
        lrRevsByte = BposReadInt(byteLocations->getChild("LrTargetRevsByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Left Rear Target Revolutions byte not listed, default to 8 - %s", excpt.GetReason());
        lrRevsByte = 34;
    }
    LrTargetRevsByte(&lrRevsByte);


    // Store the Right Rear Target Speed byte
    UINT8 rrSpeedByte = 0;
    try
    {
        rrSpeedByte = BposReadInt(byteLocations->getChild("RrTargetSpeedByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Right Rear Speed byte not listed, default to 6 - %s", excpt.GetReason());
        rrSpeedByte = 36;
    }
    RrTargetSpeedByte(&rrSpeedByte);
    // Store the Right Rear Target Revolutions byte
    UINT8 rrRevsByte = 0;
    try
    {
        rrRevsByte = BposReadInt(byteLocations->getChild("RrTargetRevsByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Right Rear Target Revolutions byte not listed, default to 8 - %s", excpt.GetReason());
        rrRevsByte = 38;
    }
    RrTargetRevsByte(&rrRevsByte);
    // Store the Parameter 1 byte
    UINT8 paramByte = 0;
    try
    {
        paramByte = BposReadInt(byteLocations->getChild("Parameter1Byte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Parameter 1 byte not listed, default to 10 - %s", excpt.GetReason());
        paramByte = 10;
    }
    ParameterByte(&paramByte);
    // Store the MFT number byte
    UINT8 mftNumByte = 0;
    try
    {
        mftNumByte = BposReadInt(byteLocations->getChild("MftNumberByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: MFT Number byte not listed, default to 19 - %s", excpt.GetReason());
        mftNumByte = 19;
    }
    MftNumberByte(&mftNumByte);
    // Store the PA Number starting byte
    UINT8 paNumByte = 0;
    try
    {
        paNumByte = BposReadInt(byteLocations->getChild("PaNumberByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: PA Number byte not listed, default to 28 - %s", excpt.GetReason());
        paNumByte = 28;
    }
    PaNumberByte(&paNumByte);
    // Store the continuous MFT number byte
    UINT8 contMftByte = 0;
    try
    {
        contMftByte = BposReadInt(byteLocations->getChild("ContinuousMftNumberByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Continuous MFT Number byte not listed, default to 41 - %s", excpt.GetReason());
        contMftByte = 41;
    }
    ContinuousMftNumberByte(&contMftByte);
    // Store the Checksum byte
    UINT8 checksumByte = 0;
    try
    {
        checksumByte = BposReadInt(byteLocations->getChild("ChecksumByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Checksum byte not listed, default to 42 - %s", excpt.GetReason());
        checksumByte = 42;
    }
    ChecksumByte(&checksumByte);
    // Store the Diagnostic data byte
    UINT8 diagByte = 0;
    try
    {
        diagByte = BposReadInt(byteLocations->getChild("DiagnosticDataByte")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Server()->Log(LOG_ERRORS, "DsaIn: Diagnostic byte not listed, default to 43 - %s", excpt.GetReason());
        diagByte = 43;
    }
    DiagnosticByte(&diagByte);
}

//-----------------------------------------------------------------------------
bool DsaIn::ValidateChecksum(const DsaSerialString_t &message)
{
    bool checksumValid = false;
    // First ensure the message is long enough
    if(message.length() > ChecksumByte())
    {   // Calculate the expected checksum
        UINT8 checkSum = 0;
        for(UINT8 index = 0; index < ChecksumByte(); index++)  checkSum ^= (UINT8)message[index];
        // Verify the checksum is valid
        checksumValid = (checkSum == (UINT8)message[ChecksumByte()]);
        Server()->Log(LOG_DEV_DATA, "Expected Checksum: %02X, Calculated: %02X", (UINT8)message[ChecksumByte()], checkSum);
    }
    else
    {   // Short message - not valid
        Server()->Log(LOG_ERRORS, "DsaIn:ValidateCheckSum() - Message is shorter than checksum byte location -- "
                      "msg len: %d, checksum byte: %d", message.length(), ChecksumByte());
    }
    // Return the status
    return checksumValid;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE DsaIn::AdjustWheelbase(INT32 &wheelbase)
{   // Write the wheelbase position to the PLC
    string response, message;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    static struct timespec startTime;
    INT32 timeRemaining = BposReadInt(m_parameters.getNode("WheelbasePositionTimeout")->getValue().c_str());
    if(!m_wheelbaseAdjusting)
    {
        Server()->Log(LOG_FN_ENTRY, "DsaIn::AdjustWheelbase - Adjusting wheelbase to %d mm (%d inches x10)", 
                      wheelbase, (wheelbase / 254));
        // Convert wheelbase mm to inches x10
        char buff[8];
        string strWheelbase = CreateMessage(buff, sizeof(buff), "%04d", wheelbase / 254);
        status = ConvertIntToBepStatus(m_ndb.Write(WHEELBASE_INCHESX10, strWheelbase, response, true));
        // Command the wheelbase to move and wait for it to complete
        if(BEP_STATUS_SUCCESS == status)
        {
            status = ConvertIntToBepStatus(m_ndb.Write(WHEELBASE_MOVE_TAG, "1", response, true));
            if(BEP_STATUS_SUCCESS == status)
            {
                m_wheelbaseAdjusting = true;
                if(clock_gettime(CLOCK_REALTIME, &startTime) == -1)
                {
                    Server()->Log(LOG_ERRORS, "DsaIn:Wheelbase - Could not get start time!");
                    Server()->UpdateServiceStatus(ServiceCompleteFail);
                }
            }
            else
            {
                Server()->Log(LOG_ERRORS, "DsaIn:Wheelbase - Could not command wheelbase to move into position: %s",
                              ConvertStatusToResponse(status).c_str());
                Server()->UpdateServiceStatus(ServiceCompleteFail);
            }
        }
        else
        {
            Server()->Log(LOG_ERRORS, "DsaIn:Wheelbase - Could not write wheelbase position: %s",
                          ConvertStatusToResponse(status).c_str());
            Server()->UpdateServiceStatus(ServiceCompleteFail);
        }
    }
    else
    {
        bool inPosition = false;
        // Check if the wheelbase is in position
        if((status = ConvertIntToBepStatus(m_ndb.Read(WHEELBASE_IN_POSITION_TAG, response, true))) == BEP_STATUS_SUCCESS)
        {   // Check if the whelbase is in position
            if((status = ConvertIntToBepStatus(m_ndb.GetByTag(WHEELBASE_IN_POSITION_TAG, response, message))) == BEP_STATUS_SUCCESS)
            {
                inPosition = atob(response.c_str());
                if(inPosition)
                {   // Clear the wheelbase move bit
                    status = ConvertIntToBepStatus(m_ndb.Write(WHEELBASE_MOVE_TAG, "0", response, true));
                    Server()->UpdateServiceStatus(ServiceCompletePass);
                    Server()->Log(LOG_DEV_DATA, "Wheelbase is in position");
                }
                else
                {   // Ensure time is remaining
                    struct timespec currentTime;
                    clock_gettime(CLOCK_REALTIME, &currentTime);
                    double elapsedTime = ( currentTime.tv_sec - startTime.tv_sec ) * 1000
                                         + ( currentTime.tv_nsec - startTime.tv_nsec ) / 1000000;
                    Server()->UpdateServiceStatus(timeRemaining > elapsedTime ? ServiceInProgress : ServiceCompleteFail);
                    Server()->Log(LOG_DEV_DATA, "Wheelbase adjustment time remaining: %s (%d > %.2f)",
                                  timeRemaining > elapsedTime ? "True" : "False", timeRemaining, elapsedTime);
                }
            }
            else
            {
                Server()->Log(LOG_ERRORS, "DsaIn::AdjustWheelbase - Could not determine in position status - %s",
                              ConvertStatusToResponse(status).c_str());
            }
        }
        else
        {
            Server()->Log(LOG_ERRORS, "DsaIn::AdjustWheelbase - Could not read In Position status - %s",
                          ConvertStatusToResponse(status).c_str());
        }
    }
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE DsaIn::MachineControl(const string &control)
{
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    string response;
    // Command the Machine component to disengage the machine
    status = ConvertIntToBepStatus(m_machineComponent->Command(m_parameters.getNode(control)->getValue(), "", response, true));
    // Return the status
    Server()->Log(LOG_FN_ENTRY, "DsaIn::EnableMotorBoost - Machine control (%s) complete: %s", 
                  control.c_str(), ConvertStatusToResponse(status).c_str());
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE DsaIn::ProcessControlRequests(INT8 &machineState, INT8 &motorMode, WheelSpeeds &speeds)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string response;
    // Determine if the retaining rolls should be raised/Loweered
    string retainerState = (machineState & atoh(m_parameters.getNode("RaiseRetainersMask")->getValue().c_str())) ? "1" : "0";
    Server()->Log(LOG_DEV_DATA, "DSaIn::ProcessControlRequest - Retainer state: %s", 
                  retainerState == "1" ? "Up" : "Down");
    status = ConvertIntToBepStatus(m_ndb.Write(RAISE_ROLLS, retainerState, response, true));
    if(BEP_STATUS_SUCCESS == status)
    {   // Set the requested motor mode
        Server()->Log(LOG_DEV_DATA, "DsaIn::ProcessControlRequest - Setting motor mode: %d", motorMode);
        string driveAxle;
        Server()->UpdateResponseDataBit("ErrorStatus2", Server()->GetBitLocation("MotorMode"), 0x00);
        switch(motorMode)
        {
        case MotorModeFreeRoll:
            // Disengage the machine
            status = MachineControl("DisengageMachine");
            break;

        case MotorModeFwd:
        case MotorModeRwd:
        case MotorModeAwd:
            driveAxle = (motorMode == MotorModeRwd) ? "DriveAxleRwd" : "DriveAxleFwd";
            // Set the drive axle and place the motors into boost mode
            status = ConvertIntToBepStatus(m_ndb.Write(Server()->GetDataTag("DriveAxle"), 
                                                       Server()->GetDataTag(driveAxle), 
                                                       response, true));
            if(BEP_STATUS_SUCCESS == status)
            {   // Enable motor boosting
                status = MachineControl("EnableBoost");
            }
            else
            {   // Could not set drive axle
                Server()->Log(LOG_ERRORS, "DsaIn::ProcessControlRequest - Could not set drive axle to %s: %s",
                              Server()->GetDataTag("DriveAxleFwd").c_str(), ConvertStatusToResponse(status).c_str());
            }
            break;

        case MotorModeStaticBrake:
            {
                // Place the motor controller into torque mode and write the torque values to the motor controller
                status = MachineControl("DisengageMachine");  // Place the motors in Torque mode
                if(BEP_STATUS_SUCCESS == status)
                {   // Set the torque value for each wheel
                    char buff[16];
                    BEP_STATUS_TYPE lfStatus = ConvertIntToBepStatus(m_ndb.Command(Server()->GetDataTag("CommandTorque"), 
                                                             itoa(speeds.lfRevs, buff, 10), response, false));
                    BEP_STATUS_TYPE rfStatus = ConvertIntToBepStatus(m_ndb.Command(Server()->GetDataTag("CommandTorque"), 
                                                             itoa(speeds.rfRevs, buff, 10), response, false));
                    BEP_STATUS_TYPE lrStatus = ConvertIntToBepStatus(m_ndb.Command(Server()->GetDataTag("CommandTorque"), 
                                                             itoa(speeds.lrRevs, buff, 10), response, false));
                    BEP_STATUS_TYPE rrStatus = ConvertIntToBepStatus(m_ndb.Command(Server()->GetDataTag("CommandTorque"), 
                                                             itoa(speeds.rrRevs, buff, 10), response, true));
                    Server()->Log(LOG_DEV_DATA, "DsaIn::ProcessControlRequest - Set torque for each wheel: "
                                  "LF - %s, RF - %s, LR - %s, RR - %s",
                                  ConvertStatusToResponse(lfStatus).c_str(), ConvertStatusToResponse(rfStatus).c_str(),
                                  ConvertStatusToResponse(lrStatus).c_str(), ConvertStatusToResponse(rrStatus).c_str());
                    // Determine the overall status
                    if((BEP_STATUS_SUCCESS == lfStatus) && (BEP_STATUS_SUCCESS == rfStatus) &&
                       (BEP_STATUS_SUCCESS == lrStatus) && (BEP_STATUS_SUCCESS == rrStatus))
                    {   // All torque commands were successful
                        status = BEP_STATUS_SUCCESS;
                    }
                    else
                    {   // Error setting torque
                        status = BEP_STATUS_FAILURE;
                    }
                }
                else
                {   // Could not place the motors into torque mode
                    Server()->Log(LOG_ERRORS, "DsaIn::ProcessControlRequest - Could not place the motors into Torque mode: %s",
                                  ConvertStatusToResponse(status).c_str());
                }
            }
            break;

        case MotorModeBrake1:
        case MotorModeBrake2:
            // Enable motor boost
            status = MachineControl("EnableBoost");
            break;

        case MotorModeAllStop:
            // Command motors to zero speed
            status = MachineControl("AllStop");
            break;

        default:
            // Unsupported Motor Mode
            Server()->UpdateResponseDataBit("ErrorStatus2", Server()->GetBitLocation("MotorMode"), 0x01);
            Server()->Log(LOG_ERRORS, "Unsupported motor mode: %d", motorMode);
        }
    }
    else
    {
        Server()->Log(LOG_ERRORS, "DSaIn::ProcessControlRequest - Could not change retainer state: %s", 
                      ConvertStatusToResponse(status).c_str());
    }
    // Update Roller data
    
    // Return the status
    return status;
}

//-----------------------------------------------------------------------------
BEP_STATUS_TYPE DsaIn::StorePaNumber(const DsaSerialString_t &message)
{   // Get the PA Number from the message string
    string paNumber((char *)&message[PaNumberByte()], NumberOfPaBytes());
    Server()->Log(LOG_DEV_DATA, "DsaIn::StorePaNumber - PA Number: %s", paNumber.c_str());
    // Write the PA number to the System
    string response;
    return ConvertIntToBepStatus(m_ndb.Write(VIN_DATA_TAG, paNumber, response, true));
}

//-----------------------------------------------------------------------------
DsaServer* DsaIn::Server(DsaServer *server) /*= NULL*/
{
    if(server != NULL)   m_server = server;
    return m_server;
}

//-----------------------------------------------------------------------------
WheelSpeeds DsaIn::ExtractRequestedRollerData(const DsaSerialString_t &rollerData)
{
    WheelSpeeds data;
    // Get the target revs
    data.lfRevs = (rollerData[LfTargetRevsByte()] << 8) | rollerData[LfTargetRevsByte()+1];
    data.rfRevs = (rollerData[RfTargetRevsByte()] << 8) | rollerData[RfTargetRevsByte()+1];
    data.lrRevs = (rollerData[LrTargetRevsByte()] << 8) | rollerData[LrTargetRevsByte()+1];
    data.rrRevs = (rollerData[RrTargetRevsByte()] << 8) | rollerData[RrTargetRevsByte()+1];
    // Get the target wheel speeds
    data.lfSpeed = (rollerData[LfTargetSpeedByte()] << 8) | rollerData[LfTargetSpeedByte()+1];
    data.rfSpeed = (rollerData[RfTargetSpeedByte()] << 8) | rollerData[RfTargetSpeedByte()+1];
    data.lrSpeed = (rollerData[LrTargetSpeedByte()] << 8) | rollerData[LrTargetSpeedByte()+1];
    data.rrSpeed = (rollerData[RrTargetSpeedByte()] << 8) | rollerData[RrTargetSpeedByte()+1];
    // Return the wheel data
    return data;
}

//-----------------------------------------------------------------------------
bool DsaIn::RequestedServiceAllowed(const UINT8 &requestedService)
{
    bool serviceAllowed = false;
    // Requested service allowed under these conditions:
    //           - Abort request (ServiceAbort)
    //           - Service in progress, must be same requested service as already running
    //           - No service in progress
    serviceAllowed = ((ServiceAbort == requestedService) || 
                      (Server()->RequestedService() == requestedService) ||
                      (Server()->RequestedService() == ServiceAbort));
    Server()->Log(LOG_FN_ENTRY, "DsaIn::RequestedServiceAllowed (service: %02X) - %s", 
                  requestedService, serviceAllowed ? "True" : "False");
    return serviceAllowed;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::ExpectedId(const UINT8 *id) /*= NULL*/
{
    if(id != NULL)   m_expectedId = *id;
    return m_expectedId;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::MessageIdByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_messageIdByte = *byte;
    return m_messageIdByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::ServiceByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_serviceRequestByte = *byte;
    return m_serviceRequestByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::StatusByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_statusByte = *byte;
    return m_statusByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::ToggleByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_toggleByte = *byte;
    return m_toggleByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::MachineStatusByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_machineStatusByte = *byte;
    return m_machineStatusByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::MotorModeByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_motorModeByte = *byte;
    return m_motorModeByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::LfTargetSpeedByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_leftFrontTargetSpeedByte = *byte;
    return m_leftFrontTargetSpeedByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::LfTargetRevsByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_leftFrontTargetRevsByte = *byte;
    return m_leftFrontTargetRevsByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::LfSimulatedMassByte(void)
{   // NOTE this is the same byte location as the target revs.  If the spec changes, this 
    // will also need to change.
    return m_leftFrontTargetRevsByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::RfTargetSpeedByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)   m_rightFrontTargetSpeedByte = *byte;
    return m_rightFrontTargetSpeedByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::RfTargetRevsByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_rightFrontTargetRevsByte = *byte;
    return m_rightFrontTargetRevsByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::RfSimulatedMassByte(void)
{   // NOTE this is the same byte location as the target revs.  If the spec changes, this 
    // will also need to change.
    return m_rightFrontTargetRevsByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::LrTargetSpeedByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_leftRearTargetSpeedByte = *byte;
    return m_leftRearTargetSpeedByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::LrTargetRevsByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_leftRearTargetRevsByte = *byte;
    return m_leftRearTargetRevsByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::LrSimulatedMassByte(void)
{   // NOTE this is the same byte location as the target revs.  If the spec changes, this 
    // will also need to change.
    return m_leftRearTargetRevsByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::RrTargetSpeedByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)   m_rightRearTargetSpeedByte = *byte;
    return m_rightRearTargetSpeedByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::RrTargetRevsByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_rightRearTargetRevsByte = *byte;
    return m_rightRearTargetRevsByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::RrSimulatedMassByte(void)
{   // NOTE this is the same byte location as the target revs.  If the spec changes, this 
    // will also need to change.
    return m_rightRearTargetRevsByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::ParameterByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_parameterByte = *byte;
    return m_parameterByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::MftNumberByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_mftNumberByte = *byte;
    return m_mftNumberByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::PaNumberByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_paNumberByte = *byte;
    return m_paNumberByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::NumberOfPaBytes(const UINT8 *byteCount) /*= NULL*/
{
    if(byteCount != NULL)  m_numberOfPaBytes = *byteCount;
    return m_numberOfPaBytes;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::ContinuousMftNumberByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_contMftNumberByte = *byte;
    return m_contMftNumberByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::ChecksumByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_checkSumByte = *byte;
    return m_checkSumByte;
}

//-----------------------------------------------------------------------------
const UINT8& DsaIn::DiagnosticByte(const UINT8 *byte) /*= NULL*/
{
    if(byte != NULL)  m_diagByte = *byte;
    return m_diagByte;
}   
