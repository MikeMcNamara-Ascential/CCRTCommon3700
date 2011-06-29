//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/MachineDataBroker/MachineDataBroker.h 3     3/10/06 3:54p Cward $
//
// FILE DESCRIPTION:
//   Machine data broker that is responsible for maintaining and publishing
//   all of the information related to the machine.
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MachineDataBroker/MachineDataBroker.h $
// 
// 3     3/10/06 3:54p Cward
// Updated to allow reloading of config files without having to reboot
//
// 1     3/06/06 1:21p Rwiersem
// Added methods used when reloading the configuration file.
//
// 2     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 1     12/21/03 6:41p Cward
//
// 3     10/24/03 1:42p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:30a Khykin
// Initial checkin
//
// 2     8/07/03 7:54a Khykin
// Update with latest changes from Flatrock.
//
// 2     6/20/03 8:50a Khykin
// Updated for Ford changes.
//
// 1     6/12/03 4:17p Khykin
// Initial checkin.
//
// 2     4/30/03 10:23a Khykin
// Added the handling of the publication of the TestResult so the GUI
// TestResultToPrint will be updated.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:50  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:12a Khykin
// Initial Checkin
//
// 1     1/18/03 8:04a Khykin
// Initial Checkin From DCTC
//
// 2     5/24/02 2:55p Kh
// Adding the timestamping for the LossCompensation cycle.
//
// 1     2/21/02 2:09p Skypig13
// Initial check in
//
//*************************************************************************
#ifndef MACHINEDATABROKER_H
#define MACHINEDATABROKER_H
//==============================================================================

#include "BepServer.h"
#include "BepTimer.h"
#include "BepSync.h"
#include "SerialChannel.h"
#include "IcmDataSupplier.h"
#include "IQnxDataServer.h"
#include <unistd.h>
#include <time.h>

/**
 * Pulse code used for private MachineDataBroker pulses
 */
#define MACHINE_DATA_BROKER_PULSE_CODE		SYSTEM_PULSE+5

/**
 * Pulse value used to indicate we need to update the cycle timer.
 */
#define	CYCLE_TIMER_PULSE	10

/**
 * Pulse value used to indicate we need to update the power supplies.
 */
#define	POWER_SUPPLY_PULSE	11
/**
 * Pulse code to use for updating encoder data to external systems.
 */
#define ENCODER_DATA_PULSE  12
/** Pulse value to use for ICM data */
#define ICM_DATA_PULSE      13

const static string m_speedTags[] = {"LfSpeed", "RfSpeed", "LrSpeed", "RrSpeed", "LtSpeed", "RtSpeed"};
const static string m_forceTags[] = {"LfForce", "RfForce", "LrForce", "RrForce", "LtForce", "RtForce"};
const static string m_distTags[]  = {"LfDist", "RfDist", "LrDist", "RrDist", "LtDist", "RtDist"};

/**
 * MachineDataBroker class responsible for maintaining machine specific information.
 * (at this point there in no behavior that is unique that needs to be overloaded)
 *
 * @author Kevin Hykin
 * @version 1.3
 */
class MachineDataBroker : public BepServer
{
    friend class MachineDataBrokerTest;

public:
	/**
	 * MachineDataBroker default constructor
	 * @since Version 1.0
	 */
	MachineDataBroker(void);
	/**
	 * MachineDataBroker destructor
	 * @since Version 1.0
	 */
	virtual ~MachineDataBroker(void);
	/**
	 * Method for initialization passing in the file name
	 *
	 * @param fileName Name of the XML configuration file to be used to initialize
	 * @since Version 2.4
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
	 * @since Version 1.2
	 */
	virtual const std::string Register(void);
	/**
	 * Method used to terminate the component.  This performs
	 * any cleanup that the destructor would not and sets
	 * the state of the component to terminate.
	 * @since Version 1.0
	 */
	virtual void Terminate(void);
	/**
	 * Request to Perform an action
	 *
	 * @param node    Action to be performed
	 * @return Status of operation
	 *         Success, Fail, Invalid, Unavailable
	 * @since Version 1.2
	 */
	virtual const std::string Command(const XmlNode *node);
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
     * <b>Description:</b>
     * Request to write a data tag.
     *
     * @param node The XML node to be written.
     * 
     * @return BEP_SUCCESS_RESPONSE if the new value was written successfully.
     *         BEP_ERROR_RESPONSE if the data tag was found, but the tag data
     *         mutex could not be locked.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was found and the tag 
     *         data mutex was locked, but the data could not be written.
     *         BEP_UNSUPPORTED_RESPONSE if the data tag could not be found.
     */
    virtual const string Write(const XmlNode *node);
    /**
     * <b>Description:</b>
     * Request to write a data tag.
     *
     * @param tag   The data tag to be written.
     * @param value The new value to be written.
     * 
     * @return BEP_SUCCESS_RESPONSE if the new value was written successfully.
     *         BEP_ERROR_RESPONSE if the data tag was found, but the tag data
     *         mutex could not be locked.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was found and the tag 
     *         data mutex was locked, but the data could not be written.
     *         BEP_UNSUPPORTED_RESPONSE if the data tag could not be found.
     */
    virtual const string Write(const string &tag, const string &value);
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
     * Handler for the cycle time pulse.
     *
     * @return The status of the operation.
     */
    INT32 CycleTimePulseHandler(void);
    /**
     * This method updates the current cycle time based on what
     * the initial cycle time was.
     *
     * @param start  Flag that indicates if this is the first time the method was
     *               called.
     */
    void UpdateCycleTime(bool start = false);
	/**
	 * Return the number of tests since the last loss compensation cycle.
	 *
	 * @return Number of tests since last loss compensation cycle.
	 * @since Version 1.1
	 */
	virtual const INT32 GetNumberOfTestsSinceLossCompensation(void);

