//******************************************************************************
// Description:
//  ReplyInterpreter implements the IReplyInterpreter interface
//  It handles XML ReplyInterpretation blocks and extracting data from
//  blocks of raw data returned from a module after a read
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/ModuleInterfaces/Include/ReplyInterpreter.h $
// 
// 8     10/30/07 1:34p Rwiersem
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
// 7     9/22/06 11:11a Rwiersem
// Changes for the 06092201 core release:
//     - Added strProcessGrammarCharsArray()
//     - Added isElementCharsArray()
//     - Added isNodeCompare()
// 
// 6     5/11/05 10:03p Cward
// Updated for 05051101 iteration
//
// 5     5/09/05 5:00p Gpattison
// Added Vector and variable length vector support
//
// 5     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 4     2/16/05 9:15p Cward
// Updated intEvaluate, and intProcessGrammar. Also corrected a few
// comment lines
//
// 3     8/18/04 11:07p Cward
// changed angle brackets to quotes for includes of non system files
//
// 2     6/09/04 10:47p Cward
// Added new standard header comment block
//
// 1     12/21/03 6:32p Cward
//
// 2     10/27/03 4:39p Derickso
// Code cleanup.
//
// 1     8/21/03 8:57a Derickso
// 05WK CCRT Project
//
// 1     7/08/03 1:05p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:28p Khykin
// Initial checkin
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef REPLYINTERPRETER_H
#define REPLYINTERPRETER_H
//==============================================================================

/**
 * @author Mike Karlesky
 * @class ReplyInterpreter
 *
 * @date Release Date:
 * @since 11 Feb 2003
 *
 * @brief Reply Interpreter
 *
 * ReplyInterpreter implements the IReplyInterpreter interface
 * It handles XML ReplyInterpretation blocks and extracting data from
 * blocks of raw data returned from a module after a read
 *
 * <hr>
 * @attention Copyright (c) 2003 Burke E. Porter Machinery
 * All Rights Reserved
 * @attention This file contains confidential information of Burke E. Porter
 * Machinery and is not to be used in any way detrimental to the
 * interests thereof.
 * Unauthorized use, distribution, copying, or transmittal of this
 * file in any way is strictly prohibited.
 *
 * <hr>
 *
 * Revision 1.1  2003/05/27 13:29:15  tenharmsel
 * Moving customer specific module interfaces into a Customers directory
 *
 * Revision 1.1.1.1  2003/05/07 15:11:59  tenharmsel
 * Bringing in the GenericModuleInterfaces
 *
 * Revision 1.5  2003/04/21 17:23:59  tenharmsel
 * Updated header comments to match BepCo standard for what information needs to go in header comments
 *
 *
 */

#include "IReplyInterpreter.h"
#include "BepXmlUtils.h"

class ReplyInterpreter : public IReplyInterpreter
{
    friend class ReplyInterpreterTest;

public:

    static const UINT16 BITMASK_SINGLE_BIT = 0;
    static const UINT16 BITMASK_UPPER_BITS_INCLUSIVE = 1;
    static const UINT16 BITMASK_LOWER_BITS_EXCLUSIVE = 2;
    static const UINT16 BITMASK_UPPER_BITS_EXCLUSIVE = 3;
    static const UINT16 BITMASK_LOWER_BITS_INCLUSIVE = 4;

    static const INT16 INT_TYPE_NONE             = -1;
    static const INT16 INT_TYPE_SIMPLE_SIGNED    =  0;
    static const INT16 INT_TYPE_UNSIGNED         =  1;
    static const INT16 INT_TYPE_OFFSET_SIGNED    =  2;
    static const INT16 INT_TYPE_TWOS_COMP_SIGNED =  3;

    static const UINT16 VECTOR_TYPE_NONE        = 0;
    static const UINT16 VECTOR_TYPE_FIXED       = 1;
    static const UINT16 VECTOR_TYPE_VARIABLE    = 2;

    // greatest UINT16 value used to signify non-match in enumerated grammar
    static const UINT16 ENUM_NON_MATCH = (UINT16)0xFFFF;

