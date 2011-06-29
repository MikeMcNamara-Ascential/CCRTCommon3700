//******************************************************************************
//
//     $Header: /Ccrt/Source/Core/ServicesLayer/Library/INamedDataBrokerBase.cpp 3     4/13/06 3:41a Cward $
//
// Description:
//	Base class for client and server interface to Named Data Broker.
//
//==============================================================================
//
//     Copyright © 2001 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Library/INamedDataBrokerBase.cpp $
// 
// 3     4/13/06 3:41a Cward
// String parameters are passed by reference to AddNodeWithAttribute() and
// FindValueInTable()
//
// 2     2/15/06 3:55p Bmeinke
// String parameters are passed by reference to AddNodeWithAttribute() and
// FindValueInTable()
//
// 2     3/11/05 3:32p Cward
// Fix memory leak
//
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "INamedDataBrokerBase.h"


//#include <string>

//#include "XmlNode.h"
//#include "XmlParser.h"
//#include "BepXmlUtils.h"
//#include "IBepCommunication.h"



INamedDataBrokerBase::INamedDataBrokerBase(void)
    : IBepCommunication(), m_serverNum(0)
{
    ClearTables();
}

INamedDataBrokerBase::~INamedDataBrokerBase(void)
{
    ClearTables();
}

void INamedDataBrokerBase::ClearTables(void)
{
    m_data.clear(true);
    m_servers.clear(true); // m_nameProcIdTable
}

bool INamedDataBrokerBase::CheckAttribute(const XmlNode *attribute)
{
    bool status = false;
    if(attribute->getName() == BEP_XML_MSG_TYPE)
    {	// If attribute name is Ok
        const XmlString value = attribute->getValue();
        status = ((value == BEP_VERIFY) |
                  (value == BEP_INVALIDATE) |
                  IBepCommunication::CheckAttribute(attribute));
    }
    return(status);
}

const INT32 INamedDataBrokerBase::GetNextAlias(void)
{
    return(m_serverNum++);
}


inline const std::string  INamedDataBrokerBase::GetServerAlias(const std::string &tag)
{	// Server name (invalid at start)
	std::string alias, name;
	if(IsDebugOn())	printf("GetServerAlias(%s)\n", tag.c_str());
	try
	{	// Get name of process that provides data
		alias = m_data.getNode(tag)->getValue();
		if(IsDebugOn())	printf("Tag: %s server alias: %s\n", tag.c_str(), alias.c_str());
	}
	catch(XmlException &XmlErr)
	{	// Tag and server are not in table (don't use InvalidateServer().  It will be recursive)
		name = FindValueInTable(BEP_UNAVAILABLE_RESPONSE, m_servers);
		if(name.empty())
		{
			char temp[10];
			sprintf(temp, "Alias%d", GetNextAlias());
			alias = temp;
		}
		else alias = name;
		XmlNode dataNode(tag, alias);
		AddTableEntry(&dataNode, m_data);
		SetServerStatus(alias, BEP_UNAVAILABLE_RESPONSE, BEP_UNAVAILABLE_RESPONSE, true);
		if(IsDebugOn())	printf("Tag %s server alias %s set to "BEP_UNAVAILABLE_RESPONSE"\n",
							   tag.c_str(), alias.c_str());
	}
	if(IsDebugOn())	printf("GetServerAlias(%s) done\n", tag.c_str());
	return(alias);
}

//--------------------------------------------------------------------------------------------------
const std::string  INamedDataBrokerBase::GetServerName(const std::string &tag)
{
	std::string alias, name = BEP_UNAVAILABLE_RESPONSE;
	if(IsDebugOn())	printf("GetServerName(%s)\n", tag.c_str());
	try
	{	// Get name of process that provides data
		alias = GetServerAlias(tag);
		name = m_servers.getNode(alias)->getValue();
		if(IsDebugOn())	printf("Tag: %s server alias %s name %s\n", tag.c_str(), alias.c_str(), name.c_str());
	}
	catch(XmlException &XmlErr)
	{	// Tag and server are not in table (don't use InvalidateServer().  It will be recursive)
		char temp[10];
		sprintf(temp, "Alias%d", GetNextAlias());
		alias = temp;
		XmlNode dataNode(tag, alias);
		AddTableEntry(&dataNode, m_data);
		SetServerStatus(alias, BEP_UNAVAILABLE_RESPONSE, BEP_UNAVAILABLE_RESPONSE, true);
	}
	if(IsDebugOn())	printf("Provider for %s data is %s\n", tag.c_str(), name.c_str());
	if(IsDebugOn())	printf("GetServerName(%s) done\n", tag.c_str());
	return(name);
}
inline const std::string INamedDataBrokerBase::GetServerStatus(const std::string &tag, const bool IsServerAlias	/* = false */)
{
	std::string stat = BEP_UNAVAILABLE_RESPONSE,
	alias = tag, name = BEP_UNAVAILABLE_RESPONSE;
	if(IsDebugOn())	printf("GetServerStatus(%s, %d)\n", tag.c_str(), IsServerAlias);
	try
	{	// Get status of server that provides tag
		if(!IsServerAlias)	alias = GetServerAlias(tag);
		if(IsDataOk(alias))
		{
			name = m_servers.getNode(alias)->getValue();
			stat = m_servers.getNode(alias)->getAttributes().getNode(BEP_STATUS)->getValue();
		}
		if(IsDebugOn())	printf("Server: %s for tag: %s is %s\n", name.c_str(), tag.c_str(), stat.c_str());
	}
	catch(XmlException &XmlErr)
	{	// Server was not found
		if(IsDebugOn())	printf("%s is not in status table\n", XmlErr.what());
		SetServerStatus(alias, stat, BEP_UNAVAILABLE_RESPONSE, true);
	}
	if(IsDebugOn())	printf("GetServerStatus(%s) returning: %s\n", tag.c_str(), stat.c_str());
	return(stat);
}

