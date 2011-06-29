//*************************************************************************
// Description:
//   Test component for testing Tachograph systems.
//
//===========================================================================
// Copyright (c) 2007- Burke E. Porter Machinery
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
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef TachographTC_h
#define TachographTC_h
//------------------------------------------------------------------------------
#include "GenericTC.h"
#include "SerialChannel.h"
//------------------------------------------------------------------------------

class TachographTC : public GenericTC
{
public:
	TachographTC();
	virtual ~TachographTC();

    /**
     * Structure to hold one node of the calibration constant to switch setting lookup table.
     */
    typedef struct calSettings
    {
        int minW;                // Minimum W value of range
        int maxW;                // Maximum W value of range
        string switchSettings;   // Switch settings for W value
    } SwitchSettings;

    typedef map<int, SwitchSettings>                 SwitchSettingLookupTable;
    typedef SwitchSettingLookupTable::iterator        SwitchSettingLookupTableItr;
    typedef SwitchSettingLookupTable::const_iterator  SwitchSettingLookupTableCItr;
    typedef SwitchSettingLookupTable::value_type      SwitchSettingLookupTableVal;

	/**
	 * Initialize the test component.
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * Perform any test component specific initialization.
	 * The valid satellite radio types will be loaded into memory during
     * this phase of the initialization.
	 * <br><p>
	 *
	 * <b>Test Component Parameters:</b>
	 * 		<ul>
     *      <li> SwitchSettingsFile - Name of the file containing the switch settings for the various cal constants. </li> 
     *      <li> SwitchTypeTachographs - List of tachographs that use switches to update the cal constant. </li>
     *      <li> TachographCommConfig - Configuration data for communicating with the Tachograph unit. </li>
	 * 		</ul>
     * 
	 * <b>Functions Called:</b>
	 * 		<ul>
     *      <li> LoadLookupTable() </li>
	 * 		<li> GenericTC::Initialize() </li>
     *      <li> InitializationComplete() </li>
	 * 		</ul>
	 * 
	 * @param config Configuration file to use for initialization.
	 */
	virtual void Initialize(const XmlNode *config);

    /**
     * Execute the specified test step.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * This is the method that will handle the execution of each test step that has been specified by the system for each
     * test objective that is passed to it.  This must be overloaded to execute the component tests.
     * <br><p>
     * 
     * <b>Test Step Info:</b>
     * 		<ul>
     *      <li> Description - Description of the test step </li>
     * 		</ul>
     * 
     * <b>Test Step Functions Called:</b>
     * 		<ul>
     *      <li> TachographDistance() </li>
     * 		</ul>
     * 
     * <b>Functions Called:</b>
     * 		<ul>
     *      <li> GetTestStepName() </li>
     *      <li> StatusCheck() </li>
     *      <li> ConvertStatusToResponse() </li>
     * 		<li> GenericTCTemplate::CommandTestStep()</li>
     * 		</ul>
     * 
     * @param value  This is the value that was commanded by the test sequencer.
     * 
     * @return The result of the test step that was executed.
     */
	virtual const string CommandTestStep(const string &value);

protected:
	/** 
     * Calculate the multiplier to write to the tachograph unit.
	 * <br><p>
     * <b>Category:</b> Test Step 
     * <br><p>
     * <b>Description:</b><br>
	 * Measure the distance traveleed by the rollers and the distance traveled by the
	 * Tachograph unit.  The multiplier will be calculated based on the difference
	 * between the unit and the rollers.  The multiplier will then be written to the
	 * tachograph unit.
     * <br><p>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> RollerDistanceTag - System tag to use for reading the current roller distance </li>
     *      <li> TachographCountTag - Tag to use to read the current tachograph count </li>
     *      <li> ResetTachCountTag - Tag to use to reset the current tachograph count </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> ResetTachographCount - Flag to determine if the tachograph count should be reset. </li>
     *      <li> ResetTachCountPulseLength - Length of time to leave the reset pulse high. </li>
     *      <li> TachographTestSpeed - Target speed for starting the tachograph test. </li>
     *      <li> TachographSpeedRange - Speed range for the operator to achieve to perform the tachograph test </li>
     *      <li> MinTachographDistance - Minimum distance to travel to accurately calculate the tachograph constant </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step.
     *      <li> ScanDelay - Amount of time to wait between speed/distance checks </li>
     *      </ul>
     * 
     * <b>Fault Codes:</b>
     *      <ul>
     *      <li> ReadDistanceFailure - Could not read distances/counts </li>
     *      <li> TimeoutWaitingForDistance - Timeout waiting for vehicle to achieve required distance </li>
     *      <li> SystemStatus - Bad system status while waiting for distance </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ShortCircuitTestStep() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> GetTestStepInfoInt() </li>
     *      <li> SystemReadFloat() </li>
     *      <li> SystemReadInt() </li>
     *      <li> SystemWrite() </li>
     *      <li> GetDataTag() </li>
     *      <li> GetParameterBool() </li>
     *      <li> GetParameterInt() </li>
     *      <li> GetParameterFloat() </li>
     *      <li> GetParameter() </li>
     *      <li> BposSleep() </li>
     *      <li> GetFaultCode() </li>
     *      <li> GetFaultDescription() </li>
     *      <li> AccelerateToTestSpeed() </li>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> SendSubTestResultWithDetail() </li>
     *      <li> SendTestResult() </li>
     *      </ul>
     * 
	 * @return Result of updating the multiplier in the tachograph unit.
	 */
	string TachographDistance(void);

