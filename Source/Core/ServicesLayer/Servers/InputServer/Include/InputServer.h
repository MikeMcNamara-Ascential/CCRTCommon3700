//************************************************************************
// Description:
//	InputServer will handle data entered by the operator for Vin information and
//	Driver ID information.  The InputServer also sets the state of the
//  Pendant to function in certain methods during the course of a test.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/InputServer.h $
// 
// 2     6/07/06 5:36p Cward
// Changes to allow the Input Server, CLV295ScannerMgr,
// MapleKeypadManager, PendantManager, and SymbolScannerMgr configurations
// to be reloaed without restrting the processes.
//
// 1     6/05/06 3:45p Rwiersem
// Made the test class a friend.  Added a flag to tell if we should launch
// the input devices.  This is used to run the unit tests without the full
// system.  Added HandlePulse() and LoadAddtionalConfigurationItems()
// which are used to reload the configuration without restarting the
// process.
//
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
//
// 5     4/29/05 8:00a Gpattison
// Refactored the entire InputServer to spawn individual input devices
// instead of having them as objects.  This allows an input device to
// be added or removed from the system by making only configuration file
// changes.  All input devices are now stand alone BepServer type executables
//
// 5     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 4     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 3     7/21/04 11:30a Cward
// Changes to allow Vin Legnth to be configurable
//
// 2     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:32a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:14p Khykin
// Initial checkin.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.3  2003/02/23 17:05:59  tbochene
//    Changed vinLength back to 17
//
//    Revision 1.2  2003/02/20 15:50:16  tbochene
//    This is hacked to work, must change back after scanner is programmed
//
//    Revision 1.1.1.1  2003/02/15 00:05:39  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:05a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 7     11/26/02 4:54p Derickso
// Added support for Maple Keypads.
//
// 6     10/15/02 1:23p Derickso
// Modified to get PlantHostInbound server name from config file.
//
// 5     8/23/02 8:17a Dan.erickson
// Updated to conditionally write driver number to GUI.
//
// 4     7/29/02 8:29a Dan.erickson
// Added functionality to clear key press tag.
//
// 3     6/13/02 2:57p Dan.erickson
// Added support for CLV295 scanners.
//
// 2     6/07/02 12:02p Dan.erickson
// Added Publish methods as well as methods to keep track of
// build record status.
//
// 1     5/31/02 3:03p Dan.erickson
// Created and Debugged.
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================
#ifndef InputServer_h
#define InputServer_h
//===========================================================================

#include "BepSync.h"
#include "BepServer.h"
#include "INamedDataBroker.h"
#include "InputServerDefs.h"

/**
 * Driver Number status values.
 * @since Version 1.3
 */
const std::string driverNumberSent("Sent");
const std::string driverNumberNotSent("NotSent");
/**
 * Value that indicates retaining rolls are down
 * @since Version 1.3
 */
const std::string rollsDown("1");


/**
 * Data type used to hold attributes for an input device
 * program
 */
typedef struct _InputDevice_t
{
    /**
     * Name of the input device relative to /dev/name/local/core/
     */
    string  deviceName;
    /**
     * Name of the executable to be launched for this input device
     */
    string  processName;
    /**
     * Name the input device will register (i.e. name_attach())
     */
    string  regName;
    /**
     * Path the input device will adopt (i.e. /dev/name/local/core/SymbolScanner)
     */
    string  devicePath;
    /**
     * Process ID of the launched input device
     */
    int     devicePid;
} InputDevice_t;

/**
 * Data type used to hold the mapping of a input device's name
 * to the process used for that input device. Has overloaded
 * methods to facilitate adding and removing contained pairs more
 * easily
 */
class InputDeviceProcessMap : private map<string, InputDevice_t*>
{
public:
    /**
     * Method used to add a new InputDevice/Process pair to the list
     *
     * @param deviceName Name of the input device to add
     * @param regName  Name the input device will register with
     * @param processName  Name of the input device to use for the given device name
     * @param pid      Process ID of the spawned input device
     */
    void AddDevice( const string &deviceName, const string &regName, const string &processName, int pid=-1)
    {
        InputDevice_t *newProcess = new InputDevice_t;

        newProcess->deviceName = deviceName;
        newProcess->processName = processName;
        newProcess->regName = regName;
        newProcess->devicePid = pid;
        insert(map<string, InputDevice_t*>::value_type(deviceName, newProcess));
    }

