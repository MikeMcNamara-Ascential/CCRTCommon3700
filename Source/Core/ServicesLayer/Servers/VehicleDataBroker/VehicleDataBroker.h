//*************************************************************************
// Description:
//   This defines the VehicleDataBroker object.
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
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/VehicleDataBroker/VehicleDataBroker.h $
// 
// 2     2/07/08 9:58a Derickso
// Updated to support global use bypass part number list tag.
// 
// 1     11/01/07 2:58p Derickso
// Added capability to support a common module fault list.
// 
// 6     10/26/06 1:25p Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 5     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/07/06 11:00a Rwiersem
// Made the test class a friend, removed the old ReloadConfiguration()
// method, added the HandlePulse() method, and added the
// LoadAdditionalConfigurationItems() method.
//
// 4     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
//
// 2     11/15/04 11:29a Bmeinke
// Added a m_useDefDriverNo data meber to say whether or not to use the
// DefaultDriverNumber from the config file when we reset the drive number
// on rolls down or if we should read the current driver number from the
// system
//
// 2     10/24/03 2:01p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
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
//    Revision 1.1.1.1  2003/02/15 00:05:48  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:11a Khykin
// Initial Checkin
//
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
//
// 15    10/22/02 3:52p Derickso
// Added set and get methods for next vehicle build tag.
//
// 14    9/05/02 3:23p Derickso
// Added J2190ActiveVehicleData support.
//
// 13    8/23/02 9:11a Dan.erickson
// Add DriverNumberDisplay items.  Added OverallTest handling.
//
// 12    7/29/02 8:11a Dan.erickson
// Added functionality to gather active data from the vehicle
// modules during testing.
//
// 11    6/03/02 1:05p Dan.erickson
// Added string constant for boolean type PLC items.
//
// 10    4/25/02 4:43p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:03  erickson
//    started
//
//
// 8     4/03/02 1:47p Dan.erickson
// Removed support for VinDisplay tag.  MachineDataBroker will
// now support this tag.
//
// 7     3/27/02 3:31p Dan.erickson
// Added support to invalidate the driver number when the
// retaining rolls are lowered.
// Added support to publish the vin to the GUI.
//
// 6     3/25/02 9:09a Dan.erickson
// Overloaded Initialize(const std::string&) method.
//
// 5     3/22/02 1:36p Dan.erickson
// Removed everything that was dependant on specific tags
// being written to the PLC manager.
//
// 4     2/18/02 1:10p Dan.erickson
// Reworked after restructure of BepServer.
//
// 3     2/06/02 2:25p Admin
// Checking in for testing in the main system.
//
// 2     1/11/02 4:52p Dan.erickson
// Added member variables and accessor and settor methods.
//
// 1     12/06/01 3:16p Dan.erickson
// Created.
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#ifndef VehicleDataBroker_h
#define VehicleDataBroker_h

#include "INamedDataBroker.h"
#include "BepServer.h"
#include <map>

/**
 * Values to set tags to.
 */
#define SET          "1"
#define CLEAR        "0"
#define ROLLS_DOWN   "1"

const std::string booleanType("bool");
/**
 * Communication bus types
 */
const std::string J2190Bus("AvtJ2190");
const std::string ISOKBus("ISO-K");
const std::string SCIBus("SCI");
const std::string CANBus("CAN");

/**
 * Provides the vehicle build record for the vehicle currently being tested.
 *
 * @author Dan Erickson
 * @see BepServer
 */
class VehicleDataBroker : public BepServer
{
    friend class VehicleDataBrokerTest;

public:
    /**
     * VehicleDataBroker Constructor.
     */
    VehicleDataBroker();
    /**
     * VehicleDataBroker destructor.
     */
    virtual ~VehicleDataBroker();
    /**
     * Method for initialization passing in the file name
     *
     * @param fileName Name of the XML configuration file to be used to initialize
     */
    virtual void VehicleDataBroker::Initialize(const std::string &fileName);
    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     */
    virtual const std::string Read(const std::string &tag, const INT32 rate = 0)
    {   // Call the base class functionality
        return(BepServer::Read(tag, rate));
    };
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *         BEP_SUCCESS_RESPONSE == Registered
     */
    virtual const std::string Register(void);
    /**
     * Request to Perform an action
     *
     * @param node    Action to be performed
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     */
    virtual const std::string Command(const XmlNode *node);