    /**
     * Handler for the power supply pulse.
     *
     * @return The status of the operation.
     */
    INT32 PowerSupplyPulseHandler(void);
    /**
     * Get the updated encoder data.
     * The data will then be sent to any exernal systems.
     * 
     * @return Status of supplying the encoder data to the external systems.
     */
    INT32 EncoderDataPulseHandler(void);
    /**
     * Update the ICM data.
     * The maximum roller speed will be found and made available to the system.
     * 
     * @return Status of updating the current maximum roller speed.
     */
    virtual INT32 IcmDataPulseHandler(void);

protected:
	/**
     * Load the additional configuration items needed by the machine data broker.
     * The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Set the machine number.  There is a node in the configuration file
     * that specifies if the machine number should come from the configuration
     * file or from the environment variable.
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void SetMachineNumber(const XmlNode *document);
    /**
     * Setup the Cycle timer.  The update rate is specified in the
     * configuration file.
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void SetupCycleTimer(const XmlNode *document);
    /**
     * Setup the configuration items for power supply monitoring.  The
     * configurable items are update rate, encoder 5V min, and encode 5V
     * max.
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void SetupPowerSupplyMonitoring(const XmlNode *document);
    /**
     * Setup the configuration items for vehicle centering position display
     * int leftLimit rightLimit
     */
    virtual void SetVehicleCenteringPositionLimits(int leftLimit, int rightLimit);
    /**
     * Setup the encoder data supply information.
     * If encoder data is to be supplied to an external system, the communication object will
     * be created and a timer will be started.
     * 
     * @param document Configuration data for setting up the encoder data supply.
     */
    void SetupEncoderDataSupply(const XmlNode *document);
	/**
     * Setup the timer for the Icm data supplier.
     * 
     * @param document Setup data for the ICM data supplier timer.
     */
    void SetupIcmDataTimer(const XmlNode *document);
	/**
	 * Get the current state of the machine.
	 *
	 * @param dataFile The data file to get the information from.
	 * @since Version 1.1
	 */
	void GetMachineState(const std::string &dataFile);
	/**
	 * Store the current state of the machine.
	 *
	 * @param dataFile The data file to write the information to.
	 * @since Version 1.1
	 */
	void StoreMachineState(const std::string &dataFile);
	/**
	 * Update the number of vehicle tests performed since the
	 * last loss compensation cycle.
	 *
	 * @param numberOfTests
	 *               Number of tests since the last loss compensation cycle.
	 *               If no number is provided, the number of tests is incremented.
	 * @since Version 1.1
	 */
	void UpdateNumberOfTestsSinceLossCompensation(const INT32 *numberOfTests = NULL);
    /**
     * Get/Set the flag indicating if the encoder data should be supplied to external systems.
     * 
     * @param supplyData Flag indicating if the encoder data should be supplied to external systems.
     * 
     * @return Flag indicating if the encoder data should be supplied to external systems.
     */
    const bool &SupplyEncoderDataExternally(const bool *supplyData = NULL);
    /**
     * Get the current roller count of the machine.
     * 
     * @return Current roller count of the machine.
     */
    INT32 RollerCount(void);
    /** Icm data supplier object */
    IcmDataSupplier *m_icmData;

    /**
     * Get the current speed scaling factor in use.
     * 
     * @return Current speed scaling factor in use.
     */
    float GetCurrentSpeedScalingFactor(void);

private:
    /**
     * Store the flag indicating if the force file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the force file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of force file.
     */
    void SetForceFileOpen(bool fileOpen);

