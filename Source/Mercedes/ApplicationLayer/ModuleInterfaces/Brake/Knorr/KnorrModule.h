//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/ModuleInterfaces/Brake/Include/KnorrModule.h 
//
// FILE DESCRIPTION:
//	Module Interface for Knorr controller.
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
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/ModuleInterfaces/Brake/Include/KnorrModule.h $
// 
// 
//*************************************************************************
#ifndef KnorrModule_h
#define KnorrModule_h
//*************************************************************************
//#include "GenericDCXJ2190Module.h"
#include "GenericABSModuleTemplate.cpp"
#include "ABSValveControl.h"


/**
 * Module interface for the Teves Mk20E ABS module.
 * This class contains all the methods neccessary to send commands and
 * retrieve data from the Teves Mk20E module.
*/
template <class ProtocolFilterType>
class KnorrModule : public GenericABSModuleTemplate<ProtocolFilterType>
{
public:
	/**
	 * Class constructor.
	 * All communication objects will be initialized in this constructor.
	 * 
	 * @since Version 1.0
	 */
	KnorrModule(void);
	/**
	 * Class destructor.
	 * All communication objects will be destroyed in this destructor.
	 * @since Version 1.0
	 */
	virtual ~KnorrModule();
	/**
	 * GetInfo for string types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
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
	 * @since 4 Mar 2003
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
	virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);
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
	 * GetInfo for methods that take no arguments
	 *
	 * @param methodName The method to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName) throw(ModuleException);

    /**
     * Command the module to hold the valves at the specified wheel.
     * 
     * @param wheel  Wheel to hold the valves.
     * 
     * @return Status of commanding the module to hold the valves at the specified wheel.
     */
    virtual BEP_STATUS_TYPE HoldValve(const string &wheel);

	/** 
	 * Initializes the module when using ISO-K
	 * 
	 */ 
	virtual BEP_STATUS_TYPE SendModuleWakeUpMessage(void);

	/**
	 * Initialize the gateway module to allow pass through communication
	 * 
	 */
	virtual BEP_STATUS_TYPE InitializeGateway(void);
protected:
	/**
	 * Initialize hook for the GenericABSModuleTemplate, simply calls
	 * the GenericModule initialize hook.
	 *
	 * @param configNode The XML Config node for the module
	 * @return Status of the operation, in this instance the
	 * value returned by GenericModule's initialize hook.
	 */
	virtual bool InitializeHook(const XmlNode *configNode);
	/**
	 * Reads the wheel speeds from the module.
	 *
	 * @param speeds map of wheel speeds keyed on LFWHEEL, RFWHEEL, etc.
	 * which come out of BepDefs.h
	 * @return The status of the operation.
	 * @throws ModuleException if CheckObjectStatus() fails
	 * @since 2003 12 03
	 */
	virtual BEP_STATUS_TYPE ReadSensorSpeeds(WheelSpeeds_t &speeds, INT16 &numberOfChannels)  throw(ModuleException);

	virtual inline void SetSignalLowTime(const std::string &lowTime)
	{
		SetSignalLowTime(atoi(lowTime.c_str()));
	};
	/**
	 * Save the time to drive the signal low.
	 * 
	 * @param lowTime Length of time to drive the line low.
	 * @since Version 1.0
	 */
	virtual inline void SetSignalLowTime(const INT32 &lowTime)
	{
		m_lineLowTime = lowTime;
	};
	/**
	 * Save the time to drive the signal high.
	 * 
	 * @param highTime Length of time to drive the line high.
	 * @since Version 1.0
	 */
	virtual inline void SetSignalHighTime(const std::string &highTime)
	{
		SetSignalHighTime(atoi(highTime.c_str()));
	};
	/**
	 * Save the time to drive the signal high.
	 * 
	 * @param highTime Length of time to drive the line high.
	 * @since Version 1.0
	 */
	virtual inline void SetSignalHighTime(const INT32 &highTime)
	{
		m_lineHighTime = highTime;
	};
	/**
	 * Save the time to drive the gateway init signal high.
	 * 
	 */
	virtual inline void SetGatewayInitHighTime(int highTime)
	{
		m_gatewayInitHighTime = highTime;
	};
	/**
	 * Save the time to drive the gateway init signal low.
	 * 
	 */
	virtual inline void SetGatewayInitLowTime(int lowTime)
	{
		m_gatewayInitLowTime = lowTime;
	};

