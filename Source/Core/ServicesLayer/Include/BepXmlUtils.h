//******************************************************************************
//
// FILE:    BepXmlUtils
//     $Archive:: /Ccrt/Source/Core/ServicesLayer/Include/BepXmlUtils.h       $
//     $Author: Rwiersem $
//     $Date: 10/26/06 9:12a $
//
// Description:
//  Utility class definitions for the BEP XML Parser/Processor
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/BepXmlUtils.h $
// 
// 2     10/26/06 9:12a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed DLL_CLASS from the class names.
// 
// 1     12/21/03 6:26p Cward
// 
// 1     10/07/03 11:37a Khykin
// Initial checkin
// 
// 2     8/06/03 3:21p Khykin
// Updated with latest changes from Flatrock.
// 
// 1     6/12/03 4:06p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:41a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:25a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:34  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 7:03a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:01a Khykin
// Initial Checkin From DCTC
// 
// 10    8/23/02 8:12a Dan.erickson
// Added defaulot parameter to DeepCopy method to allow data to be
// appended to a map instead of always clearing the map before adding
// nodes.
// 
// 9     8/14/02 10:28a Skypig13
// Added operator= prototype
// 
// 8     4/02/02 10:59p Skypig13
// Changed assignment operator to DeepCopy() method
//
// 7     3/21/02 6:06p Bmeinke
// Added assignment operator for XmlNodeMap
//
// 6     8/16/01 3:25p Brian.meinke
// Added AsciiToXml method to convert ascii strings to XML strings
//
// 5     7/31/01 1:54p Brian.meinke
// No more const on clear() method
//
// 4     7/31/01 1:39p Brian.meinke
// Added overloaded clear() method that can free memory for the contained
// XmlNodes
//
// 3     5/16/01 4:16p Brian.meinke
// Changed return type of Reason() to XmlString
// Added method GetReason
//
// 2     4/24/01 1:55p Brian.meinke
//  Added function XmlToAscii to convert an XML String into an ASCII
// string
//
// 1     4/10/01 10:46a Admin
//
// 5     3/19/01 9:02a Brian.meinke
// Move all method implementation code to the cpp file
// XmlNodeMap now inherits from std::map< XmlString, XmlNode*>
// No longer has any member data
// Removed invalidateList() and clear() methds
//
// 4     3/09/01 4:49p Brian.meinke
// Overloaded methods taking 'const XmlString &' arguments to accept const
// XML_Char * also
//
// 3     3/09/01 4:39p Kevin.hykin
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 2     12/12/00 3:13p Brian.meinke
// Updates for QNX/Watcom 10.6:
// Watcom doesn't like exceptions
// Revised declarations for XmlString and XmlOStream
// Overloaded issapce function inside XmlParser class for unicode.ascii
// Miscellaneous other Watcom quirks
//
// 1     12/07/00 12:59p Brian.meinke
// BEP XML Processor: XmlNode, XmlParser, XmlString, XmlException classes.
// Utilizes expat 'C'-based xml processor.
//
//******************************************************************************
#ifndef BEPXMLUTILS_H
#define BEPXMLUTILS_H
//==============================================================================
class XmlException;
class XmlNodeMap;
//==============================================================================
class XmlNode;
//==============================================================================
#include <pthread.h>        // pthread_mutex_*

#include "XmlDefs.h"

//==============================================================================

/**
 * Converts an XML String (wide) to a standard ascii string
 *
 * @param xmlStr Wide string to convert to ascii
 * @return ASCII equivalent of the wide string
 */
string XmlToAscii( const XmlString &xmlStr);

/**
 * Converts an ASCII String to an XML (wide) string
 *
 * @param asciiStr ASCII string to convert to XML
 * @return XML equivalent of the ASCII string
 */
XmlString AsciiToXml( const string &asciiStr);

//==============================================================================
class XmlException : public BepException
{
public:
    XmlException( const XmlString &errString) : BepException(), m_errString(errString)
    {
    }
    XmlException( const XML_Char *errString) : BepException(), m_errString(errString)
    {
    }

    const XmlString& Reason() const
    {
        // if m_errString changes from a string this needs to change
        return(m_errString);
    }
    const char* GetReason()
    {
        m_asciiErr = XmlToAscii( m_errString);
        return(m_asciiErr.c_str());
    }
private:
    XmlString m_errString;
    string m_asciiErr;
};
//==============================================================================
class XmlNodeMap : public XmlNodeMapType
{
public:
    XmlNodeMap();
    XmlNodeMap( const XmlNodeMap& copy);
    ~XmlNodeMap();

    /**
     * Adds a new node to the node list
     *
     * @param node   Node to add to the list
     */
    void addNode( const XmlNode* node);

    /**
     * Removes a given node from the node list
     *
     * @param tag    Tag of node to delete fro mthe list
     */
    void delNode( const XmlString &tag);

    /**
     * Retrieves a node from the node list using the node's tag list. Throws an
     * XmlException if tag cannot be found.
     *
     * @param tag    Tag of node to be retrieved
     *
     * @return Pointer to node (if found)
     */
    const XmlNode* getNode( const XML_Char *tag) const;

    /**
     * Retrieves a node from the node list using the node's tag list. Throws an
     * XmlException if tag cannot be found.
     *
     * @param tag    Tag of node to be retrieved
     *
     * @return Pointer to node (if found)
     */
    const XmlNode* getNode( const XmlString &tag) const;

    /**
     * Returns a node based on insertion order.Throws an XmlException if the
     * index is invalid.
     *
     * @param Index  Index of node to retrieve
     *
     * @return Pointer to desired node
     */
    const XmlNode* getNode( int Index) const;

    /**
     * Overloaded clear method to clear all nodes in the XmlNodeMap
     * and optionally free memory used by the contained node pointers
     *
     * @param freeAll Argument to control whether or not the contained node pointers are deleted.
     *                If true, the node pointers will be deleted.
     *                Default = false
     */
    void clear( bool freeAll=false);

    /**
     * Deep Copy copying map nodes and all nodes children
     * 
     * @param rhs        XmlNodeMap object to be copied into *this
     * @param clearFirst Clear the map before adding new nodes.
     * @return *this
     */
    XmlNodeMap &DeepCopy( const XmlNodeMap &rhs, const bool clearFirst = true);
    /**
     * Assignment operator
     * 
     * @param rhs    Object being assigned to (right hand side of =)
     * @return New object
     * @since Version 1.0
     */
    XmlNodeMap& operator=( const XmlNodeMap &rhs);

    inline int Lock() const
    {
        return( pthread_mutex_lock( &m_lock));
    }

    inline int Unlock() const
    {
        return( pthread_mutex_unlock( &m_lock));
    }
private:

    /**
     * Mutex used to guard our member data
     */
    pthread_mutexattr_t         m_lockAttr;
    mutable pthread_mutex_t     m_lock;

};
#endif
//==============================================================================
