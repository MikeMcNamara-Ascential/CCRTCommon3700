//******************************************************************************
//
//     $Header: /Ccrt/Source/Core/ServicesLayer/Include/INamedDataBrokerBase.h 4     10/30/07 3:41p Rwiersem $
//
// Description:
//  Base class for client and server interface to Named Data Broker.
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/INamedDataBrokerBase.h $
// 
// 4     10/30/07 3:41p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     4/13/06 3:35a Cward
// String parameters are passed by reference to AddNodeWithAttribute() and
// FindValueInTable()
//
// 2     2/15/06 3:51p Bmeinke
// String parameters are passed by reference to AddNodeWithAttribute() and
// FindValueInTable()
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
// 1     6/12/03 4:13p Khykin
// Initial checkin.
//
// 1     4/22/03 9:42a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:26a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:39  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:05a Khykin
// Initial Checkin
//
// 1     1/18/03 8:03a Khykin
// Initial Checkin From DCTC
//
// 30    7/31/02 11:34a Skypig13
// Corrected spelliing in diagnostic
//
// 29    2/12/02 5:17p Dan.erickson
// Updated to make better use of XmlNode Data.
//
// 28    2/05/02 12:38a Skypig13
// Removed compiler warnings
//
// 27    11/15/01 8:28p Skypig13
// Add diagnostics
//
// 26    11/15/01 7:13p Skypig13
// Updated to use new structure
//
// 25    11/09/01 12:58a Skypig13
// Fixed logic for procssing partial hits in the INamedDataBroker
//
// 24    11/01/01 9:48p Skypig13
// Eliminated GetNext()
//
// 23    11/01/01 5:58p Skypig13
// Moved bad data and test data structures to INamedDataBroker.h
//
// 22    10/25/01 12:48p Skypig13
// Changed printout
//
// 21    10/23/01 6:26p Skypig13
// Use BEP_STATUS
//
// 20    10/22/01 5:00p Kh
// Updating to have the alias numbering in the base class.
//
// 19    10/16/01 12:03a Skypig13
// Changed casts to C++ style casts and changed set server status around
// to work better
//
// 18    10/09/01 1:06a Skypig13
// Put extra print in GetServerName()
//
// 17    9/26/01 1:32p Kh
// Changing the GetNext method to public.
//
// 16    8/24/01 1:38p Skypig13
// Changed printouts in GetNext()
//
// 15    8/16/01 3:18p Skypig13
// Latest files tested
//
// 14    8/01/01 2:12p Skypig13
// Latest version
//
// 13    7/27/01 4:26p Skypig13
// Weekly backup
//
// 12    7/24/01 11:29a Skypig13
// Latest changes
//
// 11    7/19/01 3:25p Skypig13
// Latest round of testing that passed
//
// 10    7/17/01 8:58a Skypig13
//
// 9     7/12/01 5:59p Skypig13
// Weekly Backup
//
// 8     7/12/01 4:11p Skypig13
// New base class using IBepCommunication as base class
//
// 7     7/07/00 2:21p Skypig13
// Beer truck prevention (may not compile, get last version if it does
// not)
//
//
//******************************************************************************
#ifndef INamedDataBroker_Base_h
#define INamedDataBroker_Base_h

#include <string>

#include "XmlNode.h"
#include "IBepCommunication.h"

/**
 * Data type definition for state of server pid.
 * Start         = Starting Id lookup process
 * IdNotFound    = Not in table
 * IdPresent     = Id is in table (not necessarily valid)
 * IdVerify      = Request the Named Data Broker to verify Id
 * IdVerified    = Verified Id (not necessarily valid)
 * IdNotValid    = Provider does not exist
 * IdValid       = All good
 * @since Version 1.0
 */
typedef enum
{
    Start=0, Primary, Verify, Secondary, Invalidate, Final, Success
} ServerIdState;

/**
 * Abstract base class for both the Named Data Broker and Interface.
 *
 * @author Tim Bochenek
 * @version 2.4
 * @since Version 1.0
 */
