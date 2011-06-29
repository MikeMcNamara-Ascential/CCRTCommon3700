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
#ifndef DsaIn_h
#define DsaIn_h
//-----------------------------------------------------------------------------
#include "DsaServer.h"
#include "INamedDataBroker.h"
//-----------------------------------------------------------------------------

typedef basic_string<INT8>	DsaSerialString_t;

class DsaServer;
/**
 * Wheel speed data
 */
typedef struct _wheelSpeeds
{
    INT16 lfSpeed;  // Left Front
    INT16 lfRevs;
    INT16 rfSpeed;  // Right Front
    INT16 rfRevs;
    INT16 lrSpeed;  // Left Rear
    INT16 lrRevs;
    INT16 rrSpeed;  // Right Rear
    INT16 rrRevs;
}WheelSpeeds;

/**
 * DSA Incoming message handler.
 * This class will parse the incoming DSA/IDT messages and process the service request.
 * 
 * @author Dan Erickson
 * @see DsaServer
 * @since February 25, 2008
 */
class DsaIn
{
public:
    /**
     * Class constructor.
     * The data elements will be retrieved from the config data and used to initialize the class.
     * 
     * @param config Configuration data to setup for an inbound message.
     * @param server Pointer to the server that instantiated this object.
     */
    DsaIn(const XmlNode *config, DsaServer* server);
    /**
     * Class destructor.
     */
    virtual ~DsaIn();
    /**
     * Process the incoming message.
     * The message will be checked to ensure it is valid prior to processing.
     * 
     * @param message DSA message to be processed.
     * 
     * @return Flag indicating if the message is valid.
     */
    bool ProcessMessage(DsaSerialString_t &message);

protected:
    /**
     * Store the message data.
     * 
     * @param config Message data items.
     */
    void StoreMessageData(const XmlNode *config);
    /**
     * Store the config data indicting where specific bytes are located in the message.
     * 
     * @param byteLocations
     *               Byte location data.
     */
    void StoreByteLocations(const XmlNode *byteLocations);
    /**
     * Verify the checksum is valid.  The checksum is calculated by XORing all message bytes together.
     * 
     * @param message Message to be validated.
     * 
     * @return Flag indicatnig if the checksum is valid.
     */
    bool ValidateChecksum(const DsaSerialString_t &message);
    /**
     * Adjust the wheelbase to the specified location.
     * 
     * @param wheelbase Wheelbase position to move to.  This value will typically be in mm and converted to inches 
     *                  x10 and sent to the PLC.
     * 
     * @return Status of adjusting the wheelbase.
     */
    BEP_STATUS_TYPE AdjustWheelbase(INT32 &wheelbase);
    /**
     * Command the machine component to execute a machine control operation.
     * 
     * @param control Machine control operation to be performed.
     * 
     * @return Status of performing the machine control operation.
     */
    BEP_STATUS_TYPE MachineControl(const string &control);
    /**
     * The PA number will be extracted from the message and stored in a member variable.
     * The PA number will also be published to the system as the "VIN".
     * 
     * @param message Message from which to extract the PA number.
     * 
     * @return Status of extracting the PA number from the message.
     */
    BEP_STATUS_TYPE StorePaNumber(const DsaSerialString_t &message);

private:
    /**
     * The machine state will be set to the requested values.
     * 
     * @param machineState
     *                  Retaining roll state.
     * @param motorMode Requested motor mode.
     * @param speeds    Requested roller speed.
     * 
     * @return Status of setting the machine to the desired state.
     */
    BEP_STATUS_TYPE ProcessControlRequests(INT8 &machineState, INT8 &motorMode, WheelSpeeds &speeds);
    /**
     * Get/Set the pointer to the server that instantiated this object.
     * 
     * @param server POinter to the server that instantiated this object.
     * 
     * @return Pointer to the server that instantiated this server.
     */
    DsaServer* Server(DsaServer *server = NULL);
    /**
     * Extract the requested roller speed/torque from the message string.
     * 
     * @param rollerData Message string to extract the roller data from.
     * 
     * @return Reuested roller speed/torque values.
     */
    WheelSpeeds ExtractRequestedRollerData(const DsaSerialString_t &rollerData);
    /**
     * Determine if the requested service is allowed at this time.
     * If a service is currently in progress, no new service is allowed to be started.
     * 
     * @param requestedService
     *               Requested service ID.
     * 
     * @return Flag indicating if the requested service can be performed.
     */
    bool RequestedServiceAllowed(const UINT8 &requestedService);
    /**
     * Get/Set the expected message ID.
     * If no value is provided, the currently stored value will be returned.
     * 
     * @param id     Expected message ID.
     * 
     * @return Expected message ID.
     */
    const UINT8& ExpectedId(const UINT8 *id = NULL);
    /**
     * Get/Set the location of the message ID byte.
     * 
     * @param byte   Location of the message ID.
     * 
     * @return Location of the message ID.
     */
    const UINT8& MessageIdByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the requested service byte location.
     * 
     * @param byte   Requested service byte location.
     * 
     * @return Requested service byte location.
     */
    const UINT8& ServiceByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the status byte location.
     * 
     * @param byte   Status byte location.
     * 
     * @return Status byte location.
     */
    const UINT8& StatusByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the toggle byte location.
     * 
     * @param byte   Location of the toggle byte.
     * 
     * @return Location of the toggle byte.
     */
    const UINT8& ToggleByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the machine status byte.
     * 
     * @param byte   Location of the machine status byte.
     * 
     * @return Location of the machine status byte.
     */
    const UINT8& MachineStatusByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the requested motor mode byte.
     * 
     * @param byte   Location of the requested motor mode byte.
     * 
     * @return Location of the requested motor mode byte.
     */
    const UINT8& MotorModeByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the requested left front target speed byte.
     * 
     * @param byte   Location of the requested left front target speed byte.
     * 
     * @return Location of the requested left front target speed byte.
     */
    const UINT8& LfTargetSpeedByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the requested left front target revolutions byte.
     * 
     * @param byte   Location of the left front target revolutions byte.
     * 
     * @return Location of the left front target revolutions byte.
     */
    const UINT8& LfTargetRevsByte(const UINT8 *byte = NULL);
    /**
     * Get the location of the simulated mass byte.
     * 
     * @return The location of the Simulated Mass byte.
     */
    const UINT8& LfSimulatedMassByte(void);
    /**
     * Get/Set the location of the requested right front target speed byte.
     * 
     * @param byte   Location of the requested right front target speed byte.
     * 
     * @return Location of the requested right front target speed byte.
     */
    const UINT8& RfTargetSpeedByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the requested right front target revolutions byte.
     * 
     * @param byte   Location of the right front target revolutions byte.
     * 
     * @return Location of the right front target revolutions byte.
     */
    const UINT8& RfTargetRevsByte(const UINT8 *byte = NULL);
    /**
     * Get the location of the simulated mass byte.
     * 
     * @return The location of the Simulated Mass byte.
     */
    const UINT8& RfSimulatedMassByte(void);
    /**
     * Get/Set the location of the requested left rear target speed byte.
     * 
     * @param byte   Location of the requested left rear target speed byte.
     * 
     * @return Location of the requested left rear target speed byte.
     */
    const UINT8& LrTargetSpeedByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the requested left rear target revolutions byte.
     * 
     * @param byte   Location of the left rear target revolutions byte.
     * 
     * @return Location of the left rear target revolutions byte.
     */
    const UINT8& LrTargetRevsByte(const UINT8 *byte = NULL);
    /**
     * Get the location of the simulated mass byte.
     * 
     * @return The location of the Simulated Mass byte.
     */
    const UINT8& LrSimulatedMassByte(void);
    /**
     * Get/Set the location of the requested right rear target speed byte.
     * 
     * @param byte   Location of the requested right rear target speed byte.
     * 
     * @return Location of the requested right rear target speed byte.
     */
    const UINT8& RrTargetSpeedByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the requested right rear target revolutions byte.
     * 
     * @param byte   Location of the right rear target revolutions byte.
     * 
     * @return Location of the right rear target revolutions byte.
     */
    const UINT8& RrTargetRevsByte(const UINT8 *byte = NULL);
    /**
     * Get the location of the simulated mass byte.
     * 
     * @return The location of the Simulated Mass byte.
     */
    const UINT8& RrSimulatedMassByte(void);
    /**
     * Get/Set the location of the first parameter byte.
     * 
     * @param byte   Location of the first parameter byte.
     * 
     * @return Location of the first parameter byte.
     */
    const UINT8& ParameterByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the MFT number byte.
     * 
     * @param byte   Location of the MFT number byte.
     * 
     * @return Location of the MFT number byte.
     */
    const UINT8& MftNumberByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the starting location of the PA number bytes.
     * 
     * @param byte   Starting location of the PA number bytes.
     * 
     * @return Starting location of the PA number bytes.
     */
    const UINT8& PaNumberByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the number of bytes in the PA number.
     * 
     * @param byteCount The number of bytes in the PA number.
     * 
     * @return The number of bytes in the PA number.
     */
    const UINT8& NumberOfPaBytes(const UINT8 *byteCount = NULL);
    /**
     * Get/Set the location of the continuous MFT number byte.
     * 
     * @param byte   Location of the continuous MFT number byte.
     * 
     * @return Location of the continuous MFT number byte.
     */
    const UINT8& ContinuousMftNumberByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the checksum byte.
     * 
     * @param byte   Location of the checksum byte.
     * 
     * @return Location of the checksum byte.
     */
    const UINT8& ChecksumByte(const UINT8 *byte = NULL);
    /**
     * Get/Set the location of the diagnostic data byte.
     * 
     * @param byte   Location of the diagnostic data byte.
     * 
     * @return Location of the diagnostic data byte.
     */
    const UINT8& DiagnosticByte(const UINT8 *byte = NULL);
    /**
     * Pointer to the server that instantiated this object.
     * This will be used mainly for logging purposes.
     * 
     * @see BepServer
     */
    DsaServer *m_server;
    /**
     * Message received from the external tester.
     */
    DsaSerialString_t m_currentMessage;
    /**
     * Previously processed message.  This is used to ensure proper service request sequences.
     */
    DsaSerialString_t m_previousMessage;
    /**
     * Byte containing the message ID.
     */
    UINT8 m_messageIdByte;
    /**
     * Expected message ID.
     */
    UINT8 m_expectedId;
    /**
     * Requested service byte.
     */
    UINT8 m_serviceRequestByte;
    /**
     * External tester status byte.
     */
    UINT8 m_statusByte;
    /**
     * Toggle byte from the external tester.
     */
    UINT8 m_toggleByte;
    /**
     * Byte containing the machine status requested from the external tester.
     */
    UINT8 m_machineStatusByte;
    /**
     * Requested motor mode from the external tester.
     */
    UINT8 m_motorModeByte;
    /**
     * Target speed for the Left Front roller from the external tester.
     */
    UINT8 m_leftFrontTargetSpeedByte;
    /**
     * Requested target revolutions for the Left Front roller.
     */
    UINT8 m_leftFrontTargetRevsByte;
    /**
     * Target speed for the Right Front roller from the external tester.
     */
    UINT8 m_rightFrontTargetSpeedByte;
    /**
     * Requested target revolutions for the Right Front roller.
     */
    UINT8 m_rightFrontTargetRevsByte;
    /**
     * Target speed for the Left Rear roller from the external tester.
     */
    UINT8 m_leftRearTargetSpeedByte;
    /**
     * Requested target revolutions for the Left Rear roller.
     */
    UINT8 m_leftRearTargetRevsByte;
    /**
     * Target speed for the Right Rear roller from the external tester.
     */
    UINT8 m_rightRearTargetSpeedByte;
    /**
     * Requested target revolutions for the Right Rear roller.
     */
    UINT8 m_rightRearTargetRevsByte;
    /**
     * Parmaeter 1 byte from the external tester.
     */
    UINT8 m_parameterByte;
    /**
     * Byte location of the MFT number.
     */
    UINT8 m_mftNumberByte;
    /**
     * PA number from the external tester.
     */
    UINT8 m_paNumberByte;
    /**
     * Number of bytes in the PA number.
     */
    UINT8 m_numberOfPaBytes;
    /**
     * Byte containing the continuous MFT number.
     */
    UINT8 m_contMftNumberByte;
    /**
     * Byte containing the message checksum.
     */
    UINT8 m_checkSumByte;
    /**
     * Diagnostic data byte from the external tester.
     */
    UINT8 m_diagByte;
    /**
     * Flag to indicate if the wheelbase is adjusting.
     */
    bool m_wheelbaseAdjusting;
    /**
     * List of the valid motor modes supported.
     */
    XmlNodeMap m_validMotorModes;
    /**
     * List of valid service requests.
     */
    XmlNodeMap m_validServiceRequests;
    /**
     * List of parameters to control behavior of the program.
     */
    XmlNodeMap m_parameters;
    /**
     * NamedDataBroker Interface object.
     */
    INamedDataBroker m_ndb;
    /**
     * Machine test component communication object.
     * This will be used to update machine states.
     */
    IBepCommunication *m_machineComponent;
    /**
     * Supported services.
     */
    static const UINT8 ServiceAbort          = 0;
    static const UINT8 ServiceStartTest      = 1;
    static const UINT8 ServiceEndTest        = 2;
    static const UINT8 ServiceControlDev     = 3;
    static const UINT8 ServiceWheelbase      = 4;
    static const UINT8 ServiceDefaultState   = 5;
    static const UINT8 ServiceDataLogging    = 14;
    static const UINT8 ServiceHeightResults  = 31;
    static const UINT8 ServiceStorePaNumber  = 32;
    static const UINT8 ServiceStartPowerTest = 33;
    static const UINT8 ServiceStopPowerTest  = 34;
    /**
     * Supported motor modes.
     */
    static const UINT8 MotorModeFreeRoll    = 0;
    static const UINT8 MotorModeFwd         = 1;
    static const UINT8 MotorModeRwd         = 2;
    static const UINT8 MotorModeAwd         = 3;
    static const UINT8 MotorModeStaticBrake = 4;
    static const UINT8 MotorModeBrake1      = 5;
    static const UINT8 MotorModeBrake2      = 6;
    static const UINT8 MotorModeAllStop     = 7;
};

//-----------------------------------------------------------------------------
#endif  //DsaIn_h
