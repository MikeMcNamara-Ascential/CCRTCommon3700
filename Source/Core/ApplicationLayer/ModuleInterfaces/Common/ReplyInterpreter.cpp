//*************************************************************************
// Description:
//  Reply Interpreter for module commuinications.  This class provides a grammar
//  for translating and converting raw module data into values usable by the
//  TestComponents and other portions of the module interfaces.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Common/ReplyInterpreter.cpp $
// 
// 11    10/30/07 1:25p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added a UINT8 vectorEvaluate() method.
// - Added a UINT16 vectorEvaluate() method.
// - Added a float vectorEvaluate() method.
// - Added ucharVectorEvaluate().
// - Added a UINT16 uintVectorEvaluate() method.
// - Added a float fltVectorEvaluate() method.
// - Added ucharVectorProcessGrammar().
// - Added a UINT16 uintVectorProcessGrammar() method.
// - Added a float fltVectorProcessGrammar() method.
// 
// 10    1/17/07 2:48p Rwiersem
// Changes for the 07011901 core release:
// 
// - Set conditionsMet to false each time through the subnode loop before
// checking the conditions.
// 
// 9     9/22/06 11:09a Rwiersem
// Changes for the 06092201 core release:
//     - Added strProcessGrammarCharsArray()
//     - Added isElementCharsArray()
//     - Added isNodeCompare()
// 
// 8     5/11/05 10:01p Cward
// Added Gpattison changes of 5/09/05
//
// 7     5/09/05 5:00p Gpattison
// Added Vector and variable length vector support
//
// 7     3/03/05 11:46p Cward
// Updated for 05030302. Needed quick fix for new licensing
//
// 6     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 5     2/16/05 9:12p Cward
// Updated intEvaluate, intProcessGrammar,
//
// 4     6/09/04 10:41p Cward
// Updated header block comments
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 3     11/13/03 6:03p Derickso
// Updated to allow string substitutions.
//
// 2     10/28/03 6:46a Derickso
// Code cleanup.
// Add appropriate VSS header information.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ReplyInterpreter.h"
#include <stack>
#include <limits>
//-------------------------------------------------------------------------


ReplyInterpreter::ReplyInterpreter(const XmlNode * node)
throw(XmlException)
: IReplyInterpreter(node)
{

    if(node == NULL)
    {
        throw XmlException("null root XmlNode");
    }

    try
    {
        m_rootNodePtr = node->getChildren().getNode(0);
    }
    catch(XmlException &ex)
    {
        throw XmlException(
                          "Error constructing ReplyInterpreter from given node: no children");
    }
}

ReplyInterpreter::ReplyInterpreter()
:IReplyInterpreter()
{
    // Nothing
}

ReplyInterpreter::~ReplyInterpreter()
{
}

void ReplyInterpreter::setConfigNode(const XmlNode *newNode)
throw(XmlException)
{
    if(newNode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(newNode->getChildren().size() == (unsigned int)0)
    {
        throw XmlException("root node must have children");
    }

    // Save the node pointer for use in the next interpretation
    m_rootNodePtr = newNode->getChildren().getNode(0);
}

bool ReplyInterpreter::isInterpretTypeBool()
throw(XmlException)
{
    try
    {
        return(compareNodeToName(m_rootNodePtr, string("boolean"), 7));
    }
    catch(XmlException &err)
    {
        throw XmlException("Error processing Interpretation root node type: " +
                           err.Reason());
    }

    return(false);
}

bool ReplyInterpreter::isInterpretTypeEnum()
throw(XmlException)
{
    try
    {
        return(compareNodeToName(m_rootNodePtr, string("enumerated"), 10));
    }
    catch(XmlException &err)
    {
        throw XmlException("Error processing Interpretation root node type: " +
                           err.Reason());
    }

    return(false);
}

bool ReplyInterpreter::isInterpretTypeStr()
throw(XmlException)
{
    try
    {
        return(compareNodeToName(m_rootNodePtr, string("string"), 6));
    }
    catch(XmlException &err)
    {
        throw XmlException("Error processing Interpretation root node type: " +
                           err.Reason());
    }

    return(false);
}

bool ReplyInterpreter::isInterpretTypeUnsignedInt()
throw(XmlException)
{
    try
    {
        return(compareNodeToName(m_rootNodePtr, string("integer"), 7) &&
               isIntegerUnsigned(m_rootNodePtr));
    }
    catch(XmlException &err)
    {
        throw XmlException("Error processing Interpretation root node type: " +
                           err.Reason());
    }

    return(false);
}

bool ReplyInterpreter::isInterpretTypeSignedInt()
throw(XmlException)
{
    try
    {
        return(compareNodeToName(m_rootNodePtr, string("integer"), 7) &&
               isIntegerSigned(m_rootNodePtr));
    }
    catch(XmlException &err)
    {
        throw XmlException("Error processing Interpretation root node type: " +
                           err.Reason());
    }

    return(false);
}

bool ReplyInterpreter::isInterpretTypeFlt()
throw(XmlException)
{
    try
    {
        return(compareNodeToName(m_rootNodePtr, string("float"), 5));
    }
    catch(XmlException &err)
    {
        throw XmlException("Error processing Interpretation root node type: " +
                           err.Reason());
    }

    return(false);
}

bool ReplyInterpreter::isInterpretTypeVector()
throw(XmlException)
{
    try
    {
        return(compareNodeToName(m_rootNodePtr, string("vector"), 6));
    }
    catch (XmlException &err)
    {
        throw XmlException("Error processing Interpretation root node type: " +
                           err.Reason());
    }

    return(false);
}

UINT16 ReplyInterpreter::enumEvaluate (SerialString_t &byteArray)
throw(XmlException)
{
    return(enumEvaluate(m_rootNodePtr,byteArray));
}

bool ReplyInterpreter::boolEvaluate (SerialString_t &byteArray)
throw(XmlException)
{
    return(boolEvaluate(m_rootNodePtr,byteArray));
}

string ReplyInterpreter::strEvaluate (SerialString_t &byteArray)
throw(XmlException)
{
    return(strEvaluate(m_rootNodePtr,byteArray));
}

UINT32 ReplyInterpreter::intUnsignedEvaluate (SerialString_t &byteArray)
throw(XmlException)
{
    return((UINT32) intEvaluate(m_rootNodePtr,byteArray));
}

INT32  ReplyInterpreter::intSignedEvaluate (SerialString_t &byteArray)
throw(XmlException)
{
    return((INT32) intEvaluate(m_rootNodePtr,byteArray));
}

double ReplyInterpreter::fltEvaluate (SerialString_t &byteArray)
throw(XmlException)
{
    return(fltEvaluate(m_rootNodePtr,byteArray));
}

void ReplyInterpreter::vectorEvaluate (SerialString_t &byteArray,vector<bool> &vectorStorage)
throw(XmlException)
{
    return(boolVectorEvaluate(m_rootNodePtr,byteArray,vectorStorage));
}

void ReplyInterpreter::vectorEvaluate (SerialString_t &byteArray,vector<string> &vectorStorage)
throw(XmlException)
{
    return(strVectorEvaluate(m_rootNodePtr,byteArray,vectorStorage));
}

void ReplyInterpreter::vectorEvaluate (SerialString_t &byteArray,vector<UINT8> &vectorStorage)
throw(XmlException)
{
    return(ucharVectorEvaluate(m_rootNodePtr,byteArray,vectorStorage));
}

void ReplyInterpreter::vectorEvaluate (SerialString_t &byteArray,vector<UINT32> &vectorStorage)
throw(XmlException)
{
    return(uintVectorEvaluate(m_rootNodePtr,byteArray,vectorStorage));
}

void ReplyInterpreter::vectorEvaluate (SerialString_t &byteArray,vector<UINT16> &vectorStorage)
throw(XmlException)
{
    return(uintVectorEvaluate(m_rootNodePtr,byteArray,vectorStorage));
}

void ReplyInterpreter::vectorEvaluate (SerialString_t &byteArray,vector<INT32> &vectorStorage)
throw(XmlException)
{
    return(intVectorEvaluate(m_rootNodePtr,byteArray,vectorStorage));
}

void ReplyInterpreter::vectorEvaluate (SerialString_t &byteArray,vector<double> &vectorStorage)
throw(XmlException)
{
    return(fltVectorEvaluate(m_rootNodePtr,byteArray,vectorStorage));
}

void ReplyInterpreter::vectorEvaluate (SerialString_t &byteArray,vector<float> &vectorStorage)
throw(XmlException)
{
    return(fltVectorEvaluate(m_rootNodePtr,byteArray,vectorStorage));
}

UINT16 ReplyInterpreter::enumEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    if(!compareNodeToName(xmlnode, string("ENUMERATED"), 10))
    {
        throw XmlException("Expected Enumerated Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if(xmlnode->getChildren().size() == 0)
        {
            throw XmlException("Enumerated Grammar must include at least one Enum child");
        }

        /**
         * process bytes with enumeration grammar
         */
        return(enumProcessGrammar(xmlnode, byteArray));
    }
    catch(XmlException &err)
    {
        throw XmlException("Enumerated Grammar Parse Error: " + err.Reason());
    }

    return((UINT16)0);
}

bool ReplyInterpreter::boolEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    if(!compareNodeToName(xmlnode, string("BOOLEAN"), 7))
    {
        throw XmlException("Expected Boolean Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if(xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Boolean Grammar must include at least one top-level boolean operation");
        }

        if(xmlnode->getChildren().size() > (unsigned int)1)
        {
            throw XmlException("Boolean Grammar cannot include more than one top-level boolean operation");
        }

        /**
         * process bytes with boolean grammar
         */
        if(boolProcessGrammar ((xmlnode->getChildren()).getNode(0), byteArray))
        {
            return(true);
        }
    }
    catch(XmlException &err)
    {
        throw XmlException("Boolean Grammar Parse Error: " + err.Reason());
    }

    return(false);
}

string ReplyInterpreter::strEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    if(!compareNodeToName(xmlnode, string("STRING"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if(xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("String Grammar must include at least one Substring child");
        }

        /**
         * process bytes with string grammar
         */
        return(strProcessGrammar (xmlnode, byteArray));
    }
    catch(XmlException &err)
    {
        throw XmlException("String Grammar Parse Error: " + err.Reason());
    }

    return("");
}

UINT32 ReplyInterpreter::intEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    if(!compareNodeToName(xmlnode, string("INTEGER"), 7))
    {
        throw XmlException("Expected Integer Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if(xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Integer Grammar must include at least one Byte child");
        }

        /**
         * process bytes with integer grammar
         */
        return(intProcessGrammar(xmlnode, byteArray));
    }
    catch(XmlException &err)
    {
        throw XmlException("Integer Grammar Parse Error: " + err.Reason());
    }

    return((INT32)0);
}

