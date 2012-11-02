//******************************************************************************
// Description:
//   This defines the PlantHostInbound object.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ServicesLayer/Servers/PlantHostInbound/Include/PlantHostInbound.h $
// 
// 1     3/09/06 3:16p Gswope
// After changes to bring HMMA up to "current core"
// 
// 1     1/18/05 10:47a Gswope
// Initial Entry in this project in SourceSafe
// 
// 4     11/29/04 10:11a Bmeinke
// Removed the const qualifier from the GetInputVinLength() method
// 
// 3     11/24/04 11:52a Bmeinke
// Added TYPE_LITERAL_CODE build string field type definition. This is
// similar to the TYPE_CODE, except the field from the build string is
// appended onto "Type" verbatim when looking up the child translation
// node with no atoi() conversion (i.e. TypeEMF for HMMA would give a
// BofyStyle of "NF")
// Added GetInputVinLength() method to define the number of characters
// expected in a VIN from the scanner/pendant. Read from
// <DataTags>/<InputVinLength> value. Defaults to 17 (vinlength) if not
// defined in the config file.
// 
// 1     8/30/04 7:25p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
// 
// 1     7/21/04 10:18a Cward
//
// 3     3/04/04 5:17p Cward
// Added new methods and ivars
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     1/27/04 4:53p Cward
//
// 1     8/21/03 9:08a Derickso
// 05WK CCRT Project
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 4:19p Khykin
// Initial checkin.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.3  2003/02/23 17:13:12  tbochene
//    Changed vin legth back to 17
//
//    Revision 1.2  2003/02/20 15:50:18  tbochene
//    This is hacked to work, must change back after scanner is programmed
//
//    Revision 1.1.1.1  2003/02/15 00:05:44  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:08a Khykin
// Initial Checkin
//
// 1     1/18/03 8:05a Khykin
// Initial Checkin From DCTC
//
// 13    10/22/02 3:49p Derickso
// Added next vin tag set and get methods.
//
// 12    10/21/02 12:25p Derickso
// Check if maps are empty before attempting to clear.
//
// 11    8/14/02 12:07p Skypig13
// Updated to remove compiler warnings
//
// 10    7/30/02 11:32a Dan.erickson
// Default to SOURCE_FILE if an invalid vehicle build record source is
// specified.
//
// 9     6/07/02 12:03p Dan.erickson
// Added VINDisplay tag management methods.
//
// 8     6/03/02 1:01p Dan.erickson
// Wheelbases are now kept in a seperate config file. The format
// of the file is generic and compatible with all manufacturers.
//
// 7     5/06/02 3:49p Dan.erickson
// Corrected memory leak.
//
// 6     4/17/02 9:11a Dan.erickson
// Made TranslateBuildRecord() virtual.
//
// 5     4/16/02 2:45p Dan.erickson
// Added set and get methods for the VINReadStatus tag.
//
// 4     4/08/02 2:52p Dan.erickson
// Switch Set methods to use DeepCopy instead of addNode.
//
// 3     4/08/02 2:28p Dan.erickson
// Made destructor virtual.
// Updated ClearVehicleBuild() method to also set the status of
// the vehicle build to Invalid.
// Updated ClearMaps() method to clear all maps that are used.
// Updated methods that set the map contents to add each child
// explicitly instead of using getChildren to assign the map contents.
//
// 2     4/03/02 3:45p Dan.erickson
// Promoted LoadDataTags() to be protected instead of private.
//
// 1     4/03/02 2:05p Dan.erickson
// Created.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef PlantHostInbound_h
#define PlantHostInbound_h
//==============================================================================

#include "BepServer.h"
#include "INamedDataBroker.h"


/**
 * Value signifying the retaining rolls have been lowered.
 * @since Version 1.1
 */
const std::string retainingRollsDown("1");
/**
 * Value signifying a vehicle is present.
 * @since Version 1.1
 */
const std::string vehicleIsPresent("1");
/**
 * Value signifying a Valid status.
 * @since Version 1.1
 */
const std::string validStatus("Valid");
/**
 * Length of the Vin
 * @since Version 1.1
 */
const UINT16 vinLength = 17;
const UINT16 minVinLength = 8;
const UINT16 uniqueVinStart = 9;


/**
 * Start Index tag.
 * @since Version 1.0
 */
#define START_INDEX               "StartIndex"
/**
 * Length tag.
 * @since Version 1.0
 */
#define FIELD_LENGTH              "Length"
/**
 * Type tag
 * @since Version 1.0
 */
