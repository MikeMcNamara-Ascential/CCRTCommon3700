//******************************************************************************
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
//     $Log: /Core/ServicesLayer/Include/XmlParser.h $
// 
// 2     6/24/04 12:54a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:38a Khykin
// Initial checkin
//
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 4:27p Khykin
// Initial checkin.
//
// 1     4/22/03 9:46a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:50  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:11a Khykin
// Initial Checkin
//
// 1     1/18/03 8:08a Khykin
// Initial Checkin From DCTC
//
// 8     6/19/02 2:53p Skypig13
// Removed unused methods to get rid of warnings
//
// 7     8/20/01 12:06p Brian.meinke
// Removed unneeded methods for OS lib compatability
//
// 6     8/16/01 3:27p Brian.meinke
// Removed calls to the following expat functions in order to use OS XML
// libraries:
//  XML_SetDoctypeDeclHandler
//  XML_SetUnparsedEntityDeclHandler
//  XML_SetNotationDeclHandler
//  XML_SetExternalParsedEntityDeclHandler
//  XML_SetInternalParsedEntityDeclHandler
//  XML_SetNamespaceDeclHandler
//  XML_SetNotStandaloneHandler
//  XML_SetExternalEntityRefHandler
//
// 5     4/25/00 9:05a Skypig13
// Putting back exception throwing on empty document
//
// 4     4/17/00 2:16p Kh
// Adding method to load a configuration file and return a doc.
//
// 5     3/20/01 10:03a Brian.meinke
// Added overloaded Parse methoed which takes a const XML_Char* instead of
// XmlString&
//
// 4     3/19/01 9:05a Brian.meinke
// Added #include <stdio.h>
//
// 3     3/13/01 1:52p Brian.meinke
// Changed method XML_ErrorString to ErrorString to avoid name confusion
// with expat global function
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 5     12/18/00 2:11p Brian.meinke
// Added method getDocument: returns the entire parsed XML buffer using a
// pseudo root node element to accomodate any non-elemtn type nodes
// encountered at the beginning of the buffer (i.e. leading comment
// nodes).
// THe getDocElement method now returns the first element type node found
// during parsing, or NULL if no element type node is parsed.
//
// 4     12/18/00 12:44p Brian.meinke
// Removed overloaded isspace member and replaced with _istspace function
// call
//
// 3     12/13/00 4:29p Brian.meinke
// Set indent level of m_xmlDoc to -1 so pretty printing turns out OK
// Install Character Data Handler to process CDATA better
// Fixed the overloaded isspace methods to explicitly call the global
// isspace and iswspace functions from the C Runtime
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
// $NoKeywords: $
//==============================================================================

#ifndef XMLPARSER_H
#define XMLPARSER_H
//==============================================================================
class XmlParser;
//==============================================================================
#include <stdio.h>
#include "GeneralUtilities.h"
#include "XmlDefs.h"
#include "XmlNode.h"
#include "BepXmlUtils.h"
//==============================================================================
class DLL_CLASS XmlParser
{
public:
	XmlParser() : m_parser( NULL)
	{
		m_xmlDoc = NULL;
		initParser();
	}
	~XmlParser()
	{
		ParserFree();
		delete m_xmlDoc;
		m_xmlDoc = NULL;
	};

	void SetElementHandler(XML_StartElementHandler start,
						   XML_EndElementHandler end)
	{
		XML_SetElementHandler(m_parser, start,end);
	};

	void SetCharacterDataHandler(XML_CharacterDataHandler handler)
	{
		XML_SetCharacterDataHandler(m_parser,handler);
	};

	void SetProcessingInstructionHandler(XML_ProcessingInstructionHandler handler)
	{
		XML_SetProcessingInstructionHandler(m_parser,handler);
	};


	void SetCommentHandler(XML_CommentHandler handler)
	{
		XML_SetCommentHandler(m_parser,handler);
	};

	void SetCdataSectionHandler(XML_StartCdataSectionHandler start,
									XML_EndCdataSectionHandler end)
	{
		XML_SetCdataSectionHandler(m_parser,start,end);
	};

	/* This sets the default handler and also inhibits expansion of internal entities.
	The entity reference will be passed to the default handler. */
	void SetDefaultHandler(XML_DefaultHandler handler)
	{
		XML_SetDefaultHandler(m_parser,handler);
	};

	/* This sets the default handler but does not inhibit expansion of internal entities.
	The entity reference will not be passed to the default handler. */
	void SetDefaultHandlerExpand(XML_DefaultHandler handler)
	{
		XML_SetDefaultHandlerExpand(m_parser,handler);
	};

