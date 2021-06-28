//*************************************************************************
// FILE:
//    $Header: /Common3700/Source/Tesla/ServicesLayer/Servers/PlantHostInbound/MesDataController/MesDataController.h 1     3/09/06 3:16p Gswope $
//
// FILE DESCRIPTION:
//   MesDataController class to handle communications from the DCX
//	 broadcast system to the Burke Porter system
//
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
//    $Log: /Common3700/Source/Tesla/ServicesLayer/Servers/PlantHostInbound/MesDataController/MesDataController.h $
// 
// 1     3/09/06 3:16p Gswope
// After changes to bring Tesla up to "current core"
// 
// 1     1/18/05 10:47a Gswope
// Initial Entry in this project in SourceSafe
// 
// 1     10/19/04 2:35p Bmeinke
// MesDataController server for Tesla.
// Copied from DaimlerChrysler
// 
// 1     8/04/04 1:07p Derickso
// 
// 1     1/27/04 9:49a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 1     8/21/03 9:08a Derickso
// 05WK CCRT Project
// 
// 1     7/09/03 2:31p Rwiersem
// Initial version.
// 
// 1     7/03/03 5:20p Khykin
// Initial checkin
// 
// 1     6/12/03 4:26p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:49  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:11a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
// 
// 8     8/14/02 12:09p Skypig13
// Removed compiler warnings
// 
// 7     6/13/02 3:05p Dan.erickson
// Added suppport for WccNoResponse prompt.
// 
// 5     5/06/02 3:38p Dan.erickson
// Moved values that determine a valid message length to the
// config file in the event the length of a valid message changes.
// 
// 4     4/24/02 3:19p Dan.erickson
// Added Serial communications capability.
// 
// 3     4/08/02 2:51p Dan.erickson
// Switched Set methods to use DeepCopy instead of addNode.
// 
// 2     4/08/02 12:06p Dan.erickson
// Made destructor virtual.
// Overloaded base class ClearMaps() method.
// Modified SetBodyStyleDriveAxle to explicitly add each node to the map.
// 
// 1     4/05/02 7:49a Dan.erickson
// Created.
// 
//*************************************************************************
#ifndef MesDataController_h
#define MesDataController_h
//==============================================================================

#include <errno.h>
#include "BepSync.h"
#include "PlantHostInbound.h"
#include "IPromptServer.h"
#include "IFaultServer.h"
#include "SerialChannel.h"
#include "GeneralUtilities.h"
#include "ITestResultServer.h"


/**
 * Name of the default node for the drive axle selection.
 * @since Version 1.0
 */
#define DEFAULT_NODE    "_Default"
/**
 * Macros to use with broadcast messages.
 * @since Version 1.0
 */
#define ST          0x02
#define ET          0x03
#define ENQ         0x05
#define ACK         0x06
#define CR          0x0D
#define LF          0x0A
#define NO_INFO     0x11
#define NAK         0x15
#define KEEP_ALIVE  0x10
#define CLOCK       0xF0
/**
  * Pulse code and value to use for checking data file status.
  */
#define INTER_CCRT_COMM_SERVER_PULSE_CODE     SYSTEM_PULSE+19
#define KEEPALIVE_PULSE     11

/**
 * Constants to use when evaluating responses from broadcast.
 * @since Version 1.0
 */
const INT16 shortBroadcastMessage = 3;
/**
 * Signals to either retrieve or not retrieve the build record from braodcast.
 * @since Version 1.2
 */
const std::string getBuildRecord("GetBuildRecord");
const std::string doNotGetBuildRecord("DoNotGetBuildRecord");

const std::string requestBuild("GetOptionCodesByVin");


/**
 * This class handles data from the DaimlerChrysler broadcast system.
 * Vehicle build records are translated into a format usable by the Burke Porter
 * system.  The MesDataController class also manages the validity of the
 * Vehicle build record and displays prompts if no information or bad information
 * is received from broadcast.
 * 
 * @author Dan Erickson
 * @version Version 1.3
 * @since Version 1.0
 */
