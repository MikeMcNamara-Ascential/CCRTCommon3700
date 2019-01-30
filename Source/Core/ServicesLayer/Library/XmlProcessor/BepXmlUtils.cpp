//******************************************************************************
//
// FILE:    BepXmlUtils
//     $Archive:: /Core/ServicesLayer/Library/XmlProcessor/BepXmlUtils.cpp    $
//     $Author: Cward $
//     $Date: 1/23/04 3:19p $
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
//     $Log: /Core/ServicesLayer/Library/XmlProcessor/BepXmlUtils.cpp $
// 
// 3     1/23/04 3:19p Cward
// Last minute high priority changes. Must be added.
//
// 6     1/22/04 9:52a Derickso
// When adding a node, only delete duplicates if the duplicate is not the
// same  as the node begin added.
//
// 5     1/20/04 6:45a Derickso
// Merged with latest core from korea and ford.
//
// 4     10/20/03 10:10a Derickso
// Latest ServicesLayer update to correct for memory leaks and crashes.
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 2     8/07/03 10:24a Khykin
// Updated with latest changes for Flatrock.
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
// 1     1/18/03 7:58a Khykin
// Initial Checkin From DCTC
//
// 14    8/23/02 8:00a Dan.erickson
// Added optional parameter to DeepCopy to allow appending to a current
// Map instead of always clearing the map first.
//
// 13    8/14/02 10:07a Skypig13
// Updated clear() and added operator= for XmlNodeMap
//
// 12    4/02/02 10:49p Skypig13
// Updates to fix assignment and deletion
//
// 10    11/08/01 2:53a Brian.meinke
// Modified XmlNodeMap::addNode() to erase any node with the same tag as
// the node passed in before inserting the node into the node map
//
// 9     8/16/01 3:24p Brian.meinke
// Added AsciiToXml method to convert ascii strings to XML strings
//
// 8     7/31/01 2:03p Brian.meinke
// Stupid == vs. = !!!!!!!!!
//
// 7     7/31/01 1:54p Brian.meinke
// No more const on clear() method
//
// 6     7/31/01 1:51p Brian.meinke
// Oops...recursive clear() forever. Sorry
//
// 5     7/31/01 1:39p Brian.meinke
// Added overloaded clear() method that can free memory for the contained
// XmlNodes
//
// 3     4/30/00 1:41p Kh
// Updated the include statements to not have relational references.
//
// 2     4/24/01 1:55p Brian.meinke
// Added function XmlToAscii to convert an XML String into an ASCII string
//
// 1     4/10/01 10:46a Admin
//
// 4     3/19/01 9:01a Brian.meinke
// Move all method implementation code to the cpp file
// XmlNodeMap now inherits from std::map< XmlString, XmlNode*>
//
// 3     3/09/01 4:48p Brian.meinke
// Overloaded methods taking 'const XmlString &' arguments to accept const
// XML_Char * also
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 3     12/18/00 10:48a Brian.meinke
// Changed '#ifdef WATCOM' to  '#ifdef __WATCOMC__'
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
#include <stdio.h>
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
	#include <errno.h>
#endif

#include "BepXmlUtils.h"
#include "XmlNode.h"
//******************************************************************************

/**
 * Converts an XML String (wide) to a standard ascii string
 *
 * @param xmlStr Wide string to convert to ascii
 * @return ASCII equivalent of the wide string
 */
string XmlToAscii( const XmlString &xmlStr)
{
#ifdef XML_UNICODE_WCHAR_T
	char    *asciiBuff = new char[ xmlStr.length()+1];
	string  retStr;

	// Convert wide char to ascii
	wcstombs( asciiBuff, xmlStr.c_str(), xmlStr.length());
	retStr = string( asciiBuff);

	delete[] asciiBuff;
	return( retStr);

#else
	return( xmlStr);
#endif

}

/**
 * Converts an ASCII String to an XML (wide) string
 *
 * @param asciiStr ASCII string to convert to XML
 * @return XML equivalent of the ASCII string
 */
XmlString AsciiToXml( const string &asciiStr)
{
#ifdef XML_UNICODE_WCHAR_T
	XML_Char    *xmlBuff = new char[ asciiStr.length()+1];
	XmlString   retStr;

	// Convert wide char to ascii
	mbstowcs( xmlBuff, asciiStr.c_str(), asciiStr.length());
	retStr = XmlString( xmlBuff);

	delete[] xmlBuff;
	return( retStr);

#else
	return( asciiStr);
#endif

}