	/**
	 * Get the time to drive the gateway init signal high
	 */
	virtual inline int GetGatewayInitHighTime(void)
	{
		return m_gatewayInitHighTime;
	};

	/**
	 * Get the time to drive the gateway init signal low
	 */
	virtual inline int GetGatewayInitLowTime(void)
	{
		return m_gatewayInitLowTime;
	};

	/**
	 * Save the length of time to wait for a response to the wake up message.
	 * 
	 * @param delayTime Amount of time to wait (ms).
	 * @since Version 1.0
	 */
	virtual inline void SetWakeUpResponseDelay(const std::string &delayTime)
	{
		SetWakeUpResponseDelay(atoi(delayTime.c_str()));
	};
	/**
	 * Save the length of time to wait for a response to the wake up message.
	 * 
	 * @param delayTime Amount of time to wait (ms).
	 * @since Version 1.0
	 */
	virtual inline void SetWakeUpResponseDelay(const INT32 &delayTime)
	{
		m_wakeUpResponseDelay = delayTime;
	};
	/**
	 * Save the number of attempts to make to establish communications with the module.
	 * 
	 * @param attempts Number of attempts.
	 * @since Version 1.0
	 */
	virtual inline void SetWakeUpAttempts(const std::string &attempts)
	{
		SetWakeUpAttempts(atoi(attempts.c_str()));
	};
	/**
	 * Save the number of attempts to make to establish communications with the module.
	 * 
	 * @param attempts Number of attempts.
	 * @since Version 1.0
	 */
	virtual inline void SetWakeUpAttempts(const INT32 &attempts)
	{
		m_wakeUpAttempts = attempts;
	};

	/**
	 * Get the length of time to drive the time low.
	 * 
	 * @return Length of time to drive the time low.
	 * @since Version 1.0
	 */
	virtual inline const INT32 &GetSignalLowTime(void)
	{
		return m_lineLowTime;
	};
	/**
	 * Get the length of time to drive the time high.
	 * 
	 * @return Length of time to drive the time high.
	 * @since Version 1.0
	 */
	virtual inline const INT32 &GetSignalHighTime(void)
	{ 
		return m_lineHighTime;
	};
	/**
	 * Get the length of time to wait before looking for a response to the wake up message.
	 * 
	 * @return Length of time to wait for a response.
	 * @since Version 1.0
	 */
	virtual inline const INT32 &GetWakeUpResponseDelay(void)
	{
		return m_wakeUpResponseDelay;
	};
	/**
	 * Get the number of attepts to try to establish module communications.
	 * 
	 * @return Number of attempts.
	 * @since Version 1.0
	 */
	virtual inline const INT32 &GetWakeUpAttempts(void)
	{
		return m_wakeUpAttempts;
	};
	/**
	 * Tag for the wake up message to send to the module.
	 * @since Version 1.0
	 */
	std::string m_wakeUpMessageTag;
	/**
	 * Get the tag for the wake up message to send to the module. 
	 * 
	 * @return Tag for the wake up message.
	 * @since Version 1.0
	 */
	virtual const std::string &GetWakeUpMessageTag(void)
	{
		return m_wakeUpMessageTag;
	};

    /**
	 * Length of time to drive the line low.
	 * @since Version 1.0
	 */
	INT32 m_lineLowTime;
	/**
	 * Length of time to drive the line high.
	 * @since Version 1.0
	 */
	INT32 m_lineHighTime;
	/**
	 * Length of time to drive line high for gateway init
	 */
	int m_gatewayInitHighTime;
	/**
	 * Length of time to drive line low for gateway init
	 */
	int m_gatewayInitLowTime;
	/**
	 * Length of time to wait for a response to the wake up command.
	 * @since Version 1.0
	 */
	INT32 m_wakeUpResponseDelay;
	/**
	 * Number of attempts to establish communications with the module.
	 * @since Version 1.0
	 */
	INT32 m_wakeUpAttempts;

private:

};

//*************************************************************************
#endif  //KnorrModule_h

