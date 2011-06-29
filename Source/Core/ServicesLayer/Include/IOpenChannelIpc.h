//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/IOpenChannelIpc.h 3     10/30/07 3:43p Rwiersem $
//
// FILE DESCRIPTION:
//   This is the implementation for inter process communication.
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/IOpenChannelIpc.h $
// 
// 3     10/30/07 3:43p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 1     10/07/03 11:37a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:14p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:39  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:06a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 13    9/10/02 6:26p Tbochene
// Updated initializers to match other communication classes
//
// 12    2/19/02 2:01p Skypig13
// Updated constructors and Initialize()
//
// 11    2/19/02 10:42a Skypig13
// Restored default values in Initialize()
//
// 10    2/05/02 12:40a Skypig13
// Restructured to match other communication classes
//
// 9     11/01/01 3:31p Dan.erickson
// Added name to Initialize method prototype for
// IInterProcessCommunication.
//
// 8     10/22/01 5:34p Skypig13
// Added pulse code and pulse value to initialize method
//
// 7     8/28/01 8:24p Skypig13
// Change parameters to not take defaults and add timeout to Write()
//
// 6     8/27/01 5:58p Skypig13
// Update to use size parameter in initialize and parameterized
// constructor
//
// 5     6/22/00 4:52p Skypig13
// Tweaks needed for IBepCommunication
//
// 4     6/21/00 5:48p Skypig13
// Testing functionality of IPC and OpenChannel tests.
// Made some interface changes
//
// 3     6/19/00 12:37p Skypig13
// Changed inheritance scope to public
//
// 2     6/18/00 10:51a Skypig13
// Testing complete for this phase
//
// Class for performing Inter-Process Communication
//
//*************************************************************************
#ifndef IOpenChannelIpc_h
#define IOpenChannelIpc_h

#include "BepDefs.h"
#include "IInterProcessCommunication.h"

class IOpenChannelIpcTest;  // Forward reference to test class

/**
 * Class representing special inter-process communications
 * in the system.  This class is used when a message is
 * received on a channel and the processing of that message
 * is to be done by another object (this class).
 * Objects of this class type will read the senders buffer and
 * reply to the sender.  This will be the usual lifetime of an object of this type.
 *
 * @author Tim Bochenek
 * @version 1.1
 * @since Version 1.0
 */
class IOpenChannelIpc : public IInterProcessCommunication
{
public:
    friend class IOpenChannelIpcTest;
    /**
     * Class Constructor (default)
     * @since Version 1.0
     */
    IOpenChannelIpc(void);
    /**
     * Constructor (initializing)
     *
     * @param id      Channel Id of opened channel
     * @param name    Name of channel opened
     * @param debug   Turn on diagnostic prints (or not)
     * @param rid     Reply id
     * @param code    Pulse code to be set
     * @param value   Pulse value to be set
     * @param size    Size of receive buffer to create
     * @param timeout Timeout waiting for Write() to complete
     * @since Version 1.0
     */
    IOpenChannelIpc(const INT32 id, const std::string name,
                    const bool debug = false,
                    const INT32 rid = -1,
                    const INT32 code = 0, const INT32 value = 0,
                    const INT32 size = DEFAULT_BUFFER_SIZE,
                    const INT32 timeout = DEFAULT_TIMEOUT);
    /**
     * Destructor
     * @since Version 1.0
     */
    virtual ~IOpenChannelIpc(void);
    /**
     * Set up the communication object with a chid
     * from an opened channel and reply id of who to send back to.
     *
     * @param config Teh configuration node to use for initialization.
     * @param id     Chid of open channel
     * @param rid    Id of sending process (used for IPC objects when connecting
     *               to a channel with data waiting)
     */
    virtual void Initialize(const XmlNode *config, const INT32 id=-1, const INT32 rid=-1);
    /**
     * Set up the communication object with a chid
     * from an opened channel and reply id of who to send back to.
     *
     * @param id      Channel Id of opened channel
     * @param name    Name of the channel to talk on.
     * @param debug   Turn on diagnostic prints (or not)
     * @param rid     Reply id
     * @param code    Pulse code to be set
     * @param value   Pulse value to be set
     * @param size    Size of receive buffer to create
     * @param timeout Timeout waiting for Write() to complete
     * @since Version 1.0
     */
    void Initialize(const INT32 id, const std::string name,
                    const bool debug = false,
                    const INT32 rid = -1,
                    const INT32 code = 0,
                    const INT32 value = 0,
                    const INT32 size = DEFAULT_BUFFER_SIZE,
                    const INT32 timeout = DEFAULT_TIMEOUT);
    /**
     * Operation: Read
     * Method used to read information from a process via IPC.
     *
     * @param message The information to write to the socket.
     * @param id      Id of channel to be read
     * @return Number of bytes read from channel
     *         (If error == -1)
     * @since Version 1.0
     */
    virtual INT32 Read(std::string &message, INT32 id=-1);
protected:
    /**
     * Set the the pulse code
     *
     * @param code   Pulse code to set
     * @since Version 1.1
     */
    void SetPulseCode(const INT32 code);
    /**
     * Set the value of the pulse
     *
     * @param value Pulse value to set.
     */
    void SetPulseValue(const INT32 value);
};
#endif  // IOpenChannelIpc_h