    /**
     * Method used to remove a InputDevice/Process pair from the list
     *
     * @param deviceName Name of the input device to add
     */
    void RemoveDevice(const string &deviceName)
    {
        map<string, InputDevice_t*>::const_iterator  itr;
        if ((itr=find(deviceName)) != end())
        {
            delete itr->second;
        }
        erase(deviceName);
    }

    void SetPid( const string &deviceName, int pid)
    {
        map<string, InputDevice_t*>::const_iterator itr;
        if ( (itr=find(deviceName)) != end())
        {
            itr->second->devicePid = pid;
        }
    }

    iterator begin()
    {
        return(map<string, InputDevice_t*>::begin());
    }

    const_iterator begin() const
    {
        return(map<string, InputDevice_t*>::begin());
    }

    iterator end()
    {
        return(map<string, InputDevice_t*>::end());
    }

    const_iterator end() const
    {
        return(map<string, InputDevice_t*>::end());
    }

    iterator find(const key_type& key )
    {
        return(map<string, InputDevice_t*>::find( key));
    }

    const_iterator find(const key_type& key ) const
    {
        return(map<string, InputDevice_t*>::find( key));
    }

    size_type size()
    {
        return(map<string, InputDevice_t*>::size());
    }

    void clear()
    {
        map<string, InputDevice_t*>::iterator itr;
        for (itr=begin(); itr!=end(); itr++)
        {
            delete itr->second;
            itr->second = NULL;
        }

        map<string, InputDevice_t*>::clear();
    }
};

/**
 * InputServer class.
 * The InputServer manages all the data input devices such as the pendant and the barcode scanners.
 * The InputServer spins off a thread for each input device and then manages the data provided from
 * each device.  For example, once the pendant thread has a complete vin, it will be written to the
 * InputServer.  The InputServer will then write the vin to the WorkCellControllerPlantHostInbound
 * object.  During normal operations, the InputServer will publish the data to the object that is
 * requesting the data.
 *
 * @author Dan Erickson
 * @version Version 1.5
 */