class MesDataController : public PlantHostInbound
{
public:
	/**
	 * Default constructor.
	 * @since Version 1.0
	 */
	MesDataController();
	/**
	 * Default Destructor
	 * @since Version 1.0
	 */
	virtual ~MesDataController();
	/**
	 * Method for initialization passing in the file name
	 *
	 * @param fileName Name of the XML configuration file to be used to initialize
	 * @since Version 1.0
	 */
	virtual void Initialize(const std::string &fileName);
	/**
	 * Method for initialization.
	 *
	 * @param document Reference to a parsed configuration document.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);
	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
	 *		   BEP_SUCCESS_RESPONSE == Registered
	 * @since Version 1.1
	 */
	virtual const std::string Register(void);
	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag and data that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 * @since Version 1.2
	 */
	virtual const std::string Publish(const XmlNode *node);
	/**
	 * Main run method that subclasses can overload if different
	 * behavior is desired.
	 *
	 * @param terminateFlag
	 *               Flag used to terminate the system.
	 * @since Version 1.2
	 */
	virtual void Run(volatile bool *terminateFlag=NULL);
	/**
	 * Get the build record from the broadcast system.
	 * 
	 * @return Status of the retrieval.
	 * @since Version 1.2
	 */
	virtual const std::string RetrieveBuildRecord(void);
	/**
	 * Add any information that was derived from the vehicle build to the vehicle build record.
	 * 
	 * @since Version 1.0
	 */
	virtual void AddDerivedBuildInfo(XmlNodeMap &buildData);
	/**
	 * Determines if the vehicle is equipped with All Wheel Drive.
	 * 
	 * @param transferCaseType
	 *               The type of transfer case the vehicle is equipped with.
	 * @return Yes - The vehicle is equipped with All Wheel Drive.
	 *         No  - The vehicle is not equipped with All Wheel Drive.
	 * @since Version 1.0
	 */
	virtual const std::string HasAllWheelDrive(const std::string &transferCaseType);
	/**
	 * Determines if the vehicle is equipped with Four Wheel Drive.
	 * 
	 * @param transferCaseType
	 *               The type of transfer case the vehicle is equipped with.
	 * @return Yes - The vehicle is equipped with Four Wheel Drive.
	 *         No  - The vehicle is not equipped with Four Wheel Drive.
	 * @since Version 1.0
	 */
	virtual const std::string HasFourWheelDrive(const std::string &transferCaseType);
	/**
	 * Uses the body style to select the drive axle for the current vehicle.
	 * 
	 * @param bodyStyle The vehicle body style.
	 * @return The drive axle for the supplied body style (Rear, Front).
	 * @since Version 1.0
	 */
	const std::string GetDriveAxle(const std::string &bodyStyle);
	/**
	 * Return the tag being used to signify the selection of rear wheel drive.
	 * 
	 * @return Name of the tag.
	 * @since Version 1.0
	 */
	const std::string GetRearWheelDriveTag(void);
	/**
	 * Return the tag being used for selection of front wheel drive.
	 * 
	 * @return Name of the tag.
	 * @since Version 1.0
	 */
	const std::string GetFrontWheelDriveTag(void);
	/**
	 * Return the tag being used for selection of four wheel drive.
	 * 
	 * @return Name of the tag.
	 * @since Version 1.0
	 */
	const std::string GetFourWheelDriveTag(void);
	/**
	 * Return the tag being used for selection of automatic transmission.
	 * 
	 * @return Name of the tag.
	 * @since Version 1.0
	 */
	const std::string GetAutomaticTransmissionTag(void);
	/**
	 * Return the tag being used for selection of manual transmission.
	 * 
	 * @return Name of the tag.
	 * @since Version 1.0
	 */
	const std::string GetManualTransmissionTag(void);

