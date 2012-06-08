//*************************************************************************
// Description:
//   Interface class used to communicate with WinCcrtInterface
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Core/ServicesLayer/Include/WinCcrtInterface.h $
// 
// 1     6/7/12 3:49p Tkelch
// Initial creation and development of interface file
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#ifndef IWinInterfaceServer_h
#define IWinInterfaceServer_h

#include "IBepCommunication.h"
#include <string.h>

/**
 * Class used to communicate with the WinCcrtInterface
 *
 * @author Tim Kelch
 * @version 1.0
 * @see IBepCommunication
 * @since Version 1.0
 */
class IWinInterfaceServer : protected IBepCommunication
{
public:
	/**
	 * Class constructor (default)
	 *
	 * @param debug  If debugging information should be enabled
	 * @since Version 1.0
	 */
	IWinInterfaceServer(void);

    /**
	 * Class destructor
	 * @since Version 1.0
	 */
	virtual ~IWinInterfaceServer(void);
   
	/**
	 * Sends a string to the selected interface
	 *
	 * @param messageTag    Data string that should be sent
	 *
	 * @return The status of the send function
	 * @since Version 1.0
	 */
	virtual const BEP_STATUS_TYPE SendMessage(std::string messageTag);
   
	/**
	 * Wait for the string response
	 *
	 * @param messageTag    Data string that should be sent
	 *
	 * @return The status of the send function
	 * @since Version 1.0
	 */   
   virtual int WaitForFullResponse(SerialString_t &response);
};
#endif // IWinInterfaceServer_h
