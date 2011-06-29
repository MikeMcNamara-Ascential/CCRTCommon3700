//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the Bosch ABS.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#ifndef BoschAbsModule_h
#define BoschAbsModule_h
//-------------------------------------------------------------------------------------------------
#include "GenericABSModuleTemplate.cpp"
//-------------------------------------------------------------------------------------------------
template<class ProtocolFilter>
class BoschAbsModule : public GenericABSModuleTemplate<ProtocolFilter>
{
public:
    BoschAbsModule();
    virtual ~BoschAbsModule();

    /**
     * Return the name of this module.
     * <p><b>Description:</b><br>
     * The name of the module will be returned to the calling function.
     * <p>
     *
     * @return Bosch ABS Module
     */
    virtual string ModuleName();

    /**
     * Establish comms to the module.
     * 
     * @return Status of establishing communications.
     */
    BEP_STATUS_TYPE EnterDiagnosticMode();

    /**
     * Initialize the module interface.
     * <p><b>Description:</b><br>
     * Module specific initialization will be completed.  The main module communication object will be intialized.
     * If any sub-class needs to perform specific initialization, it should override this class instead of Initialize().
     * <p>
     * 
     * @param configNode Configuration data to use for initializing the module interface.
     * 
     * @return Flag indicating if module specific initialization has been completed.
     */
    virtual bool InitializeHook(const XmlNode *configNode);

protected:
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

private:
    /**
     * Get/Set the index for the byte that contains the number of reported DTCs.
     * 
     * @param byte   Index for the byte that contains the number of reported DTCs.
     * 
     * @return Index for the byte that contains the number of reported DTCs.
     */
    const UINT8& DtcCountIndex(const UINT8 *byte = NULL);
    /**
     * Get/Set the index of the first byte of the first DTC in the module response.
     * 
     * @param byte   Index of the first byte of the first DTC in the module response.
     * 
     * @return Index of the first byte of the first DTC in the module response.
     */
    const UINT8& FirstDtcByteIndex(const UINT8 *byte = NULL);
    /**
     * Get/Set the number of bytes per DTC record.
     * 
     * @param byte   Number of bytes per DTC record.
     * 
     * @return Number of bytes per DTC record.
     */
    const UINT8& BytesPerDtc(const UINT8 *byte = NULL);
    /** Index of the byte containing the number of DTCs reported in the module response. */
    UINT8 m_dtcCountIndex;
    /** Index of the first byte of the first DTC in the module response. */
    UINT8 m_firstDtcIndex;
    /** Number of bytes from the module response that compose one DTC */
    UINT8 m_bytesPerDtc;
};
//-------------------------------------------------------------------------------------------------
#endif //BoschAbsModule_h