    /**
     * Store the flag indicating if the speed file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the speed file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of speed file.
     */
    void SetSpeedFileOpen(bool fileOpen);

    /**
     * Store the flag indicating if the distance file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Set the flag indicating if the wheel distance file is open.
     * <p>
     * @param fileOpen A flag indicating the open status of wheel distance file.
     */
    void SetWheelDistanceFileOpen(bool fileOpen);
    /**
     * Get the current roller speeds.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current wheel speeds from the system.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentSpeeds - Name of the file conatining the current roller speeds. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetRollerCount() </li>
     *      <li> IsSpeedFileOpen() </li>
     *      <li> OpenSpeedFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param wheelSpeed The wheel speed array to populate with the current speeds.
     * 
     * @return  Status of reading the roller speeds.
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the wheel speeds were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if the speed file mutex could not be acquired or an exception was thrown. </li>
     *          <li> BEP_STATUS_FAILURE for any other errors. </li>
     *          </ul>
     */
    INT32 GetWheelSpeeds(float wheelSpeed[]);
    /**
     * Get the current wheel distances.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current distance for all wheels.  If the wheel distance file has not yet been opened, it will be opened.
     * The wheel distances will then be read from the file.  The wheel distance file will remain open to expedite
     * further force reads.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentDistances - Name of the current wheel distance file. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetRollerCount() </li>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      <li> OpenWheelDistanceFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param wheelDistance An array for storing the distance values.
     * 
     * @return Status of reading the wheel distances:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the wheel distances were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if an exception is thrown or the wheel distance file mutex could not be acquired. </li>
     *          </ul>
     */
    INT32 GetWheelDistances(float wheelDistance[]);
    /**
     * Get the current forces.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Read the current force for all wheels.  If the force file has not been opened yet, the force file will be opened.
     * The forces will then be read from the file.  The file will remain open to expedite further force reads.
     * <p>
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> CurrentForces - Name of the file that contains the current forces. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetTestStepName() </li>
     *      <li> GetRollerCount() </li>
     *      <li> IsForceFileOpen() </li>
     *      <li> OpenForceFile() </li>
     *      <li> GetParameter() </li>
     *      </ul>
     * 
     * @param forces An array for storing the force values.
     * 
     * @return Status of reading the wheel forces from the force file:
     *          <ul>
     *          <li> BEP_STATUS_SUCCESS if the forces were read successfully. </li>
     *          <li> BEP_STATUS_ERROR if an exception is thrown or the force file mutex could not be acquired. </li>
     *          </ul>
     */
    INT32 GetForces(float force[]);
    /**
     * Open the force file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the ICM force file for reading wheel forces.  A flag will also be set indicating if the file has been 
     * opened successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentForces - Name of the ICM force file to be opened. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetForceFileOpen() </li>
     *      <li> IsForceFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenForceFile(void);
    /**
     * Close the force file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the force file.  A flag will be set indicating the force file was closed.  If the force file was not opened,
     * nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsForceFileOpen() </li>
     *      <li> SetForceFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the force file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the force file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseForceFile(void);
    /**
     * Check if the force file has been opened.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the force file is open.
     * <p>
     * @return Flag indicating if the force file is open.
     */
    bool& IsForceFileOpen(void);
    /**
     * Open the speed file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the speed file for reading wheel speeds.  A flag will also be set indicating if the file has been opened 
     * successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentSpeeds - Name of the current wheels speed file to be opened. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetSpeedFileOpen() </li>
     *      <li> IsSpeedFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenSpeedFile(void);
    /**
     * Close the ICM speed file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the speed file.  A flag will be set indicating the speed file was closed.  If the speed file was not opened,
     * nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsSpeedFileOpen() </li>
     *      <li> SetSpeedFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the speed file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the speed file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseSpeedFile(void);
    /**
     * Check if the speed file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the speed file is open.
     * <p>
     * @return Flag indicating if the speed file is open.
     */
    bool& IsSpeedFileOpen(void);
    /**
     * Open the ICM wheel distance file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Open the wheel distance file for reading wheel distances.  A flag will also be set indicating of the file has 
     * been opened successfully.
     * <p>
     * <b>Test Component Parameters:</b>
     *     <ul>
     *     <li> CurrentDistances - Name of the ICM file containing the current wheel distances. </li>
     *     </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetParameter() </li>
     *      <li> SetWheelDistanceFileOpen() </li>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      </ul>
     * 
     * @return BEP_STATUS_SUCCESS if the file was opened successfully.<br>
     *         BEP_STATUS_FAILURE if the file was not opened.
     */
    BEP_STATUS_TYPE OpenWheelDistanceFile(void);
    /**
     * Close the ICM wheel distance file.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Close the wheel distance file.  A flag will be set indicating the wheel distance file was closed.  If the wheel
     * distance file was not opened, nothing will be done.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IsWheelDistanceFileOpen() </li>
     *      <li> SetWheelDistanceFileOpen() </li>
     *      </ul>
     *
     * @return BEP_STATUS_SUCCESS if the wheel distance file was closed succesfully.<br>
     *         BEP_STATUS_FAILURE if the wheel distance file was not closed successfully.
     */
    BEP_STATUS_TYPE CloseWheelDistanceFile(void);
    /**
     * Check if the wheel distance file is open.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * Return the flag indicating if the wheel distance file is open.
     * <p>
     * @return The flag indicating if the wheel distance file is open.
     */
    bool& IsWheelDistanceFileOpen(void);
    /**
     * Build the encoder data string using the provided encoder data.
     * 
     * @param encoderData
     *               Encoder data.
     * 
     * @return Encoder data string.
     */
    string BuildEncoderDataString(const XmlNode *encoderData);
	/**
     * Get/Set the flag indicating of the QNX Data Server should be connected to.
     * 
     * @param connectToServer
     *               Flag indicating if the QNX data Server should be used.
     * 
     * @return Flag indicating if the QNX data Server should be used.
     */
    const bool& ConnectToQnxDataServer(const bool *connectToServer = NULL);
    /**
	 * The file to store the persistent data in.
	 */
	string m_dataFile;
	/**
	 * Number of vehicles tested since the last Loss Compensation cycle.
	 */
	INT32  m_numberOfTestsSinceLossCompensation;

