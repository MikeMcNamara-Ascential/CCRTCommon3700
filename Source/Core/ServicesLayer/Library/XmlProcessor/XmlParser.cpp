//******************************************************************************
//
// FILE:	XmlParser
//     $Archive:: /Core/ServicesLayer/Library/XmlProcessor/XmlParser.cpp      $
//     $Author: Cward $
//     $Date: 12/21/03 6:27p $
//
// Description:
//	XML Parser class wrapper for the expat 'C'-based XML processor
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Core/ServicesLayer/Library/XmlProcessor/XmlParser.cpp $
// 
// 1     12/21/03 6:27p Cward
// 
// 2     10/24/03 1:05p Khykin
// Updated with latest changes from Korea and Flatrock.
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
// 1     6/12/03 4:51p Khykin
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
// 1     1/27/03 6:27a Khykin
// Initial Checkin
// 
// 1     1/18/03 7:58a Khykin
// Initial Checkin From DCTC
// 
// 9     6/19/02 2:02p Skypig13
// Removed warnings
// 
// 8     2/05/02 12:19a Skypig13
// Removed compiler warnings
//
// 7     10/11/01 11:45a Brian.meinke
// ReturnXMLDocument constructs a tring from the return value of LoadFile
//
// 6     9/14/01 2:02a Brian.meinke
// Changed ReadXMLDocument() method to use a const string reference to
// hold the results of loading an XML file (prior to parsing)
//
// 4     4/30/00 1:41p Kh
// Updated the include statements to not have relational references.
//
// 3     4/20/00 8:57a Kh
// Changes due to the changes in GeneralUtilities.  Now LoadFile does not
// need class scope.
//
// 2     4/17/00 2:15p Kh
// Added method to load and parse a configuration file.
//
// 3     3/19/01 10:39a Brian.meinke
// Remove call to addChild in StartElementHandler (XmlNode constrructor
// automatically does this)
//
// 2     3/05/01 8:12a Tim.bochenek
// Still testing
//
// 9     12/19/00 10:36a Brian.meinke
// Character Data handler handles regular text as well as CDATA text.
// Changed handler to process non-empty text and set value of Parent
// Element node if not processing CDATA
//
// 8     12/19/00 9:44a Brian.meinke
// Change tchar.H to tchar.h
//
// 7     12/19/00 9:04a Brian.meinke
// Added TCHAR.H header for the _istspace macro definition
//
// 6     12/18/00 2:11p Brian.meinke
// Added method getDocument: returns the entire parsed XML buffer using a
// pseudo root node element to accomodate any non-elemtn type nodes
// encountered at the beginning of the buffer (i.e. leading comment
// nodes).
// THe getDocElement method now returns the first element type node found
// during parsing, or NULL if no element type node is parsed.
//
// 5     12/18/00 12:44p Brian.meinke
// Removed overloaded isspace member and replaced with _istspace function
// call
//
// 4     12/18/00 9:55a Brian.meinke
// Children TEXT_NODEs are now added as the Value of their parent node
// instead of a child TEXT_NODE
//
// 3     12/13/00 4:27p Brian.meinke
// Changed CDATA processing: now uses CharacterDataHandler method
// Small modification to PI ELement Handl;er method for efficiency
// Modifications to DefaultHAndl;er method because of new CDATA processing
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
//#include <tchar.h>
#include "XmlParser.h"

#if defined(_MSC_VER)
	#pragma warn -aus
#endif // defined(_MSC_VER)

/* atts is array of name/value pairs, terminated by 0;
   names and values are 0 terminated. */
void XmlParser::StartElementHandler(void *userData,
									const XML_Char *name,
									const XML_Char **atts)
{
	XmlParser   *xmlParser = (XmlParser*)userData;
	XmlNode     *xmlParent = NULL;
	XmlElement  *xmlElement = NULL;
	register int ii=0;

	// Set proper parent propery for this new node
	if(xmlParser->m_elemLifo.size() > 0)
	{
		// Get parent node for this element from list of nodes being processed
		xmlParent = xmlParser->m_elemLifo[ 0];

		// The parent will delete this child when destroyed
		xmlElement = new XmlElement( name, XML_T(""), xmlParent);
		//xmlParent->addChild( xmlElement);

		// IF this is the first element node encountered
		if(xmlParser->m_elemCount++ == 0)
		{
			// make it the document element
			xmlParser->m_docElement = xmlElement;
		}

		// Fill in attribute list
		for(ii=0; atts[ ii] && atts[ ii+1]; ii+=2)
		{
			xmlElement->addChild( XmlString(atts[ii]), XmlString(atts[ii+1]), ATTRIBUTE_NODE);
		}
	}
	else
	{
	}

	// Add this node to the head of nodes being processed
	xmlParser->m_elemLifo.insert( xmlParser->m_elemLifo.begin(), xmlElement);
}

void XmlParser::EndElementHandler(void *userData,
								  const XML_Char *name)
{
	XmlParser   *xmlParser = (XmlParser *)userData;

	// Remove last node from list of nodes being processed
	if(xmlParser->m_elemLifo.size() > 0)
	{
		xmlParser->m_elemLifo.erase( xmlParser->m_elemLifo.begin());
	}
}

