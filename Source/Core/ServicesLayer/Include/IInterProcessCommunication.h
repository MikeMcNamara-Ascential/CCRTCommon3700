//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Include/IInterProcessCommunication.h 3     10/30/07 3:39p Rwiersem $
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Include/IInterProcessCommunication.h $
// 
// 3     10/30/07 3:39p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:26p Cward
//
// 2     10/24/03 11:42a Khykin
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
// 1     6/12/03 4:13p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:38  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:05a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 39    10/07/02 6:24a Khykin
// Adding debug information.
//
// 38    5/30/02 4:18p Skypig13
// Made InvalidateId() const and added Close()
//
// 37    2/05/02 12:36a Skypig13
// Updated to match changes in IBepCommunicaiton
//
// 36    11/01/01 3:31p Dan.erickson
// Added name to Initialize method prototype for
// IInterProcessCommunication.
//
// 35    10/23/01 7:48p Skypig13
// Moved DEFAULT_BUFFER_SIZE to ISystemCommunication (made 1500 from 1024)
//
// 34    10/08/01 9:53p Skypig13
// Fixed prototype for Initialize()
//
// 33    10/02/01 9:30a Skypig13
// Moved timeout to ISystemCommunication
//
// 32    9/07/01 10:29a Kh
// Adding some more defensive programming around system calls.
//
// 31    8/30/01 4:15p Skypig13
// Restored default parameters
//
// 30    8/28/01 8:24p Skypig13
// Change parameters to not take defaults and add timeout to Write()
//
// 29    8/24/01 2:21p Kh
// Adding Bpos include.
//
// 28    8/21/01 3:05p Skypig13
// Added InvalidateId()
//
// 27    8/17/01 4:42p Skypig13
// Put retries around blocking functions if they are interrupted
// (MsgReceive() and MsgSendvs())
//
// 26    7/20/01 9:23a Skypig13
// Changed m_pulse scope to protected
//
// 25    7/18/01 3:29p Skypig13
// Added dosconnect notification
//
// 24    7/17/01 7:01p Skypig13
// Add pulse handling
//
// 23    7/17/01 8:56a Skypig13
//
// 22    7/11/01 5:54p Skypig13
// Change from multi-part response to fixed and made minimum buffer size =
// 1024 so that object can handle large messages in response.  Parameter
// for buffer size will allow for configurable message sizes.
//
// 21    7/11/00 4:45p Skypig13
// Changed constructor to take default parameter for rid and made
// defaults for bool parameters take true/false instead of TRUE/FALSE
//
// 20    6/22/00 4:52p Skypig13
// Tweaks needed for IBepCommunication
//
// 19    6/22/00 10:39a Skypig13
// Changes to intitialize(int, int, bool) and added reply id to
// class and change functionality to use accessor methods
//
// 18    6/21/00 5:48p Skypig13
// Testing functionality of IPC and OpenChannel tests.
// Made some interface changes
//
// 17    6/15/01 4:00p Skypig13
// Created helper functions to make derived IOpenChannelIpc class
// easier to impliment
//
// 16    6/13/01 7:23p Admin
// Change Boolean to bool
//
// 15    6/13/01 7:08p Skypig13
// Added interface for ProcessRequest()
//
// 14    6/13/01 4:09p Skypig13
// Added comments to the constructor with id and debug parameters
//
// 13    6/13/01 3:24p Skypig13
// Add functionality to connect to an open channel that has
// already been unblocked.  Restructured Read() and added
// ReadSendersBuffer().
//
// 12    6/11/01 4:31p Skypig13
// Added functionality  connecting to a channel already open
// by another IPC object
//
// 11    6/05/01 8:55a Skypig13
// Move Initialize() with descrete parameters from base class
//
// 10    5/31/01 9:36a Skypig13
// Change cout to printf for thread-safe operation and added Inititalize
// with descrete parameters for type, name and debug
//
// 9     5/24/01 4:17p Skypig13
// Safe backup
//
// 8     5/21/01 2:43p Skypig13
// Put BposRegisterTaskName() inside class.
//
// 7     5/17/01 11:16a Skypig13
// Fixed so sending to any channel Id works
//
// 6     5/14/01 1:05p Skypig13
// Add multi-part messages done
//
// 3     5/09/01 6:49a Skypig13
// Initial working version
//
// 2     5/04/01 12:50p Skypig13
// Weekly backup
//
// 1     5/04/01 7:12a Skypig13
// Class for performing Inter-Process Communication
//
//*************************************************************************
#ifndef IInterProcessCommunication_h
#define IInterProcessCommunication_h

#include <sys/dispatch.h>
#include <sys/neutrino.h>

#include "BepDefs.h"
#include "Bepos.h"
#include "ISystemCommunication.h"

/**
 * Data type definition of pulse structure
 * @since Version 1.0
 */
typedef struct _pulse Pulse;

/**
 * Data type definition for generic header type
 * @since Version 1.0
 */
typedef union
{
    UINT16  type;
    struct  _io_read    io_read;
    struct  _io_write   io_write;
} header_t;

class IpcTest;  // Forward reference to test class

/**
 * Class representing inter-process communications
 * in the system.  The class can handle vairable-length
 * messages and be initialized in many different
 * configurations.
 *
 * @author Tim Bochenek
 * @version 2.6
 * @since Version 1.0
 */
class IInterProcessCommunication : public ISystemCommunication
{
public:
    friend class IpcTest;
    /**
     * Class Constructor (default)
     * @since Version 1.0
     */
    IInterProcessCommunication(void);
    /**
     * Class Constructor (passing initializing Xml node)
     * @param config Configuration node from Xml document
     * @since Version 1.0
     */
    IInterProcessCommunication(const XmlNode *config);
    /**
     * Destructor
     * @since Version 1.0
     */
    virtual ~IInterProcessCommunication(void);
    /**
     * Constructor (passing in open channel id and debug)
     *
     * @param id     Channel Id of opened channel
     * @param name   Name of the channel to talk on.
     * @param size   Size of buffer
     * @param timeout Time to wait before calls to Write() fail
     * @param rid    Reply id of process to reply to
     * @param debug  Turn on diagnostic prints (or not)
     * @since Version 1.1
     */
    IInterProcessCommunication(const INT32 id, const std::string name, const bool debug = false,
                               const INT32 rid = -1, const INT32 size = DEFAULT_BUFFER_SIZE,
                               const INT32 timeout = DEFAULT_TIMEOUT);
    /**
     * Class Constructor (passing initializing data descretely)
     *
     * @param name    Name to register with
     * @param type    Type of communicaiton object (Client/Server)
     * @param debug   Turn on diagnostic prints (or not)
     * @param size    Size of buffer
     * @param timeout Time to wait before calls to Write() fail
     * @since Version 1.0
     */
    IInterProcessCommunication(const std::string name, const std::string type,
                               const bool debug = false,
                               const INT32 size = DEFAULT_BUFFER_SIZE,
                               const INT32 timeout = DEFAULT_TIMEOUT);
    /**
     * Operation: Initialize
     *
     * @param name   Name to register with
     * @param type   Type of communicaiton object (Client/Server)
     * @param size   Size of buffer to create
     * @param timeout Time to wait before calls to Write() fail
     * @param debug  Turn on diagnostic prints (or not)
     * @since Version 1.0
     */
    void Initialize(const std::string &name, const std::string &type,
                    const bool debug = false,
                    const INT32 size = DEFAULT_BUFFER_SIZE,
                    const INT32 timeout = DEFAULT_TIMEOUT);
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
    virtual void Initialize(const XmlNode *config, const INT32 id = -1, const INT32 rid = -1);
    /**
     * Set up the communication object with a chid
     * from an opened channel.
     *
     * @param id      Chid of open channel
     * @param name    Name of the channel we are talking over.
     * @param debug   Turn on diagnostic prints (or not)
     * @param rid     Id of sending process (used for IPC objects when connecting
     *                to a channel with data waiting)
     * @param size    Size of receiving buffer
     * @param timeout Time to wait for sending data
     * @since Version 1.1
     */
    virtual void Initialize(const INT32 id, const std::string name, const bool debug = false,
                            const INT32 rid = -1, const INT32 size = DEFAULT_BUFFER_SIZE,
                            const INT32 timeout = DEFAULT_TIMEOUT);
    /**
     * Operation: Read
     * Method used to read information from a process via IPC.
     *
     * @param message The information to write to the socket.
     * @param id      The sender id.
     */
    virtual INT32 Read(std::string &message, INT32 id = -1);
    /**
     * Operation: Write
     * Method used to write data via IPC to another process.
     *
     * @param message The information to write to the socket.
     * @param id      The sender id.
     */
    virtual void Write(const std::string &message, INT32 id = -1);
    /**
     * Clears the receive buffer
     * @since Version 1.0
     */
    virtual void ClearPublicBuffer(void);

    /**
     * Sends a pulse to a channel
     *
     * @param code     Pulse code (8-bit value)
     *                 Should be > 0
     * @param value    Value of pulse
     * @param id       Where to send pulse
     * @param priority Priority of pulse
     * @since Version 2.1
     */
    const INT32 SendPulse(const INT32 code, const INT32 value,
                          INT32 priority = 100, INT32 id = -1);
    /**
     * Waits for data to be received on a channel.  The data
     * will not be read but the id will be returned so that
     * it can be processed later
     *
     * @return Id of channel with data to be processed
     * @since Version 1.2
     */
    virtual INT32 ProcessRequest(void);
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
     * Get the information about a pulse
     *
     * @param pulse  structure to fill
     * 
     * @return If pulse was handled
     */
    INT32 GetPulse(Pulse &pulse);
    /**
     * Retrieves the pulse code from pulse that was received
     *
     * @return Pulse code
     * @since Version 2.2
     */
    const INT32 GetPulseCode(void);

    /**
     * Retrieves the pulse value from pulse that was received
     *
     * @return Pulse value
     * @since Version 2.2
     */
    const INT32 GetPulseValue(void);
    /**
     * Checks the pulse to determine if pulse is a Bep defined pulse
     *
     * @return If pulse was a Bep Pulse
     * @since Version 2.2
     */
    const bool IsBepPulse(void);

    /**
     * Gets the length of the receive buffer
     *
     * @return Lentgh (in bytes) of buffer
     * @since Version 1.0
     */
    UINT32 GetRxLength(void);

    /**
     * Get registered name of process invloved in IPC
     *
     * @return Name of process
     * @since Version 1.0
     */
    const std::string GetName(void);

    /**
     * Read the senders buffer without blocking.
     * The sender has sent a message and the receiver
     * has been unblocked.  The buffer is only to be read
     *
     * @param message String to populate with data
     * @param offset  Number of bytes to offset in the senders buffer when reading
     * @param id      Id of sender
     * @return Id of channel read
     * @since Version 1.1
     */
    INT32 ReadSendersBuffer(std::string &message, const INT32 offset, INT32 id);
    /**
     * Reply to a senders message
     *
     * @param message Data to send
     * @param id      Who to send to
     * @since Version 1.0
     */
    virtual void Reply(const std::string &message, INT32 id =-1);
    /**
     * Gets the id of the channel that must be replied to
     *
     * @return Id to reply to
     * @since Version 1.3
     */
    const INT32 GetReplyId(void);

    /**
     * Indicates whether or not a the object on other end disconnected
     *
     * @return If a terminate message was received
     * @since Version 2.3
     */
    const bool WasDisconnected(void);

    /**
     * Indicates whether or not a the object on other end of channel 
     * disconnected.
     *
     * @param status Status to set terminate variable.
     */
    void SetDisconnected(const bool status);

    /**
     * Closes communication channel object is connected to
     * @since Version 2.6
     */
    virtual void Close(void) const;

protected:
    /**
     * Sets/Resets the length of the receive buffer
     *
     * @param len    Length of buffer
     * @since Version 1.0
     */
    void SetRxLength(UINT32 len);
    /**
     * Set the name of the process involved in IPC
     *
     * @param name   Name of process
     * @param Setup  Parameter to either set up channel or just use it
     * @since Version 1.0
     */
    void SetName(const std::string &name, bool Setup = true);
    /**
     * Set the communication id for the object
     *
     * @param id     What to communicate to
     * @since Version 1.0
     */
    virtual void SetId(const INT32 &id);
    /**
     * Set the Id of the device that is communicating with this object
     *
     * @return Id of other device
     * @since Version 1.0
     */
    virtual void SetType(const std::string &type);
    /**
     * Set the channel Id of a process with registered name
     *
     * @param name   Registered name of process
     * @since Version 1.0
     */
    void SetId(const std::string &name);
    /**
     * Set up the communication channel
     *
     * @param chid   Id for a server to connect to
     * @since Version 1.0
     */
    void SetupChannel(const std::string &chid = "");
    /**
     * Set the name Id (for a server only)
     *
     * @param id     Name Id for server
     * @since Version 1.0
     */
    void SetNameId(INT32 id);
    /**
     * Get the name Id (for server only)
     *
     * @return Name Id
     * @since Version 1.0
     */
    INT32 GetNameId(void);
    /**
     * Set if the Read() method should read a channel or the internal buffer
     *
     * @param read   Whether to read the chnannel
     * @since Version 1.0
     */
    void SetReadChannel(bool read);

    /**
     * Return whether the channel should be read
     *
     * @return If the channel should be read
     * @since Version 1.0
     */
    bool ShouldReadChannel(void);

    /**
     * Clears the receive buffer
     * @since Version 1.0
     */
    void ClearBuffer(void);
    /**
     * Returns the contents of the receive buffer
     *
     * @return Data in the receive buffer
     * @since Version 1.0
     */
    const std::string GetBuffer(void);

    /**
     * Set the data in the communication receive buffer
     *
     * @param data   data to assign the buffer
     * @since Version 1.0
     */
    void SetBuffer(const std::string &data);
    /**
     * Handles pulses.
     * This is a default implementation that only handles
     * system pulses.  Derived classes should overrride this,
     * call this base class method first and then process
     * special pulses if they are not handled by base class.
     *
     * @param pulse  Pulse structure
     * @return Was pulse handled
     */
    bool HandlePulse(Pulse &pulse);
    /**
     * Handles IPC errors
     *
     * @param source Source of error
     * @exception BepException
     *                   This is the exception that will be thrown.
     *                   (This fuction always thrws an exception).
     * @since Version 1.0
     */
    void HandleError(const std::string &source) throw (BepException);
    /**
     * Get the acknowledge message to indicate the sender is ready for reply
     *
     * @return If the write operation should reply to a channel
     * @since Version 1.0
     */
    const bool ShouldReply(void);

    /**
     * Set the variable indicating whether or not the next write should be a reply.
     *
     * @param reply  Data to send
     * @since Version 1.0
     */
    void SetReply(const bool reply);

    /**
     * Returns the status of pulse handling
     *
     * @return Id pulse was handled
     * @since Version 1.0
     */
    const bool WasPulseHandled(void);

    /**
     * Sets the status of pulse handling
     *
     * @param stat If pulse handled status
     * @since Version 1.0
     */
    void SetPulseHandled(const bool stat);
    
    /**
     * Sets the id of the channel that must be replied to
     *
     * @param rid    Id to reply to
     * @since Version 1.3
     */
    void SetReplyId(const INT32 rid);

    /**
     * Invalidates channel id
     * @since Version 2.4
     */
    void InvalidateId(void) const;

protected:
    /**
     * Structure to handle pulse
     * @since Version 2.2
     */
    Pulse m_pulse;
private:
    /**
     * Receive buffer
     * @since Version 1.0
     */
    char *m_rxBuffer;
    /**
     * Length of receive buffer
     * @since Version 1.0
     */
    UINT32 m_rxLength;
    /**
     * Registered name of process (Server sets this, client gets chid)
     * @since Version 1.0
     */
    std::string m_name;
    /**
     * Flag to read the channel
     * (If true, the Read method will wait on the channel
     * for a message, else it will read the receive buffer)
     * Write will se this value when a process replies to a Write()
     * @since Version 1.0
     */
    bool m_readChannel;
    /**
     * Variable indicating that a sender needs to be replied to
     * @since Version 1.0
     */
    bool m_reply;
    /**
     * Name attach structure used to manage registered name
     * @since Version 1.0
     */
    mutable name_attach_t* m_nameStruct;
    /**
     * If a pulse was received and handled
     * @since Version 1.0
     */
    bool m_pulseHandled;
    /**
     * Header of information that describes message
     * @since Version 1.0
     */
    header_t m_header;
    /**
     * Id of sender that needs to be replied to
     * @since Version 1.3
     */
    INT32 m_replyId;
    /**
     * Status of connection to other end of channel
     * @since Version 2.3
     */
    bool m_disconnect;
};
#endif  // IInterProcessCommunication_h