    /**
     * Return the name of the plant host process.
     *
     * @return Plant host process name
     */
    inline const std::string& GetPlantHost(void)
    {
        return m_plantHost;
    };
    /**
     * Return the name of the tag being used for StartTest
     *
     * @return Name of StartTest tag.
     */
    inline const std::string& GetStartTestTag(void)
    {
        return m_startTestTag;
    };
    /**
     * Return the tag being used for vehicle build.
     *
     * @return Vehicle Build Tag.
     */
    inline const std::string& GetVehicleBuildTag(void)
    {
        return m_vehicleBuildTag;
    };
    /**
     * Return the tag being used for the next vehicle build.
     *
     * @return Next Vehicle Build Tag.
     */
    inline const std::string& GetNextVehicleBuildTag(void)
    {
        return m_nextVehicleBuildTag;
    };
    /**
     * Return the name being used for the Vin tag.
     *
     * @return Name of the Vin tag
     */
    inline const std::string& GetVinTag(void)
    {
        return m_vinTag;
    };
    /**
     * Return the name being used for the vin display tag.
     *
     * @return Name of the vin display tag.
     */
    inline const std::string& GetVinDisplayTag(void)
    {
        return m_vinDisplayTag;
    };
    /**
     * Return the name being used for the DriverNumber tag.
     *
     * @return Name of the DriverNumber tag.
     */
    inline const std::string& GetDriverNumberTag(void)
    {
        return m_driverNumberTag;
    };
    /**
     * Get the name of the driver number display tag.
     *
     * @return Name of the tag.
     */
    inline const std::string& GetDriverNumberDisplayTag(void)
    {
        return m_driverNumberDisplayTag;
    };
    /**
     * Return the default driver number.
     *
     * @return Default Driver Number.
     */
    inline const std::string& GetDefaultDriverNumber(void)
    {
        return m_defaultDriverNumber;
    };
    /**
     * Get the name of the driver number background color tag
     *
     * @return Name of the tag.
     */
    inline const std::string& GetDriverNumberBGColorTag(void)
    {
        return m_driverNumberBGColorTag;
    };
    /**
     * Get the default driver number background color.
     *
     * @return Default background color.
     */
    inline const std::string& GetDriverNumberBGColor(void)
    {
        return m_driverNumberBGColor;
    };
    /**
     * Return the name of the tag being used for Retainers down / Elevators up.
     *
     * @return Name of the tag.
     */
    inline const std::string& GetRetainersDownTag(void)
    {
        return m_retainersDownTag;
    };
    /**
     * Return the name of the cable connect tag.
     *
     * @return Name of the tag.
     */
    inline const std::string& GetCableConnectTag(void)
    {
        return m_cableConnectTag;
    };
    /**
     * Get the name of the tag being used for OverallTest.
     *
     * @return Name of the tag.
     */
    inline const std::string& GetOverallTestTag(void)
    {
        return m_overallTestTag;
    };

protected:
    /**
     * Load the additional configuration items needed by the vehicle data
     * broker.  The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 1.0
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * Read the current vehicle build record from the WorkCellController.
     *
     * @return Result of the operation - BEP_SUCCESS_RESPONSE, BEP_ERROR_RESPONSE, etc.
     * @since Version 1.0
     */
    virtual const std::string ReadLatestVehicleBuildRecord(bool publishAfterRead=true);
    /**
     * Update the current vehicle build record with a new record to be used for testing.
     *
     * @param vehicleBuildRecord
     *               New vehicle build record.
     * @return Result of the operation - BEP_SUCCESS_RESPONSE, BEP_ERROR_RESPONSE, etc.
     * @since Version 1.0
     */
    const std::string UpdateVehicleBuildRecord(const XmlNode *vehicleBuildRecord,bool publishRecord=true);
    /**
     * Publish the individual pieces of the build record to any subscribers.
     *
     * @param vehicleBuildRecord
     *               Vehicle Build Record.
     * @return Result of the operation - BEP_SUCCESS_RESPONSE, BEP_ERROR_RESPONSE, etc.
     * @since Version 1.0
     */
    const std::string UpdateVehicleBuildTags(const XmlNode *vehicleBuildRecord, bool publishTags);
    /**
     * Update the PLC for the new vehicle configuration.
     *
     * @param vehicleBuildRecord
     *               The vehicle build record.
     * @return Result of the operation - BEP_SUCCESS_RESPONSE, BEP_ERROR_RESPONSE, etc.
     * @since Version 1.0
     */
    const std::string UpdatePLC(const XmlNode *vehicleBuildRecord);
    /**
     * Load the data tags into the system.
     *
     * @param dataTags List of tags to use.
     * @since Version 1.0
     */
    void LoadDataTags(const XmlNode *dataTags);