//******************************************************************************
XmlNodeMap::XmlNodeMap() : XmlNodeMapType()
{
	memset( &m_lockAttr, 0, sizeof( m_lockAttr));
	memset( &m_lock, 0, sizeof( m_lock));
	
	if( (errno=pthread_mutexattr_init( &m_lockAttr)) != EOK)
	{
		perror( "XmlNodeMap: pthread_mutexattr_init error");
	}																						
    else if( (errno=pthread_mutexattr_settype( &m_lockAttr, PTHREAD_MUTEX_RECURSIVE)) != EOK)
    {
		perror( "XmlNodeMap: pthread_mutexattr_settype error");
    }
	else if ( (errno=pthread_mutexattr_setrecursive( &m_lockAttr, PTHREAD_RECURSIVE_ENABLE)) != EOK)
	{
		perror( "XmlNodeMap: pthread_mutexattr_setrecursive error");
	}
	else if ((errno=pthread_mutex_init( &m_lock, &m_lockAttr)) != EOK)
	{
		perror( "XmlNodeMap: pthread_mutex_init error");
	}
}
XmlNodeMap::XmlNodeMap( const XmlNodeMap& copy) : XmlNodeMapType( copy)
{
	memset( &m_lockAttr, 0, sizeof( m_lockAttr));
	memset( &m_lock, 0, sizeof( m_lock));
	
	if( (errno=pthread_mutexattr_init( &m_lockAttr)) != EOK)
	{
		perror( "XmlNodeMap: pthread_mutexattr_init error");
	}																						
    else if( (errno=pthread_mutexattr_settype( &m_lockAttr, PTHREAD_MUTEX_RECURSIVE)) != EOK)
    {
		perror( "XmlNodeMap: pthread_mutexattr_settype error");
    }
	else if ( (errno=pthread_mutexattr_setrecursive( &m_lockAttr, PTHREAD_RECURSIVE_ENABLE)) != EOK)
	{
		perror( "XmlNodeMap: pthread_mutexattr_setrecursive error");
	}
	else if ((errno=pthread_mutex_init( &m_lock, &m_lockAttr)) != EOK)
	{
		perror( "XmlNodeMap: pthread_mutex_init error");
	}
}
XmlNodeMap::~XmlNodeMap()
{
	Lock();
	clear( false);
	Unlock();

	pthread_mutexattr_destroy( &m_lockAttr);
	pthread_mutex_destroy( &m_lock);
}

/**
 * Adds a new node to the node list
 *
 * @param node   Node to add to the list
 */
void XmlNodeMap::addNode( const XmlNode* node)
{
	if( (errno=Lock()) == EOK)
	{
		try
		{
			const XmlString &nodeName = node->getName();
			
			XmlNodeMap::iterator nodeIter = find( nodeName);
			if(nodeIter != end())
			{
				// get the node reference
				XmlNode *duplicate = nodeIter->second;
				// delete the reference from the list
				delNode( nodeName);
				// delete the memory if not inserting the
				// exact same object twice
				if(node != duplicate)
				{
					delete duplicate;
					duplicate = NULL;
				}
			}
			
			insert( XmlNodeMapVal( nodeName, (XmlNode*)node) );
		}
		catch( ...)
		{
			Unlock();
			throw;
		}
		Unlock();
	}
	else
	{
		XmlString errStr( "Unable to lock map in addNode");
		errStr += XmlString( strerror( errno));
		throw( XmlException( errStr));
	}
}			

/**
 * Removes a given node from the node list
 *
 * @param tag    Tag of node to delete fro mthe list
 */
void XmlNodeMap::delNode( const XmlString &tag)
{
	if( (errno=Lock()) == EOK)
	{
		try
		{
			erase( tag);
		}
		catch( ...)
		{
			Unlock();
			throw;
		}

		Unlock();
	}
	else
	{
		XmlString errStr( "Unable to lock map in delNode");
		errStr += XmlString( strerror( errno));
		throw( XmlException( errStr));
	}
}

/**
 * Retrieves a node from the node list using the node's tag list. Throws an
 * XmlException if tag cannot be found.
 *
 * @param tag    Tag of node to be retrieved
 *
 * @return Pointer to node (if found)
 */
const XmlNode* XmlNodeMap::getNode( const XML_Char *tag) const
{
	const XmlString tempTag( tag);

	return( getNode( tempTag));
}

/**
 * Retrieves a node from the node list using the node's tag list. Throws an
 * XmlException if tag cannot be found.
 *
 * @param tag    Tag of node to be retrieved
 *
 * @return Pointer to node (if found)
 */
const XmlNode* XmlNodeMap::getNode( const XmlString &tag) const
{
	const XmlNode* retVal = NULL;
	
	if( (errno=Lock()) == EOK)
	{
		try
		{
			XmlNodeMapCItr Itr = find( tag);
			if(Itr != end())
			{
				retVal = (*Itr).second;
			}
			else
			{
				throw(XmlException(XmlString("Tag Not Found: ") + tag));
			}
		}
		catch( ...)
		{
			Unlock();
			throw;
		}

		Unlock();
	}
	else
	{
		XmlString errStr( "Unable to lock map in getNode");
		errStr += XmlString( strerror( errno));
		throw( XmlException( errStr));
	}

	return( retVal);
}

/**
 * Returns a node based on insertion order.Throws an XmlException if the
 * index is invalid.
 *
 * @param Index  Index of node to retrieve
 *
 * @return Pointer to desired node
 */