	/* If a non-null value for arg is specified here, then it will be passed
	as the first argument to the external entity ref handler instead
	of the parser object. */
	void SetExternalEntityRefHandlerArg(void *arg)
	{
		XML_SetExternalEntityRefHandlerArg(m_parser, arg);
	};
	
	void SetUnknownEncodingHandler(XML_UnknownEncodingHandler handler, void *encodingHandlerData)
	{
		XML_SetUnknownEncodingHandler(m_parser, handler, encodingHandlerData);
	};
	
	/* This can be called within a handler for a start element, end element,
	processing instruction or character data.  It causes the corresponding
	markup to be passed to the default handler. */
	void DefaultCurrent()
	{
		XML_DefaultCurrent(m_parser);
	};
	
	/* This value is passed as the userData argument to callbacks. */
	void SetUserData(void *userData)
	{
		XML_SetUserData(m_parser, userData);
	};
	
	/* This is equivalent to supplying an encoding argument
	to XML_ParserCreate. It must not be called after XML_Parse
	or XML_ParseBuffer. */
    int SetEncoding(const XML_Char *encoding)
	{
		return( XML_SetEncoding(m_parser, encoding));
	};
	
	/* If this function is called, then the parser will be passed
	as the first argument to callbacks instead of userData.
	The userData will still be accessible using XML_GetUserData. */
    void UseParserAsHandlerArg()
	{
		XML_UseParserAsHandlerArg(m_parser);
	};
	
	/* Sets the base to be used for resolving relative URIs in system identifiers in
	declarations.  Resolving relative identifiers is left to the application:
	this value will be passed through as the base argument to the
	XML_ExternalEntityRefHandler, XML_NotationDeclHandler
	and XML_UnparsedEntityDeclHandler. The base argument will be copied.
	Returns zero if out of memory, non-zero otherwise. */
    int SetBase(const XML_Char *base)
	{
		return( XML_SetBase(m_parser, base));
	};
	
	const XML_Char* GetBase()
	{
		return( XML_GetBase(m_parser));
	};
	
	
	/* Returns the number of the attribute/value pairs passed in last call
	to the XML_StartElementHandler that were specified in the start-tag
	rather than defaulted. Each attribute/value pair counts as 2; thus
	this correspondds to an index into the atts array passed to the
	XML_StartElementHandler. */
    int GetSpecifiedAttributeCount()
	{
		return( XML_GetSpecifiedAttributeCount(m_parser));
	};
	
	/* Parses some input. Returns 0 if a fatal error is detected.
	The last call to XML_Parse must have isFinal true;
	len may be zero for this call (or any other). */
	int Parse(const char *s, int len, int isFinal, const XmlString &name)
	{
		return(Parse(s,len,isFinal,name.c_str()));
	};

	/* Parses some input. Returns 0 if a fatal error is detected.
	The last call to XML_Parse must have isFinal true;
	len may be zero for this call (or any other). */
	int Parse(const char *s, int len, int isFinal, const XML_Char *name)
	{
		// Force the Parser to flush its internal buffer
		ParserFree();
		
		initParser();

		if( m_xmlDoc != NULL)	delete m_xmlDoc;
		m_xmlDoc = new XmlDocument( name);
		m_xmlDoc->m_indentLevel = -1;

		// No elements yet
		m_elemCount = 0;

		// Invalidate the Document Element
		m_docElement = NULL;

		// Add this node to the head of nodes being processed
		m_elemLifo.clear();
		m_elemLifo.push_back( m_xmlDoc);
		
		int retVal = XML_Parse(m_parser, s, len, isFinal);
		return(retVal);
	};

	void *GetBuffer(int len)
	{
		return( XML_GetBuffer(m_parser, len));
	};

	int ParseBuffer(int len, int isFinal)
	{
		return( XML_ParseBuffer(m_parser, len, isFinal));
	};
	
	/* Creates an XML_Parser object that can parse an external general entity;
	context is a '\0'-terminated string specifying the parse context;
	encoding is a '\0'-terminated string giving the name of the externally specified encoding,
	or null if there is no externally specified encoding.
	The context string consists of a sequence of tokens separated by formfeeds (\f);
	a token consisting of a name specifies that the general entity of the name
	is open; a token of the form prefix=uri specifies the namespace for a particular
	prefix; a token of the form =uri specifies the default namespace.
	This can be called at any point after the first call to an ExternalEntityRefHandler
	so longer as the parser has not yet been freed.
	The new parser is completely independent and may safely be used in a separate thread.
	The handlers and userData are initialized from the parser argument.
	Returns 0 if out of memory.  Otherwise returns a new XML_Parser object. */
	XML_Parser ExternalEntityParserCreate(const XML_Char *context, const XML_Char *encoding)
	{
		return( XML_ExternalEntityParserCreate(m_parser, context, encoding));
	};

	
	/* If XML_Parse or XML_ParseBuffer have returned 0, then XML_GetErrorCode
	returns information about the error. */
    enum XML_Error GetErrorCode()
	{
		return( XML_GetErrorCode(m_parser));
	};
	