#define FIELD_TYPE                "Type"
/**
 * Tag to use for the Perform/Abort test field.
 * @since Version 1.0
 */
#define PERFORM_ABORT_TEST        "PerformAbortTest"
/**
 * Type attribute definitions for field type.
 * @since Version 1.0
 */
#define TYPE_CODE                 "Code"
#define TYPE_VALUE                "Value"
#define TYPE_DETAIL               "Detail"
#define TYPE_LITERAL_CODE         "LiteralCode"
/**
 * Valid source designations for the vehicle build record.
 * @since Version 1.0
 */
#define SOURCE_FILE               "File"
#define SOURCE_BROADCAST          "Broadcast"


/**
 * PlantHostInbound object.
 * This class is responsible for translating broadcast data to a format usable by
 * the system.  It only handles data coming from the plant broadcast system into
 * the Burke Porter system.
 *
 * @author Dan Erickson
 * @version Version 1.3
 * @since Version 1.0
 */
class PlantHostInbound : public BepServer
{
public:
	/**
	 * Class constructor
	 * @since Version 1.0
	 */
	PlantHostInbound();
	/**
	 * Class destructor.
	 * @since Version 1.0
	 */
	virtual ~PlantHostInbound();
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
	 * Method used to set all of the objects in the component
	 * to their original states.  This would include all
	 * member variables such as buffers, lists, and stored
	 * data.
	 * @since Version 1.0
	 */
	virtual void Reset(void);
	/**
	 * Request to read a data tag
	 *
	 * @param dataNode    Data to be read
	 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
	 * @return Value of tag
	 * @since Version 1.0
	 */
	virtual const std::string Read(const std::string &tag, const INT32 rate=0);
	/**
	 * Request to read a data tag
	 *
	 * @param dataNode    Data to be read
	 * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
	 * @return Value of tag
	 * @since Version 1.0
	 */
	virtual const std::string Read(const XmlNode *dataNode, const INT32 rate=0);
	/**
	 * Request to write a data tag
	 *
	 * @param tag    Tag to write.
	 * @param value  Data to write
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 1.0
	 */
	virtual const std::string Write(const std::string &tag, const std::string &value);
	/**
	 * Request to write a data tag
	 *
	 * @param dataNode  Data to be written
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 1.0
	 */
	virtual const std::string Write(const XmlNode *dataNode);
	/**
	 * Reloads the configuration file and resets the
	 * base components objects.
	 * @since Version 1.0
	 */
	virtual void ReloadConfiguration(void);
	/**
	 * Get the source of the vehicle build record.
	 *
	 * @return Source of vehicle build (File, Broadcast).
	 * @since Version 1.0
	 */
	const std::string GetVehicleBuildSource(void);

	/**
	 * Return the path of the files containing the vehicle build records.
	 *
	 * @return Path of vehicle build record file.
	 * @since Version 1.0
	 */
	const std::string GetVehicleBuildFilePath(void);
	
	/**
	 * Return the vehicle build tag.
	 *
	 * @return Vehicle build tag.
	 * @since Version 1.0
	 */
	const std::string GetVehicleBuildTag(void);
	
	/**
	 * Return the tag being used for vehicle present.
	 *
	 * @return Name of the tag.
	 * @since Version 1.1
	 */
	const std::string GetVehiclePresentTag(void);

	/**
	 * Return the BuildRecord tag.
	 *
	 * @return BuidlRecord tag.
	 * @since Version 1.0
	 */
	const std::string GetBuildRecordTag(void);

	/**
	 * Return the vin tag.
	 *
	 * @return Vin tag.
	 * @since Version 1.0
	 */
	const std::string GetVinTag(void);

	/**
	 * Return the tag being used for the next vin.
	 *
	 * @return Name of the tag.
	 * @since Version 1.3
	 */
	const std::string GetNextVinTag(void);

	/**
	 * Return the name of the vin display tag.
	 *
	 * @return Name of the tag.
	 * @since Version 1.2
	 */
	const std::string GetVinDisplayTag(void);

	/**
	 * Return the tag being used for the vin read status.
	 *
	 * @return Name of the tag.
	 * @since Version 1.1
	 */
	const std::string GetVinReadStatusTag(void);

	/**
	 * Return the tag being used for retaining rolls down.
	 *
	 * @return Name of the tag.
	 * @since Version 1.1
	 */
	const std::string GetRetainersDownTag(void);

