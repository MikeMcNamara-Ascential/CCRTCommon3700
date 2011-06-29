//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WorkCellController.h 1     12/08/04 3:46p Derickso $
//
// FILE DESCRIPTION:
//   WorkCellController class to handle communications from the DCX
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
//    $Log: /CCRT/Common/ServicesLayer/Servers/PlantHostInbound/WorkCellController/WorkCellController.h $
// 
// 1     12/08/04 3:46p Derickso
// Created new VSS database.
// 
// 1     12/07/04 4:27p Derickso
// Created new VSS database.
// 
// 2     11/01/04 3:25p Derickso
// Updated to provide a printout for PFS aborted vehicles.
// Updated to only set the clock if the time is off the first time a
// vehicle build is scanned in.
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
// 6     6/13/02 7:26a Dan.erickson
// Added code to support the WccRequestHandler thread.
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
#ifndef WorkCellController_h
#define WorkCellController_h
//==============================================================================

#include <errno.h>
#include "BepSync.h"
#include "PlantHostInbound.h"
#include "IPromptServer.h"
#include "SerialChannel.h"
#include "WccRequestHandler.h"
#include "WccKeepAlive.h"
#include "GeneralUtilities.h"
#include "ITestResultServer.h"

class WccRequestHandler;
class WccKeepAlive;

/**
 * Name of the default node for the drive axle selection.
 * @since Version 1.0
 */
#define DEFAULT_NODE    "_Default"
/**
 * Macros to use with broadcast messages.
 * @since Version 1.0
 */
#define ENQ      0x05
#define ACK      0x06
#define CR       0x0D
#define LF       0x0A
#define NO_INFO  0x11
#define DC2      0x12
#define NAK      0x15
#define CLOCK    0xF0
#define CLOCK2   0xF1
/**
 * Constants to use when evaluating responses from broadcast.
 * @since Version 1.0
 */
const INT16 shortBroadcastMessage = 3;
/**
 * PFS block status
 * @since Version 1.0
 */
#define PFS_PERFORM_STATUS "Perform"
#define PFS_ABORT_STATUS   "Abort"
#define PFS_SHIP_STATUS    "Ship"
/**
 * Signals to either retrieve or not retrieve the build record from braodcast.
 * @since Version 1.2
 */
const std::string getBuildRecord("GetBuildRecord");
const std::string doNotGetBuildRecord("DoNotGetBuildRecord");


/**
 * This class handles data from the DaimlerChrysler broadcast system.
 * Vehicle build records are translated into a format usable by the Burke Porter
 * system.  The WorkCellController class also manages the validity of the
 * Vehicle build record and displays prompts if no information or bad information
 * is received from broadcast.
 * 
 * @author Dan Erickson
 * @version Version 1.3
 * @since Version 1.0
 */
