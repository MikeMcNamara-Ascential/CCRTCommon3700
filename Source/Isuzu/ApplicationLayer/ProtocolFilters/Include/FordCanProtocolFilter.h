//******************************************************************************
//
// $Archive: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/ProtocolFilters/Include/FordCanProtocolFilter.h $
// $Author: Mmcnamar $
//   $Date: 2/07/11 4:45p $
//
// Description:
//    Protocol filter for the Ford modules on a CAN bus
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/ProtocolFilters/Include/FordCanProtocolFilter.h $
// 
// 1     2/07/11 4:45p Mmcnamar
// backup c drive source for cross compiler
// 
// 1     2/07/11 4:44p Mmcnamar
// backup c drive
// 
// 1     5/10/10 2:09p Mmcnamar
// initial check-in for new core.
// 
// 4     11/14/08 3:24p Mmcnamar
// Added prototypes for SendMessage( ) method that takes in arguments;
// Added prototype for GetModuleData().
// 
// 3     5/01/06 12:18p Bmeinke
// Added SendMessage(std::string) to comply with latest core
// 
// 2     6/13/05 9:18a Bmeinke
// Override Initialize() to read new TX object number to be used for new
// short form transmit message format
// Override SendMessage(SerialString_t) to prepend TX object number to the
// beginning of the transmit string used for new short form transmit
// message format
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:42p Bmeinke
// Initial full core integration for Ford
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef FordCanProtocolFilter_h
#define FordCanProtocolFilter_h

#include "ProtocolFilter.h"

class FordCanProtocolFilter : public ProtocolFilter
{
public:
	/**
	 * Class constructor.
	 * @since 12 Jun 2003
	 */
	//FordCanProtocolFilter(BepMutex *commsInUse=NULL);
    //MAM 4/26/10 - For new core
    FordCanProtocolFilter(KeepAliveTimer_t &lastTxTime, StopCommsBepCondVar *stopCommsBepCondVar = NULL, BepMutex *commPortInUse = NULL);

	/**
	 * Initialize the serial channel.
	 *
	 * @param config The configuration to use for the channel initialization.
	 * @return true if the initialization is successful.
	 * @since Version 1.0
	 */
	virtual bool Initialize(const XmlNode *config);

    //mam 10/29/08
    /**
	 * Send a message to a vehicle module.
	 *
	 * @param messageTag Name of the message to be sent.
	 * @param args       Extra data to be added to the message.
	 * @return Status of the opeation.
	 * @since Version 1.0
	 */
	virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag, SerialArgs_t &args);

	/**
	 * Send a message to a vehicle module.
	 *
	 * @param message Message to be sent.
	 * @return Status of the send.
	 * @since Version 1.0
	 */
	virtual const BEP_STATUS_TYPE SendMessage(SerialString_t &message);
	
	/**
	 * Send a message to vehicle module.
	 *
	 * @param messageTag Name of message to be retrieved and sent.
	 * @return Status of the operation.
	 * @since Version 1.0
	 */
	virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag);
	
	/**
	 * For the Ford CAN protocol filter we dont' actually
	 * add a checksum, so we are overridding this message
 	 * to not do anything
	 * @since 16 Jun 2003
	 */
	virtual void AddChecksumToMessage(SerialString_t &message);
	
	/**
	 * Extract the data from the response.
	 *
	 * @param moduleResponse
	 *               Response from the module.
	 * @return The data that was included in the response.
	 * @since Version 1.0
	 */
	virtual const SerialString_t ExtractModuleData(SerialString_t &moduleResponse);
    virtual const SerialString_t ExtractModuleData2(SerialString_t &moduleResponse);

    //MAM 10/29/08
    /**
	 * Send a message to the module and read the response.
	 *
	 * @param messageTag Tag of message to send.
	 * @param reply      Reply from the module.
	 * @param args       Optional data to be written to the module.
	 * @return Status of the operation.
	 * @since Version 1.0
	 */
	virtual const BEP_STATUS_TYPE GetModuleData(std::string messageTag,
												SerialString_t &reply,
												SerialArgs_t *args = NULL);
    //MAM 1/22/15
    virtual const BEP_STATUS_TYPE GetModuleDataNew(std::string messageTag,
												SerialString_t &reply,
												SerialArgs_t *args = NULL);

    virtual const BEP_STATUS_TYPE GetBusBroadcastMessage(string messageTag,
												      const long messageWaitTime,
													  SerialString_t &busMssg);

protected:
	/**
	 * AVT-718 Transmit object number to use when sending messages
	 */
	uint8_t		m_avtTxObjNo;

	/**
	 * Flag indicating if we want have a vlid TX object number to use.
	 * IF we don't, no TX object will be added to the messages we send
	 * to the AVT comm proxy
	 */
	bool		m_useTxObjNo;
};

#endif //FordCanProtocolFilter_h