	/**
	 * Return the tag being used for vehicle build record status.
	 *
	 * @return Name of the tag.
	 * @since Version 1.1
	 */
	const std::string GetVehicleBuildRecordStatusTag(void);

	/**
	 * Return the name of the tag being used for the state of the InputServer.
	 *
	 * @return  Name of the tag.
	 * @since Version 1.1
	 */
	const std::string GetInputServerStateTag(void);

	/**
	 * Return the status of our internal vehicle build record.
	 *
	 * @return Status (Valid, Invalid)
	 * @since Version 1.1
	 */
	const std::string GetVehicleBuildRecordStatus(void);

	/**
	 * Get the name of the wheelbase position tag.
	 *
	 * @return Name of the tag.
	 * @since Version 1.2
	 */
	const std::string GetWheelbasePositionTag(void);

	/**
	 * Add any information that was derived from the vehicle build to the vehicle build record.
	 *
	 * @since Version 1.0
	 */
	virtual void AddDerivedBuildInfo(void);

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
	 * Check if the vehicle build data should be invalidated when
	 * read from the server.
	 *
	 * @return True - the vehicle build record will be invalidated when read.
	 * @since Version 1.5
	 */
	virtual const bool &InvalidateBuildRecordOnRead(void);
	/**
	 * Communication object to talk to the NamedDataBroker.
	 * @since Version 1.0
	 */
	INamedDataBroker  *m_broker;

protected:
	/**
	 * Handle the subscribed for data by the process.
	 *
	 * @param node    Tag that was subscribed for
	 * @return Status of the updating of the published tag
	 *         Success, Fail
	 * @since Version 1.0
	 */
	virtual const std::string Publish(const XmlNode *node);
	/**
	 * Reads the vehicle build record from the plant broadcast system.  This will be specific to each
	 * manufacturer so no base implementation is provided.
	 *
	 * @param vin    Vin to request build record for.
	 * @param buildRecord
	 *               The build record for the vehicle.
	 * @return Status of the operation (Success, Failure, Not Implemented...)
	 * @since Version 1.1
	 */
	virtual const std::string ReadBuildRecordFromBroadcast(const std::string &vin, std::string &buildRecord);

	/**
	 * Update the state of the InputServer if it needs to be changed.
	 * @since Version 1.1
	 */
	virtual void UpdateInputServerState(void);
	/**
	 * Clears the data sent to the server from the data structure containing it
	 * @since Version 1.0
	 */
	virtual void ClearData(void);
	/**
	 * Translate the build string read from broadcast to an XML representation.
	 *
	 * @param buildRecord
	 *               Vehicle build record from broadcast.
	 * @return Xml representation of build record.
	 * @since Version 1.0
	 */
	virtual void TranslateBuildRecord(const std::string &buildRecord);
	/**
	 * Get the vehicle build record for the specofied vin.
	 *
	 * @param vin    Vehicle Identifier.
	 * @return Status of the load operation.
	 * @since Version 1.0
	 */
	const std::string LoadVehicleBuildRecord(const std::string &vin);
	/**
	 * Returns the build record for the requested vin.
	 *
	 * @param vin    Vin of the vehicle to be tested.
	 * @return Status of the operation (Success, Unavailable,...)
	 * @since Version 1.1
	 */
	const std::string LoadVehicleBuildFromFile(const std::string &vin);
	/**
	 * Gets the vehicle build record from broadcast for the requested vin.
	 *
	 * @param vin    Vehicle to be tested.
	 * @return Status of the operation(Success, Unavailable,...).
	 * @since Version 1.1
	 */
	const std::string LoadVehicleBuildFromBroadcast(const std::string &vin);
	/**
	 * Saves the vehicle build information for the vehicle that was just scanned.
	 *
	 * @param buildRecord
	 *               The vehicle build information to be saved.
	 * @since Version 1.0
	 */
	 inline void SetVehicleBuild(const XmlNode *buildRecord);

	 /**
	  * Adds an item to the vehicle build information.
	  *
	  * @param item   Build item to be added.
	  * @param value  Value of item to be added.
	  * @since Version 1.0
	  */
	 inline void AddVehicleBuildItem(const std::string &item, const std::string &value);

	 /**
	  * Return the value of a vehicle build item.
	  *
	  * @param item   Item to get the value for.
	  * @return The value of the requested build item or Unavailable if the item is not part of the build info.
	  * @since Version 1.0
	  */
	 const std::string GetVehicleBuildItem(const std::string &item);