    /**
     * Constructor. Construct a new ReplyInterpreter for each
     * reply you wish to interpret.  Not suited for use as a
     * class iVar.
     *
     * @param xmlnode The node you should construct with should be
     * the child of the ReplyInterpretation child-node you are using
     * to interpret.  In following example XML, you would pass in the
     * child of the VINAvailable node, ie. the boolean node.
     */
    ReplyInterpreter(const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Empty constructor
     */
    ReplyInterpreter();

    /**
     * Destructor
     * @since 15 Feb 2003
     */
    virtual ~ReplyInterpreter();

    /**
     * Sets the XML node used for interpreting the next message
     *
     * @param newNode The new Xml Node to use for interpretation
     * @throws XmlException if there is an error with the XML
     */
    virtual void setConfigNode(const XmlNode *newNode)
        throw(XmlException);

    /**
     * Check if root XML node's child elements contain Enumerated grammar
     *
     * @throws XmlException if there is an error with the XML used
     * in interpretation.
     * @since 15 Feb 2003
     */
    virtual bool isInterpretTypeEnum()
        throw(XmlException);

    /**
     * Check if root XML node's child elements contain Boolean grammar
     *
     * @throws XmlException if there is an error with the XML used
     * in interpretation.
     * @since 15 Feb 2003
     */
    virtual bool isInterpretTypeBool()
        throw(XmlException);

    /**
     * Check if root XML node's child elements contain String grammar
     *
     * @throws XmlException if there is an error with the XML used
     * in interpretation.
     * @since 15 Feb 2003
     */
    virtual bool isInterpretTypeStr()
        throw(XmlException);

    /**
     * Check if root XML node's child elements contain Signed Integer grammar
     *
     * @throws XmlException if there is an error with the XML used
     * in interpretation.
     * @since 15 Feb 2003
     */
    virtual bool isInterpretTypeSignedInt()
        throw(XmlException);

    /**
     * Check if root XML node's child elements contain Unsigned Integer grammar
     *
     * @throws XmlException if there is an error with the XML used
     * in interpretation.
     * @since 15 Feb 2003
     */
    virtual bool isInterpretTypeUnsignedInt()
        throw(XmlException);

    /**
     * Check if root XML node's child elements contain Float grammar
     *
     * @throws XmlException if there is an error with the XML used
     * in interpretation.
     * @since 15 Feb 2003
     */
    virtual bool isInterpretTypeFlt()
        throw(XmlException);

    /**
     * Check if root XML node's child elements contain Vector grammar
     *
     * @throws XmlException if there is an error with the XML used
     * in interpretation.
     * @since 01 Mar 2005
     */
    virtual bool isInterpretTypeVector()
        throw(XmlException);

    /**
     * Wrapper method for processing Enumerated grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     * @note return of 0 is reserved for non-match condition
     */
    virtual UINT16 enumEvaluate(SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Boolean grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool boolEvaluate(SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing String grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual string strEvaluate(SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Integer grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual INT32  intSignedEvaluate(SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Integer grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 intUnsignedEvaluate(SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Floating point grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual double fltEvaluate(SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<bool> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<string> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<UINT8> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<UINT32> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<UINT16> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<INT32> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<double> &vectorStorage)
        throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon contents of SerialString_t
     *
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<float> &vectorStorage)
    throw(XmlException);

protected:
    /**
     * Wrapper method for processing Enumerated grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     * @note return of 0 is reserved for non-match condition
     */
    virtual UINT16 enumEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Boolean grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool boolEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing String grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual string strEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Integer grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 intEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Floating point grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual double fltEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void boolVectorEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray,vector<bool> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void strVectorEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray,vector<string> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void ucharVectorEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray,vector<UINT8> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void uintVectorEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray,vector<UINT32> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void uintVectorEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray,vector<UINT16> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void intVectorEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray,vector<INT32> &vectorStorage)
    throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void fltVectorEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray,vector<double> &vectorStorage)
        throw(XmlException);

    /**
     * Wrapper method for processing Vector grammar of XML
     * node input upon bytes in SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void fltVectorEvaluate(const XmlNode * xmlnode, SerialString_t &byteArray,vector<float> &vectorStorage)
    throw(XmlException);

    /**
     * Process Enumerated grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT16 enumProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Process Boolean grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool boolProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Process String grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual string strProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
    throw(XmlException);

    /**
     * String grammar Helper for Integer types
     *
     * @param xmlnode   The XML node from the configuration file.
     * @param formatStr The format string from the configuration file.
     * @param byteArray The byte array from the module message.
     * 
     * @throws XmlException if the XML used to interpret the string is 
     *         incorrectly formed
     */
    virtual string strProcessGrammarInt (const XmlNode * xmlnode,const string &formatStr,SerialString_t &byteArray)
    throw(XmlException);

    /**
     * String grammar Helper for float types
     *
     * @param xmlnode   The XML node from the configuration file.
     * @param formatStr The format string from the configuration file.
     * @param byteArray The byte array from the module message.
     * 
     * @throws XmlException if the XML used to interpret the string is 
     *         incorrectly formed
     */
    virtual string strProcessGrammarFlt (const XmlNode * xmlnode,const string &formatStr,SerialString_t &byteArray)
    throw(XmlException);

    /**
     * String grammar Helper for ASCII byte extraction.
     * Method Changed Oct 10 2003 to allow substitution
     * of entire strings instead of just one character.
     *
     * @param xmlnode
     * @param byteArray
     *
     * @return
     * @throws XmlException
     *                if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual string strProcessGrammarChars (const XmlNode * xmlnode,SerialString_t &byteArray)
    throw(XmlException);

    /**
     * String grammar Helper for ASCII byte extraction.
     * Method Added Dec 13 2005 to allow substitution
     * based more than one byte of characters
     *
     * @param xmlnode
     * @param byteArray
     *
     * @return
     * @throws XmlException
     *                if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */     
    virtual string strProcessGrammarCharsArray (const XmlNode * xmlnode,SerialString_t &byteArray)
    throw(XmlException);
    
    /**
     * Process Integer grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 intProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Integer grammar Helper for Unsigned int extraction
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 intProcessGrammarUnsigned (const XmlNode * xmlnode,SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Integer grammar Helper for Simple Signed int extraction
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual INT32 intProcessGrammarSimpleSigned (const XmlNode * xmlnode,SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Integer grammar Helper for Offset Signed int extraction
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual INT32 intProcessGrammarOffsetSigned (const XmlNode * xmlnode,SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Integer grammar Helper for Two's Complement Signed int extraction
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual INT32 intProcessGrammarTwosCompSigned (const XmlNode * xmlnode,SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Process Floating point grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual double fltProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray)
        throw(XmlException);

    /**
     * Process Vector grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void boolVectorProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray,vector<bool> &vectorStorage)
    throw(XmlException);

    /**
     * Process Vector grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void strVectorProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray,vector<string> &vectorStorage)
    throw(XmlException);

    /**
     * Process Vector grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void ucharVectorProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray,vector<UINT8> &vectorStorage)
    throw(XmlException);

    /**
     * Process Vector grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void uintVectorProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray,vector<UINT32> &vectorStorage)
    throw(XmlException);

    /**
     * Process Vector grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void uintVectorProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray,vector<UINT16> &vectorStorage)
    throw(XmlException);

    /**
     * Process Vector grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void intVectorProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray,vector<INT32> &vectorStorage)
    throw(XmlException);

    /**
     * Process Vector grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void fltVectorProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray,vector<double> &vectorStorage)
        throw(XmlException);

    /**
     * Process Vector grammar of XML node input upon bytes in
     *  SerialString_t
     *
     * @param xmlnode
     * @param byteArray
     * @param vectorStorage
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    void fltVectorProcessGrammar (const XmlNode * xmlnode,SerialString_t &byteArray,vector<float> &vectorStorage)
    throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'AND'
     *
     * @param opElement
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementAND (const XmlNode * opElement)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'OR'
     *
     * @param opElement
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementOR (const XmlNode * opElement)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'XOR'
     *
     * @param opElement
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementXOR (const XmlNode * opElement)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'NOT'
     *
     * @param opElement
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementNOT (const XmlNode * opElement)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Enum'
     *
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementEnum (const XmlNode * element)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Integer'
     *
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementInteger (const XmlNode * element)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Float'
     *
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementFloat (const XmlNode * element)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Substring'
     *
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementSubstring (const XmlNode * element)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Subvector'
     *
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 01 Mar 2005
     */
    virtual bool isElementSubvector (const XmlNode * element)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Chars'
     *
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementChars (const XmlNode * element)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Chars'
     *
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isElementCharsArray (const XmlNode * element)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Byte'
     *
     * @param node
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isNodeByte (const XmlNode * node)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Chars'
     *
     * @param node
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isNodeFixed (const XmlNode * node)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Substitute'
     *
     * @param node
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isNodeSubstitute (const XmlNode * node)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'StringSubstitute'
     *
     * @param node
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool isNodeStringSubstitute (const XmlNode * node)
        throw(XmlException);

    /**
     * Determine if given XmlNode has name of 'Compare'
     *
     * @param node
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 5 Apr 2006
     */
    virtual bool isNodeCompare (const XmlNode * node)
        throw(XmlException);
    /**
     * Extract Enumerated value from XmlNode's enum attribute
     *
     * @param enumElement
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT16 getEnumVal (const XmlNode * enumElement)
        throw(XmlException);

    /**
     * Extract priority value from XmlNode's match_priority attribute
     *
     * @param enumElement
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT16 getPriorityVal (const XmlNode * enumElement)
        throw(XmlException);

    /**
     * Extract actual bit size from XmlNode's 'bits' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT16 getBitSize (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual multiplication factor from XmlNode's 'scale' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual double getScaleVal (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual offset from XmlNode's 'offset' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual double getOffsetVal (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual mask byte from XmlNode's 'mask' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT8 getMaskVal (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual comparison byte from XmlNode's 'compare' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT8 getCompareVal (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual length from XmlNode's 'length' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT16 getLengthVal (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual replacement byte from XmlNode's 'replace' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT8 getReplaceValUINT8 (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual replacement string from XmlNode's 'replace' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 13 Oct 2003
     */
    virtual string getReplaceValString (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual actual condition string from XmlNode's 'condition' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual string getConditionStr (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual numeric index from XmlNode's 'input_index' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 getInputIndexVal (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual numeric index from XmlNode's 'output_index' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 getOutputIndexVal (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Extract actual numeric index from XmlNode's 'startbyte' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 getStartByteIndexVal (const XmlNode * xmlnode)
    throw(XmlException);

    /**
     * Extract actual numeric value from XmlNode's 'offset' attribute
     *
     * @param xmlnode
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 getVectorByteOffsetVal (const XmlNode * xmlnode)
    throw(XmlException);

    /**
     * Safely extract byte from input buffer
     *
     * @param byteArray
     * @param index
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT8 getInputArrayVal (SerialString_t &byteArray, UINT32 index)
        throw(XmlException);

  /**
     * Compare XmlNode and given name
     *
     * @param xmlnode
     * @param name
     * @param chars
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool compareNodeToName (const XmlNode * xmlnode,const string &name, unsigned int chars)
        throw(XmlException);

  /**
     * Compare specified XmlNode attribute's value to given value
     *
     * @param xmlnode
     * @param attribute
     * @param value
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual bool compareAttributeValueToString (const XmlNode * xmlnode,const string &attribute,const string &value)
        throw(XmlException);

    /**
     * Sign checking in integer processing
     *
     * @param xmlnode The XML node to examine.
     * 
     * @throws XmlException if the XML used to interpret the string is 
     *         incorrectly formed
     */
    virtual bool isIntegerSigned (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Sign checking in integer processing
     *
     * @param xmlnode The XML node to examine.
     * 
     * @throws XmlException if the XML used to interpret the string is 
     *         incorrectly formed
     */
    virtual bool isIntegerUnsigned (const XmlNode * xmlnode)
        throw(XmlException);

    /**
     * Parse and return Integer type
     *
     * @note types are defined in .cpp file
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual INT16 getIntegerTypeFromAttribute (const XmlNode * element)
        throw(XmlException);

    /**
     * Parse and return Integer type
     *
     * @note types are defined in .cpp file
     * @param element
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT16 getVectorTypeFromAttribute (const XmlNode * element)
    throw(XmlException);

    /**
     * Extract UINT8 from specified XmlNode attribute
     *
     * @param xmlnode
     * @param attribute
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT8 getUINT8FromHexAttribute (const XmlNode * xmlnode,const string &attribute)
        throw(XmlException);

    /**
     * Extract UINT16 from specified XmlNode attribute
     *
     * @param xmlnode
     * @param attribute
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT16 getUINT16FromAttribute (const XmlNode * xmlnode,const string &attribute)
        throw(XmlException);

    /**
     * Extract UINT32 from specified XmlNode attribute
     *
     * @param xmlnode
     * @param attribute
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 getUINT32FromAttribute (const XmlNode * xmlnode,const string &attribute)
        throw(XmlException);

    /**
     * Extract float from specified XmlNode attribute
     *
     * @param xmlnode
     * @param attribute
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual double getFloatFromAttribute (const XmlNode * xmlnode,const string &attribute)
        throw(XmlException);

    /**
     * Extract value string from specified XmlNode attribute
     *
     * @param xmlnode
     * @param attribute
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual string getStringFromAttribute (const XmlNode * xmlnode,const string &attribute)
        throw(XmlException);

    /**
     * Create Bit masks for use in integer processing
     *
     * @param bitNum
     * @param maskType
     * @throws XmlException if the XML used to interpret the
     * string is incorrectly formed
     * @since 15 Feb 2003
     */
    virtual UINT32 createBitMask (UINT16 bitNum, UINT16 maskType)
        throw(XmlException);

    /**
     * local copy of XmlNode encountered in constructor
     */
    const XmlNode * m_rootNodePtr;

};

#endif
/*
 * Local Variables: ***
 * mode: c++ ***
 * eval: ( c-set-style "bepco") ***
 * End: ***
 */












