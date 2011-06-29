//*************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//  The  Gen 3 Interface Server will interface to the GM DVT using the Gen 3 Protocol.
//  Messages will be received from the DVT and processed.  A response will be generated to
//  transmit back to the DVT.
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
#ifndef Gen3Interface_h
#define Gen3Interface_h
//-----------------------------------------------------------------------------
#include "BepServer.h"
#include "BepSync.h"
#include "BepTimer.h"
#include "SerialChannel.h"
#include "Gen3Message.h"
#include "Gen3Incoming.h"
#include "Gen3Outgoing.h"
#include "IcmDataSupplier.h"
#include "IBepCommunication.h"
#include "INamedDataBroker.h"
#include <list>

//-----------------------------------------------------------------------------
class Gen3Message;
class Gen3Incoming;
class Gen3Outgoing;

//-----------------------------------------------------------------------------
#define GEN3_PULSE_CODE             SYSTEM_PULSE+6
#define ICM_DATA_PULSE              15
#define SCHEDULED_MSG_PULSE         16
#define INCOMING_MSG_PULSE          17
#define SYSTEM_DATA_UPDATE_PULSE    18
#define RESET_SYSTEM_PULSE          19
#define WHEELBASE_TIMER_PULSE       20
#define FORTY_KMH_BUTTON_PULSE      21

