//******************************************************************************
// Description:
//  Genric Module Interface.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Include/GenericModuleTemplate.h $
// 
// 15    1/17/07 2:58p Rwiersem
// Changes for the 07011901 core release:
// 
// - Made the test class a friend.
// - Added StartSpecialKeepAlive() to add the ability to send different
// keep alive messages.
// - Added IsInitialized() and Uninitialize().
// 
// 14    10/25/06 3:08p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added OpenCom() and CloseCom() 
//     - Added a last transmit time member variable
// 
// 13    7/05/06 9:50p Cward
// Added support for reading and reporting module data
//
// 12    3/30/06 1:46a Cward
// Updated to add the capability to enter diagnostic mode without a
// response
//
// 11    8/17/05 8:08p Cward
// Added optional arg to the Read Module Data and Command Module methods
// for returning the raw serial string
//
// 10    5/11/05 10:03p Cward
// Updated for 05051101 iteration
//
// 9     5/09/05 5:00p Gpattison
// Added all necessary ReadModuleData() methods and ParseVector()
// methods to support new addition of Vectors in the
// Reply Interpreter class
//
// 9     2/16/05 9:13p Cward
// Added overloaded ReadModuleData, and added SerialArgs_t parameter to
// CommandModule.
//
// 8     9/01/04 8:52p Cward
//
// 7     8/05/04 2:37a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 6     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 5     3/04/04 4:15p Cward
// Added wait until done method calls
//
// 4     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 3     1/27/04 4:49p Cward
// Changes resulting from DE's testing and RW's trip
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:32p Cward
//
// 6     10/27/03 4:41p Derickso
// Updated log files to be more specific.
//
// 1     10/24/03 3:16p Canyanwu
// Start of DCX working directory
//
// 5     10/20/03 9:34a Derickso
// Added flags to determine when spun up threads have completed and can be
// safely deleted.
//
// 4     9/16/03 8:05p Derickso
// Moved definition of EEPROM_TEST_RESULT to
// GenericTCTemplate.  Was causing compile errors here.
//
// 3     9/08/03 7:25a Derickso
// Moved EEPROM_TEST_RESULT enum to GenericMOduleTemplate
// since it is used by more than one module class.
//
// 2     8/26/03 10:21a Derickso
// SendModuleKeepAliveIP updated to SendModuleKeepAlive.
//
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
//
// 10    8/20/03 6:41a Derickso
// Recombined cpp and h files.  Compiler does not support the
// implementation being seperate for template classes.
//
// 9     8/19/03 9:37a Derickso
// Updated class names.  Added Template to the end of the Generic class
// names to avoid conflicts with code written earlier.
//
// 6     8/12/03 4:27p Derickso
// Split implementation out to cpp file.
//
// 5     6/09/03 7:59a Derickso
// Added ReadPartNumber, ReadVIN, ReadTraceData methods.
//
// 4     5/28/03 7:42a Derickso
// Made GetInfo methods virtual.
//
// 3     5/28/03 7:36a Derickso
// Added GetInfo methods back in.
//
// 2     5/28/03 12:59a Tbochene
// Updated to use templates
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
// 3     4/04/03 8:52a Derickso
// Latest updates from AO.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GENERICMODULETemplate_H
#define GENERICMODULETemplate_H
//==============================================================================

#include <string>
#include <vector>

#include "XmlNode.h"
#include "ILogger.h"
#include "SwitchMonitor.h"
#include "IGenericModule.h"
#include "IProtocolFilter.h"
#include "ReplyInterpreter.h"
#include "SendModuleKeepAlive.h"

/**
 * GenericModuleTemplate is a generic interface for communication
 * with a vehicle module.  It has a IProtocolFilter object
 * to accomplish this communications.
 *
 * In order to use the module you must specify the protocol
 * filter that will be used within a sublclass of GenericModuleTemplate.
 * It should be noted that nowhere in this code is
 * the IProtocolFilter object initialized, the best place
 * for this to be done is in the initialize hook.
 *
 * @class GenericModuleTemplate
 * @since 21 Jan 2003
 */
template <class ProtocolFilterType>
class GenericModuleTemplate : public IGenericModule, public ILogger
{
    friend class GenericModuleTemplateTest;

public:
    /**
     * Class constructor.
     * <br>Common function names will be defined.  The last transmit timer will be initialized in order to keep track of 
     * keep alive timing requirements.  Communication objects will also be created for switch monitoring, keep alive
     * messages and the main communication channel to the module.
     */
    GenericModuleTemplate(void);