    /**
     * Update the calibration constant in the tachograph unit.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The type of tachograph will dictate how the calibration constant is updated in the module.  Certain tachogrpahs
     * have switches that must be moved to the correct position.  Other untis support communications and the constant
     * can be written to the module directly.
     * <br><p>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> CalConstantMsgTag - Message tag to use for writing the cal constant to the tachogrpah. </li>
     *      </ul>
     * 
     * <b>Test Component Parameters:</b>
     *      <ul>
     *      <li> SwitchSettingPromptDelay - Length of time to leave the switch setting prompt displayed. </li>
     *      <li> TachographUpdateDelay - Amount of time to wait for the tachograph process to respond with update results. </li>
     *      <li> PortReconnectDelay - Amount of time to wait to transmit message after reconnecting the port driver. </li>
     *      </ul>
     * 
     * <b>Test Step Info:</b>
     *      <ul>
     *      <li> Description - Description of the test step. </li>
     *      </ul>
     * 
     * <b>Prompts:</b>
     *      <ul>
     *      <li> SwitchSettings - Prompt to display which switches must be moved on the tachograph unit. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> LookupSwitchSettings() </li>
     *      <li> GetParameterInt() </li>
     *      <li> FindSwitchSettings() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      <li> BposSleep() </li>
     *      <li> CreateMessage() </li>
     *      <li> GetDataTag() </li>
     *      <li> SendSubTestResultWithDetail() </li>
     *      <li> GetTestStepInfo() </li>
     *      <li> ReadSubscribeData() </li>
     *      </ul>
     * 
     * @param calConst Calibration constant to update in the module.
     * 
     * @return Result of updating the calibration constant in the tachograph.
     */
    string UpdateCalConstant(const float &calConst);

    /**
     * Determine if the lookup table and switch settings should be used to set the calibration constant for
     * the tachograph unit. 
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The list of switchable tachograph types will be searched for the current tachograph type.  If the current type is 
     * not found in the list, false will be returned to the calling function.
     * <br><p>
     * 
     * <b>Data Tags:</b>
     *      <ul>
     *      <li> TachographType - Data tag for the tachograph build option. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetDataTag() </li>
     *      <li> SystemRead() </li>
     *      </ul>
     * 
     * @return Flag indicating if the switch settings should be used to set the calibration constant.
     */
    bool LookupSwitchSettings(void);
    /**
	 * Determine if the calibration constant should be written to the INS module.
	 * <br><p>
	 * <b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * The list of INS style tachograph types will be searched for the current tachograph type.  If the current type is 
	 * not found in the list, false will be returned to the calling function.
	 * <br><p>
	 * 
	 * <b>Data Tags:</b>
	 *      <ul>
	 *      <li> TachographType - Data tag for the tachograph build option. </li>
	 *      </ul>
	 * 
	 * <b>Functions Called:</b>
	 *      <ul>
	 *      <li> GetDataTag() </li>
	 *      <li> SystemRead() </li>
	 *      </ul>
	 * 
	 * @return Flag indicating if the switch settings should be used to set the calibration constant.
	 */
	bool WriteCalConstantToInsModule(void);
	/**
     * Use the lookup table to determine what the correct switch settings should be for the tachograph unit.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The lookup table will be searched for the correct switch settings based on the calibration constant 
     * provided.  The lookup will use the lower_bound function to find the correct range.  The lower_bound 
     * function will return the item that is one passed the item we are really looking for, so the iterator 
     * will be decremented by one before the data is extracted.
     * <br><p>
     * 
     * @param calIndex Calibration index to use for looking up the switch settings.
     * 
     * @return Switch settings from the lookup table to set on the tachograph unit.
     */
    string FindSwitchSettings(const int &calIndex);