class WorkCellController : public PlantHostInbound
{
public:
	/**
	 * Default constructor.
	 * @since Version 1.0
	 */
	WorkCellController();
	/**
	 * Default Destructor
	 * @since Version 1.0
	 */
	virtual ~WorkCellController();
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
     * Request to read a data tag
     *
     * @param dataNode    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     */
    virtual const string Read(const std::string &tag, const INT32 rate=0);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     */
    virtual const string Read(const XmlNode *node, const INT32 rate = 0);
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
     * The Baumuster from the vehicle build will be used to load the parameter file for the current vehicle type.
     * The parameter file will then be added to the vehicle build data.
     */
    void LoadVehicleParameterFile(void);
	/**
	 * Add any information that was derived from the vehicle build to the vehicle build record.
	 * 
     * @param buildData Build data map to add the information to.
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
	 * Utility method to get a value from a list.
	 * The data item specified will be looked up in the provided list and the value from the list will be returned.
	 * If the requested data item cannot be found, the default value from the list will be returned to the calling function.
	 * 
	 * @param dataItem Data item to be looked up in the list.
	 * @param itemList Data list to look for the requested data item.
	 * 
	 * @return Value found in the list for the requested data item or the default value if the requested data item was not found.
	 */
	const string GetDataItemFromList(const string &dataItem, const XmlNodeMap *itemList);
	/**
	 * Get the index for the provided body dtyle.  This data will be sent to the PLC and forwarded onto the FIS system.
	 * 
	 * @param bodyStyle Body style of the current vehicle.
	 * 
	 * @return Index associated with the current body style.
	 */
	const string GetBodyStyleIndex(const string &bodyStyle);
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
	/**
	 * Get the tag used to set the air pressure for the adjustable regulator.
	 * 
	 * @return Adjustable regulator air pressure tag.
	 */
	const string& GetAirPressureTag(void);
	/**
	 * Return the prompt tag for "No Information".
	 * 
	 * @return Prompt Tag
	 * @since Version 1.0
	 */
	const std::string GetNoInfoPrompt(void);
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
	 * Get the prompt being used for Blocked by PFS.
	 * 
	 * @return Prompt tag.
	 * @since Version 1.0
	 */
	const std::string GetPfsBlockPrompt(void);
	/**
	 * Get the prompt being used for PFS Abort status.
	 * 
	 * @return Prompt tag.
	 * @since Version 1.0
	 */
	const std::string GetPfsAbortPrompt(void);
	/**
	 * Get the prompt being used for Vehicle in Ship status.
	 * 
	 * @return Prompt tag.
	 * @since Version 1.0
	 */
	const std::string GetPfsShipPrompt(void);
	/**
	 * Get the background color to use for the PFS Abort message.
	 *
	 * @return PFS abort message background color.
	 * @since Version 1.3
	 */
	const string& GetPfsAbortColor(void);
	/**
	 * Get the background color to use for the PFS Ship message.
	 *
	 * @return PFS ship message background color.
	 * @since Version 1.3
	 */
	const string& GetPfsShipColor(void);
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
	 */
	const INT16 GetChecksumIndex(void);
	/**
	 * Return the number of attempts to make before failing.
	 * 
	 * @return Number of attempts.
	 */
	const INT32 GetMaxAttempts(void);
	/**
	 * Get the maximum amount of time to wait for a response from broadcast.
	 * 
	 * @return Maximum amount of time to wait.
	 */
	const INT32 GetMaxResponseTime(void);
	/**
	 * Return the length of valid messages recieved from broadcast.
	 * 
	 * @return Length of a valid message.
	 */
	const UINT32 GetValidMessageLength(void);
	/**
	 * Return the length of valid messages recieved from broadcast.
	 * 
	 * @return Length of a valid message.
	 */
	const UINT32 GetValidWccResponseLength(void);
    /**
	 * Retrieve the section lengths from the message.
	 * 
	 * @param buildMessage
	 *               Build record message received from the WCC.
	 */
	void StoreSectionLengths(const string &buildMessage);
	/**
	 * Determines if the retaining rolls are down.
	 * 
	 * @return True  - Retaining rolls are down
	 *         False - Retaining rolls are up
	 */
	const bool AreRetainersDown(void);
	/**
	 * Return the setup being used for the serial channel communications to the plant broadcast system.
	 * 
	 * @return Serial channel setup data.
	 */
	const XmlNode* GetBroadcastCommSetup(void);
	/**
	 * Return the configuration information for the request handler.
	 * 
	 * @return Configuration information.
	 */
	const XmlNode* GetRequestHandlerConfig(void);
	/**
	 * Get the VIN to retrieve.
	 * 
	 * @return VIN for Build record.
	 */
	const std::string GetVINToLoad(void);
	/**
	 * Lock the port that is passed into this function.
	 * 
	 * @param commObject Comm port to be loacked.
	 * 
	 * @return Status of locking the port.
	 *         True - The port was locked.
	 *         False - The port could not be locked.
	 */
	bool LockWccCommPort(SerialChannel *commObject);
	/**
	 * Unlock the comm port for the provided comm object.
	 * 
	 * @param commObject Comm object to unlock the port for.
	 * 
	 * @return Lock status of the port.
	 *         True - the comm port is still locked.
	 *         False - the comm port is unlocked.
	 */
	bool UnlockWccCommPort(SerialChannel *commObject);
	/**
	 * Command any threads to ignore incoming data.
	 * 
	 * @param ignoreIncomingData
	 *               Flag to either ignore incoming data ro to process the incoming data.
	 *               True - ignore incoming data.
	 *               False - resume processing of incoming data.
	 */
	void IgnoreIncomingData(bool ignoreIncomingData = true);

protected:
	/**
	 * Reads the vehicle build record from the plant broadcast system.  This will be specific to each
	 * manufacturer so no base implementation is provided.
	 * 
	 * @param vin    Vin to request build record for.
     * @param buildRecord
     *               Build record string retrieved from broadcast.
     * @param updateStatus
     *               Flag to indicate if prompts or system status should be updated.
     * 
	 * @return The vehicle build record returned from the plant broadcast system.
	 */
    virtual const std::string ReadBuildRecordFromBroadcast(const std::string &vin, 
                                                           std::string &buildRecord, 
                                                           const bool &updateStatus);
    /**
     * factored from above. allows command to be specified
     * 
     * @param command     first byte of message to WCC
     * @param vin         Vin to request build record for. May also contain post vin bytes
     * @param buildRecord Where to store the build record
     * @param updateStatus
     *               Flag to indicate if prompts or system status should be updated.
     * 
     * @return The vehicle build record returned from the plant broadcast system.
     */
    virtual const std::string ReadBuildRecordFromBroadcast(const std::string &command, 
                                                           const std::string &vin, 
                                                           std::string &buildRecord, 
                                                           const bool &updateStatus);
	/**
	 * Get the response from our request for data.
	 * 
	 * @param response The response from broadcast
	 * @param vin    Vin that is being requested.
	 * @since Version 1.0
	 */
    virtual BEP_STATUS_TYPE GetBroadcastResponse(std::string &response, const std::string &vin, const bool &updateStatus);
	/**
	 * Evaluate the response that was received from broadcast.
	 * 
	 * @param response The response from broadcast.
     * @param updateStatus
     *                 Flag to indicate if system status or prmpts should be updated
     * 
	 * @return Status of the evaluation.
	 */
    virtual const BEP_STATUS_TYPE EvaluateResponse(std::string &response, const std::string &vin, const bool &updateStatus);
	/**
	 * Translate the build string read from broadcast to an XML representation.
	 * 
	 * @param buildRecord
	 *               Vehicle build record from broadcast.
     * @param buildData Place to store the build data retrieved.
     * @param updateStatus
     *                  Flag to indicate if system status or prompts should be updated.
     * 
	 * @return Xml representation of build record.
	 */
    virtual void TranslateBuildRecord(const std::string &buildRecord, XmlNodeMap &buildData, const bool &updateStatus);
	/**
	 * Parse the sales codes to determine how the vehicle is equipped.
	 * The vehicle build translation will be used to convert sales codes into options and to
	 * set any necessary testing limits or values.
	 * 
	 * @param buildRecord
	 *               Build record data retrieved from the plant host system.
	 */
	void ParseSalesCodes(const string &buildRecord);
	/**
	 * Break the sales code section into its bits and pieces.
	 * 
	 * @param salesCodeSection
	 *               Sales code section of the broadcast string.
	 */
	void ExtractSalesCodeData(const string &salesCodeSection);
	/**
	 * Process the information contained in the supplied build item to create an entry for the vehicle build record.
	 * 
	 * @param buildItem Data to process.
	 */
	void ProcessBuildItem(const XmlNode* buildItem);
	/**
	 * Evaluates the criteria to determine which wheelbase position to select.
	 * Calls GetWheelBasePosition(bodyStyle, false)
	 * 
	 * @param bodyStyle used to determine which wheelbase to select
	 * @return Wheelbase position for the specified selectData.
	 * @since Version 1.0
	 */	
	virtual const std::string GetWheelBasePosition(const std::string &bodyStyle);
	/**
	 * Evaluates the criteria to determine which wheelbase position to select.
	 * Calls GetWheelBasePosition(bodyStyle, true)
	 * 
	 * @param bodyStyle used to determine which wheelbase to select
	 * @return Wheelbase position for the specified selectData.
	 * @since Version 1.0
	 */
	virtual const std::string GetTandemWheelBasePosition(const std::string &bodyStyle);
	/**
	 * Evaluates the criteria to determine which wheelbase position to select.
	 * 
	 * @param bodyStyle used to determine which wheelbase to select
	 * @param isTandemWheelbase used to determine whether to return the tandem or primary wheelbase value
	 * @return Wheelbase position for the specified selectData.
	 * @since Version 1.0
	 */
	virtual const std::string GetWheelBasePosition(const std::string &bodyStyle, bool isTandemWheelbase); 
	/**
	 * Get the air pressure setting to use for this vehicle.
	 * 
	 * @param selectData Data used to select the appropriate pressure setting for the adjustable regulator.
	 * 
	 * @return Air pressure setting for the adjustable regulator (psi X 10).
	 */
	virtual const string GetAirPressureSetting(const string &selectData);
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
	/**
	 * Save the list of body styles and the drive axle for each body style.
	 * 
	 * @param bodyStyleDriveAxle
	 *               List of body styles and the corresponding drive axle.
	 * @since Version 1.0
	 */
	void SetBodyStyleDriveAxle(const XmlNode *bodyStyleDriveAxle);
	/**
	 * Store the air pressure regulator settings for each tire size.
	 * 
	 * @param airPressures
	 *               Node containing the air pressure regulator settings for each tire size.
	 */
	void StoreTireSizeAirPressures(const XmlNode *airPressures);
	/**
	 * Store the minimum air pressure to be used for the adjustable regulator.
	 * 
	 * @param minPressure
	 *               Minimum air pressure.
	 */
	void SetMinimumAirPressure(const float &minPressure);
	/**
	 * Store the maximum air pressure to be used for the adjustable regulator.
	 * 
	 * @param maxPressure
	 *               Maximum air pressure.
	 */
	void SetMaximumAirPressure(const float &maxPressure);
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
	/**
	 * Store the tag being used to set the adjustable regulator air pressure.
	 * 
	 * @param tag    System tag to use for setting the pressure regulator.
	 */
	void SetAirPressureTag(const string &tag);
	/**
	 * Store the prompt tag for "No Information".
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetNoInfoPrompt(const std::string &prompt);
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
	void SetPfsBlockPrompt(const std::string &prompt);
	/**
	 * Set the prompt being used for PFS Abort status.
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetPfsAbortPrompt(const std::string &prompt);
	/**
	 * Set the prompt being used for Vehicle in Ship status.
	 * 
	 * @param prompt Prompt tag.
	 * @since Version 1.0
	 */
	void SetPfsShipPrompt(const std::string &prompt);
	/**
	 * Store the background color to use for the PFS Abort message.
	 *
	 * @param color  Background color to use for the prompt box.
	 * @since Version 1.3
	 */
	void SetPfsAbortColor(const string &color);
	/**
	 * Store the background color to use for the PFS Ship message.
	 *
	 * @param color  Background color to use for the prompt box.
	 * @since Version 1.3
	 */
	void SetPfsShipColor(const string &color);
	/**
	 * Set the prompt to use if a NAK is recevied from the Work Cell.
	 * 
	 * @param prompt Prompt tag.
	 */
	void SetNAKPrompt(const std::string &prompt);
	/**
	 * Set the prompt to usse if no response is sent from 
	 * the external workcell controller.
	 * 
	 * @param prompt Prompt tag.
	 */
	void SetNoResponsePrompt(const std::string &prompt);
	/**
	 * Save the index for the time field in the build record from broadcast.
	 * 
	 * @param index  Index.
	 */
	void SetTimeIndex(const INT32 &index);
	/**
	 * Save the index for the time field in the build record from broadcast.
	 * 
	 * @param index  Node containing the location information of the time data
	 */
	void SetTimeIndex(const XmlNode *timeIndex);
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
	void SetDateIndex(const INT32 &index);
	/**
	 * Store the index of the date field in the broadcast build record.
	 * 
	 * @param index  Niode containing the location information for the date data.
	 */
	void SetDateIndex(const XmlNode* dateIndex);
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
	void SetChecksumIndex(const INT16 &index);
	/**
	 * Store the location information for the WCC checksum.
	 * 
	 * @param checksumInfo
	 *               Node containing the checksum location information from the WCC.
	 */
	void SetChecksumIndex(const XmlNode *checksumInfo);
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
	void SetMaxResponseTime(const INT32 &maxTime);
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
	 * Store the configuration data for the request handler.
	 * 
	 * @param config Configuration data.
	 */
	void SetRequestHandlerConfig(const XmlNode *config);
	/**
	 * Store the configuration for the keep alive thread.
	 *
	 * @param  config  Configuration data for the keep alive thread.
	 */
	void SetKeepAliveConfig(const XmlNode *config);
	/**
	 * Get the keep alive configuration data for the keep alive thread.
	 */
	const XmlNode* GetKeepAliveConfig(void);
	/**
	 * Store the VIN for the build record to be retrieved
	 * 
	 * @param vin    VIN to be retrieved.
	 */
	void SetVINToLoad(const std::string &vin);
	/**
	 * Semaphore to signal us to go get the build record from broadcast.
	 */
	BepCondVar<std::string, false> m_getBuildRecord;

private:
	/**
	 * Calculate the starting index of the requested section in the build record.
	 * 
	 * @param section Section to calcualte the starting index for.
	 * 
	 * @return Overall position in the build record for the start of the requested section.
	 */
	const INT32 CalculateSectionStartIndex(const string &section);
	/**
	 * Get the start index for the requested field.
	 * 
	 * @param node   Field to get the start index for.
	 * 
	 * @return Start index for the field requested.
	 */
	virtual INT32 GetStartIndex(const XmlNode *node);
	/**
	 * Check the data location to ensure it is completely in the data string being parsed.
	 * 
	 * @param field  Build data information used to extract data from the message.
	 * @param messageLength
	 * 
	 * @return Flag indicating if the data location is valid.
	 *         <ul>
	 *         <li> True - Data field is within the string being parsed. </li>
	 *         <li> False - The data field does not fall within in the string being parsed. </li>
	 *         </ul>
	 */
	virtual bool IsStartIndexValid(const XmlNode *field, const INT32 &messageLength);
	/**
	 * Get the length of the requested section.
	 * 
	 * @param section Section to get the length for.
	 * 
	 * @return Length of the requested section.
	 */
	INT32 GetSectionLength(const string &section);
	/**
	 * Check or set the flag concerning if a keep alive thread should be started.
	 * If the startKeepAlive parameter is NULL, the flag will just be returned to the caller.  If a value
	 * is specified, the flag will be set to that value and then returned to the caller.
	 *
	 * @param startKeepAlive  State to set the keep alive flag to.
	 *						  True - Start the keep alive thread.
	 * @return  Value of the keep alive flag.
	 *			True - Start the keep alive thread.
	 */
	const bool& StartKeepAlive(const bool *startKeepAlive = NULL);
	/**
	 * Check or set the flag concerning if a request handler thread should be started.
	 * If the startRequestHandler parameter is NULL, the flag will just be returned to the caller.  If a value
	 * is specified, the flag will be set to that value and then returned to the caller.
	 *
	 * @param startRequestHandler  State to set the request handler flag to.
	 *						  	   True - Start the request handler thread.
	 * @return  Value of the request handler flag.
	 *			True - Start the request handler thread.
	 */
	const bool& StartRequestHandler(const bool *startRequestHandler = NULL);
	/**
	 * Check or set the flag indicating if the section length data from the message should be used instead of
	 * the configured indicies.
	 * 
	 * @param useSectionData
	 *         Flag to determine if section data should be used to determine data locations.
	 *         <ul>
	 *         <li>True - Use the section data from the message.</li>
	 *         <li>False - Use the configured indicies from the config files.</li>
	 *         </ul>
	 * 
	 * @return Flag indicating how to get data from the build information.
	 *         <ul>
	 *         <li>True - Use the section data from the message to determine data locations.</li>
	 *         <li>False - Use the configured indicies from the config files to find the data.</li>
	 *         </ul>
	 */
	const bool& UseSectionLengthData(const bool *useSectionData = NULL);
	/**
	 * Check if broadcast is sending sales codes or the typical data string.
	 * 
	 * @param useSalesCodes
	 *               Flag to indicate if sales codes are to be used instead of typical data string.
	 *               <ul>
	 *               <li> True  - Use sales codes for build data. </li>
	 *               <li> False - Use the typical broadcast data strnig for build data. </li>
	 *               </ul>
	 * 
	 * @return Flag indicating type of build data being used.
	 *         <ul>
	 *         <li> True  - Sales codes will be used for build data. </li>
	 *         <li> False - The typical broadcast string will be used for build data. </li>
	 *         </ul>
	 */
	const bool& UseSalesCodes(const bool *useSalesCodes = NULL);
	/**
	 * Store the length of the sales code field
	 * 
	 * @param length Length of the sales code field.
	 */
	void SetSalesCodeFieldLength(INT32 length);
	/**
	 * Get the length of the sales code field.
	 * 
	 * @return Length of the sales code field.
	 */
	const INT32& GetSalesCodeFieldLength(void);
	/**
	 * Store the length of the sales code description field.
	 * 
	 * @param length Length of the sales code description field.
	 */
	void SetDescriptionFieldLength(INT32 length);
	/**
	 * Get the length of the sales code description field.
	 * 
	 * @return Length of the sales code description field.
	 */
	const INT32& GetDescriptionFieldLength(void);
	/**
	 * Store the index of the VIN in the broadcast data.
	 * 
	 * @param index  Index of the VIN.
	 */
	void SetVinIndex(const INT32 index);
	/**
	 * Store the length of the VIN in the broadcast data.
	 * 
	 * @param index  Length of the VIN.
	 */
	void SetVinLength(const INT32 length);
	/**
	 * Get the index of the VIN in the broadcast data.
	 * 
	 * @return Index of the VIN field.
	 */
	const INT32& GetVinIndex(void);
	/**
	 * Get the length of the VIN in the broadcast data.
	 * 
	 * @return Length of the VIN field.
	 */
	const INT32& GetVinLength(void);
	/**
	 * Get the name of the tandem wheelbase position tag.
	 *
	 * @return Name of the tag.
	 * @since Version 1.2
	 */
	const string GetTandemWheelbasePositionTag(void);
	/**
	 * Get the value of m_readWheelbaseFromBroadcast 
	 * 
	 */
	bool GetReadWheelbaseFromBroadcast(void);
	/**
	 * Set the value of m_readWheelbaseFromBroadcast 
	 * 
	 */
	void SetReadWheelbaseFromBroadcast(bool tag);
	/**
	 * Get the value of m_readVehicleAxleCountFromBroacast 
	 * 
	 */
	bool GetReadVehicleAxleCountFromBroadcast(void);
	/**
	 * Set the value of m_readVehicleAxleCountFromBroacast
	 * 
	 */
	void SetReadVehicleAxleCountFromBroadcast(bool tag);
	/**
	 * Store the starting index for the sales code section.
	 * 
	 * @param index  Index of the sales code section.
	 */
	void SetSalesCodeStartIndex(const INT32 &index);
	/**
	 * Get the starting index of the sales code section.
	 * 
	 * @return Sales code starting index.
	 */
	const INT32& GetSalesCodeStartIndex(void);

