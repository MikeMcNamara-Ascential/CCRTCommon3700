//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/ISystemCommunication.h 3     10/30/07 3:44p Rwiersem $
//
// FILE DESCRIPTION:
//   This defines the interface for system communication objects.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/ISystemCommunication.h $
// 
// 3     10/30/07 3:44p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 2     10/24/03 11:47a Khykin
// Updated with latest changes from Korea and Flatrock.
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
// 1     6/12/03 4:15p Khykin
// Initial checkin.
//
// 1     4/22/03 9:43a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:40  tbochene
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
// 39    10/25/02 6:37a Khykin
// Updated the code for setting the timeout to allow for a 0 (never)
// timeout.
//
// 38    10/21/02 11:52a Khykin
// Promoted the SetTimeout method to public and updated the method to take
// 0 as a valid timeout.  (0 = no timeout)
//
// 37    10/07/02 6:26a Khykin
// Removing the fixed timeout define due problem was found that was
// causing the unsubscribe.
//
// 36    7/31/02 11:39a Skypig13
// Added fixed timeout macro for testing
//
// 35    5/30/02 4:20p Skypig13
// Made GetType(), GetId(), and SetId() const
//
// 34    4/02/02 11:08p Skypig13
// Moved defines to BepDefs.h
//
// 33    3/19/02 12:01p Kh
// Updating the default buffer size for system communications.
//
// 32    2/05/02 12:41a Skypig13
// Removed compiler warnings
//
// 31    11/12/01 12:17p Skypig13
// Working on FindAndReplaceSpecialCharacter()
//
// 30    11/09/01 12:54a Skypig13
// Added special character conversion (not tested yet)
//
// 29    10/24/01 9:46a Skypig13
// Fixed some comments
//
// 28    10/23/01 7:52p Skypig13
// Added prepend and append string functions and moved DEFAULT_BUFFER_SIZE
// from
// IInterProcessCommunication and changed from 1024 to 1500 (for TCP/IP)
//
// 27    10/02/01 9:31a Skypig13
// Added timeout
//
// 26    7/06/01 3:16p Kh
// Updating the constructor.
//
// 25    7/07/00 9:39a Skypig13
// Dervied from Debuggable
//
// 24    6/22/00 9:10a Skypig13
// Changed diagnostic print in CheckId()
//
// 23    6/21/00 3:42p Skypig13
// Added third parameter to Initialize(XmlNode*, INT32) to
// facilitate receive id in IPC class used to communicate on open
// channel with data waiting
//
// 22    6/20/00 2:50p Skypig13
// Put condition on printf in CheckId()
//
// 21    6/20/00 2:45p Skypig13
// Fixed bug in CheckId()
//
// 20    6/20/00 2:37p Skypig13
// Put diagnostic printf in CheckId() and moved body outside class
// declaration as inline
//
// 19    6/15/01 4:20p Skypig13
// Added CheckId()
//
// 18    6/13/01 7:23p Admin
// Change Boolean to bool
//
// 17    6/13/01 4:46p Skypig13
// Fixed comments
//
// 16    6/13/01 4:44p Skypig13
// Added ProcessReauest() to interface
//
// 15    6/08/00 5:16p Kh
// Updating Initialize to take a communication channel id.
//
// 14    6/05/01 8:55a Skypig13
// Remove Initialize() with descrete parameters
//
// 13    5/31/01 9:35a Skypig13
// Add Initialize with descrete parameters for name, type and debug
//
// 12    5/08/01 2:49p Skypig13
// Defined SetId()
//
// 11    5/07/01 5:48p Skypig13
// m_id now protected
//
// 10    5/07/01 10:40a Skypig13
// Undo last change
//
// 9     5/07/01 9:26a Skypig13
// Change Read() return type to void
//
// 8     5/04/00 4:04p Kh
// Defined the destructor.
//
// 7     5/04/00 11:03a Kh
// Changing Get methods to const type.
//
// 5     5/04/01 10:20a Skypig13
// Make protected class variables private
//
// 3     5/04/01 7:04a Skypig13
// Change prototype for Initialize to match other classes
//
// 2     5/03/00 2:58p Kh
//
// 1     5/02/00 8:13p Kh
// New file
//
//*************************************************************************

#ifndef ISystemCommunication_h
#define ISystemCommunication_h
//==============================================================================

#include <string>
#include <stdio.h>

#include "BepDefs.h"
#include "XmlNode.h"
#include "Debuggable.h"

/**
 * Converts a standard character to a special (non-printable)
 * character (e.g. convert 'n' to '\n')
 * @since Version 1.5
 */
#define TO_SPECIAL(x)   "\\"#x
/**
 * Base class for system communications (IPC, TCP/IP, ....)
 *
 * @author Tim Bochenek
 * @version 1.6
 */
class ISystemCommunication : public Debuggable
{
public:
    /**
     * Constructor (default)
     * @since Version 1.0
     */
    ISystemCommunication();

