//*************************************************************************
// FILE DESCRIPTION:
//	Module Interface for the Isuzu Mimamori module.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#ifndef MimamoriModule_h
#define MimamoriModule_h
//-------------------------------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"
//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
class MimamoriModule : public GenericModuleTemplate<ProtocolFilterType>
{
public:
	MimamoriModule();
	virtual ~MimamoriModule();

	/**
	 * Enter diagnostic mode.
	 * <p><b>Description:</b><br>
	 * The module will be commanded to enter a new diagnostic session.
	 * <p>
	 *
	 * @return Status of commanding the module to enter diagnostic mode.
	 * @throws ModuleException if object needed is null
	 */
	virtual BEP_STATUS_TYPE EnterDiagnosticMode(void);

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
    bool InitializeHook (const XmlNode *configNode);

	/**
	 * <p><b>Description:</b><br>
	 * Return a string representing the name of the module interface.
	 * <p>
	 * @returns "GenericModuleTemplate" for this class
	 */
	virtual string ModuleName(void);

	/**
	 * Read module faults.
	 * <p><b>Description:</b><br>
	 * Read the fault data from the module.
	 * <p>
	 * @param faultCodes The fault data read from the module.
	 * 
	 * @return Status of reading the fault data from the module.
	 */
	virtual BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);

    /**
     * GetInfo for string types
     *
     * @param methodName The method to use
     * @param value The value to use
     * @return Status
     * @throw ModuleException if the method cannot be found
     * @since 18 December 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, string &value) throw(ModuleException);

    /**
     * GetInfo for float types
     *
     * @param methodName The method to use
     * @param value The value to fill
     * @return Status
     * @throw ModuleException if the method cannot be found
     * @since 18 December 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

    /**
     * GetInfo for int types, it takes a method name and a value and
     * executes the specified method using the value as the parameter
     * to the method. This method can be used for enum data types, such
     * as Corner in this case.
     *
     * @param methodName The method to use
     * @param value The value to use/fill in
     * @throw ModuleException if the method cannot be found, or the
     * method we call throws a module exception
     * @return The status of the operation
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value) throw(ModuleException);

    /**
     * GetInfo for UINT8 types
     * 
     * @param methodName The method to use
     * @param value The value to fill
     * @return Status
     * @since 2 Apr 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);

    /**
     * GetInfo for UINT16 types
     * 
     * @param methodName The method to use
     * @param value The value to fill
     * @return Status
     * @since 2 Apr 2003
     */
    virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);

    virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t & value) throw(ModuleException);



private:

	BEP_STATUS_TYPE GatherMimamoriData();

	void Commcheckout();

    /**
     * Get/Set the index for the byte that contains the number of reported DTCs.
     * 
     * @param byte   Index for the byte that contains the number of reported DTCs.
     * 
     * @return Index for the byte that contains the number of reported DTCs.
     */
    const UINT8& DtcStartIndex(const UINT8 *byte = NULL);
    /** Index of the byte containing the start Index for DTC. */
    UINT8 m_dtcStartIndex;

	typedef struct _VehicleInfoData
	{
		string engineModel;
		string tmModel;
		float tireRadius;
		float finalGearRatio;
		string dealerArea;
		string vin;
	}VehicleInfoData;

	VehicleInfoData m_vehicleInfo;
};

//-------------------------------------------------------------------------------------------------
#endif //MimamoriModule_h
