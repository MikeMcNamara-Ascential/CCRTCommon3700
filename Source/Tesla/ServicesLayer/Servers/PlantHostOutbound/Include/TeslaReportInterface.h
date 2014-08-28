//******************************************************************************
// Description:
//   Host Interface base lass to process a generic text result with no header or checksum
//   and send to a host system.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef TeslaReportInterface_h
#define TeslaReportInterface_h
//-----------------------------------------------------------------------------
#include "HostInterface.h"
//-----------------------------------------------------------------------------


/**
 * Generic Text Interface class.
 * This class will process the test result data and create a generic string.
 * No special header will be added to the result string.  No trailer data or checksum will be
 * added to the result string.  The result string will be transmitted out the port.  No ACK
 * or response is expected from the host system.  If the message is successfully transmitted,
 * it will be assumed the host system received the message.
 * 
 * @author Dan Erickson
 * @see HostInterface
 * @since March 10, 2008
 */
class TeslaReportInterface : public HostInterface
{
public:
    /**
     * Class constructor.
     */
    TeslaReportInterface();
    /**
     * Class destructor.
     */
    virtual ~TeslaReportInterface();

protected:
    /**
     * Send the test result string to the external host.
     *
     * @param resultString
     *                 Test result string to be sent.
     * @param portComm Port object to send the test result string.
     * @param maxAttempts Maximum number of times to try to successfully send
     *                    the message.
     *
     * @return Status of the message transmit operation.
     */
    BEP_STATUS_TYPE SendTestResultString(const string &resultString, 
                                         SerialChannel &portComm, 
                                         const INT32 &maxAttempts);

    /**
     * Transmit the result string to the plant host system system.
     *
     * @param result The result string to be sent.
     */
    virtual void SendResultToHost(const string &result);
    /**
     * Process the node from the map specifying how to build the result string.
     *
     * @param resultMapNode Result string field to build.
     * @param testResults
     *                The reported test results.
     * @param vehicleData
     *                The vehicle build information from the tested vehicle.
     * @param resultConversions  The map specifying test result conversion. 
     * @param totalSerialNumbers
     *                The total number of Serial numbers reported in the test 
     *                results.
     * @return The data to place in the current result string field.
     */
    virtual const string ProcessMapNode(const XmlNode *resultMapNode,
                                        const XmlNodeMap &testResults,
                                        const XmlNodeMap &vehicleData,
                                        const XmlNodeMap &resultConversions,
                                        INT32 &totalSerialNumbers);

    string GetMathResult(const XmlNodeMap &testResultMap, const XmlNode *testResultStringField);

    string GetDetailAttributeValue(const XmlNodeMap &testResultMap, const XmlNode *testResultStringField);

    virtual string ProcessDtcNode(const XmlNode *dtcFormat, 
                                  const XmlNodeMap &testResults,
                                  const XmlNodeMap &vehicleData,
                                  const XmlNodeMap &resultConversions,
                                  INT32 &totalSerialNumbers);
};

//-----------------------------------------------------------------------------
#endif //TeslaReportInterface_h
