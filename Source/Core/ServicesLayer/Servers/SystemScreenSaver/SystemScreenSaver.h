//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaver.h 1     5/01/07 1:10p Rwiersem $
//
//===========================================================================
// Copyright (c) 2005-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaver.h $
// 
// 1     5/01/07 1:10p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//
//*************************************************************************
#ifndef SystemScreenSaver_h
#define SystemScreenSaver_h
//==============================================================================

#include "BepServer.h"
#include "SerialChannel.h"

/**
 * Class for controlling monitors that support serial communication.  This 
 * class alters the screen appearance to prevent monitor burn in.
 */ 
class SystemScreenSaver : public BepServer
{
    friend class SystemScreenSaverTest;

public:

    /** SystemScreenSaver default constructor. */
    SystemScreenSaver();

    /** SystemScreenSaver destructor. */
    virtual ~SystemScreenSaver();

    /**
     * <b>Description:</b>
     * Method for initialization that takes an XmlNode pointing to the parsed 
     * configuration file as a parameter.
     * 
     * @param document A pointer to the parsed configuration file.
     */
    virtual void Initialize(const XmlNode *document);

    /**
     * <b>Description:</b>
     * Method for intialization that takes the configuration file name as a
     * parameter.
     * 
     * @param fileName The configuration file name.
     */ 
    virtual void Initialize(const std::string &fileName);

    /**
     * <b>Description:</b>
     * Handle the subscribe data needed by the process.
     *
     * @param node The subscription tag and data.
     * 
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string Publish(const XmlNode *node);

    /**
     * <b>Description:</b>
     * Handle the subscribe data needed by the process.
     * 
     * @param tag   The subscription data tag.
     * @param value The subscription data value.
     * 
     * @return BEP_SUCCESS_RESPONSE if the data tag was succesfully written.
     *         BEP_UNAVAILABLE_RESPONSE if the data tag was not found.
     *         BEP_ERROR_RESPONSE if the tag data mutex could not be locked.
     */
    virtual const std::string Publish(const std::string &tag, const std::string &value);

    /**
     * <b>Description:</b>
     * Overridden HandlePulse mehtod used to handle the TIMER_PULSE pulse code.
     * 
     * @param code  The pulse code.
     * @param value The pulse value.
     * 
     * @return BEP_STATUS_SUCCESS if the pulse was successfully handled.
     *         BEP_STATUS_FAILURE if the pulse was not handled.
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);

protected:

    /**
     * <b>Description:</b>
     * Go through m_videoControls and set up the comm channels as required.
     */
    virtual void InitControls(void);

    /**
     * <b>Description:</b>
     * Go through the setup portion of the document, and pull out the data we 
     * need.
     * 
     * @param document A pointer to the resident copy of the configuration file.
     */
    virtual void ReadParameters(const XmlNode *document); 

    /**
     * <b>Description:</b>
     * Translate the action configured in the subscribe tag node corresponding 
     * to the received pubish data into a video control type.
     * 
     * @param thisAction The action associated with the data received by 
     *                   Publish().
     * 
     * @return The control mechanism to be activated.
     */
    virtual std::string DoAction(std::string thisAction);

    /**
     * <b>Description:</b>
     * Process the control requested, searching for a method of accomplishing 
     * it.
     * 
     * @param thisControl What we are attempting to do.
     */
    virtual void DoControl(std::string thisControl);

    /**
     * <b>Description:</b>
     * Send out the serial strings needed to accomplish the control requested.
     * 
     * @param controlNode The complete control node that we are processing.
     * @param doAction    The name of the action to perform.
     */
    virtual void DoSerialControl(const XmlNode *controlNode, std::string doAction);

    /**
     * <b>Description:</b>
     * Do an IPC to do the control function.  
     * 
     * NOTE:
     * This is currently not implemented.
     * 
     * @param controlNode The complete control node that we are using.
     */
    virtual void DoIpcControl(const XmlNode *controlNode);

    /**
     * <b>Description:</b>
     * Overridden Register() method to start the timer.
     * 
     * @return BEP_SUCCESS_RESPONSE if the registration was successful.
     *         BEP_FAILURE_RESPONSE if the registration was not successful.
     */
    virtual const std::string Register(void);

private:

    /**
     * <b>Description:</b>
     * Determine if the current screen should be changed.
     */ 
    void TimingFunction(void); 

    /**
     * A map of all the received publish tags, with the action that is 
     * associated with the value received.
     */
    XmlNodeMap m_triggers;

    /**
     * A map of translations of what Control we are going to do, depending on 
     * the action required.
     */
    XmlNodeMap m_actions;

    /**
     * A map of the various methods of accomplishing a control, based on the 
     * type of communications that we are using.
     */
    XmlNodeMap m_videoControl;

    /** Serial channel used for communications. */
    SerialChannel m_videoControlComm;

    /** Timer that tells if the video should be updated. */
    BepTimer m_updateVideoTimer;

    /** The number of milliseconds used for the update video timer. */
    UINT64 m_videoTimerTime;

    /** The amount of time the screen has been in the current mode. */
    INT64 m_modeAccumulator;

    /** The current screen mode ("Normal" or "Inverted"). */
    std::string m_currentMode;

    /** The current waiting time. */
    INT64 m_waitCount;

    /** The minimum time between timer activated mode switching. */
    INT64 m_switchDelay;

    /** 
     * The amount of time to wait before changing the current screen mode if
     * there are no other changes.
     */ 
    INT64 m_okDelta;
};

#endif // SystemScreenSaver_h
