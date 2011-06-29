//******************************************************************************
//
// FILE:    IXmlNodeContainer.h
//     $Archive:: /Ccrt/Source/Core/ServicesLayer/Include/IXmlNodeContainer.h $
//     $Author: Rwiersem $
//     $Date: 10/30/07 3:46p $
//
// Description:
//  Interface definition for an object which will contain XML nodes
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/IXmlNodeContainer.h $
// 
// 2     10/30/07 3:46p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 1     12/21/03 6:27p Cward
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
//     Revision 1.1.1.1  2003/02/15 00:05:40  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 7:06a Khykin
// Initial Checkin
// 
// 1     12/19/02 12:20p Bmeinke
// Refactorization of parser to use std::list instead of std::map for
// storing children nodes
// 
//******************************************************************************
#ifndef _IXMNODECREATOR_H_INCLUDED_
#define _IXMNODECREATOR_H_INCLUDED_

class IXmlNodeContainer;

#include "BepXmlUtils.h"
#include "XmlNode.h"

/**
 * Interface definition for an object which will contain XML nodes
 * 
 * @author Brian Meinke
 * @version 1.0
 */
class IXmlNodeContainer
{
public:

    /**
     * Default constructor
     */
    IXmlNodeContainer();
    
    /**
     * Destructor
     */
    virtual ~IXmlNodeContainer();

    /**
     * Adds a new node to the node container
     *
     * @param node   Node to add to the container
     */
    virtual void AddNode( const XmlNode* node) = 0;

    /**
     * Removes a given node from the node container
     * 
     * @param tag    Tag of node to delete from the container
     * @param delAll Flag used to indicate whether all nodes with the given tag should
     *               be removed (true) or not (false)
     * @return true if at least one node with the given tag was removed
     */
    virtual bool DelNode(const XML_Char *tag, bool delAll=false) = 0;

    /**
     * Removes a given node from the node container
     *
     * @param tag    Tag of node to delete from the container
     * @param delAll Tells if the memory for the node should be deleted.
     */
    virtual void DelNode( const XmlString &tag, bool delAll=false);

    /**
     * Removes the given node from the node container
     * 
     * @param delNode Node to be removed
     */
    virtual void DelNode( const XmlNode* delNode);

    /**
     * Retrieves a node from the node container using the node's tag list.
     * 
     * @param tag    Tag of node to be retrieved
     * @return Pointer to node (if found)
     * @exception XmlException
     *                   Throws an XmlException if tag cannot be found.
     */
    virtual const XmlNode* GetNode( const XML_Char *tag) const throw( XmlException) = 0;

    /**
     * Retrieves a node from the node list using the node's tag list.
     * 
     * @param tag    Tag of node to be retrieved
     * @return Pointer to node (if found)
     * @exception XmlException
     *                   Throws an XmlException if tag cannot be found.
     */
    virtual const XmlNode* GetNode( const XmlString &tag) const throw( XmlException);

    /**
     * Returns a node based on insertion order.
     * 
     * @param Index  Index of node to retrieve
     * @return Pointer to desired node
     * @exception XmlException
     *                   Throws an XmlException if tag cannot be found.
     */
    virtual const XmlNode* GetNode( int Index) const throw( XmlException) = 0;

    /**
     * Removes all contained nodes from the container
     * 
     * @param freeAll Flag indicating whether or not we should delete each contained item
     *                before emptying the container
     */
    virtual void Clear( bool freeAll=true) = 0;

    /**
     * Deep Copy copying map nodes and all nodes children
     * 
     * @param rhs    IXmlNodeContainer object to be copied into *this
     * @return *this
     */
    virtual IXmlNodeContainer &DeepCopy( const IXmlNodeContainer &rhs) = 0;

    /**
     * Performs a deep copy of *this into rhs
     * 
     * @param rhs    IXmlNodeContainer object into which *this will be copied
     * @return *this
     */
    virtual void CopyTo( IXmlNodeContainer &rhs) const = 0;

    /**
     * Assignment operator
     * 
     * @param rhs    Object being assigned to (right hand side of =)
     * @return *this
     */
    virtual IXmlNodeContainer& operator=( const IXmlNodeContainer &rhs);

    /**
     * Return the number of nodes in the container
     * 
     * @return The number of nodes in the container
     */
    virtual const int Size() const = 0;

    /**
     * Returns a "pretty" string representation of the XML document with
     * whitespaces and linefeeds
     * 
     * @return A "pretty" string representation of the XML document with
     *         whitespaces and linefeeds
     */
    virtual XmlString GetString() = 0;

    /**
     * Returns a "raw" string representation of the XML document without
     * whitespaces or linefeeds
     * 
     * @param xmlString String to be populated with the XML data
     */
    virtual void GetString( XmlString &xmlString) = 0;
};

#endif // _IXMNODECREATOR_H_INCLUDED_
