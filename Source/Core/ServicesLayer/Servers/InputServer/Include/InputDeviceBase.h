//*************************************************************************
// FILE DESCRIPTION:
//	The InputDeviceBase is a base class that all individual input devices
//  should inherit from to provide easy to reuse of base functionality.
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
//    $Log: /Working/Source/Core/ServicesLayer/Servers/InputServer/Include/InputDeviceBase.h $
// 
// 1     5/25/05 8:50p Cward
// Changed structure of the Input Server
// 
//*************************************************************************
#ifndef INPUT_DEVICE_BASE_H
#define INPUT_DEVICE_BASE_H
//===========================================================================

#include "BepServer.h"

/**
 * The InputDeviceBase class provides base functionality that all
 * individual input device device classes should inherit from to
 * enable the re-use of common code.
 *
 * @author Greg Pattison
 */
class InputDeviceBase : public BepServer
{
public:
    /**
     * Class constructor.
     */
    InputDeviceBase();
    /**
     * Default class destructor.
     */
    virtual ~InputDeviceBase();
    /**
     * Initialization method that parses the command line
     */
    virtual void Initialize(int argc, char *argv[]);
    /**
     * Initialization method that retrieves the xml document
     * 
     * @param fileName Configuration file which contains the object setup
     *                 information.
     */
    virtual void Initialize(const std::string &fileName);
    /**
     * Initialization method that parses the configuration file
     * and initializes the object.
     *
     * @param document Configuration document.
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Communication obj to use for sending information to the
     * Input Server process
     */
    IBepCommunication *m_inputServerComm;

protected:
    /**
     * Parses command line parameters
     *
     * @param argc      Number of command line arguments in the argv parameter
     * @param argv      Command line option strings
     */
    virtual void ParseCommandLine( int argc, char *argv[]);
    /**
     * Method used to handle a command line argument. Called from
     * ParseCommandLine() for each comamnd line argument encountered.
     *
     * @param option The command line option (i.e. 'f' if -f specified on command line)
     * @param value  Value associated with the command line option being processed.
     *               May be NULL is no argument is associated with the command line
     *               option
     *
     * @return true if the argument was processed, false if not recognized
     */
    virtual bool HandleCommandLineArg(char option, const char *value);

private:

    /**
     * Path for our input device to adopt
     */
    string m_pathName;
    /**
     * Name of this InputServer
     */
    string m_parentName;
    /**
     * Parser for the configuration file. This is protected due to
     * non supported feature in gcc 2.95 when we go to 3.0 this
     * can go private.
     * @since Version 1.0
     */
    XmlParser    m_parser;
};
#endif  // INPUT_DEVICE_BASE_H