    const std::string GetRequestingInfoPrompt(void);
	/**
	 * Return the prompt tag for "No Information".
	 * 
	 * @return Prompt Tag
	 * @since Version 1.0
	 */
	const std::string GetNoInfoPrompt(void);

    const std::string GetInfoErrorPrompt(void);
	/**
	 * Return the prompt tag for "Bad Checksum".
	 * 
	 * @return Prompt Tag.
	 * @since Version 1.0
	 */
	const std::string GetBadChecksumPrompt(void);
	/**
	 * Return the tag being used for the prompt "Unknown Message".
	 * 
	 * @return Prompt tag.
	 * @since Version 1.0
	 */
	const std::string GetUnknownMessagePrompt(void);
	/**
	 * Return the prompt tag for NAK from Work Cell.
	 * 
	 * @return Prompt tag.
	 * @since Version 1.0
	 */
	const std::string GetNAKPrompt(void);
	/**
	 * Return the prompt tag for no response from the 
	 * external workcell controller.
	 * 
	 * @return Prompt tag.
	 * @since Version 1.1
	 */
	const std::string GetNoResponsePrompt(void);
	/**
	 * Return the index for the time field.
	 * 
	 * @return Index
	 * @since Version 1.0
	 */
	const INT32 GetTimeIndex(void);
	/**
	 * Return the length of the time field.
	 * 
	 * @return Field length.
	 * @since Version 1.0
	 */
	const INT32 GetTimeLength(void);
	/**
	 * Return the start index for the date field.
	 * 
	 * @return Start index.
	 * @since Version 1.0
	 */
	const INT32 GetDateIndex(void);
	/**
	 * Return the length of the date field
	 * 
	 * @return Field Length.
	 * @since Version 1.0
	 */
	const INT32 GetDateLength(void);
	/**
	 * Return the index of the checksum for the broadcast string.
	 * 
	 * @return Checksum index.
	 * @since Version 1.0
	 */
	const INT16 GetChecksumIndex(void);
	/**
	 * Return the number of attempts to make before failing.
	 * 
	 * @return Number of attempts.
	 * @since Version 1.0
	 */
	const INT32 GetMaxAttempts(void);
	/**
	 * Get the maximum amount of time to wait for a response from broadcast.
	 * 
	 * @return Maximum amount of time to wait.
	 * @since Version 1.0
	 */
	const INT32 GetMaxResponseTime(void);
    /**
	 * Get the amount of time to wait between checking for new data at the serial port.
	 * 
	 * @return Delay time in ms.
	 * @since Version 1.0
	 */
	const INT32 GetMessageDelay(void);
	/**
	 * Return the length of valid messages recieved from broadcast.
	 * 
	 * @return Length of a valid message.
	 * @since Version 1.0
	 */
	const UINT32 GetValidMessageLength(void);
	/**
	 * Return the length of valid messages recieved from broadcast.
	 * 
	 * @return Length of a valid message.
	 * @since Version 1.0
	 */
	const UINT32 GetValidWccResponseLength(void);
	/**
	 * Determines if the retaining rolls are down.
	 * 
	 * @return True  - Retaining rolls are down
	 *         False - Retaining rolls are up
	 * @since Version 1.0
	 */
	const bool AreRetainersDown(void);
	/**
	 * Return the setup being used for the serial channel communications to the plant broadcast system.
	 * 
	 * @return Serial channel setup data.
	 * @since Version 1.0
	 */
	const XmlNode* GetBroadcastCommSetup(void);
	/**
	 * Get the VIN to retrieve.
	 * 
	 * @return VIN for Build record.
	 * @since Version 1.2
	 */
	const std::string GetVINToLoad(void);

protected:
	/**
	 * Reads the vehicle build record from the plant broadcast system.  This will be specific to each
	 * manufacturer so no base implementation is provided.
	 * 
	 * @param vin    Vin to request build record for.
	 * @return The vehicle build record returned from the plant broadcast system.
	 * @since Version 1.1
	 */
	virtual const std::string ReadBuildRecordFromBroadcast(const std::string &vin, std::string &buildRecord, const bool &updateStatus);
	/**
	 * Get the response from our request for data.
	 * 
	 * @param response The response from broadcast
	 * @param vin    Vin that is being requested.
	 * @since Version 1.0
	 */
	virtual void GetBroadcastResponse(std::string &response, const std::string &vin);
	/**
	 * Evaluate the response that was received from broadcast.
	 * 
	 * @param response The response from broadcast.
	 * @return Status of the evaluation.
	 * @since Version 1.0
	 */
	const BEP_STATUS_TYPE EvaluateResponse(std::string &response, const std::string &vin);
	/**
	 * Translate the build string read from broadcast to an XML representation.
	 * 
	 * @param buildRecord
	 *               Vehicle build record from broadcast.
	 * @return Xml representation of build record.
	 * @since Version 1.0
	 */
	virtual void TranslateBuildRecord(const std::string &buildRecord, XmlNodeMap &buildData, const bool &updateStatus);
	/**
	 * Evaluates the criteria to determine which wheelbase position to select.
	 * 
	 * @param selectData Data used to determine which wheelbase to select
	 * @return Wheelbase position for the specified selectData.
	 * @since Version 1.0
	 */
	virtual const std::string GetWheelBasePosition(const std:: string &selectData); 
	/**
	 * Stores all the data tags that will be used by a PlantHost object.
	 * 
	 * @param setupNode The setup node where all the information is located.
	 * @since Version 1.1
	 */
	virtual void LoadDataTags(const XmlNode *setupNode);
	/**
	 * Stores the prompt tags the server will be responsible for displaying.
	 * 
	 * @param promptTags The PromptTags node from the config file.
	 * @since Version 1.0
	 */
	virtual void LoadPromptTags(const XmlNode *promptTags);
	/**
	 * Loads any data specified in the config file.  Looking for indices and field lengths
	 * for special use data such as the current broadcast date and time fields. 
	 * 
	 * @param wccDataNode
	 *               Data node from config file.
	 * @since Version 1.0
	 */
	virtual void LoadData(const XmlNode *wccDataNode);
    virtual const INT32 CalculateSectionStartIndex(const string &section);
    virtual INT32 GetStartIndex(const XmlNode *node);
    virtual INT32 GetFieldLength(const XmlNode *node);
    virtual bool IsStartIndexValid(const XmlNode *field, const INT32 &messageLength);
    virtual INT32 GetSectionLength(const string &section);
    virtual void StoreSectionLengths(const string &buildMessage);
	/**
	 * Save the list of body styles and the drive axle for each body style.
	 * 
	 * @param bodyStyleDriveAxle
	 *               List of body styles and the corresponding drive axle.
	 * @since Version 1.0
	 */
	void SetBodyStyleDriveAxle(const XmlNode *bodyStyleDriveAxle);
	/**
	 * Store the tag to use to signify the selection of rear wheel drive.
	 * 
	 * @param tag    Name of the tag.
	 * @since Version 1.0
	 */
	void SetRearWheelDriveTag(const std::string &tag);
	/**
	 * Store the tag to use for selection of front wheel drive.
	 * 
	 * @param tag    Name of the tag.
	 * @since Version 1.0
	 */
	void SetFrontWheelDriveTag(const std::string &tag);
	/**
	 * Store the tag to use for selection of four wheel drive.
	 * 
	 * @param tag    Name of the tag.
	 * @since Version 1.0
	 */
	void SetFourWheelDriveTag(const std::string &tag);
	/**
	 * Store the tag to use for selection of automatic transmission.
	 * 
	 * @param tag    Name of the tag.
	 * @since Version 1.0
	 */
	void SetAutomaticTransmissionTag(const std::string &tag);
	/**
	 * Store the tag to use for selection of manual transmission.
	 * 
	 * @param tag    Name of the tag.
	 * @since Version 1.0
	 */
	void SetManualTransmissionTag(const std::string &tag);

