//******************************************************************************
// FILE:
//    $Header: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/ModuleInterfaces/Include/FordABSModuleTemplate.h 1     2/07/11 4:45p Mmcnamar $
//
// FILE DESCRIPTION:
//  Module Interface for a Ford ABS Controller module.
//
//==============================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/ModuleInterfaces/Include/FordABSModuleTemplate.h $
// 
// 1     2/07/11 4:45p Mmcnamar
// backup c drive source for cross compiler
// 
// 1     2/07/11 4:43p Mmcnamar
// backup c drive
// 
// 1     5/10/10 2:09p Mmcnamar
// initial check-in for new core.
// 
// 6     11/14/08 3:14p Mmcnamar
// Added method prototype SendModuleKeepAlive().
// 
// 5     6/16/08 1:49p Jwynia
// Updated to read pedal travel DID
// 
// 4     5/05/08 2:46p Mmcnamar
// Updated for hybrids: ABS security; Read sensor DID's; Set/clear test
// status  DID.
// 
// 3     3/28/07 2:01p Mmcnamar
// Added method SetBrakeSwitchStatusOn() .
// 
// 2     5/06/05 12:57p Bmeinke
// Added GetInfo(string, SerialString_t) method
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 3     3/11/05 6:06p Bmeinke
// Added AllStoredCodeLogging() and NoStoredCodeLogging() for D219 Bosch
// ABS
// 
// 2     12/22/04 7:44p Bmeinke
// Added InitializeHook() method to initialize the keep alive and switch
// monitor protocol filters
// Removed the ActuateReduction() method
// Moved StopPumpMotor() to GenericABSModuleTemplate
// 
// 1     9/30/04 2:42p Bmeinke
// Initial full core integration for Ford
// 
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef FordABSModuleTemplate_h
#define FordABSModuleTemplate_h

#include "GenericABSModuleTemplate.h"
#include "BepDefs.h"
#include "ModuleException.h"

class XmlNode;

/**
 * Class to handle messages specific to a Ford ABS.
 * 
 * @author Brian Meinke
 * @since 1 September 2004
 */
template <class ProtocolFilterType>
class FordABSModuleTemplate : public GenericABSModuleTemplate<ProtocolFilterType>
{
    friend class FordABSModuleTemplateTest;

public:
    
    /**
     * Constructor
     */
    FordABSModuleTemplate();

    /**
     * Destructor
     */
    virtual ~FordABSModuleTemplate();

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
     * Returns the name of this module.
     */
    inline virtual string ModuleName();

	/**
	 * GetInfo for string types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, string &value) throw(ModuleException);

	/**
	 * GetInfo for SerialString types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, SerialString_t &value) throw(ModuleException);

	/**
	 * GetInfo for bool types, it takes a method name and a value and
	 * executes the specified method using the value as the parameter
	 * to the method.
	 *
	 * @param methodName The method to use
	 * @param value The value to use/fill in
	 * @throw ModuleException if the method cannot be found, or the
	 * method we call throws a module exception
	 * @return The status of the operation
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, bool &value)  throw(ModuleException);

	/**
	 * GetInfo for Fault types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t & value) throw(ModuleException);

	/**
	 * GetInfo for WheelSpeeds_t types, it takes a method name and a value and
	 * executes the specified method using the value as the parameter
	 * to the method.
	 *
	 * @param methodName The method to use
	 * @param value The value to use/fill in
	 * @throw ModuleException if the method cannot be found, or the
	 * method we call throws a module exception
	 * @return The status of the operation
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, WheelSpeeds_t &value)  throw(ModuleException);

    /**
     * MAM 3/11/08
	 * GetInfo for unsigned char types, it takes a method name and a value and
	 * executes the specified method using the value as the parameter
	 * to the method.
	 *
	 * @param methodName The method to use
	 * @param value The value to use/fill in
	 * @throw ModuleException if the method cannot be found, or the
	 * method we call throws a module exception
	 * @return The status of the operation
	 */
	//virtual BEP_STATUS_TYPE GetInfo(string methodName, unsigned char *value)  throw(ModuleException);



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
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);

	/**
	 * GetInfo for UINT16 types
	 *
	 * @param methodName The method to use
	 * @param value The value to fill
	 * @return Status
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);

	/**
	 * GetInfo for float types
	 *
	 * @param methodName The method to use
	 * @param value The value to fill
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

	/**
	 * GetInfo for methods that take no arguments
	 *
	 * @param methodName The method to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName) throw(ModuleException);

    /**
	 * Method used to set the brake switch status
	 *
	 * @return BEP_STATUS_SUCCESS
	 */
	virtual BEP_STATUS_TYPE SetBrakeSwitchStatusOn(void);

