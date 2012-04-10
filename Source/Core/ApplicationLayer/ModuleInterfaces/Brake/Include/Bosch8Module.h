//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch8Module.h 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//  Module Interface for Bosch 8 ABS controller.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/Bosch8Module.h $
// 
// 1     10/24/11 dmazur
// made funtion ReadFaults() to be virtual 
//
//1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 4     11/15/04 8:52a Bmeinke
// Added a handler for ReadSpeedDeltas inside GetInfo(WheelSpeeds_t)
// Added the new ESP mehod handlers to the generic GetInfo() method
// 
// 1     10/27/04 5:40p Bmeinke
// 
// 3     10/26/04 12:42p Bmeinke
// ALWAYS put 2 WheelSpeed_t entries into the vector inside GetInfo(
// StartSensorQualityTest)
// Inside StartSensorQualityTest(), we do not get a message for "Starting"
// , so don't look for one.
// Inside StartSensorQualityTest(), consider "failed" to meane "not
// passed"
// Parse the module reply inside ReadFaults() the "hard" way (i.e. no
// response interpretaion: too difficult according to Dan E)
// 
// 2     10/15/04 2:02p Bmeinke
// Modified ReadFaults to manually parse the list of faults
// 
// 1     8/30/04 7:23p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
// 
// 1     8/09/04 9:17a Bmeinke
// Removed the RunPumpMotor() method (already provided in
// GenericABSModuleTemplate.h)
// Added Log( FN_ENTRY, ...) calls to EnterDiagnosticMode()
// 
// 1     12/22/03 12:34p Rwiersem
// Initial version.
// 
//*************************************************************************
#ifndef BOSCH8MODULE_H
#define BOSCH8MODULE_H

#include "../Bosch8/KoreaAbsModuleTemplate.cpp"

/**
 * Module interface for the Bosch8 ABS module.
 * This class contains all the methods neccessary to send commands and
 * retrieve data from the Bosch8 module.
 * 
 * @author Ross Wiersema
 * @see KoreaABSModuleTemplate
 * @see GenericABSModuleTemplate
 * @see GenericModule
 * @since 19 December 2003
 */
template <class ProtocolFilterType>
class Bosch8Module : public KoreaAbsModuleTemplate<ProtocolFilterType>
{
public:

    /**
     * Class constructor.
     * All communication objects will be initialized in this constructor.
     * 
     * @since 19 December 2003
     */
    Bosch8Module(void);

    /**
     * Class destructor.
     * All communication objects will be destroyed in this destructor.
     * @since 19 December 2003
     */
    virtual ~Bosch8Module();

    /**
     * GetInfo for string types
     *
     * @param methodName The method to use
     * @param value The value to use
     * @return Status
     * @throw ModuleException if the method cannot be found
     * @since 18 December 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, string &value) throw(ModuleException);

    /**
     * GetInfo for bool types, it takes a method name and a value and
     * executes the specified method using the value as the parameter
     * to the method.
     *
     * @param methodName The method to use
     * @param value The value to use/fill in
     * @throw ModuleException if the method cannot be found, or the
     * method we call throws a module exception
     * @return The status of the operation
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, bool &value) throw(ModuleException);

    /**
     * GetInfo for float types
     *
     * @param methodName The method to use
     * @param value The value to fill
     * @return Status
     * @throw ModuleException if the method cannot be found
     * @since 18 December 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

    /**
     * GetInfo for int types, it takes a method name and a value and
     * executes the specified method using the value as the parameter
     * to the method. This method can be used for enum data types, such
     * as Corner in this case.
     *
     * @param methodName The method to use
     * @param value The value to use/fill in
     * @throw ModuleException if the method cannot be found, or the
     * method we call throws a module exception
     * @return The status of the operation
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value) throw(ModuleException);

    /**
     * GetInfo for UINT8 types
     * 
     * @param methodName The method to use
     * @param value The value to fill
     * @return Status
     * @since 2 Apr 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);

    /**
     * GetInfo for UINT16 types
     * 
     * @param methodName The method to use
     * @param value The value to fill
     * @return Status
     * @since 2 Apr 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);

    /**
     * GetInfo for Fault types
     *
     * @param methodName The method to use
     * @param value The value to use
     * @return Status
     * @throw ModuleException if the method cannot be found
     * @since 3 Mar 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t & value) throw(ModuleException);

    /**
     * GetInfo for WheelSpeeds_t types, it takes a method name and a value and
     * executes the specified method using the value as the parameter
     * to the method.
     *
     * @param methodName The method to use
     * @param value The value to use/fill in
     * @throw ModuleException if the method cannot be found, or the
     * method we call throws a module exception
     * @return The status of the operation
     */
    virtual BEP_STATUS_TYPE GetInfo(string method, WheelSpeeds_t &value)  throw(ModuleException);

