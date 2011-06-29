//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/ModuleInterfaces/EngineController/Include/BoschEmissionsModule.h 2     6/09/05 2:19p Derickso $
//
// FILE DESCRIPTION:
//	Module Interface for DaimlerChrysler NGC-III controller.  This controller
//  incorporates the engine controller and the transmission controller into one
//  physical module.  The transmission control portion and the engine control
//  portion have different module IDs.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/ModuleInterfaces/EngineController/Include/BoschEmissionsModule.h $
// 
// 2     6/09/05 2:19p Derickso
// Added functionality to store comm bus from the config file.
// 
// 1     2/22/05 12:02p Derickso
// New engine controller.
// 
// 1     1/20/05 3:17p BMiedlar
// Converting class to template form.
// 
// 
//*************************************************************************
#ifndef BoschEmissionsModule_h
#define BoschEmissionsModule_h
//*************************************************************************
#include "GenericEmissionsModuleTemplate.cpp"

/**
 * Bosch Module Interface Class.
 * Module Interface for DaimlerChrysler Bosch controller.  This controller
 * incorporates the engine controller and the transmission controller into one
 * physical module.  The transmission control portion and the engine control
 * portion have diferent module IDs.
 * This class provides an interface to the module for all
 * engine controller operations.
 * 
 * @see GenericModule
 * @see GenericEmissionsModuleTemplate
 */
template <class ProtocolFilterType>
class BoschEmissionsModule : public GenericEmissionsModuleTemplate<ProtocolFilterType>
{
public:
	/**
	 * Class constructor.
	 * The constructor is also responsible for creating a protocol filter
	 * object to use in communicating with the vehicle.
	 */
	BoschEmissionsModule(void);
	/**
	 * Class destructor.
	 */
	virtual ~BoschEmissionsModule();
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
	 * GetInfo for int types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, int &value) throw(ModuleException);
	/**
	 * GetInfo for unsigned short types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT16 &value) throw(ModuleException);
	/**
	 * GetInfo for unsigned char types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, UINT8 &value) throw(ModuleException);
	/**
	 * GetInfo for a vector of unsigned short types
	 *
	 * @param methodName The method to use
	 * @param value The vector to fill
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT16>&value) throw(ModuleException);
	/**
	 * GetInfo for a vector of unsigned character types
	 *
	 * @param methodName The method to use
	 * @param value The vector to fill
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, vector<UINT8>&value) throw(ModuleException);
	/**
	 * GetInfo for bool types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, bool &value) throw(ModuleException);
	/**
	 * GetInfo for FaultVector_t  types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, FaultVector_t &value) throw(ModuleException);
	/**
	 * GetInfo for float types
	 *
	 * @param methodName The method to use
	 * @param value The value to use
	 * @return Status
	 * @throw ModuleException if the method cannot be found
	 * @since 3 Mar 2003
	 */
	virtual BEP_STATUS_TYPE GetInfo(string methodName, float &value) throw(ModuleException);
	/**
	 * Method that returns a string representing the name of
	 * this module. Child classes should override this to
	 * return their own name.
	 *
	 * @returns "GenericModule" for this class
	 * @since 21 Jan 2003
	 */
	virtual string ModuleName();
	/**
	 * Unlock module security so data can be written to the module.
	 * 
	 * @return Status of the operation.
	 * @since Version 1.0
	 */
	virtual BEP_STATUS_TYPE UnlockModuleSecurity();
	/**;
	 * Read the requested axle speed from the module.
	 * 
	 * @param axle      Which axle to read the speed from.
	 *                  Valid values are Front and Rear.
	 * @param axleSpeed Speed for the axle read from the module.
	 * @return Status of the operation.
	 * @since Version 1.0
	 */
	virtual BEP_STATUS_TYPE ReadAxleSpeed(const string &axle, float &axleSpeed);
    /**
	 * Save the security key gen
	 * 
	 * @return Status of the operation.
	 * @since Version 1.0
	 */
    virtual inline void StoreSecurityKeyGen(const INT32 &keyGen);
    
    /**
	 * Get the Security Key Gen
	 * 
	 * @return Status of the operation.
	 * @since Version 1.0
	 */
    virtual inline const INT32& GetSecurityKeyGen(void);

    /**
     * Load the look up table form the .dat file
     * 
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual BEP_STATUS_TYPE LoadLookupTable(UINT8 lookupTable[8][4]);

    
	/**
	 * ReadSensors - reads sensor information and stores valuees
	 * 
	 */
	virtual BEP_STATUS_TYPE ReadSensors(INT32 &engineRPM,
										INT32 &boostPressure,
										float &vehicleSpeed,
										float &throttlePosition,
										string &cruiseMode);	

    virtual BEP_STATUS_TYPE ReadSensors(INT32 &engineRPM,
                                        INT32 &boostPressure,
                                        float &vehicleSpeed,
                                        float &throttlePosition,
                                        string &cruiseMode,
                                        bool &parkNeutralSwitch);

	/**
	 * Establish communication with the module.  The Bosch module requires the line to
	 * be driven low for a specific amount of time followed by the letting the line
	 * go high for the same amount of time.
	 * 
	 * @param messageTag Tag for the message to send to the module.
	 * @return True - communications were established with the module.
	 * @since Version 1.0
	 */
	virtual BEP_STATUS_TYPE SendModuleWakeUpMessage(void);
	/**
	 * Setup a dynamic LID in order to read multiple LIDs at the same time.
	 * 
	 * @return BEP_STATUS_SUCCESS will be returned if the LID is setup properly.
	 */
	virtual BEP_STATUS_TYPE SetupDynamicLID(void);
	/**
	 * Clear the dynamic LID so we can redefine it.
	 * 
	 * @return BEP_STATUS_SUCCESS if the LID was cleared properly.
	 */
	virtual BEP_STATUS_TYPE ClearDynamicLID(void);
	/**
	 * Update the status byte with the result of a test.
	 *
	 * @param address    Status byte to be updated.
	 * @param testBit    Bit in the status byte to update.
	 * @param testResult Value to update bit with.
	 * @return Status of the operation.
	 */
	virtual BEP_STATUS_TYPE UpdateEEPROMTestBit(const UINT16 address, const UINT16 testBit,
												const EEPROM_TEST_RESULT testResult);
	/**
	 * Write the EOL status bytes to the module.
	 * 
	 * @param statusBytes
	 *               Status bytes to write to the module.
	 * 
	 * @return Status of the operation.
	 */
	virtual BEP_STATUS_TYPE WriteEOLStatusBytes(const vector<UINT8> &statusBytes);
	/**
	 * Begin reading switches in the background.
	 * 
	 * @param isAutoTrans
	 *               True - the vehicle is equipped with an automatic transmission.
	 * 
	 * @return Result of the operation.
	 * @throws ModuleException
	 *                if the keep alive comm is null
	 */
	virtual BEP_STATUS_TYPE StartSwitchMonitor(const bool &isAutoTrans) throw(ModuleException);
	/**
	 * Begin reading switches in the background.
	 * 
	 * @return Result of the operation.
	 * @throws ModuleException
	 *                if the keep alive comm is null
	 */
	virtual BEP_STATUS_TYPE StartSwitchMonitor(void) throw(ModuleException);
	 
