//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/VINStamperMgr.h 3     02/01/16 4:15p kstutzma $
//
// FILE DESCRIPTION:
//  The VINStamperMgr handles all data from the VIN Stamper
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/InputServer/Include/VINStamperMgr.h $
// 
//
// 1     2/1/16 4:16p kstutzma
// Created.
//
//*************************************************************************
#ifndef VINStamperMgr_h
#define VINStamperMgr_h
//===========================================================================

#include "InputServerDefs.h"
#include "InputDeviceBase.h"
#include "SerialChannel.h"

class InputServer;

/**
 * Input Mode setup information.
 * @since Version 1.2
 */
const std::string dataLength("DataLength");
const std::string dataMap("DataMap");
const std::string dataTag("DataTag");
const std::string defaultStartIndex("DefaultStartIndex");
const std::string displayTag("DisplayTag");
const std::string invalidKeyPress("InvalidKeyPress");
const std::string publishKeyPress("PublishKeyPress");
const std::string validKeyPad("ValidKeyPad");
/**
 * Data map field attributes
 * @since Version 1.2
 */
const std::string currentFieldIndicator("CurrentFieldIndicator");
const std::string defaultValue("DefaultValue");
const std::string typeField("Type");

/**
 * Valid data field types.
 * @since Version 1.2
 */
const std::string alphanumericType("Alphanumeric");
const std::string characterType("Character");
const std::string integerType("Integer");


/**
 * VINStamperMgr class.
 * This class manages all data received from the VIN Stamper
 *
 * @author Kalee Stutzman
 * @version Version 1.0
 * @see VINStamperMgr
 */
class VINStamperMgr : public InputDeviceBase
{
    friend class VINStamperMgrTest;

public:
    /**
     * Default constructor for the VINStamperMgr.
     *
     * @since Version 1.0
     */
    VINStamperMgr();
    /**
     * Default destructor.
     * @since Version 1.0
     */
    virtual ~VINStamperMgr();
    /**
     * Initializes the VINStamperMgr to talk to the VIN Stamper as well as any data that needs to
     * be setup before we begin processing inputs.
     *
     * @param configNode Configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *configNode);
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
     * Main Run method to control the process
     *
     * @param terminateFlag
     *               Flag used to terminate the system.
     */
    virtual void Run(volatile bool *terminateFlag=NULL);
    /**
     * Store the current state of the VINStamperMgr.
     *
     * @param state  State to set the VINStamperMgr to.
     * @since Version 1.0
     */
    void SetCurrentState(const std::string &state);
    /**
     * Return the current state of the VINStamperMgr.
     *
     * @return Current input state.
     * @since Version 1.0
     */
    inline const std::string& GetCurrentState(void);
    /**
     * Get the length of the input data string.
     *
     * @return Length of the data string.
     * @since Version 1.2
     */
    inline const INT32& GetInputDataLength(void);
   
protected:
    /**
     * Load the additional configuration items needed by the VINStamperMgr.
     * The "standard" items will be handled by the call to
     * InputDeviceBase::Initialize().
     *
     * @param configNode A pointer to the VINStamperMgr config node
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *configNode);
    /**
     * Set the valid length of the input data.
     *
     * @param length Length of the data string.
     * @since Version 1.2
     */
    inline void SetInputDataLength(const INT32 &length);
    /**
     * Set the valid length of the input data.
     *
     * @param length Length of the data string.
     * @since Version 1.2
     */
    inline void SetInputDataLength(const std::string &length);
    
    /**
     * Convert the input data into a usable format and save it to a file
     *
     * @param data input data.
     * @since Version 1.0
     */
    void ConvertDataToFile(char * data, int byteCount);

private:
    /**
     * SerialChannel object to use to get info from the VIN Stamper.
     * @since Version 1.0
     */
    SerialChannel m_stamperComm;
    /**
     * Length of the data string being input by the VIN Stamper.
     * @since Version 1.0
     */
    INT32 m_inputDataLength;
    /**
     * Length of the VIN being input by the VIN Stamper.
     * @since Version 1.0
     */
    INT32 m_vinLength;
    /**
     * Position of the VIN in the data string being input by the VIN Stamper.
     * @since Version 1.0
     */
    INT32 m_vinStartPosition;
    /**
     * indicator if we should shorten file data to the input length
     * @since Version 1.0
     */
    bool m_shortenData;
    /**
     * Flag to tell if we should open the logical port.  This allows us to run
     * the unit tests without the full system.
     */
    bool m_openLogicalPort;
};
#endif  // VINStamperMgr.h
