//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Module Interface for the Advics ABS.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
// 
//*************************************************************************
#ifndef AdvicsISOABSModule_h
#define AdvicsISOABSModule_h
//-----------------------------------------------------------------------------
#include "GenericABSModuleTemplate.cpp"

//-----------------------------------------------------------------------------
template <class ProtocolFilter>
class AdvicsISOABSModule : public GenericABSModuleTemplate<ProtocolFilter>
{
public:
    AdvicsISOABSModule();
    virtual ~AdvicsISOABSModule();
    /**
     * Returns the name of this module
     */
    virtual string ModuleName();
    /**
     * Establish comms to the module.
     * 
     * @return Status of establishing communications.
     */
    BEP_STATUS_TYPE PerformModuleLinkup();
    /**
     * Reads the part number
     *
     * @param partNumber destination of part number read
     * @returns Status of the communication
     * @since 21 Jan 2003
     * @note Need to figure out how to do multipart
     * reads
     */
    virtual BEP_STATUS_TYPE ReadPartNumber(string &partNumber);
    BEP_STATUS_TYPE PerformFastInitWakeup(void);
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
     * Unlock module security so data can be written to the module.
     * <p><b>Description:</b><br>
     * The security seed will first be read from the module.  Once the seed is retrieved, it will be checked to 
     * determine if module security has already been unlocked.  A seed value of 0 indicates the module security
     * has already been unlocked.  If module security has not been unlocked, the security unlock key will be
     * calculated and sent back to the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> UnlockModuleSecurity<i><x></i>Bytes - where x is the number of bytes in the key. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CalculateSecurityKey() </li>
     *      <li> CommandModule() </li>
     *      </ul>
     * 
     * @return Status of unlocking module security.
     */
    virtual BEP_STATUS_TYPE UnlockModuleSecurity();
    /**
     * Calculate the security access key for the module.
     * <p><b>Description:</b><br>
     * The security unlock key will be generated using the provided seed read from the module.
     * <p>
     * @param seed   Seed to use for calculating the key.
     * @param key    Calculated key for unlocking module security.
     * @return Status of calculating he security unlock key.
     */
    virtual BEP_STATUS_TYPE CalculateSecurityKey(const SerialArgs_t &seed, SerialArgs_t &key);
    /**
     * Method used to perform an ABS reduction or recovery. The Advics module responds to 
     * the valve actuation commands with a speed delta for all 4 wheels. We can use this
     * speed data to check for sensor cross.

     * @param methodName The method to use
     * @param speeds Buffer to fill with the speed delta values
     * @throws ModuleException if the method cannot be found, or the
     *                         method we call throws a module exception
     * @return The status of the operation
     */
    virtual BEP_STATUS_TYPE ReadSensorSpeeds(WheelSpeeds_t &speeds) throw(ModuleException);
protected:
    /**
     * Initialize hook for the GenericABSModuleTemplate, simply calls
     * the GenericModuleTemplate initialize hook.
     *
     * @param configNode The XML Config node for the module
     * @return Status of the operation, in this instance the
     * value returned by GenericModuleTemplate's initialize hook.
     */
    virtual bool InitializeHook(const XmlNode *configNode);
    /**
     * Read module faults
     *
     * @return Status of the operation
     * @param faultCodes The returned fault codes will
     * be filled in here
     * @throws ModuleException if object needed is null
     * @since 21 Jan 2003
     * @note Need to figure out how to do multipart
     * reads
     */
    virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);

private:
    /**
     * Accessor function to get and set byte count
     * 
     * @param byteCount Expected byte count from initial module message
     * 
     * @return Expected byte count from initial module message
     */
    const INT32& InitMessageByteCount(INT32 *byteCount = NULL);
    /**
     * Accessor function to get and set echo message
     * 
     * @param echo   Expected echo message from initial module message
     * 
     * @return Expected echo message from initial module message
     */
    const UINT8& InitMessageEcho(UINT8 *echo = NULL);
    /**
     * Get/Set the number of messages to send to read the part number data from the module.
     * 
     * @param number Number of messages to send to read the part number data.
     * 
     * @return Number of messages to send to read the part number data.
     */
    const INT32& NumberOfPartNumberReads(const INT32 *number = NULL);
    /**
     * Get/Set the number of bytes for each DTC.
     * 
     * @param bytes  Number of bytes for each DTC.
     * 
     * @return Number of bytes for each DTC.
     */
    const UINT8& BytesPerDtc(const UINT8 *bytes = NULL);
    /**
     * Get/Set the index to the first DTC.
     * 
     * @param index  Index to the first DTC.
     * 
     * @return Index to the first DTC.
     */
    const UINT8& DtcStartIndex(const UINT8 *index = NULL);
    /**
     * Get/Set the index of the DTC count
     * 
     * @param index  Index of the DTC count
     * 
     * @return Index of the DTC count
     */
    const UINT8& DtcCountIndex(const UINT8 *index = NULL);
    /**
     * Get/Set the length of time to drive the line low during the init sequence.
     * 
     * @param lowTime Length of time to drive the line low during the init sequence.
     * 
     * @return Length of time to drive the line low during the init sequence.
     */
    const UINT32& SignalLowTime(const UINT32 *lowTime = NULL);
    /**
     * Get/Set the length of time to let the line float high during the init sequence.
     * 
     * @param highTime Length of time to let the line float high during the init sequence.
     * 
     * @return Length of time to let the line float high during the init sequence.
     */
    const UINT32& SignalHighTime(const UINT32 *highTime = NULL);
    /**
     * Get/Set the length of time to wait for the module to respond to the wakeup message.
     * 
     * @param wakeupDelay
     *               Length of time to wait for the module to respond to the wakeup message.
     * 
     * @return Length of time to wait for the module to respond to the wakeup message.
     */
    const UINT32& WakeUpResponseDelay(const UINT32 *wakeupDelay = NULL);
    /**
     * Get/Set the number of attmepts to wake up the module.
     * 
     * @param attempts Number of attmepts to wake up the module.
     * 
     * @return Number of attmepts to wake up the module.
     */
    const INT32& WakeUpAttempts(const INT32 *attempts = NULL);
    /**
     * Method to read the motor relay and valve relay states.
     * 
     * @param motorRelay The location to store the motor relay.
     * @param valveRelay The location to store the valve relay.
     * @return Status of the operation.
     */ 
    BEP_STATUS_TYPE ReadRelayState(bool &motorRelay, bool &valveRelay) throw(ModuleException);

    /**
     * initial message byte count
     */
    INT32 m_initMessageByteCount;
    /**
     * initial message echo
     */
    UINT8 m_initMessageEcho;
    /**
     * The number of messages to send to read the part number data from the module.
     */
    INT32 m_numberOfPartNumberReads;
    /**
     * Number of bytes in each DTC.
     */
    UINT8 m_bytesPerDtc;
    /**
     * Location of the first DTC.
     */
    UINT8 m_dtcStartIndex;
    /**
     * Location the DTC Count
     */
    UINT8 m_dtcCountIndex;
        /** 
     * Length of time to drive the line low during the init sequence 
     */
    UINT32 m_signalLowTime;
    /** 
     * Length of time to float the line high during the init sequence 
     */
    UINT32 m_signalHighTime;
    /** 
     * Length of time to wait for the module to respond to the wake up message 
     */
    UINT32 m_wakeupResponseDelay;
    /** 
     * Number of times to attempt to wake up the module 
     */
    INT32 m_wakeupAttempts;
};


//-----------------------------------------------------------------------------
#endif //AdvicsISOABSModule_h