void INamedDataBrokerBase::AddTableEntry(const XmlNode *dataNode, XmlNodeMap &table)
{
	if(IsDebugOn())	printf("AddTableEntry(%s)\n", dataNode->ToString().c_str());
	XmlNode *node;
	try
	{
		node = const_cast<XmlNode *>(table.getNode(dataNode->getName()));
		node->setValue(dataNode->getValue());
		if(IsDebugOn())	printf("Updating node %s\n", dataNode->ToString().c_str());
	}
	catch(XmlException &Err)
	{
		table.addNode(dataNode->Copy());			// Insert into table
		if(IsDebugOn())	printf("Creating node %s \n", dataNode->ToString().c_str());
	}
	if(IsDebugOn())	printf("AddTableEntry(%s) size = %d done\n", dataNode->ToString().c_str(), table.size());
}
inline void INamedDataBrokerBase::AddNodeWithAttribute(const std::string& tag,
													   const std::string& tagVal,
													   const std::string& attribute,
													   const std::string& attributeVal,
													   XmlNodeMap &table)
{
	XmlNode *node, *attrib;
	if(IsDebugOn())	printf("AddNodeWithAttribute(%s, %s, %s, %s)\n", tag.c_str(),
						   tagVal.c_str(), attribute.c_str(), attributeVal.c_str());
	try
	{
		node = const_cast<XmlNode *>(table.getNode(tag));
		node->setValue(tagVal);
		try
		{
			attrib = const_cast<XmlNode *>(node->getAttributes().getNode(attribute));
		}
		catch(XmlException &XmlErr)
		{
			node->addChild(attribute, attributeVal, ATTRIBUTE_NODE);
		}
	}
	catch(XmlException &XmlErr)
	{
		node = new XmlNode(tag, tagVal, NULL, ELEMENT_NODE);
		node->addChild(attribute, attributeVal, ATTRIBUTE_NODE);
		table.addNode(node);
	}
	if(IsDebugOn())	printf("added: %s\n", node->ToString().c_str());
	if(IsDebugOn())	printf("AddNodeWithAttribute(%s, %s, %s, %s) done\n", tag.c_str(),
						   tagVal.c_str(), attribute.c_str(), attributeVal.c_str());
}

const std::string INamedDataBrokerBase::FindValueInTable(const std::string& value, XmlNodeMap &Table)
{
	if(IsDebugOn())	printf("FindValueInTable(%s)\n", value.c_str());
	std::string retVal;	// Return value
	UINT32 ii = 0;
	while((ii < Table.size()) && retVal.empty())
	{
		if(value == Table.getNode(ii)->getValue())
		{	// If value was found return it
			retVal = Table.getNode(ii)->getName();
			if(IsDebugOn())	printf("Found %s as key for %s\n", retVal.c_str(), value.c_str());
		}
		ii++;
	}	// Loop through table until end or match found
	if(retVal.empty()) if(IsDebugOn()) printf("Did not find a key for %s\n", value.c_str());
	if(IsDebugOn())	printf("FindValueInTable(%s) done\n", value.c_str());
	return(retVal);
}

const INT32 INamedDataBrokerBase::SetServerStatus(const std::string &tag, const std::string &value,
														 const std::string &attribute,
														 const bool IsServerName /* = false */)
{
	INT32 stat = BEP_STATUS_ERROR;
	std::string alias = tag, name;
	if(!IsServerName) alias = GetServerAlias(tag);	// Get alias if not passed in
	if(IsDebugOn())	printf("SetServerStatus(%s, %s, %s, %d)\n", alias.c_str(), value.c_str(), attribute.c_str(), IsServerName);
	if(IsDataOk(alias))
	{	// If alias is ok
		try
		{
#if 0
			name = m_servers.getNode(alias)->getValue();
			m_servers.delNode(alias);
#else
            // The pointer to the node was being deleted, but the memory was
            //   not being freed.  The XmlNodeMap is only responsible for the
            //   pointer to the memory, not for the memory itself
            //      - Greg Pattison 03/08/2005

            // Get a pointer to the XmlNode
            const XmlNode *aliasNode = m_servers.getNode(alias);
			// Get the value of the node
			name = aliasNode->getValue();
            // Remove the pointer from the XmlNodeMap
			m_servers.delNode(alias);
            // Free the memory of the XmlNode
            delete aliasNode;
#endif
		}
		catch(XmlException &XmlErr)
		{	// We failed to find node, oh well
		}
		if(!value.empty()) name = value;
		AddNodeWithAttribute(alias, name, BEP_STATUS, attribute, m_servers);
		XmlNode *node = const_cast<XmlNode *>(m_servers.getNode(alias)->getAttributes().getNode(BEP_STATUS));
		node->setValue(attribute);
	}
	else if(IsDebugOn()) printf("alias %s is not Ok\n", alias.c_str());
	stat = BEP_STATUS_SUCCESS;
	if(IsDebugOn())	printf("SetServerStatus(%s, %s, %s, %d) done\n", alias.c_str(), value.c_str(), attribute.c_str(), IsServerName);
	return(stat);
}

