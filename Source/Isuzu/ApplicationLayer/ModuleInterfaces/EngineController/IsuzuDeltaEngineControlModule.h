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
#ifndef IsuzuDeltaEngineControlModule_h
#define IsuzuDeltaEngineControlModule_h
//-------------------------------------------------------------------------------------------------
#include "GenericEmissionsModuleTemplate.cpp"
#define DTC_MESSAGE_SIZE 8
//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
class IsuzuDeltaEngineControlModule : public GenericEmissionsModuleTemplate<ProtocolFilterType>
{
public:
    IsuzuDeltaEngineControlModule();
    virtual ~IsuzuDeltaEngineControlModule();

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
     * GetInfo for string types
     * <p><b>Description:</b><br>
     * String type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadVIN </li>
     *      <li> ReadSystemSupplierID </li>
     *      <li> ReadRepairShopCode </li>
     *      <li> ReadProgrammingDate </li>
     *      <li> ReadDiagnosticDataIdentifier </li>
     *      <li> ReadECUAddress </li>
     *      <li> ReadECUSystems </li>
     *      <li> ReadManufacturingDate </li>
     *      <li> ReadTraceData </li>
     *      <li> ReadSoftwareUsageDesc </li>
     *      <li> ReadSoftwareModuleID </li>
     *      <li> ReadPartNumber </li>
     *      <li> WriteVIN </li>
     *      </ul>
     * 
     * @param methodName The data to read from the module.
     * @param value      The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was successfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data item from the module. </li>
     *         </ul>
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, string &value) throw(ModuleException);
    /**
     * GetInfo for unsigned short types
     * <p><b>Description:</b><br>
     * Unsigned short data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericEmissionsModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);

    /**
     * GetInfo for a vector of unsigned short types
     * <p><b>Description:</b><br>
     * A vector of unsigned short data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericEmissionsModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The values read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT16>&value) throw(ModuleException);

    /**
     * GetInfo for a vector of unsigned char types
     * <p><b>Description:</b><br>
     * A vector of unsigned character data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericEmissionsModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <br>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The vector data read from the module.
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
     * GetInfo for integer types
     * <p><b>Description:</b><br>
     * Integer data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericEmissionsModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadEngineIdleRPM </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     * @param methodName The data to be read from the module.
     * @param value      The vector data read from the module.
     * 
     * @return Status of reading the data from the module.
     *         <ul>
     *         <li> BEP_STATUS_SUCCESS - Data was succesfully read from the module. </li>
     *         <li> BEP_STATUS_FAILURE - Could not read data from the module. </li>
     *         </ul>
     * @exception ModuleException
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value) throw(ModuleException);

    /**
     * GetInfo for unsigned char types
     * <p><b>Description:</b><br>
     * Unsigned character data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericEmissionsModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadNeutralState </li>
     * 	    </ul>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);

    /**
     * GetInfo for bool types
     * <p><b>Description:</b><br>
     * Boolean type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <li> GenericEmissionsModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * 
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, bool &value) throw(ModuleException);

    /**
     * GetInfo for FaultVector_t  types
     * <p><b>Description:</b><br>
     * Fault data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> GenericEmissionsModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * 
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t &value) throw(ModuleException);

    /**
     * GetInfo for float types
     * <p><b>Description:</b><br>
     * Float type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> GenericEmissionsModuleTemplate::GetInfo() </li>
     *      </ul>
     *       
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

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
    virtual BEP_STATUS_TYPE ReadPGNModuleData(const string messageTag, string &data, 
                                           SerialArgs_t *args = NULL, 
                                           SerialString_t *rawData = NULL, 
                                           IProtocolFilter *commObject = NULL);
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
    BEP_STATUS_TYPE GetMAFLearningValue(string messageTag,string &value);

    BEP_STATUS_TYPE EnterNormalMode(void);
    BEP_STATUS_TYPE DisableNormalComms(void);
    BEP_STATUS_TYPE ReadMemoryLocation(vector<UINT8> &memoryContents);
    

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

    UINT8 m_dtcMessageSize;

};
//-------------------------------------------------------------------------------------------------
#endif //IsuzuDeltaEngineControlModule_h