    void SetRequestingInfoPrompt(const std::string &prompt);
	/**
	 * Store the prompt tag for "No Information".
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetNoInfoPrompt(const std::string &prompt);

    void SetInfoErrorPrompt(const std::string &prompt);
	/**
	 * Store the prompt tag for "Bad checksum".
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetBadChecksumPrompt(const std::string &prompt);
	/**
	 * Store the prompt tag being used for "Unknown Message"
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetUnknownMessagePrompt(const std::string &prompt);
	/**
	 * Set the prompt being used for Blocked by PFS.
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetTestBlockPrompt(const std::string &prompt);
	/**
	 * Set the prompt being used for PFS Abort status.
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetAbortPrompt(const std::string &prompt);
	/**
	 * Set the prompt being used for Vehicle in Ship status.
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetShipStatusPrompt(const std::string &prompt);
	/**
	 * Store the background color to use for the PFS Abort message.
	 *
	 * @param color  Background color to use for the prompt box.
	 * @since Version 1.3
	 */
	void SetAbortColor(const string &color);
	/**
	 * Store the background color to use for the PFS Ship message.
	 *
	 * @param color  Background color to use for the prompt box.
	 * @since Version 1.3
	 */
	void SetShipStatusColor(const string &color);
	/**
	 * Set the prompt to use if a NAK is recevied from the Work Cell.
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetNAKPrompt(const std::string &prompt);
	/**
	 * Set the prompt to usse if no response is sent from 
	 * the external workcell controller.
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.1
	 */
	void SetNoResponsePrompt(const std::string &prompt);
	/**
	 * Save the index for the time field in the build record from broadcast.
	 * 
	 * @param index  Index.
	 * @since Version 1.0
	 */
	void SetTimeIndex(const std::string &index);
	/**
	 * Save the index for the time field in the build record from broadcast.
	 * 
	 * @param index  Index.
	 * @since Version 1.0
	 */
	void SetTimeIndex(const INT32 &index);
	/**
	 * Store the length of the time field.
	 * 
	 * @param length Length of the field.
	 * @since Version 1.0
	 */
	void SetTimeLength(const std::string &length);
	/**
	 * Store the length of the time field.
	 * 
	 * @param length Length of the field.
	 * @since Version 1.0
	 */
	void SetTimeLength(const INT32 &length);
	/**
	 * Store the index of the date field in the broadcast build record.
	 * 
	 * @param index  Start index.
	 * @since Version 1.0
	 */
	void SetDateIndex(const std::string &index);
	/**
	 * Store the index of the date field in the broadcast build record.
	 * 
	 * @param index  Start index.
	 * @since Version 1.0
	 */
	void SetDateIndex(const INT32 &index);
	/**
	 * Store the length of the date field.
	 * 
	 * @param length length of the field.
	 * @since Version 1.0
	 */
	void SetDateLength(const std::string &length);
	/**
	 * Store the length of the date field.
	 * 
	 * @param length length of the field.
	 * @since Version 1.0
	 */
	void SetDateLength(const INT32 &length);
	/**
	 * Store the index for the checksum in the broadcast string.
	 * 
	 * @param index  Index of the checksum.
	 * @since Version 1.0
	 */
	void SetChecksumIndex(const std::string &index);
	/**
	 * Store the index for the checksum in the broadcast string.
	 * 
	 * @param index  Index of the checksum.
	 * @since Version 1.0
	 */
	void SetChecksumIndex(const INT16 &index);
	/**
	 * Set the maximum number of times to request data from broadcast before failing. 
	 * 
	 * @param attempts Number of attempts to make.
	 * @since Version 1.0
	 */
	void SetMaxAttempts(const std::string &attempts);
	/**
	 * Set the maximum number of times to request data from broadcast before failing. 
	 * 
	 * @param attempts Number of attempts to make.
	 * @since Version 1.0
	 */
	void SetMaxAttempts(const INT32 &attempts);
	/**
	 * Set the maximum of time to wait for a response from broadcast.
	 * 
	 * @param maxTime Maximum time to wait.
	 * @since Version 1.0
	 */
	void SetMaxResponseTime(const std::string &maxTime);
	/**
	 * Set the maximum of time to wait for a response from broadcast.
	 * 
	 * @param maxTime Maximum time to wait.
	 * @since Version 1.0
	 */
	void SetMaxResponseTime(const INT32 &maxTime);