// Keyboard parameter map -- contains indicies for the keyboard parameters
typedef map<string, int>                        KeyboardParameterMap;
typedef KeyboardParameterMap::iterator          KeyboardParameterMapIter;
typedef KeyboardParameterMap::const_iterator    KeyboardParameterMapCIter;
typedef KeyboardParameterMap::value_type        KeyboardParameterMapVal;
// DVT Marker defs
typedef struct _DvtMarker
{
    INT16 type;    // DVT marker type
    INT16 name;    // DVT marker name
    INT32 index;   // Array index of the marker tag
} DvtMarker;
typedef vector<DvtMarker*>       DvtMarkerArray;
// const params
const UINT8 DvtNAK = 0x0F;
const UINT8 DvtACK = 0x06;
// Mode 4 commands
#define Reset                   0x00
#define TractionControlVehicle  0x01
#define AbsVehicle              0x02
#define ArmBrakeTest            0x03
#define ExecuteDragTest         0x04
#define TractionControlTest     0x05
#define BrakeTest               0x06
#define AbsTest                 0x07
#define QuickStop               0x08
#define AxleRatio               0x09
#define FourWdVehicle           0x0C
#define AllWdVehicle            0x0D
#define MaxBrakeForceTest       0x0E
#define MaxParkBrakeForceTest   0x0F
#define BrakeRetest             0x10
#define AbsReset                0x11
#define PlcSoftReset            0x12
#define DeclutchMachine         0x13
#define ClutchMachine           0x14
#define LowerRolls              0x15
#define SetDriveAxle            0x16
#define DriveControl            0x17
#define ApplyRollBrake          0x18
#define SelectWheelbase         0x19
#define HornControl             0x1A
#define StatusLightControl      0x1B
#define PlcAux1Control          0x1C
#define PlcAux2Control          0x1D
#define BoostToSpeed            0x1E
#define SetFrontWheelDrive      0x1F
#define SetRearWheelDrive       0x20
#define ZeroWheelDistanceCntr   0x21
#define SetRetRollPosition      0x22
// Mode 4 Command values
#define EndTractionTest         0
#define Start2ChannelTracTest   1
#define Start4ChannelTracTest   2
#define Start3ChannelTracTest   3
#define BrakeTestEnd            0
#define BrakeTestStart          1
#define EndAbsTest              0
#define StartClass2RwalAbs      1
#define StartClass2AwdAbs       2
#define StartClass23c4cAbs      3
#define StartClass23chanWinAbs  4
#define StartClass24chanWinAbs  5
#define StartKhRwalAbs          6
#define StartKh4walAbs          7
#define RetRollLowPosition      0
#define RetRollHighPosition     1
// Drive axle masks
const UINT8 FrontWheelDriveMask = 0x01;
const UINT8 RearWheelDriveMask  = 0x02;
const UINT8 FourWheelDriveMask  = 0x03;
// Drive control data
#define FrontDrivesOn           0x01
#define RearDrivesOn            0x02
#define FrontAndRearDrivesOn    0x03
const UINT8 DriveControlMask    = 0x03;
// Prompts
const UINT8 PromptNoPrompt                  = 0x00;
const UINT8 PromptStartBrakeTest            = 0x01;
//const UINT8 PromptTireFactorReceived        = 0x02;
const UINT8 PromptAwdBrakeTest              = 0x03;
const UINT8 PromptDragTest                  = 0x04;
const UINT8 PromptBaseBrakeTest             = 0x05;
const UINT8 PromptAbsTest                   = 0x06;
const UINT8 PromptTestComplete              = 0x07;
const UINT8 PromptPedalForceLowFault        = 0x08;
const UINT8 PromptFrontForceHighFault       = 0x09;
const UINT8 PromptRearForceHighFault        = 0x0A;
const UINT8 PromptAbortAbsTest              = 0x0B;
const UINT8 PromptTractionTest              = 0x0C;
const UINT8 PromptPeakBrakeTest             = 0x0D;
const UINT8 PromptWaitingForTestCommands    = 0x10;
const UINT8 PromptAxleRatioTest             = 0x20;
const UINT8 PromptAxleRatioTestComplete     = 0x21;
const UINT8 Prompt941CircuitCheck           = 0x22;
const UINT8 PromptPositractionTest          = 0x30;
const UINT8 PromptPositractionTestComplete  = 0x31;
const UINT8 PromptCrossWireTest             = 0x40;
const UINT8 PromptCrossWireTestComplete     = 0x41;
const UINT8 PromptEncoderRunoutFault        = 0x48;
const UINT8 PromptDriveFault                = 0x49;
const UINT8 PromptWheelSpeedTest            = 0x50;
const UINT8 PromptWheelSpeedTestComplete    = 0x51;
const UINT8 PromptStartParkBrakeTest        = 0x70;
const UINT8 PromptParkBrakeTestArmed        = 0x71;
const UINT8 PromptParkBrakeTestComplete     = 0x72;
const UINT8 PromptWaitForPowerSense         = 0x79;
const UINT8 PromptMachineResetting          = 0x80;
const UINT8 PromptOverSpeed                 = 0x81;
const UINT8 PromptInitializeSystem          = 0x82;
const UINT8 PromptMachineError              = 0x83;
const UINT8 PromptWaitingForZeroSpeed       = 0x88;
const UINT8 PromptParamDownloadInProgress   = 0x90;
const UINT8 PromptParamDownloadComplete     = 0x91;
const UINT8 PromptCalibrateWarmup           = 0xFB;
const UINT8 PromptCalibrateComplete         = 0xFC;
const UINT8 PromptCalibrateSampling         = 0xFD;
const UINT8 PromptCalibrateArm              = 0xFE;
const UINT8 PromptCalibrate                 = 0xFF;

