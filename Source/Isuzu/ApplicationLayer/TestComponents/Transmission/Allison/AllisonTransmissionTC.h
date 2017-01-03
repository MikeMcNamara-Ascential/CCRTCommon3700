//*************************************************************************
// FILE:
//    $Header:
//
// FILE DESCRIPTION:
//  Allison transmission Test Component.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#ifndef AllisonTransmissionTC_h
#define AllisonTransmissionTC_h
//*************************************************************************
#include "GenericTransmissionTCTemplate.cpp"

/**
 * Test component to test the Allison Transmission..
 * 
 * @author 
 * @see GenericTransmissionTCTemplate
 */
template <class VehicleModuleType>
class AllisonTransmissionTC : public GenericTransmissionTCTemplate<VehicleModuleType>
{
public:
    /**
     * Class constructor.
     */
    AllisonTransmissionTC();
    /**
     * Class destructor.
     */
    virtual ~AllisonTransmissionTC();
    /**
	 * Sequence a test step.
	 * <p><b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * This is the method that will handle the execution of each test step that has been specified by the system for each
	 * test objective that is passed to it.  This must be overloaded to execute the component tests.
	 * <br><p>
     * 
	 * @param value    This is the value that was commanded by the test sequencer.
	 * @return  The result of the test step that was executed.
     */
    virtual const string CommandTestStep(const string &value);

protected:
    /**
	 * Initialize the test component.
	 * <p><b>Category:</b> Utility
	 * <br><p>
	 * <b>Description:</b><br>
	 * Perform any test component specific initialization.  The market type will be evaluated to determine what type of 
	 * engine controller module interface to create.
	 * <br><p>
     * 
	 * @param config Configuration file to use for initialization.
     */
    virtual void InitializeHook(const XmlNode *config);

    /**
     * Prompts driver for a requested state.
     * <p><b>Category:</b> Utility
     * <p><b>Description:</b><br>
     * The operator will be prompted to place the transmission into the requested state.  The transmission state will
     * then be monitored to ensure the operator has placed the transmission in the requested state.
     * <p>
     * <b>Module Interface Functions:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> TimeRemaining() </li>
     *      <li> StatusCheck() </li>
     *      <li> GetDataTag() </li>
     *      <li> DisplayPrompt() </li>
     *      <li> RemovePrompt() </li>
     *      <li> GetPromptBox() </li>
     *      <li> GetPrompt() </li>
     *      <li> GetPromptPriority() </li>
     *      </ul>
     * 
     * @param requestedState
     *               State in which the transmission is to be placed.
     * @param promptForState
     *               Prompt box in which to display the operator instructions.
     * @param currentStateTag
     *               The message tag to read the current transmission state from the module.
     * @param minimumSuccessiveReads
     *               The amount of successive reads where the current state matches the requested state.
     * @param successiveReadDelay
     *               The delay in milliseconds between successive reads.
     * 
     * @return Flag indicating if the driver was able to place the transmission in the requested state.
     */
    virtual bool RequestStateByPrompt(string requestedState, 
                              string promptForState,
                              string currentStateTag, 
                              INT32 minimumSuccessiveReads, 
                              INT32 successiveReadDelay);



private:

      virtual string ClearFaults(void);
      virtual string ReadFaults(void);
        /**
     * Verify PRNDL display, switches, TIPM Park Neutral state and gear.
     * <p>
     * <b>Category:</b> Test Step
     * <p>
     * <b>Description:</b><br>
     * The shift lever will be tested to ensure the switches and display are consistent.
     * <p>
     * 
     * @return Result of testing the PRNDL position.
     */
    virtual string ShiftLeverTest(void);

    /** List of PRNDL positions to check for */
    XmlNodeMap m_prndlPositions;
};
//*************************************************************************
#endif  //AllisonransmissionTC_h