    /**
     * Class destructor.
     * <br>The message table will be cleared out.  The keep alive monitor and the switch monitor will be deleted.  The 
     * communication objects created in the constructor will also be deleted.
     */
    virtual ~GenericModuleTemplate(void);

    /**
     * Initialize the module interface.
     * <p><b>Description:</b><br>
     * The logging interface will be setup.  The RepltInterpretations and the Message Table will be stored.  A copy of
     * the config file aill also be stored.  InitializeHook will then be called so any sub-classes can perform any
     * specific initialization.
     * <br><b><i>Note:</i></b> Sub-classes should override InitializeHook instead of over-riding the Initialize function.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> SerialMessages/ReplyInterpretations - Module response data interpretation. </li>
     *      <li> SerialMessages/VehicleMessages - Message table to be used for module communications. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ILogger::Initialize() </li>
     *      <li> ModuleName() </li>
     *      <li> InitializeHook() </li>
     *      </ul>
     * 
     * @param configNode Configuration data to use for initializing the module interface class.
     *
     * @return Flag indicating if module interface initialization completed successfully.
     */
    bool Initialize (const XmlNode *configNode);

    /**
     * Open the serial connection.
     * <p><b>Description:</b><br>
     * The communication channel to the module will be established.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IProtocolFilter::OpenCom() </li>
     *      </ul>
     * 
     * @return Flag indicating if the serial connection was opened successfully.
     */
    virtual bool OpenCom();
    
    /**
     * Close all communication channels to the module.
     * <p><b>Description:</b><br>
     * If the keep alive monitor or switch monitor are still running, they will be commanded to stop and then will be 
     * deleted.  The keep alive and switch monitor comm objects will then be closed as well as the main communication
     * object for the module interface.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> IProtocolFilter::CloseCom() </li>
     *      </ul>
     */
    virtual void CloseCom();

    /**
     * <p><b>Description:</b><br>
     * Return a string representing the name of the module interface.
     * <p>
     * @returns "GenericModuleTemplate" for this class
     */
    virtual string ModuleName(void);

    /**
     * Enter diagnostic mode.
     * <p><b>Description:</b><br>
     * The module will be commanded to enter a new diagnostic session.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> EnterDiagnosticMode </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding the module to enter diagnostic mode.
     * @throws ModuleException if object needed is null
     */
    virtual BEP_STATUS_TYPE EnterDiagnosticMode(void);

    /**
     * Exit diagnostic mode.
     * <p><b>Description:</b><br>
     * The module will be commanded to exit the current diagnostic session.
     * <p>
     * <b>Message tags:</b>
     *      <ul>
     *      <li> ExitDiagnosticMode </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding the module to exit diagnostic mode.
     * @throws ModuleException if object needed is null
     */
    virtual BEP_STATUS_TYPE ExitDiagnosticMode(void);

    /**
     * Enter Diagnostics Mode, ignore response.
     * <p><b>Description:</b><br>
     * The module will be commanded to enter diagnostic mode.  The module response will be completely ignored.  The 
     * response will not be analyzed to determine if a negative response or any response was received.  The return
     * status is the status of sending the message to the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> EnterDiagnosticMode </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> SendMessage() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      </ul>
     *
     * @return Status of commanding the module to enter diagnostic mode
     * @throws ModuleException if object needed is null
     */
    virtual BEP_STATUS_TYPE EnterDiagnosticModeNoResponse(void);
    virtual BEP_STATUS_TYPE SendKeepAliveNoResponse(void);

    /**
     * Clear the module faults.
     * <p><b>Description:</b><br>
     * The module will be commanded to clear any faults that are active or stored.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ClearFaults </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @return Status of clearing faults from the module.
     * @throws ModuleException if object needed is null
     */
    virtual BEP_STATUS_TYPE ClearFaults(void);

    /**
     * Read the part number.
     * <p><b>Description:</b><br>
     * Read the part number from the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadPartNumber </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param partNumber Part number read from the module.
     * @returns Status of reading the part number from the module.
     */
    virtual BEP_STATUS_TYPE ReadPartNumber(string &partNumber);

    /**
     * Read the VIN.
     * <p><b>Description:</b><br>
     * Read the VIn from the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadVIN </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param vin VIN read from the module.
     * @return Status of reading the VIN from the module.
     */
    virtual BEP_STATUS_TYPE ReadVIN(string &vin);