double ReplyInterpreter::fltEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    if(!compareNodeToName(xmlnode, string("float"), 5))
    {
        throw XmlException("Expected Float Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if(xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Float Grammar must include one Integer child");
        }

        // sanity checks on root node's children
        if(xmlnode->getChildren().size() > (unsigned int)1)
        {
            throw XmlException("Float Grammar cannot include more than one Integer child");
        }

        /**
         * process integer bytes as a float
         */
        return(fltProcessGrammar(xmlnode, byteArray));
    }
    catch(XmlException &err)
    {
        throw XmlException("Float Grammar Parse Error: " + err.Reason());
    }

    return(0.0);
}

void ReplyInterpreter::boolVectorEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray,vector<bool> &vectorStorage)
throw(XmlException)
{
    if (!compareNodeToName(xmlnode, string("VECTOR"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if (xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Vector Grammar must include at least one Subvector child");
        }
        /**
         * process bytes with string grammar
         */
        return(boolVectorProcessGrammar(xmlnode, byteArray,vectorStorage));
    }
    catch (XmlException &err)
    {
        throw XmlException("Vector Grammar Parse Error: " + err.Reason());
    }
}

void ReplyInterpreter::strVectorEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray,vector<string> &vectorStorage)
throw(XmlException)
{
    if (!compareNodeToName(xmlnode, string("VECTOR"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if (xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Vector Grammar must include at least one Subvector child");
        }
        /**
         * process bytes with string grammar
         */
        return(strVectorProcessGrammar(xmlnode, byteArray,vectorStorage));
    }
    catch (XmlException &err)
    {
        throw XmlException("Vector Grammar Parse Error: " + err.Reason());
    }
}

void ReplyInterpreter::ucharVectorEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray,vector<UINT8> &vectorStorage)
throw(XmlException)
{
    if (!compareNodeToName(xmlnode, string("VECTOR"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if (xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Vector Grammar must include at least one Subvector child");
        }
        /**
         * process bytes with string grammar
         */
        return(ucharVectorProcessGrammar(xmlnode, byteArray,vectorStorage));
    }
    catch (XmlException &err)
    {
        throw XmlException("Vector Grammar Parse Error: " + err.Reason());
    }
}

void ReplyInterpreter::uintVectorEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray,vector<UINT32> &vectorStorage)
throw(XmlException)
{
    if (!compareNodeToName(xmlnode, string("VECTOR"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if (xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Vector Grammar must include at least one Subvector child");
        }
        /**
         * process bytes with string grammar
         */
        return(uintVectorProcessGrammar(xmlnode, byteArray,vectorStorage));
    }
    catch (XmlException &err)
    {
        throw XmlException("Vector Grammar Parse Error: " + err.Reason());
    }
}

void ReplyInterpreter::uintVectorEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray,vector<UINT16> &vectorStorage)
throw(XmlException)
{
    if (!compareNodeToName(xmlnode, string("VECTOR"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if (xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Vector Grammar must include at least one Subvector child");
        }
        /**
         * process bytes with string grammar
         */
        return(uintVectorProcessGrammar(xmlnode, byteArray,vectorStorage));
    }
    catch (XmlException &err)
    {
        throw XmlException("Vector Grammar Parse Error: " + err.Reason());
    }
}

void ReplyInterpreter::intVectorEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray,vector<INT32> &vectorStorage)
throw(XmlException)
{
    if (!compareNodeToName(xmlnode, string("VECTOR"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if (xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Vector Grammar must include at least one Subvector child");
        }
        /**
         * process bytes with string grammar
         */
        return(intVectorProcessGrammar(xmlnode, byteArray,vectorStorage));
    }
    catch (XmlException &err)
    {
        throw XmlException("Vector Grammar Parse Error: " + err.Reason());
    }
}

void ReplyInterpreter::fltVectorEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray,vector<double> &vectorStorage)
throw(XmlException)
{
    if (!compareNodeToName(xmlnode, string("VECTOR"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if (xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Vector Grammar must include at least one Subvector child");
        }
        /**
         * process bytes with string grammar
         */
        return(fltVectorProcessGrammar(xmlnode, byteArray,vectorStorage));
    }
    catch (XmlException &err)
    {
        throw XmlException("Vector Grammar Parse Error: " + err.Reason());
    }
}

void ReplyInterpreter::fltVectorEvaluate(const XmlNode * xmlnode,SerialString_t &byteArray,vector<float> &vectorStorage)
throw(XmlException)
{
    if (!compareNodeToName(xmlnode, string("VECTOR"), 6))
    {
        throw XmlException("Expected String Grammar");
    }

    try
    {
        // sanity checks on root node's children
        if (xmlnode->getChildren().size() == (unsigned int)0)
        {
            throw XmlException("Vector Grammar must include at least one Subvector child");
        }
        /**
         * process bytes with string grammar
         */
        return(fltVectorProcessGrammar(xmlnode, byteArray,vectorStorage));
    }
    catch (XmlException &err)
    {
        throw XmlException("Vector Grammar Parse Error: " + err.Reason());
    }
}

UINT16 ReplyInterpreter::enumProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    unsigned int ii;
    // default to 0 for non-matched condition
    UINT16 enumVal = (UINT16)0;
    // work down from max to find highest priority
    UINT16 lastPriority = ENUM_NON_MATCH;

    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the enum children
    const XmlNodeMap &enumNodes = xmlnode->getChildren();

    // loop through the enum children
    for(ii=0; ii<enumNodes.size(); ii++)
    {
        if(!isElementEnum(enumNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                enumNodes.getNode(ii)->getName());
        }

        // get the boolean grammar nodes within current enum node
        const XmlNodeMap &valNodes = (enumNodes.getNode(ii))->getChildren();

        // sanity check
        if(valNodes.size() == (unsigned int)0)
        {
            throw XmlException("Enum elements must include one Boolean child");
        }

        // sanity check
        if(valNodes.size() > (unsigned int)1)
        {
            throw XmlException(
                              "Enum elements cannot include more than one Boolean child");
        }

        /**
         * if the boolean grammar node evaluates to true,
         * extract the 'enum' val of its parent node and return
         */
        if(boolEvaluate(valNodes.getNode(0), byteArray))
        {
            /**
             * upon boolProcessGrammar == true, find a match with the highest priority;
             * (highest possible priority is 0)
             */
            if(getPriorityVal(enumNodes.getNode(ii)) < lastPriority)
            {
                lastPriority = getPriorityVal(enumNodes.getNode(ii));

                enumVal = getEnumVal(enumNodes.getNode(ii));
            }
        }
    }

    return(enumVal);
}

bool ReplyInterpreter::boolProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    stack <bool> boolStack;
    bool retVal = false;

    UINT32 ii;

    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // retrieve children of node passed in
    const XmlNodeMap &childNodes = xmlnode->getChildren();

    /**
     * recursive calls to traverse XML nodes,
     * placing results on stack
     *  (to handle n terms AND'd, OR'd, XOR'd, and NOT'd together)
     */
    for(ii = 0; ii < childNodes.size(); ii++)
    {
        boolStack.push(boolProcessGrammar(childNodes.getNode(ii), byteArray));
    }

    // AND sanity check
    if(isElementAND(xmlnode) && (boolStack.size() <= (unsigned int)1))
    {
        throw XmlException ("less than two values to AND together");
    }

    //  OR sanity check
    if(isElementOR(xmlnode) && (boolStack.size() <= (unsigned int)1))
    {
        throw XmlException ("less than two values to OR together");
    }

    // XOR sanity check
    if(isElementXOR(xmlnode) && (boolStack.size() != (unsigned int)2))
    {
        throw XmlException ("only two values can be processed by the XOR operation");
    }

    // NOT sanity check
    if(isElementNOT(xmlnode) && (boolStack.size() != (unsigned int)1))
    {
        throw XmlException ("only one value can be processed by the NOT operation");
    }

    // AND operation - by definition, two or more inputs possible
    if(isElementAND(xmlnode))
    {
        retVal = boolStack.top();
        boolStack.pop();

        while(!boolStack.empty())
        {
            retVal &= boolStack.top();
            boolStack.pop();
        }
    }

    // OR operation - by definition, two or more inputs possible
    else if(isElementOR(xmlnode))
    {
        retVal = boolStack.top();
        boolStack.pop();

        while(!boolStack.empty())
        {
            retVal |= boolStack.top();
            boolStack.pop();
        }
    }

    // XOR operation - by definition, only two inputs possible
    else if(isElementXOR(xmlnode))
    {
        retVal = boolStack.top();
        boolStack.pop();

        retVal ^= boolStack.top();
        boolStack.pop();
    }

    // NOT operation - by definition, only one input possible
    else if(isElementNOT(xmlnode))
    {
        bool val;

        val = boolStack.top();
        boolStack.pop();

        if(val == true)
        {
            retVal = false;
        }
        else if(val == false)
        {
            retVal = true;
        }
    }

    // Handle Byte node
    else if(isNodeByte(xmlnode))
    {
        UINT8  dataByte;
        UINT8  maskByte;
        UINT8  compareByte;

        retVal = false;

        // get mask byte
        maskByte = getMaskVal(xmlnode);

        // get byte from input buffer
        dataByte = getInputArrayVal(byteArray,getInputIndexVal(xmlnode));

        // get compare byte
        compareByte = getCompareVal(xmlnode);

        if((UINT8)(maskByte & dataByte) == compareByte)
        {
            retVal = true;
        }
        else
        {
            retVal = false;
        }
    }

    // fall-through error - unrecognized Boolean grammar element
    else
    {
        throw XmlException("unrecognized element: " + xmlnode->getName());

        return(false);
    }

    return(retVal);
}

string ReplyInterpreter::strProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    string retVal = "";
    unsigned int ii;

    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the substring children
    const XmlNodeMap &substringNodes = xmlnode->getChildren();

    // loop through the substring children
    for(ii=0; ii<substringNodes.size(); ii++)
    {
        if(!isElementSubstring(substringNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                substringNodes.getNode(ii)->getName());
        }

        // get the children of current substring element
        const XmlNodeMap &childNodes = (substringNodes.getNode(ii))->getChildren();

        // sanity check
        if(childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Substring elements must include one child");
        }

        // sanity check
        if(childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Substring elements cannot include more than one child");
        }

        // --> handle user-defined 'Fixed' strings
        if(isNodeFixed(childNodes.getNode(0)))
        {
            // retrieve user-defined string and concatenate with retVal string
            retVal += (childNodes.getNode(0))->getValue();
        }

        // --> handle Integer to string conversion
        else if(isElementInteger(childNodes.getNode(0)))
        {
            retVal += strProcessGrammarInt(childNodes.getNode(0),
                                          getStringFromAttribute(substringNodes.getNode(ii),string("format")),
                                          byteArray);
        }

        // --> handle Float to string conversion
        else if(isElementFloat(childNodes.getNode(0)))
        {
            retVal += strProcessGrammarFlt(childNodes.getNode(0),
                                          getStringFromAttribute(substringNodes.getNode(ii),string("format")),
                                          byteArray);
        }

        // --> handle ASCII character array string extraction
        else if(isElementCharsArray(childNodes.getNode(0)))
        {
            retVal += strProcessGrammarCharsArray(childNodes.getNode(0),byteArray);
        }

        // --> handle ASCII character string extraction
        else if(isElementChars(childNodes.getNode(0)))
        {
            retVal += strProcessGrammarChars(childNodes.getNode(0),byteArray);
        }

        // --> handle unrecognized String grammar child element
        else
        {
            throw XmlException("unrecognized child element: " +
                               childNodes.getNode(0)->getName());
        }

    }
    return(retVal);
}

string ReplyInterpreter::strProcessGrammarInt (const XmlNode * xmlnode,const string &formatStr,
                                               SerialString_t &byteArray)
throw(XmlException)
{
    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(!isElementInteger(xmlnode))
    {
        throw XmlException("not an Integer element");
    }

    char temp[50];

    /**
     * evaluate Integer grammar and write result to temp buffer
     * using printf-style format string in the 'format' attribute
     */
    if(
      snprintf (temp,
                (size_t)(sizeof(temp)-1),
                formatStr.c_str(),
                intEvaluate(xmlnode,byteArray)) > 0)
    {
        // create basic_string out of temp buffer
        return(string(temp));
    }
    else
    {
        throw XmlException("could not process " +
                           xmlnode->getName() +
                           " 'format' string");
    }

    return("");
}

string ReplyInterpreter::strProcessGrammarFlt (const XmlNode * xmlnode,const string &formatStr,
                                               SerialString_t &byteArray)
throw(XmlException)
{
    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(!isElementFloat(xmlnode))
    {
        throw XmlException("not a Float element");
    }

    char temp[50];

    /**
     * evaluate Float grammar and write result to temp buffer
     * using printf-style format string in the 'format' attribute
     */
    if(
      snprintf (temp,(size_t)(sizeof(temp)-1), formatStr.c_str(), fltEvaluate(xmlnode,byteArray)) > 0)
    {
        // create basic_string out of temp buffer
        return(string(temp));
    }
    else
    {
        throw XmlException("could not process " +
                           xmlnode->getName() +
                           " 'format' string");
    }

    return("");
}

string ReplyInterpreter::strProcessGrammarChars (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(!isElementChars(xmlnode))
    {
        throw XmlException("not a Chars element");
    }

    UINT16 ii, jj;
    string strVal = "";

    UINT8  maskByte;

    // get mask for each byte processed
    maskByte = getMaskVal(xmlnode);

    // get how many characters to extract
    UINT16 len = getLengthVal(xmlnode);

    // ceate a buffer to hold extracted data
    UINT8 temp[len + 1];
    string substituteString[len];   // Used to hold data to be returned
    string originalString[len];     // Used as comparison for stringSubstitute, allows blank to be returned if no match found


    // fill buffer with data extracted from input byteArray
    for(ii = 0; ii < len; ii++)
    {
        temp[ii] = maskByte & getInputArrayVal(byteArray,(getInputIndexVal(xmlnode) + ii));
        substituteString[ii] = temp[ii];
        originalString[ii] = temp[ii];
    }
    // null terminate buffer
    temp[ii] = '\0';

    UINT8 compare;
    string replace = "";

    // get child nodes
    const XmlNodeMap &subNodes = xmlnode->getChildren();

    // loop through any children the Chars element posseses
    for(ii = 0; ii < subNodes.size(); ii++)
    {
        // sanity check - only support for children named "Substitute" or "SubstituteString"
        if(isNodeStringSubstitute(subNodes.getNode(ii)))
        {
            replace = getReplaceValString(subNodes.getNode(ii));
        }
        else if(isNodeSubstitute(subNodes.getNode(ii)))
        {
            replace = getReplaceValUINT8(subNodes.getNode(ii));
        }
        else
        {
            throw XmlException("unrecognized Chars child: " + subNodes.getNode(ii)->getName());
        }

        // get other terms necessary for Substituteion
        compare = getCompareVal(subNodes.getNode(ii));

        /**
         * Substitute buffer byte with replace byte if buffer byte
         * is equal to compare byte
         */
        if(getConditionStr(subNodes.getNode(ii)) == "equals")
        {
            for(jj = 0; jj < len; jj++)
            {
                if(temp[jj] == compare)
                {
                    substituteString[jj] = replace;
                }
                else if((isNodeStringSubstitute(subNodes.getNode(ii))) && (originalString[jj] == substituteString[jj]))
                {   // No match, so remove data
                    substituteString[jj] = "";
                }
            }
        }
        /**
         * Substitute buffer byte with replace byte if buffer byte
         * is less than compare byte
         */
        else if(getConditionStr(subNodes.getNode(ii)) == "less than")
        {
            for(jj = 0; jj < len; jj++)
            {
                if(temp[jj] < compare)
                {
                    substituteString[jj] = replace;
                }
                else if((isNodeStringSubstitute(subNodes.getNode(ii))) && (originalString[jj] == substituteString[jj]))
                {   // No match, so remove data
                    substituteString[jj] = "";
                }
            }
        }
        /**
         * Substitute buffer byte with replace byte if buffer byte
         * is greater than compare byte
         */
        else if(getConditionStr(subNodes.getNode(ii)) == "greater than")
        {
            for(jj = 0; jj < len; jj++)
            {
                if(temp[jj] > compare)
                {
                    substituteString[jj] = replace;
                }
                else if((isNodeStringSubstitute(subNodes.getNode(ii))) && (originalString[jj] == substituteString[jj]))
                {   // No match, so remove data
                    substituteString[jj] = "";
                }
            }
        }
        /**
         * Substitute buffer byte with replace byte if buffer byte
         * is not equal to the compare byte
         */
        else if(getConditionStr(subNodes.getNode(ii)) == "not equal")
        {
            for(jj = 0; jj < len; jj++)
            {
                if(temp[jj] != compare)
                {
                    substituteString[jj] = replace;
                }
                else if((isNodeStringSubstitute(subNodes.getNode(ii))) && (originalString[jj] == substituteString[jj]))
                {   // No match, so remove data
                    substituteString[jj] = "";
                }
            }
        }
        else
        {
            throw XmlException("unrecognized Substitute condition in node: " +
                               subNodes.getNode(ii)->getName());
        }
    }

    // Populate the return string with the substituted values
    for(ii = 0; ii < len; ii++)
    {   // Place all substitutions in return string
        strVal += substituteString[ii];
    }
    // create basic string out of extracted ASCII bytes or Strings
    return(strVal);
}

string ReplyInterpreter::strProcessGrammarCharsArray (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(!isElementCharsArray(xmlnode))
    {
        throw XmlException("not a CharsArray element");
    }

    UINT16 ii, kk;
    INT16 jj;
    string strVal = "";
    UINT8  maskByte;

    // get mask for each byte processed
    maskByte = getMaskVal(xmlnode);

    // get how many characters to extract
    UINT16 len = getLengthVal(xmlnode);

    // ceate a buffer to hold extracted data
    UINT8 temp[len + 1];
    string originalString[len];     // Used as comparison for stringSubstitute, allows blank to be returned if no match found
    bool boolSubstituteString = true;
    bool conditionsMet = false;

    // fill buffer with data extracted from input byteArray
    for(ii = 0; ii < len; ii++)
    {
        temp[ii] = maskByte & getInputArrayVal(byteArray,(getInputIndexVal(xmlnode) + ii));
        originalString[ii] = temp[ii];
    }
    // null terminate buffer
    temp[ii] = '\0';
    //buffer to hold compare data
    UINT8 compare[len + 1];
    string replace = "";

    // get child nodes
    const XmlNodeMap &subNodes = xmlnode->getChildren();

    // loop through any children the Chars element posseses
    for(ii = 0; ii < subNodes.size(); ii++)
    {
        // sanity check - only support for children named "Substitute" or "StringSubstitute"
        if(isNodeStringSubstitute(subNodes.getNode(ii)))
        {
            replace = getReplaceValString(subNodes.getNode(ii));
        }
        else if(isNodeSubstitute(subNodes.getNode(ii)))
        {
            replace = getReplaceValUINT8(subNodes.getNode(ii));
        }
        else
        {
            throw XmlException("unrecognized CharsArray child: " + subNodes.getNode(ii)->getName());
        }

        // get compare value children
        const XmlNodeMap &compareNodes = subNodes.getNode(ii)->getChildren();

        // Make sure number of subnodes and length matches
        if (compareNodes.size() == (unsigned int)len) 
        {    //sizes match do compare
            for (kk = 0; kk < compareNodes.size(); kk++) 
            {
                if(isNodeCompare(compareNodes.getNode(kk)))
                {
                    compare[kk] = getCompareVal(compareNodes.getNode(kk));
                    maskByte = getMaskVal(compareNodes.getNode(kk));
                    temp[kk] = temp[kk] & maskByte;
                }
                else
                {
                    throw XmlException("unrecognized CharsArray child: " + compareNodes.getNode(kk)->getName());
                }
            }

            /**
             * Substitute buffer byte with replace byte if buffer byte
             * is equal to compare byte
             */
            jj = 0;
            conditionsMet = false;
            if(getConditionStr(subNodes.getNode(ii)) == "equals")
            {//Note if one byte is not equal comparison fails
                do
                {
                    if (temp[jj] != compare[jj]) 
                    {//Comparison is not equal no need to continue
                        boolSubstituteString = false;
                        conditionsMet = true;
                    }
                    else
                    {
                        boolSubstituteString = true;
                    }
                    jj++;
                }while(jj < len && boolSubstituteString && !conditionsMet);
                if (boolSubstituteString) strVal = replace;
            }

            /**
             * Substitute buffer byte with replace byte if buffer byte
             * is less than compare byte
             */
            else if(getConditionStr(subNodes.getNode(ii)) == "less than")
            {
                do
                {
                    if (temp[jj] < compare[jj]) 
                    {//Comparison is less no need to continue
                        boolSubstituteString = true;
                        conditionsMet = true;
                    }
                    else if((temp[jj] == compare[jj]) && (jj != (len-1)))
                    {
                        boolSubstituteString = true;
                    }
                    else
                    {   // No match, so remove data
                        boolSubstituteString = false;
                    }
                    jj++;
                }while(jj < len && boolSubstituteString && !conditionsMet);
                if (boolSubstituteString) strVal = replace;
            }

            /**
             * Substitute buffer byte with replace byte if buffer byte
             * is greater than compare byte
             */
            else if(getConditionStr(subNodes.getNode(ii)) == "greater than")
            {
                do
                {
                    if (temp[jj] > compare[jj]) 
                    {//Comparison is greater no need to continue
                        boolSubstituteString = true;
                        conditionsMet = true;
                    }
                    else if((temp[jj] == compare[jj]) && (jj != (len-1)))
                    {
                        boolSubstituteString = true;
                    }
                    else
                    {   // No match, so remove data
                        boolSubstituteString = false;
                    }
                    jj++;
                }while(jj < len && boolSubstituteString && !conditionsMet);
                if (boolSubstituteString) strVal = replace;
            }

            /**
             * Substitute buffer byte with replace byte if buffer byte
             * is not equal to the compare byte
             */
            else if(getConditionStr(subNodes.getNode(ii)) == "not equal")
            {//Note if one byte is equal comparison fails
                do
                {
                    if (temp[jj] == compare[jj]) 
                    {//Comparison is equal no need to continue
                        boolSubstituteString = false;
                        conditionsMet = true;
                    }
                    else
                    {
                        boolSubstituteString = true;
                    }
                    jj++;
                }while(jj < len && boolSubstituteString && !conditionsMet);
                if (boolSubstituteString) strVal = replace;
            }
            else
            {
                throw XmlException("unrecognized Substitute condition in node: " +
                                   subNodes.getNode(ii)->getName());
            }
        }
        else
        {//sizes do not match
            throw XmlException("Length value and number of compare nodes do not match");
        }
    }

    // create basic string out of extracted ASCII bytes or Strings
    return(strVal);
}

UINT32 ReplyInterpreter::intProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    INT32  retVal = 0;

    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // switch on the type of Integer being inspected
    switch(getIntegerTypeFromAttribute(xmlnode))
    {

    // --> Standard Unsigned Integer ("raw")
    case INT_TYPE_UNSIGNED:
        {
            return(intProcessGrammarUnsigned(xmlnode,byteArray));

            // break;
        }

        // --> Simple Signed Integer (MSB is sign bit)
    case INT_TYPE_SIMPLE_SIGNED:
        {
            return(intProcessGrammarSimpleSigned(xmlnode,byteArray));

            // break;
        }

        // --> Offset Signed Integer (popular with Analog/Digital conversions)
        /**
         * zero is defined as half the integer's max value:
         * everything above half-way is positive, below is negative
         */
    case INT_TYPE_OFFSET_SIGNED:
        {
            return(intProcessGrammarOffsetSigned(xmlnode,byteArray));

            // break;
        }

        // --> Standard Signed Integer
        //      (2's Complement - normal, modern computer representation)
    case INT_TYPE_TWOS_COMP_SIGNED:
        {
            return(intProcessGrammarTwosCompSigned(xmlnode,byteArray));

            //break;
        }

        // --> handle fall through on switch statement
    default:
        {
            throw XmlException("unrecognized Integer type in element: " +
                               xmlnode->getName());

            break;
        }

    }

    return(retVal);
}

UINT32 ReplyInterpreter::intProcessGrammarUnsigned (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(getIntegerTypeFromAttribute(xmlnode) != INT_TYPE_UNSIGNED)
    {
        throw XmlException("element not unsigned Integer type as expected");
    }

    unsigned int ii;

    UINT32 retVal = 0;
    UINT32 outputIndexVal;
    UINT8  dataByte;
    UINT8  maskByte;

    double  offset;
    double  scaleFactor;

    /**
     * union to hold extracted bytes for
     * interpretation as an Integer
     */
    union
    {
        UINT32 value;
        UINT8  array[sizeof(INT32)];
    } outputUnion;

    // clear the union
    for(ii = 0; ii < sizeof(INT32); ii++)
    {
        outputUnion.array[ii] = 0x00;
    }

    // get all the integer children
    const XmlNodeMap &intNodes = xmlnode->getChildren();

    // sanity check
    if(intNodes.size() == (unsigned int)0)
    {
        throw XmlException("no Integer children to process");
    }

    // extract offset and scale factors from Integer attributes
    offset = getOffsetVal(xmlnode);
    scaleFactor = getScaleVal(xmlnode);

    // loop through the int children
    for(ii=0; ii<intNodes.size(); ii++)
    {
        // sanity check - looking for 'Byte' children nodes
        if(!isNodeByte(intNodes.getNode(ii)))
        {
            throw XmlException("unrecognized node: " + intNodes.getNode(ii)->getName());
        }

        // get output index (index position in outputUnion.array)
        outputIndexVal = getOutputIndexVal(intNodes.getNode(ii));

        // outputUnion.array bounds check
        if(outputIndexVal > (sizeof(outputUnion.array)-1))
        {
            throw XmlException("integer union array index out of bounds");
        }

        // get data byte from byteArray
        dataByte = getInputArrayVal(byteArray,getInputIndexVal(intNodes.getNode(ii)));

        // get mask from current Byte node
        maskByte = getMaskVal(intNodes.getNode(ii));

        // store result in outputUnion's array
        outputUnion.array[outputIndexVal] = dataByte & maskByte;
    }

    retVal = outputUnion.value;

    // multiply final val by scale factor, add offset, and round
    retVal = (UINT32)((((double)retVal) * scaleFactor) + offset + 0.5f);

    return(retVal);
}

INT32 ReplyInterpreter::intProcessGrammarSimpleSigned (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(getIntegerTypeFromAttribute(xmlnode) != INT_TYPE_SIMPLE_SIGNED)
    {
        throw XmlException("element not simple signed Integer type as expected");
    }

    unsigned int ii;

    INT32  retVal = 0;
    UINT32 outputIndexVal;
    UINT8  dataByte;
    UINT8  maskByte;
    UINT16 bits;
    bool   negativeFlag = false;

    double  offset;
    double  scaleFactor;

    /**
     * union to hold extracted bytes for
     * interpretation as an Integer
     */
    union
    {
        INT32 value;
        UINT8 array[sizeof(INT32)];
    } outputUnion;

    // clear the union
    for(ii = 0; ii < sizeof(INT32); ii++)
    {
        outputUnion.array[ii] = 0x00;
    }

    // get all the integer children
    const XmlNodeMap &intNodes = xmlnode->getChildren();

    // sanity check
    if(intNodes.size() == (unsigned int)0)
    {
        throw XmlException("no Integer children to process");
    }

    // extract offset and scale factors from Integer attributes
    offset = getOffsetVal(xmlnode);
    scaleFactor = getScaleVal(xmlnode);

    // loop through the int children
    for(ii=0; ii<intNodes.size(); ii++)
    {
        // sanity check - looking for 'Byte' children nodes
        if(!isNodeByte(intNodes.getNode(ii)))
        {
            throw XmlException("unrecognized node: " + intNodes.getNode(ii)->getName());
        }

        // get output index (index position in outputUnion.array)
        outputIndexVal = getOutputIndexVal(intNodes.getNode(ii));

        // outputUnion.array bounds check
        if(outputIndexVal > (sizeof(outputUnion.array)-1))
        {
            throw XmlException("integer union array index out of bounds");
        }

        // get data byte from byteArray
        dataByte = getInputArrayVal(byteArray,getInputIndexVal(intNodes.getNode(ii)));

        // get mask from current Byte node
        maskByte = getMaskVal(intNodes.getNode(ii));

        // store result in outputUnion's array
        outputUnion.array[outputIndexVal] = dataByte & maskByte;
    }

    // get bit length of integer being assembled
    bits = getBitSize(xmlnode);

    // if most signigicant bit is 1, integer is negative
    if((outputUnion.value & createBitMask(bits,BITMASK_SINGLE_BIT)) > 0)
    {
        negativeFlag = true;
    }

    // mask out only the number of data bits being processed
    outputUnion.value &= createBitMask(bits,BITMASK_LOWER_BITS_EXCLUSIVE);

    // handle negative value
    if(negativeFlag)
    {
        retVal = -(outputUnion.value);

        // multiply final val by scale factor, add offset, and round
        retVal = (INT32)((((double)retVal) * scaleFactor) + offset - 0.5f);
    }

    // handle positive value
    else
    {
        retVal = outputUnion.value;

        // multiply final val by scale factor, add offset, and round
        retVal = (INT32)((((double)retVal) * scaleFactor) + offset + 0.5f);
    }

    return(retVal);
}

INT32 ReplyInterpreter::intProcessGrammarOffsetSigned (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(getIntegerTypeFromAttribute(xmlnode) != INT_TYPE_OFFSET_SIGNED)
    {
        throw XmlException("element not simple signed Integer type as expected");
    }

    unsigned int ii;

    INT32  retVal = 0;
    UINT32 outputIndexVal;
    UINT8  dataByte;
    UINT8  maskByte;
    UINT16 bits;
    UINT32 halfMaxByteVal;

    double  offset;
    double  scaleFactor;

    /**
     * union to hold extracted bytes for
     * interpretation as an Integer
     */
    union
    {
        INT32 value;
        UINT8 array[sizeof(INT32)];
    } outputUnion;

    // clear the union
    for(ii = 0; ii < sizeof(INT32); ii++)
    {
        outputUnion.array[ii] = 0x00;
    }

    // get all the integer children
    const XmlNodeMap &intNodes = xmlnode->getChildren();

    // sanity check
    if(intNodes.size() == (unsigned int)0)
    {
        throw XmlException("no Integer children to process");
    }

    // extract offset and scale factors from Integer attributes
    offset = getOffsetVal(xmlnode);
    scaleFactor = getScaleVal(xmlnode);

    // loop through the int children
    for(ii=0; ii<intNodes.size(); ii++)
    {
        // sanity check - looking for 'Byte' children nodes
        if(!isNodeByte(intNodes.getNode(ii)))
        {
            throw XmlException("unrecognized node: " + intNodes.getNode(ii)->getName());
        }

        // get output index (index position in outputUnion.array)
        outputIndexVal = getOutputIndexVal(intNodes.getNode(ii));

        // outputUnion.array bounds check
        if(outputIndexVal > (sizeof(outputUnion.array)-1))
        {
            throw XmlException("integer union array index out of bounds");
        }

        // get data byte from byteArray
        dataByte = getInputArrayVal(byteArray,getInputIndexVal(intNodes.getNode(ii)));

        // get mask from current Byte node
        maskByte = getMaskVal(intNodes.getNode(ii));

        // store result in outputUnion's array
        outputUnion.array[outputIndexVal] = dataByte & maskByte;
    }

    // get bit length of integer being assembled
    bits = getBitSize(xmlnode);

    // mask off only the 'bits' we're inspecting
    outputUnion.value &= createBitMask(bits,BITMASK_LOWER_BITS_INCLUSIVE);

    /**
     * use the createBitMask output not as a mask but as an integer
     * to determine half of max value for given number of bits;
     * we're exploiting the nature of bit position and binary math here
     */
    halfMaxByteVal = createBitMask(bits,BITMASK_SINGLE_BIT);

    // --> handle positive half of offset integer
    if((UINT32)outputUnion.value > halfMaxByteVal)
    {
        retVal = (INT32)((UINT32)outputUnion.value - halfMaxByteVal);

        // multiply final val by scale factor, add offset, and round
        retVal = (INT32)((((double)retVal) * scaleFactor) + offset + 0.5f);
    }

    // --> handle negative half of offset integer
    else if((UINT32)outputUnion.value < halfMaxByteVal)
    {
        retVal = (INT32)((UINT32)outputUnion.value - halfMaxByteVal);

        // multiply final val by scale factor, add offset, and round
        retVal = (INT32)((((double)retVal) * scaleFactor) + offset - 0.5f);
    }

    // --> handle fall through to zero value
    /**
     *     halfMaxByteVal and extracted byte values are equal,
     *     which is 0 in offset representation
     */
    else
    {
        // handle offset and rounding of offset
        if(offset >= 0.0f)
        {
            retVal = (INT32) (offset + 0.5F);
        }
        else
        {
            retVal = (INT32) (offset - 0.5F);
        }
    }

    return(retVal);
}

INT32 ReplyInterpreter::intProcessGrammarTwosCompSigned (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");
    }

    if(getIntegerTypeFromAttribute(xmlnode) != INT_TYPE_TWOS_COMP_SIGNED)
    {
        throw XmlException("element not simple signed Integer type as expected");
    }

    unsigned int ii;

    INT32  retVal = 0;
    UINT32 outputIndexVal;
    UINT8  dataByte;
    UINT8  maskByte;
    UINT16 bits;

    double  offset;
    double  scaleFactor;

    /**
     * union to hold extracted bytes for
     * interpretation as an Integer
     */
    union
    {
        INT32 value;
        UINT8 array[sizeof(INT32)];
    } outputUnion;

    // clear the union
    for(ii = 0; ii < sizeof(INT32); ii++)
    {
        outputUnion.array[ii] = 0x00;
    }

    // get all the integer children
    const XmlNodeMap &intNodes = xmlnode->getChildren();

    // sanity check
    if(intNodes.size() == (unsigned int)0)
    {
        throw XmlException("no Integer children to process");
    }

    // extract offset and scale factors from Integer attributes
    offset = getOffsetVal(xmlnode);
    scaleFactor = getScaleVal(xmlnode);

    // loop through the int children
    for(ii=0; ii<intNodes.size(); ii++)
    {
        // sanity check - looking for 'Byte' children nodes
        if(!isNodeByte(intNodes.getNode(ii)))
        {
            throw XmlException("unrecognized node: " + intNodes.getNode(ii)->getName());
        }


        // get output index (index position in outputUnion.array)
        outputIndexVal = getOutputIndexVal(intNodes.getNode(ii));

        // outputUnion.array bounds check
        if(outputIndexVal > (sizeof(outputUnion.array)-1))
        {
            throw XmlException("integer union array index out of bounds");
        }

        // get data byte from byteArray
        dataByte = getInputArrayVal(byteArray,getInputIndexVal(intNodes.getNode(ii)));

        // get mask from current Byte node
        maskByte = getMaskVal(intNodes.getNode(ii));

        // store result in outputUnion's array
        outputUnion.array[outputIndexVal] = dataByte & maskByte;
    }

    // get bit length of integer being assembled
    bits = getBitSize(xmlnode);

    // mask off only the 'bits' we're inspecting
    outputUnion.value &= createBitMask(bits,BITMASK_LOWER_BITS_INCLUSIVE);

    retVal = (INT32)outputUnion.value;

    // multiply final val by scale factor, add offset, and round
    retVal = (INT32)((((double)retVal) * scaleFactor) + offset + 0.5f);

    /**
     * deal with negative number and extend the sign bits to preserve
     *  two's complement format in a 4 byte wide Integer;
     * positive case is already handled by default in previous lines
     */
    if((outputUnion.value & createBitMask(bits,BITMASK_SINGLE_BIT)) > 0)
    {
        outputUnion.value |= createBitMask(bits,BITMASK_UPPER_BITS_INCLUSIVE);

        retVal = (INT32)outputUnion.value;

        // multiply final val by scale factor, add offset, and round
        retVal = (INT32)((((double)retVal) * scaleFactor) + offset - 0.5f);
    }

    return(retVal);
}

double ReplyInterpreter::fltProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
throw(XmlException)
{
    double offset;
    double scaleFactor;

    // handle null node
    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get the integer children
    const XmlNodeMap &intNodes = xmlnode->getChildren();

    // sanity check
    if(intNodes.size() == (unsigned int)0)
    {
        throw XmlException("no Integer child element to process");
    }

    // sanity check
    if(intNodes.size() > (unsigned int)1)
    {
        throw XmlException("only one Integer child element allowed");
    }

    // extract offset and scale factor from Float attributes
    offset = getOffsetVal(xmlnode);
    scaleFactor = getScaleVal(xmlnode);

    //handle ascii encoded float value
    if (compareNodeToName(intNodes.getNode(0), string("STRING"), 6))
    {
       float processedFloat = atof(strProcessGrammar (intNodes.getNode(0),byteArray).c_str());
    
       return((((double)processedFloat) * scaleFactor) + offset);
    }

    // --> handle an unsigned child Integer
    else if(isIntegerUnsigned(intNodes.getNode(0)))
    {
        UINT32 processedInt =
        (UINT32)intEvaluate(xmlnode->getChildren().getNode(0),byteArray);

        return((((double)processedInt) * scaleFactor) + offset);
    }

    // --> handle a signed child Integer
    else if(isIntegerSigned(intNodes.getNode(0)))
    {
        INT32  processedInt =
        intEvaluate(xmlnode->getChildren().getNode(0),byteArray);

        return((((double)processedInt) * scaleFactor) + offset);
    }

    // --> handle unknown Integer type
    else
    {
        throw XmlException ("unrecognized Integer type for: " +
                            intNodes.getNode(0)->getName());
    }

    // fall through return type (just to be safe)
    return(0.0f);
}

void ReplyInterpreter::boolVectorProcessGrammar (const XmlNode * xmlnode,
                                                 SerialString_t &byteArray,
                                                 vector<bool> &vectorStorage)
throw(XmlException)
{
    unsigned int ii;
    UINT16 vectorType;
    SerialString_t currentByteArray;
    INT32 startPosition = 0;
    INT32 offset = 0;
    UINT32 currentPosition = 0;


    // handle null node
    if (xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the subvector children
    const XmlNodeMap &subvectorNodes = xmlnode->getChildren();

    vectorType = getVectorTypeFromAttribute(xmlnode);
    // switch on the type of Integer being inspected
    switch (vectorType)
    {
        case VECTOR_TYPE_FIXED:
            {
                startPosition = 0;
                offset = 0;
                break;
            }
        case VECTOR_TYPE_VARIABLE:
            {
                // sanity check
                if (subvectorNodes.size() > (unsigned int)1)
                {
                    throw XmlException("Only one subvector element allowed for variable type vector");
                }
                else
                {
                    // Get necessary information for looping for the varible vector
                    startPosition = getStartByteIndexVal(xmlnode);
                    offset = getVectorByteOffsetVal(xmlnode);
                }
                break;
            }
        default:
            {
                throw XmlException("unrecognized vector type in element: " +
                                   xmlnode->getName());
                break;
            }
    }

    // loop through the subvector children
    for (ii=0; ii<subvectorNodes.size(); ii++)
    {
        if (!isElementSubvector(subvectorNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                subvectorNodes.getNode(ii)->getName());
        }

        // get the children of current subvector element
        const XmlNodeMap &childNodes = (subvectorNodes.getNode(ii))->getChildren();

        // sanity check
        if (childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Subvector elements must include one child");
        }

        // sanity check
        if (childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Subvector elements are allowed to have only one child");
        }

        if (!compareNodeToName(childNodes.getNode(0), string("BOOLEAN"), 7))
        {
            throw XmlException("not a Boolean element as expected");
        }

        currentPosition = startPosition;
        do
        {
            if (byteArray.length() > currentPosition)
            {
                currentByteArray = byteArray.substr(currentPosition);
                vectorStorage.push_back(boolEvaluate(childNodes.getNode(0),currentByteArray));
                currentPosition = currentPosition + offset;
            }
            else
            {
                throw XmlException("Start Byte larger than byte array size");
            }
        }
        while ((byteArray.length() >= (currentPosition + offset)) &&
               (VECTOR_TYPE_VARIABLE == vectorType));
    }
}

void ReplyInterpreter::strVectorProcessGrammar (const XmlNode * xmlnode,
                                                SerialString_t &byteArray,
                                                vector<string> &vectorStorage)
throw(XmlException)
{
    unsigned int ii;
    UINT16 vectorType;
    SerialString_t currentByteArray;
    INT32 startPosition = 0;
    INT32 offset = 0;
    UINT32 currentPosition = 0;


    // handle null node
    if (xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the subvector children
    const XmlNodeMap &subvectorNodes = xmlnode->getChildren();

    vectorType = getVectorTypeFromAttribute(xmlnode);
    // switch on the type of Integer being inspected
    switch (vectorType)
    {
        case VECTOR_TYPE_FIXED:
            {
                startPosition = 0;
                offset = 0;
                break;
            }
        case VECTOR_TYPE_VARIABLE:
            {
                // sanity check
                if (subvectorNodes.size() > (unsigned int)1)
                {
                    throw XmlException("Only one subvector element allowed for variable type vector");
                }
                else
                {
                    // Get necessary information for looping for the varible vector
                    startPosition = getStartByteIndexVal(xmlnode);
                    offset = getVectorByteOffsetVal(xmlnode);
                }
                break;
            }
        default:
            {
                throw XmlException("unrecognized vector type in element: " +
                                   xmlnode->getName());
                break;
            }
    }

    // loop through the subvector children
    for (ii=0; ii<subvectorNodes.size(); ii++)
    {
        if (!isElementSubvector(subvectorNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                subvectorNodes.getNode(ii)->getName());
        }

        // get the children of current subvector element
        const XmlNodeMap &childNodes = (subvectorNodes.getNode(ii))->getChildren();

        // sanity check
        if (childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Subvector elements must include one child");
        }

        // sanity check
        if (childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Subvector elements are allowed to have only one child");
        }

        if (!compareNodeToName(childNodes.getNode(0), string("STRING"), 6))
        {
            throw XmlException("not a String element as expected");
        }

        currentPosition = startPosition;
        do
        {
            if (byteArray.length() > currentPosition)
            {
                currentByteArray = byteArray.substr(currentPosition);
                vectorStorage.push_back(strEvaluate(childNodes.getNode(0),byteArray));
                currentPosition = currentPosition + offset;
            }
            else
            {
                throw XmlException("Start Byte larger than byte array size");
            }
        }
        while ((byteArray.length() >= (currentPosition + offset)) &&
               (VECTOR_TYPE_VARIABLE == vectorType));
    }
}

void ReplyInterpreter::ucharVectorProcessGrammar (const XmlNode * xmlnode,
                                                  SerialString_t &byteArray,
                                                  vector<UINT8> &vectorStorage)
throw(XmlException)
{
    unsigned int ii;
    UINT16 vectorType;
    SerialString_t currentByteArray;
    INT32 startPosition = 0;
    INT32 offset = 0;
    UINT32 currentPosition = 0;

    // handle null node
    if (xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the subvector children
    const XmlNodeMap &subvectorNodes = xmlnode->getChildren();

    vectorType = getVectorTypeFromAttribute(xmlnode);
    // switch on the type of Integer being inspected
    switch (vectorType)
    {
    case VECTOR_TYPE_FIXED:
        {
            startPosition = 0;
            offset = 0;
            break;
        }
    case VECTOR_TYPE_VARIABLE:
        {
            // sanity check
            if (subvectorNodes.size() > (unsigned int)1)
            {
                throw XmlException("Only one subvector element allowed for variable type vector");
            }
            else
            {
                // Get necessary information for looping for the varible vector
                startPosition = getStartByteIndexVal(xmlnode);
                offset = getVectorByteOffsetVal(xmlnode);
            }
            break;
        }
    default:
        {
            throw XmlException("unrecognized vector type in element: " +
                               xmlnode->getName());
            break;
        }
    }

    // loop through the subvector children
    for (ii=0; ii<subvectorNodes.size(); ii++)
    {
        if (!isElementSubvector(subvectorNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                subvectorNodes.getNode(ii)->getName());
        }

        // get the children of current subvector element
        const XmlNodeMap &childNodes = (subvectorNodes.getNode(ii))->getChildren();

        // sanity check
        if (childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Subvector elements must include one child");
        }

        // sanity check
        if (childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Subvector elements are allowed to have only one child");
        }

        if (!compareNodeToName(childNodes.getNode(0), string("INTEGER"), 7)
            || (!isIntegerUnsigned(childNodes.getNode(0))))
        {
            throw XmlException("not an Unsigned Integer element as expected");
        }

        currentPosition = startPosition;
        do
        {
            if (byteArray.length() > currentPosition)
            {
                currentByteArray = byteArray.substr(currentPosition);
                vectorStorage.push_back(intEvaluate(childNodes.getNode(0),currentByteArray));
                currentPosition = currentPosition + offset;
            }
            else
            {
                throw XmlException("Start Byte larger than byte array size");
            }
        }
        while ((byteArray.length() >= (currentPosition + offset)) &&
               (VECTOR_TYPE_VARIABLE == vectorType));
    }
}

void ReplyInterpreter::uintVectorProcessGrammar (const XmlNode * xmlnode,
                                                 SerialString_t &byteArray,
                                                 vector<UINT32> &vectorStorage)
throw(XmlException)
{
    unsigned int ii;
    UINT16 vectorType;
    SerialString_t currentByteArray;
    INT32 startPosition = 0;
    INT32 offset = 0;
    UINT32 currentPosition = 0;

    // handle null node
    if (xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the subvector children
    const XmlNodeMap &subvectorNodes = xmlnode->getChildren();

    vectorType = getVectorTypeFromAttribute(xmlnode);
    // switch on the type of Integer being inspected
    switch (vectorType)
    {
        case VECTOR_TYPE_FIXED:
            {
                startPosition = 0;
                offset = 0;
                break;
            }
        case VECTOR_TYPE_VARIABLE:
            {
                // sanity check
                if (subvectorNodes.size() > (unsigned int)1)
                {
                    throw XmlException("Only one subvector element allowed for variable type vector");
                }
                else
                {
                    // Get necessary information for looping for the varible vector
                    startPosition = getStartByteIndexVal(xmlnode);
                    offset = getVectorByteOffsetVal(xmlnode);
                }
                break;
            }
        default:
            {
                throw XmlException("unrecognized vector type in element: " +
                                   xmlnode->getName());
                break;
            }
    }

    // loop through the subvector children
    for (ii=0; ii<subvectorNodes.size(); ii++)
    {
        if (!isElementSubvector(subvectorNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                subvectorNodes.getNode(ii)->getName());
        }

        // get the children of current subvector element
        const XmlNodeMap &childNodes = (subvectorNodes.getNode(ii))->getChildren();

        // sanity check
        if (childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Subvector elements must include one child");
        }

        // sanity check
        if (childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Subvector elements are allowed to have only one child");
        }

        if (!compareNodeToName(childNodes.getNode(0), string("INTEGER"), 7)
            || (!isIntegerUnsigned(childNodes.getNode(0))))
        {
            throw XmlException("not an Unsigned Integer element as expected");
        }

        currentPosition = startPosition;
        do
        {
            if (byteArray.length() > currentPosition)
            {
                currentByteArray = byteArray.substr(currentPosition);
                vectorStorage.push_back(intEvaluate(childNodes.getNode(0),byteArray));
                currentPosition = currentPosition + offset;
            }
            else
            {
                throw XmlException("Start Byte larger than byte array size");
            }
        }
        while ((byteArray.length() >= (currentPosition + offset)) &&
               (VECTOR_TYPE_VARIABLE == vectorType));
    }
}

void ReplyInterpreter::uintVectorProcessGrammar (const XmlNode * xmlnode,
                                                 SerialString_t &byteArray,
                                                 vector<UINT16> &vectorStorage)
throw(XmlException)
{
    unsigned int ii;
    UINT16 vectorType;
    SerialString_t currentByteArray;
    INT32 startPosition = 0;
    INT32 offset = 0;
    UINT32 currentPosition = 0;

    // handle null node
    if (xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the subvector children
    const XmlNodeMap &subvectorNodes = xmlnode->getChildren();

    vectorType = getVectorTypeFromAttribute(xmlnode);
    // switch on the type of Integer being inspected
    switch (vectorType)
    {
    case VECTOR_TYPE_FIXED:
        {
            startPosition = 0;
            offset = 0;
            break;
        }
    case VECTOR_TYPE_VARIABLE:
        {
            // sanity check
            if (subvectorNodes.size() > (unsigned int)1)
            {
                throw XmlException("Only one subvector element allowed for variable type vector");
            }
            else
            {
                // Get necessary information for looping for the varible vector
                startPosition = getStartByteIndexVal(xmlnode);
                offset = getVectorByteOffsetVal(xmlnode);
            }
            break;
        }
    default:
        {
            throw XmlException("unrecognized vector type in element: " +
                               xmlnode->getName());
            break;
        }
    }

    // loop through the subvector children
    for (ii=0; ii<subvectorNodes.size(); ii++)
    {
        if (!isElementSubvector(subvectorNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                subvectorNodes.getNode(ii)->getName());
        }

        // get the children of current subvector element
        const XmlNodeMap &childNodes = (subvectorNodes.getNode(ii))->getChildren();

        // sanity check
        if (childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Subvector elements must include one child");
        }

        // sanity check
        if (childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Subvector elements are allowed to have only one child");
        }

        if (!compareNodeToName(childNodes.getNode(0), string("INTEGER"), 7)
            || (!isIntegerUnsigned(childNodes.getNode(0))))
        {
            throw XmlException("not an Unsigned Integer element as expected");
        }

        currentPosition = startPosition;
        do
        {
            if (byteArray.length() > currentPosition)
            {
                currentByteArray = byteArray.substr(currentPosition);
                vectorStorage.push_back(intEvaluate(childNodes.getNode(0),byteArray));
                currentPosition = currentPosition + offset;
            }
            else
            {
                throw XmlException("Start Byte larger than byte array size");
            }
        }
        while ((byteArray.length() >= (currentPosition + offset)) &&
               (VECTOR_TYPE_VARIABLE == vectorType));
    }
}

void ReplyInterpreter::intVectorProcessGrammar (const XmlNode * xmlnode,
                                                SerialString_t &byteArray,
                                                vector<INT32> &vectorStorage)
throw(XmlException)
{
    unsigned int ii;
    UINT16 vectorType;
    SerialString_t currentByteArray;
    INT32 startPosition = 0;
    INT32 offset = 0;
    UINT32 currentPosition = 0;


    // handle null node
    if (xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the subvector children
    const XmlNodeMap &subvectorNodes = xmlnode->getChildren();

    vectorType = getVectorTypeFromAttribute(xmlnode);
    // switch on the type of Integer being inspected
    switch (vectorType)
    {
        case VECTOR_TYPE_FIXED:
            {
                startPosition = 0;
                offset = 0;
                break;
            }
        case VECTOR_TYPE_VARIABLE:
            {
                // sanity check
                if (subvectorNodes.size() > (unsigned int)1)
                {
                    throw XmlException("Only one subvector element allowed for variable type vector");
                }
                else
                {
                    // Get necessary information for looping for the varible vector
                    startPosition = getStartByteIndexVal(xmlnode);
                    offset = getVectorByteOffsetVal(xmlnode);
                }
                break;
            }
        default:
            {
                throw XmlException("unrecognized vector type in element: " +
                                   xmlnode->getName());
                break;
            }
    }

    // loop through the subvector children
    for (ii=0; ii<subvectorNodes.size(); ii++)
    {
        if (!isElementSubvector(subvectorNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                subvectorNodes.getNode(ii)->getName());
        }

        // get the children of current subvector element
        const XmlNodeMap &childNodes = (subvectorNodes.getNode(ii))->getChildren();

        // sanity check
        if (childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Subvector elements must include one child");
        }

        // sanity check
        if (childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Subvector elements are allowed to have only one child");
        }

        if (!compareNodeToName(childNodes.getNode(0), string("INTEGER"), 7)
            || (!isIntegerSigned(childNodes.getNode(0))))
        {
            throw XmlException("not a Signed Integer element as expected");
        }

        currentPosition = startPosition;
        do
        {
            if (byteArray.length() > currentPosition)
            {
                currentByteArray = byteArray.substr(currentPosition);
        vectorStorage.push_back((INT32) intEvaluate(childNodes.getNode(0),byteArray));
                currentPosition = currentPosition + offset;
            }
            else
            {
                throw XmlException("Start Byte larger than byte array size");
            }
        }
        while ((byteArray.length() >= (currentPosition + offset)) &&
               (VECTOR_TYPE_VARIABLE == vectorType));
    }
}

void ReplyInterpreter::fltVectorProcessGrammar (const XmlNode * xmlnode,
                                                SerialString_t &byteArray,
                                                vector<double> &vectorStorage)
throw(XmlException)
{
    unsigned int ii;
    UINT16 vectorType;
    SerialString_t currentByteArray;
    INT32 startPosition = 0;
    INT32 offset = 0;
    UINT32 currentPosition = 0;


    // handle null node
    if (xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the subvector children
    const XmlNodeMap &subvectorNodes = xmlnode->getChildren();

    vectorType = getVectorTypeFromAttribute(xmlnode);
    // switch on the type of Integer being inspected
    switch (vectorType)
    {
        case VECTOR_TYPE_FIXED:
            {
                startPosition = 0;
                offset = 0;
                break;
            }
        case VECTOR_TYPE_VARIABLE:
            {
                // sanity check
                if (subvectorNodes.size() > (unsigned int)1)
                {
                    throw XmlException("Only one subvector element allowed for variable type vector");
                }
                else
                {
                    // Get necessary information for looping for the varible vector
                    startPosition = getStartByteIndexVal(xmlnode);
                    offset = getVectorByteOffsetVal(xmlnode);
                }
                break;
            }
        default:
            {
                throw XmlException("unrecognized vector type in element: " +
                                   xmlnode->getName());
                break;
            }
    }

    // loop through the subvector children
    for (ii=0; ii<subvectorNodes.size(); ii++)
    {
        if (!isElementSubvector(subvectorNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                subvectorNodes.getNode(ii)->getName());
        }

        // get the children of current subvector element
        const XmlNodeMap &childNodes = (subvectorNodes.getNode(ii))->getChildren();

        // sanity check
        if (childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Subvector elements must include one child");
        }

        // sanity check
        if (childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Subvector elements are allowed to have only one child");
        }

        if (!isElementFloat(childNodes.getNode(0)))
        {
            throw XmlException("not a Float element as expected");
        }

        currentPosition = startPosition;
        do
        {
            if (byteArray.length() > currentPosition)
            {
                currentByteArray = byteArray.substr(currentPosition);
        vectorStorage.push_back(fltEvaluate(childNodes.getNode(0),byteArray));
                currentPosition = currentPosition + offset;
            }
            else
            {
                throw XmlException("Start Byte larger than byte array size");
            }
        }
        while ((byteArray.length() >= (currentPosition + offset)) &&
               (VECTOR_TYPE_VARIABLE == vectorType));
    }
}

void ReplyInterpreter::fltVectorProcessGrammar (const XmlNode * xmlnode,
                                                SerialString_t &byteArray,
                                                vector<float> &vectorStorage)
throw(XmlException)
{
    unsigned int ii;
    UINT16 vectorType;
    SerialString_t currentByteArray;
    INT32 startPosition = 0;
    INT32 offset = 0;
    UINT32 currentPosition = 0;


    // handle null node
    if (xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    // get all the subvector children
    const XmlNodeMap &subvectorNodes = xmlnode->getChildren();

    vectorType = getVectorTypeFromAttribute(xmlnode);
    // switch on the type of Integer being inspected
    switch (vectorType)
    {
    case VECTOR_TYPE_FIXED:
        {
            startPosition = 0;
            offset = 0;
            break;
        }
    case VECTOR_TYPE_VARIABLE:
        {
            // sanity check
            if (subvectorNodes.size() > (unsigned int)1)
            {
                throw XmlException("Only one subvector element allowed for variable type vector");
            }
            else
            {
                // Get necessary information for looping for the varible vector
                startPosition = getStartByteIndexVal(xmlnode);
                offset = getVectorByteOffsetVal(xmlnode);
            }
            break;
        }
    default:
        {
            throw XmlException("unrecognized vector type in element: " +
                               xmlnode->getName());
            break;
        }
    }

    // loop through the subvector children
    for (ii=0; ii<subvectorNodes.size(); ii++)
    {
        if (!isElementSubvector(subvectorNodes.getNode(ii)))
        {
            throw XmlException ("unrecognized element: " +
                                subvectorNodes.getNode(ii)->getName());
        }

        // get the children of current subvector element
        const XmlNodeMap &childNodes = (subvectorNodes.getNode(ii))->getChildren();

        // sanity check
        if (childNodes.size() == (unsigned int)0)
        {
            throw XmlException("Subvector elements must include one child");
        }

        // sanity check
        if (childNodes.size() > (unsigned int)1)
        {
            throw XmlException("Subvector elements are allowed to have only one child");
        }

        if (!isElementFloat(childNodes.getNode(0)))
        {
            throw XmlException("not a Float element as expected");
        }

        currentPosition = startPosition;
        do
        {
            if (byteArray.length() > currentPosition)
            {
                currentByteArray = byteArray.substr(currentPosition);
                vectorStorage.push_back(fltEvaluate(childNodes.getNode(0),byteArray));
                currentPosition = currentPosition + offset;
            }
            else
            {
                throw XmlException("Start Byte larger than byte array size");
            }
        }
        while ((byteArray.length() >= (currentPosition + offset)) &&
               (VECTOR_TYPE_VARIABLE == vectorType));
    }
}

/////////////////////////////////////////////////////////////////////////////////////
bool ReplyInterpreter::isElementAND (const XmlNode * opElement)
throw(XmlException)
{
    return(compareNodeToName(opElement, string("AND"), 3));
}

bool ReplyInterpreter::isElementOR (const XmlNode * opElement)
throw(XmlException)
{
    return(compareNodeToName(opElement, string("OR"), 2));
}

bool ReplyInterpreter::isElementXOR (const XmlNode * opElement)
throw(XmlException)
{
    return(compareNodeToName(opElement, string("XOR"), 3));
}

bool ReplyInterpreter::isElementNOT (const XmlNode * opElement)
throw(XmlException)
{
    return(compareNodeToName(opElement, string("NOT"), 3));
}

bool ReplyInterpreter::isElementEnum (const XmlNode * element)
throw(XmlException)
{
    return(compareNodeToName(element, string("ENUM"), 4));
}

bool ReplyInterpreter::isElementInteger (const XmlNode * element)
throw(XmlException)
{
    return(compareNodeToName(element, string("INTEGER"), 7));
}

bool ReplyInterpreter::isElementFloat (const XmlNode * element)
throw(XmlException)
{
    return(compareNodeToName(element, string("FLOAT"), 5));
}

bool ReplyInterpreter::isElementSubstring (const XmlNode * element)
throw(XmlException)
{
    return(compareNodeToName(element, string("SUBSTRING"), 9));
}

bool ReplyInterpreter::isElementSubvector (const XmlNode * element)
throw(XmlException)
{
    return(compareNodeToName(element, string("SUBVECTOR"), 9));
}

bool ReplyInterpreter::isElementChars (const XmlNode * element)
throw(XmlException)
{
    return(compareNodeToName(element, string("CHARS"), 5));
}

bool ReplyInterpreter::isElementCharsArray (const XmlNode * element)
throw(XmlException)
{
    return(compareNodeToName(element, string("CHARSARRAY"), 10));
}

bool ReplyInterpreter::isNodeByte (const XmlNode * node)
throw(XmlException)
{
    return(compareNodeToName(node, string("BYTE"), 4));
}

bool ReplyInterpreter::isNodeFixed (const XmlNode * node)
throw(XmlException)
{
    return(compareNodeToName(node, string("FIXED"), 5));
}

bool ReplyInterpreter::isNodeSubstitute (const XmlNode * node)
throw(XmlException)
{
    return(compareNodeToName(node, string("SUBSTITUTE"), 10));
}

bool ReplyInterpreter::isNodeStringSubstitute (const XmlNode * node)
throw(XmlException)
{
    return(compareNodeToName(node, string("STRINGSUBSTITUTE"), 16));
}

bool ReplyInterpreter::isNodeCompare (const XmlNode * node)
throw(XmlException)
{
    return(compareNodeToName(node, string("COMPARE"), 7));
}

UINT16 ReplyInterpreter::getEnumVal (const XmlNode * enumElement)
throw(XmlException)
{
    return(getUINT16FromAttribute(enumElement, string("enum")));
}

UINT16 ReplyInterpreter::getPriorityVal (const XmlNode * enumElement)
throw(XmlException)
{
    return(getUINT16FromAttribute(enumElement, string("match_priority")));
}

UINT16 ReplyInterpreter::getBitSize (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT16FromAttribute(xmlnode, string("bits")));
}

double ReplyInterpreter::getScaleVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getFloatFromAttribute(xmlnode, string("scale")));
}

double ReplyInterpreter::getOffsetVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getFloatFromAttribute(xmlnode, string("offset")));
}

UINT8 ReplyInterpreter::getMaskVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT8FromHexAttribute(xmlnode, string("mask")));
}

UINT8 ReplyInterpreter::getCompareVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT8FromHexAttribute(xmlnode, string("compare")));
}

UINT16 ReplyInterpreter::getLengthVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT16FromAttribute(xmlnode, string("length")));
}

