//******************************************************************************
//
// FILE:    XmlNode
//     $Archive:: /Ccrt/Source/Core/ServicesLayer/Include/XmlNode.h           $
//     $Author: Rwiersem $
//     $Date: 10/26/06 9:24a $
//
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Include/XmlNode.h $
// 
// 3     10/26/06 9:24a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed DLL_CLASS from the class names.
// 
// 2     4/13/06 3:39a Cward
// Added an overloaded ToString() method to the XmlCDataNode class
//
// 2     2/15/06 3:53p Bmeinke
// Added an overloaded ToString() method to the XmlCDataNode class
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
//     Revision 1.1.1.1  2003/02/15 00:05:49  tbochene
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
// 7     8/14/02 10:33a Skypig13
// Added destructors
//
// 6     5/30/02 12:49p Bmeinke
// Added getAttribute() method(s)
// Modified getChild() to allow navigation to child nodes using UNIX-like
// path strings (i.e. "Child1/Child2/Child3")
//
// 5     11/20/01 2:29p Brian.meinke
// Added eraseChild() methods to simply remove a child node from the
// parent's XML node map
// Modified delChild() to also search the parent's attribute node map for
// the node to remove
//
// 4     11/01/01 10:59a Brian.meinke
// Added getParent() method
// Added javadoc comments for more methods and data members
//
// 3     6/27/01 2:30p Brian.meinke
// Added Copy() method to perform a deep copy of a node (copies node and
// children)
// Added CreateNode() static method
// Added FixIndent() method to fix-up indentation levels after adding a
// pre-existing node as a child
//
// 2     5/22/01 1:31p Brian.meinke
// Added delChild methods to remove child nodes
//
// 1     4/10/01 10:46a Admin
//
// 8     3/20/01 4:21p Brian.meinke
// Added getChild() method to XmlNode class: avoids having to explicitly
// go through XmlNodeMap
//
// 7     3/19/01 10:37a Brian.meinke
// Changed XmlNode constructor to tak a nodeType argument
// Automatically call addChild from constructor if parent is not NULL
//
// 6     3/19/01 9:04a Brian.meinke
// Removed the m_document data member from XmlNode
// Moved all method implementation code to the CPP file
//
// 5     3/09/01 4:49p Brian.meinke
// Overloaded methods taking 'const XmlString &' arguments to accept const
// XML_Char * also
//
// 4     3/06/01 9:03a Tim.bochenek
// Removed Watcom preprocessor that excluded exceptions
//
// 3     3/06/01 8:59a Tim.bochenek
// Removed Watcom preprocessor that excluded exceptions
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 4     12/18/00 10:48a Brian.meinke
// Changed '#ifdef WATCOM' to  '#ifdef __WATCOMC__'
//
// 3     12/13/00 4:24p Brian.meinke
// Made XmlParser a friend class (in order to change m_indentLevel)
// Fixed PI node output to add space between the PI node tag and value
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
#ifndef XMLNODE_H
#define XMLNODE_H
//==============================================================================
class XmlNode;
class XmlElement;
class XmlAttribute;
class XmlTextNode;
class XmlComment;
class XmlDocument;
class XmlParser;
//==============================================================================
#include "BepXmlUtils.h"
//==============================================================================
typedef enum {
    NODE_TYPE_NONE              = 0,
    /**
     * XML Element capable of having children nodes
     */
    ELEMENT_NODE                = 1,
    /**
     * XML Attribute node (no children)
     */
    ATTRIBUTE_NODE              = 2,
    /**
     * XML Text Node (no children)
     */
    TEXT_NODE                   = 3,
    /**
     * XML CDATA Node (no children)
     */
    CDATA_SECTION_NODE          = 4,
    ENTITY_REFERENCE_NODE       = 5,
    ENTITY_NODE                 = 6,
    PROCESSING_INSTRUCTION_NODE = 7,
    /**
     * XML Comment Node (no children)
     */
    COMMENT_NODE                = 8,
    DOCUMENT_NODE               = 9,
    DOCUMENT_TYPE_NODE          = 10,
    DOCUMENT_FRAGMENT_NODE      = 11,
    NOTATION_NODE               = 12,
    XML_DECL_NODE               = 13
}  XmlNodeType;
//==============================================================================
class XmlNode
{
    friend class XmlParser;

public:
    XmlNode( const XML_Char *name, const XML_Char *val, XmlNode* parent=NULL, XmlNodeType nodeType=ELEMENT_NODE);
    XmlNode( const XmlString &name, const XmlString &val, XmlNode* parent=NULL, XmlNodeType nodeType=ELEMENT_NODE);
    XmlNode( const XmlNode &copy);
    virtual ~XmlNode( );

