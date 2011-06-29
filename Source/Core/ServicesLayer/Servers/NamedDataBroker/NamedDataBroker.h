//****************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Servers/NamedDataBroker/NamedDataBroker.h 3     10/30/07 4:19p Rwiersem $
//
// DESCRIPTION:
//    Header file for the Name Data Broker methods.
//
//============================================================================
//
//    Copyright (c) 1999 Burke E. Porter Machinery
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/NamedDataBroker/NamedDataBroker.h $
// 
// 3     10/30/07 4:19p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 2     4/26/06 7:01p Cward
// Made changes to support reloading config files without rebooting
//
// 1     4/12/06 1:05p Rwiersem
// Added LoadAdditionalConfigurationItems() and HandlePulse().  These
// methods are used to reload the configuration without restarting the
// process.
//
// 1     12/21/03 6:42p Cward
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 1     7/08/03 12:59p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:25p Khykin
// Initial checkin
//
// 1     6/12/03 4:18p Khykin
// Initial checkin.
//
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:44  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:08a Khykin
// Initial Checkin
//
// 1     1/18/03 8:05a Khykin
// Initial Checkin From DCTC
//
// 18    2/25/02 8:48p Skypig13
// Updated Initialize()
//
// 17    2/19/02 4:38p Skypig13
// Changes to match BepServer updates
//
// 16    2/05/02 12:45a Skypig13
// Updated to remove compiler warnings
//
// 15    11/21/01 8:37a Skypig13
// Removed Terminate()
//
// 14    11/15/01 7:14p Skypig13
// Updated to use new structure inherited from BepServer
//
// 13    10/17/01 9:56a Skypig13
// Changed prototype of Read() to include data reate
//
// 12    10/12/01 10:39a Skypig13
// Make BepComponent inheritance protected
//
// 11    8/01/01 2:12p Skypig13
// Latest version
//
// 10    7/27/01 4:26p Skypig13
// Weekly backup
//
// 9     7/24/01 11:29a Skypig13
// Latest changes
//
// 8     7/19/01 3:25p Skypig13
// Latest round of testing that passed
//
// 7     7/17/01 8:58a Skypig13
//
// 6     7/12/01 4:09p Skypig13
// Compilable version (not operable) with IBepCommunicaiton as base
// class of INamedDataBrokerBase
//
// 5     7/07/00 12:10a Skypig13
// First compilable version derived from IBepCommunicaiton
//
// 4     6/13/01 7:08p Skypig13
// Added visitor pattern interface
//
// 3     6/05/01 8:58a Skypig13
// Latest revision
//
// 2     5/02/01 9:13p Skypig13
// Updating
//
// 1     4/10/01 10:46a Admin
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 1    01/03/01 8:00a Kevin.Hykin
// Initial version.
//
//****************************************************************************
#ifndef NamedDataBroker_h
#define NamedDataBroker_h
//==============================================================================
#include "BepServer.h"

/**
 * Named data broker provides information about which process
 * provides requested data to the process requesting that data.
 *
 * The Named Data Broker maintains 2 tables.  One table contains
 * the Tag and the name of the process providing that data.  The
 * other table contains the process name and availability status.
 *
 * @author Tim Bochenek
 * @version 4.0
 * @since Version 1.0
 */
class NamedDataBroker : public BepServer
{
public:
    friend class NamedDataBrokerTest;
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    NamedDataBroker(void);
    /**
     * Class destructor
     * @since Version 1.0
     */
    virtual ~NamedDataBroker(void);
    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    virtual void Initialize(const XmlNode *document);
    /**
     * Process the reload configuration pulse.  If any other pulses
     * are received, send them on to BepComponent.
     *
     * @param code   Pulse code
     * @param value  Pulse value
     * @return If pulse is handled.
     *         Handled     == BEP_STATUS_SUCCESS
     *         Not Handled == BEP_STATUS_FAILURE (default if not overriden)
     */
    virtual const INT32 HandlePulse(const INT32 code, const INT32 value);
    /**
     * Request to read a data tag
     *
     * @param node    Data to be read
     * @param rate   Rate the data should be provided (times/sec, 0 = one time read)
     * @return Value of tag
     * @since Version 2.0
     */
    virtual const std::string Read(const XmlNode *node, const INT32 rate = 0);
    /**
     * Request to write a data tag
     *
     * @param node    Data to be written
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Write(const XmlNode *node);
    /**
     * Request to Perform an action
     *
     * @param node    Action to be performed
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Command(const XmlNode *node);
    /**
     * Request to be inserted into the list to be notified of a change in a data 
     * tag.
     *
     * @param tag      Tag for who is requesting the subscription information.
     * @param Process  The process name of who wants the data requested.
     * 
     * @return Status of operation
     *         BEP_SUCCESS_RESPONSE == Subscriber successfully added
     *         BEP_FAILURE_RESPONSE == Error adding subscriber
     */
    virtual const std::string Subscribe(const std::string &tag, const std::string &Process);
    /**
     * Request to be removed from the list to be notified
     * of a change in a data tag
     *
     * @param tag     Tag for who is requesting to be unsubscribed.
     * @param Process The process name of who wanted the data
     * @return Status of operation
     *         BEP_SUCCESS_RESPONSE == Subscriber successfully removed
     *         BEP_FAILURE_RESPONSE == Error removing subscriber
     * @since Version 2.0
     */
    virtual const std::string Unsubscribe(const std::string &tag, const std::string &Process);
    /**
     * Handle the subscribed for data by the process.
     *
     * @param tag    Tag that was subscribed for
     * @param value  New data value of the tag
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 2.1
     */
    virtual const std::string Publish(const std::string &tag, const std::string &value);
    /**
     * Method used to abort the current processing based
     * on the needs of the component.
     */
    virtual void Abort(void);
    /**
     * Method used to set all of the objects in the component
     * to their original states.  This would includes all
     * member variables such as buffers, lists, and stored
     * data.
     */
    virtual void Reset(void);
protected:
    /**
     * Load the additional configuration items needed by the named data broker.
     * The "standard" items will be handled by the call to
     * BepServer::Initialize().
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Load lookup table for tag and server name
     *
     * @param servers XmlNodeMap containing server/registered name pairs.
     */
    void LoadServerStatusTable(const XmlNodeMap &servers);
    /**
     * Takes a server name and verifies that the server is available
     *
     * @param tag    Data tag to lookup server for
     * @param IsServerName
     *               Is first parameter server name (if false it is tag)
     * @return Status of server
     *         BEP_AVAILABLE_RESPONSE == All groovy
     *         BEP_UNAVAILABLE_RESPONSE == No soup for you!
     * @since Version 3.0
     */
    const std::string VerifyServer(const std::string &tag, const bool IsServerName = false);
    /**
     * Clears the data structures
     * @since Version 1.0
     */
    virtual void ClearData(void)
    {
        BepServer::ClearData();
        m_servers.clear(true);
    }
private:
    /**
     * Container holding the Process Name/process Id pairs
     * @since Version 1.0
     */
    XmlNodeMap m_servers;
};

#endif // NamedDataBroker_h