UINT8 ReplyInterpreter::getReplaceValUINT8 (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT8FromHexAttribute(xmlnode, string("replace")));
}

string ReplyInterpreter::getReplaceValString (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getStringFromAttribute(xmlnode, string("replace")));
}

string ReplyInterpreter::getConditionStr (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getStringFromAttribute(xmlnode, string("condition")));
}

UINT32 ReplyInterpreter::getInputIndexVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT32FromAttribute(xmlnode, string("input_index")));
}

UINT32 ReplyInterpreter::getOutputIndexVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT32FromAttribute(xmlnode, string("output_index")));
}

UINT32 ReplyInterpreter::getStartByteIndexVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT32FromAttribute(xmlnode, string("startbyte")));
}

UINT32 ReplyInterpreter::getVectorByteOffsetVal (const XmlNode * xmlnode)
throw(XmlException)
{
    return(getUINT32FromAttribute(xmlnode, string("offset")));
}

UINT8 ReplyInterpreter::getInputArrayVal (SerialString_t &byteArray, UINT32 index)
throw(XmlException)
{
    if((index > (byteArray.size() - 1)) ||
       (index < 0))
    {
        char buf[100];

        sprintf(buf,"Input buffer access index of %d is out of bounds - buffer size: %u",
                index, byteArray.size());

        throw XmlException(string(buf));
    }
    // retrieve indexed byte from array
    return(byteArray[index]);
}

