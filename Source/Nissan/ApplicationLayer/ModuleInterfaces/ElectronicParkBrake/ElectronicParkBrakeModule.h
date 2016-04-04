//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the Electronic Park Brake.
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
#ifndef ElectronicParkBrakeModule_h
#define ElectronicParkBrakeModule_h
//-------------------------------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"
//-------------------------------------------------------------------------------------------------
template<class ProtocolFilterType>
class ElectronicParkBrakeModule : public GenericModuleTemplate<ProtocolFilterType>
{
public:
    ElectronicParkBrakeModule();
    virtual ~ElectronicParkBrakeModule();

    /**
     * <p><b>Description:</b><br>
     * Return a string representing the name of the module interface.
     * <p>
     * @returns "ElectronicParkBrake" for this class
     */
    virtual string ModuleName(void);

    /**
     * Initialize the module interface.
     * <p><b>Description:</b><br>
     * Module specific initialization will be completed.  The main module communication object will be intialized.
     * If any sub-class needs to perform specific initialization, it should override this class instead of Initialize().
     * <p>
     * 
     * @param configNode Configuration data to use for initializing the module interface.
     * 
     * @return Flag indicating if module specific initialization has been completed.
     */
    virtual bool InitializeHook(const XmlNode *configNode);

    virtual BEP_STATUS_TYPE PerformAssemblyCheck(void);

    virtual BEP_STATUS_TYPE ParameterArbitration(void);

    /**
     * Unlock module security.
     * <p><b>Description:</b><br>
     * The security seed will be read from the module.  The security unlock key will then be calculated using the seed
     * read from the module.  The key will then be sent to the module in order to unlock security.  If the seed read
     * from the module is 0, then security has already been unlocked, so no key calculation is needed.
     * <p>
     * <b>Message Tags:</b>
     *      <ul>
     *      <li> ReadSecuritySeed </li>
     *      <li> UnlockModuleSecurity </li>
     *      </ul>
     * 
     * <b>Functions Called:</b>
     *      <ul>
     *      <li> ReadModuleData() </li>
     *      <li> CommandModule() </li>
     *      <li> CalculateSecurityKey() </li>
     *      </ul>
     * 
     * @return Status of unlocking module security.
     */
    virtual BEP_STATUS_TYPE UnlockModuleSecurity();

protected:
    
private:
    /**
     * Calculate a security unlock key given a specific seed that is read from the module.
     * <p><b>Description:</b><br>
     * The security unlock key will be calculated from the provided seed.
     * <p>
     * 
     * @param seed   Security seed used to generate a security unlock key.
     * 
     * @return The calculated security key
     */
    const SerialArgs_t CalculateSecurityKey(const UINT32 &seed);

    /**
     * Modifiy the seed value as part of the unlock calculation 
     * <p><b>Description:</b><br>
     * Part of the security unlock calculation, the sequence of bits in the seed must 
     * be reversed.  For Example: 00111111 becomes 11111100.  I suppose Reflect would
     * be a more accurate discription than rotate, but this is what the spec calls it.
     * <p>
     * 
     * @param seed   Security key used to generate a security unlock key.
     * 
     */
    void RotateKey(UINT32 &key);
};

//-------------------------------------------------------------------------------------------------
#endif //ElectronicParkBrakeModule_h
