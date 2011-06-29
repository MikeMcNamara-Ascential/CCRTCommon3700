//******************************************************************************
// Description:
//	IGenericModule is a generic interface for communication
//	with a vehicle module.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/IGenericModule.h $
// 
// 2     11/16/07 10:24a Derickso
// Updated comments to doxygen format.
// 
// 1     11/01/07 1:54p Derickso
// Corrected comments.
// Added optional comm object for CommandModule.
// 
// 8     10/09/07 1:56p Derickso
// Updated CommandMOdule to accept an optional IProtocolFilter to use
// instead of using m_protocolFilter.
// 
// 7     10/01/07 2:12p Derickso
// Corrected header comments.
// 
// 4     8/17/05 8:10p Cward
// Added pure virtual definitions for Read Module Data methods and Command
// Module method. Moved Read Vin method from protected to public.
//
// 3     8/18/04 11:05p Cward
// changed angle brakets to quotes in includes of non system files
//
// 2     6/09/04 10:44p Cward
// Added new standard Header comment block
//
// 1     12/21/03 6:32p Cward
//
// 2     10/27/03 1:31p Bmeinke
// Fixed ParkBrake / ParkPawl test failures
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef IGENERICMODULE_H
#define IGENERICMODULE_H
//==============================================================================
#include <string>
#include <vector>
#include "BepDefs.h"
#include "XmlNode.h"
#include "SerialDefs.h"
#include <map>
#include "ModuleException.h"
#include "IProtocolFilter.h"

//-----------------------------------------------------------------------------
/**
 * FaultCode_t as string wrapper class, typedef is not sufficient.
 */
class FaultCode_t : public string
{
public:
	FaultCode_t(string input):string(input)
		{}
};

//-----------------------------------------------------------------------------
/**
 * FaultCode_t as string wrapper class, typedef is not sufficient.
 */
class FaultCodeStatus_t
{
public:
	FaultCodeStatus_t(string code, string status)
    {
        m_code = code;
        m_status = status;
    }
    string m_code;
    string m_status;
};

//-----------------------------------------------------------------------------
/**
 * Typedef for a vector of FaultCode_ts.
 */
typedef vector<FaultCode_t> FaultVector_t;


//-----------------------------------------------------------------------------
/**
 * Typedef for a vector of FaultCodeStatus_ts.
 */
typedef vector<FaultCodeStatus_t> FaultStatusVector_t;

//-----------------------------------------------------------------------------
/**
 * Typedef for a map of wheel speed data.  It uses keys found in BepDefs.h, ex: LFWHEEL, RFWHEEL, etc.
 */
typedef map<int,double> WheelSpeeds_t;

//-----------------------------------------------------------------------------
/**
 * IGenericModule is a generic interface for communication
 * with a vehicle module.
 *
 * @class IGenericModule
 * @since 5 Feb 2003
 */

class IGenericModule {

public:
	/**
	 * This constructor will build a GenericModule with no protocol filter object.
	 */
	IGenericModule(){};

	/**
	 * Class Destructor
	 */
	virtual ~IGenericModule(){};

	/**
     * Initialize the module interface.
     * <p><b>Description:</b><br>
     * Initialize the module interface.  Configuration data should be saved into member variables and 
     * any protocol filters created should also be initialized.
	 * <p>
	 * @param configNode  Configuration data to use for initializing the module interface.
	 */
	virtual bool Initialize (const XmlNode *configNode) = 0;

	/**
     * Get the name of the module interface class.
     * <p><b>Description:</b><br>
     * Return a string representing the name of the module interface class.
	 * <p>
	 * @returns Name of the module interface class.
	 */
	virtual string ModuleName() = 0;