protected:

	/**
	 * Method used to execute the ABS valve sequence built into an ABS module
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE ExecuteAbsRollTestSequence( SerialString_t &response) throw(ModuleException);
	
	/**
	 * Method used to "arm" the ABS valve sequence built into an ABS module
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE ArmAbsRollTestSequence( SerialString_t &response) throw(ModuleException);

	/**
	 * Method used to tell the ABS module to start storing fault codes
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE AllStoredCodeLogging( SerialString_t &response) throw(ModuleException);

	/**
	 * Method used to tell the ABS module to stop storing fault codes
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE NoStoredCodeLogging( SerialString_t &response) throw(ModuleException);

    //MAM 1/8/15
    virtual BEP_STATUS_TYPE DisableTraction( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE EnableTraction( SerialString_t &response) throw(ModuleException);
    //MAM 1/9/15
    virtual BEP_STATUS_TYPE CycleValves1( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE CycleValves2( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE CycleValves3( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE CycleValves4( SerialString_t &response) throw(ModuleException);
    //MAM 3/31/15
    virtual BEP_STATUS_TYPE CycleValves1Stop( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE CycleValves2Stop( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE CycleValves3Stop( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE CycleValves4Stop( SerialString_t &response) throw(ModuleException);

    //MAM 3/14/08
    /**
	 * Method used to read the SBA Test Status DID
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE ReadStatusDID( SerialString_t &response) throw(ModuleException);    

    /**
	 * Method used to read the SBA Pedal Travel DID
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE ReadPedalTravel( SerialString_t &response) throw(ModuleException);

    //Bendix tire size programming
    //MAM 2/10/15 - Updated for EC80
    virtual BEP_STATUS_TYPE ReadCurrentTireSizes( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE ReadCurrentTireSizesEC80( SerialString_t &response) throw(ModuleException);    
    virtual BEP_STATUS_TYPE ReadConfigBytes45( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE ReadLampGndChkEC80( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE ReadHrwCcvs( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE ReadHrwCcvsEC80( SerialString_t &response) throw(ModuleException);
    
    virtual BEP_STATUS_TYPE ReadABSPartNumber( SerialString_t &response) throw(ModuleException);
    //MAM 1/6/15
    virtual BEP_STATUS_TYPE ReadABSSwId( SerialString_t &response) throw(ModuleException);
    //MAM 1/6/15
    virtual BEP_STATUS_TYPE ReadABSSn( SerialString_t &response) throw(ModuleException);    

    /**
	 * Method used to clear the SBA Test Status DID
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE ClearStatusDID( SerialString_t &response) throw(ModuleException);

    /**
	 * Method used to set the SBA Test Status DID if the brake test passes
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE SetStatusDIDPass( SerialString_t &response) throw(ModuleException);    

    /**
	 * Method used to get the ABS module security access seeds
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE RequestSecuritySeeds( SerialString_t &response) throw(ModuleException);

    /**
	 * Method used to get the ABS module security access
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE RequestSecurityAccess( SerialString_t &response) throw(ModuleException);

    /**
	 * Method used to get the ABS module security access seeds
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE RequestBendixSeeds( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE RequestBendixSeeds1( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE RequestBendixSeeds3( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE RequestBendixSeeds7( SerialString_t &response) throw(ModuleException); //MAM 2/9/15

    /**
	 * Method used to get the ABS module security access
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE RequestBendixAccess( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE RequestBendixAccess1( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE RequestBendixAccess3( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE RequestBendixAccess7( SerialString_t &response) throw(ModuleException); //MAM 2/9/15

    /**
	 * Reads the wheel speeds from the module.
	 *
	 * @param speeds map of wheel speeds keyed on LFWHEEL, RFWHEEL, etc.
	 * which come out of BepDefs.h
	 * @return The status of the operation.
	 * @throws ModuleException if CheckObjectStatus() fails
	 */
	virtual BEP_STATUS_TYPE ReadSensorDIDs(WheelSpeeds_t &speeds)  throw(ModuleException);
	
	/**
	 * Reads an individual wheel speed from the module.
	 * 
	 * @param methodName Wheelspeed sensor tag to read (ReadLFSensorSpeed, ReadRFSensorSpeed, etc)
	 * @param value      Wheelspeed sensor data from the module
	 * 
	 * @return The status of the operation.
	 */
	virtual BEP_STATUS_TYPE ReadSensorDID(const std::string &methodName, float &value);

    //MAM 10/30/08
    /**
	 * Method used to send keep alive messages when needed
	 * 
	 * @param response Response from the vehicle to our command
	 * 
	 * @return Status
	 * @exception ModuleException
	 *                   if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE SendModuleKeepAlive( SerialString_t &response) throw(ModuleException);

    //MAM 12/3/14
    virtual BEP_STATUS_TYPE WriteFingerprint( SerialString_t &response) throw(ModuleException);

    //MAM 12/18/14 - Bendix ECU programming
    //MAM 2/10/15 - Updated for Bendix EC-80
    virtual BEP_STATUS_TYPE ProgramTireSizes( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE ProgramTireSizesEC80( SerialString_t &response) throw(ModuleException);
    
    virtual BEP_STATUS_TYPE ProgramConfigBytes45( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE ProgramLampGndChkEC80( SerialString_t &response) throw(ModuleException);
    
    virtual BEP_STATUS_TYPE ProgramHrwCcvs( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE ProgramHrwCcvsEC80( SerialString_t &response) throw(ModuleException);

    //For EC-60 tire sizes only
    virtual BEP_STATUS_TYPE WheelAlignAddAxle( SerialString_t &response) throw(ModuleException);
    virtual BEP_STATUS_TYPE WheelAlignSDAxles( SerialString_t &response) throw(ModuleException);

private:

    //MAM 1/9/15
    /** 
     * Method to read the faults from the module
     * @param faultCodes Vector to store fault codes fromt the module
     * @return The status of the operation
     */ 
    BEP_STATUS_TYPE ReadFaults(FaultVector_t &faultCodes);
    
};

#endif                                
