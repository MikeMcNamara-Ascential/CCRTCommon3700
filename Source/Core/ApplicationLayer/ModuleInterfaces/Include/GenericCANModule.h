//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/ModuleInterfaces/Include/GenericDCXCANModule.h 2     10/08/07 12:16p Derickso $
//
// FILE DESCRIPTION:
// Generic Daimler Chrysler module interface for a CAN based module.
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
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/ModuleInterfaces/Include/GenericDCXCANModule.h $
// 
// 2     10/08/07 12:16p Derickso
// Updated header comments to doxygen format.
// REmoved InitializeHook since all it did was call the base class.
// 
// 1     10/26/06 1:21p Bbarrett
// Intro to Cmake.
// 
// 2     6/09/05 4:17p Derickso
// Added ReadBitEncodedFaultRegisters.
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
// 3     1/20/04 5:56a Derickso
// Updated to inherit from new GenericDCXModuleTool
// interface class.
// 
// 2     8/26/03 10:20a Derickso
// Updated for new directory structure.
// Compiles, not tested.
// 
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
// 
// 1     8/18/03 9:20a Derickso
// Created.  Initial Checkin
// 
//*************************************************************************
#ifndef GenericCANModule_h
#define GenericCANModule_h
//*************************************************************************
#include "GenericCANModuleTool.h"

/**
 * Generic DCX CAN Module interface.
 * This class provides the methods that are common to
 * all DaimlerChrysler CAN base modules.
 * 
 * @author Dan Erickson
 * @see KeywordProtocolFilter
 * @since August 18, 2003
 */
class GenericCANModule : public GenericCANModuleTool
{
public:
	/**
	 * Class constructor.
	 */
	GenericCANModule(IProtocolFilter *protocolFilter);
	/**
	 * Class destructor.
	 */
	virtual ~GenericCANModule();
	/**
     * Read bit encoded fault data from registers.
     * <p><b>Description:</b><br>
     * The fault registers will be read from the module.  The fault data read from the module will be returned to the
     * calling function.
     * <br><b><i>Note:</i></b> Fault registers are not currently supported in any DCX CAN modules.  This function will
     * throw a ModuleException if called.
     * <p>
	 * 
	 * @param faultCodes Fault data read from the module.
	 * @return BEP_STATUS_SOFTWARE
	 */
    virtual BEP_STATUS_TYPE ReadBitEncodedFaultRegisters(FaultVector_t &faultCodes);
	/**
     * Read fault registers.
     * <p><b>Description:</b><br>
     * The fault registers will be read from the module.  The fault data read from the module will be returned to the
     * calling function.
     * <br><b><i>Note:</i></b> Fault registers are not currently supported in any DCX CAN modules.  This function will
     * throw a ModuleException if called.
     * <p>
	 * 
	 * @param faultCodes Fault data read from the module.
	 * @return BEP_STATUS_SOFTWARE
	 */
	virtual BEP_STATUS_TYPE ReadFaultRegisters(FaultVector_t &faultCodes);
    /**
     * Read module faults.
     * <p><b>Description:</b><br>
     * The fault data will be read from the module and stored in <i>faultCodes</i> for return to the calling function.
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
     *      <li> SetReadFaultsInProgress() </li>
     *      <li> SetReadFaultsComplete() </li>
     *      <li> GetDTCCountIndex() </li>
     *      <li> GetBytesPerDTC() </li>
     *      <li> GetFirstDTCByteIndex() </li>
     *      </ul>
     * 
     * @param faultCodes Fault codes read from the module.
     * 
     * @return Status of reading the fault data from the module.
     * @throws ModuleException
     */
	virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);
};


//*************************************************************************
#endif //GenericCANModule_h
