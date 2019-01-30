//******************************************************************************
// Description:
//  XML Node classes for use with the BEP XML Processor/Parser
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/XmlProcessor/XmlNode.cpp $
// 
// 4     10/26/06 10:04a Rwiersem
// Changes for the 06102701 core release:
// 
// Updated clear() to call m_nodeAttribs.clear(true) and
// m_childNodes.clear(true).
// 
// 3     4/13/06 3:47a Cward
// Added an overloaded ToString() method to the XmlCDataNode class
//
// 2     2/15/06 3:53p Bmeinke
// Added an overloaded ToString() method to the XmlCDataNode class
//
// 2     6/24/04 1:02a Cward
// Removed pound if 0 blocks. Updated the header block comments to the new
// standard.
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:36a Khykin
// Initial checkin
//
// 2     8/07/03 10:25a Khykin
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
// 17    10/21/02 11:39a Khykin
// Updated the deleting of nodes in the Clear method to check to determine
// if
// the node had previously been deleted.
//
// 16    8/14/02 10:08a Skypig13
// Updated operator= to check if assigning to self
//
// 15    6/03/02 4:40p Bmeinke
// Fixed bug in GetNextPathTag() where stand-alone node tags (i.e. no
// delimitters in path) were not properly parsed
//
// 14    5/30/02 12:49p Bmeinke
// Added getAttribute() method(s)
// Modified getChild() to allow navigation to child nodes using UNIX-like
// path strings (i.e. "Child1/Child2/Child3")
//
// 13    4/02/02 10:49p Skypig13
// Updates to fix assignment and deletion
//
// 12    2/05/02 12:18a Skypig13
// Removed some compiler warnings and optimized writeNode() for better
// performance
//
// 11    11/21/01 12:52p Brian.meinke
// Removed call to m_parentNode.eraseChild() in XmlNode destructor
//
// 10    11/20/01 2:51p Brian.meinke
// Make sure m_parentNode != NULL before calling m_parent->eraseNode() in
// destructor
//
// 9     11/20/01 2:29p Brian.meinke
// Added eraseChild() methods to simply remove a child node from the
// parent's XML node map
// Modified delChild() to also search the parent's attribute node map for
// the node to remove
//
// 8     10/25/01 9:43a Brian.meinke
// Fixed bug in getChild( const XML_Char *tag) where temp XmlString not
// initialized with tag passed as parameter
//
// 7     6/27/01 2:29p Brian.meinke
// Added Copy() method to perform a deep copy of a node (copies node and
// children)
// Added CreateNode() static method
//
// 6     6/26/01 11:04a Brian.meinke
// Changed operator= to insert (itr->first, itr->second)
//
// 5     6/26/01 10:16a Brian.meinke
// Removed copying of parent node in operator=
//
// 4     5/22/01 1:31p Brian.meinke
// Added delChild methods to remove child nodes
//
// 2     4/30/00 1:41p Kh
// Updated the include statements to not have relational references.
//
// 1     4/10/01 10:46a Admin
//
// 5     3/20/01 4:21p Brian.meinke
// Added getChild() method to XmlNode class: avoids having to explicitly
// go through XmlNodeMap
//
// 4     3/19/01 10:37a Brian.meinke
// Changed XmlNode constructor to tak a nodeType argument
// Automatically call addChild from constructor if parent is not NULL
//
// 3     3/09/01 4:48p Brian.meinke
// Overloaded methods taking 'const XmlString &' arguments to accept const
// XML_Char * also
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 4     12/19/00 10:35a Brian.meinke
// Changed output of Element Nodes
//
// 3     12/13/00 4:23p Brian.meinke
// Changed formatNode to add newline character before outputting a PI node
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
#include <stdio.h>

#include "XmlNode.h"
//==============================================================================
const XmlString XmlNode::m_ampStr( XML_T("&amp;"));
//==============================================================================
XmlNode::XmlNode( const XmlString &name, const XmlString &val, XmlNode* parent, XmlNodeType nodeType) :
m_nodeType( nodeType), m_parentNode( parent), m_nodeName( name), m_nodeValue( val),
m_pathSeperator( XML_T( '/'))
{
    if(m_parentNode == NULL)    m_indentLevel = 0;
    else                        m_parentNode->addChild( this);
}
//==============================================================================
XmlNode::XmlNode( const XML_Char *name, const XML_Char *val, XmlNode* parent, XmlNodeType nodeType) :
m_nodeType( nodeType), m_parentNode( parent), m_nodeName( name), m_nodeValue( val),
m_pathSeperator( XML_T( '/'))
{
    if(m_parentNode == NULL)    m_indentLevel = 0;
    else                        m_parentNode->addChild( this);
}
//==============================================================================
XmlNode::XmlNode( const XmlNode &copy) :
    m_pathSeperator( XML_T('/'))
{
    *this = copy;
}
//==============================================================================
XmlNode::~XmlNode( )
{
    clear();
}
//==============================================================================
/**
 * Get the name (or tag) of this node
 *
 * @return Node Name
 */
