//******************************************************************************
// Description:
//   Host Interface to send results to MQS.  If results cannot be sent, they
//   will be stored and tried again at a later time.
//
//===========================================================================
// Copyright (c) 2015- Burke E. Porter Machinery
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
#ifndef MqsResultInterface_h
#define MqsResultInterface_h
//-------------------------------------------------------------------------------------------------
#include "HostInterface.h"
//-----------------------------------------------------------------------------

class MqsResultInterface : public HostInterface
{
public:
	MqsResultInterface();
	virtual ~MqsResultInterface();



protected:
	/**
	 * Store additional items from the config file to complete the
	 * initialization of the host interface.
	 *
	 * @param config Configuration data used to complete the initialization of
	 *               the host interface.
	 */
	virtual void LoadAdditionalConfigurationItems(const XmlNode *config);

	/**
	 * Transmit the result string to the plant host system system.
	 *
	 * @param result The result string to be sent.
	 */
	virtual void SendResultToHost(const string &result);

	/**
	 * Send the test result string to the external host.
	 *
	 * @param resultString
	 *                 Test result string to be sent.
	 * @param portComm Port object to send the test result string.
	 * @param ackChar  The acknowledgement character to look for.
	 * @param maxAttempts Maximum number of times to try to successfully send
	 *                    the message.
	 * @param hostResponseTime
	 *                 Length of time (in ms) to wait for a response from the
	 *                 external host.
	 *
	 * @return Status of the message transmit operation.
	 */
	virtual const BEP_STATUS_TYPE SendTestResultString(string &resultString, 
													   SerialChannel &portComm, 
													   const INT32 &maxAttempts, 
													   const INT32 &hostResponseTime);



private:

	/**
	 * Get/Set the name of the file that contains the failed transmit results
	 * 
	 * @param fileName  Name of the file that will contain the failed transmit results.
	 * 
	 * @return  Name of the file that will contain the failed transmite results.
	 */
	const string& FailedTxFileName(const string *fileName = NULL);

	string m_failedTxFileName;
};

//-------------------------------------------------------------------------------------------------
#endif //MqsResultInterface_h
