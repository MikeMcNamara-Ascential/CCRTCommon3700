//*************************************************************************
// FILE DESCRIPTION:
//	Module Interface for the Isuzu engine controller.
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
#ifndef IsuzuEngineControlModule_h
#define IsuzuEngineControlModule_h
//-------------------------------------------------------------------------------------------------
#include "GenericEmissionsModuleTemplate.cpp"
#define DTC_MESSAGE_SIZE 8
//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
class IsuzuEngineControlModule : public GenericEmissionsModuleTemplate<ProtocolFilterType>
{
public:
    IsuzuEngineControlModule();
    virtual ~IsuzuEngineControlModule();

    /**
     * Initialize the keep alive and switch monitor threads.  Then call our
     * parent's InitializeHook().
     *
     * @param configNode The XML Config node for the module
     * 
     * @return Status of the operation.
     */
    virtual bool InitializeHook(const XmlNode *configNode);

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
    BEP_STATUS_TYPE ReadPartNumber(string &partNumber);

    /**
     * Read faults stored in the module.
     * <p><b>Description:</b><br>
     * The fault data will be read from the module.
     * <br><b><i>Note:</i></b> Many modules handle fault data differently.  There is no generic implementation.
     * <p>
     * @param faultCodes Fault Codes read from the module.
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE ReadFaults(FaultStatusVector_t &faultCodes);

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
#endif //IsuzuEngineControlModule_h