bool ReplyInterpreter::compareNodeToName (const XmlNode * xmlnode,const string &name,unsigned int chars)
throw(XmlException)
{
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");

        return(false);
    }

    if(chars == (unsigned int)0)
    {
        return(false);
    }

    try
    {
        // string compare of chars characters, ignoring case
        if(strnicmp((xmlnode->getName()).c_str(), name.c_str(), chars) == 0)
        {
            return(true);
        }
    }
    catch(XmlException &err)
    {
        throw XmlException ("error comparing name \"" +
                            name +
                            "\" with XmlNode \"" +
                            xmlnode->getName() +
                            "\": " +
                            err.Reason());
    }

    return(false);
}

bool ReplyInterpreter::compareAttributeValueToString (const XmlNode * xmlnode,const string &attribute,const string &value)
throw(XmlException)
{
    if(xmlnode == NULL)
    {
        throw XmlException("null XmlNode");

        return(false);
    }

    try
    {
        // ignoring case, compare given string (value) and XmlNode attribute
        if( stricmp(((xmlnode->getAttribute(attribute.c_str()))->getValue()).c_str(),
                    value.c_str()) == 0)
        {
            return(true);
        }
    }
    catch(XmlException &err)
    {
        throw XmlException ("error retrieving attribute \"" +
                            attribute +
                            "\" from XmlNode \"" +
                            xmlnode->getName() +
                            "\": " +
                            err.Reason());
    }

    return(false);
}