	/**
	 * GetInfo for string types.
     * <p><b>Description:</b><br>
     * String type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, string &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for a vector of string types.
     * <p><b>Description:</b><br>
     * A vector of string type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<string> &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for bool types
     * <p><b>Description:</b><br>
     * Boolean type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, bool &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for a vector of bool types
     * <p><b>Description:</b><br>
     * A vector of Boolean type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<bool> &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for integer types
     * <p><b>Description:</b><br>
     * Integer type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for a vector of integer types
     * <p><b>Description:</b><br>
     * A vector of integer type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<int> &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for a fault data types
     * <p><b>Description:</b><br>
     * Fault type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for float types
     * <p><b>Description:</b><br>
     * Float type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for a vector of float types
     * <p><b>Description:</b><br>
     * A vector of float type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<float> &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for WheelSpeeds_t types
     * <p><b>Description:</b><br>
     * Wheel speed data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, WheelSpeeds_t &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for a vector of WheelSpeeds_t types
     * <p><b>Description:</b><br>
     * A vector of wheel speed data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<WheelSpeeds_t> &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for methods that take no arguments
     * <p><b>Description:</b><br>
     * The module will be commanded to perform an action.
	 * <p>
	 * @param methodName The method to use
	 * @return Status of commanding the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for UINT8 types
     * <p><b>Description:</b><br>
     * UINT8 type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for vectors of UINT8 types
     * <p><b>Description:</b><br>
     * A vector of UINT8 type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT8> &value)
		throw(ModuleException) = 0;


	/**
	 * GetInfo for UINT16 types
     * <p><b>Description:</b><br>
     * UINT16 type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value)
		throw(ModuleException) = 0;

	/**
	 * GetInfo for vectors of UINT16 types
     * <p><b>Description:</b><br>
     * A vector of UINT16 type data will be read from the module and returned to the calling function.
	 * <p>
	 * @param methodName Data item to read from the module.
	 * @param value The value read from the module
	 * @return Status of reading the data item from the module.
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT16> &value)
		throw(ModuleException) = 0;
	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, string &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;

	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, bool &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;

	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, float &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;

	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, INT32 &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;

	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, UINT8 &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;

	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, UINT16 &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;
	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, UINT32 &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;
	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<bool> &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;
	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<string> &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;
	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<UINT32> &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;
	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<UINT16> &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;
	/**
     * Read data from the module.
     * <p><b>Description:</b><br>
	 * Read data from the module using the message tag provided.
	 *
	 * @param messageTag Message tag to use for getting data from the module.
	 * @param data       Data read from the module.
	 * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<INT32> &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;
    /**
     * Read data from the module.
     * <p><b>Description:</b><br>
     * Read data from the module using the message tag provided.
     * 
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
     */
	virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<double> &data,
										   SerialArgs_t *args = NULL,
										   SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;
    /**
     * Read data from the module.
     * <p><b>Description:</b><br>
     * Read data from the module using the message tag provided.
     * 
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Message received from the module.
     * @param commObject Communication object to use to talk to the module.  
     *                   If this is not provided, m_protocolFilter will be used.
	 * @return Status of reading the requested data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<float> &data,
                                           SerialArgs_t *args = NULL,
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL) = 0;

    /**
     * Send a command to the module.
     * <p><b>Description:</b><br>
     * Send the module a command to perform some work.  Note that this method just looks for a positive response
     * from the module.  No other data is evaluated.
     * 
     * @param command    Command to send to the module.
     * @param args       Data to be sent to the module with the command.
     * @param rawData    Response from the module.
     * @param commObject Communication object to use to send the message.  If NULL, m_protocolFilter will be used.
     * 
     * @return Status of commanding the module.
     */
    virtual BEP_STATUS_TYPE CommandModule(const string command, 
                                          SerialArgs_t *args = NULL, 
                                          SerialString_t *rawData = NULL, 
                                          IProtocolFilter *commObject = NULL) = 0;

	/**
     * Read switch data from the module.
     * <p><b>Description:</b><br>
	 * Read switches from module. This will normally be called by a SwitchMonitor from another thread, which is why it
	 * is passed it's own IProtocolFilter.
	 * <p>
	 * @param filter IProtocolFilter to use for comms.
	 * @return Status of evaluating the swith data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadSwitches(IProtocolFilter *filter) = 0;

	/**
     * Launch the keep alive thread.
     * <p><b>Description:</b><br>
     * The keep alive thread will be launched to periodically send tester present messages to the module.
	 * <p>
	 * @return Status of starting the keep alive thread.
	 */
	virtual BEP_STATUS_TYPE StartModuleKeepAlive() = 0;

	/**
     * Stop the keep alive thread.
     * <p><b>Description:</b><br>
     * The keep alive thread will be commanded to stop sending tester present messages and exit.
	 * <p>
	 * @return Status of stopping the keep alive thread.
	 */
	virtual BEP_STATUS_TYPE StopModuleKeepAlive() = 0;

	/**
     * Launch the switch monitor thread.
     * <p><b>Description:</b><br>
     * Switch data will be monitored and processed.
	 * <p>
	 * @return Status of launching the switch monitor thread.
	 */
	virtual BEP_STATUS_TYPE StartSwitchMonitor() = 0;

	/**
     * Stop the switch monitor thread.
     * <p><b>Description:</b><br>
     * The switch monitor thread will be commanded to stop monitor switch data from the module.  The switch monitor
     * thread will then exit.
     * <p>
	 * @return Status of stopping the switch monitor.
	 */
	virtual BEP_STATUS_TYPE StopSwitchMonitor() = 0;
	/**
	 * Read the VIN.
     * <p><b>Description:</b><br>
     * The VIN will be read from the module and returned to the calling function in <i>vin</i>.
	 * <p>
	 * @param vin VIN read from the module.
	 * @return Status of reading the VIn from the module.
	 */
	virtual BEP_STATUS_TYPE ReadVIN(string &vin) = 0;

protected:

	/**
	 * Read the part number.
     * <p><b>Description:</b><br>
     * The part number will be read from the module and returned to the calling function in <i>partNumber</i>.
     * <p>
	 * @param partNumber Part number read from the module.
	 * @returns Status of reading the part number from the module.
	 */
	virtual BEP_STATUS_TYPE ReadPartNumber(string &partNumber) = 0;

	/**
	 * Lock the Module.
     * <p><b>Description:</b><br>
     * Access to the module will be locked.  This is typically performed at the end of a successful roll test sequence.
     * <p>
	 * @return Status of locking the module.
	 */
	virtual BEP_STATUS_TYPE LockModule() = 0;

	/**
	 * Unlock the module.
     * <p><b>Description:</b>
     * The module will be unlocked for testing.  This method is abstract because the algorithm used to generate the key 
     * to unlock the module is very module specific.  
	 * <p>
	 * @param key Optionally specify a key, in which case it should not be generated.
	 * @return Status of unlocking the module.
	 */
	virtual BEP_STATUS_TYPE UnLockModule(string key = "") = 0;

	/**
	 * Clear the module faults.
     * <p><b>Description:</b><br>
     * The module will be commaded to clear fault data.
	 * <p>
	 * @return Status of clearing fault data from the module.
	 */
	virtual BEP_STATUS_TYPE ClearFaults() = 0;

    /**
     * Program the VIN.
     * <p><b>Description:</b><br>
     * The provided VIN will be programmed into the module.
     * <p>
     * 
     * @param vin    VIN to program into the module.
     * 
     * @return Status of programming the VIN into the module.
     */
	virtual BEP_STATUS_TYPE ProgramVIN(string vin) = 0;

    /**
     * Read module trace data.
     * <p><b>Description:</b><br>
     * The traceability data string will be read from the module and returned to the calling function in
     * <i>traceabilityData</i>.
     * <p>
     * 
     * @param traceabilityData
     *               Traceability data read from the module.
     * 
     * @return Status of reading the traceability data from the module.
     */
	virtual BEP_STATUS_TYPE ReadTraceData(string &traceabilityData) = 0;

    /**
     * Read module faults.
     * <p><b>Description:</b><br>
     * Fault data will be read from the module and returned to the calling function in <i>faultCodes</i>.
     * <p>
     * 
     * @param faultCodes The fault data read from the module.
     * 
     * @return Status of reading the fault data from the module.
     */
	virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes) = 0;

    /**
     * Read the module software info.
     * <p><b>Description:</b><br>
     * The module software info will be read from the module.  This typically consists of the revision number and the
     * module software date.  The software info is usually only logged for tracking purposes.
     * <p>
     * 
     * @return Status of reading the module software info.
     */
	virtual BEP_STATUS_TYPE ReadSoftwareInfo() = 0;

	/**
	 * Enter Diagnostic Mode.
     * <p><b>Description:</b><br>
     * The module will be commanded to begin a diagnostic session for testing purposes.
	 * <p>
	 * @return Status of placing the module into diagnostic mode.
	 */
	virtual BEP_STATUS_TYPE EnterDiagnosticMode() = 0;

	/**
	 * Exit Diagnostics Mode.
     * <p><b>Description:</b><br>
     * The module will be commanded to exit diagnostic mode.  This should be done at the end of the test sequence when
     * module communications are completed.
	 * <p>
	 * @return Status of commanding the module to exit the current diagnostic session.
	 */
	virtual BEP_STATUS_TYPE ExitDiagnosticMode() = 0;

	/**
	 * Resume Normal Operation.
     * <p><b>Description:</b><br>
     * Command the module to resume normal operations.
	 * <p>
	 * @return Status of commanding the module to resume normal operations.
	 */
	virtual BEP_STATUS_TYPE ResumeNormalOps() = 0;

	/**
     * Initialize the module interface.
     * <p><b>Description:</b><br>
     * Derived classes should override this function to perform module specific initialization instead of overriding
     * the Initialize function.  Initialize() will call this function during the initialization process after the
     * base items are setup.
	 * <p>
	 * @param configNode Configuration data to use for initializing the module interface.
     * @return Flag indicating module specifc initialization has completed.
	 */
	virtual bool InitializeHook(const XmlNode *configNode) = 0;

};

//-----------------------------------------------------------------------------
#endif