    /**
     * Read module trace data.
     * <p><b>Description:</b><br>
     * The traceability data will be read from the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadTraceData </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     * 
     * @param traceabilityData
     *               Traceability data read from the module.
     *
     * @return Status of reading the traceability data from the module.
     */
    virtual BEP_STATUS_TYPE ReadTraceData(string &traceabilityData);

    /**
     * Read switch data from the module. 
     * <p><b>Description:</b><br>
     * Read switch data from the module. This will normally be called by a SwitchMonitor from another thread, 
     * which is why it is passed it's own IProtocolFilter.
     * <br><b><i>Note:</i></b> There is no generic switch monitoring available.  Each module has it's own unique 
     * requirements for monitoring switches.  If this function is called, it will return BEP_STATUS_FAILURE.
     *
     * @param filter IProtocolFilter to use for reading the switch data from the module.
     * @return BEP_STATUS_FAILURE
     */
    virtual BEP_STATUS_TYPE ReadSwitches(IProtocolFilter *filter);

    /**
     * Begin sending the keep alive message to the module.
     * <p><b>Description:</b><br>
     * A thread will be created to send keep alive messages to the module in order to keep the module in diagnostic mode.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ModuleKeepAlive </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StartSpecialKeepAlive() </li>
     *      </ul>
     * 
     * @return Status of starting the keep alive thread.
     * @throws ModuleException if the keep alive comm is null
     */
    virtual BEP_STATUS_TYPE StartModuleKeepAlive(void) throw(ModuleException);

    /**
     * Begin sending the specified keep alive message.
     * <p><b>Description:</b><br>
     * A thread will be created to periodically send the specified keep alive message to the module.    A keep alive
     * message is typically required to keep a module in diagnostic mode.  If no <i>messageId</i> is provided, 
     * <i>ModuleKeepAlive</i> will be used.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> ModuleKeepAliveTime - Time(ms) to wait between keep alive message transmits. </li>
     *      <li> KeepAliveResponseRequired - Flag indicating if the module will be sending a response to the keep 
     *                                       alive message. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> IProtocolFilter::OpenCom() </li>
     *      </ul>
     * 
     * @param messageId Message tag to use for sending the keep alive messahe to the module.
     * 
     * @return Status of starting the keep alive thread.
     */
    virtual BEP_STATUS_TYPE StartSpecialKeepAlive(std::string messageId) throw(ModuleException);

    /**
     * Stop sending keep alive message to the module.
     * <p><b>Description:</b><br>
     * If the keep alive thread has been started, it will be commanded to stop sending messages.  The thread will then
     * be deleted.
     * <p>
     * @return Status of stopping the keep alive thread.
     */
    virtual BEP_STATUS_TYPE StopModuleKeepAlive(void);

    /**
     * Begin reading switches in the background.
     * <p><b>Description:</b><br>
     * A new SwitchMonitor thread will be created if one does not yet exist.  The thread will then periodically read
     * switch data from the module.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> SwitchMonitorDelay - Amount of time(ms) to wait between switch data reads. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      </ul>
     *
     * @return Status of starting the SwitchMonitor.
     */
    virtual BEP_STATUS_TYPE StartSwitchMonitor(void) throw(ModuleException);

    /**
     * Stop reading switches in the background.
     * <p><b>Description:</b><br>
     * The switch monitor will be commanded to stop reading switch data from the module.  The switch monitor will then
     * be deleted.
     * <p>
     * @return Status of stopping the background SwitchMonitor.
     */
    virtual BEP_STATUS_TYPE StopSwitchMonitor(void);

    /**
     * Program the VIN.
     * <p><b>Description:</b><br>
     * Program the provided VIN into the module.
     * <br><b><i>Note:</i></b> There is no generic method for programming the VIN into a module.  This functions will 
     * return BEP_STATUS_SOFTWARE if called.
     * <p>
     * @param vin VIN to program into the module.
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE ProgramVIN(string vin);
    /**
     * Unlock module security so data can be written to the module.
     * <p><b>Description:</b><br>
     * Module security will be unlocked.
     * <br><b><i>Note:</i></b> The security unlock algorithm is unique to each module.  If this function is called, it
     * will return BEP_STATUS_SOFTWARE.
     * <p>
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE UnlockModuleSecurity();
    /**
     * Read consecutive memory addresses from a module.
     * <p><b>Description:</b><br>
     * Consecutive memory addresses will be read from the module starting at <i>address</i>  The next address to be
     * read will be <i>address + addressIncrement</i>.  Consecutive memory addresses will be read until 
     * <i>numberOfReads</i> memory addresses has been completed.  The module data from each read will be returned to
     * the calling function.
     * <p>
     * <b>Protocol Filter Functions Called:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     *
     * @param serialMessage
     *                Message tag used to read the memory address.
     * @param address The first memory address to read.
     * @param numberOfAddressBytes
     *                The number of bytes contained in the address.
     * @param moduleResponses
     *                Vector used to pass back each of the module read responses.
     * @param numberOfReads
     *                Number of memory addresses to read.
     * @param addressIncrement
     *                Offset to the next memory address to be read.
     *
     * @return Status of reading the memory addresses from the module.
     */
    virtual BEP_STATUS_TYPE ReadConsecutiveMemoryAddresses(const string &serialMessage, const UINT16 &address,
                                                           const UINT16 &numberOfAddressBytes,
                                                           vector<SerialString_t> &moduleResponses,
                                                           const UINT16 &numberOfReads = 1,
                                                           const UINT16 &addressIncrement = 1);