    const static XmlString m_ampStr;

    /**
     * Get the name (or tag) of this node
     *
     * @return Node Name
     */
    const XmlString& getName() const;
    /**
     * Get the name (or tag) of this node
     *
     * @return Node Name
     */
    void setName( const XmlString &name);
    /**
     * Get the name (or tag) of this node
     *
     * @return Node Name
     */
    void setName( const XML_Char *name);

    /**
     * Get the text value of this node
     *
     * @return Node text
     */
    const XmlString& getValue() const;
    /**
     * Get the text value of this node
     *
     * @return Node text
     */
    void setValue( const XmlString &val);
    /**
     * Get the text value of this node
     *
     * @return Node text
     */
    void setValue( const XML_Char *val);

    /**
     * Returns a pointer to this nodes parent. A dosument's root node has
     * a NULL parent
     *
     * @return A pointer to this nodes parent. A dosument's root node has
     *         a NULL parent.
     */
    const XmlNode *getParent() {
        return( m_parentNode);
    };

    /**
     * Gets the child node with the given tag
     *
     * @param tag    Tag of the child node to return
     * @return Child Node List
     */
    const XmlNode* getChild( const XmlString &tag) const;

    /**
     * Gets the child node with the given tag
     *
     * @param tag    Tag of the child node to return
     * @return Child Node List
     */
    const XmlNode* getChild( const XML_Char *tag) const;

    /**
     * Get a list of child nodes belonging to this node
     *
     * @return Child Node List
     */
    const XmlNodeMap&   getChildren() const;

    /**
     * Get the list of attributes belonging to this node
     *
     * @return List of node attributes (name=value pairs)
     */
    const XmlNodeMap&   getAttributes() const;

    /**
     * Gets the child ATTRIBUTE node with the given tag
     *
     * @param tag    Tag of the child node to return
     * @return Child attribute node with the given tag
     */
    const XmlNode* getAttribute( const XmlString &tag) const;

    /**
     * Gets the child ATTRIBUTE node with the given tag
     *
     * @param tag    Tag of the child node to return
     * @return Child attribute node with the given tag
     */
    const XmlNode* getAttribute( const XML_Char *tag) const;

    /**
     * Adds a child node to this node
     *
     * @param child  Node to be added as a child
     *
     * @return The child node just added
     */
    virtual XmlNode *addChild( XmlNode *child);

    /**
     * Adds a child node to this node
     *
     * @param tag    Tag for the new node
     * @param val    Value for the new node
     * @param type   Node type to create (Element, Attribute, etc)
     * @return The child node just added
     * @see XmlNodeType
     */
    virtual XmlNode *addChild( const XmlString &tag,
                                const XmlString &val,
                                const XmlNodeType &type);

    /**
     * Adds a child node to this node
     *
     * @param tag    Tag for the new node
     * @param val    Value for the new node
     * @param type   Node type to create (Element, Attribute, etc)
     * @return The child node just added
     * @see XmlNodeType
     */
    virtual XmlNode *addChild( const XML_Char *tag,
                                const XML_Char *val,
                                const XmlNodeType &type);

    /**
     * Creates a new node with no initial parent node
     *
     * @param tag    Tag for the new node
     * @param val    Value for the new node
     * @param type   Node type to create (Element, Attribute, etc)
     * @return Pointer to new node if successful, NULL if error
     * @see XmlNodeType
     */
    static XmlNode* CreateNode( const XML_Char* tag, const XML_Char* val,
                                const XmlNodeType &type);