    /**
     * Get a pointer to the plant host communication object
     * 
     * @return Pointer to the plant host comm object
     */
    IBepCommunication* PlantHostComm(void);

private:
    /**
     * Set the name of the process being used as the Plant Host.
     *
     * @param plantHost Name of the registered process.
     * @since Version 1.1
     */
    inline void SetPlantHost(const std::string &plantHost)
    {
        m_plantHost = plantHost;
    };
    /**
     * Set the name of the tag being used for StartTest.
     *
     * @param tag    Name of the start test tag.
     * @since Version 1.0
     */
    inline void SetStartTestTag(const std::string &tag)
    {
        m_startTestTag = tag;
    };
    /**
     * Save the tag being used for vehicle build information.
     *
     * @param tag    Vehicle build info tag.
     * @since Version 1.0
     */
    inline void SetVehicleBuildTag(const std::string &tag)
    {
        m_vehicleBuildTag = tag;
    };
    /**
     * Save the tag being used for the next vehicle build information.
     *
     * @param tag    Next vehicle build info tag.
     * @since Version 1.0
     */
    inline void SetNextVehicleBuildTag(const std::string &tag)
    {
        m_nextVehicleBuildTag = tag;
    };
    /**
     * Store the name being used for the Vin tag.
     *
     * @param tag    Name of the Vin tag
     * @since Version 1.2
     */
    inline void SetVinTag(const std::string &tag)
    {
        m_vinTag = tag;
    };
    /**
     * Store the name being used for Vin display.
     *
     * @param tag    Name of the vin display tag.
     * @since Version 1.2
     */
    inline void SetVinDisplayTag(const std::string &tag)
    {
        m_vinDisplayTag = tag;
    };
    /**
     * Store the name being used for the DriverNumber tag.
     *
     * @param tag    Name of the DriverNumber tag.
     * @since Version 1.2
     */
    inline void SetDriverNumberTag(const std::string &tag)
    {
        m_driverNumberTag = tag;
    };
    /**
     * Store the tag being used for the driver number display tag.
     *
     * @param tag    Name of the tag.
     * @since Version 1.3
     */
    inline void SetDriverNumberDisplayTag(const std::string &tag)
    {
        m_driverNumberDisplayTag = tag;
    };
    /**
     * Set the driver number background color tag.
     *
     * @param tag    Driver number background color tag.
     * @since Version 1.3
     */
    inline void SetDriverNumberBGColorTag(const std::string &tag)
    {
        m_driverNumberBGColorTag = tag;
    };
    /**
     * Store the default color for the driver number background field.
     *
     * @param color  Default color to use.
     * @since Version 1.3
     */
    inline void SetDriverNumberBGColor(const std::string &color)
    {
        m_driverNumberBGColor = color;
    };
    /**
     * Store the default driver number.
     *
     * @param defaultDriverNumber
     *               Driver Number.
     * @since Version 1.2
     */
    inline void SetDefaultDriverNumber(const std::string defaultDriverNumber)
    {
        m_defaultDriverNumber = defaultDriverNumber;
    };
    /**
     * Store the name of the tag being used for Retainers down / Elevators up.
     *
     * @param tag    Name of the tag.
     * @since Version 1.2
     */
    inline void SetRetainersDownTag(const std::string &tag)
    {
        m_retainersDownTag = tag;
    };
    /**
     * Stores the list of tags that need to be reported to the PLC when a new
     * vehicle build record is received.
     *
     * @param plcData Node containing the tags to be written to the PLC.
     * @since Version 1.1
     */
    inline void SetPlcData(const XmlNode *plcData)
    {
        if (plcData != NULL) m_dataToPLC.DeepCopy(plcData->getChildren());
        else m_dataToPLC.clear(true);
    };
    /**
     * Store the name of the cable connect tag.
     *
     * @param tag    Name of the tag.
     * @since Version 1.3
     */
    inline void SetCableConnectTag(const std::string &tag)
    {
        m_cableConnectTag = tag;
    };
    /**
     * Store the name of the OverallTest tag.
     *
     * @param tag    Name of the tag.
     * @since Version 1.3
     */
    inline void SetOverallTestTag(const std::string &tag)
    {
        m_overallTestTag = tag;
    };
    /**
     * Load the common module fault list from the file specified.
     * The format of the file should be defined in the config file as not all files are necessarily the same format.
     *
     * @param faultListData  Configuration data for common fault list
     */
    void LoadCommonModuleFaultList(const XmlNode *faultListData);
    /**
     * The config file will be examined to determine if the setting for using the bypass part number list has been 
     * changed.  The value will then be published to any subscribers.
     */
    void UpdateUseBypassPartNumberListTag();