    /**
     * Set the maximum of time to wait for a response from broadcast.
     * 
     * @param maxTime Maximum time to wait.
     * @since Version 1.0
     */
    void SetMessageDelay(const std::string &delay);


    /**
     * Set the time to wait in ms between failed attempts to send a message to the
     * external workcell controller.
     * 
     * @param delay  Delay time in ms.
     * @since Version 1.0
     */
    void SetMessageDelay(const INT32 &delay);

	/**
	 * Set the minimum length for valid messages recieved from broadcast.
	 * 
	 * @param length Length of a valid message.
	 * @since Version 1.0
	 */
	void SetValidMessageLength(const UINT32 &length);
	/**
	 * Set the minimum length for valid messages recieved from broadcast.
	 * 
	 * @param length Length of a valid message.
	 * @since Version 1.0
	 */
	void SetValidWccResponseLength(const UINT32 &length);
	/**
	 * Clear the mappings.
	 * @since Version 1.0
	 */
	virtual void ClearMaps(void);
	/**
	 * Store the setup data for the serial channel communication object to the plant broadcast system.
	 * 
	 * @param setup  Serial channel setup data.
	 * @since Version 1.0
	 */
	void SetBroadcastCommSetup(const XmlNode *setup);
	/**
	 * Store the VIN for the build record to be retrieved
	 * 
	 * @param vin    VIN to be retrieved.
	 * @since Version 1.2
	 */
	inline void SetVINToLoad(const std::string &vin);
    const bool& UseSectionLengthData(const bool *useSectionData = NULL);
    void WaitForMesHost(void);
    void SetReconnectDelay(const INT32 &delay);
    const INT32& GetReconnectDelay(void);
    /**
	 * Update the state of the InputServer if it needs to be changed.
	 */
	virtual void UpdateInputServerState(void);
	/**
	 * Semaphore to signal us to go get the build record from broadcast.
	 * @since Version 1.2
	 */
	BepCondVar<std::string, false> m_getBuildRecord;

