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




private:

	BEP_STATUS_TYPE GatherMimamoriData();

	void Commcheckout();

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