const XmlNode* XmlNodeMap::getNode(int Index) const
{
	const XmlNode* retVal = NULL;
	if( (errno=Lock()) == EOK)
	{
		try
		{
			// Validate desired index
			if(Index < (int)size())
			{
				// Start at head of list
				XmlNodeMapCItr  itr = begin();
				while(Index--)	   itr++;
				// Return Node in this position
				retVal = (*itr).second;
			}
			else
			{	// Invalidate index passed in
				XML_Char temp[255];
				//_stprintf(temp, XML_T("Node %d not found"), Index);
				sprintf(temp, XML_T("Node %d not found"), Index);

				throw XmlException(XmlString(temp));
			}
		}
		catch( ...)
		{
			Unlock();
			throw;
		}
		Unlock();
	}
	else
	{
		XmlString errStr( "Unable to lock map in getNode");
		errStr += XmlString( strerror( errno));
		throw( XmlException( errStr));
	}

	return( retVal);
}

/**
 * Retrieves a node from the node list using the node's tag list. Throws an
 * XmlException if tag cannot be found.
 *
 * @param tag    Tag of node to be retrieved
 *
 * @return Pointer to node (if found)
 */
const bool XmlNodeMap::hasNode(const XML_Char *tag) const
{
    const XmlString tempTag(tag);

    return (hasNode(tempTag));
}

/**
 * Retrieves a node from the node list using the node's tag list. Throws an
 * XmlException if tag cannot be found.
 *
 * @param tag    Tag of node to be retrieved
 *
 * @return Pointer to node (if found)
 */
bool XmlNodeMap::hasNode(const XmlString &tag) const
{
    bool nodeExists = false;

    if ((errno = Lock()) == EOK)
    {
        try
        {
            XmlNodeMapCItr Itr = find(tag);
            if (Itr != end())
            {
                nodeExists = true;
            }
            else
            {
                nodeExists = false;
            }
        }
        catch (...)
        {
            Unlock();
            throw;
        }

        Unlock();
    }
    else
    {
        XmlString errStr("Unable to lock map in getNode");
        errStr += XmlString(strerror(errno));
        throw (XmlException(errStr));
    }

    return (nodeExists);
}

/**
 * Overloaded clear method to clear all nodes in the XmlNodeMap
 * and optionally free memory used by the contained node pointers
 *
 * @param freeAll Argument to control whether or not the contained node pointers are deleted.
 *                If true, the node pointers will be deleted.
 *                Default = false
 */
void XmlNodeMap::clear( bool freeAll)
{
	if( (errno=Lock()) == EOK)
	{
		try
		{
			// If caller wants to free memory used bu the contained nodes
			if(freeAll == true)
			{
				XmlNodeMapCItr  itr;
				// Walk through list of all nodes and delete them
				for(itr=begin(); itr!=end(); itr++)
				{
					XmlNode *node = itr->second;
					if(node)
					{
						delete node;
						node = NULL;
					}
					else
					{
						printf( "\n\n\tNULL node found in XMlNodeMap::clear()\n\n");
					}
				}
			}
			// Erase all map entries
			XmlNodeMapType::clear();
		}
		catch(...)
		{
			Unlock();
			throw;
		}

		Unlock();
	}
	else
	{
		XmlString	errStr( "Unable to lock map in clear: ");
		errStr += XmlString( strerror( errno));
		throw( XmlException( errStr));
	}

}

/**
 * Deep Copy copying map nodes and all nodes children
 *
 * @param rhs    XmlNodeMap object to be copied into *this
 * @return *this
 */
XmlNodeMap &XmlNodeMap::DeepCopy( const XmlNodeMap &rhs, const bool clearFirst /*= true*/)
{
	XmlNodeMapCItr  itr;
	if( (errno=Lock()) == EOK)
	{
		try
		{
			if( (errno=rhs.Lock()) == EOK)
			{
				try
				{
					if( this != &rhs)
					{
						// Clear existing nodes and free their memory
						if(clearFirst) clear( true);
						// Copy the source XmlNodeMap object
						for(itr=rhs.begin(); itr!=rhs.end(); itr++)
						{
							// Get reference to this source node
							const XmlNode *sourceNode = itr->second;

							// Add a newly allocated copy of the source node
							addNode( sourceNode->Copy());
						}
					}
				}
				catch( ...)
				{
					rhs.Unlock();
					throw;
				}
				rhs.Unlock();
			}
			else
			{
				XmlString errStr( "Unable to lock source copy in DeepCopy");
				errStr += XmlString( strerror( errno));
				throw( XmlException( errStr));
			}
		}
		catch( ...)
		{
			Unlock();
			throw;
		}

		Unlock();
	}
	else
	{
		XmlString errStr( "Unable to lock target map in DeepCopy");
		errStr += XmlString( strerror( errno));
		throw( XmlException( errStr));
	}

	return( *this);
}
XmlNodeMap& XmlNodeMap::operator=( const XmlNodeMap &rhs)
{
	return( DeepCopy( rhs, true));
}
//==============================================================================