/* s is not 0 terminated. */
void XmlParser::CharacterDataHandler(void *userData,
									 const XML_Char *s,
									 int len)
{
	XmlParser   *xmlParser = (XmlParser*)userData;
	XmlNode     *xmlParent = NULL;
	bool        blankItem = true;
	register int ii;

	// Make sure data is not whitespace
	for(ii=0; ii<len; ii++)
	{
//		if( _istspace( s[ ii]) == false)
		if(isspace( s[ ii]) == false)
		{
			if(ii > 0)
			{
				blankItem = true;
			}
			blankItem = false;
			break;
		}
	}

	if(blankItem == false)
	{
		// If currently processing CDATA section
		if(xmlParser->m_currentCDataNode != NULL)
		{
			xmlParser->m_currentCData += XmlString( s, len);
		}
		else
		{
			// Set proper parent propery for this new node
			if(xmlParser->m_elemLifo.size() > 0)
			{
				// Get parent node for this element from list of nodes being processed
				xmlParent = xmlParser->m_elemLifo[ 0];
			}
			else
			{
				xmlParent = xmlParser->m_xmlDoc;
			}
			XmlString xmlVal( s, len);
			xmlParent->setValue( xmlVal);
		}
	}
}

/* target and data are 0 terminated */
void XmlParser::ProcessingInstructionHandler(void *userData,
											 const XML_Char *target,
											 const XML_Char *data)
{
	XmlParser   *xmlParser = (XmlParser*)userData;

	// Set proper parent propery for this new node
	if(xmlParser->m_elemLifo.size() > 0)
	{
		xmlParser->m_elemLifo[ 0]->addChild( XmlString( target),
											 XmlString( data),
											 PROCESSING_INSTRUCTION_NODE);
	}
}

/* data is 0 terminated */
void XmlParser::CommentHandler(void *userData,
							   const XML_Char *data)
{
	XmlParser   *xmlParser = (XmlParser*)userData;
	XmlNode *xmlParent = NULL;

	// Set proper parent propery for this new node
	if(xmlParser->m_elemLifo.size() > 0)
	{
		// Get parent node for this element from list of nodes being processed
		xmlParent = xmlParser->m_elemLifo[ 0];
	}
	//xmlParent->addChild( XmlString( XML_T("")), XmlString( data), COMMENT_NODE);
}

void XmlParser::StartCdataSectionHandler(void *userData)
{
	XmlParser   *xmlParser = (XmlParser*)userData;
	XmlNode *xmlParent = NULL;

	// Set proper parent propery for this new node
	if(xmlParser->m_elemLifo.size() > 0)
	{
		// Get parent node for this element from list of nodes being processed
		xmlParent = xmlParser->m_elemLifo[ 0];
	}
	else
	{
		xmlParent = xmlParser->m_xmlDoc;
	}
	/*
	xmlParser->m_currentCDataNode = (XmlCDataNode*)xmlParent->addChild(
																	  XmlString( XML_T("")), XmlString( XML_T("")), CDATA_SECTION_NODE);
	*/

}
void XmlParser::EndCdataSectionHandler(void *userData)
{
	XmlParser   *xmlParser = (XmlParser *)userData;

	if(xmlParser->m_currentCDataNode != NULL)
	{
		xmlParser->m_currentCDataNode->setValue( xmlParser->m_currentCData);
	}
	xmlParser->m_currentCDataNode = NULL;
	xmlParser->m_currentCData = XmlString( XML_T(""));
}

/* This is called for any characters in the XML document for
which there is no applicable handler.  This includes both
characters that are part of markup which is of a kind that is
not reported (comments, markup declarations), or characters
that are part of a construct which could be reported but
for which no handler has been supplied. The characters are passed
exactly as they were in the XML document except that
they will be encoded in UTF-8.  Line boundaries are not normalized.
Note that a byte order mark character is not passed to the default handler.
There are no guarantees about how characters are divided between calls
to the default handler: for example, a comment might be split between
multiple calls. */
void XmlParser::DefaultHandler(void *userData,
							   const XML_Char *s,
							   int len)
{
	XmlParser   *xmlParser = (XmlParser*)userData;
	XmlNode     *xmlParent = NULL;
	bool        blankItem = true;
	register int ii;

	// Make sure data is not whitespace
	for(ii=0; ii<len; ii++)
	{
//		if( _istspace( s[ ii]) == false)
		if(isspace( s[ ii]) == false)
		{
			if(ii > 0)
			{
				blankItem = true;
			}
			blankItem = false;
			break;
		}
	}

	if(blankItem == false)
	{
		XmlString   xmlTag( s, len);
		// Set proper parent propery for this new node
		if(xmlParser->m_elemLifo.size() > 0)
		{
			// Get parent node for this element from list of nodes being processed
			xmlParent = xmlParser->m_elemLifo[ 0];
		}
		else
		{
			xmlParent = xmlParser->m_xmlDoc;
		}
//		xmlParent->addChild( XmlString( XML_T("")), xmlTag, TEXT_NODE);
		xmlParent->setValue( xmlTag);
	}
}

const XmlNode *XmlParser::ReturnXMLDocument(const std::string &ConfigFile)
{
	string buffer(LoadFile(ConfigFile));
	XmlString xmlRoot( XML_T("ROOT"));

	Parse( buffer.c_str(), buffer.length(), true, xmlRoot);
	const XmlNode *doc;
	if(getDocument() != NULL)  // if valid document
		doc = getDocElement();	// get the root node
	else						// else throw and exception
		throw XmlException("XML document not valid!" + ConfigFile);

	return(doc);
}

#if defined(_MSC_VER)
	#pragma warn -aus
#endif // defined(_MSC_VER)