//-----------------------------------------------------------------------------
class Gen3Interface : public BepServer
{
public:
    Gen3Interface();
    virtual ~Gen3Interface();
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
     * Handle the specified pulse.  This method simply calls
     * BepComponent::HandlePulse().
     *
     * @param code  The pulse code to process.
     * @param value The pulse value to process.
     *
     * @return BEP_STATUS_SUCCESS if the pulse was successfully handled.
     *         BEP_STATUS_FAILURE if the pulse was not handled.
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
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
    virtual void Run(volatile bool *terminateFlag);
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
     * <b>Description:</b>
     * Request to perform an action.  This method simply calls 
     * BepComponent::Command().
     *
     * @param node The action to be performed and its value.
     * 
     * @return BEP_SUCCESS_RESPONSE if the action is supported.
     *         BEP_UNSUPPORTED_RESPONSE if the action is unsupported.
     */
    virtual const string Command(const XmlNode *node);
    /**
     * Process a Mode 1 message from the DVT.
     * 
     * @param messageNumber
     *                  Mode 1 message number to process.
     * @param mode1Data Data to use while processing the current mode 1 message.
     * 
     * @return Status of processing the Mode 1 message.
     */
    BEP_STATUS_TYPE HandleMode1Message(const UINT8 &messageNumber, const INT16 &mode1Data);
    /**
     * Get the speed and force data for the requested marker index/type.  The current index plus the next three samples will
     * be added to the message and sent to the host.
     * 
     * @param markerType The type of marker to retrieve.
     * @param markerName The name of the marker to be retrieved.
     * @param markerIndex
     *                   The index of the marker to be retrieved.
     * 
     * @return Status of getting the ICM data and sending it to the host.
     */
    BEP_STATUS_TYPE HandleMode2Message(const INT16 &markerType,
                                       const INT16 &markerName,
                                       const INT16 &markerIndex);
    /**
     * Process the MOde 4 command sent by the test head.
     * 
     * @param command Command that was received from the test head.
     * @param commandValue
     *                Value of the command.
     * 
     * @return Status of processing the command.
     */
    BEP_STATUS_TYPE HandleMode4Message(const UINT8 &command, const UINT8 &commandValue);
    /**
     * Update the specified parameter with the new value.
     * 
     * @param parameterNumber
     *               Parameter to update.
     * @param parameterValue
     *               New value for the specified parameter.
     * 
     * @return Status of updating the parameter to the new value.
     */
    BEP_STATUS_TYPE HandleMode6Message(const INT16 &parameterNumber, const INT16 &parameterValue);
    /**
     * Set the specified marker in the ICM data array.
     * 
     * @param markerType Type of marker to set.
     * @param markerName Name of the marker to set.
     * 
     * @return Status of setting the dvt marker in the ICM data array.
     */
    BEP_STATUS_TYPE HandleMode7Message(const INT16 &markerType, const INT16 &markerName);
    /**
     * Gather the requested test result data and send it to the test head.
     * 
     * @param testResultType
     *               Test result data to be reported.
     * 
     * @return Status of handling the Mode 20 command.
     */
    BEP_STATUS_TYPE HandleMode20Message(const INT16 &testResultType);
    /**
     * Schedule the provided Mode 1 messages for automatic transmission to the test head.
     * 
     * @param scheduledMessages
     *               Scheduled Mode 1 messages to automatically send to the test head.
     * 
     * @return Status of scheduling the mode 1 messages for automatic transmission.
     */
    BEP_STATUS_TYPE HandleMode42Message(const vector<UINT8> &scheduledMessages);

private:
    /**
     * Setup the timer for the ICM data.
     * 
     * @param config Configuration data for setting up the ICM data timer.
     */
    void SetupIcmDataTimer(const XmlNode *config);
    /**
     * Setup the timer for the scheduled mode 1 messages to be sent to the test head.
     * 
     * @param config Configuration data to use for setting up the timer.
     */
    void SetupScheduledMsgTimer(const XmlNode *config);
    /**
     * Setup the timer that checks for and handles incoming messages.
     * 
     * @param config Configuration data to use for setting up the timer.
     */
    void SetupIncomingMessageTimer(const XmlNode *config);
    /**
     * Setup the timer that will update system data items.
     * 
     * @param config Configuration data to use for setting up the timer.
     */
    void SetupSystemDataTimer(const XmlNode *config);
    /**
     * Setup the timer for the reset functionality.
     * 
     * @param config Timer configuration data.
     */
    void SetupResetTimer(const XmlNode *config);
    /**
     * Setup the wheelbase adjust timer.
     * 
     * @param config Data to configure the wheelbase timer.
     */
    void SetupWheelbaseTimer(const XmlNode *config);
    /**
     * Setup the timer for latching the 40km/h button press.
     * 
     * @param config 40km/h button latch timer data.
     */
    void Setup40kmhButtonLatchTimer(const XmlNode *config);
    /**
     * Initialize Mode 1 message structures.
     * 
     * @param msgConfig Data used to initialize mode 1 messages.
     */
    void InitializeMode1Messages(const XmlNode *msgConfig);
    /**
     * Initialize the mode 1 messages with the data items that have been subscribed for.
     */
    void InitializeMode1MessageData(void);
    /**
     * Build the mode 1 message 22 message.
     * The brake force and wheel force data will be read and inserted into the message.
     * 
     * @param startIndex Array index of where to start reading the data.
     * 
     * @return Status of building the message.
     */
    BEP_STATUS_TYPE BuildMode1Message22(const UINT16 &startIndex);
    /**
     * Setup the message structure for mode 2 messages.
     * 
     * @param msgConfig Configuration data for initializing the outgoing message.
     */
    void InitializeMode2Message(const XmlNode *msgConfig);
    /**
     * Setup the parameter data for mode 6 messages.
     * 
     * @param msgConfig Configuration data for initializing the parameter data.
     */
    void InitializeMode6Message(const XmlNode *msgConfig);
    /**
     * Setup the translation tables for marker types and names.
     * 
     * @param msgConfig Translation tables for marker types and marker names.
     */
    void InitializeMode7Message(const XmlNode *msgConfig);
    /**
     * Setup the messages for the mode 20 brake test result items.
     * 
     * @param msgConfig mode 20 message configuration data.
     */
    void InitializeMode20Message(const XmlNode *msgConfig);
    /**
     * Get/Set the timeout for DVT messages.
     * 
     * @param timeout Timeout for DVT messages.
     * 
     * @return Timeout for DVT messages.
     */
    const long& DvtMessageTimeout(const long *timeout = NULL);
    /**
     * Get/Set the amount of time to wait between bytes to determine if the end of the message was received.
     * 
     * @param gapTime Amount of time to wait between bytes to determine if the end of the message was received.
     * 
     * @return Amount of time to wait between bytes to determine if the end of the message was received.
     */
    const long& DvtEndOfMessageGapTime(const long *gapTime = NULL);
    /**
     * Get/Set the number of rollers on the machine.
     * 
     * @param rollerCount
     *               Number of rollers on the machine.
     * 
     * @return Number of roller on the machine.
     */
    UINT8 RollerCount(const UINT8 *rollerCount = NULL);
    /**
     * Update the ICM data.  The ICM data will be read from the system and updated in all the appropriate messages.
     * 
     * @return Status of updating the ICM data.
     */
    INT32 UpdateIcmData(void);
    /**
     * Update the system data items in the Mode 1 messages.
     * 
     * @return Status of updating the system data items.
     */
    INT32 UpdateSystemDataItems(void);
    /**
     * Send the currently scheduled message to the test head.  The message schedule will be checked to make sure there is a
     * valid message to send.  The schedule will also be checked to make sure it is active.  After a message has successfully
     * been sent to the test head, the current message index will be updated.
     * 
     * @return Status of sending the currently scheduled message.
     */
    INT32 SendScheduledMessage(void);
    /**
     * Check for and handle any queued messages.
     * 
     * @return Status of handling messages.
     */
    INT32 CheckForIncomingMessage(void);
    /**
     * Handle the reset system pulse.
     * 
     * @return Status of handling the reset timer.
     */
    INT32 ResetSystemPulseHandler(void);
    /**
     * Handle the wheelbase timer pulse.
     * 
     * @return Status of the wheelbase timer pulse handling.
     */
    INT32 WheelbaseTimerPulseHandler(void);
    /**
     * Handle the timer pulse for the 40km/h button latch timer.
     * 
     * @return Status of handling the 40km/h button latch timer pulse.
     */
    INT32 FortyKmhButtonPulseHandler(void);
    /**
     * Store the indicies of the keyboard parameters.
     * 
     * @param paramIndex Parameter index list.
     */
    void LoadKeyboardParamIndices(const XmlNode *paramIndex);
    /**
     * Get the named keyboard parameter number.
     * 
     * @param parameter Keyboard parameter number to be retrieved.
     * 
     * @return Number of the requested parameter.
     */
    INT32 GetKeyboardParameterIndex(const string &parameter);
    /**
     * Look up the name of the given parameter number.
     * 
     * @param parameterNum
     *               Number of the parameter to look up.
     * 
     * @return The name associated with the specified parameter number.
     */
    string FindParameterName(const INT16 &parameterNum);
    /**
     * Populates messages with the software version information.
     */
    void LoadSoftwareVersion(void);
    /**
     * Look up the code for the provided PLC type.
     * If no code exists for the provided PLC, -1 wil be returned.
     * 
     * @param plcType Type of PLC being used.
     * 
     * @return Code for the specified PLC type or -1 if no code is provided for the PLC type specified.
     */
    INT16 FindPlcCode(const string &plcType);
    /**
     * Find the specified marker that was set by the test head.
     * 
     * @param markerType Marker type to search for.
     * @param markerName Name of the marker to search for.
     * 
     * @return Index of the marker that was found.  If the marker was not found, 0 will be returned.
     */
    INT32 FindDvtMarker(const INT16 &markerType, const INT16 markerName);
    /**
     * Store the DVT marker specified by the test head.
     * 
     * @param markerType Type of marker to set.
     * @param markerName Name of the marker to set.
     */
    void SetDvtMarker(const INT16 &markerType, const INT16 &markerName);
    /**
     * Clear all DVT markers.
     */
    void ClearDvtMarkers(void);
    /**
     * Get/Set the flag indicating if the test stand is ready.
     * 
     * @param ready  Flag indicating if the test stand is ready.
     * 
     * @return Flag indicating if the test sranda is ready.
     */
    const bool& TestStandReady(const bool *ready = NULL);
    /**
     * Create a tag to be used to tag the ICM data array.
     * 
     * @param markerType Type of marker being set.
     * @param markerName Name of the marker being set.
     * 
     * @return Tag to use for tagging the ICM data arrays.
     */
    string CreateIcmTag(const INT16 &markerType, const INT16 &markerName);
    /**
     * Reset the machine.
     * Resetting the system will involve clearing out all stored data from previous testing.
     * 
     * @param command   Command that was sent from the DVT.
     * @return Status of resetting the system.
     */
    BEP_STATUS_TYPE ResetMachine(const UINT8 &command, bool resetBrakes = true);
    /**
     * Set the specified data item to the provided value.
     * 
     * @param dataItem Mode 1 message data item to set.
     * @param value    Value to store for the specified item.
     */
    void SetMode1MessageDataItem(const string dataItem, const UINT8 &value);
    /**
     * Set the specified data item to the provided value.
     * 
     * @param dataItem Mode 1 message data item to set.
     * @param value    Value to store for the specified item.
     */
    void SetMode1MessageDataItem(const string dataItem, const INT16 &value);
    /**
     * Set the specified data item to the provided value.
     * 
     * @param dataItem Mode 1 message data item to set.
     * @param value    Value to store for the specified item.
     */
    void SetMode1MessageDataItem(const string dataItem, const string &value);
    /**
     * Send a command to the brake test component to execute the specified test objective.
     * 
     * @param objective Objective to be performed.
     * @param command   Command that was sent from the DVT.
     * 
     * @return Status of commanding the objective to be executed.
     */
    BEP_STATUS_TYPE CommandBrakeTestObjective(const string &objective, const UINT8& command);
    /**
     * Send a command to the T-case test component to execute the specified test objective.
     * 
     * @param objective Objective to be performed.
     * @param command   Command that was sent from the DVT.
     * 
     * @return Status of commanding the objective to be executed.
     */
    BEP_STATUS_TYPE CommandTcaseTestObjective(const string &objective, const UINT8& command);
    /**
     * Process the traction control command value that was received.
     * 
     * @param commandValue
     *               Command value to process.
     * @param command   Command that was sent from the DVT.
     * 
     * @return Status of processing the traction control command.
     */
    BEP_STATUS_TYPE ProcessTractionControlCommand(const UINT8 &commandValue, const UINT8& command);
    /**
     * Process the base brake command value that was received.
     * 
     * @param commandValue
     *               Command value to process.
     * @param command   Command that was sent from the DVT.
     * 
     * @return Status of processing the base brake command.
     */
    BEP_STATUS_TYPE ProcessBaseBrakeTestCommand(const UINT8 &commandValue, const UINT8& command);
    /**
     * Process the ABS test command that has been received.
     * 
     * @param commandValue
     *               ABS test command to process.
     * @param command   Command that was sent from the DVT.
     * 
     * @return Status of processing the ABS test command.
     */
    BEP_STATUS_TYPE ProcessAbsTestCommand(const UINT8 &commandValue, const UINT8& command);
    /**
     * Perform the axle ratio test.
     * 
     * @param command   Command that was sent from the DVT.
     * @return Result of performing the axle ratio test.
     */
    BEP_STATUS_TYPE PerformAxleRatioTest(const UINT8& command);
    /**
     * Process the drive control command  that was received.
     * 
     * @param control Drive control command to process.
     * 
     * @return Status of processing the drive control.
     */
    BEP_STATUS_TYPE ProcessDriveControlRequest(const UINT8 &control);
    /**
     * move the wheelbase to the specified position.
     * 
     * @param wheelbase Wheelbase that has been selected.
     * 
     * @return Status of moving the wheelbase to the desired position.
     */
    BEP_STATUS_TYPE SelectWheelbasePosition(const UINT8 &wheelbase);
    /**
     * Clear all the selected wheelbase positions.
     * 
     * @return Status of clearing the wheelbase selection.
     */
    BEP_STATUS_TYPE ClearAllWheelbaseSelections(void);
    /**
     * Set the selected wheelbase positions.
     * 
     * @param positionTag Tag of the wheelbase position to lookup in the wheelbase position table.
     * 
     * @return Status of setting the wheelbase positions.
     */
    BEP_STATUS_TYPE SetWheelbasePosition(const string &positionTag);
    /**
     * Get the current wheelbase position from the system.  The value will be looked up in the position table and the 
     * appropriate bits will be set in all Mode 1 messages.
     */
    void SetCurrentWheelbasePosition(void);
    /**
     * Boost the rollers to arm speed for vehicles that are governed.
     * All wheels must be above 30mph in order to boost the rollers to arm speed.
     * 
     * @return Status of boosting the rollers to speed.
     */
    BEP_STATUS_TYPE BoostToArmSpeed(void);
    /**
     * Set the height of the retaining rolls to either the high or low position.
     * 
     * @param height Height position of the retaining rolls.
     * 
     * @return Status of setting the height of the retaining rolls.
     */
    BEP_STATUS_TYPE SetRetainingRollPosition(const UINT8 &height);
    /**
     * Read the requested data tag from the system and return the value.
     * 
     * @param dataTag Data tag to read from the system.
     * 
     * @return Value of the data tag read from the system.
     */
    UINT8 ReadSystemData(const string &dataTag);
    /**
     * Read the requested data tag from the system and return the value.
     * 
     * @param dataTag Data tag to read from the system.
     * 
     * @return Value of the data tag read from the system.
     */
    string ReadSystemDataString(const string &dataTag);
    /**
     * Get/Set the flag indicating if the message schedule is currently active.
     * 
     * @param active Flag indicating if the message schedule is currently active.
     * 
     * @return Flag indicating if the message schedule is currently active.
     */
    const bool& MessageScheduleActive(const bool *active = NULL);
    /**
     * Brake stat data will be saved in all appropriate Mode 20 messages.
     * 
     * @param data   Brake stat data to be inserted into the messages.
     */
    void SetMode20BrakeStatData(const XmlNode *data);
    /**
     * The fault list will be checked to determine if there are any fault listed, the proper messages will be updated.
     * 
     * @param faultList List of current machine faults.
     */
    void UpdateMachineDtcList(const XmlNode *faultList);
    /**
     * Get/Set the maximum number of machine faults to store.
     * 
     * @param maxFaults Maximum number of machine faults to store.
     * 
     * @return Maximum number of machine faults to store.
     */
    const INT32& MaxMachineFaults(const INT32 *maxFaults = NULL);
    /**
     * Look up the specified fault tag to get the associated code.  If the fault tag is not found, FFFF will be returned.
     * 
     * @param faultTag Fault tag to look up.
     * 
     * @return Code for the specified fault tag.
     */
    const string GetFaultCode(const string &faultTag);
    /**
     * Get/Set the minimum amount of time to wait for the wheelbase to start adjusting.
     * 
     * @param startDelay Minimum amount of time to wait for the wheelbase to start adjusting.
     * 
     * @return Minimum amount of time to wait for the wheelbase to start adjusting.
     */
    const INT32& WheelbaseAdjustStartDelay(const INT32 *startDelay = NULL);
    /**
     * Get/Set the flag indicating if the retaining rolls should be raised automatically.
     * 
     * @param autoRaise Flag indicating if the retaining rolls should be raised automatically.
     * 
     * @return Flag indicating if the retaining rolls should be raised automatically.
     */
    const bool& AutoRaiseRetainers(const bool *autoRaise = NULL);
    /**
     * Get/Set the flag indicating if the rollers will be auto-lowered from the GM test head.
     * 
     * @param autoLower Flag indicating if the rollers will be auto-lowered from the GM test head.
     * 
     * @return Flag indicating if the rollers will be auto-lowered from the GM test head.
     */
    const bool& AutoLowerRetainers(const bool *autoLower = NULL);
    /**
     * Amount of time to hold each state of the retainers command.
     * 
     * @param toggleTime Time to hold each state of the retainers command.
     * 
     * @return Time to hold each state of the retainers command.
     */
    const INT32& AutoRetainersToggleTime(const INT32 *toggleTime = NULL);
    /**
     * Load the wheelbase position table.
     */
    void LoadWheelbasePositionTable(void);
    /**
     * Get/Set the name of the file containing the wheelbase position table.
     * 
     * @param name   The name of the file containing the wheelbase position table.
     * 
     * @return The name of the file containing the wheelbase position table.
     */
    const string& WheelbasePositionTableFileName(const string *name = NULL);
    /** Timeout for DVT messages. */
    long m_dvtMsgTimeout;
    /** Amount of time to wait for the end of a DVT message */
    long m_dvtMsgEndWaitTime;
    /** Number of rollers on the machine. */
    UINT8 m_rollerCount;
    /** Maximum number of machine faults to report */
    UINT8 m_maxMachineFaults;
    /** ICM tag index for the start of test */
    INT32 m_testStartIndex;
    /** Flag indicating of the machine is in a ready state. */
    bool m_testStandReady;
    /** Flag to start/stop the automatic sending of scheduled mode 1 messages */
    bool m_activateMessageSchedule;
    /** Copy of the configuration data used to initialize the server. */
    XmlNode *m_config;
    /** List of machine faults and the corresponding codes */
    XmlNodeMap m_machineFaultCodes;
    /** Communication object to use for interfacing with the GM DVT system. */
    SerialChannel *m_dvtComm;
    /** Object to handle processing an incoming Gen 3 message. */
    Gen3Incoming *m_gen3In;
    /** Object to handle sending data back to the GM DVT. */
    Gen3Outgoing *m_gen3Out;
    /** Mode 1 Messages. */
    vector<Gen3Message *> m_mode1Messages;
    /** Mode 2 message. */
    Gen3Message *m_mode2Message;
    /** Mode 20 Messages. */
    vector<Gen3Message *> m_mode20Messages;
    /** Incoming message queue */
    list<SerialString_t> m_incomingMessageQueue;
    /** Timer to use for sending scheduled mode 1 messages to the test head */
    BepTimer m_scheduledMessageTimer;
    /** Timer to use for updating icm data in each of the messages. */
    BepTimer m_icmDataTimer;
    /** Timer to check for new incoming messages */
    BepTimer m_incomingMsgTimer;
    /** Timer to update system data items */
    BepTimer m_systemDataTimer;
    /** Timer to use for resetting the system */
    BepTimer m_resetTimer;
    /** Timer to use to latch the 40km/h button press */
    BepTimer m_40kmhButtonLatchTimer;
    /** Timer to use to make sure the wheelbase start command is turned off */
    BepTimer m_wheelbaseTimer;
    /** Object to use for obtaining ICM data from the system */
    IcmDataSupplier *m_icmData;
    /** Keyboard parameters provided by GM test head. */
    KeyboardParameterMap m_keyboardParamsIndex;
    /**  Keyboard parameters received from GM test head. */
    vector<UINT16> m_keyboardParams;
    /** Wheelbase positions */
    XmlNodeMap m_wheelbasePositions;
    /** List of test result tags. */
    XmlNodeMap m_testResultTags;
    /** Test status tags. */
    XmlNodeMap m_testStatusTags;
    /** Data tags that contain brake statistic information */
    XmlNodeMap m_brakeStatTags;
    /** Codes for supported PLCs. */
    XmlNodeMap m_plcCodes;
    /** Table to use for translating marker types to ascii names. */
    XmlNodeMap m_markerTypeTable;
    /** Table to use for translating marker names to ascii names. */
    XmlNodeMap m_markerNameTable;
    /** Vector to store the DVT markers requested by the test head. */
    DvtMarkerArray m_dvtMarkers;
    /** Scheduled mode 1 messages to be automatically sent to the test head */
    vector<UINT8> m_messageSchedule;
    /** Mutex to coordinate access to the message schedule */
    BepMutex m_msgScheduleMutex;
    /** Mutex to coordinate access to the incoming message queue */
    BepMutex m_incomingMsgQueueMutex;
    /** Communication object to use for command the brake test component to do stuff. */
    IBepCommunication m_brakeComponentComm;
    /** Communication object to use for command the brake test component to do stuff. */
    IBepCommunication m_tcaseComponentComm;
    /** Communication object to use for updating the motor mode */
    IBepCommunication m_motorControllerComm;
    /** Communication object for writing system data */
    INamedDataBroker  *m_ndb;
    /** Current scheduled Mode 1 message to send to the test head */
    UINT8 m_currentScheduledMessage;
    /** System data tags to monitor */
    XmlNodeMap m_systemDataMonitorItems;
    /** Flag to indicate if the wheelbase is adjusting */
    bool m_wheelbaseAdjusting;
    /** Flag to indicate the vehicle is AWD */
    bool m_isAwdVehicle;
    /** Flag to signal brake test and analysis is complete */
    bool m_brakeTestComplete;
    /** Flag to determine if the retaining rolls should be raised automatically */
    bool m_autoRaiseRetainers;
    /** Flag to determine if the retaining rolls should be lowered automatically from the GM test head */
    bool m_autoLowerRetainers;
    /** Amount of time to allow each state when toggling the retainers control flag */
    INT32 m_autoRetainersToggleTime;
    /** Minimum Time to wait for wheelbase to start adjusting */
    INT32 m_wheelbaseAdustStartDelay;
    /** The name of the file containing the wheelbase position table. */
    string m_wheelbasePositionTableFileName;
};

//-----------------------------------------------------------------------------
#endif //Gen3Interface_h