	/**
	 * Timer object used to update the cycle time during a test.
	 */
	BepTimer m_cycleTimeTimer;

    /**
     * The start time of the vehicle test.
     */
    time_t m_vehicleTestStart;

    /**
     * The cycle timer started flag.
     */
    bool m_cycleTimerStarted;

    /**
	 * Timer object used to update the power supplies during a test.
	 */
	BepTimer m_PowerSupplyTimer;

    /**
     * Encoder 5 volt power supply minimum parameter.
     */
    float m_encoder5vMin;

    /**
     * Encoder 5 volt power supply maximum parameter.
     */
    float m_encoder5vMax;
    /**
     * Communication object to use for sending encoder data to external systems.
     */
    SerialChannel *m_encoderDataComm;
    /**
     * Timer object to use for sending encoder data to external systems.
     */
    BepTimer m_encoderDataTimer;
    /**
     * Flag used to determine if the ecoder data should be supplied to external systems.
     */
    bool m_supplyEncoderData;
    /**
     * Configuration data to use for setting up the encoder data supply comm object.
     */
    XmlNode *m_encoderCommConfig;
    /** File descriptor for the force file. */
    INT32 m_forceFile;
    /**
     * Flag indicating the open status of the force file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_forceFileOpen;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_forceFileMutex;
    /** File descriptor for the speed file. */
    INT32 m_speedFile;
    /**
     * Flag indicating the open status of the speed file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_speedFileOpen;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_speedFileMutex;
    /** File descriptor for the wheel distance file. */
    INT32 m_wheelDistanceFile;
    /**
     * Flag indicating the open status of the wheel distance file.
     * True - File is open.
     * False - File is closed.
     */
    bool m_wheelDistanceFileOpen;
    /**
     * value indicating vehicle centering left limit
     */
    int m_vehicleCenteringPositionLeftLimit;
    /**
     * value indicating vehicle centering right limit
     */
    int m_vehicleCenteringPositionRightLimit;
    /**
     * Mutex used to prevent multiple threads from using the same file 
     * descriptor to access the file.
     */
    BepMutex m_wheelDistanceFileMutex;
    /**
     * Format to use for building the encoder data string.
     */
    XmlNodeMap m_encoderDataStringFormat;
    /**
     * Format to use for reporting the encoder data.
     */
    string m_encoderDataFormat;
    /**
     * Flag to indicate if the server should connect to the QNX Data Server.
     */
    bool m_connectToQnxDataServer;
    /**
     * Interface to the QNX Data Server.
     */
    IQnxDataServer m_qnxDataServer;
    /** List of ICM data files to use */
    XmlNode *m_icmDataFiles;
    /** Timer to use for getting the maximum roller speed */
    BepTimer m_icmDataTimer;
    /** Speed scaling factor */
    float m_speedScalingFactor;
    /** Current speed scaling factor in use */
    float m_currentSpeedScalingFactor;
};
#endif  //MACHINEDATABROKER_H