	/**
	 * Get/Set the length (in characters) of the item code tag.
	 * 
	 * @param length Length of the item code tag.
	 * 
	 * @return Length of the item code tag.
	 */
	const UINT8& ItemCodeLength(const UINT8 *length = NULL);

    /** Tag to use for StartTest. */
    string m_startTestTag;
    /** Vehicle Build tag */
    string m_vehicleBuildTag;
    /** Next Vehicle Build tag */
    string m_nextVehicleBuildTag;
    /** Name of the registered process being used as the plant host. */
    string m_plantHost;
    /** Name of the tag being used for the Vin. */
    string m_vinTag;
    /** Name of the tag being used for the Vin display. */
    string m_vinDisplayTag;
    /** Name of the tag being used for the driver number. */
    string m_driverNumberTag;
    /** Name of the tag being used for DriveNumber display field on GUI. */
    string m_driverNumberDisplayTag;
    /** Driver Number Background Color tag. */
    string m_driverNumberBGColorTag;
    /** The default color for the Driver Number background color field on the GUI. */
    string m_driverNumberBGColor;
    /** Name of the tag being used for the Retainers down event. */
    string m_retainersDownTag;
    /** Default driver id. */
    string m_defaultDriverNumber;
    /** Tag being used for cable connect. */
    string m_cableConnectTag;
    /** Name of the OverallTest tag. */
    string m_overallTestTag;
    /**
	 *  Map containing tags that will be written to the PLC when a new
     * vehicle build record is read in.
     */
    XmlNodeMap m_dataToPLC;
    /** Communication object to interface with the NamedDataBroker. */
    INamedDataBroker *m_ndb;
    /** Communication object to use to communicate to the plant host object. */
    IBepCommunication *m_plantHostComm;
    /**
     * Flag used to indicate whether we want to reset the default driver number
     * to the default value from our config file or use the current system driver
     * number value when the rolls are  dropped
     */
    bool m_useDefDriverNo;
    /**
     * List of module faults common to all modules.
     * This can be used in the place of individual fault lists for each component.
     */
    XmlNodeMap m_commonModuleFaultList;
    /** Undefined fault format. */
    XmlNode *m_undefinedFault;
	/** Item code length for reporting common module faults */
	UINT8 m_itemCodeLength;
};

#endif  //VehicleDataBroker_h