	 /**
	  * Save the vehicle build record map.
	  *
	  * @param buildMap Build record mapping.
	  * @since Version 1.0
	  */
	 void SetBuildRecordMap(const XmlNode *buildMap);

	 /**
	  * Set the source of the vehicle build record.
	  *
	  * @param source Source of the vehicle build record.
	  * @since Version 1.0
	  */
	 void SetVehicleBuildSource(const std::string &source);

	 /**
	  * Store the wheel base positions.
	  *
	  * @param wheelBasePositions
	  *               List of wheel base positions and what data selects the wheelbase, body style for example.
	  * @since Version 1.0
	  */
	 void SetWheelBasePositions(const XmlNode *wheelBasePositions);

	 /**
	  * Store the data that indicate the vehicle is equipped with All Wheel drive.
	  *
	  * @param allWheelDriveValues
	  *               Node containing the data inidicating All wheel drive equipped.
	  * @since Version 1.0
	  */
	 void SetAllWheelDriveValues(const XmlNode *allWheelDriveValues);

	 /**
	  * Store the data that indicate the vehicle is not equipped with four wheel drive.
	  *
	  * @param fourWheelDriveValues
	  *               Node containing the data indicating not equipped.
	  * @since Version 1.0
	  */
	 void SetFourWheelDriveValues(const XmlNode *fourWheelDriveValues);

	 /**
	  * Set the name of the vehicle build file path to use.
	  *
	  * @param filePath Path of the files containing the vehicle build records.
	  * @since Version 1.0
	  */
	 void SetVehicleBuildFilePath(const std::string &filePath);

	 /**
	  * Set the vehicle build tag.
	  *
	  * @param tag Tag being used for vehicle build.
	  * @since Version 1.0
	  */
	 void SetVehicleBuildTag(const std::string &tag);

	 /**
	  * Store the name of the tag being used for vehicle present.
	  *
	  * @param tag    Name of the tag.
	  * @since Version 1.1
	  */
	 void SetVehiclePresentTag(const std::string &tag);

	 /**
	  * Set the build record tag.
	  *
	  * @param tag Tag being used for build record.
	  * @since Version 1.0
	  */
	 void SetBuildRecordTag(const std::string &tag);

	 /**
	  * Set the vin tag.
	  *
	  * @param tag Tag being used for vin.
	  * @since Version 1.0
	  */
	 void SetVinTag(const std::string &tag);

	 /**
	  * Set the next vin tag.
	  *
	  * @param tag Tag being used for next vin.
	  * @since Version 1.3
	  */
	 void SetNextVinTag(const std::string &tag);

	 /**
	  * Store the name of the vin display tag.
	  *
	  * @param tag    Name of the tag.
	  * @since Version 1.2
	  */
	 void SetVinDisplayTag(const std::string	&tag);

	 /**
	  * Set the tag being used for vin read status.
	  *
	  * @param tag    Name of the tag.
	  * @since Version 1.1
	  */
	 void SetVinReadStatusTag(const std::string &tag);

	 /**
	  * Set the tag being used for retaining rolls down.
	  *
	  * @param tag    Name of the tag.
	  * @since Version 1.1
	  */
	 void SetRetainersDownTag(const std::string &tag);

	 /**
	  * Set the name of the tag being used for vehicle build status.
	  *
	  * @param tag    Name of the tag.
	  * @since Version 1.1
	  */
	 void SetVehicleBuildRecordStatusTag(const std::string &tag);

	 /**
	  * Store the name of the tag being used for the state of the InputServer.
	  *
	  * @param tag    Name of the tag.
	  * @since Version 1.1
	  */
	 void SetInputServerStateTag(const std::string &tag);

	 /**
	  * Set the status of our internal vehicle build record.
	  *
	  * @param status Status (Valid, Invalid)
	  * @since Version 1.1
	  */
	 void SetVehicleBuildRecordStatus(const std::string &status);

	 /**
	  * Store the name of the Wheelbase position tag.
	  *
	  * @param tag    Name of the wheelbase position tag.
	  * @since Version 1.2
	  */
	 void SetWheelbasePositionTag(const std::string &tag);

	 /**
	  * Clear the vehicle build record.
	  * @since Version 1.0
	  */
	 void ClearVehicleBuild(void);

	 /**
	  * Clear the mappings.
	  * @since Version 1.0
	  */
	 virtual void ClearMaps(void);

	 /**
	  * Evaluates the criteria to determine which wheelbase position to select.
	  *
	  * @param selectData Data used to determine which wheelbase to select
	  * @return Wheelbase position for the specified selectData.
	  * @since Version 1.0
	  */
	 virtual const std::string GetWheelBasePosition(const std::string &selectData);