    /**
     * Creates a new node with no initial parent node
     *
     * @param tag    Tag for the new node
     * @param val    Value for the new node
     * @param type   Node type to create (Element, Attribute, etc)
     * @return Pointer to new node if successful, NULL if error
     * @see XmlNodeType
     */
    static XmlNode* CreateNode( const XmlString &tag, const XmlString &val,
                                const XmlNodeType &type);

    /**
     * Copies this node and returns a pointer to the new node.
     * The new will be dynamically allocated. If the new node is not added
     * as a child to an existing node, you will need to delete it manually.
     *
     * @return Pointer to new node if successful, NULL if error
     */
    XmlNode* Copy() const;

    /**
     * Removes the first child with the given tag
     *
     * @param tag    Tag of the node to be deleted
     */
    virtual void delChild( const XML_Char *tag);

    /**
     * Removes the first child with the given tag
     *
     * @param child  Node to be deleted
     */
    virtual void delChild( const XmlNode *child);

    /**
     * Removes the first child with the given tag
     *
     * @param child  Node to be deleted
     */
    virtual void delChild( const XmlAttribute *child);

    /**
     * Removes the child nodewith the given tag from this nodes child node map
     *
     * @param childTag Tag of the child node to remove from our child node map
     */
    virtual void eraseChild( const XML_Char *childTag);

    /**
     * Removes the given child node from this nodes child node map
     *
     * @param child  Child node to remove from our child node map
     */
    virtual void eraseChild( const XmlNode* child);

    /**
     * Removes the given child node from this nodes child node map
     *
     * @param child  Child node to remove from our child node map
     */
    virtual void eraseChild( const XmlAttribute* child);

    /**
     * Clears all children nodes from this node object
     */
    void clear();

    XmlNode& operator=( const XmlNode &rhs);

    /**
     * Stream insertion operator. Used to stream an Xml node out.
     *
     * @param out    Stream into which the Xml node will be streamed
     *
     * @param node   Node to be streamed out
     *
     * @return stream object into which the node was streamed
     */
    friend XmlOStream& operator<<( XmlOStream &out, const XmlNode &node)
    {
        return( node.formatNode( out));
    };

    /**
     * Returns the node type
     *
     * @return Node type for current node
     * @see XmlNodeType
     */
    XmlNodeType     getNodeType() const     { return( m_nodeType);  };

    /**
     * Appends the current node's string representation to the given
     * outStr argument
     *
     * @param outStr String to append the current node to
     * @return The string argument passed in (outStr)
     */
    virtual XmlString& writeNode( XmlString &outStr) const;

    /**
     * Writes the current node's "pretty" string representation to the
     * given stream. "Pretty" means that newlines and tabs are inserted
     * to make the output look "pretty".
     *
     * @param out    Stream to write the node to
     * @return The stream argument passed in (out)
     */
    virtual XmlOStream& formatNode( XmlOStream &out) const;

    /**
     * Writes the current node's "pretty" string representation to an
     * XML string. "Pretty" means that newlines and tabs are inserted
     * to make the output look "pretty".
     *
     * @return The current node's "pretty" string representation
     */
    virtual XmlString ToString() const;

    /**
     * Replaces special characters (&,<,>, etc) in a nodes string representation with
     * their special representations.
     *
     * @param xmlStr String to convert
     * @return The string argument passed in (xmlStr)
     */
    inline XmlString convertString( const XmlString &xmlStr) const
    {
        if( xmlStr == XmlNode::m_ampStr)
        {
            return( XmlString( XML_T("&")) );
        }
        else
        {
            return( xmlStr);
        }

    };

protected:

    /**
     * Adjusts the indent level of all child nodes after a Copy() operation
     */
    void FixIndent();

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
                                                  XmlString::size_type pos) const;

    /**
     * Indent level for this node. Used when "pretty" printing the node
     */
    int             m_indentLevel;

    /**
     * Node's type (Element, Attribute, etc)
     *
     * @see XmlNodeType
     */
    XmlNodeType     m_nodeType;

    /**
     * List of children nodes (no attributes)
     *
     * @see XmlNodeMap
     */
    XmlNodeMap      m_childNodes;

    /**
     * List of attributes nodes belonging to this node
     *
     * @see XmlNodeMap
     */
    XmlNodeMap      m_nodeAttribs;  // List of Node Attributes

    /**
     * Parent node for this node. This will be NULL for a document's
     * root node
     *
     * @see XmlNode
     */
    XmlNode         *m_parentNode;

    /**
     * Name of this node
     *
     * @see XmlString
     */
    XmlString       m_nodeName;

    /**
     * Node's text value
     *
     * @see XmlString
     */
    XmlString       m_nodeValue;

    /**
     * Character used to seperate node tags in a "node path". A node
     * path is a OS-style path string which specifes the sequence
     * of tags to a given node (i.e. "node1/child1/Child2")
     */
    const XML_Char  m_pathSeperator;
};
//==============================================================================
class XmlDocument : public XmlNode
{
public:
    XmlDocument( const XmlString &docElementName, XmlNode* parent=NULL);
    virtual ~XmlDocument(void)
    {
    }
};
//==============================================================================
class XmlElement : public XmlNode
{
public:
    XmlElement( const XmlString &name, const XmlString &val, XmlNode* parent=NULL);
    virtual ~XmlElement(void)
    {
    }
};
//==============================================================================
class XmlAttribute : public XmlNode
{
public:
    XmlAttribute( const XmlString &name, const XmlString &val, XmlNode* parent=NULL);
    virtual ~XmlAttribute(void)
    {
    }
    XmlNode* addChild( XmlNode *child);
    XmlNode* addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type);
    virtual XmlString& writeNode( XmlString &outStr) const;
    virtual XmlOStream& formatNode( XmlOStream &out) const;
};
//==============================================================================
class XmlTextNode : public XmlNode
{
public:
    XmlTextNode( const XmlString &val, XmlNode* parent=NULL);
    virtual ~XmlTextNode(void)
    {
    }
    XmlNode* addChild( XmlNode *child);
    XmlNode* addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type);
    virtual XmlString& writeNode( XmlString &outStr) const;
    virtual XmlOStream& formatNode( XmlOStream &out) const;
};
//==============================================================================
class XmlComment : public XmlNode
{
public:
    XmlComment( const XmlString &val, XmlNode* parent=NULL);
    virtual ~XmlComment(void)
    {
    }
    XmlNode* addChild( XmlNode *child);
    XmlNode* addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type);
    virtual XmlString& writeNode( XmlString &outStr) const;
    virtual XmlOStream& formatNode( XmlOStream &out) const;
};
//==============================================================================
class XmlCDataNode : public XmlNode
{
public:
    XmlCDataNode( const XmlString &val, XmlNode* parent=NULL);
    virtual ~XmlCDataNode(void)
    {
    }
    XmlNode* addChild( XmlNode *child);
    XmlNode* addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type);
    virtual XmlString& writeNode( XmlString &outStr) const;
    virtual XmlOStream& formatNode( XmlOStream &out) const;
    virtual XmlString ToString() const;
};
//==============================================================================
class XmlPINode : public XmlNode
{
public:
    XmlPINode( const XmlString &name, const XmlString &val, XmlNode* parent=NULL);
    virtual ~XmlPINode(void)
    {
    }
    XmlNode* addChild( XmlNode *child);
    XmlNode* addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type);
    virtual XmlString& writeNode( XmlString &outStr) const;
    virtual XmlOStream& formatNode( XmlOStream &out) const;
};
//==============================================================================
class XmlDeclNode : public XmlPINode
{
public:
    XmlDeclNode( const XmlString &val, XmlNode* parent=NULL);
    virtual ~XmlDeclNode(void)
    {
    }
    XmlNode* addChild( XmlNode *child);
    XmlNode* addChild( const XmlString &tag,const XmlString &val, const XmlNodeType &type);
};
//==============================================================================
#endif  //XMLNODE_H