protected:
	/**
	 * This is a method that allows subclasses to insert
	 * instructions to be done during the Initialize block.
	 * Initialize calls InitializeHook during it's execution,
	 * so any call to GenericModule::Initialize() will  call
	 * this method.  The default implementation given here
	 * simply returns true
	 *
	 * @param configNode An XML node to get configuration
	 * information from.
	 *
	 * @since 21 Jan 2003
	 */
	virtual bool InitializeHook(const XmlNode *configNode);
	/**
	 * Read an EOL status register from the module.
	 *
	 * @param eolStatusRegister
	 *               EOL status register to be read.
	 * @param registerContents
	 *               Value read from the register.
	 * @return Status of the operation.
	 */
	virtual BEP_STATUS_TYPE ReadEOLStatusRegister(const UINT16 &eolStatusRegister, UINT16 &registerContents);
	/**
	 * Read the EOL status bytes from the module.
	 * 
	 * @param statusBytes  Status bytes read from the module.
	 * @return Status of the operation.
	 */
	virtual BEP_STATUS_TYPE ReadEOLStatusBytes(vector<UINT8> &statusBytes);
    
    /**
	 * Calculate security seed for Gen 0 modules
	 * 
	 * @return Status of the operation.
	 * @since Version 1.0
	 */
    virtual BEP_STATUS_TYPE CalculateSecurityKeyGen0(const SerialArgs_t &seed, SerialArgs_t &key);

    /**
	 * Calculate security seed for Gen 1 modules
	 * 
	 * @return Status of the operation.
	 * @since Version 1.0
	 */
    virtual BEP_STATUS_TYPE CalculateSecurityKeyGen1(const SerialArgs_t &seed, SerialArgs_t &key);
	 
	/**
	 * Request a security seed from the module.
	 * 
	 * @param seed       Seed read from the module.
	 * @param messageTag Tag for the message to send to the module.
	 * @return Status of the read.
	 * @since Version 1.0
	 */
	virtual BEP_STATUS_TYPE RequestSecuritySeed(SerialArgs_t &seed, const std::string &messageTag);
	/**
	 * Get the length of time to drive the time low.
	 * 
	 * @return Length of time to drive the time low.
	 * @since Version 1.0
	 */
	virtual inline const INT32 &GetSignalLowTime(void)
	{
		return m_lineLowTime;
	};
	/**
	 * Get the length of time to drive the time high.
	 * 
	 * @return Length of time to drive the time high.
	 * @since Version 1.0
	 */
	virtual inline const INT32 &GetSignalHighTime(void)
	{ 
		return m_lineHighTime;
	};
	/**
	 * Get the length of time to wait before looking for a response to the wake up message.
	 * 
	 * @return Length of time to wait for a response.
	 * @since Version 1.0
	 */
	virtual inline const INT32 &GetWakeUpResponseDelay(void)
	{
		return m_wakeUpResponseDelay;
	};
	/**
	 * Get the number of attepts to try to establish module communications.
	 * 
	 * @return Number of attempts.
	 * @since Version 1.0
	 */
	virtual inline const INT32 &GetWakeUpAttempts(void)
	{
		return m_wakeUpAttempts;
	};
	/**
	 * Get the tag for the wake up message to send to the module. 
	 * 
	 * @return Tag for the wake up message.
	 * @since Version 1.0
	 */
	virtual const std::string &GetWakeUpMessageTag(void)
	{
		return m_wakeUpMessageTag;
	};
	/**
	 * Get the number of bytes in the security seeds.
	 *
	 * @return Number of security seed bytes.
	 */
	virtual const INT32& GetNumberOfSecuritySeedBytes(void);
	/**
	 * Get the index of the first security seed byte in the response.
	 *
	 * @return Index to the first security seed byte.
	 */
	virtual const INT32& GetFirstSecuritySeedIndex(void);
	/**
	 * Get the number of EOL status bytes.
	 *
	 * @return Number of status bytes.
	 */
	virtual const INT32& GetNumberOfStatusBytes(void);
	/**
	 * Get the index of the first status byte.
	 *
	 * @return Index of the first status bytes.
	 */
	virtual const INT32& GetFirstStatusByteIndex(void);
	/**
	 * Save the time to drive the signal low.
	 * 
	 * @param lowTime Length of time to drive the line low.
	 * @since Version 1.0
	 */
	virtual inline void SetSignalLowTime(const std::string &lowTime)
	{
		SetSignalLowTime(atoi(lowTime.c_str()));
	};
	/**
	 * Save the time to drive the signal low.
	 * 
	 * @param lowTime Length of time to drive the line low.
	 * @since Version 1.0
	 */
	virtual inline void SetSignalLowTime(const INT32 &lowTime)
	{
		m_lineLowTime = lowTime;
	};
	/**
	 * Save the time to drive the signal high.
	 * 
	 * @param highTime Length of time to drive the line high.
	 * @since Version 1.0
	 */
	virtual inline void SetSignalHighTime(const std::string &highTime)
	{
		SetSignalHighTime(atoi(highTime.c_str()));
	};
	/**
	 * Save the time to drive the signal high.
	 * 
	 * @param highTime Length of time to drive the line high.
	 * @since Version 1.0
	 */
	virtual inline void SetSignalHighTime(const INT32 &highTime)
	{
		m_lineHighTime = highTime;
	};
	/**
	 * Save the length of time to wait for a response to the wake up message.
	 * 
	 * @param delayTime Amount of time to wait (ms).
	 * @since Version 1.0
	 */
	virtual inline void SetWakeUpResponseDelay(const std::string &delayTime)
	{
		SetWakeUpResponseDelay(atoi(delayTime.c_str()));
	};
	/**
	 * Save the length of time to wait for a response to the wake up message.
	 * 
	 * @param delayTime Amount of time to wait (ms).
	 * @since Version 1.0
	 */
	virtual inline void SetWakeUpResponseDelay(const INT32 &delayTime)
	{
		m_wakeUpResponseDelay = delayTime;
	};
	/**
	 * Save the number of attempts to make to establish communications with the module.
	 * 
	 * @param attempts Number of attempts.
	 * @since Version 1.0
	 */
	virtual inline void SetWakeUpAttempts(const std::string &attempts)
	{
		SetWakeUpAttempts(atoi(attempts.c_str()));
	};
	/**
	 * Save the number of attempts to make to establish communications with the module.
	 * 
	 * @param attempts Number of attempts.
	 * @since Version 1.0
	 */
	virtual inline void SetWakeUpAttempts(const INT32 &attempts)
	{
		m_wakeUpAttempts = attempts;
	};
	/**
	 * Store the number of bytes in the security seed.
	 *
	 * @param numberOfBytes  Number of bytes in the security seed.
	 */
	virtual void StoreNumberOfSecuritySeedBytes(const INT32 &numberOfBytes);
	/**
	 * Store the index of the first security seed byte.
	 *
	 * @param index  Index of the first security seed byte.
	 */
	virtual void StoreFirstSecuritySeedIndex(const INT32 &index);
	/**
	 * Store the number of EOL status bytes.
	 *
	 * @param numberOfBytes  Number of status bytes.
	 */
	virtual void StoreNumberOfStatusBytes(const INT32 &numberOfBytes);
	/**
	 * Store the index of the first status byte.
	 *
	 * @param index  Index of the first status bytes.
	 */
	virtual void StoreFirstStatusByteIndex(const INT32 &index);
    /**
     * Get the CommunicationsBus tag from the configuration file.
     * 
     * @return Tag identifying the communications Bus.
     * @since 18 Apr 2005
     */
	virtual const std::string &GetCommunicationsBus(void)
	{
		return m_moduleCommunicationsBus;
	};
	