    /**
     * GetInfo for vector<WheelSpeeds_t> types, it takes a method name and a value and
     * executes the specified method using the value as the parameter
     * to the method.
     *
     * @param methodName The method to use
     * @param value The value to use/fill in
     * @throw ModuleException if the method cannot be found, or the
     * method we call throws a module exception
     * @return The status of the operation
     */
    virtual BEP_STATUS_TYPE GetInfo(string method, vector<WheelSpeeds_t> &value)  throw(ModuleException);

    /**
     * GetInfo for methods that take no arguments
     *
     * @param methodName The method to use
     * @return Status
     * @throw ModuleException if the method cannot be found
     * @since 19 December 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName) throw(ModuleException);

    /**
     * GetInfo for methods that take vector<bool> arguments.
     * 
     * @param methodName The method to use
     * @param value The value to use/fill
     * @throws ModuleException if the method cannot be found, or the
     *                         method we call throws a module exception
     * @return The status of the operation
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<bool> &value) throw(ModuleException);

    /**
     * Command the module to perform a power on reset.  The module
     * re-initializes its software for a static electrical check.
     * 
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual BEP_STATUS_TYPE PerformPowerOnReset(void);

    /**
     * Method to command the module to enter diagnostic mode.
     * 
     * @return The status of the operation
     */ 
    BEP_STATUS_TYPE EnterDiagnosticMode() throw(ModuleException);

    /**
     * Read module faults.
     * <p><b>Description:</b><br>
     * Read fault data from the module.  The fault data will be stored in <i>faultCodes</i> for return to the calling
     * function. </li>
     * 
     * @param faultCodes Fault codes read from the module.
     * 
     * @return Status of reading the fault data from the module.
     */
    virtual BEP_STATUS_TYPE UDSReadFaults(FaultVector_t &faultCodes);

    /** 
     * Method to read the faults from the module
     * @param faultCodes Vector to store fault codes fromt the module
     * @return The status of the operation
     */ 
    virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);

protected:

    /**
     * Initialize hook for the Bosch8Module simply calls
     * the KoreaABSModule initialize hook.
     *
     * @param configNode The XML Config node for the module
     * @return Status of the operation, in this instance the
     * value returned by KoreaABSModule's initialize hook.
     */
    virtual bool InitializeHook(const XmlNode *configNode);

    /**
     * Method used to perform an ABS reduction or recovery. The Bosch8 module responds to 
     * the valve actuation commands with a speed delta for all 4 wheels. We can use this
     * speed data to check for sensor cross.
     
     * @param methodName The method to use
     * @param speeds Buffer to fill with the speed delta values
     * @throws ModuleException if the method cannot be found, or the
     *                         method we call throws a module exception
     * @return The status of the operation
     */
    virtual BEP_STATUS_TYPE ABSValveActuation(string methodName, WheelSpeeds_t &speedDeltas) throw(ModuleException);

    /**
     * <p><b>Description:</b><br>
     * Get the index of the DTC count.
     * <p>
     * @return Index of the DTC count.
     */
    virtual const INT32& GetDTCCountIndex(void);
    /**
     * <p><b>Description:</b><br>
     * Get the number of bytes that make up one DTC field.
     * <p>
     * @return Number of bytes for a DTC field.
     */
    virtual const INT32& GetBytesPerDTC(void);
    /**
     * <p><b>Description:</b><br>
     * Get the index for the first byte of the first report DTC.
     * <p>
     * @return Index of the first byte of the first DTC.
     */
    virtual const INT32& GetFirstDTCByteIndex(void);
    /**
     * <p><b>Description:</b><br>
     * Get the address of the first byte of the module fault registers
     * <p>
     * @return Address of the first byte of the module fault registers
     */
    virtual const INT16& GetFirstFaultRegister(void);
    /**
     * <p><b>Description:</b><br>
     * Get the number of fault registers for the module
     * <p>
     * @return Number of fault registers
     */
    virtual const INT32& GetNumberOfFaultRegisters(void);
    /**
     * <p><b>Description:</b><br>
     * Check if a fault read is in progress.
     * <p>
     * @return Flag indicating if a fault read is in progress.
     *          <ul>
     *          <li> True - fault read is in progress, port may be locked. </li>
     *          <li> False - fault read is not in progress, port should not be locked. </li>
     *          </ul>
     */
    const bool& FaultReadInProgress(void);
    /**
     * <p><b>Description:</b><br>
     * Check if fault reads should continue if no faults are recorded in the current fault register/byte.
     * <p>
     * @return  Flag indicating if fault reads should continue when a value of 0x00 is encountered.
     *          <ul>
     *          <li> True - read and check all fault register bytes.</li>
     *          <li> False - stop if the 1st byt in the 1st register is zero. </li>
     *          </ul>
     */
    const bool& GetStopNoDTCs(void);
    /**
     * <p><b>Description:</b><br>
     * Set the flag indicating a fault read is in progress.
     */
    void SetReadFaultsInProgress(void);
    /**
     * <p><b>Description:</b><br>
     * Clear the flag indicating a fault read is in progress.
     */
    void SetReadFaultsComplete(void);
    /**
     * List of Registers used for determining faults.
     */
    XmlNodeMap m_faultByteCodes;
    /**
     * Module configuration data.
     */
    XmlNode *m_moduleConfig;