const XmlString& XmlNode::getName() const
{
    return( m_nodeName);
}
//==============================================================================
void XmlNode::setName( const XmlString &name)
{
    m_nodeName = name;
}
//==============================================================================
void XmlNode::setName( const XML_Char *name)
{
    XmlString tempName( name);
    setName( tempName);
}
//==============================================================================
/**
 * Get the text value of this node
 *
 * @return Node text
 */
const XmlString& XmlNode::getValue() const
{
    return( m_nodeValue);
}
//==============================================================================
void XmlNode::setValue(const XmlString &val)
{
    m_nodeValue = val;
}
//==============================================================================
void XmlNode::setValue(const XML_Char *val)
{
    XmlString   tempVal( val);
    setValue( tempVal);
}
//==============================================================================
/**
 * Gets the child node with the given tag
 *
 * @return Child Node List
 */
const XmlNode* XmlNode::getChild( const XmlString &tag) const
{
    XmlString               nextTag;
    XmlString::size_type    delimPos=0;
    const XmlNode           *nextNode = this;

    while(delimPos != tag.npos)
    {
        delimPos = GetNextPathTag( tag, nextTag, delimPos);
        nextNode = nextNode->getChildren().getNode( nextTag);
    }

    return( nextNode);

}
//==============================================================================
const XmlNode* XmlNode::getChild( const XML_Char *tag) const
{
    const XmlString tempTag( tag);
    return( getChild( tempTag));
}
//==============================================================================
/**
 * Get a list of child nodes belonging to this node
 *
 * @return Child Node List
 */
const XmlNodeMap& XmlNode::getChildren() const
{
    return( m_childNodes);
}
//==============================================================================
/**
 * Get the list of attributes belonging to this node
 *
 * @return List of node attributes (name=value pairs)
 */
const XmlNodeMap& XmlNode::getAttributes() const
{
    return( m_nodeAttribs);
}

/**
 * Gets the child ATTRIBUTE node with the given tag
 *
 * @param tag    Tag of the child node to return
 * @return Child attribute node with the given tag
 */
const XmlNode* XmlNode::getAttribute( const XmlString &tag) const
{
    return( getAttributes().getNode( tag));
}

/**
 * Gets the child ATTRIBUTE node with the given tag
 *
 * @param tag    Tag of the child node to return
 * @return Child attribute node with the given tag
 */
const XmlNode* XmlNode::getAttribute( const XML_Char *tag) const
{
    return( getAttributes().getNode( tag));
}
/**
 * Gets the child ATTRIBUTE node with the given tag
 *
 * @param tag    Tag of the child node to return
 * @return Child attribute node with the given tag
 */
const bool XmlNode::hasAttribute(const XML_Char *tag) const
{
    return (getAttributes().hasNode(tag));
}
/**
 * Gets the child ATTRIBUTE node with the given tag
 *
 * @param tag    Tag of the child node to return
 * @return Child attribute node with the given tag
 */
const bool XmlNode::hasAttribute(const XmlString &tag) const
{
    return (getAttributes().hasNode(tag));
}


//==============================================================================
/**
 * Adds a child node to this node
 *
 * @param child  Node to be added as a child
 *
 * @return The child node just added
 */
XmlNode* XmlNode::addChild( XmlNode *child)
{
    if(child != NULL)
    {
        child->m_parentNode = this;
        if(child->m_nodeType == ATTRIBUTE_NODE)
        {
            m_nodeAttribs.addNode( child);
        }
        else if(child->m_nodeType != DOCUMENT_NODE)
        {
            m_childNodes.addNode( child);
        }
        child->m_indentLevel = m_indentLevel + 1;
        child->FixIndent();
    }

    return( child);
}
//==============================================================================
/**
 * Adds a child node to this node
 *
 * @param tag    Tag for the new node
 * @param val    Value for the new node
 * @param type   Node type to create (Element, Attribute, etc)
 * @return Pointer to new node if successful, NULL if error
 * @see XmlNodeType
 */