    /**
     * Load the switch setting lookup table.
     * <br><p>
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * The switch settings will be loaded from the specified data file.  If the file cannot be loaded, an error
     * will be logged and the switch setting list will remain empty.
     * <br><p>
     * 
     * @param dataFile File that contains the switch setting lookup table.
     */
    void LoadLookupTable(const string &dataFile);

    /**
     * Utility test step for checking interface to external DLL app.
     * <br><p>
     * <b>Category:</b> Utility Test Step
     * <br><p>
     * <b>Description:</b><br>
     * The cal constant will be changed to a number and written to the tachograph interface application.
     * This is a test utility to ensure comms can be established between the test component and the DLL interface app.
     * <br><p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> UpdateCalConstant() </li>
     *      </ul>
     * 
     * @param calConst Constant to write to the tachograph unit.
     * 
     * @return Result of the interface test.
     */
    string TestInterface(const string &calConst);

    /**
     * Read the tachogrpah pulse count from the PLC.
     * 
     * @return Current Tachograph pulse count.
     */
    int ReadTachographPulses(void);

	/**
	 * Finishup method
	 *
	 * @return status string
	 */
	virtual inline string FinishUp(void);

	/**
	 * Controls output to the PLC - which circuit should be selected 
	 * based upon the type of Tachograph coming in from Broadcast.
	 *
	 * @return status string
	 */
	virtual string BlackBoxCircuitSelect(void);

	/**
	 * Does the real tach type scan work
	 *
	 * @return status bool
	 */
	virtual bool BlackBoxCircuitSelectScan(XmlNodeMap tachographTypesXX, int ctrlHi, int ctrlLo);

    /**
     * Abort the test sequence.
     * <b>Category:</b> Utility
     * <br><p>
     * <b>Description:</b><br>
     * Method used to abort the current processing based
     * on the needs of the component.<br>
     * <b><i>Note:</i></b>  The component will only set the abort flag
     * and the test status to Abort if the vehicle is equipped
     * with a 6x4 or 6x6 Tandem drive.
     * 
     * <b>Functions Called:</b><br>
     * 		<ul>
     * 		<li> IsTandemDriveEquipped() </li>
     * 		<li> GenericTC::Abort() </li>
     * 		</ul>
     */
	void Abort(void);

private:
    /**
     * Switch settings to use for the specific calibration constants.
     */
    SwitchSettingLookupTable m_calSwitchSettings;

    /**
     * List of switch type tachographs.
     * This list will be used to determine how to update the cal constant in the tachograph unit. 
     */
    XmlNodeMap m_switchTypeTacs;
	/**
	 * List of INS type tachographs.
	 * This list will be used to determine how to update the cal constant in the tachograph unit. 
	 */
	XmlNodeMap m_insTypeTacs;
    /**
     * Configuration data to use for establishing tachograph communications.
     */
    XmlNode *m_tachCommConfig;

    /**
     * Communication object to use for updating the cal constant in the tachogrpah unit.
     */
    SerialChannel *m_tachographComm;

	/**
	 * Starting and ending distance readings for the park brake test
	 */
	WHEELINFO	m_initialWheelDistances;
	WHEELINFO	m_finalWheelDistances;

	/**
	 * The lists of the respective tach types from the list in the config
	 */
	XmlNodeMap m_tachographTypes00;
	XmlNodeMap m_tachographTypes01;
	XmlNodeMap m_tachographTypes10;
	XmlNodeMap m_tachographTypes11;
};

//------------------------------------------------------------------------------
#endif //TachographTC_h
