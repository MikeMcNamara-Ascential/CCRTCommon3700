//*************************************************************************
// Description:
//   Tata Test Result Server to handle all test results.
//
//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// $Log$
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "TeslaTestResultServer.h"

//-------------------------------------------------------------------------------------------------
TeslaTestResultServer::TeslaTestResultServer() : TestResultServer(), m_ndb(NULL)
{   // NOthing special to do here
}

//-------------------------------------------------------------------------------------------------
TeslaTestResultServer::~TeslaTestResultServer()
{
	if(m_ndb != NULL)  delete m_ndb;
}

//-------------------------------------------------------------------------------------------------
void TeslaTestResultServer::Initialize(const XmlNode *document)
{   // Call the base class to begin the initialization
	TestResultServer::Initialize(document);
	// Create a new NDB interface
	if(m_ndb != NULL)   delete m_ndb;
	m_ndb = new INamedDataBroker();
}

//-------------------------------------------------------------------------------------------------
const string TeslaTestResultServer::Publish(const XmlNode *node)
{
	string result(BEP_ERROR_RESPONSE);
	XmlNode driveCurveNode("DriveCurve", "Unknown");
	Log(LOG_FN_ENTRY, "TeslaTestResultServer::Publish(node: %s) - Enter", node->ToString().c_str());
	// Let the base class handle it
	result = TestResultServer::Publish(node);
	// Determine if the operator notes need to be cleared
	if(!node->getName().compare("OverallTest") && node->getValue().compare(BEP_TESTING_RESPONSE))
	{
		string response;
		m_ndb->Write(string("Notes"), string(""), response, true);
	}
	Log(LOG_FN_ENTRY, "TeslaTestResultServer::Publish(node: %s) - Exit", node->ToString().c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
const string TeslaTestResultServer::NdbRead(const string &tag)
{
	string value, response;
	INT32 status = m_ndb->Read(tag, response, true);
	if(BEP_STATUS_SUCCESS == status)
	{
		status = m_ndb->GetByTag(tag, value, response);
	}
	if(BEP_STATUS_SUCCESS != status)
	{
		Log(LOG_ERRORS, "Error reading %s: %s", tag.c_str(), ConvertStatusToResponse(status).c_str());
		value = BEP_ERROR_RESPONSE;
	}
	return value;
}