XmlNode* XmlNode::addChild( const XmlString &tag, const XmlString &val,
                            const XmlNodeType &type)
{
    return( addChild( tag.c_str(), val.c_str(), type));
}
/**
 * Adds a child node to this node
 *
 * @param tag    Tag for the new node
 * @param val    Value for the new node
 * @param type   Node type to create (Element, Attribute, etc)
 * @return Pointer to new node if successful, NULL if error
 * @see XmlNodeType
 */
XmlNode* XmlNode::addChild( const XML_Char* tag, const XML_Char* val,
                            const XmlNodeType &type)
{
    XmlNode *child = XmlNode::CreateNode( tag, val, type);

    return( addChild( child));
}

/**
 * Creates a new node with no initial parent node
 *
 * @param tag    Tag for the new node
 * @param val    Value for the new node
 * @param type   Node type to create (Element, Attribute, etc)
 * @return Pointer to new node if successful, NULL if error
 * @see XmlNodeType
 */
XmlNode* XmlNode::CreateNode( const XML_Char* tag, const XML_Char* val,
                              const XmlNodeType &type)
{
    XmlNode *newNode;

    switch(type)
    {
    case ELEMENT_NODE:                  newNode = new XmlElement( tag, val, NULL);      break;
    case ATTRIBUTE_NODE:                newNode = new XmlAttribute( tag, val, NULL);    break;
    case TEXT_NODE:                     newNode = new XmlTextNode( val, NULL);          break;
    case COMMENT_NODE:                  newNode = new XmlComment( val, NULL);           break;
    case DOCUMENT_NODE:                 newNode = new XmlDocument( tag);                break;
    case CDATA_SECTION_NODE:            newNode = new XmlCDataNode( val, NULL);         break;
    case PROCESSING_INSTRUCTION_NODE:   newNode = new XmlPINode( tag, val, NULL);       break;
    case XML_DECL_NODE:                 newNode = new XmlDeclNode( val, NULL);          break;

    case ENTITY_REFERENCE_NODE:
    case ENTITY_NODE:
    case DOCUMENT_TYPE_NODE:
    case DOCUMENT_FRAGMENT_NODE:
    case NOTATION_NODE:
    case NODE_TYPE_NONE:
    default:                            newNode = NULL;                                 break;
    }

    return( newNode);
}

/**
 * Creates a new node with no initial parent node
 *
 * @param tag    Tag for the new node
 * @param val    Value for the new node
 * @param type   Node type to create (Element, Attribute, etc)
 * @return Pointer to new node if successful, NULL if error
 * @see XmlNodeType
 */
XmlNode* XmlNode::CreateNode( const XmlString &tag, const XmlString &val,
                              const XmlNodeType &type)
{
    return( XmlNode::CreateNode( tag.c_str(), val.c_str(), type));
}

/**
 * Copies this node and returns a pointer to the new node.
 * The new will be dynamically allocated. If the new node is not added
 * as a child to an existing node, you will need to delete it manually.
 *
 * @return Pointer to new node if successful, NULL if error
 */
XmlNode *XmlNode::Copy() const
{
    XmlNodeMapCItr  itr;
    XmlNode         *newNode;

    if (!m_nodeName.empty() && (m_nodeType != NODE_TYPE_NONE))
    {
        // Create the new node
        newNode = XmlNode::CreateNode( m_nodeName, m_nodeValue, m_nodeType);

        // Copy children nodes
        newNode->m_childNodes.clear();
        for(itr=m_childNodes.begin(); itr!=m_childNodes.end(); itr++)
        {
            // Get pointer to next chi;ld node to be copied
            const XmlNode *toCopy = itr->second;
            // Copy this child node
            XmlNode *newChild = toCopy->Copy();
            // MAke sure node is valid
            if(newChild == NULL)
            {
                XmlString   errMssg( XML_T("Unable to copy node '"));
                errMssg += itr->first;
                errMssg += XmlString( XML_T( "'"));

                throw( XmlException( errMssg));
            }
            // Add new child node to the newly created copy node
            newNode->addChild( newChild);
        }

        // Copy node attributes
        newNode->m_nodeAttribs.clear();
        for(itr=m_nodeAttribs.begin(); itr!=m_nodeAttribs.end(); itr++)
        {
            const XmlNode *toCopy = itr->second;
            XmlNode *newChild = toCopy->Copy();
            if(newChild == NULL)
            {
                XmlString   errMssg( XML_T("Unable to copy attribute '"));
                errMssg += itr->first;
                errMssg += XmlString( XML_T( "'"));

                throw( XmlException( errMssg));
            }
            newNode->m_nodeAttribs.addNode( newChild);
        }
    }
    else
    {
        char nodeType[5];
        sprintf(nodeType, "%d", m_nodeType);
        XmlString error = "Node Name: [" + m_nodeName + "], Node Type: [" + nodeType + "]";
        throw(XmlException(error));
    }
    return( newNode);
}