class INamedDataBrokerBase : public IBepCommunication
{
public:
    /**
     * Class constructor (default)
     * @since Version 1.0
     */
    INamedDataBrokerBase(void);

    /**
     * Class destructor
     * @since Version 1.0
     */
    virtual ~INamedDataBrokerBase(void);

protected:
    /**
     * When the tables need to be invalidated this function will be called.
     * The Named Data Broker will send out a message indicating that a data
     * server has changed.  When this happens, the tables are no longer valid.
     * They will be cleared and refreshed as requests are made for data that
     * is not in the table.  This will keep the tables small, clean, and efficient.
     * @since Verision 1.0
     */
    void ClearTables(void);

    /**
     * Verifies that the attribute is valid
     *
     * @param attribute value of attribute
     * @return If valid
     * @since Version 1.0
     */
    virtual bool CheckAttribute(const XmlNode *attribute);

    /**
     * Lookup server alias for tag
     *
     * @param tag    Tag of data requested
     * @return Name of server alias
     * @since Version 2.3
     */
    const std::string  INamedDataBrokerBase::GetServerAlias(const std::string &tag);
    /**
     * Lookup server for requsted tag
     *
     * @param tag    Tag of data requested
     * @return Name of server that provides the data requested
     * @since Version 2.1
     */
    const std::string  GetServerName(const std::string &tag);
    /**
     * Lookup server status requested server.
     *
     * @param tag           Tag of data requested.
     * @param IsServerAlias Flag to tell if a server alias was provided.
     * 
     * @return Status of server that provides the data requested
     *         BEP_STATUS_AVAILABLE   == Server is valid
     *         BEP_STATUS_UNAVAILABLE == Server is invalid
     */
    const std::string GetServerStatus(const std::string &tag, const bool IsServerAlias = false);
    /**
     * Adds an entry to a table
     *
     * @param dataNode    Data to add
     * @param table  Table to add pair to
     * @since Version 2.1
     */
    void AddTableEntry(const XmlNode *dataNode, XmlNodeMap &table);
    /**
     * Adds a node to a table (XmlNodeMap)
     *
     * @param tag       Name of tag (Element)
     * @param tagVal    Valu for tag (Element)
     * @param attribute Name of attribute
     * @param attributeVal
     *                  Value of attribute
     * @param table     Table to add to
     * @since Version 2.3
     */
    void AddNodeWithAttribute(const std::string& tag,
                              const std::string& tagVal,
                              const std::string& attribute,
                              const std::string& attributeVal,
                              XmlNodeMap &table);
    /**
     * Finds a value (non-key) in a table and returns the key value
     *
     * @param value  Value to find
     * @param Table  Table to find it in
     * @return Value of key (empty string if not found)
     * @since Version 2.4
     */
    const std::string FindValueInTable(const std::string& value, XmlNodeMap &Table);
    /**
     * Takes a server name and verifies that the server is available
     *
     * @param tag       Name of server to verify
     *                  (if IsServerName == false, it is a tag else it is Server alias name)
     * @param value     Status to set
     * @param attribute Value of attribute
     * @param IsServerName
     *                  If the tag parameter is the alias of the server
     * @return Status of server
     *         BEP_STATUS_SUCCESS == All groovy
     *         BEP_STATUS_ERROR   == Oh poop!
     * @since Version 3.1
     */
    const INT32 INamedDataBrokerBase::SetServerStatus(const std::string &tag, const std::string &value,
                                                      const std::string &attribute,
                                                      const bool IsServerName = false);
    /**
     * Container holding the Tag/process name pairs
     * @since Version 1.0
     */
    XmlNodeMap   m_data;
    /**
     * Container holding the Process Name/process Id pairs
     * @since Version 1.0
     */
    XmlNodeMap  m_servers;
    /**
     * Mehtod to get the next alias number avaliable.
     *
     * @return The next alias number avaliable.
     */
    const INT32 GetNextAlias(void);

    /**
     * Counter for server alias'
     * @since Version 2.3
     */
    INT32 m_serverNum;
};
#endif // INamedDataBroker_Base_h

