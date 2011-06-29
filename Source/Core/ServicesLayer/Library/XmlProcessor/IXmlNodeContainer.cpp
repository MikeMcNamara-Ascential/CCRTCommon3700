//******************************************************************************
//
// FILE:    IXmlNodeContainer.h
//     $Archive:: /Ccrt/Source/Core/ServicesLayer/Library/XmlProcessor/IXmlNo $
//     $Author: Cward $
//     $Date: 4/13/06 3:47a $
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/XmlProcessor/IXmlNodeContainer.cpp $
// 
// 2     4/13/06 3:47a Cward
// Removed the default parameter value specifications from the cpp file
//
// 2     2/15/06 3:59p Bmeinke
// Removed the default parameter value specifications from the cpp file
// (should only be in the h file)
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:50p Khykin
// Initial checkin.
//
// 1     4/22/03 9:33a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:22a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:03:45  tbochene
//
//     PooBag
//
//
// 1     1/27/03 6:26a Khykin
// Initial Checkin
//
// 1     12/19/02 12:20p Bmeinke
// Refactorization of parser to use std::list instead of std::map for
// storing children nodes
//
//******************************************************************************

#include "IXmlNodeContainer.h"

/**
 * Default constructor
 */
IXmlNodeContainer::IXmlNodeContainer()
{
}

/**
 * Destructor
 */
IXmlNodeContainer::~IXmlNodeContainer()
{
}

/**
 * Removes a given node from the node container
 *
 * @param tag    Tag of node to delete from the container
 */
void IXmlNodeContainer::DelNode( const XmlString &tag, bool delAll)
{
	DelNode( tag.c_str(), delAll);
}

/**
 * Removes the given node from the node container
 *
 * @param delNode Node to be removed
 */
void IXmlNodeContainer::DelNode( const XmlNode *delNode)
{
	DelNode( delNode->getName());
}

/**
 * Retrieves a node from the node list using the node's tag list.
 *
 * @param tag    Tag of node to be retrieved
 * @return Pointer to node (if found)
 * @exception XmlException
 *                   Throws an XmlException if tag cannot be found.
 */
const XmlNode* IXmlNodeContainer::GetNode( const XmlString &tag) const throw( XmlException)
{
	return( GetNode( tag.c_str()));
}

/**
 * Assignment operator
 *
 * @param rhs    Object being assigned to (right hand side of =)
 * @return *this
 */
IXmlNodeContainer& IXmlNodeContainer::operator=( const IXmlNodeContainer &rhs)
{
	return( DeepCopy( rhs));
}