/**
 * Adjusts the 8ndent level of all child nodes after a Copy() operation
 */
void XmlNode::FixIndent()
{
    XmlNodeMapCItr  itr;

    // Copy children nodes
    for(itr=m_childNodes.begin(); itr!=m_childNodes.end(); itr++)
    {
        XmlNode *toFix = itr->second;
        toFix->m_indentLevel = m_indentLevel + 1;
    }
}

/**
 * Returns the next tag from the OS-style "node path" string pathString
 * starting AFTER pos
 *
 * @param pathString The "node path" string to read the next node tag from
 * @param nextTag    Tag of the next node in the pathString
 * @param pos        Position AFTER which to start looking for the next node tag
 * @return Position of the next delimitter AFTER pos or pathString.npos if
 *         no more node tags in pathString
 */
XmlString::size_type XmlNode::GetNextPathTag( const XmlString &pathString,
                                              XmlString &nextTag,
                                              XmlString::size_type pos) const
{
    XmlString::size_type    nextDelim = pathString.npos;

    // IF we have not reached the end of the string
    if((pos < pathString.length()) && (pos != pathString.npos))
    {
        // Look for the next delimitter after pos
        if((nextDelim=pathString.find( m_pathSeperator, pos+1)) != pathString.npos)
        {
            // Extract the tag for this sub-node
            if(pathString[ pos] == m_pathSeperator)    pos++;
            nextTag = pathString.substr( pos, (nextDelim-pos));
        }
        else
        {
            if(pathString[ pos] == m_pathSeperator)
            {
                // We're at the end of the string...read the last node tag
                nextTag = pathString.substr( pos+1, (pathString.length()-pos));
            }
            else
            {
                // We're at the end of the string...read the last node tag
                nextTag = pathString;
            }
        }
    }

    return( nextDelim);
}

/**
 * Removes the first child with the given tag
 *
 * @param tag    Tag of the node to be deleted
 */
void XmlNode::delChild( const XML_Char *tag)
{
    XmlNode *child = NULL;

    if((child = (XmlNode *)m_childNodes.getNode( tag)) != NULL)
    {
        delChild( (XmlNode*)child);
    }
    else if((child = (XmlNode *)m_nodeAttribs.getNode( tag)) != NULL)
    {
        delChild( (XmlAttribute*)child);
    }
}

/**
 * Removes the first child with the given tag
 *
 * @param child  Node to be deleted
 */
void XmlNode::delChild( const XmlNode *child)
{
    if(child != NULL)
    {
        // Remove the child from the child list
        m_childNodes.delNode( child->m_nodeName);

        // Free memory
        delete child;
        // Node no longer valid
        child = NULL;
    }
}

/**
 * Removes the first child with the given tag
 *
 * @param child  Node to be deleted
 */
void XmlNode::delChild( const XmlAttribute *child)
{
    if(child != NULL)
    {
        // Remove the child from the child list
        m_nodeAttribs.delNode( child->m_nodeName);

        // Free memory
        delete child;
        // Node no longer valid
        child = NULL;
    }
}

/**
 * Removes the child nodewith the given tag from this nodes child node map
 *
 * @param childTag Tag of the child node to remove from our child node map
 */
void XmlNode::eraseChild( const XML_Char *childTag)
{
    XmlNode *child = NULL;

    // If this tag represents a valid child
    if((child = (XmlNode *)m_childNodes.getNode( childTag)) != NULL)
    {
        // Remove the child from the child list
        eraseChild( child);
    }
    else if((child=(XmlNode *)m_nodeAttribs.getNode( childTag)) != NULL)
    {
        eraseChild( (XmlAttribute*)child);
    }
}

