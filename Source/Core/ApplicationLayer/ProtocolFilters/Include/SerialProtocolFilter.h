// Description:
//  Serial ProtocolFilter for communicating with vehicle modules.
//  This protocol filter just sends the raw data and expects raw data back.
//  No Checksum is required and there is no data header required.
//
//==============================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#ifndef SerialProtocolFilter_h
#define SerialProtocolFilter_h
//-------------------------------------------------------------------------------------------------
#include "ProtocolFilter.h"

class SerialProtocolFilter : public ProtocolFilter
{
public:
	SerialProtocolFilter(KeepAliveTimer_t &lastTxTime, 
						 StopCommsBepCondVar *stopCommsBepCondVar = NULL, 
						 BepMutex *commsInUse = NULL);
	virtual ~SerialProtocolFilter();

    /**
     * Check if the message received from the module is valid.
     *
     * @param moduleResponse
     *               Response received from the module
     * @return Status of the operation.
     */
    virtual const BEP_STATUS_TYPE CheckForValidResponse(const SerialString_t &moduleResponse);

	/**
	 * Extract the data from the response.
	 *
	 * @param moduleResponse
	 *               Response from the module.
	 * @return The data that was included in the response.
	 */
	const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);

	/**
	 * Send a message to a vehicle module.
	 *
	 * @param message Message to be sent.
	 * @return Status of the send.
	 */
	virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message);

	/**
	 * Send a message to vehicle module.
	 *
	 * @param messageTag Name of message to be retrieved and sent.
	 * @return Status of the operation.
	 */
	virtual const BEP_STATUS_TYPE SendMessage(string messageTag);

	/**
	 * Send a message to a vehicle module.
	 *
	 * @param messageTag Name of the message to be sent.
	 * @param args       Extra data to be added to the message.
	 * @return Status of the opeation.
	 */
	virtual const BEP_STATUS_TYPE SendMessage(string messageTag, SerialArgs_t &args);
};

//-------------------------------------------------------------------------------------------------
#endif //SerialProtocolFilter_h
