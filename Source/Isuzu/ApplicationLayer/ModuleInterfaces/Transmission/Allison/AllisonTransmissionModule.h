//*************************************************************************
// FILE DESCRIPTION:
//	Module Interface for the Allison Transmission controller.
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
#ifndef AllisonTransmissionModule_h
#define AllisonTransmissionModule_h
#define DTC_MESSAGE_SIZE 8
//*************************************************************************
#include "GenericTransmissionModuleTemplate.cpp"

/**
 * Allison Transmission control module interface.
 * This class handles all the Allison module specific functions.
 */
template <class ProtocolFilterType> 
class AllisonTransmissionModule : public GenericTransmissionModuleTemplate<ProtocolFilterType>
{
public:
	/**
	 * Class constructor.  
	 */
	AllisonTransmissionModule(void);

    /**
	 * Class destructor.
	 * All communication objects created in the constructor should be destroyed
	 * here.
	 */
	virtual ~AllisonTransmissionModule(void);

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
     * Read the current gear from the transmission control module.
     * 
     * @param filter ProtocolFilter object to use for reading the current transmission gear.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual BEP_STATUS_TYPE MonitorTransmissionGear(IProtocolFilter *filter);

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
     * Read faults stored in the module.
     * <p><b>Description:</b><br>
     * The fault data will be read from the module.
     * <br><b><i>Note:</i></b> Many modules handle fault data differently.  There is no generic implementation.
     * <p>
     * @param faultCodes Fault Codes read from the module.
     * @return BEP_STATUS_SOFTWARE
     */
    virtual BEP_STATUS_TYPE ReadFaults(FaultStatusVector_t &faultCodes);

protected:


    /**
     * Read faults stored in the module.
     * <p><b>Description:</b><br>
     * The number of faults stored in the module will first be read.  If faults have been stored in the module, then
     * the faults will be read from the module.  The provided communication object will be used to read the fault data
     * from the module.
     * <p>
     * 
     * @param filter     Communication object to use to read the fault data from the module.
     * @param faultCodes Fault Codes read from the module.
     * 
     * @return Status of reading the fault data from the module.
     */
    virtual BEP_STATUS_TYPE ReadFaults(IProtocolFilter *filter, FaultVector_t &faultCodes);

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

//*************************************************************************
#endif  //AllisonTransmissionModule_h
