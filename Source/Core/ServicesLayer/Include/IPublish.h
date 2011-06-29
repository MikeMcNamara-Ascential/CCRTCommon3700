//******************************************************************************
// Description:
//	Definition for the Publish Interface class
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
//    $Log: /Core/ServicesLayer/Include/IPublish.h $
// 
// 2     6/09/04 11:30p Cward
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef IPublish_h
#define IPublish_h
//==============================================================================

#include <string>

#include "BepDefs.h"
#include "XmlNode.h"

/**
 * Interface that defines the ability to publish data
 *
 * @author Tim Bochenek
 * @author Carl Erickson
 * @version 1.2
 * @since Version 1.0
 */
class IPublish
{
public:
	/**
	 * Class destructor
	 * @since Version 1.1
	 */
	virtual ~IPublish(void)
	{
	}
	/**
	 * Remove from subscription list for notification of a specified tag
	 *
	 * @param tag    Tag to publish to subcribers
	 * @param value    Server name subscribed for data
	 * @param response Data returned by operation
	 * @param IsFinal If this tag is the las tag to be requested
	 * @return Status of publish operation
	 * @since Version 1.0
	 */
	virtual INT32 Publish(const std::string &tag, const std::string &value,
						  std::string &response, const bool IsFinal = false) = 0;
	/**
	 * Remove from subscription list for notification of a specified tag
	 *
	 * @param node   Node to be Published.
	 * @param response Data returned by operation
	 * @param IsFinal If this tag is the las tag to be requested
	 * @return Status of publish operation
	 * @since Version 2.4
	 */
	virtual INT32 Publish(const XmlNode *node, std::string &response, const bool IsFinal = false) = 0;
	/**
	 * Create a communication object connected to name
	 *
	 * @param name    Name of server
	 * @param type    Type of object (Client/Server)
	 * @param debug   Diagnostic print variable
	 * @param size    Size of receive buffer to create (>= DEFAULT_BUFFER_SIZE)
	 * @param timeout Timeout to wait when sending a message (>= DEFAULT_TIMEOUT)
	 * @since Version 2.7
	 */
	virtual void Initialize(const std::string &name, const std::string &type,
							const bool debug = false,
							const INT32 size = DEFAULT_BUFFER_SIZE,
							const INT32 timeout = DEFAULT_TIMEOUT) = 0;
	/**
	 * Get the named channel the object is using.
	 *
	 * @return Name of the channel (IPC only).
	 * @since Version 1.6
	 */
	virtual const std::string GetName(void) = 0;
	/**
	 * Closes communication channel object is connected to
	 * @since Version 1.2
	 */
	virtual void Close(void) const = 0;
};
#endif // IPublish_h