bool ReplyInterpreter::isIntegerSigned (const XmlNode * xmlnode)
throw(XmlException)
{
    int intType;

    intType = getIntegerTypeFromAttribute(xmlnode);

    return((intType == INT_TYPE_SIMPLE_SIGNED) ||
           (intType == INT_TYPE_OFFSET_SIGNED) ||
           (intType == INT_TYPE_TWOS_COMP_SIGNED));
}

bool ReplyInterpreter::isIntegerUnsigned (const XmlNode * xmlnode)
throw(XmlException)
{
    int intType;

    intType = getIntegerTypeFromAttribute(xmlnode);

    return(intType == INT_TYPE_UNSIGNED);
}

INT16 ReplyInterpreter::getIntegerTypeFromAttribute (const XmlNode * element)throw(XmlException)
{
    if(compareAttributeValueToString(element,string("type"),string("simple signed")))
    {
        return(INT_TYPE_SIMPLE_SIGNED);
    }
    else if(compareAttributeValueToString(element,string("type"),string("unsigned")))
    {
        return(INT_TYPE_UNSIGNED);
    }
    else if(compareAttributeValueToString(element,string("type"),string("offset signed")))
    {
        return(INT_TYPE_OFFSET_SIGNED);
    }
    else if(compareAttributeValueToString(element,string("type"),string("two's complement signed")))
    {
        return(INT_TYPE_TWOS_COMP_SIGNED);
    }

    return(INT_TYPE_NONE);
}