    /**
     * Read the specified register from the module.
     * <p><b>Description:</b><br>
     * the register <i>moduleRegister</i> will be read from the module.  The contents will be stored in 
     * <i>registerContents</i> and returned to the calling function.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadModuleRegister </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param moduleRegister        Register to be read from the module.
     * @param registercontents  Data read from the register.
     * @return Status of reading the specified register from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleRegister(const UINT16 &moduleRegister, UINT16 &registerContents);

    /**
     * GetInfo for string types
     * <p><b>Description:</b><br>
     * String type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadPartNumber() </li>
     *      <li> UnLockModule() </li>
     *      <li> ProgramVIN() </li>
     *      <li> ReadVIN() </li>
     *      <li> ReadTraceData() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadPartNumber </li>
     *      <li> UnLockModule </li>
     *      <li> ProgramVIN </li>
     *      <li> ReadVIN </li>
     *      <li> ReadTraceData </li>
     *      </ul>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was successfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data item from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, string & value) throw(ModuleException);

    /**
     * GetInfo for a vector of string types
    * <p><b>Description:</b><br>
    * A vector of string type data will be read from the module and returned to the calling function.
    * <p>
     *
    * <b>Functions Called:</b><br>
    *      <ul>
    *      <li> GetInfo() </li>
    *      </ul>
    * 
    * <b>Data Items Supported:</b> None <br>
    * 
     * @throw ModuleException if the method cannot be found
    * @param methodName The data to read from the module.
    * @param value      The value read from the module.
    * 
    * @return Status of reading the data from the module.
    *         <ul>
    *         <li> BEP_STATUS_SUCCESS - Data was successfully read from the module. </li>
    *         <li> BEP_STATUS_FAILURE - Could not read data item from the module. </li>
    *         </ul>
    * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<string> & value) throw(ModuleException);

    /**
     * GetInfo for bool types
     * <p><b>Description:</b><br>
     * Boolean type data will be read from the module and returned to the calling function.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a 
     * ModuleException if called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, bool &value) throw(ModuleException);

    /**
     * GetInfo for a vector of bool types
     * <p><b>Description:</b><br>
     * A vector of boolean type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<bool> & value) throw(ModuleException);

    /**
     * GetInfo for integer types
     * <p><b>Description:</b><br>
     * Integer type data will be read from the module and returned to the calling function.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a
     * ModuleException if called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, int & value) throw(ModuleException);

    /**
     * GetInfo for a vector of integer types
     * <p><b>Description:</b><br>
     * A vector of integer type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<INT32> & value) throw(ModuleException);

    /**
     * GetInfo for FaultVector_t  types
     * <p><b>Description:</b><br>
     * Fault data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadFaults() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadFaults </li>
     *      </ul>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The faults read from the module.
     * 
     * @return Status of reading the faults from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Faults were succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read faults from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t & value) throw(ModuleException);

    /**
     * GetInfo for float types
     * <p><b>Description:</b><br>
     * Float type data will be read from the module and returned to the calling function.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a
     * ModuleException if called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

    /**
     * GetInfo for a vector of float types
     * <p><b>Description:</b><br>
     * A vector of float type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     *
     * <b>Data Items Supported:</b> None <br>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<float> & value) throw(ModuleException);

    /**
     * GetInfo for WheelSpeeds_t types
     * <p><b>Description:</b><br>
     * WheelSppeds_t type data will be read from the module and returned to the calling function.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a
     * ModuleException if called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, WheelSpeeds_t & value) throw(ModuleException);

    /**
     * GetInfo for a vector of WheelSpeeds_t types
     * <p><b>Description:</b><br>
     * A vector of WheelSpeeds_t type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<WheelSpeeds_t> & value) throw(ModuleException);

    /**
     * Perform the requested action.
     * <p><b>Description:</b><br>
     * The requested action or module command will be completed.
     * <p>
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> StartSwitchMonitor </li>
     *      <li> StopSwitchMonitor </li>
     *      <li> StartModuleKeepAlive </li>
     *      <li> StopModuleKeepAlive </li>
     *      <li> LockModule </li>
     *      <li> ClearFaults </li>
     *      <li> EnterDiagnosticMode </li>
     *      <li> ExitDiagnosticMode </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> StartSwitchMonitor() </li>
     *      <li> StopSwitchMonitor() </li>
     *      <li> StartModuleKeepAlive() </li>
     *      <li> StopModuleKeepAlive() </li>
     *      <li> LockModule() </li>
     *      <li> ClearFaults() </li>
     *      <li> EnterDiagnosticMode() </li>
     *      <li> ExitDiagnosticMode() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * @param methodName Action or command to be completed.
     *
     * @return Status of completing the requested action.
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName) throw(ModuleException);

    /**
     * GetInfo for unsigned character types
     * <p><b>Description:</b><br>
     * Unsigned character type data will be read from the module and returned to the calling function.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a
     * ModuleException if called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 & value) throw(ModuleException);

    /**
     * GetInfo for a vector of unsigned character types
     * <p><b>Description:</b><br>
     * A vector of unsigned character type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT8> & value) throw(ModuleException);

    /**
     * GetInfo for unsigned short types
     * <p><b>Description:</b><br>
     * Unsigned short type data will be read from the module and returned to the calling function.
     * <br><b><i>Note:</i></b>  No generic data is supported.  This function will return BEP_STATUS_SOFTWARE and throw a
     * ModuleException if called.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * <b>Data Items Supported:</b> None <br>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return BEP_STATUS_SOFTWARE
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 & value) throw(ModuleException);

    /**
     * GetInfo for a vector of unsigned short types
     * <p><b>Description:</b><br>
     * A vector of unsigned short type data will be read from the module and returned to the calling function.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     *
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT16> & value) throw(ModuleException);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseStringResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, string &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL, 
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseBooleanResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, bool &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL, 
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseFloatResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, float &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseSignedIntegerResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, INT32 &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseUnsignedByteResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, UINT8 &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseUnsignedShortResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, UINT16 &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseUnsignedIntegerResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, UINT32 &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseVectorResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<bool> &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseVectorResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<string> &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseVectorResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<UINT8> &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseVectorResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<UINT32> &data,                                                       
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseVectorResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<UINT16> &data,                                                       
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);


    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseVectorResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<INT32> &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     *
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseVectorResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<double> &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Read data from the module using the message tag provided.
     * <p><b>Description:</b><br>
     * The specified data will be read from the module.  If any <i>args</i> are provided, they will be added to the
     * message to be sent to the module.  If a SerialString_t object is provided, the module response will be stored
     * in it for return to the calling function.  If an IProtocolFilter object is provided, it will be used to send
     * the message to the module instead of m_protocolFilter.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseVectorResponse() </li>
     *      <li> GetReplyInterpretationTag() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @param messageTag Message tag to use for getting data from the module.
     * @param data       Data read from the module.
     * @param args       Additional data to be added to the message.
     * @param rawData    Complete response from the module.
     * @param commObject Communication object to use for talking to the module.  If one is not provided, m_protocolFilter will be used.
     * 
     * @return Status of reading the data from the module.
     */
    virtual BEP_STATUS_TYPE ReadModuleData(const string messageTag, vector<float> &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL,
                                           IProtocolFilter *commObject = NULL);