    /**
     * Destructor
     * @since Version 1.0
     */
    virtual ~ISystemCommunication();

    /**
     * Operation: Initialize
     * Method to initialize the communication object.
     *
     * @param config Configuration node from Xml document
     * @param id     Communication channel Id.  If this is provided, the
     *               component will operate on the specified channel.
     * @param rid    Id of sending process (used for IPC objects when connecting
     *               to a channel with data waiting)
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *config, const INT32 id=-1, const INT32 rid=-1) = 0;
    /**
     * Operation: Read
     * Generic method used to read information.
     *
     * @param message The information to write to the socket.
     * @param id      Id of channel being communicated on
     * @return Id of channel that was read
     *         0 if pulse
     *         -1 if error
     * @since Version 1.1
     */
    virtual INT32 Read(std::string &message, INT32 id=-1) = 0;
    /**
     * Operation: Write
     * Generic method used to write information.
     *
     * @param message The information to write to the socket.
     * @param id      Id of the channel to use for communication.
     */
    virtual void Write(const std::string &message, INT32 id=-1) = 0;
    /**
     * Waits for data to be received on a channel.  The data
     * will not be read but the id will be returned so that
     * it can be processed later
     *
     * @return Id of channel with data to be processed
     * @since Version 1.2
     */
    virtual INT32 ProcessRequest(void) = 0;
    /**
     * Waits for data, pulse, or error to be received on a channel.
     * The data will not be read but the id will be returned so that
     * it can be processed later
     *
     * @return Id of channel with data to be processed
     * @since Version 1.2
     */
    virtual INT32 ProcessRequestSpecial(void);
    /**
     * Closes communication channel object is connected to
     * @since Version 1.6
     */
    virtual void Close(void) const = 0;
    /**
     * Get the type of communicator (Clien/Server)
     *
     * @return Type of communications object
     * @since Version 1.0
     */
    const std::string GetType(void) const;

    /**
     * Get the Id of the device that is communicating with this object
     *
     * @return Id of other device
     * @since Version 1.0
     */
    const INT32 GetId(void) const;

    /**
     * Get the send timeout value
     *
     * @return Value (in nanoseconds) of timeout value for sending data
     * @since Version 1.3
     */
    const UINT64 GetNanoTimeout(void);

    /**
     * Get the send timeout value
     *
     * @return Value (in nanoseconds) of timeout value for sending data
     * @since Version 1.3
     */
    const UINT32 GetTimeout(void);

    /**
     * Get string that is appended to data to be sent
     *
     * @return String to be prepended to message
     * @since Version 1.4
     */
    const std::string GetPrependString(void);

    /**
     * Get string that is prepended to data to be sent
     *
     * @return String to be appended to message
     * @since Version 1.4
     */
    const std::string GetAppendString(void);

    /**
     * Set the time that sending data will fail if not sent and replied to
     *
     * @param timeout Value to be set for timeout (in millliseconds)
     * @since Version 1.3
     */
    void SetTimeout(const UINT32 timeout);

	const string& CommType(const string *commType = NULL);

protected:
    /**
     * Set the communication id for the object
     *
     * @param id     What to communicate to
     * @since Version 1.0
     */
    virtual void SetId(const INT32 &id) const;

    /**
     * Set communication type (Client/Server)
     *
     * @return Id of other device
     * @since Version 1.0
     */
    virtual void SetType(const std::string &type);

    /**
     * Checks if id is valid.  If it is not the id is set to
     * the id that the communicaiton object was initialized with.
     *
     * @param id     Id to be checked (may be modified)
     * @since Version 1.2
     */
    void CheckId(INT32 &id);
    /**
     * Set string that is appended to data to be sent
     *
     * @param data   String to be added to data
     * @return The end string.
     * @since Version 1.1
     */
    void SetPrependString(const std::string &data);
    /**
     * Set string that is prepended to data to be sent
     *
     * @param data   String to be added to data
     * @since Version 1.1
     */
    void SetAppendString(const std::string &data);
    /**
     * Finds special (non-printable) characters represented
     * by a sequence of characters and replaces with special.
     * (e.g.  the sequence '1','2','3','4','5','\','n', '6'
     * would be replaced by '1','2','3','4','5','\n', '6'
     *
     * @param data    Data to be searced for special characters
     * @param special special characters to find
     * @since version 1.5
     */
    void FindAndReplaceSpecialCharacter(std::string &data, char special);
private:
    /**
     * Type of communication device (Client/Server)
     * @since Version 1.0
     */
    std::string m_type;
	string m_commType;
    /**
     * Identifier of the device to communicate with
     * @since Version 1.0
     */
    mutable INT32 m_id;
    /**
     * Timeout value for sending messages
     * @since Version 1.3
     */
    UINT64 m_timeout;
    /**
     * String that is prepended to data to be sent
     * @since 1.4
     */
    std::string m_prepend;

    /**
     * String that is appended to data to be sent
     * @since 1.4
     */
    std::string m_append;
};

#endif // ISystemCommunication_h
