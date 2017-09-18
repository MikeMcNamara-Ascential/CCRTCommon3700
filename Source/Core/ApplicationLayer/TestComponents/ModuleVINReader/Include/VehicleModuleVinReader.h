//*************************************************************************
// FILE:
//    $Header: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/ModuleServer/Include/VehicleModuleVinReader.h 2     4/18/07 1:53p Jsemann $
//
// FILE DESCRIPTION:
//	The VehicleModuleVinReader will read the VIN from a module
//  in the vehicle and send it to the WorkCellController so the build data 
//  can be retrieved.
//
//===========================================================================
// Copyright (c) 2005- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT-cmake/Source/DCX/ApplicationLayer/TestComponents/ModuleServer/Include/VehicleModuleVinReader.h $
// 
// 2     4/18/07 1:53p Jsemann
// 1.  GetCommModulePath() Added
// 2.  SetCommModulePath() Added
// 3.  GetCommModuleFileName() Added
// 4.  SetCommModuleFileName() Added
// 5.  WaitForCommModule() Added
// 6.  FindCommLink() Added
// 7.  m_commModuleFileName added
// 8.  m_commModulePath added
// 
// 1     11/13/06 3:54p Bbarrett
// Add Module VIN Reader.
// 
// 1     8/29/06 12:25p Jsemann
// 
// 1     3/03/06 12:32p Jsemann
// 3/3/06
// 
// 1     10/31/05 1:22p Jsemann
//==============================================================================
#ifndef VehicleModuleVinReader_h
#define VehicleModuleVinReader_h
//------------------------------------------------------------------------------
#include "GenericModuleTemplate.cpp"
#include "KwpCanProtocolFilter.h"
#include "InputDeviceBase.h"
#include "INamedDataBroker.h"

class InputServer;


const string cableConnected("1");
const string cableNotConnected("0");

class VehicleModuleVinReader : public InputDeviceBase
{
public:
	/**
	 * Class Constructor
	 */
	VehicleModuleVinReader();
	/**
	 * Class Destructor
	 */
	virtual ~VehicleModuleVinReader();
	/**
	 * Initializes the manager to talk to the vehicle module as well as any data that needs to
	 * be setup before we begin processing inputs.
	 *
	 * @param document Configuration document.
	 */
	virtual void Initialize(const XmlNode *document);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 1.1
     */
    virtual const std::string Publish(const XmlNode *node);
	/**
	 * Funciton to be executed when thread is run
	 *
	 * @return Status
	 */
	virtual void Run(volatile bool *terminateFlag=NULL);

	/**
	 * Return the name of the VIN tag.
	 *
	 * @return Vin tag.
	 * @since Version 1.0
	*/
    inline const std::string GetVinTag(void)
	{
		return m_vinTag;
	};
	/**
	 * Store the name of the vin tag.
	 *
	 * @param vinTag Vin Tag.
	 * @since Version 1.0
	 */
	inline void SetVinTag(const std::string &vinTag)
	{
		m_vinTag = vinTag;
	};
    /**
	 * Return the name of the Presence tag.
	 *
	 * @return Vin tag.
	 * @since Version 1.0
	*/
    inline const std::string GetPresenceTag(void)
	{
		return m_commPresenceTag;
	};
	/**
	 * Store the name of the Presence tag.
	 *
	 * @param vinTag Vin Tag.
	 * @since Version 1.0
	 */
	inline void SetPresenceTag(const std::string &presenceTag)
	{
		m_commPresenceTag = presenceTag;
	};
    /**
	 * Return the name of the Presence tag.
	 *
	 * @return Vin tag.
	 * @since Version 1.0
	*/
    inline const std::string GetReaderModeTag(void)
	{
		return m_readerModeTag;
	};
	/**
	 * Store the name of the Presence tag.
	 *
	 * @param vinTag Vin Tag.
	 * @since Version 1.0
	 */
	inline void SetReaderModeTag(const std::string &modeTag)
	{
		m_readerModeTag = modeTag;
	};
	/**
	 * Return the path to the Comm module link.
	 *
	 * @return m_commModulePath.
	*/
    inline const std::string GetCommModulePath(void)
	{
		return m_commModulePath;
	};
	/**
	 * Store the path to the Comm module link.
	 *
	 * @param commModulePath.
	 * @since Version 1.0
	 */
	inline void SetCommModulePath(const std::string &commModulePath)
	{
		m_commModulePath = commModulePath;
	};
	/**
	 * Return the filename of the CommModule link.
	 *
	 * @return m_commModulePath.
	*/
    inline const std::string GetCommModuleFileName(void)
	{
		return m_commModuleFileName;
	};
	/**
	 * Store the CommModule filename link.
	 *
	 * @param commModuleFileName.
	 */
	inline void SetCommModuleFileName(const std::string &commModuleFileName)
	{
		m_commModuleFileName = commModuleFileName;
	};

protected:
    /**
     * Load the additional configuration items needed by the SymbolScannerMgr.
     * The "standard" items will be handled by the call to
     * InputDeviceBase::Initialize().
     *
     * @param configNode A pointer to the Pendant config node
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *configNode);
	/**
	 * Read the VIN from the module and use the InputServer to write it to the system.
	 */
	virtual void ReadVIN(void);
    /**
	 * Read the VIN from the module and use the InputServer to write it to the system.
	 */
	virtual void CheckPresence(void);
	/**
	 * Wait for link to communications module to be created before attempting to read vin 
     * from module.
	 */
    virtual void WaitForCommModule(void);
	/**
	 * Find Link created to communications device
	 */
    virtual bool FindCommLink(void);

    BepCondVar<string, false> m_cableConnected;
	/**
	 * List of Vin read messages.
	 * @since Version 1.0
	 */
	XmlNodeMap m_vinReadMessages;

    XmlNodeMap m_presenceMessages;

private:
	/**
	 * Object to be used to read the VIN from the module.
	 */
	IGenericModule *m_vinModule;
	InputServer *m_inputServer;
	/**
	 * Name of the VIN tag.
	 * @since Version 1.0
	 */
	std::string m_vinTag;
    /**
	 * Name of the Presence tag.
	 * @since Version 1.0
	 */
	std::string m_commPresenceTag;
    /**
	 * Name of the ReaderMode tag.
	 * @since Version 1.0
	 */
	std::string m_readerModeTag;
    /**
	 * commModule link name.
	 * @since Version 1.0
	 */
	std::string m_commModuleFileName;
	/**
	 * Path to comm Module.
	 * @since Version 1.0
	 */
	std::string m_commModulePath;
    /**
     * Communication object to interface with the NamedDataBroker.
     */
    INamedDataBroker *m_ndb;

    bool m_vehPresentSignal;
};
//------------------------------------------------------------------------------
#endif //VehicleModuleVinReader_h
