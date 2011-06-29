//*************************************************************************
// FILE:
//    $Header:  $
//
// FILE DESCRIPTION:
//	Module Interface for INS Module.
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
#ifndef InsModule_h
#define InsModule_h
//=============================================================================
#include "GenericModuleTemplate.cpp"

template <class ProtocolFilterType>
class InsModule : public GenericModuleTemplate<ProtocolFilterType>
{
public:
    /**
     * Class contructor.
     */
    InsModule();
    /**
     * Class Destructor.
     */
    virtual ~InsModule();
    /**
     * Initializes the gateway module when using ISO-K
     * <br><p>
     * <b>Description:</b><br>
     * The wake up message will be sent to the module after the line is asserted for the required amount of time.
     * Timing is extremely critical for establishing communications, so most of the normal work of the protocol
     * filter is performed in this fundtion.
     * <br><p>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *		<li> InitializeGateway - Message to send to the gateway to establish communications. </li>
     *		<li> InitializeGatewayStep2 - Second message to send to the gateway to establish communications. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *		<li> GatewayInitLowTime() </li>
     *		<li> GatewayInitHighTime() </li>
     *		<li> EstablishComms() </li>
     *		<li> WakeUpResponseDelay() </li>
     *		<li> WakeUpAttempts() </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions Called:</b>
     *      <ul>
     *		<li> LockPort() </li>
     *		<li> GetMessage() </li>
     *		<li> GetBinaryMssg() </li>
     *		<li> SendMessage() </li>
     *		<li> GetResponse() </li>
     *		<li> UnlockPort() </li>
     *      </ul>
     * 
     * @return Status of waking up the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Gateway Module comms were established successfully. </li>
     *         <li> BEP_STATUS_FAILURE - Gateway Module comms could not be established. </li>
     *         </ul>
     */
	BEP_STATUS_TYPE InitializeGateway(void);
    /**
     * Initializes the module when using ISO-K
     * <br><p>
     * <b>Description:</b><br>
     * The wake up message will be sent to the module after the line is asserted for the required amount of time.
     * Timing is extremely critical for establishing communications, so most of the normal work of the protocol
     * filter is performed in this fundtion.
     * <br><p>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ModuleWakeUp - Message used to wake up the module and establish communications. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> SignalLowTime() </li>
     *      <li> SignalHighTime() </li>
     *      <li> WakeUpResponseDelay() </li>
     *      <li> WakeUpAttempts() </li>
     *      <li> BposSleep() </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions Called:</b>
     *      <ul>
     *      <li> LockPort() </li>
     *      <li> UnlockPort() </li>
     *      <li> GetMessage() </li>
     *      <li> GetBinaryMssg() </li>
     *      <li> AddChecksumToMessage() </li>
     *      <li> EstablishComms() </li>
     *      <li> GetResponse() </li>
     *      </ul>
     * 
     * @return Status of waking up the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Module comms were established successfully. </li>
     *         <li> BEP_STATUS_FAILURE - Module comms could not be established. </li>
     *         </ul>
     */
    virtual BEP_STATUS_TYPE SendModuleWakeUpMessage(void);
    /**
     * Update the K-factor for the tacograph function.
     * <br><p>
     * <b>Description:</b><br>
     * The K-factor will be written to the module.  All the steps necessary to establish communications
     * with the module and write the k-factor to the module will be performed in this function.
     * <br><p>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *		<li> FinalizeDiagnostics - Command the module to finalize communications. </li>
     *		<li> EndCommunications - Command the module to end communications. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *		<li> SendModuleWakeUpMessage() </li>
     *		<li> EnterDiagnosticMode() </li>
     *		<li> UnlockModuleSecurity() </li>
     *		<li> WriteKFactor() </li>
     *		<li> CommandModule() </li>
     *		<li> ConvertStatusToResponse() </li>
     *      </ul>
     * 
     * @param kFactor K-factor to write to the module for calibrating the tacograph feature.
     * 
     * @return Status of the operation.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - K-factor was updated successfully. </li>
     *         <li> BEP_STATUS_FAILURE - K-factor could not be updated. </li>
     *         </ul>
     */
	BEP_STATUS_TYPE UpdateKFactor(UINT16 &kFactor, string tacType);

protected:
	/**
	 * Initialize hook for the GenericABSModuleTemplate, simply calls
	 * the GenericModule initialize hook.
	 * <br><p>
     * <b>Description:</b><br>
     * Store any necessary information from the configuration file into member variables for quick/easy retrieval.
     * Any necessary initialization will also be performed for the module interface to function corectly.  The keep 
     * alive message thread will also be initialized at this point.
     * <br><p>
     * 
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> SignalLowTime - Amount of time in ms to drive the line low to establish comms. </li>
     *      <li> SignalHighTime - Amount of time in ms to float the line high to establish comms. <li>
     *      <li> WakeUpResponseDelay - Amount of time in ms to wait for the module to respond to the wake up command. </li>
     *      <li> WakeUpAttempts - Number of times to attempt to establish comms with the module. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> BposReadInt() </li>
     *      <li> SignalLowTime() </li>
     *      <li> SignalHighTime() </li>
     *      <li> WakeUpResponseDelay() </li>
     *      <li> WakeUpAttempts() </li>
     *      <li> GenericModuleTemplate::InitializeHook() </li>
     *      </ul>
     * 
	 * @param configNode The XML Config node for the module
	 * @return Status of the operation, in this instance the
	 * value returned by GenericModule's initialize hook.
	 */
	virtual bool InitializeHook(const XmlNode *configNode);

private:
    /**
     * Unlock module security so data can be written to the module.
     * <br><p>
     * <b>Description:</b><br>
     * 
     * <br><p>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSecuritySeed - Read the security seed from the module. </li>
     *      <li> UnlockSecurity - Unlock module security with the calculated key. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> CalculateSecurityKey() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      <li> CommandModule() </li>
     *      <li> ConvertStatusToResponse() </li>
     *      </ul>
     * 
     * @return Status of the operation.
     */
	virtual BEP_STATUS_TYPE UnlockModuleSecurity(string tacType);
    /**
     * Calculate the key to unlock module security using the seed provided.
     * <br><p>
     * <b>Description:</b><br>
     * 
     * <br><p>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      </ul>
     * 
     * @param seed   Security seed read from the module.
     * 
     * @return Security unlock key to be sent to the module.
     */
	SerialArgs_t CalculateSecurityKey(const UINT16 &seed, string tacType);
    /**
     * Write the current date to the module.
     * <br><p>
     * <b>Description:</b><br>
     * The current date information will be retrieved from the system and formatted for the module.
     * The format consists of the month number, day of the month number, decades since 1990 and the
     * year in the current decade.
     * <br><p>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> WriteCurrentDate - Write the current date to the module. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>      
     *      <li> ConvertStatusToResponse() </li>
     *      </ul>
     * 
     * @return Status of updating the module date.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Current date was successfully written to the module. </li>
     *         <li> BEP_STATUS_FAILURE - Current date could not be written to the module. </li>
     *         </ul>
     */
    BEP_STATUS_TYPE UpdateModuleDate(void);
    /**
     * Update the k calibration constant in the module.
     * <br><p>
     * <b>Description:</b><br>
     * The current k-factor will be read in the module as there is data in the same mesage that must be written 
     * back to the module in order to not screw anything up.  The k-factro will be updated and written back to
     * the module along with the other data that was read.
     * <br><p>
     * 
     * <b>Message Tags:</b>
     *      <ul>
     *		<li> ReadKFactorInit - Command the module to begin reading k-factor data from the module. </li>
     *		<li> ReadKData - Read the k-factor data from the module. </li>
     *		<li> StopKFactorRead - Tell the module we are done reading k-factor data. </li>
     *		<li> WriteKFactorInit - Inform the module we will be writing k-factor data back to the module. </li>
     *		<li> WriteKFactors - Write a set of k-factor data to the module. </li>
     *		<li> KFactorWriteComplete - Tell the module we are done writing k-factor data. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *		<li> CommandModule() </li>
     *		<li> ReadModuleData() </li>
     *		<li> ConvertStatusToResponse() </li>
     *      </ul>
     * 
     * @param kFactor Constant to write to the module.
     * 
     * @return Status of updating the calibration constant in the module.
     */
	BEP_STATUS_TYPE WriteKFactor(UINT16 &kFactor);/**
	 * Update the k calibration constant in the module.
	 * <br><p>
	 * <b>Description:</b><br>
	 * The current k-factor will be read in the module as there is data in the same mesage that must be written 
	 * back to the module in order to not screw anything up.  The k-factro will be updated and written back to
	 * the module along with the other data that was read.
	 * <br><p>
	 * 
	 * <b>Message Tags:</b>
	 *      <ul>
	 *		<li> ReadKData - Read the k-factor data from the module. </li>
	 *		<li> WriteKFactors - Write a set of k-factor data to the module. </li>
	 *      </ul>
	 * 
	 * <b>Functions Called:</b>
	 *      <ul>
	 *		<li> CommandModule() </li>
	 *		<li> ReadModuleData() </li>
	 *		<li> ConvertStatusToResponse() </li>
	 *      </ul>
	 * 
	 * @param kFactor Constant to write to the module.
	 * 
	 * @return Status of updating the calibration constant in the module.
	 */
	BEP_STATUS_TYPE WriteKFactorLtc(UINT16 &kFactor);
    /**
     * Set/Get the time in ms to drive the signal low.
     * 
     * @param lowTime Length of time (in ms) to drive the line low.
     * 
     * @return Length of time (in ms) to drive the line low.
     */
    virtual const INT32& SignalLowTime(INT32 *lowTime = NULL);
    /**
     * Set/Get the time in ms to drive the signal high.
     * 
     * @param highTime Length of time (in ms) to drive the line high.
     * 
     * @return Length of time (in ms) to drive the line high.
     */
    virtual const INT32& SignalHighTime(INT32 *highTime = NULL);
    /**
     * Set/Get the time in ms to drive the signal low for the gateway module.
     * 
     * @param lowTime Length of time (in ms) to drive the line low for the gateway module.
     * 
     * @return Length of time (in ms) to drive the line low for the gateway module.
     */
    virtual const INT32& GatewaySignalLowTime(INT32 *lowTime = NULL);
    /**
     * Set/Get the time in ms to drive the signal high for the gateway module.
     * 
     * @param highTime Length of time (in ms) to drive the line high for the gateway module.
     * 
     * @return Length of time (in ms) to drive the line high for the gateway module.
     */
    virtual const INT32& GatewaySignalHighTime(INT32 *highTime = NULL);
    /**
     * Set/Get the length of time (in ms) to wait for a response to the wake up message.
     * 
     * @param delayTime Length of time (in ms) to wait for module to respond.
     * 
     * @return Length of time (in ms) to wait for module to respond.
     */
    virtual const INT32& WakeUpResponseDelay(INT32 *delayTime = NULL);
    /**
     * Set/Get the number of attempts to make to establish communications with the module.
     * 
     * @param attempts Number of attempts.
     * 
     * @return Number of attempts to make to establish comms with the module.
     */
    virtual const INT32& WakeUpAttempts(INT32 *attempts = NULL);
    /**
     * Tag for the wake up message to send to the module.
     */
    string m_wakeUpMessageTag;
    /**
     * Length of time in ms to drive the line low.
     */
    INT32 m_lineLowTime;
    /**
     * Length of time to drive the line high.
     */
    INT32 m_lineHighTime;
    /**
     * Length of time in ms to drive the line low for the gateway module.
     */
    INT32 m_gatewayLineLowTime;
    /**
     * Length of time to drive the line high for the gateway module.
     */
    INT32 m_gatewayLineHighTime;
    /**
     * Length of time to wait for a response to the wake up command.
     */
    INT32 m_wakeUpResponseDelay;
    /**
     * Number of attempts to establish communications with the module.
     */
    INT32 m_wakeUpAttempts;

};
//=============================================================================
#endif  //InsModule_h