	 /**
	 * Stores all the data tags that will be used by a PlantHost object.
	 *
	 * @param setupNode The setup node where all the information is located.
	 * @since Version 1.1
	 */
	virtual void LoadDataTags(const XmlNode *setupNode);

	/**
	 * Returns the number of characters that a scnned/typed VIN 
	 * should be
	 * 
	 * @return The number of characters that a scnned/typed VIN 
	 *         should be
	 */
	virtual int GetInputVinLength();

	/**
	 * Communication object to use to talk to the InputServer object.
	 * @since Version 1.1
	 */
	IBepCommunication  m_inputServerComm;
	/**
	 * List of wheel base positions and body styles.
	 * @since Version 1.0
	 */
	XmlNodeMap   m_wheelBasePositions;
	/**
	 * Contains values to use for FourWheelDrive tag in vehicle build.
	 * @since Version 1.0
	 */
	XmlNodeMap   m_fourWheelDrive;
	/**
	 * Contains values to use for AllWheelDrive tag in vehicle build.
	 * @since Version 1.0
	 */
	XmlNodeMap   m_allWheelDrive;
	/**
	 * The parsed Xml representation of the vehicle build.
	 * @since Version 1.0
	 */
	XmlNodeMap   m_vehicleBuild;
	/**
	 * Name of the tag being used for retainers down.
	 * @since Version 1.1
	 */
	std::string  m_retainersDownTag;

private:
	/**
	 * Store all the data maps that will be used by a PlantHost object.
	 *
	 * @param broadCastNode
	 *               The node containing all the broadcast data.
	 * @since Version 1.1
	 */
	virtual void LoadDataMaps(const XmlNode *broadCastNode);
	/**
	 * Store the flag indicating if the vehicle build record should be
	 * invalidated when read.
	 *
	 * @param invalidateOnRead	True - Vehicle build data will be
	 *							invalidated when read from the server.
	 * @since Version 1.5
	 */
	virtual void StoreInvalidateBuildRecordOnReadFlag(const bool &invalidateOnRead);
	/**
	 * Tag being used for the vehicle build status.
	 * @since Version 1.1
	 */
	std::string  m_vehicleBuildStatusTag;
	/**
	 * Status of our internal build record (Valid, Invalid).
	 * @since Version 1.1
	 */
	std::string  m_vehicleBuildStatus;
	/**
	 * Map of tags and their starting indices in the build record.
	 * @since Version 1.0
	 */
	XmlNodeMap   m_buildRecordMap;
	/**
	 * File to look in for vehicle build records.
	 * @since Version 1.0
	 */
	std::string  m_vehicleBuildFilePath;
	/**
	 * Tag being used for vehicle build.
	 * @since Version 1.0
	 */
	std::string  m_vehicleBuildTag;
	/**
	 * Stores the source for the vehicle build records.
	 * @since Version 1.0
	 */
	std::string  m_vehicleBuildRecordSource;
	/**
	 * Tag being used for vehicle present.
	 * @since Version 1.1
	 */
	std::string  m_vehiclePresentTag;
	/**
	 * Tag being used for build record.
	 * @since Version 1.0
	 */
	std::string  m_buildRecordTag;
	/**
	 * Tag being used for VIN.
	 * @since Version 1.0
	 */
	std::string  m_vinTag;
	/**
	 * Tag being used for the next VIN.
	 * @since Version 1.3
	 */
	std::string  m_nextVinTag;
	/**
	 * Tag being used to report the status of the vin read.
	 * @since Version 1.1
	 */
	std::string  m_vinReadStatusTag;
	/**
	 * Name of the VINDisplay tag
	 * @since Version 1.2
	 */
	std::string	 m_vinDisplayTag;
	/**
	 * Name of the tag being used for InputServer state.
	 * @since Version 1.1
	 */
	std::string  m_inputServerStateTag;
	/**
	 * Name of the tag being used for wheelbase position.
	 * @since Version 1.2
	 */
	std::string  m_wheelbasePositionTag;
	/**
	 * Flag to indicate if the PlantHostInbound object is responsible
	 * updating the VIN read status.
	 * @since Version 1.4
	 */
	bool m_updateVINReadStatus;
	/**
	 * Flag to indicate if the build record status should be invalidated
	 * when read.
	 * @since Version 1.5
	 */
	 bool m_invalidateBuildRecordOnRead;
};

#endif // PlantHostInbound_h