UINT16 ReplyInterpreter::getVectorTypeFromAttribute (const XmlNode * element)throw(XmlException)
{
    if (compareAttributeValueToString(element,string("type"),string("fixed")))
    {
        return(VECTOR_TYPE_FIXED);
    }
    else if (compareAttributeValueToString(element,string("type"),string("variable")))
    {
        return(VECTOR_TYPE_VARIABLE);
    }

    return(VECTOR_TYPE_NONE);
}

UINT8 ReplyInterpreter::getUINT8FromHexAttribute (const XmlNode * xmlnode,const string &attribute)
throw(XmlException)
{
    INT32  readInt = 0;
    string valStr;

    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    try
    {
        valStr = xmlnode->getAttributes().getNode(XML_T(attribute.c_str()))->getValue();
    }
    catch(XmlException &err)
    {
        throw XmlException ("error retrieving attribute \"" +
                            attribute +
                            "\" from XmlNode \"" +
                            xmlnode->getName() +
                            "\": " +
                            err.Reason());
    }

    if((sscanf(valStr.c_str(), "0x%X", &readInt) != 1) &&
       (sscanf(valStr.c_str(), "0X%X", &readInt) != 1) &&
       (sscanf(valStr.c_str(), "%X",   &readInt) != 1))
    {
        throw XmlException ("hex string format error in attribute \"" +
                            attribute +
                            "\" of XmlNode \"" +
                            xmlnode->getName() +
                            "\"");
    }

    if(readInt < 0)
    {
        throw XmlException ("hex value less than zero in attribute \"" +
                            attribute +
                            "\" of XmlNode \"" +
                            xmlnode->getName() +
                            "\"");
    }

    if(readInt > (INT32)numeric_limits<UINT8>::max())
    {
        throw XmlException ("hex value out of bounds in attribute \"" +
                            attribute +
                            "\" of XmlNode \"" +
                            xmlnode->getName() +
                            "\"");
    }

    return((UINT8)readInt);
}

