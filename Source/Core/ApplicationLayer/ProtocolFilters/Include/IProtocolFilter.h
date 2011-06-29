//******************************************************************************
// Description:
//   ProtocolFilter Interface for communicating with vehicle modules.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ProtocolFilters/Include/IProtocolFilter.h $
// 
// 4     10/25/06 3:26p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added OpenCom() and CloseCom()
// 
// 3     6/09/04 10:51p Cward
// Updated header comment block to new standard
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 1     7/08/03 1:43p Derickso
// Restructured for new directory structure.
//    Revision 1.3  2003/05/30 21:20:49  tenharmsel
//    updated to get ford abs test component built
//
//    Revision 1.2  2003/05/08 18:48:39  tenharmsel
//    Getting Application Layer building
//
//    Revision 1.1  2003/05/08 17:42:11  tenharmsel
//    Moved from the Include Directory to here
//
//    Revision 1.3  2003/05/07 19:29:08  swieton
//    Fixed a handful of syntax errors.
//
//    Revision 1.2  2003/05/07 18:55:39  swieton
//    Added Makefile.am
//
//    Revision 1.1.1.1  2003/05/07 15:13:34  tenharmsel
//    Bringing in the ProtocolFilter includes
//
//    Revision 1.1.1.1  2003/04/30 20:10:36  tenharmsel
//    Source code for CCRT Server
//
//
// 1     4/22/03 10:26a Khykin
// Initial Working Checkin.
//
// 1     4/11/03 7:33a Khykin
// Initial checkin.
//    Revision 1.1  2003/02/23 16:51:51  tbochene
//    New protocol filter stuff
//
//
// 2     2/10/03 11:59a Derickso
// Corrected compile errors.
//
// 1     2/03/03 4:19p Derickso
// Created.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef IProtocolFilter_h
#define IProtocolFilter_h
//==============================================================================
#include <vector>
#include "SerialDefs.h"
#include "BepDefs.h"

/**
 * Vector to hold serial arguments to use for replacing wild card characters.
 * @since Version 1.0
 */
typedef vector<UINT8>  SerialArgs_t;

class XmlNode;

/**
 * IProtocolFilter.
 * Abstract ProtocolFilter Interface class.  This class povides the interface methods
 * needed for communicating with a vehicle module.
 *
 * @author Dan Erickson
 * @version Version 1.0
 * @since February 03, 2003
 */
class IProtocolFilter
{
public:
    /**
     * Class constructor.  Nothing special to do here.
     * @since Version 1.0
     */
    IProtocolFilter()
    {
    };
    /**
     * Class destructor.  Nothing special to do here.
     * @since Version 1.0
     */
    virtual ~IProtocolFilter()
    {
    };

    /**
     * The initialize method for the protocol filter
     */
    virtual bool Initialize(const XmlNode *config) = 0;
                    
    /**
     * Opens the serial connection
     * 
     * @return true if the serial connection was opened successfully
     */
    virtual bool OpenCom() = 0;

    /**
     * Closes the serial connection
     */
    virtual void CloseCom() = 0;
                    
    /**
     * Send a message to a vehicle module.
     *
     * @param message Message to be sent.
     * @return Status of the send.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message) = 0;
    /**
     * Send a message to vehicle module.
     *
     * @param messageTag Name of message to be retrieved and sent.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag) = 0;
    /**
     * Send a message to a vehicle module.
     *
     * @param messageTag Name of the message to be sent.
     * @param args       Extra data to be added to the message.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag, SerialArgs_t &args) = 0;
    /**
     * Read a module response to the message with the given tag.
     *
     * @param messageTag Tag of the message response to read.
     * @param reply      Reply from the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetResponse(const std::string &messageTag, SerialString_t &reply) = 0;
    /**
     * Read a module response.
     *
     * @param reply      Reply from the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetResponse(SerialString_t &reply) = 0;
    /**
     * Send a message to the module and read the response.
     *
     * @param messageTag Tag of message to send.
     * @param reply      Reply from the module.
     * @param args       Optional data to be written to the module.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag, SerialString_t &reply,
                                                SerialArgs_t *args = NULL) = 0;
    /**
     * Subscribe for a message with the specified filter.
     *
     * @param messageTag Tag of the message to subscribe for.
     * @param event      event to wait on.
     * @return Status of the operation.
     * @since Version 1.0
     */
    virtual const BEP_STATUS_TYPE FilterSubscribe(const std::string &messageTag, struct sigevent *event) = 0;
    /**
     * Extract the data from the response.
     *
     * @param moduleResponse
     *               Response from the module.
     * @return The data that was included in the response.
     * @since Version 1.0
     */
    virtual const SerialString_t ExtractModuleData(SerialString_t &moduleResponse) = 0;

    /**
     * Request to lock the port for exclusive use. The flags parameter
     * allows you to specify a blocking mode for this operation: If O_NONBLOCK
     * is specified and the port is already locked by another process, an error
     * is returned. If O_NONBLOCK is NOT specified, this operation will block
     * until the port can be locked.
     *
     * @param flags  Optional flags argument.
     * @return true if the port was successfully locked, false if an error
     *         occurred (use errno for more detailed error information)
     */
    virtual bool LockPort(int flags=0) = 0;

    /**
     * Request to unlock a port. A successful call to LockPort() must have
     * been issued for this method to succeed.
     *
     * @return true if port was unlocked, false if an error occurred (use errno
     *         for more detailed error information)
     */
    virtual bool UnlockPort() = 0;
};

#endif  //IProtocolFilter_h