private:

    /**
     * Method to read the motor relay and valve relay states.
     * 
     * @param motorRelay The location to store the motor relay.
     * @param valveRelay The location to store the valve relay.
     * @return Status of the operation.
     */ 
    BEP_STATUS_TYPE ReadRelayState(bool &motorRelay, bool &valveRelay) throw(ModuleException);

    /**
     * Method to start the sensor quality test and get the results.
     * @param min Vector of minimum sensor speeds for each wheel.
     * @param max Vector of maximum sensor speeds for each wheel.
     * @return The status of the operation
     */ 
    BEP_STATUS_TYPE StartSensorQualityTest(WheelSpeeds_t &min,WheelSpeeds_t &max) throw(ModuleException);

    /**
     * <p><b>Description:</b><br>
     * Store the index into the response where the number of recorded DTCs resides.
     * <p>
     * @param index  Index into the response of the DTC count.
     */
    void SetDTCCountIndex(const INT32 &index);
    /**
     * <p><b>Description:</b><br>
     * Store the number of bytes in one DTC field.
     * <p>
     * @param bytesPerDTC
     *               Number of bytes in one DTC field.
     */
    void SetBytesPerDTC(const INT32 &bytesPerDTC);
    /**
     * <p><b>Description:</b><br>
     * Store the index of the first byte of the first DTC reported.
     * <p>
     * @param firstDTCByteIndex
     *               Index of the first byte of the first DTC.
     */
    void SetFirstDTCByteIndex(const INT32 &firstDTCByteIndex);
    /**
     * <p><b>Description:</b><br>
     * Store the address of the first byte of the module fault registers.
     * <p>
     * @param firstFaultRegister
     *               Address of the first byte of the module fault registers.
     */
    void SetFirstFaultRegister(const INT16 &firstFaultRegister);
    /**
     * <p><b>Description:</b><br>
     * Set the number of fault registers for the module.
     * <p>
     * @param numberOfFaultRegisters
     *               The number of fault registers
     */
    void SetNumberOfFaultRegisters(const INT32 &numberOfFaultRegisters);
    /**
     * <p><b>Description:</b><br>
     * Store the flag indicating fault reads should stop when 0x0000 is encountered for fault data.
     * <p>
     * @param stopDTCRead Flag indicating if fault reads should stop when a value of 0x0000 is read from a fault register.
     */
    void SetStopNoDTCs(const string &stopDTCRead);
    /**
     * <p><b>Description:</b><br>
     * Store the flag indicating if the DTC format is UDS
     * <p>
     * @param setValue Flag indicating if the DTC format is UDS.
     */
    void SetDTCFormatUDS(const bool &setValue);
    /**
     * <p><b>Description:</b><br>
     * Get the flag indicating if the DTC format is UDS
     * <p>
     * @return  Flag indicating if the DTC format is UDS
     *          <ul>
     *          <li> True - DTCs read from module are in UDS format.</li>
     *          <li> False - DTCs read from module are not in UDS format </li>
     *          </ul>
     */
    bool GetDTCFormatUDS(void);
    /**
     * Index into the response where the number of reported DTCs resides.
     */
    INT32 m_dtcCountIndex;
    /**
     * Number of bytes that make up one DTC field.
     */
    INT32 m_bytesPerDTC;
    /**
     * Index of the first byte of the first reported DTC.
     */
    INT32 m_firstDTCByteIndex;
    /**
     * Flag to indicate the object is reading faults from the  module and most likely has the port locked.
     */
    bool m_readingFaults;
    /**
     * The address of the first byte for reading the module fault registers.
     */
    INT16 m_firstFaultRegister;
    /**
     * The number of fault registers the module contains.
     */
    INT32 m_numberOfFaultRegisters;
    /**
     * Flag to indicate to stop fault reads if the 1st fault byte is 0.
     */
    bool m_stopNoDTCs;
    /**
     * Flag to indicate if the DTC format is UDS.
     */
    bool m_dtcFormatUds;

};

#endif
