//******************************************************************************
// Description:
//  Interface class for the BEP Data Collector.
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
//    $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ServicesLayer/Servers/PlantHostOutbound/Include/DAQInterface.h $
// 
// 1     11/01/07 3:22p Derickso
// Made LoadAdditionalConfigurationItems virtual so child classes could
// initialize properly.
// Updated strategy for processing test results.
// 
// 5     10/31/07 10:03a Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     5/01/07 1:03p Rwiersem
// Changes for the 07050201 core release:
// 
// Updated to use the HostInterface class.
// 
// 3     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/18/06 2:12p Rwiersem
// Made testing classes friends.  A SerialChannel object is now passed to
// the constructor.  This allows us to use a MockSerialChannel for
// testing.  Added LoadAdditionalConfigurationItems() which is used to
// reload the configuration without restarting the process.
//
// 2     3/30/06 1:41a Cward
// Updated for sending machine faults to the DAQ. PE #80
//
// 1     7/21/04 10:18a Cward
//
// 2     5/25/04 4:32p Cward
// Removed ACK and NAK definitions
//
// 1     1/28/04 12:47p Cward
//
// 1     8/21/03 9:08a Derickso
// 05WK CCRT Project
//
// 2     8/12/03 2:57p Khykin
// Updated with new directory update changes.
//
// 1     7/08/03 1:00p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 1     6/12/03 4:08p Khykin
// Initial checkin.
//
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//
// 1     7/29/02 8:22a Dan.erickson
// Created.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef DAQInterface_h
#define DAQInterface_h
//==============================================================================
#include "HostInterface.h"

/**
 * Constants to use in serial communications.
 */
#define TEST_RESULT_STX        '\x002'
#define LOSS_COMPENSATION_STX  '&'
#define MACHINE_FAULT_STX      '\x025'

/**
 * This thread is spun up by the PlantHostOutbound if the plant is equipped 
 * with a DataCollector system.  The DAQInterface class will invoke the 
 * PlantHostOutbound methods for building the test result string to be sent to 
 * the DataCollector after it has been signaled that new test results have been 
 * posted.  The DAQInterface class will then add the appropriate wrappers to 
 * the message and then call the PlantHostoutbound method for transmitting the 
 * message to the external host, which for this class is the DataCollector.
 *
 * @author Dan Erickson
 */
