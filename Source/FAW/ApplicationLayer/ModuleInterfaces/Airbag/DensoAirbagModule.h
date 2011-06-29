//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef DensoAirbagModule_h
#define DensoAirbagModule_h
//-----------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"
//-----------------------------------------------------------------------------
template<class ProtocolFilterType>
class DensoAirbagModule : public GenericModuleTemplate<ProtocolFilterType>
{
public:
    DensoAirbagModule();
    virtual ~DensoAirbagModule();
    /**
     * Return the name of this module.
     * <p><b>Description:</b><br>
     * The name of the module will be returned to the calling function.
     * <p>
     *
     * @return Denso Airbag Module
     */
    virtual string ModuleName();
    /**
     * Perform a low baud init sequence in order to establish communications with the module.
     * 
     * @return Status of establishing communications with the module.
     */
    BEP_STATUS_TYPE PerformModuleLinkup(void);
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
     * Get/Set the amount of time(ms) to drive the comm line low.
     * 
     * @param lowTime Amount of time(ms) to drive the line low.
     * 
     * @return Amount of time(ms) to drive the line low.
     */
    UINT32& SignalLowTime(UINT32 *lowTime = NULL);
    /**
     * Get/Set the amount of time(ms) to float the comm line high.
     * 
     * @param lowTime Amount of time(ms) to float the line high.
     * 
     * @return Amount of time(ms) to float the line high.
     */
    UINT32& SignalHighTime(UINT32 *highTime = NULL);
    /**
     * Get/Set the amount of time(ms) to wait for a message after sending the wake up signal.
     * 
     * @param wakeupDelay
     *               Amount of time(ms) to wait for the module to response to the wake up signal.
     * 
     * @return Amount of time(ms) to wait for the module to response to the wake up signal.
     */
    INT32 WakeUpResponseDelay(const INT32 *wakeupDelay = NULL);
    /**
     * Get/Set the number of wake up attempts to make with the module.
     * 
     * @param attempts The number of wake up attempts to establish comms with the module.
     * 
     * @return The number of wake up attempts to establish comms with the module.
     */
    UINT8 WakeUpAttempts(const UINT8 *attempts = NULL);
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
     * Amount of time (ms) to drive the line low.
     */
    UINT32 m_signalLowTime;
    /**
     * Amount of time(ms) to float the line high.
     */
    UINT32 m_signalHighTime;
    /**
     * Amount of time for the modue to respond to the wake up signal.
     */
    INT32  m_wakeupResponseDelay;
    /**
     * Number of times to try to establish comms with the module.
     */
    UINT8  m_wakeupAttempts;
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
#endif //DensoAirbagModule_h