/**
 * Removes the given child node from this nodes child node map
 *
 * @param child  Child node to remove from our child node map
 */
void XmlNode::eraseChild( const XmlNode* child)
{
    if(child != NULL)
    {
        m_childNodes.delNode( child->getName());

        // Free memory
        delete child;

        // Node no longer valid
        child = NULL;
    }
}


/**
 * Removes the given child node from this nodes child node map
 *
 * @param child  Child node to remove from our child node map
 */
void XmlNode::eraseChild( const XmlAttribute* child)
{
    if(child != NULL)
    {
        m_nodeAttribs.delNode( child->getName());

        // Free memory
        delete child;

        // Node no longer valid
        child = NULL;
    }
}

//==============================================================================
XmlString& XmlNode::writeNode( XmlString &outStr) const
{
    register int ii, jj;

    outStr += XML_T("<");
    outStr += m_nodeName;

    // Out put the list of attributes first
    jj = m_nodeAttribs.size();
    for(ii=0; ii<jj; ii++)
    {
        m_nodeAttribs.getNode( ii)->writeNode( outStr);
    }

    // If no children nodes
    if((m_childNodes.size() == 0) && (m_nodeValue.size() == 0))
    {
        // Add proper termination
        outStr += XML_T("/");
    }
    outStr += XML_T(">");

    // Output the node text first
    outStr += m_nodeValue;

    // Output the list of child nodes next
    jj = m_childNodes.size();
    for(ii=0; ii<jj; ii++)
    {
        m_childNodes.getNode( ii)->writeNode( outStr);
    }

    // Add proper termination
    if((m_childNodes.size() != 0) || (m_nodeValue.size() != 0))
    {
        outStr += XML_T("</");
        outStr += m_nodeName;
        outStr += XML_T(">");
    }

    return( outStr);
}
//==============================================================================
XmlOStream &XmlNode::formatNode( XmlOStream &out) const
{
    register int ii, jj;
    XmlString   tabStr( XML_T(" "), m_indentLevel);
    bool        useLf = false;

    for(ii=0; ii<m_indentLevel; ii++)
    {
        out << XML_TAB;
    }
    out << XML_T("<") << m_nodeName;

    // Out put the list of attributes first
    jj = m_nodeAttribs.size();
    for(ii=0; ii<jj; ii++)
    {
        out << *(m_nodeAttribs.getNode( ii));
    }

    // If no children nodes
    if((m_childNodes.size() == 0) && (m_nodeValue.size() == 0))
    {
        // Add proper termination
        out << XML_T("/>") << endl;
    }
    else
    {
        out << XML_T(">")/* << endl*/;

        // Output the node text first
        if(m_nodeValue.size() > 0)
        {
            {
                out << convertString( m_nodeValue);
            }
        }
        // Add childern nodes after node text
        if(m_childNodes.size() > 0)
        {
            // Output the list of child nodes next
            switch(m_childNodes.getNode( 0)->m_nodeType)
            {
            case ELEMENT_NODE:
            case PROCESSING_INSTRUCTION_NODE:
            case COMMENT_NODE:
                out << endl;
                useLf = true;
                break;
            default:
                break;
            }

            jj = m_childNodes.size();
            for(ii=0; ii<jj; ii++)
            {
                out << *(m_childNodes.getNode( ii));
            }
        }
        // Add proper termination
        if(useLf)
        {
            for(ii=0; ii<m_indentLevel; ii++)
            {
                out << XML_TAB;
            }
        }
        out << XML_T("</") << m_nodeName << XML_T(">")  << endl;
    }
    return( out);
}
//==============================================================================
XmlString XmlNode::ToString() const
{
    register int ii, jj;
    XmlString   tabStr( XML_T(" "), m_indentLevel);
    bool        useLf = false;
    XmlString       docString;

    for(ii=0; ii<m_indentLevel; ii++)
    {
        docString += XML_TAB;
    }

    docString += XML_T("<");
    docString += m_nodeName;

    // Out put the list of attributes first
    jj = m_nodeAttribs.size();
    for(ii=0; ii<jj; ii++)
    {
        docString += XML_T(" ");
        docString += m_nodeAttribs.getNode( ii)->getName();
        docString += XML_T("=\"");
        docString += m_nodeAttribs.getNode( ii)->getValue();
        docString += XML_T("\"");
    }

    // If no children nodes
    if((m_childNodes.size() == 0) && (m_nodeValue.size() == 0))
    {   // Add proper termination
        docString += XML_T("/>");
        docString += XML_T("\n");
    }
    else
    {
        docString += XML_T(">")/* << endl*/;

        // Output the node text first
        if(m_nodeValue.size() > 0)
        {
            docString += convertString( m_nodeValue);
        }
        // Add childern nodes after node text
        if(m_childNodes.size() > 0)
        {
            // Output the list of child nodes next
            switch(m_childNodes.getNode( 0)->m_nodeType)
            {
            case ELEMENT_NODE:
            case PROCESSING_INSTRUCTION_NODE:
            case COMMENT_NODE:
                docString += XML_T("\n");
                useLf = true;
                break;
            default:
                break;
            }

            jj = m_childNodes.size();
            for(ii=0; ii<jj; ii++)
            {
                docString += m_childNodes.getNode( ii)->ToString();
            }
        }
        // Add proper termination
        if(useLf)
        {
            for(ii=0; ii<m_indentLevel; ii++)
            {
                docString += XML_TAB;
            }
        }
        docString += XML_T("</");
        docString += m_nodeName;
        docString += XML_T(">");
    }
    return( docString);
}
//==============================================================================
//==============================================================================
XmlNode& XmlNode::operator=( const XmlNode &rhs)
{
    if(this != &rhs)
    {
        m_nodeType = rhs.m_nodeType;
    //  m_parentNode = rhs.m_parentNode;
        m_nodeName = rhs.m_nodeName;
        m_nodeValue = rhs.m_nodeValue;

        m_childNodes = rhs.m_childNodes;
        m_nodeAttribs = rhs.m_nodeAttribs;
    }
    return( *this);
}
//==============================================================================
void XmlNode::clear()
{
#if 0
    XmlNodeMapCItr  itr;
    const XmlNode   *child;

    for( itr=m_nodeAttribs.begin(); itr!=m_nodeAttribs.end(); itr++)
    {
        if( (child=itr->second) != NULL)
        {
            delete child;
            child = NULL;
        }
    }
    for( itr=m_childNodes.begin(); itr!=m_childNodes.end(); itr++)
    {
        if( (child=itr->second) != NULL)
        {
            delete child;
            child = NULL;
        }
    }

    m_nodeAttribs.clear( false);
    m_childNodes.clear( false);
#else
    m_nodeAttribs.clear( true);
    m_childNodes.clear( true);
#endif
}
//==============================================================================