class DAQInterface : public HostInterface
{
    friend class DAQInterfaceTest;

public:
    /**
     * Class constructor.
     */
    DAQInterface();
    /**
     * Class destructor.
     */
    virtual ~DAQInterface();
    /**
     * Main Run method to control the process.
     *
     * @param terminateFlag Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
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
     * <b>Description:</b>
     * Request to write a data tag.
     *
     * @param node The data tag and new value to be written.
     * 
     * @return BEP_SUCCESS_RESPONSE if the new value was written successfully.
     *         BEP_ERROR_RESPONSE if the data tag was found, but the tag data
     *         mutex could not be locked.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was found and the tag 
     *         data mutex was locked, but the data could not be written.
     *         BEP_UNSUPPORTED_RESPONSE if the data tag could not be found.
     */
    virtual const string Write(const XmlNode *node);

protected:
    /**
     * Store additional items from the config file to complete the 
     * initialization of the host interface.
     * 
     * @param config Configuration data used to complete the initialization of 
     *               the host interface.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *config);
    /**
     * Determine if the test results should be processed and if a result string 
     * should be generated.  The test result data will be analyzed to determine 
     * if processing should continue.  If the VehicleBuild node is not in the 
     * test results, this is assumed to be a Loss Compensation result.  A 
     * result string should not be generated for the host system.  If the 
     * PerformAbort node is not set to Perform, there are no valid results to 
     * report to the host system.
     * 
     * @param results Test results to be analyzed.
     * 
     * @return Flag indicating if a result string should be generated.
     *         <ul>
     *         <li> True - Generate a result string to send to the host system. </li>
     *         <li> False - Skip generating a result string. </li>
     *         </ul>
     */
    virtual bool ShouldProcessTestResults(const XmlNode *results);
    /**
     * Generate the result string to be sent to the host system.
     * 
     * @param testResults Test result data to be used for generating the result 
     *                    string for the host system.
     * 
     * @return Result string to be transmitted to the host system.
     */
    virtual string GenerateHostResultString(const XmlNode *testResults);
    /**
     * Process the current fault data that was written to us from the 
     * PlanHostOutbound server.  The fault code will be formatted and sent to 
     * the DAQ.  A single fault is sent in each message, so the message format 
     * is:
     * 
     * Field    Data    Explanation
     *     1    0x25,   % (Diag.String Identifier)
     *     2    XXXX,   Machine Number
     * 
     *     3    XXXY,   Diagnostic Fault Code,
     *                  where XXX = Diag. Fault Number
     *                          Y = + ($2B) if fault set, or
     *                            = - ($2D) if fault cleared
     *     4       X    Checksum
     *     5     $0D    CR
     *     6     $0A    LF
     */
    void ProcessFaults(void);
    /**
     * Transmit the result string to the plant host system system.
     *
     * @param result The result string to be sent.
     */
    virtual void SendResultToHost(const string &result);
    /**
     * Get the header information for machine fault messages.
     *
     * @return Test result message header.
     */
    const string GetMachineFaultHeader(void);
    /**
     * Get the header information for vehicle test result messages.
     *
     * @return Test result message header.
     */
    const string GetTestResultHeader(void);
    /**
     * Get the loss compensation message header information.
     *
     * @return Loss compensation header.
     */
    const string GetLossCompensationHeader(void);
    /**
     * Store/Get the fault code to be processed.
     * 
     * @param faultCode Fault code to be procesed.
     * 
     * @return Fault code to be procesed.
     */
    const INT32& FaultCode(const INT32 *faultCode = NULL);
    /**
     * Get/Set the length of the machine number field in the result string.
     * 
     * @param length Length of the machine number field in the result string.
     * 
     * @return Length of the machine number field in the result string.
     */
    const INT32& MachineNumberLength(const INT32 *length = NULL);
    /**
     * Store/Retrieve the map for the loss compensation result string.
     *
     * @param mapList Filename for the loss comp map to use.
     */
    const XmlNodeMap& LossCompResultStringMap(const XmlNode* mapList = NULL);
    /**
     * Set the header information for the different types of result strings we 
     * are capable of sending.
     */
    void SetResultStringHeaders(void);
    /**
     * Get/Set the measurement system being used for calculations.
     * 
     * @param measurementSystem Current measurement system (E)nglish, (M)metric.
     * 
     * @return Current measurement system (E)nglish, (M)metric.
     */
    const string& MeasurementSystem(const string *measurementSystem = NULL);
    /**
     * Store/retrieve the flag indicating if a fault is ready to be processed.
     * 
     * @param readyToProcess Flag indicating if a fault code is ready to be 
     *                       processed.
     * 
     * @return Flag indicating if a fault code is ready to be processed.
     */
    const bool& FaultReadyToProcess(const bool *readyToProcess = NULL);

private:
    /**
     * Flag indicating if a fault code is ready to be processed.
     */
    bool m_faultReadyToProcess;
    /**
     * Length of the machine number field in the result string.
     */
    INT32 m_machineNumberLength;
    /**
     * Fault code to be processed.
     */
    INT32 m_faultCode;
    /**
     * Test result header information.
     */
    string m_testResultHeader;
    /**
     * Loss compensation result string header information.
     */
    string m_lossCompensationHeader;
    /**
     * machine fault string header information.
     */
    string m_machineFaultHeader;
    /**
     * The current selected measurement system.
     */
    string m_measurementSystem;
    /**
     * Map to build the loss compensation result string.
     */
    XmlNodeMap m_lossCompensationMap;
};

#endif  // DAQInterface_h