	/* These functions return information about the current parse location.
	They may be called when XML_Parse or XML_ParseBuffer return 0;
	in this case the location is the location of the character at which
	the error was detected.
	They may also be called from any other callback called to report
	some parse event; in this the location is the location of the first
	of the sequence of characters that generated the event. */
    int GetCurrentLineNumber()
	{
		return( XML_GetCurrentLineNumber(m_parser));
	};

	int GetCurrentColumnNumber()
	{
		return( XML_GetCurrentColumnNumber(m_parser));
	};
	
	long GetCurrentByteIndex()
	{
		return( XML_GetCurrentByteIndex(m_parser));
	};
	
	/* Return the number of bytes in the current event.
	Returns 0 if the event is in an internal entity. */
	int GetCurrentByteCount()
	{
		return( XML_GetCurrentByteCount(m_parser));
	};

	/* Frees memory used by the parser. */
	void ParserFree()
	{
		XML_ParserFree(m_parser);
	};

	/* Returns a string describing the error. */
	const XML_LChar* ErrorString(int code)
	{
		return( XML_ErrorString(code));
	};

	/**
	 * Returns the entire XML document. This includes any nodes encouintered
	 * before the first element node of the buffer. All nodes parsed will be
	 * children of the returned XmlNode.
	 *
	 * @return Pseudo root nodeof the parsed buffer.
	 */
	const XmlNode* getDocument()
	{
		return( (XmlNode*)m_xmlDoc);
	};

	/**
	 * Retrieves the document element of the parsed buffer
	 *
	 * @return The document element node of the parsed buffer
	 */
	const XmlNode* getDocElement()
	{
		if( m_docElement == NULL)
		{
			throw( XmlException( XML_T("DocElement is NULL")));
		}
		return( (XmlNode*)m_docElement);
	};

	void initParser()
	{
		// Create the parser
		m_parser = XML_ParserCreate( NULL);

		// No elements yet
		m_elemCount = 0;

		// Invalidate the Document Element
		m_docElement = NULL;

		// Send the parser as argument to doc handlers
		XML_SetUserData(m_parser, (void *)this);

		// Setup document handlers
		SetElementHandler( XmlParser::StartElementHandler,
							XmlParser::EndElementHandler);
		SetCommentHandler( XmlParser::CommentHandler);
		SetDefaultHandler( XmlParser::DefaultHandler);

		// Set "null" handlers (stuff we don't care about)
		SetProcessingInstructionHandler( XmlParser::ProcessingInstructionHandler);
		SetCdataSectionHandler( XmlParser::StartCdataSectionHandler,
								XmlParser::EndCdataSectionHandler);
		SetCharacterDataHandler( XmlParser::CharacterDataHandler);

		m_currentCDataNode = NULL;
		m_currentCData = XmlString( XML_T(""));
	};



	/**
	 * Method which takes a configuration file name and
	 * returns the parsed document.
	 *
	 * @param ConfigFile The name of the configuration file to parse.
	 * @return The parsed document.
	 */
	const XmlNode *ReturnXMLDocument(const std::string &ConfigFile);

private:
	XmlDocument		*m_xmlDoc;
	XmlNode			*m_docElement;
	XML_Parser		m_parser;
	XmlNodeList		m_elemLifo;		// List of elements being processed
	unsigned long	m_elemCount;

	// CDATA processing helpers
	XmlCDataNode	*m_currentCDataNode;	// CDATA node in process
	XmlString		m_currentCData;			// CDATA value buffer
	

	/* atts is array of name/value pairs, terminated by 0;
	   names and values are 0 terminated. */
	static void StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
	static void EndElementHandler(void *userData,const XML_Char *name);

	/* s is not 0 terminated. */
	static void CharacterDataHandler(void *userData, const XML_Char *s, int len);
	
	/* target and data are 0 terminated */
	static void ProcessingInstructionHandler(void *userData, const XML_Char *target, const XML_Char *data);
	
	/* data is 0 terminated */
	static void CommentHandler(void *userData, const XML_Char *data);

	static void StartCdataSectionHandler(void *userData);
	static void EndCdataSectionHandler(void *userData);

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
	static void DefaultHandler(void *userData, const XML_Char *s, int len);
};

#endif	//XMLPARSER_H

