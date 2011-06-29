//******************************************************************************
// Description:
//    Mercedes System Monitor Task
//
//
//==============================================================================
//     Copyright © 2010 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
// 
//
//******************************************************************************

#ifndef Mercedes_SYSTEM_MONITOR_INCLUDED
#define Mercedes_SYSTEM_MONITOR_INCLUDED

/*******************
 include files
 ******************/
#include "SystemMonitor.h"

/**
 * Mercedes System Monitor class.
 * Handles system events: Start Test push button, ABORT, VIN processing, etc
 */
class MercedesSystemMonitor : public SystemMonitor
{
	typedef enum
	{
		CHECK_PANEL_ESTOP,
		CHECK_LEFT_PENDANT_ESTOP,
		CHECK_RIGHT_PENDANT_ESTOP,
		ESTOP_BUTTON_COUNT
	} BEP_CHECK_ESTOP;

public:
	/**
	 * Default constructor. Set debug to false, channel ID's to -1, etc
	 */
	MercedesSystemMonitor();

	/**
	 * Constructor from command line args
	 *
	 * @param argc   Number of command line arguments
	 * @param argv   Array of command line args
	 */
	MercedesSystemMonitor( int argc, char *argv[]);

	/**
	 * Class destructor
	 */
	virtual ~MercedesSystemMonitor();

protected:
	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         Success, Fail
	 * @since Version 2.5
	 */
	virtual const string Register(void);
	/**
	 * method for component initialization.
	 *
	 * @param document Reference to a parsed configuration document.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);

	/**
     * Overloaded BepServer Publish method which provides the ability
     * to handle the fault level messages that were requested.
     *
     * @param node the published information to deal with.
     * @return the status of the operation.
     */
    virtual const std::string Publish(const XmlNode *node);


    /**
	 * Monitors for system events such as ABORT, CAL, SIM, START_TEST, etc
	 * 
	 * @param ctrl   Current value of control data needed for system control events
	 */
	virtual void UpdateControl(ControlData *ctrl);

	/**
     * Checking for any Estops being hit so they can be displayed to the driver
     * and we can abort the current test in progress
     *
     * @return bool value which reflects an estop being found/pressed
     */
    virtual bool CheckForAnyEstopPressed(void);
	/**
	 * Testing control logic
	 *
	 * @param ctrl Current value of control data needed for system control events
	 */
	virtual void CheckTesting( ControlData *ctrl);
	
