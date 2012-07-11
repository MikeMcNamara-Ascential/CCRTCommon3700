//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/ModuleInterfaces/Include/GenericDCXModuleTool.h 2     10/08/07 2:50p Derickso $
//
// FILE DESCRIPTION:
// Generic Daimler Chrysler module interface for a J2190 based module.
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
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/ModuleInterfaces/Include/GenericDCXModuleTool.h $
// 
// 2     10/08/07 2:50p Derickso
// Updated header comments to doxygen format.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 2     6/09/05 4:19p Derickso
// Added ReadBitEncodedFaultRegisters.
// Added option to stop reading DTCs when a DTC value of 0 is detected.
// 
// 1     12/01/04 12:31p Derickso
// Created new VSS database.
// 
// 1     6/09/04 8:35a Derickso
// Dcx common header files.
// 
// 1     1/27/04 9:29a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 2     1/20/04 5:57a Derickso
// Promoted m_protocolFilter and m_faultByteCodes to protected from
// private.
// 
// 1     1/16/04 9:40a Derickso
// Generic DCX module tool interface.
// 
// 1     1/16/04 8:53a Derickso
// Generic DCX module interface.
// 
// 1     1/12/04 4:32p Derickso
// Generic J2190 Module class.
// 
//*************************************************************************
#ifndef GenericCANModuleTool_h
#define GenericCANModuleTool_h
//*************************************************************************
#include "IGenericModule.h"
#include "IProtocolFilter.h"
#include "ILogger.h"
#include "BepDefs.h"

/**
 * Generic DCX Module interface.<br>
 * This class provides the methods that are common to
 * all DaimlerChrysler modules.
 * 
 * @author Dan Erickson
 * @see KeywordProtocolFilter
 * @since January 16, 2004
 */
class GenericCANModuleTool : public ILogger
{
public:
    /**
     * Class constructor.
     * <p><b>Description:</b><br>
     * The pointer to the protocol filter will be saved to a member variable and used for gathering data from the module.
     * <p>
     * 
     * @param protocolFilter
     *               Pointer to the protocol filter to be used to read dat from the module.
     */
	GenericCANModuleTool(IProtocolFilter *protocolFilter);
    /**
     * Class destructor.
     * <p><b>Description:</b><br>
     * If a fault read is currently in progress, the software will wait up to 500ms for the fault read to complete.  The fault
     * data will then be cleared as well as the module config file copy that was saved during initialization.
     * <p>
     */
	virtual ~GenericCANModuleTool();
	/**
     * Read the fault registers.
     * <p><b>Description:</b><br>
     * Read the fault registers from the module and return the fault data to the calling function.
     * <p>
	 * @param faultCodes Fault codes read from the module.
	 * 
	 * @return Status of reading fault data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadFaultRegisters(FaultVector_t &faultCodes) = 0;
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
	virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes) = 0;
	/**
     * Read faults from the module.
     * <p><b>Description:</b><br>
     * Bit encoded fault registers will be read from the module.  The bits wil be parsed to determine which faults are
     * present.  The resulting fault codes will then be returned to the calling function.
     * <p>
	 * @param faultCodes Fault codes read from the module.
	 * 
	 * @return Status of reading bit encoded fault registers from the module.
	 */
    virtual BEP_STATUS_TYPE ReadBitEncodedFaultRegisters(FaultVector_t &faultCodes) = 0;
    /**
     * Initialize the module tool.
     * <p><b>Description:</b><br>
     * Data will be saved from the config file into member variables for easy access.  This mainly consists of fault
     * data and DTC data so faults can easily be read from the module.  A copy of the config file will be saved.
     * <p>
     * <b>Module Parameters:</b>
     *      <ul>
     *      <li> FaultByteCodes - List of fault byte codes to use. </li>
     *      <li> DTCData/DTCCountIndex - Byte location in the response containing the number of DTCs in the response. </li>
     *      <li> DTCData/BytesPerDTC - Number of bytes that make up one DTC record. </li>
     *      <li> DTCData/FirstDTCByteIndex - Byte index for the first DTC record. </li>
     *      <li> DTCData/FirstFaultRegister - PID of the first fault register to retrieve data from. </li>
     *      <li> DTCData/NumberOfFaultRegisters - Number of fault registers to read for fault data. </li>
     *      <li> DTCData/StopDTCReadOnZero - Flag to determine if fault reads should stop when the register read is 0x0000. </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ILogger::Initialize() </li>
     *      <li> SetDTCCountIndex() </li>
     *      <li> SetBytesPerDTC() </li>
     *      <li> SetFirstDTCByteIndex() </li>
     *      <li> SetFirstFaultRegister() </li>
     *      <li> SetNumberOfFaultRegisters() </li>
     *      <li> SetStopNoDTCs() </li>
     *      </ul>
	 *
	 * @param configNode Configuration data to use for initialization.
	 */
	virtual void Initialize(const XmlNode *configNode);

protected:
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
     * ProtocolFilter object to use when reading faults.
     */
    IProtocolFilter *m_protocolFilter;
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
};


//*************************************************************************
#endif //GenericCANModuleTool_h