private:
    /**
     * Store the module communications BUS type.
     * 
     * @param communicationsBus
     *               Communictions Bus type.
     * * @since 18 Apr 2005
     */
	void StoreCommunicationsBus(const string &communicationsBus);
    /**
	 * Length of time to drive the line low.
	 * @since Version 1.0
	 */
	INT32 m_lineLowTime;
	/**
	 * Length of time to drive the line high.
	 * @since Version 1.0
	 */
	INT32 m_lineHighTime;
	/**
	 * Length of time to wait for a response to the wake up command.
	 * @since Version 1.0
	 */
	INT32 m_wakeUpResponseDelay;
	/**
	 * Number of attempts to establish communications with the module.
	 * @since Version 1.0
	 */
	INT32 m_wakeUpAttempts;
	/**
	 * Index of the first seed byte in the module response.
	 */
	INT32 m_seedStartIndex;
	/**
	 * Number of bytes the security seed contains.
	 */
	INT32 m_numberOfSeedBytes;
	/**
	 * Index of the first status byte.
	 */
	INT32 m_statusByteStartIndex;
	/**
	 * Number of status bytes.
	 */
	INT32 m_numberOfStatusBytes;
	/**
	 * Tag for the wake up message to send to the module.
	 * @since Version 1.0
	 */
	std::string m_wakeUpMessageTag;
	/**
	 * Mapping of body style code to text.
	 * @since Version 1.0
	 */
	XmlNodeMap m_bodyStyleMapping;
	/**
	 * Mapping of the engine controller code to text.
	 * @since Version 1.0
	 */
	XmlNodeMap m_engineControllerMapping;
	/**
	 * Mapping of the engine type code to text.
	 * @since Version 1.0
	 */
	XmlNodeMap m_engineTypeMapping;
	/**
	 * Mapping of the emissions type code to text.
	 * @since Version 1.0
	 */
	XmlNodeMap m_emissionsTypeMapping;
	/**
	 * Mapping of the transmission type code to text.
	 * @since Version 1.0
	 */
	XmlNodeMap m_transmissionTypeMapping;
    /**
	 * Module Bus communications type. Set by the tag
	 * CommunicationsBus.
     * * @since 18 Apr 2005
	 */
	string m_moduleCommunicationsBus;

    /**
	 * Module Seuctity Gen 
     * @since 18 Apr 2005
	 */
    INT32 m_securityKeyGen;
};

//*************************************************************************
#endif  // BoschEmissionsModule_h