    void SetPortLockedStatus(const bool portStatus);

    const bool GetPortLockedStatus(void);
	/**
	 * Caontains a list of body styles and their corresponding drive axle.
	 */
	XmlNodeMap  m_bodyStyleDriveAxle;
	/**
	 * Index to send to the PLC to indicate what type of vehicle is being tested.
	 * The PLC passes this data to FIS for reporting purposes.
	 */
	XmlNodeMap  m_bodyStyleIndex;
	/**
	 * List of air pressures to use per body style.
	 */
	XmlNodeMap  m_airPressures;
	/**
	 * Tag to use for rear wheel drive selection.
	 */
	std::string m_rearWheelDriveTag;
	/**
	 * Tag to use for front wheel drive selection.
	 */
	std::string m_frontWheelDriveTag;
	/**
	 * Tag to use for four wheel drive selection.
	 */
	std::string m_fourWheelDriveTag;
	/**
	 * Tag to use for automatic transmission selection.
	 */
	std::string m_autoTransTag;
	/**
	 * Tag to use for manual transmission selection.
	 */
	std::string m_manualTransTag;
	/**
	 * Tag to use for setting the air pressure regulator.
	 */
	string m_airPressureTag;
	/**
	 * Stores the prompt tag for "No Information"
	 */
	std::string m_noInfoPrompt;
	/**
	 * Store the prompt tag for "Bad Checksum".
	 */
	std::string m_badCheckSumPrompt;
	/**
	 * Stores the prompt tag for "Unknown Message".
	 * @since Version 1.0
	 */
	std::string m_unknownMessagePrompt;
	/**
	 * Name of the tag being used for wheelbase position.
	 * @since Version 1.2
	 */
	std::string  m_tandemWheelbasePositionTag;
	/**
	 * Prompt for Vehicle Blocked by PFS.
	 * @since Version 1.0
	 */
	std::string m_pfsBlockPrompt;
	/**
	 * Prompt for PFS test status set to Abort.
	 * @since Version 1.0
	 */
	std::string m_pfsAbortPrompt;
	/**
	 * Prompt for PFS test status set to Ship.
	 * @since Version 1.0
	 */
	std::string m_pfsShipPrompt;
	/** 
	 * Background color for the PFS Abort message.
	 * @since Version 1.3
	 */
	string m_pfsAbortColor;
	/** 
	 * Background color for the PFS Ship message.
	 * @since Version 1.3
	 */
	string m_pfsShipColor;
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
	 * Minimum air pressure allowed for the adjustable air regulator.
	 */
	float m_minimumAirPressure;
	/**
	 * Maximum air pressure allowed for the adjustable air regulator.
	 */
	float m_maximumAirPressure;
	/**
	 * Start index for the current time in the response from broadcast.
	 * @since Version 1.0
	 */
	INT32 m_wccTimeIndex;
	/**
	 * Location information for the time data supplied by the WCC.
	 */
	XmlNode *m_wccTimeInfo;
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
	 * Location information for the date data supplied by the WCC.
	 */
	XmlNode *m_wccDateInfo;
	/**
	 * Length of the date string in the response from broadcast.
	 * @since Version 1.0
	 */
	INT32 m_wccDateLength;
	/**
	 * Index to the checksum in the string from broadcast.
	 */
	INT16 m_wccChecksumIndex;
	/**
	 * Location information for the checksum in the string from broadcast.
	 */
	XmlNode *m_wccChecksumInfo;
	/**
	 * Map containing the build record section data.
	 */
	XmlNodeMap m_wccSectionMap;
	/**
	 * Map containing the build record section length data.
	 */
	XmlNodeMap m_wccSectionLengthMap;
	/**
	 * Maximum number of times to request data from broadcast.
	 */
	INT32 m_maxAttempts;
	/**
	 * Maximum amount of time to wait for a response from broadcast.
	 */
	INT32 m_maxResponseTime;
	/**
	 * Start index of the VIN.
	 * This is currently only used with the sales codes setting.
	 */
	INT32 m_vinIndex;
	/**
	 * Length of the VIN.
	 * This is currently only used with the sales codes setting.
	 */
	INT32 m_vinLength;
	/**
	 * Starting index of the sales codes.
	 */
	INT32 m_salesCodeStartIndex;
	/**
	 * Length of the sales code field
	 */
	INT32 m_salesCodeLength;
	/**
	 * Length of the sales code description field.
	 */
	INT32 m_salesCodeDescriptionLength;
	/**
	 * The minimum length of a valid message.
	 */
	UINT32 m_validMessageLength;
	/**
	 * The minimum length of a valid message when using message section lengths.
	 */
	UINT32 m_validWccResponseLength;
    /**
	 * Setup node for the serial channel to the WorkCellController system.
	 */
	XmlNode *m_wccCommSetup;
	/**
	 * Configuration information for the request handler.
	 */
	XmlNode *m_requestHandlerConfig;
	/**
	 * Configuration data for the keep alive thread.
	 */
	XmlNode *m_keepAliveConfig;
	/**
	 * Serial Communication object to use for communicaion with the Plant broadcast system
	 */
	SerialChannel  m_wccComm;
	/**
	 * PromptServer client to use for displaying prompts.
	 */
	IPromptServer  m_promptComm;
	/**
	 * Thread to handle unsolicited requests from the external workcell controller.
	 */
	WccRequestHandler *m_requestHandler;
    /**
	 * Thread to handle sending keep alive messages to the WCC.
	 */
	WccKeepAlive      *m_keepAlive;
    /**
     * TestResultServer interface used by the component to report
     * the results to each of the commanded tests.
     * @since Version 1.0
     */
    ITestResultServer *m_testResult;
	/**
	 * Structure of the sales codes records from broadcast.
	 */
	typedef struct salesCodeRec
	{
		string salesCode;    // Sales Code Item
		string description;  // Description of the item
	} SalesCodeRecord;
	/**
	 * Map to contain the sales codes for the VIN requested from broadcast.
	 */
	typedef map<string, SalesCodeRecord>   SalesCodeList;
	typedef SalesCodeList::iterator        SalesCodeListItr;
	typedef SalesCodeList::value_type      SalesCodeListVal;
	/**
	 * The sales codes extracted from the broadcast data.
	 */
	SalesCodeList  m_salesCodes;
	/**
	 * Flag to determine if Workcell controller
	 * is suppose to format and sent a test result
	 * to the GUI in order to enable it to print
	 * out a page for a Pfs vehicle block.
	 */
	bool m_printOnPfsBlock;
	/**
	 * Flag to determine if the keep alive thread should be started.
	 */
	bool m_startKeepAlive;
	/**
	 * Flag to indicate if the request handler should be started.
	 */
	bool m_startRequestHandler;
	/**
	 * Flag to indicate if section length data should be used from the message.
	 */
	bool m_useSectionData;
	/**
	 * Flag to indicate if broadcast is sending sales codes instead of the typical build data.
	 */
	bool m_useSalesCodes;
	/**
	 * Flag to indicate if wcc port is currently locked.
	 */
	bool m_portLockedStatus;
	/**
	 * Flag to indicate if we should use wheelbase directly from broadcast
	 */
	bool m_readWheelbaseFromBroadcast;
	/**
	 * Flag to indicate if we should use vehicle axle count directly from broadcast
	 */	
	bool m_readVehicleAxleCountFromBroadcast;
				
};

#endif  // WorkCellController_h

