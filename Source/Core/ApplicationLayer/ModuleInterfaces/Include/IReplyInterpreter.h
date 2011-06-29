//******************************************************************************
// Description:
//  IReplyInterpreter is a generic interface for reading
//  XML ReplyInterpretation blocks and extracting data from
//  blocks of raw data returned from a module after a read
//
//==============================================================================
// Copyright (c) 2004- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/ModuleInterfaces/Include/IReplyInterpreter.h $
// 
// 1     11/16/07 1:24p Derickso
// Updated comments to doxygen format.
// 
// 5     10/30/07 1:33p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added a UINT8 vectorEvaluate() method. 
// - Added a UINT16 vectorEvaluate() method.
// - Added a float vectorEvaluate() method.
// 
// 4     5/11/05 10:03p Cward
// Updated for 05051101 iteration
//
// 3     5/09/05 5:00p Gpattison
// Added Vector and variable length vector support
//
// 3     8/18/04 11:05p Cward
// Changed angle brackets to quotes for includes of non system files
//
// 2     6/09/04 10:45p Cward
// Added new standard header comment block
//
// 1     12/21/03 6:32p Cward
//
// 2     10/27/03 1:31p Bmeinke
// Fixed ParkBrake / ParkPawl test failures
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
#ifndef IMODULEINTERPRETER_H
#define IMODULEINTERPRETER_H
//==============================================================================
#include <string>
#include "XmlNode.h"
#include "BepDefs.h"
#include "SerialDefs.h"
//-----------------------------------------------------------------------------

/**
 * Reply interpretation interface class.
 * 
 * @author Chris TenHarmsel
 * @author Mike Karlesky
 * @author Bill Bereza
 */
class IReplyInterpreter
{
public:
    /**
     * Class constructor.
     * 
     * @param node   Reply interpretation section of the message table.
     */
    IReplyInterpreter(const XmlNode * node)
    {
    };

    /**
     * Class constructor.
     */
    IReplyInterpreter()
    {
    };

    /**
     * Class destructor
     */
    virtual ~IReplyInterpreter()
    {
    };

    /**
     * Store the reply interpretation data.
     * <p><b>Description:</b><br>
     * The reply interpretation section of the message table will be stored.
     * <p>
     * @param newNode The reply interpretation section of the message table.
     */
    virtual void setConfigNode(const XmlNode *newNode) = 0;
    /**
     * Check if this is a boolean interpretation.
     * <p><b>Description:</b><br>
     * The interpretation node will be checked to determine if the data should be translated using a
     * boolean interpretation.
     * <p>
     * 
     * @return Flag indicating if the interpretation is for boolean type data.
     */
    virtual bool isInterpretTypeBool() = 0;

    /**
     * Check if this is an enumeration interpretation.
     * <p><b>Description:</b><br>
     * The interpretation node will be checked to determine if the data should be translated using an
     * enumeration interpretation.
     * <p>
     * 
     * @return Flag indicating if the interpretation is for enumeration type data.
     */
    virtual bool isInterpretTypeEnum() = 0;

    /**
     * Check if this is a string interpretation.
     * <p><b>Description:</b><br>
     * The interpretation node will be checked to determine if the data should be translated using a
     * string interpretation.
     * <p>
     * 
     * @return Flag indicating if the interpretation is for string type data.
     */
    virtual bool isInterpretTypeStr() = 0;

    /**
     * Check if this is a unsigned integer interpretation.
     * <p><b>Description:</b><br>
     * The interpretation node will be checked to determine if the data should be translated using a
     * unsigned integer interpretation.
     * <p>
     * 
     * @return Flag indicating if the interpretation is for unsigned integer type data.
     */
    virtual bool isInterpretTypeUnsignedInt() = 0;

    /**
     * Check if this is a signed integer interpretation.
     * <p><b>Description:</b><br>
     * The interpretation node will be checked to determine if the data should be translated using a
     * signed integer interpretation.
     * <p>
     * 
     * @return Flag indicating if the interpretation is for signed integer type data.
     */
    virtual bool isInterpretTypeSignedInt() = 0;

    /**
     * Check if this is a float interpretation.
     * <p><b>Description:</b><br>
     * The interpretation node will be checked to determine if the data should be translated using a
     * float interpretation.
     * <p>
     * 
     * @return Flag indicating if the interpretation is for float type data.
     */
    virtual bool isInterpretTypeFlt() = 0;

    /**
     * Check if this is a vector interpretation.
     * <p><b>Description:</b><br>
     * The interpretation node will be checked to determine if the data should be translated using a
     * vector interpretation.
     * <p>
     * 
     * @return Flag indicating if the interpretation is for vector type data.
     */
    virtual bool isInterpretTypeVector() = 0;

    /**
     * Process enumeration type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract enumeration data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @return Data translated from the module data string.
     */
    virtual UINT16 enumEvaluate(SerialString_t &byteArray) = 0;

    /**
     * Process boolean type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract boolean data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @return Data translated from the module data string.
     */
    virtual bool boolEvaluate(SerialString_t &byteArray) = 0;

    /**
     * Process string type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract string data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @return Data translated from the module data string.
     */
    virtual string strEvaluate(SerialString_t &byteArray) = 0;

    /**
     * Process signed integer type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract signed integer data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @return Data translated from the module data string.
     */
    virtual INT32  intSignedEvaluate(SerialString_t &byteArray) = 0;

    /**
     * Process unsigned integer type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract unsigned integer data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @return Data translated from the module data string.
     */
    virtual UINT32 intUnsignedEvaluate(SerialString_t &byteArray) = 0;

    /**
     * Process float type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract float data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @return Data translated from the module data string.
     */
    virtual double fltEvaluate(SerialString_t &byteArray) = 0;

    /**
     * Process vector type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract vector data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @param vectorStorage  Vector of data extracted from the module data string.
     * @return Data translated from the module data string.
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<bool> &vectorStorage)= 0;

    /**
     * Process vector type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract vector data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @param vectorStorage  Vector of data extracted from the module data string.
     * @return Data translated from the module data string.
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<string> &vectorStorage) = 0;

    /**
     * Process vector type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract vector data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @param vectorStorage  Vector of data extracted from the module data string.
     * @return Data translated from the module data string.
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<UINT8> &vectorStorage) = 0;

    /**
     * Process vector type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract vector data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @param vectorStorage  Vector of data extracted from the module data string.
     * @return Data translated from the module data string.
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<UINT32> &vectorStorage) = 0;

    /**
     * Process vector type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract vector data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @param vectorStorage  Vector of data extracted from the module data string.
     * @return Data translated from the module data string.
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<UINT16> &vectorStorage) = 0;

    /**
     * Process vector type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract vector data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @param vectorStorage  Vector of data extracted from the module data string.
     * @return Data translated from the module data string.
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<INT32> &vectorStorage) = 0;

    /**
     * Process vector type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract vector data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @param vectorStorage  Vector of data extracted from the module data string.
     * @return Data translated from the module data string.
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<double> &vectorStorage) = 0;

    /**
     * Process vector type data.
     * <p><b>Description:</b><br>
     * The reply interpretation will be process in order to extract vector data from the module response data.
     * <p>
     * 
     * @param byteArray Data extracted from the module response.
     * @param vectorStorage  Vector of data extracted from the module data string.
     * @return Data translated from the module data string.
     */
    virtual void vectorEvaluate(SerialString_t &byteArray, vector<float> &vectorStorage) = 0;
};


//-----------------------------------------------------------------------------
#endif