class InputServer : public BepServer
{
    friend class InputServerTest;

public:
    /**
     * Class Constructor.
     * @since Version 1.0
     */
    InputServer();
    /**
     * Class destructor.
     * @since Version 1.0
     */
    ~InputServer();
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
     * Process the reload configuration pulse.  If any other pulses
     * are received, send them on to BepComponent.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
    /**
	 * Method used to terminate the component.  This performs
	 * any cleanup that the destructor would not and sets
	 * the state of the component to terminate.
	 */
	virtual void Terminate(void);
	/**
	 * Method used to terminate the individual input device.
	 */
	virtual void TerminateDevice(const InputDevice_t *deviceProcess);
    /**
     * Reads the logical-to-physical input device mapping table from the
     * specified XML node
     *
     * @param docNode Reference to a parsed configuration document.
     * @return true if successful, false on error
     * @since Version 1.0
     */
    bool ReadInputDeviceMap( const XmlNode *docNode);
    /**
     * Request to write a data tag
     *
     * @param node    Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 1.0
     */
    virtual const std::string Write(const XmlNode *node);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 1.1
     */
    virtual const std::string Publish(const XmlNode *node);
    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *		   BEP_SUCCESS_RESPONSE == Registered
     * @since Version 1.0
     */
    virtual const std::string Register(void);
    /**
     * Get the current driver number from the VehicleDataBroker.
     *
     * @return The current driver number.
     * @since Version 1.3
     */
    std::string GetCurrentDriverNumber(void);
    /**
     * Get the status of the vehicle build record.
     *
     * @return Status of the vehicle build record (Valid, Invalid).
     * @since Version 1.1
     */
    inline const std::string GetInputServerState(void);
    /**
     * Returns the lowered status of the retaining rolls.
     *
     * @return True - Retaining rolls are down / Elevators up.
     * @since Version 1.3
     */
    inline const bool& RollsAreDown(void);
    /**
     * Get the status of the driver number.
     *
     * @return Status - Sent, Not Sent.
     * @since Version 1.3
     */
    inline const std::string& GetDriverNumberStatus(void);
    /**
     * Get the default driver number.
     *
     * @return Default driver number.
     * @since Version 1.3
     */
    inline const std::string& GetDefaultDriverNumber(void);
    /**
     * Return the name of the PlantHostInbound server.
     *
     * @return Name of the server.
     * @since Version 1.4
     */
    inline const std::string GetPlantHostInboundName(void);
    /**
     * Returns whether or not the PLantHostInbound server name was specified.
     *
     * @return True - a name was provided.
     *         False - a name was NOT provided.
     * @since Version 1.4
     */
    inline const bool IsPlantHostInboundNameSpecified(void);
    /**
     * Get the VIN length.
     *
     * @return the VIN length
     */
    virtual inline UINT16 GetVinLength();

protected:
    /**
     * Load the additional configuration items needed by the input server.
     * The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

    /**
     * Launches the necessary individual device input processes
     */
    void LaunchInputDevices();
    /**
     * Launches the given input device processes
     *
     * @param deviceDrv input device to launch
     * @return true if successful, false on error
     */
    bool LaunchInputDevice(InputDevice_t    *deviceDrv);
    /**
     * Commands the individual input device processes to register
     */
    void RegisterInputDevices();
    /**
     * Commands the given input device to register
     */
    void RegisterInputDevice(const InputDevice_t *deviceDrv);
    /**
     * Expands environment variable names in the input device name
     *
     * @param deviceDrv Path of the input device to be cleaned up
     */
    void FixUpDevicePath(string &deviceDrv);
    /**
     * Determine if the driver number needs to be sent.
     *
     * @return True - Send the driver number.
     * @since Version 1.3
     */
    const bool SendDriverNumber(void);
    /**
     * Set the Sent status of the driver number.
     *
     * @param status Send status.
     * @since Version 1.3
     */
    inline void SetDriverNumberStatus(const std::string &status);
    /**
     * Store the default driver number.
     *
     * @param driverNumber
     *               Default driver number.
     * @since Version 1.3
     */
    inline void SetDefaultDriverNumber(const std::string driverNumber);
    /**
     * Stores the status of the retaining rolls.  True - rolls are down / elevators up.
     *
     * @param status Lowered status for retaining rolls.
     * @since Version 1.3
     */
    inline void SetRollsDownStatus(const bool &status);
    /**
     * Store the configuration nodes for the input device managers.
     *
     * @param config Input device manager configurations.
     * @since Version 1.1
     */
    inline void StoreInputManagerConfigs(const XmlNode *config);
    /**
     * Store the name of the PlantHostInbound object.
     *
     * @param name   Name of the PlantHostInbound server.
     * @since Version 1.4
     */
    inline void SetPlantHostInboundName(const std::string &name);
    /**
     * Sets the flag for whether the PlantHostInbound name was provided in the
     * configuration file.
     *
     * @param status True - the name was specified.
     *               False - the name was NOT specified.
     * @since Version 1.4
     */
    inline void SetPlantHostNameProvided(const bool status);
    /**
     * Sets the VIN Length.
     *
     * @param vinLength - length of the VIN
     */
    inline void SetVinLength(const UINT16 vinLength);

private:
    /**
     * Communication object to talk to the WorkCellController object.
     * @since Version 1.0
     */
    IBepCommunication m_wccComm;
    /**
     * Name of the PlantHostInbound object.
     * @since Version 1.4
     */
    std::string m_plantHostInboundName;
    /**
     * Status of the driver number - Sent, Not Sent.
     * @since Version 1.3
     */
    std::string m_driverNumberStatus;
    /**
     * Default driver number.
     * @since Version 1.3
     */
    std::string m_defaultDriverNumber;
    /**
     * Rolls down / Elevators up status.
     * @since Version 1.3
     */
    bool m_rollsDownStatus;
    /**
     * Flag to determine if the PlantHostInbound name was provided in the config file.
     * @since Version 1.4
     */
    bool m_plantHostSpecified;
    /**
     * Contains the config information for all Input Device managers that need to be started.
     * @since Version 1.1
     */
    XmlNodeMap m_inputDeviceManagers;
    /*
     * VIN length for barcode
     */
    INT16 m_vinLength;
    /**
     * Communication object to interface with the NamedDataBroker.
     */
    INamedDataBroker *m_ndb;
    /**
     * Mapping of input device name to the device executable name
     */
    InputDeviceProcessMap   m_inputDeviceMap;
    /**
     * Flag that tells if we should launch the input devices.  This allows us
     * to perform the unit tests without the full system running.
     */
    bool m_launchInputDevices;
};

#endif  // InputServer_h