//==============================================================================
//==============================================================================
XmlDocument::XmlDocument( const XmlString &docElementName, XmlNode* parent)
: XmlNode( docElementName, XmlString(""), parent, DOCUMENT_NODE)
{
}
//==============================================================================
//==============================================================================
XmlElement::XmlElement( const XmlString &name, const XmlString &val, XmlNode *parent)
: XmlNode( name, val, parent, ELEMENT_NODE)
{
}
//==============================================================================
//==============================================================================
XmlAttribute::XmlAttribute( const XmlString &name, const XmlString &val, XmlNode *parent)
: XmlNode( name, val, parent, ATTRIBUTE_NODE)
{
};
//==============================================================================
XmlNode* XmlAttribute::addChild( XmlNode *child)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for Attribute Nodes"))));
}
//==============================================================================
XmlNode* XmlAttribute::addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for Attribute Nodes"))));
}
//==============================================================================
XmlString& XmlAttribute::writeNode( XmlString &outStr) const
{
    outStr += XML_T(" ");
    outStr += m_nodeName;
    outStr += XML_T("=\"");
    outStr += m_nodeValue;
    outStr += XML_T("\"");
    return( outStr);
}
//==============================================================================
XmlOStream& XmlAttribute::formatNode( XmlOStream &out) const
{
    out << XML_T(" ") << convertString( m_nodeName) << XML_T("=\"")
    << convertString( m_nodeValue) << XML_T("\"");
    return( out);
}
//==============================================================================
//==============================================================================
XmlTextNode::XmlTextNode( const XmlString &val, XmlNode *parent)
: XmlNode( XmlString(XML_T("[TEXT]")), val, parent, TEXT_NODE)
{
}
//==============================================================================
XmlNode* XmlTextNode::addChild( XmlNode *child)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for Text Nodes"))));
}
//==============================================================================
XmlNode* XmlTextNode::addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for Text Nodes"))));
}
//==============================================================================
XmlString& XmlTextNode::writeNode( XmlString &outStr) const
{
    outStr += m_nodeValue;
    return( outStr);
}
//==============================================================================
XmlOStream& XmlTextNode::formatNode( XmlOStream &out) const
{
    out << convertString( m_nodeValue);
    return( out);
}
//==============================================================================
//==============================================================================
XmlComment::XmlComment( const XmlString &val, XmlNode *parent)
: XmlNode( XmlString(XML_T("[COMMENT]")), val, parent, COMMENT_NODE)
{
}
//==============================================================================
XmlNode* XmlComment::addChild( XmlNode *child)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for Comment Nodes"))));
}
//==============================================================================
XmlNode* XmlComment::addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for Comment Nodes"))));
}
//==============================================================================
XmlString& XmlComment::writeNode( XmlString &outStr) const
{
    outStr += XML_T("<!--");
    outStr += m_nodeValue;
    outStr += XML_T("-->");
    return( outStr);
}
//==============================================================================
XmlOStream& XmlComment::formatNode( XmlOStream &out) const
{
    for(register int ii=0; ii<m_indentLevel; ii++)
    {
        out << XML_TAB;
    }
    out << XML_T("<!--") << convertString( m_nodeValue) << XML_T("-->") << endl;
    return( out);
}
//==============================================================================
//==============================================================================
XmlCDataNode::XmlCDataNode( const XmlString &val, XmlNode *parent) :
XmlNode( XmlString("![CDATA["), val, parent, CDATA_SECTION_NODE)
{
}
//==============================================================================
XmlNode* XmlCDataNode::addChild( XmlNode *child)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for CDATA Nodes"))));
}
//==============================================================================
XmlNode* XmlCDataNode::addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for CDATA Nodes"))));
}
//==============================================================================
XmlString& XmlCDataNode::writeNode( XmlString &outStr) const
{
    outStr += XML_T("<![CDATA[");
    outStr += m_nodeValue;
    outStr += XML_T("]]>");
    return( outStr);
}
//==============================================================================
XmlOStream& XmlCDataNode::formatNode( XmlOStream &out) const
{
    for(register int ii=0; ii<m_indentLevel; ii++)
    {
        out << XML_TAB;
    }
    out << XML_T("<![CDATA[") << convertString( m_nodeValue) << XML_T("]]>") << endl;
    return( out);
}
//==============================================================================
XmlString XmlCDataNode::ToString() const
{
    XmlString   retVal("");
    for(register int ii=0; ii<m_indentLevel; ii++)
    {
        retVal += XML_TAB;
    }
    retVal += XML_T("<![CDATA[") + convertString( m_nodeValue) + XML_T("]]>\n");
    return( retVal);
}
//==============================================================================
//==============================================================================
XmlPINode::XmlPINode( const XmlString &name, const XmlString &val, XmlNode *parent) :
XmlNode( name, val, parent, PROCESSING_INSTRUCTION_NODE)
{
}
//==============================================================================
XmlNode* XmlPINode::addChild( XmlNode *child)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for PI Nodes"))));
}
//==============================================================================
XmlNode* XmlPINode::addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for PI Nodes"))));
}
//==============================================================================
XmlString& XmlPINode::writeNode( XmlString &outStr) const
{
    outStr += XML_T("<?");
    outStr += m_nodeName;
    outStr += XmlString( XML_T(" "));
    outStr += m_nodeValue;
    outStr += XML_T("?>");
    return( outStr);
}
//==============================================================================
XmlOStream& XmlPINode::formatNode( XmlOStream &out) const
{
    for(register int ii=0; ii<m_indentLevel; ii++)
    {
        out << XML_TAB;
    }
    out << XML_T("<?") << convertString( m_nodeName) << XML_T(" ") << convertString( m_nodeValue) << XML_T("?>") << endl;
    return( out);
}
//==============================================================================
//==============================================================================
XmlDeclNode::XmlDeclNode( const XmlString &val, XmlNode *parent) :
XmlPINode( XmlString( XML_T("xml")), val, parent)
{
    m_nodeType = XML_DECL_NODE;
}
//==============================================================================
XmlNode* XmlDeclNode::addChild( XmlNode *child)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for XML Decl Nodes"))));
}
//==============================================================================
XmlNode* XmlDeclNode::addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type)
{
    throw( XmlException( XmlString( XML_T("addChild not supported for XML Decl Nodes"))));
}
//==============================================================================

