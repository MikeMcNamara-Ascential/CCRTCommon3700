//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Core/ServicesLayer/Library/Subscriber.cpp 5     4/13/06 3:45a Cward $
//
// FILE DESCRIPTION:
//		This is a class which handles subscriptions for an active client.
//
//===========================================================================
// Copyright (c) 2002 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/Subscriber.cpp $
// 
// 5     4/13/06 3:45a Cward
// Init some vars
//
// 2     2/15/06 3:57p Bmeinke
// Initialize some uninitialized variables
//
// 4     10/27/04 6:46p Cward
// Changes for 04102701 build. Primarily to back out most of the
// Dataserver changes mad in the previous (04101401) build.
//
// 3     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
//
// 1     12/21/03 6:27p Cward
//
// 1     10/07/03 11:35a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/12/03 3:58p Khykin
// Initial checkin.
//
// 1     4/22/03 9:51a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:06:33  tbochene
//
//    PooBag
//
//
// 1     1/27/03 7:18a Khykin
// Initial Checkin
//
// 1     1/18/03 8:12a Khykin
// Initial Checkin From DCTC
//
// 7     7/31/02 12:10p Skypig13
// Updated so subscriber will not be added unless communication is ok
//
// 6     6/19/02 3:03p Skypig13
// Added printout in HandleRequest()
//
// 5     4/25/02 4:46p Skypig13
// Updated with new changes
//    Revision 1.1.1.1  2002/04/08 19:49:01  erickson
//    started
//
//
// 3     4/08/02 1:46p Skypig13
// Added header
//
//*************************************************************************

#include "Subscriber.h"
#include "INamedDataBroker.h"
#include "SubscriptionManager.h"



Subscriber::Subscriber(void) : Debuggable()
{
}

Subscriber::Subscriber(const bool debug) : Debuggable(debug)
{
}

Subscriber::~Subscriber(void)
{
}


const INT32 Subscriber::HandleRequest(XmlNode* data, IPublish* comm, std::string& message)
{
	UINT32 ii = 0;
	INT32 stat = BEP_STATUS_SUCCESS;			  // Counter and status of operation
	std::string tag, value, response;
	if(IsDebugOn())	printf("Subscriber::HandleRequest(%s, %p)\n", data->ToString().c_str(), comm);

	INamedDataBroker iNdb(IsDebugOn());
	const XmlNodeMap Nodes = data->getChildren();
	for(XmlNodeMapCItr iter = Nodes.begin(); iter != Nodes.end(); ++iter, ii++)
	{	// Subscribe for the data and send response to the subscription manager
		stat = iNdb.Subscribe(iter->second->getName(), SubscriptionManager::GetManager()->GetName(), message, (ii == (Nodes.size() -1)));
	}
	if(stat == BEP_STATUS_SUCCESS)
	{
		for(XmlNodeMapCItr iter = Nodes.begin(); iter != Nodes.end(); ++iter, ii++)
		{	// Subscribe for the data and send response to the subscription manager
			iNdb.GetNext(tag, value, response);
			if(iNdb.IsDataOk(tag) && iNdb.IsDataOk(value))
			{
				SubscriptionManager::GetManager()->AddSubscriber(data, comm);
			}
		}
	}
	if(IsDebugOn())	printf("Subscriber::HandleRequest(%s, %p, %s) done\n", data->ToString().c_str(), comm, message.c_str());
	return(stat);
}

