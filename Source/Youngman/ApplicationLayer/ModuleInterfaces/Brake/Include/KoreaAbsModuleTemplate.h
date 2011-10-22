//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/KoreaAbsModuleTemplate.h 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//  Module Interface for a Korea ABS Controller module.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/ModuleInterfaces/Include/KoreaAbsModuleTemplate.h $
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 1     8/30/04 7:23p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
// 
//*************************************************************************
#ifndef KoreaAbsModuleTemplate_h
#define KoreaAbsModuleTemplate_h

#include "GenericABSModuleTemplate.h"
#include "BepDefs.h"
#include "ModuleException.h"

class XmlNode;

/**
 * Class to handle messages specific to a Korea ABS.
 * 
 * @author Ross Wiersema
 * @since 18 December 2003
 */

template <class ProtocolFilterType>
class KoreaAbsModuleTemplate : public GenericABSModuleTemplate<ProtocolFilterType>
{
    friend class KoreaAbsModuleTemplateTest;

public:
    
    /**
     * Constructor
     */
    KoreaAbsModuleTemplate();

    /**
     * Destructor
     */
    virtual ~KoreaAbsModuleTemplate();

    /**
     * Returns the name of this module.
     * @since December 18 2003
     */
    inline virtual string ModuleName();

    // These string represent the methods that GetInfo calls can execute
    const string m_readSensorSpeeds;

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
	virtual BEP_STATUS_TYPE GetInfo(string method, bool &value)  throw(ModuleException);

    /**
	 * GetInfo for Fault types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
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
	virtual BEP_STATUS_TYPE GetInfo(string method, WheelSpeeds_t &value)  throw(ModuleException);

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

    /**
	 * GetInfo for float types
	 *
	 * @param methodName The method to use
	 * @param value The value to fill
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 4 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);

    /**
	 * GetInfo for methods that take no arguments
	 *
	 * @param methodName The method to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName) throw(ModuleException);

protected:
    /**
	 * Reads the wheel speeds from the module.
	 *
	 * @param speeds map of wheel speeds keyed on LFWHEEL, RFWHEEL, etc.
	 * which come out of BepDefs.h
	 * @return The status of the operation.
	 * @throws ModuleException if CheckObjectStatus() fails
	 * @since 2003 02 25
	 */
	virtual BEP_STATUS_TYPE ReadSensorSpeeds(WheelSpeeds_t &speeds)  throw(ModuleException);

    /**
	 * Initialize hook for the GenericABSModuleTemplate, simply calls
	 * the GenericModuleTemplate initialize hook.
	 *
	 * @param configNode The XML Config node for the module
	 * @return Status of the operation, in this instance the
	 * value returned by GenericModuleTemplate's initialize hook.
	 */
	virtual bool InitializeHook(const XmlNode *configNode);
};

#endif                                