UINT16 ReplyInterpreter::getUINT16FromAttribute (const XmlNode * xmlnode,const string &attribute)
throw(XmlException)
{
    string valStr;
    INT32 readInt;

    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    try
    {
        valStr =
        xmlnode->getAttributes().getNode(XML_T(attribute.c_str()))->getValue();
    }
    catch(XmlException &err)
    {
        throw XmlException ("error retrieving attribute \"" +
                            attribute +
                            "\" from XmlNode \"" +
                            xmlnode->getName() +
                            "\": " +
                            err.Reason());
    }

    if(sscanf(valStr.c_str(), "%d", &readInt) != 1)
    {
        throw XmlException ("UINT16 string format error in attribute \"" +
                            attribute +
                            "\" of XmlNode \"" +
                            xmlnode->getName() +
                            "\"");
    }

    if(readInt < 0)
    {
        throw XmlException ("UINT16 string less than zero in attribute  \"" +
                            attribute +
                            "\" of XmlNode \"" +
                            xmlnode->getName() +
                            "\"");
    }

    if(readInt >  (INT32)numeric_limits<UINT16>::max())
    {
        throw XmlException (
                           "UINT16 string greater than max allowed value in attribute \"" +
                           attribute +
                           "\" of XmlNode \"" +
                           xmlnode->getName() +
                           "\"");
    }

    return((UINT16)readInt);
}

