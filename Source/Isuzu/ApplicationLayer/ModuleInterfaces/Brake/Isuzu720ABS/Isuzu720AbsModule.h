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
#ifndef Isuzu720AbsModule_h
#define Isuzu720AbsModule_h
//-----------------------------------------------------------------------------
#include "GenericABSModuleTemplate.cpp"
//-----------------------------------------------------------------------------

template<class ProtocolFilterType>
class Isuzu720AbsModule : public GenericABSModuleTemplate<ProtocolFilterType>
{
public:
    /** Class constructor */
    Isuzu720AbsModule();
    /** Class destructor */
    virtual ~Isuzu720AbsModule();
    /**
     * Return the name of this module.
     * <p><b>Description:</b><br>
     * The name of the module will be returned to the calling function.
     * <p>
     *
     * @return TevesAbsModule
     */
    virtual string ModuleName();

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
     *      <li> GenericABSModuleTemplate<ProtocolFilterType>::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadPartNumberRegisters </li>
     *      <li> ReadTraceDataRegisters </li>
     *      <li> ReadTireSize </li>
     *      </ul>
     * 
     * @param methodName The data to read from the module.
     * @param value The value read from the module.
     * 
     * @return Status of reading the data from the module.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Data was successfully read from the module. </li>
     *      <li> BEP_STATUS_FAILURE - Could not read data item from the module. </li>
     *      </ul>
     * 
     * @throw ModuleException if the method cannot be found
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, string &value) throw(ModuleException);
    /**
     * GetInfo for boolean types
     * <p><b>Description:</b><br>
     * Boolean type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadBrakeSwitch() </li>
     *      <li> CurrentBrakeSwitchState() </li>
     *      <li> SawBrakeSwitchOff() </li>
     *      <li> SawBrakeSwitchOn() </li>
     *      <li> ReadModuleData() </li>
     *      <li> ModuleName() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> CheckInPlantStatus </li>
     *      <li> IsEvacAndFillComplete </li>
     *      <li> IsFluidFillSwitchOn </li>
     *      <li> IsGSwitchPresent </li>
     *      <li> ReadBrakeSwitch </li>
     *      <li> CurrentBrakeSwitchState </li>
     *      <li> SawBrakeSwitchOff </li>
     *      <li> SawBrakeSwitchOn </li>
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
    virtual BEP_STATUS_TYPE GetInfo(string method, bool &value)  throw(ModuleException);
    /**
     * GetInfo for fault data
     * <p><b>Description:</b><br>
     * Fault data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None<p>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t & value) throw(ModuleException);

    /**
     * GetInfo for WheelSpeeds_t type data.
     * <p><b>Description:</b><br>
     * Wheelspeed_t data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> ReadSensorSpeeds() </li>
     *      <li> GenericABSModuleTemplate::ModuleName() </li>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b>
     *      <ul>
     *      <li> ReadSensorSpeeds </li>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, WheelSpeeds_t &value)  throw(ModuleException);

    /**
     * GetInfo for integer types.
     * <p><b>Description:</b><br>
     * Integer data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None<p>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value) throw(ModuleException);

    /**
     * GetInfo for unsigned char types
     * <p><b>Description:</b><br>
     * Unsigned char type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <p>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);

    /**
     * GetInfo for unsigned short types
     * <p><b>Description:</b><br>
     * Unsigned short type data will be read from the module and returned to the calling function.
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
     *      <li> ReadModuleConfiguration </li>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);

    /**
     * GetInfo for float types
     * <p><b>Description:</b><br>
     * Float type data will be read from the module and returned to the calling function.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <p>
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
    virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

    /**
     * Command the module to perform the specified action.
     * <p><b>Description:</b><br>
     * The module will be commanded to perform an action.
     * <p>
     * 
     * <b>Functions Called:</b><br>
     *      <ul>
     *      <li> GenericModuleTemplate::GetInfo() </li>
     *      </ul>
     * 
     * <b>Data Items Supported:</b> None <p>
     *
     * @param methodName The action to command to the module.
     * @return Status of commanding the module.
     * @throw ModuleException if the method cannot be found
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName) throw(ModuleException);

protected:
    /**
     * Read the part number from the module.
     * <p><b>Description:</b><br>
     * Uses two reads to get the first and last sections of the partnumber from the module. 
     *<p>
     * 
     * <b>Config file item:</b>
     *          
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadPartNumber </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *           
     * @param partNumber used to pass back the part number read from the module.          
     * 
     * @return Status of reading the part number from the module.
     *      <ul>
     *      <li> BEP_STATUS_SUCCESS - Successfully read the part number from the module. </li>
     *      <li> BEP_STATUS_FAILURE - Could not correctly read the part number from the module. </li>
     *      </ul>
     */
    virtual BEP_STATUS_TYPE ReadModulePartNumber(string &partNumber);

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
//-----------------------------------------------------------------------------
#endif  //Isuzu720AbsModule_h