    /**
     * Send a command to the module.
     * <p><b>Description:</b><br>
     * Send the module a command to perform some work.  Note that this method just looks for a positive response
     * from the module.  No other data is evaluated.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     *
     * @param command Command to send to the module.
     * @param args    Data to be sent to the module with the command.
     * @param rawData    Response from the module.
     * @param commObject Communication object to use to send the message.  If NULL, m_protocolFilter will be used.
     *
     * @return Status of commanding the module.
     */
    virtual BEP_STATUS_TYPE CommandModule(const string command, 
                                          SerialArgs_t *args = NULL, 
                                          SerialString_t *rawData = NULL, 
                                          IProtocolFilter *commObject = NULL);

    /**
     * Lock the module.
     * <p><b>Description:</b><br>
     * The module will be commanded to lock security.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> LockModule </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of commanding the module to lock.
     */
    virtual BEP_STATUS_TYPE LockModule(void);

    /**
     * Unlocks the module.
     * <p><b>Description:</b><br>
     * The module will be commanded to unlock.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> UnLockModule </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @param key Key to be sent to the module.  In this instance, it is not used.
     * @return Status of unlocking the module.
     */
    virtual BEP_STATUS_TYPE UnLockModule(string key);

    /**
     * Read module faults.
     * <p><b>Description:</b><br>
     * Read the fault data from the module.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadFaults </li>
     *      </ul>
     * 
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> ParseStringResponse() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     *
     * @note Need to figure out how to do multipart
     * reads
     * @param faultCodes The fault data read from the module.
     * 
     * @return Status of reading the fault data from the module.
     */
    virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);

    /**
     * Read the software info.
     * <p><b>Description:</b><br>
     * Read the software info from the module.  This typically includes revision data, software version, part number, etc.
     * <br><b><i>Note:</i></b> No generic method is available.  This function will return BEP_STATUS_SOFTWARE if called.
     * <p>
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE ReadSoftwareInfo(void);

    /**
     * Set MEC Counter.
     * <p><b>Description:</b><br>
     * The MEC will be set in the module.
     * <br><b><i>Note:</i></b> There is no generic method for setting the MEC.  This function will return 
     *                         BEP_STATUS_SOFTWARE if called.
     * <p>
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE SetMecCounter(void);

    /**
     * Resume normal operation.
     * <p><b>Description:</b><br>
     * Command the module to return to normal operation.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ResumeNormalOps </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CheckObjectsStatus() </li>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of returning the module to normal operation.
     */
    virtual BEP_STATUS_TYPE ResumeNormalOps(void);

    /**
     * Determine if the ignition is in the On position.
     * <p><b>Description:</b><br>
     * A message will be sent to the module.  If the module responds to the message, it is assumed the ignition is in
     * the On position.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadIgnitionStatus </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Flag indicating if the ignition is in the On position.
     */
    virtual bool IsIgnitionOn(void);

    /**
     * Determine if the engine is running.
     * <p><b>Description:</b><br>
     * The engine RPM will be read from the module.  If the RPM read from the module is greater than <i>minimumEngineRPM</i>
     * the engine is considered to be running.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadEngineRPMActual </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      </ul>
     *
     * @param minimumEngineRPM
     *               Minimum engine RPM to determine if the engine is running.
     * @return Flag indicating if the engine is running.
     */
    virtual bool IsEngineRunning(const INT32 &minimumEngineRPM);

    /**
     * Initialize the module interface.
     * <p><b>Description:</b><br>
     * Module specific initialization will be completed.  The main module communication object will be intialized.
     * If any sub-class needs to perform specific initialization, it should override this class instead of Initialize().
     * <p>
     * @param configNode  Configuration data to use for initializing the module interface.
     * @return  Flag indicating if module specific initialization has been completed.
     */
    virtual bool InitializeHook(const XmlNode *configNode);

    /**
     * Check if this module interface has already been initialized. 
     * <p><b>Description:</b><br>
     * The flag indicating module initialization has been completed will be checked.  This flag is used when
     * performing a WarmInitialize() in order to know if we can simply OpenCom() on this module interface or 
     * if we need to perform a full re-initialization
     * <p>
     * @return Flag indicating if the module interface has already been initialized
     */
    virtual bool IsInitialized() const;

    /**
     * Uninitialize the module interface.
     * <p><b>Description:</b><br>
     * The module interface will be instructed to uninitialize itself.  The module communication objects will be closed.
     * <p>
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CloseCom() </li>
     *      </ul>
     */
    virtual void Uninitialize();
    
    /**
     * Parse string data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  String type data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the string data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return String data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual string ParseStringResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse boolean data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  Boolean type data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the boolean data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Boolean data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual bool ParseBooleanResponse(string interpTag, SerialString_t rawResponse)   throw(XmlException, ModuleException);

    /**
     * Parse unsigned integer data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  unsigned integer data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the unsigned integer data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Unsigned integer data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual UINT32 ParseUnsignedIntegerResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse unsigned character data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  unsigned character data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the unsigned character data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Unsigned character data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual UINT8 ParseUnsignedByteResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse unsigned short data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  unsigned short data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the unsigned short data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Unsigned short data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual UINT16 ParseUnsignedShortResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse integer data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  Integer data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the integer data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Integer data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual INT32 ParseSignedIntegerResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse character data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  Character data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the character data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Character data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual INT8 ParseSignedByteResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse short data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  Short data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the short data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Short data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual INT16 ParseSignedShortResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse float data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  Float data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the float data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Float data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual double ParseFloatResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse enumerated data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  Enumerated data
     * will be extracted from the response and returned to the calling function.  The module interpreter will be used
     * to extract the enumerated data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     *
     * @return Enumerated data extracted from the module response.
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual UINT16 ParseEnumeratedResponse(string interpTag, SerialString_t rawResponse)
    throw(XmlException, ModuleException);

    /**
     * Parse a vector of boolean data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.  
     * The vector of boolean data will be extracted from the response and returned to the calling function.  
     * The module interpreter will be used to extract the vector of boolean data from the module response.
     * <p>
     *
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     * @param data      Vector of boolean data extracted from the module response.
     *
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual void ParseVectorResponse(string interpTag, SerialString_t rawResponse, vector<bool> &data)
    throw(XmlException, ModuleException);

    /**
     * Parse a vector of string data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.
     * The vector of string data will be extracted from the response and returned to the calling function.
     * The module interpreter will be used to extract the vector of string data from the module response.
     * <p>
     *
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     * @param data      Vector of string data extracted from the module response.
     *
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual void ParseVectorResponse(string interpTag, SerialString_t rawResponse, vector<string> &data)
    throw(XmlException, ModuleException);

    /**
     * Parse a vector of unsigned character data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.
     * The vector of unsigned character data will be extracted from the response and returned to the calling function.
     * The module interpreter will be used to extract the vector of unsigned character data from the module response.
     * <p>
     *
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     * @param data      Vector of unsigned character data extracted from the module response.
     *
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual void ParseVectorResponse(string interpTag, SerialString_t rawResponse, vector<UINT8> &data)
    throw(XmlException, ModuleException);

    /**
     * Parse a vector of unsigned integer data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.
     * The vector of unsigned integer data will be extracted from the response and returned to the calling function.
     * The module interpreter will be used to extract the vector of unsigned integer data from the module response.
     * <p>
     *
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     * @param data      Vector of unsigned integer data extracted from the module response.
     *
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual void ParseVectorResponse(string interpTag, SerialString_t rawResponse, vector<UINT32> &data)
    throw(XmlException, ModuleException);

    /**
     * Parse a vector of unsigned short data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.
     * The vector of unsigned short data will be extracted from the response and returned to the calling function.
     * The module interpreter will be used to extract the vector of unsigned short data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     * @param data      Vector of unsigned short data extracted from the module response.
     * 
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual void ParseVectorResponse(string interpTag, SerialString_t rawResponse, vector<UINT16> &data)
    throw(XmlException, ModuleException);

    /**
     * Parse a vector of integer data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.
     * The vector of integer data will be extracted from the response and returned to the calling function.
     * The module interpreter will be used to extract the vector of integer data from the module response.
     * <p>
     *
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     * @param data      Vector of integer data extracted from the module response.
     *
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual void ParseVectorResponse(string interpTag, SerialString_t rawResponse, vector<INT32> &data)
    throw(XmlException, ModuleException);

    /**
     * Parse a vector of double data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.
     * The vector of double data will be extracted from the response and returned to the calling function.
     * The module interpreter will be used to extract the vector of double data from the module response.
     * <p>
     *
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     * @param data      Vector of double data extracted from the module response.
     *
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual void ParseVectorResponse(string interpTag, SerialString_t rawResponse, vector<double> &data)
    throw(XmlException, ModuleException);

    /**
     * Parse a vector of float data from a module response.
     * <p><b>Description:</b><br>
     * The provided <i>interpTag</i> will be used for interpreting the data in <i>rawResponse</i>.
     * The vector of float data will be extracted from the response and returned to the calling function.
     * The module interpreter will be used to extract the vector of float data from the module response.
     * <p>
     * 
     * @param interpTag The reply interpretation tag to use.
     * @param rawResponse
     *                  The response from the module.
     * @param data      Vector of float data extracted from the module response.
     *
     * @throws XmlException
     *                If there is an error using the XmlNode that is specified.
     * @throws ModuleException
     *                if the reply interpretation xml node is null
     */
    virtual void ParseVectorResponse(string interpTag, SerialString_t rawResponse, vector<float> &data)
    throw(XmlException, ModuleException);

    /**
     * Get the reply interpretation tag.
     * <p><b>Description:</b><br>
     * Get the reply interpretation tag for the specified message table entry.
     * <p>
     * @param messageTag Message table entry to use.
     * @return Tag of the reply interpretation node to use for the response.
     */
    virtual string GetReplyInterpretationTag(const string &messageTag);

    /**
     * Verify message handling can be performed.
     * <p><b>Description:</b><br>
     * The reply interpretations will be checked to ensure data can be extracted from any module response data.
     * The message table will also be checked to ensure it is properly loaded.
     * <p>
     */
    virtual void CheckObjectsStatus(void) throw(ModuleException);
    /**
     * Read and record ECU information.
     * <p><b>Description:</b><br>
     * Data will be read from the module and recorded in <i>moduleDataItems</i> for return to the calling function.
     * <p>
     * <b>Protocol Filter Functions:</b>
     *      <ul>
     *      <li> GetModuleData() </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ParseStringResponse() </li>
     *      <li> ModuleName() </li>
     *      </ul>
     * 
     * @param moduleDataItems
     *                   Data items to be extracted from the module response.
     * @param messageTag Message tag to use for reading the data items from the module.
     *
     * @return Status of reading and recording the data items from the module.
     */
    virtual BEP_STATUS_TYPE ReadInfoToReport(XmlNodeMap *moduleDataItems, const string &messageTag);
    /**
	 * Method to signal protocol filter to stop
     * module communications
	 *
	 * @since 26 March 2007
	 */
    virtual void StopVehicleComms(const bool &stopComms);

    /**
     * Command the module to perform a power on reset.
     * <p><b>Description:</b><br>
     * The module will be commanded to perform a power-on reset.  The module will re-initialize its software for a 
     * static electrical check.  This is essentially the same thing as performing a key cycle.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> PerformPowerOnReset </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> CommandModule() </li>
     *      </ul>
     *
     * @return Status of performing the power-on reset.
     */
    virtual BEP_STATUS_TYPE PerformPowerOnReset(void);
    /**
     * Handle to protocol filter object. This is the main communication object for the module interface.
     */
    ProtocolFilterType *m_protocolFilter;
    /**
     * Variable used for testing if initialize hook is called
     */
    bool m_initializeHookCalled;
    /**
     * Variable used for testing if initialize has been called
     */
    bool m_initializeCalled;
    /**
     * XmlNode pointer to our config node as passed into Initialize.
     */
    const XmlNode *m_configNode;
    /**
     * XmlNode pointer for the reply interpretations node
     */
    const XmlNode *m_replyInterpretationNode;
    /**
     * XmlNode pointer for the VehicleMessages node
     */
    const XmlNode *m_vehicleMessagesNode;
    /**
     * Our Module Interpretation object
     */
    IReplyInterpreter* m_moduleInterpreter;
    /**
     * Protocol filter object to be used for sending keep alive messages to the module.
     */
    ProtocolFilterType *m_keepAliveComm;
    /**
     * Thread to send the keep alive message to the module.
     */
    SendModuleKeepAlive* m_moduleKeepAlive;
    /**
     * Protocol filter object to be used for reading switch states.
     */
    ProtocolFilterType *m_switchMonitorComm;
    /**
     * Thread to monitor switch data in the module.
     */
    SwitchMonitor* m_switchMonitor;
    string m_driveCurveRewriteDir;

protected:
    /**
     * Create a Semaphore for locking the comm port for the process only.
     * If background threads are spun up, this will ensure that only one
     * thread is accessing the comm port at a time.
     */
    BepMutex m_commsInUse;
    // These are method names that this module has functionality
    // for.
    string m_readPartNumber;
    string m_unLockModule;
    string m_programVIN;
    string m_readVIN;
    string m_readTraceData;
    string m_readFaults;
    string m_startSwitchMonitor;
    string m_stopSwitchMonitor;
    string m_startModuleKeepAlive;
    string m_stopModuleKeepAlive;
    string m_lockModule;
    string m_clearFaults;
    string m_enterDiagnosticMode;
    string m_enterNormalMode;
    string m_exitDiagnosticMode;
    string m_disableNormalMode;
    /**
     * Last transmit time to determine if it is time to send a keep alive message to the module.
     */
    KeepAliveTimer_t    m_lastTxTime;
    StopCommsBepCondVar    m_stopCommsFlag;
};

#endif