    const INT32 HandlePulse(const INT32 code, const INT32 value);

private:
	/**
	 * Caontains a list of body styles and their corresponding drive axle.
	 * @since Version 1.0
	 */
	XmlNodeMap  m_bodyStyleDriveAxle;
	/**
	 * Tag to use for rear wheel drive selection.
	 * @since Version 1.0
	 */
	std::string m_rearWheelDriveTag;
	/**
	 * Tag to use for front wheel drive selection.
	 * @since Version 1.0
	 */
	std::string m_frontWheelDriveTag;
	/**
	 * Tag to use for four wheel drive selection.
	 * @since Version 1.0
	 */
	std::string m_fourWheelDriveTag;
	/**
	 * Tag to use for automatic transmission selection.
	 * @since Version 1.0
	 */
	std::string m_autoTransTag;
	/**
	 * Tag to use for manual transmission selection.
	 * @since Version 1.0
	 */
	std::string m_manualTransTag;
	/**
	 * Stores the prompt tag for "Requesting Information"
	 * @since Version 1.0
	 */
    std::string m_requestingInfoPrompt;
	/**
	 * Stores the prompt tag for "No Information"
	 * @since Version 1.0
	 */
	std::string m_noInfoPrompt;
	/**
	 * Stores the prompt tag for "Information Error"
	 * @since Version 1.0
	 */
	std::string m_infoErrorPrompt;
	/**
	 * Store the prompt tag for "Bad Checksum".
	 * @since Version 1.0
	 */
	std::string m_badCheckSumPrompt;
	/**
	 * Stores the prompt tag for "Unknown Message".
	 * @since Version 1.0
	 */
	std::string m_unknownMessagePrompt;
	/**
	 * Prompt to use if a NAK is received from the Work Cell.
	 * @since Version 1.0
	 */
	std::string m_wccNAKPrompt;
	/**
	 * Prompt for no response from broadcast.
	 * @since Version 1.1
	 */
	std::string m_wccNoResponsePrompt;
	/**
	 * VIN data to be retrieved.
	 * @since Version 1.2
	 */
	std::string m_loadVIN;
	/**
	 * Start index for the current time in the response from broadcast.
	 * @since Version 1.0
	 */
	INT32 m_wccTimeIndex;
	/**
	 * Length of the time string in the response from broadcast.
	 * @since Version 1.0
	 */
	INT32 m_wccTimeLength;
	/**
	 * start index for the current date in the broadcast response.
	 * @since Version 1.0
	 */
	INT32 m_wccDateIndex;
	/**
	 * Length of the date string in the response from broadcast.
	 * @since Version 1.0
	 */
	INT32 m_wccDateLength;
	/**
	 * Index to the checksum in the string from broadcast.
	 * @since Version 1.0
	 */
	INT16 m_wccChecksumIndex;
	/**
	 * Maximum number of times to request data from broadcast.
	 * @since Version 1.0
	 */
	INT32 m_maxAttempts;
    /**
	 * Time to wait in ms between failed attempts to send a response to the external workcell controller.
	 */
	INT32 m_messageDelay;
	/**
	 * Maximum amount of time to wait for a response from broadcast.
	 * @since Version 1.0
	 */
	INT32 m_maxResponseTime;
	/**
	 * The minimum length of a valid message.
	 * @since Version 1.0
	 */
	UINT32 m_validMessageLength;
	/**
	 * The minimum length of a valid message.
	 * @since Version 1.0
	 */
	UINT32 m_validWccResponseLength;
	/**
	 * Setup node for the serial channel to the MesDataController system.
	 * @since Version 1.0
	 */
	XmlNode *m_wccCommSetup;
	/**
	 * Serial Communication object to use for communicaion with the Plant broadcast system
	 * @since Version 1.0
	 */
	SerialChannel  m_wccComm;
    /**
     * TestResultServer interface used by the component to report
     * the results to each of the commanded tests.
     * @since Version 1.0
     */
    ITestResultServer *m_testResult;
	/**
	 * Flag to determine if Workcell controller
	 * is suppose to format and sent a test result
	 * to the GUI in order to enable it to print
	 * out a page for a Pfs vehicle block.
	 */
	bool m_printOnPfsBlock;
    /**
     * Flag to indicate if section length data should be used from the message.
     */
    bool m_useSectionData;

    bool m_requestBuildInfo;
    /**
     * Map containing the build record section data.
     */
    XmlNodeMap m_wccSectionMap;
    /**
     * Map containing the build record section length data.
     */
    XmlNodeMap m_wccSectionLengthMap;

    XmlNodeMap m_wccDataNodeMap;

    INT32 m_reconnectDelay;
    /**
     * Interface to the fault server
     */
    IFaultServer        *m_faultServer;

    /**
     * Reference to the fault information (Name, Description, ...).
     */
    XmlNode *m_faultInformation;

    bool m_reconnectHasBeenRequested;

	bool m_reconnectOnNoResponse;
    /**
     * Timer for sending keep alive messages.
     */
    BepTimer m_keepAliveTimer;

};

#endif  // MesDataController_h