	/**
	 * Reads current values of control data items. The m_subscribedData data is
	 * updated asynchronously via subscription notifications from the servers to
	 * whom we are subscribed
	 *
	 * @param ctrl   Control Data structure to fill with values
	 */
	virtual void GetCurrentControlData( ControlData *ctrl);
	/**
	 * Set the flag to indicate the vehicle build was already read.
	 *
	 * @param readStatus  Value to set the vehcle build reacord read flag to.
	 * @since Verison 1.3
	 */
	virtual void SetVehicleBuildRecordRead(const bool& readStatus);
	/**
	 * Check if the vehicle build record has already been read.
	 *
	 * @since Version 1.3
	 */
	virtual const bool& VehicleBuildRecordRead(void);
	/**
	 * Set flag to indicate if the component
	 * data for the next test has been loaded.
	 * 
	 * @param testDataLoaded
	 *               Value to set the member variable with
	 */
	virtual void SetLoadVehicleTestData(const bool &testDataLoaded);
	/**
	 * Check if the Test Component data has
	 * been loaded for the next test.
	 * 
	 * @return Value of boolean member variable that stores
	 *         the information
	 */
	virtual bool IsVehicleTestDataLoaded(void);
	/**
	 * Get the prompt box background color to be used when an E-Stop is pressed.
	 *
	 * @return Prompt box background color for E-Stop prompts.
	 */
	const string& GetEStopPressedBackgroundColor(void);
	/**
	 * Get the prompt box background color to be used when an E-Stop is cleared.
	 *
	 * @return Prompt box background color for E-Stop cleared prompts.
	 */
	const string& GetEStopClearedBackgroundColor(void);
	/**
	 * Get the prompt tag to use when the panel E-Stop is pressed.
	 * 
	 * @return Prompt tag for panel E-stop pressed.
	 */
	const string& GetPanelEStopPressedPromptTag(void);
	/**
	 * Get the prompt tag to use when the left pendant E-Stop is pressed.
	 * 
	 * @return Prompt tag for left pendant E-stop pressed.
	 */
	const string& GetLeftPendantEStopPressedPromptTag(void);
	/**
	 * Get the prompt tag to use when the right pendant E-Stop is pressed.
	 * 
	 * @return Prompt tag for right pendant E-stop pressed.
	 */
	const string& GetRightPendantEStopPressedPromptTag(void);
	/**
	 * Get the prompt box to be used for displaying the E-Stop prompts.
	 * 
	 * @return Prompt Box to be used for E-Stop prompts.
	 */
	const INT32& GetEStopPromptBox(void);

private:
	/**
	 * Determine if the specified E-stop button should be monitored.
	 * 
	 * @param estopButton Which E-stop button to check.
	 * @param checkForEstop
	 *                   True  - Monitor the specified E-Stop.
	 *                   False - Do not monitor the specified E-stop.
	 *                   NULL  - Get the value of the specified E-stop to determine if it should be monitored.
	 * 
	 * @return True - The specified E-stop should be monitored.
	 */
	const bool CheckForEstop(BEP_CHECK_ESTOP estopButton, const bool *checkForEstop = NULL);
	/**
	 * Store the background color to use when an E-Stop is pressed.
	 *
	 * @param color  Prompt box background color to use when an E-Stop is pressed.
	 */
	void SetEStopPressedBackgroundColor(const string &color);
	/**
	 * Store the background color to use when an E-Stop is pressed.
	 *
	 * @param color  Prompt box background color to use when an E-Stop is pressed.
	 */
	void SetEStopClearedBackgroundColor(const string &color);
	/**
	 * Store the prompt to be used to let the operator know the panel E-Stop
	 * button is pressed.
	 *
	 * @param tag  Prompt tag for Panel E-Stop pressed condition.
	 */
	void SetPanelEStopPressedPromptTag(const string &tag);
	/**
	 * Store the prompt to be used to let the operator know the left pendant E-Stop
	 * button is pressed.
	 *
	 * @param tag  Prompt tag for left pendant E-Stop pressed condition.
	 */
	void SetLeftPendantEStopPressedPromptTag(const string &tag);
	/**
	 * Store the prompt to be used to let the operator know the right pendant E-Stop
	 * button is pressed.
	 *
	 * @param tag  Prompt tag for right pendant E-Stop pressed condition.
	 */
	void SetRightPendantEStopPressedPromptTag(const string &tag);
	/** 
	 * Store the prompt box to use for the E-Stop prompts.
	 *
	 * @params promptBox  Prompt box to use for displaying E-Stop prompts.
	 */
	void SetEStopPromptBox(const INT32 &promptBox);
	/**
	 * Command the system to load the test data.
	 * The wheelbase will also be commanded to move into position.
	 * 
	 * @param checkIfDataLoaded
	 *               Check if the vehicle test data has already been loaded.
	 */
	void LoadTestData(bool checkIfDataLoaded = true);
	/**
	 * Flag to indicate if the vehicle build record has already been retrieved.
	 *
	 * @since Version 1.3
	 */
	bool m_vehicleBuildRecordRead;
	/**
	 * Store if the test component data has been
	 * loaded for the next vehicle test
	 */
	bool m_testDataLoaded;
	/**
	 * Background color to be used when an E-Stop is pressed.
	 */
	string m_eStopPressedColor;
	/**
	 * Background color to be used when an E-Stop is cleared.
	 */
	string m_eStopClearedColor;
	/**
	 * Prompt tag to use when the panel E-Stop is pressed.
	 */
	string m_panelEstopPrompt;
	/**
	 * Prompt tag to use when the left pendant E-Stop is pressed.
	 */
	string m_leftPendantEstopPrompt;
	/**
	 * Prompt tag to use when the right pendant E-Stop is pressed.
	 */
	string m_rightPendantEstopPrompt;
	/** 
	 * Prompt box to use for displaying E-Stop pressed prompts.
	 */
	INT32 m_estopPromptBox;
	/**
	 * Flags to determine if the software should check for the E-Stop
	 * button pressed.
	 */
	vector<bool> m_checkForEstop;
};

#endif	// Mercedes_SYSTEM_MONITOR_INCLUDED

