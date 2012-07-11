//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Module Interface for the Bosch ABS.
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
#ifndef MandoAbsModule_h
#define MandoAbsModule_h
//-----------------------------------------------------------------------------
#include "GenericABSModuleTemplate.cpp"

//-----------------------------------------------------------------------------
template <class ProtocolType>
class MandoAbsModule : public GenericABSModuleTemplate<ProtocolType>
{
public:
    MandoAbsModule();
    virtual ~MandoAbsModule();
    /**
     * Returns the name of this module
     */
    virtual string ModuleName();
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
    /**
     * Read the ECU ID
     *
     * @return Status of the operation
     * @param response The returned values will be stored here
     * @throws ModuleException if object needed is null
     * @since 24 June 2008
     */
    virtual BEP_STATUS_TYPE CheckEspEquipped(bool &response) throw(ModuleException);

    /**
     * Check values of I/O signals
     *
     * @return Status of the operation
     * @param response The returned values will be stored here
     * @throws ModuleException if object needed is null
     * @since 24 June 2008
     */
    virtual BEP_STATUS_TYPE ReadIoSignals(SerialString_t &response) throw(ModuleException);
    /**
     * Check value of EOL test byte
     *
     * @return Status of the operation
     * @param response The returned values will be stored here
     * @throws ModuleException if object needed is null
     * @since 15 July 2008
     */
    virtual BEP_STATUS_TYPE ReadEolTestByte(SerialString_t &response) throw(ModuleException);
    /**
     * Write EOL test byte
     *
     * @return Status of the operation
     * @param response The result of the Test Sequence
     * @throws ModuleException if object needed is null
     * @since 17 July 2008
     */
    virtual BEP_STATUS_TYPE WriteEolTestByte(UINT16 &testByte) throw(ModuleException);
    /**
     * Reads the wheel speeds from the module.
     *
     * @param speeds map of wheel speeds keyed on LFWHEEL, RFWHEEL, etc.
     * which come out of BepDefs.h
     * @return The status of the operation.
     * @throws ModuleException if CheckObjectStatus() fails
     */
    virtual BEP_STATUS_TYPE ReadSensorSpeeds(WheelSpeeds_t &speeds)  throw(ModuleException);

    virtual BEP_STATUS_TYPE EspRecovery(const string &valve)  throw(ModuleException);
    virtual BEP_STATUS_TYPE EspReduction(const string &corner)  throw(ModuleException);



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
     * Store the DTC parameters from the config file.
     * 
     * @param dtcData DTC data from the config file.
     */
    void LoadDtcData(const XmlNode *dtcData);
    /**
     * Get/Set the index to the number of DTCs returned by the module.
     * 
     * @param index  Index to the number of DTCs returned by the module.
     * 
     * @return Index to the number of DTCs returned by the module.
     */
    const UINT8& DtcCountIndex(const UINT8 *index = NULL);
    /**
     * Get/Set the number of bytes that make up one DTC record.
     * 
     * @param numBytes The number of bytes that make up one DTC record.
     * 
     * @return The number of bytes that make up one DTC record.
     */
    const UINT8& BytesPerDtc(const UINT8 *numBytes = NULL);
    /**
     * Get/Set the index of the first byte of the first DTC returned by the module.
     * 
     * @param index  The index of the first byte of the first DTC returned by the module.
     * 
     * @return The index of the first byte of the first DTC returned by the module.
     */
    const UINT8& DtcStartIndex(const UINT8 *index = NULL);
    /**
     * Index to the byte that indicates the number of DTCs returned by the module.
     */
    UINT8  m_dtcCountIndex;
    /**
     * Number of bytes for each DTC returned by the module.
     */
    UINT8  m_bytesPerDtc;
    /**
     * Index to the first DTC returned by the module.
     */
    UINT8  m_dtcStartIndex;
};
//-----------------------------------------------------------------------------
#endif //MandoAbsModule_h