UINT32 ReplyInterpreter::getUINT32FromAttribute (const XmlNode * xmlnode,const string &attribute)
throw(XmlException)
{
    string valStr;
    double readInt = 0.0f;

    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    try
    {
        valStr = xmlnode->getAttributes().getNode(XML_T(attribute.c_str()))->getValue();
    }
    catch(XmlException &err)
    {
        throw XmlException ("error retrieving attribute \"" +
                            attribute +
                            "\" from XmlNode \"" +
                            xmlnode->getName() +
                            "\": " +
                            err.Reason());
    }

    if(sscanf(valStr.c_str(), "%lf", &readInt) != 1)
    {
        throw XmlException ("UINT32 string format error in attribute \"" +
                            attribute +
                            "\" of XmlNode \"" +
                            xmlnode->getName() +
                            "\"");
    }

    if(readInt > (double)numeric_limits<UINT32>::max())
    {
        throw XmlException (
                           "UINT32 string greater than max allowed value in attribute \"" +
                           attribute +
                           "\" of XmlNode \"" +
                           xmlnode->getName() +
                           "\"");
    }

    if(readInt < 0.0f)
    {
        throw XmlException ("UINT32 string less than zero in attribute \"" +
                            attribute +
                            "\" of XmlNode \"" +
                            xmlnode->getName() +
                            "\"");
    }

    return((UINT32)readInt);
}

double ReplyInterpreter::getFloatFromAttribute (const XmlNode * xmlnode,const string &attribute)
throw(XmlException)
{
    string valStr;
    double readFloat = 0.0;

    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    try
    {
        valStr =
        xmlnode->getAttributes().getNode(XML_T(attribute.c_str()))->getValue();
    }
    catch(XmlException &err)
    {
        throw XmlException ("error retrieving attribute \"" +
                            attribute +
                            "\" from XmlNode \"" +
                            xmlnode->getName() +
                            "\": " +
                            err.Reason());
    }

    if(sscanf(valStr.c_str(), "%lg", &readFloat) != 1)
    {
        throw XmlException ("float string format error in attribute \"" +
                            attribute +
                            "\" of XmlNode \"" +
                            xmlnode->getName() +
                            "\"");
    }

    return(readFloat);
}

string ReplyInterpreter::getStringFromAttribute (const XmlNode * xmlnode,const string &attribute)
throw(XmlException)
{
    string valStr = "";

    if(xmlnode == NULL)
    {
        throw XmlException ("null XmlNode");
    }

    try
    {
        valStr = xmlnode->getAttributes().getNode(XML_T(attribute.c_str()))->getValue();
    }
    catch(XmlException &err)
    {
        throw XmlException ("error retrieving attribute \"" +
                            attribute +
                            "\" from XmlNode \"" +
                            xmlnode->getName() +
                            "\": " +
                            err.Reason());
    }

    return(valStr);
}

UINT32 ReplyInterpreter::createBitMask (UINT16 bitNum, UINT16 maskType)
throw(XmlException)
{
    UINT32 retVal = 0;
    UINT32 manipulateByte = 0x00000001;

    if((bitNum > (sizeof(UINT32) * 8)) ||
       (bitNum == 0))
    {
        throw XmlException("bit size exceeds bounds of 32 bits");
    }

    // do initial bit shifting from bit position 1
    manipulateByte <<= (bitNum - 1);

    switch(maskType)
    {
    // mask with single set bit at bitNum position
    case BITMASK_SINGLE_BIT:
        {
            return(manipulateByte);
            //break;
        }
        /**
         * mask with all bit positions below bitNum
         * (excluding bitNum) position set
         */
    case BITMASK_LOWER_BITS_EXCLUSIVE:
        {
            retVal = (manipulateByte - 1);
            return(retVal);
            //break;
        }
        /**
         * mask with all bit positions above bitNum
         * (including bitNum) position set
         */
    case BITMASK_UPPER_BITS_INCLUSIVE:
        {
            manipulateByte -= 1;
            retVal = ~manipulateByte;
            return(retVal);
            //break;
        }
        /**
         * mask with all bit positions below bitNum
         * (including bitNum) position set
         */
    case BITMASK_LOWER_BITS_INCLUSIVE:
        {
            retVal = (manipulateByte - 1) + manipulateByte;
            return(retVal);
            //break;
        }
        /**
         * mask with all bit positions above bitNum
         * (excluding bitNum) position set
         */
    case BITMASK_UPPER_BITS_EXCLUSIVE:
        {
            manipulateByte = (manipulateByte - 1) + manipulateByte;
            retVal = ~manipulateByte;
            return(retVal);
            //break;
        }

        // fall-through: unrecognized type
    default:
        {
            throw XmlException("unrecognized bitmask type");
        }
    }

    return((UINT32)0);
}



/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */

