//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/CAN/GryphonCCanProxy.cpp $
// $Author:: Rwiersem         $
//   $Date:: 1/18/07 12:52p   $
//
// Description:
//  Logical port driver for Chrysler CAN communication on Gryphon
//  This file contains routines that are used only by Chrysler Can
//
//
//==============================================================================
//
//     Copyright © 2003 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/CAN/GryphonCCanProxy.cpp $
// 
// 6     1/18/07 12:52p Rwiersem
// Changes for the 07011901 core release:
// 
// - Removed DecodeIpAddress().
// 
// 5     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 4     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 3     2/26/04 6:33p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:29p Cward
//
// 3     11/13/03 6:02p Derickso
// Updates to register gryphon usdt server to allow larger range of module
// IDs.
//
// 2     10/07/03 1:05p Derickso
// Latest Gryphon Comm proxy.
//
// 1     9/24/03 6:46a Derickso
// Gryphon Comm Proxy files.
//
// 2     7/03/03 7:22a Gswope
//
// 1     7/01/03 11:20a Gswope
// Stolen from CAMI CCRT - GMLan
//
// 1     5/08/03 8:41a Gswope
// tested on CAMI machines with vehicle
//
// 3     5/05/03 8:22a Gswope
// placing in stasis
//
// 2     4/22/03 11:25a Gswope
//
// 1     4/22/03 10:38a Gswope
// First Entry
//
// 6     4/15/03 8:30a Gswope
//
// 5     4/07/03 3:14p Gswope
//
// 4     4/05/03 10:22a Gswope
// Daily Update -- class 2 working
//
// 3     4/01/03 8:44a Gswope
//
// 2     3/25/03 12:58p Gswope
//
// 1     3/17/03 4:29p Gswope
// Initial entry into source safe
//
//******************************************************************************
//In case Dearborn group still hasn't fixed their bugs (In registration we specify UUDT nodes - USDT server should know to pass these to us)
#define CARD_UUDT_STILL_REQUIRED_NON_LEGACY 0
//In case broadcast messages are not forwarded as they should be from non legacy device
#define J1939_BROADCASTS_THROUGH_CARD_FILTER 0
#define J1939_SendToCard 0

#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

#include "IGryphonChannel.h"
#include "IGCDebug.h"
#include "GryphonCCanProxy.h"
//******************************************************************************

GryphonCCanProxy::GryphonCCanProxy( int argc, char *argv[])
: IGryphonChannel( argc, argv)
{
// let the base classes set this, they do it correctly?
//  SetProcessName("logport/GryphonCCan");
//  m_pathName = "GryphonCCan";
    // this is where we would select alternate channels of the same type
    // based on a command line parameter ? config data ?
    m_channelOverride = 0;
}

GryphonCCanProxy::~GryphonCCanProxy()
{   // Nothing special to do here
}

void GryphonCCanProxy::Initialize(const XmlNode *document)
{
    const XmlNode *portSetup, *nodePairX;
    // Log the function entry -
    // This next line does not seem to work,
    // I believe the logger interface is not yet running at this point
    Log( LOG_FN_ENTRY, "Enter GryphonCCan::Initialize(%s)\n", document->getName().c_str());
    m_channelType = GCAN;        // 0x02    /* CAN TYPE */
    // Initialize base class
    RawCommProxy::Initialize( document);
    Log( LOG_FN_ENTRY, "Enter(2) GryphonCCan::Initialize(%s)\n", document->getName().c_str());
    // Get the CAN bus specific stuff
    if(document != NULL)
    {
        try
        {
            XmlString portNode = XmlString( XML_T("Setup/LogicalPorts/"));
            portNode += m_pathName;
            portSetup = document->getChild( portNode);
        }
        catch(XmlException &err)
        {
            Log( LOG_ERRORS, "\tError reading port configuration\n");
            throw;
        }
    }
    else
    {   // the following line was the original version, preserved, just in case
        Log( LOG_ERRORS, "GryphonCCanProxy::Initialize() - No configuration supplied.\n");
    }

    IPAddr   = DecodeIpAddress(portSetup->getChild("Setup/Login/IPAddress")->getValue());
    myPort   = atoi(portSetup->getChild("Setup/Login/Port")->getValue().c_str());
    UserId   = portSetup->getChild("Setup/Login/UserID")->getValue();
    Password = portSetup->getChild("Setup/Login/Password")->getValue();
    try
    {
        PortPathAlias = portSetup->getChild("Setup/Alias")->getValue();
    }
    catch(...)
    {
        // if the node doesnt exist, set target to null
        PortPathAlias = "";
    }
    try
    {
        m_stMinMultiplier = atof(portSetup->getChild("Setup/StMinMultiplier")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "ST Min Multiplier not specified, setting to 0 - %s", excpt.GetReason());
        m_stMinMultiplier = 0.0;
    }

    try
    {
        m_flowControlStMin = atoi(portSetup->getChild("Setup/FlowControlStMin")->getValue().c_str());
        /**
         *  0 - 127 should represent miliseconds
         *  0xF1 - 0xF9, 100 - 900 microseconds
         * 
         * Beacon simply defines this value as miliseconds though...
         * Maybe just try to keep value between 0 and 127
         *         
         */
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Flow Control ST Min not specified, setting to 0 - %s", excpt.GetReason());
        m_flowControlStMin = 0;
    }
	try
    {
        m_j1939TesterAddress = (UINT8)BposReadInt(portSetup->getChild("Setup/J1939TesterAddress")->getValue().c_str());
        Log(LOG_DEV_DATA, "J1939 tester address %s interpreted to %d", portSetup->getChild("Setup/J1939TesterAddress")->getValue().c_str(), m_j1939TesterAddress);
	}
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "J1939 tester address not specified, setting to 0 - %s", excpt.GetReason());
        m_j1939TesterAddress = 0;
    }
    try
    {
        //subtract 1 - gryphon starts on channel 1
        m_channelOverride = (atoi(portSetup->getChild("Setup/Channel")->getValue().c_str())) - 1;
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "Channel not specified, setting to 0 - %s", excpt.GetReason());
        m_channelOverride = 0;
    }

    PortDevPath = std::string("/dev/LogicalPorts/") + m_pathName;
    Log( LOG_DEV_DATA, "[GryphonCCan] port dev path and alias: \"%s\"  \"%s\"\n",PortDevPath.c_str(),PortPathAlias.c_str());

    m_nodePairCount = portSetup->getChild("Setup/NodePairs")->getChildren().size();
    m_nodePairSetupCount = portSetup->getChild("Setup/NodePairSetup")->getChildren().size();
    Log( LOG_DEV_DATA, "[GryphonCCan] Found %d node pairs\n", m_nodePairCount);

    try
    {
        m_nonLegacyDevice = atob(portSetup->getChild("Setup/NonLegacyDevice")->getValue().c_str()); 
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "NonLegacyDevice parameter not specified, setting to false - %s", excpt.GetReason());
        m_nonLegacyDevice = false;
    }

    try
    {
        m_j1939Channel = atob(portSetup->getChild("Setup/IsJ1939Channel")->getValue().c_str()); 
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "J1939 parameter not specified, setting to false - %s", excpt.GetReason());
        m_j1939Channel = false;
    }
    try
    {
        m_optimizeForLatency = atob(portSetup->getChild("Setup/OptimizeForLatency")->getValue().c_str()); 
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "OptimizeForLatency parameter not specified, setting to false (optimize for throughput) - %s", excpt.GetReason());
        m_optimizeForLatency = false;
    }
    // Sanity check the node pairs
    if(m_nodePairCount > 100) m_nodePairCount = 100;
    if(m_nodePairCount < 0) m_nodePairCount = 100;
    if(m_nodePairCount > 0)
    {
        // with 5 as the default msgSize, the header bit length will default to 11-bit headers
        int nodePairLength = 5;

        // headerBitLength is used as a control variable. 
        // 0 = bit lenght is unknown. 
        // 1 = bit length is 11 exclusivly. 
        // 2 = bit length is 29 exclusivly. 
        // 3 = bit length is both 11 and 29.
        int headerBitLength = 0;

        // 2 - 11 bit header
        // 4 - 29 bit header
        // 6 both headers
        m_moduleIDByteLength = 2;

        // Store the node pairs
        for(int ii = 0; ii < m_nodePairCount; ii++)
        {
            nodePairX = portSetup->getChild("Setup/NodePairs")->getChildren().getNode(ii);
            m_nodeMap[ii].outgoing = (UINT32)BposReadInt(nodePairX->getAttribute("Outgoing")->getValue().c_str());
            m_nodeMap[ii].incoming = (UINT32)BposReadInt(nodePairX->getAttribute("Incoming")->getValue().c_str());
			// Check for a UUDT response ID
			XmlNodeMapCItr uudtIter = nodePairX->getAttributes().find("UudtResponse");
			if(uudtIter != nodePairX->getAttributes().end())
			{
				m_nodeMap[ii].uudtIncoming = (UINT32)BposReadInt(uudtIter->second->getValue().c_str());
			}
            else
			{
				m_nodeMap[ii].uudtIncoming = m_nodeMap[ii].incoming;
			}
            // if it has been found that both 11 and 29 bit headers are being used, there is not use to continue to check,
            if(3 != headerBitLength)
            {
                nodePairLength = strlen(nodePairX->getAttribute("Outgoing")->getValue().c_str());
                if(nodePairLength == 5 && 0 == headerBitLength)
                {
                    //set as exclusive 11-bit header
                    headerBitLength = 1;
                    m_moduleIDByteLength = 2;
                }
                else if(nodePairLength == 5 && 2 == headerBitLength)
                {
                    //set as both 11 and 29-bit headers
                    headerBitLength = 3;
                    m_moduleIDByteLength = 6;
                }
                else if(nodePairLength == 10 && 0 == headerBitLength)
                {
                    //set as exclusive 29-bit header
                    headerBitLength = 2;
                    m_moduleIDByteLength = 4;
                }
                else if(nodePairLength == 10 && 1 == headerBitLength)
                {
                    //set as both 29 and 11-bit headers 
                    headerBitLength = 3;
                    m_moduleIDByteLength = 6;
                }
            }
        }

        //if for some crazy reason the header bit length wasn't spcified above default it to 11 bit headers.
        if(0 == headerBitLength)
        {
            Log(LOG_ERRORS, "Module ID Length not specified. Defaulting to 11-bit headers");
            m_moduleIDByteLength = 2;
        }

        // Store the node pairs for setup
        for(int ii = 0; ii < m_nodePairSetupCount; ii++)
        {
            nodePairX = portSetup->getChild("Setup/NodePairSetup")->getChildren().getNode(ii);
            m_nodePairSetupMap[ii].outgoing = (UINT32)BposReadInt(nodePairX->getAttribute("FirstRequestID")->getValue().c_str());
            m_nodePairSetupMap[ii].incoming = (UINT32)BposReadInt(nodePairX->getAttribute("FirstResponseID")->getValue().c_str());
            try
            {
                m_nodePairSetupMap[ii].uudtIncoming = (UINT32)BposReadInt(nodePairX->getAttribute("FirstUUDTResponseID")->getValue().c_str());
            }
            catch(...)
            {
                m_nodePairSetupMap[ii].uudtIncoming = 0x0000;
            }

            m_nodePairSetupMap[ii].numberOfPairs = (INT16)BposReadInt(nodePairX->getAttribute("NumberOfPairs")->getValue().c_str());
            
            // Define as 11/29 bit header
            if(m_nonLegacyDevice)
            {
                nodePairLength = strlen(nodePairX->getAttribute("FirstRequestID")->getValue().c_str());
                m_nodePairSetupMap[ii].is29BitHeader = (nodePairLength == 10);
            }
        }
    }
	if(!m_nonLegacyDevice)
	{//Legacy devices only
		try
		{
			XmlNodeMap bcastMessages;
			m_broadcastMessageCount = 0;
			bcastMessages.DeepCopy(portSetup->getChild("Setup/BroadcastMessageIDs")->getChildren());
			m_broadcastMessageCount = portSetup->getChild("Setup/BroadcastMessageIDs")->getChildren().size();
			Log( LOG_DEV_DATA, "[GryphonCCan] Found %d broadcast messages\n", m_broadcastMessageCount);
			m_broadcastMessageCount = 0;
			for(XmlNodeMapCItr iter = bcastMessages.begin();iter != bcastMessages.end(); iter++)
			{   // Get list of broadcast message ids to be recorded
				m_broadcastMessages[m_broadcastMessageCount].incoming = (UINT32)BposReadInt(iter->second->getAttribute("Incoming")->getValue().c_str());
				//initialize as blocked
				m_broadcastMessages[m_broadcastMessageCount].blocked = true;
				Log( LOG_DETAILED_DATA, "Added Broadcast Incoming Msg ID: %04X\n",m_broadcastMessages[m_broadcastMessageCount].incoming);
				m_broadcastMessageCount++;
			}
			m_recordBroadcastMessages = true;
		}
		catch(XmlException &excpt)
		{//No broadcast messages specified default to null
			Log(LOG_ERRORS, "No broadcast messages specified %s", excpt.GetReason());
			m_recordBroadcastMessages = false;
		}
	}
	else
	{
		#if J1939_BROADCASTS_THROUGH_CARD_FILTER
			try
			{
				XmlNodeMap bcastMessages;
				m_broadcastMessageCount = 0;
				bcastMessages.DeepCopy(portSetup->getChild("Setup/BroadcastMessageIDs")->getChildren());
				m_broadcastMessageCount = portSetup->getChild("Setup/BroadcastMessageIDs")->getChildren().size();
				Log( LOG_DEV_DATA, "[GryphonCCan] Found %d broadcast messages\n", m_broadcastMessageCount);
				m_broadcastMessageCount = 0;
				for(XmlNodeMapCItr iter = bcastMessages.begin();iter != bcastMessages.end(); iter++)
				{   // Get list of broadcast message ids to be recorded
					m_broadcastMessages[m_broadcastMessageCount].incoming = (UINT32)BposReadInt(iter->second->getAttribute("Incoming")->getValue().c_str());
					//initialize as blocked
					m_broadcastMessages[m_broadcastMessageCount].blocked = true;
					Log( LOG_DETAILED_DATA, "Added Broadcast Incoming Msg ID: %04X\n",m_broadcastMessages[m_broadcastMessageCount].incoming);
					m_broadcastMessageCount++;
				}
				m_recordBroadcastMessages = true;
			}
			catch(XmlException &excpt)
			{//No broadcast messages specified default to null
				Log(LOG_ERRORS, "No broadcast messages specified %s", excpt.GetReason());
				m_recordBroadcastMessages = false;
			}
		#else
			Log(LOG_DEV_DATA, "Non-Legacy device, not settting card filters for Broadcast messages, these are forwarded automatically by device now");
		#endif
	}
#if IGCDEBUG
    dumpNodePairs(m_nodeMap, m_nodePairCount);
#endif
    // Log the exit
    Log( LOG_FN_ENTRY, "Exit GryphonCCan::Initialize()\n");
}

vector<UINT32> GryphonCCanProxy::FindNodePair(const UINT32 locModule)
{
    vector<UINT32> locNode;
    for(int ii = 0; ii < m_nodePairCount; ii++)
    {
        if(locModule == m_nodeMap[ii].outgoing)
        {
            locNode.push_back(m_nodeMap[ii].incoming);
			locNode.push_back(m_nodeMap[ii].uudtIncoming);
        }
    }
    return(locNode);
}

vector<UINT32> GryphonCCanProxy::BuildPGNRequestNodePair(SerialString_t rawMessage)
{//push back each byte independently (6 bytes are needed to determine match)
	//example
	//request: <ReadVinPGN>0x00,0xEA,0x00,0x06,0xF9,0x00,0xEC,0xFE,0x00</ReadVinPGN>
	//response: 00 FE EC 06 00 F9
    vector<UINT32> locNode;
	if(rawMessage.length() >= 9)
    {
		//PGN in reverse order then destination (which will come back as source)
        locNode.push_back(((0xFF & (int) rawMessage[8]) * 0x1000000) + ((0xFF & (int) rawMessage[7]) * 0x10000) + 
                     ((0xFF & (int) rawMessage[6]) * 0x100) + (0xFF & (int) rawMessage[5]));
		Log(LOG_DEV_DATA, "Response ID set: %08X", locNode[0]);
	}
	else
	{
		Log(LOG_ERRORS, "Insufficient length to build node pair message length: %d",rawMessage.length());
	}
    return(locNode);
}
bool GryphonCCanProxy::IsBroadcastModuleID(const UINT32 locModule)
{
    for (int ii = 0; ii < m_broadcastMessageCount; ii++)
    {
        if (locModule == m_broadcastMessages[ii].incoming)
        {
            return(true);
        }
    }
    return(false);
}

bool GryphonCCanProxy::IsUudtId(const UINT32 &locModule)
{
	for(int index = 0; index < m_nodePairCount; index++)
	{
		if(locModule == m_nodeMap[index].uudtIncoming)
		{
			return true;
		}
	}
	return false;
}
int GryphonCCanProxy::getExpectedFromRaw(SerialString_t rawMessage)
{
#warning  Index should possibly be 2 instead of 3.  Need to test.
    return(0xFF & ((int) rawMessage[3]) + 0x40); // calculate expected response
}

UINT32 GryphonCCanProxy::getModuleId(const SerialString_t &rawMessage)
{
    int locModule;
    int tempModule;
    // Log the entry
    Log( LOG_FN_ENTRY, "Enter GryphonCCan::getModuleId\n");
    bool isFourByteHeader = false;
    if(m_moduleIDByteLength != 2 && rawMessage.length() >= 3)
    {   //32 bit header
        //Log( LOG_DEV_DATA, "Looking for a 32Bit Header\n");
        tempModule = ((0xFF & (int) rawMessage[0]) * 0x1000000) + ((0xFF & (int) rawMessage[1]) * 0x10000) + 
                     ((0xFF & (int) rawMessage[2]) * 0x100) + (0xFF & (int) rawMessage[3]);
        //check if incoming message exists in table
        for(int ii = 0; ii < m_nodePairCount; ii++)
        {
            if(tempModule == m_nodeMap[ii].incoming)
            {
                isFourByteHeader = true;
                break;
            }
        }
        if(isFourByteHeader)
        {
            locModule = tempModule;
        }
		else if ( m_j1939Channel)
        {//PGN (skip priority) then source
            locModule =  ((0xFF & (int) rawMessage[0]) * 0x1000000) + ((0xFF & (int) rawMessage[1]) * 0x10000) + 
                     ((0xFF & (int) rawMessage[2]) * 0x100) + (0xFF & (int) rawMessage[4]);
        }
        else
        {
            locModule = ((0xFF & (int) rawMessage[0]) * 0x100) + (0xFF & (int) rawMessage[1]);
        }
    }
    else
    {   //16 bit default
        //Log( LOG_DEV_DATA, "Looking for a 16Bit Header\n");
        locModule = ((0xFF & (int) rawMessage[0]) * 0x100) + (0xFF & (int) rawMessage[1]);
    }
    Log( LOG_FN_ENTRY, "Exit GryphonCCan::getModuleId(%d)\n", locModule);
    return(locModule);
}

int GryphonCCanProxy::getRespCode(const char *rawMessage)
{
    return(0xFF & ((int) rawMessage[3]) ); // actual response
}

vector<UINT32> GryphonCCanProxy::getModuleIdsFromRaw(SerialString_t rawMessage)
{
    UINT32 locModule;
    vector<UINT32> locResponseModule;
    // Log the entry
    Log( LOG_FN_ENTRY, "Enter GryphonCCan::getModuleIdFromRaw\n");
    if(m_moduleIDByteLength != 2)
    {
        if(IsFourByteHeader(rawMessage))
        {
            //32 bit header
            locModule = ((0xFF & (int) rawMessage[0]) * 0x1000000) + ((0xFF & (int) rawMessage[1]) * 0x10000) + 
                        ((0xFF & (int) rawMessage[2]) * 0x100) + (0xFF & (int) rawMessage[3]);
        }
        else
        {
            locModule = ((0xFF & (int) rawMessage[0]) * 0x100) + (0xFF & (int) rawMessage[1]);
        }
    }
    else
    {
        locModule = ((0xFF & (int) rawMessage[0]) * 0x100) + (0xFF & (int) rawMessage[1]);
    }
	if ( IsPGNRequest(rawMessage) )
	{
		locResponseModule = BuildPGNRequestNodePair(rawMessage);
	}
	else
	{
		locResponseModule = FindNodePair(locModule);
	}
    Log( LOG_FN_ENTRY, "Exit GryphonCCan::getModuleIdFromRaw(%d -> %s)\n", locModule, GetModuleIDsString(locResponseModule).c_str());
    return(locResponseModule);
}
bool GryphonCCanProxy::IsFourByteHeader(SerialString_t rawMessage)
{
    UINT32 locModule;
    vector<UINT32> locResponseModule;
    if(rawMessage.length() >= 4)
    {
        locModule = ((0xFF & (int) rawMessage[0]) * 0x1000000) + ((0xFF & (int) rawMessage[1]) * 0x10000) + 
                    ((0xFF & (int) rawMessage[2]) * 0x100) + (0xFF & (int) rawMessage[3]);
        for(int ii = 0; ii < m_nodePairCount; ii++)
        {
            if(locModule == m_nodeMap[ii].outgoing)
            {
                return true;
            }
        }
    }
    return false;
}
bool GryphonCCanProxy::IsPGNRequest(SerialString_t rawMessage)
{
    Log(LOG_DEV_DATA, "IsPGNRequest");
    vector<UINT32> locResponseModule;
	if ( m_j1939Channel )
	{
		if ( rawMessage.length() >= 4 )
		{
			if ((0xFF & (int)rawMessage[1]) == 0xEA)
			{//this is a PGN request
                Log(LOG_DEV_DATA, "IsPGNRequest: true");

				return true;
			}
		}
	}
     Log(LOG_DEV_DATA, "IsPGNRequest: false");
   return false;
}
bool GryphonCCanProxy::CheckForBlock(const uint8_t *inBuf)
{
    bool isBlocked = false;
    // if this is from a card, and it is not a USDT message, block it.
    // currently, according to scott, ALL messages from Chrysler are USDT
    //JPS Removing blocking card messages - needed for UUDT responses
    //isBlocked = (SD_CARD == inBuf[0]);
    Log(LOG_DEV_DATA, "CheckForBlock is %s -- SD_CARD: %02X %s inbuf[0]: %02X\n",
        (isBlocked ? "BLOCKED" : "NOT blocked"), SD_CARD, (isBlocked ? "==" : "!="), inBuf[0]);
    // Return the result
    return(isBlocked);
}

bool GryphonCCanProxy::UsingGryphonUSDT()
{
    return true;
}
void GryphonCCanProxy::BuildCardMessage(SerialString_t &outBuf, const SerialString_t &inBuf)
{
	SerialString_t locDS; // intermediate buffer
	// Log the entry
	Log( LOG_FN_ENTRY, "Enter GryphonCCanProxy::BuildCardMessage(inBuf(%d))\n", inBuf.size());
	if ( IsPGNRequest(inBuf) )
	{
		BuildPGNRequestMessage(locDS, inBuf);	// add the protocol info
		WrapPGNMessage(outBuf, locDS);	// add gryphon data frame
	}
	else
	{
		BuildMessage(locDS, inBuf);	// add the protocol info
		WrapMessage(outBuf, locDS);	// add gryphon data frame
	}
	Log( LOG_FN_ENTRY, "Exit GryphonCCanProxy::BuildCardMessage()\n");
}
void GryphonCCanProxy::BuildMessage(SerialString_t &outBuf, const SerialString_t &inBuf)
{
		gdatahdr locDH;
		int moduleIDByteLength;

		m_destinationType = SD_USDT; // using USDT processor for outgoing messages
		if(m_moduleIDByteLength != 2)
		{
			if(IsFourByteHeader(inBuf))
			{
				//32bit
				locDH.hdrlen = 4;        // Can header is 4 bytes
				locDH.hdrbits = 29;      // only 29 bits are used
				moduleIDByteLength = 4;
			}
			else
			{//16bit
				locDH.hdrlen = 2;        // Can header is 2 bytes
				locDH.hdrbits = 11;      // only 11 bits are used
				moduleIDByteLength = 2;
			}
		}
		else
		{//16bit
			locDH.hdrlen = 2;        // Can header is 2 bytes
			locDH.hdrbits = 11;      // only 11 bits are used
			moduleIDByteLength = 2;
		}
		Log(LOG_DEV_DATA, "InBuf Contents(%d)\n",inBuf.size());
		for(int i=0;i<inBuf.size();i++)
		{
			Log(LOG_DEV_DATA, "\t\tindex: %04d -- %c <0x%02X>\n",
				i,isprint(inBuf[i]) ? inBuf[i] : '?',inBuf[i]);
		}
		Log(LOG_DEV_DATA, "Module ID Length: %d InBuf Length: %d\n",moduleIDByteLength, inBuf.length());
		locDH.datalen = htons(inBuf.length()-moduleIDByteLength);  // length of the DATA part of the message
		locDH.extralen = 0;      // we dont support this, always 0
		locDH.mode = MODE_LOCAL; // i suspect this is not used by the box
		locDH.priority = 0;      // not used
		locDH.stat = 0;          // probably not used
		locDH.time = 0;          // probably not used
		locDH.context = UseContext();  // sequential message number
		locDH.reservedchar = 0;  // not used
		locDH.reservedshort = 0; // not used

		outBuf = SerialString_t((uint8_t *) &locDH, sizeof(locDH));

		outBuf += inBuf;         // append
}
#if J1939_SendToCard
void GryphonCCanProxy::BuildPGNRequestMessage(SerialString_t &outBuf, const SerialString_t &inBuf)
{
	Log( LOG_FN_ENTRY, "Enter BuildPGNRequestMessage\n");
	gdatahdr locDH;
	int moduleIDByteLength;
	m_destinationType = SD_CARD;
	//m_destinationType = SD_J1939TP;

	//32bit
	locDH.hdrlen = 4;        // Can header is 4 bytes
	locDH.hdrbits = 29;      // only 29 bits are used
	moduleIDByteLength = 4;
	Log(LOG_DEV_DATA, "InBuf Contents(%d)\n",inBuf.size());
	for(int i=0;i<inBuf.size();i++)
	{
		Log(LOG_DEV_DATA, "\t\tindex: %04d -- %c <0x%02X>\n",
			i,isprint(inBuf[i]) ? inBuf[i] : '?',inBuf[i]);
	}
	Log(LOG_DEV_DATA, "Module ID Length: %d InBuf Length: %d\n",moduleIDByteLength, inBuf.length());
	locDH.datalen = htons(inBuf.length()-moduleIDByteLength);  // length of the DATA part of the message
	locDH.extralen = 0;      // we dont support this, always 0
	locDH.mode = MODE_LOCAL;
	locDH.priority = 0;      // not used
	locDH.stat = 0;          // probably not used
	locDH.time = 0;          // probably not used
	locDH.context = UseContext();  // sequential message number
	locDH.reservedchar = 0;  // not used
	locDH.reservedshort = 0; // not used

	outBuf = SerialString_t((uint8_t *) &locDH, sizeof(locDH));

	outBuf += inBuf;         // append
}
void GryphonCCanProxy::WrapPGNMessage(SerialString_t &frameData, const SerialString_t &inBuf)
{
		char locBuf[30];
		unsigned int locSize;
		Msg *locMsg;
		// Log the entry
		Log( LOG_FN_ENTRY, "Enter GryphonCCanProxy::WrapPGNMessage()\n");
		locMsg = (Msg *) locBuf;

		locMsg->fh.src = SD_CLIENT;
		locMsg->fh.srcchan = m_clientChannel;
		locMsg->fh.dst = m_destinationType;
		locMsg->fh.dstchan = m_channelId;

		locSize = inBuf.size() + 4;

		locMsg->fh.msglen=htons(locSize);
		//locMsg->fh.frametype = FT_DATA;
		locMsg->fh.frametype = FT_CMD;
		locMsg->fh.reserved = 0;

		//Send directly to card to skip USDT server formatting
		uint8_t localBuffer[20];
		// Clear out the message data
		memset(localBuffer,0,sizeof(localBuffer));
		localBuffer[0] = CMD_CARD_TX;
		localBuffer[1] = UseContext();
		localBuffer[2] = 0x00;
		localBuffer[3] = 0x00;

		frameData = SerialString_t ((uint8_t *)locBuf, 8);
		frameData += SerialString_t ((uint8_t *)localBuffer, 4);;
		frameData += inBuf;

		BufferGryphonMessage(frameData);
		Log( LOG_FN_ENTRY, "Exit GryphonCCanProxy::WrapPGNMessage()\n");
}
#else
void GryphonCCanProxy::BuildPGNRequestMessage(SerialString_t &outBuf, const SerialString_t &inBuf)
{
	Log( LOG_FN_ENTRY, "Enter BuildPGNRequestMessage\n");
	gdatahdr locDH;
	int moduleIDByteLength;
	m_destinationType = SD_J1939TP;

	//32bit
	locDH.hdrlen = 6;        // Can header is 4 bytes
	locDH.hdrbits = 29;      // only 29 bits are used
	moduleIDByteLength = 6;
	Log(LOG_DEV_DATA, "InBuf Contents(%d)\n",inBuf.size());
	for(UINT32 i=0;i<inBuf.size();i++)
	{
		Log(LOG_DEV_DATA, "\t\tindex: %04d -- %c <0x%02X>\n",
			i,isprint(inBuf[i]) ? inBuf[i] : '?',inBuf[i]);
	}
	Log(LOG_DEV_DATA, "Module ID Length: %d InBuf Length: %d\n",moduleIDByteLength, inBuf.length());
	locDH.datalen = htons(inBuf.length()-moduleIDByteLength);  // length of the DATA part of the message
	locDH.extralen = 0;      // we dont support this, always 0
	locDH.mode = MODE_TX;
	locDH.priority = 0;      // not used
	locDH.stat = 0;          // probably not used
	locDH.time = 0;          // probably not used
	locDH.context = UseContext();  // sequential message number
	locDH.reservedchar = 0;  // not used
	locDH.reservedshort = 0; // not used

	outBuf = SerialString_t((uint8_t *) &locDH, sizeof(locDH));

	outBuf += inBuf;         // append
}
void GryphonCCanProxy::WrapPGNMessage(SerialString_t &frameData, const SerialString_t &inBuf)
{
	WrapMessage(frameData, inBuf);
}
#endif
void GryphonCCanProxy::CreateFilter(bool is29BitHeader, uint32_t incomingId)
{
    if (is29BitHeader)
    {
        char idToMatch[4] = {char((incomingId >> 24) & 0x000000FF),
            char((incomingId >> 16) & 0x000000FF),
            char((incomingId >> 8) & 0x000000FF),
            char(incomingId & 0x000000FF)};
        char idMask[4] = {0xFF,0xFF,0xFF,0xFF};

        Log( LOG_FN_ENTRY, "Filter found: (%02x, %02x, %02x, %02x)\n",idToMatch[3], idToMatch[2], idToMatch[1], idToMatch[0]);
        //Create filter to allow messages to pass
        AddFilter( (FILTER_FLAG_PASS | FILTER_FLAG_ACTIVE),    /*const uint8_t locFlags*/
                  0,                                        /*const uint16_t locOffset*/
                  4,                                        /*const uint16_t locLength, */
                  FILTER_DATA_TYPE_HEADER,                  /*const uint8_t locField*/
                  BIT_FIELD_CHECK,                          /*const uint8_t locOperator*/ 
                  idToMatch,                                     /*const char *firstValue, */
                  idMask);                                    /*const char *secondValue*/
    }
    else
    {
        //test for uudt message
        char idToMatch[2] = {char((incomingId >> 8) & 0x00FF),
            char(incomingId & 0x00FF)};
        char idMask[2] = {0xFF,0xFF};

        Log( LOG_FN_ENTRY, "Filter found: (%02x, %02x)\n",idToMatch[1], idToMatch[0]);
        //Create filter to allow messages to pass
        AddFilter( (FILTER_FLAG_PASS | FILTER_FLAG_ACTIVE),    /*const uint8_t locFlags*/
                  0,                                        /*const uint16_t locOffset*/
                  2,                                        /*const uint16_t locLength, */
                  FILTER_DATA_TYPE_HEADER,                  /*const uint8_t locField*/
                  BIT_FIELD_CHECK,                          /*const uint8_t locOperator*/ 
                  idToMatch,                                     /*const char *firstValue, */
                  idMask);                                    /*const char *secondValue*/
    }
}
void GryphonCCanProxy::CreateJ1939BroadcastFilter(void)
{
	if ( m_recordBroadcastMessages )
    {
        for (int ii = 0; ii < m_broadcastMessageCount; ii++)
        {//all broadcast messages are j1939 therefore 29 bit headers
			uint32_t incomingId = m_broadcastMessages[ii].incoming;
			char idToMatch[6] = {char((incomingId >> 24) & 0x000000FF),
            char((incomingId >> 16) & 0x000000FF),
            char((incomingId >> 8) & 0x000000FF),
            char(incomingId & 0x000000FF),
			char(0x00),
			char(0x00)};
			char idMask[6] = {0xFF,0xFF,0xFF,0x00,0xFF,0x00};

        Log( LOG_FN_ENTRY, "CreateJ1939BroadcastFilter Filter found: (%02x, %02x, %02x, %02x, %02, %02)\n",idToMatch[5], idToMatch[4],idToMatch[3], idToMatch[2], idToMatch[1], idToMatch[0]);

        //Create filter to allow messages to pass
        AddFilter( (FILTER_FLAG_PASS | FILTER_FLAG_ACTIVE),    /*const uint8_t locFlags*/
                  0,                                        /*const uint16_t locOffset*/
                  6,                                        /*const uint16_t locLength, */
                  FILTER_DATA_TYPE_HEADER,                  /*const uint8_t locField*/
                  BIT_FIELD_CHECK,                          /*const uint8_t locOperator*/ 
                  idToMatch,                                     /*const char *firstValue, */
                  idMask);                                    /*const char *secondValue*/
		}
        //TURN ON THE FILTER
        SetFilterMode(FILTER_ON);

        //BLOCK ALL OTHER MESSAGES???
        SetDefaultFilterMode(DEFAULT_FILTER_BLOCK);
    }

}
void GryphonCCanProxy::LegacyChannelSpecificUUDTInit(void)
{
    // Check for UUDT filters - if they exist set up the card filter (gryph usdt server does not handle 
    // these messages correctly)
    for (int ii = 0; ii < m_nodePairSetupCount; ii++)
    {
        if(m_nodePairSetupMap[ii].uudtIncoming != 0x0000)
        {//uudt filter exists - need to setup card to accept this filter
			CreateFilter(m_nodePairSetupMap[ii].is29BitHeader, m_nodePairSetupMap[ii].uudtIncoming);

            //TURN ON THE FILTER
            SetFilterMode(FILTER_ON);

            //BLOCK ALL OTHER MESSAGES???
            SetDefaultFilterMode(DEFAULT_FILTER_BLOCK);
        }
    }
}
void GryphonCCanProxy::LegacyChannelSpecificJ1939Init(void)
{
	//Legacy devices only - J1939 broadcast messages are now passed to us automatically in non-Legacy devices
	if ( m_recordBroadcastMessages )
    {
        for (int ii = 0; ii < m_broadcastMessageCount; ii++)
        {//all broadcast messages are j1939 therefore 29 bit headers
            CreateFilter(true,m_broadcastMessages[ii].incoming);
        }
        //TURN ON THE FILTER
        SetFilterMode(FILTER_ON);

        //BLOCK ALL OTHER MESSAGES???
        SetDefaultFilterMode(DEFAULT_FILTER_BLOCK);
    }
}
int GryphonCCanProxy::ChannelSpecificInit(void)
{
    int retVal;
    Log(LOG_FN_ENTRY, "GryphonCCanProxy::ChannelSpecificInit() -- Enter");
	if ( !m_nonLegacyDevice )
	{
		LegacyChannelSpecificUUDTInit();
		LegacyChannelSpecificJ1939Init();
	}
	else
	{
		#if CARD_UUDT_STILL_REQUIRED_NON_LEGACY
			LegacyChannelSpecificUUDTInit();
		#else
			Log(LOG_DEV_DATA, "Non-Legacy device, not settting card filters for UUDT messages, these are handled by USDT server now");
		#endif
		#if J1939_BROADCASTS_THROUGH_CARD_FILTER
			CreateJ1939BroadcastFilter();
		#else
			Log(LOG_DEV_DATA, "Non-Legacy device, not settting card filters for Broadcast messages, these are forwarded automatically by device now");
		#endif
	}

    // Set the filter mode
    // disabled for test SetFilterMode(FILTER_OFF_BLOCK_ALL);
    // Register and return the result
    bool registrationOk = false;
    int maxAttempts = 10;
    for(int i = 1; i <= maxAttempts && !registrationOk; i++)
    {
        if(m_nonLegacyDevice)
            registrationOk = RegisterWithUsdtNonLegacy(m_pathName, i);
        else
            registrationOk = RegisterWithUsdt(m_pathName, i);

        if(!registrationOk)
        {
            Log(LOG_DEV_DATA, "USDT Registration failed! Try again? %s", maxAttempts <= 10 ? "Yes" : "No - Maxxed out");
            BposSleep(750);
        }
        else
            retVal = EOK;
    }
    
    // Set min msg separation for flow control
    if(retVal == EOK)  retVal = SetMinimumMessageSeperationTime(m_flowControlStMin);

    // Set the minimum seperation time of each message (including flow control and consecutive frames)    
    if(retVal == EOK)  retVal = SetMinimumMessageSeperationTimeMultiplier(m_stMinMultiplier);
	if ( m_nonLegacyDevice && m_j1939Channel )
	{
			if ( retVal == EOK )  retVal = EnableJ1939TransportProtocol();
		#if J1939_BROADCASTS_THROUGH_CARD_FILTER
			if ( retVal == EOK )  retVal = ClaimJ1939Address(m_j1939TesterAddress,true);
		#else
			if ( retVal == EOK )  retVal = ClaimJ1939Address(m_j1939TesterAddress,false);
			SetDefaultFilterMode(DEFAULT_FILTER_PASS);
			SetFilterMode(FILTER_OFF_PASS_ALL);
		#endif
	}

    Log(LOG_FN_ENTRY, "GryphonCCanProxy::ChannelSpecificInit() complete - retval: %d", retVal);
    return(retVal);
}

/**
 * Handler method for client subscription requests
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int GryphonCCanProxy::PortSubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                       resMgrIoOcb_t *ioOcb)
{
    logPortMsg_t            *portMsg = (logPortMsg_t*)_DEVCTL_DATA( msg->i);
    logPortSubscribe_t      &subMsg = portMsg->portSubscribe;
    logPortSubscribeReply_t &reply = portMsg->portSubscribeReply;
    CommIoOcb_t             *clientOcb = (CommIoOcb_t*)ioOcb;
    int                     retVal;
    LogPortSubscriptionId_t subId = LOG_PORT_INVALID_SUB;

    Log( LOG_FN_ENTRY, "Enter GryphonCCanProxy::PortSubscribeHandler(), size = %d\n", ctp->size);

    // Need to lock this operation to avoid duplicate subscription ID's
    if( (retVal = pthread_mutex_lock( &m_dataLock)) == EOK)
    {
        clientOcb->subMask |= subMsg.flags;

        // If subscribing for received data
        if( subMsg.flags & PORT_SUBSCRIBE_RX)
        {
            // If not already subscribed for RX data
            if( clientOcb->rxSubscription->ShouldNotify() == false)
            {
				//clear fifo
				clientOcb->rxSubscription->fifo.Reset();
                // Get a reference to the existing RX subscription
                clientOcb->rxSubscription->count = subMsg.byteCount;
                clientOcb->rxSubscription->SetNotifyEvent( subMsg.notifyEvent, ctp->rcvid);
                clientOcb->rxSubscription->notifyModeMask = subMsg.modeMask;

                // If there is a filter string specified for this subscription
                if( (subMsg.flags & PORT_NOTIFY_FILTER) && (subMsg.filterByteCount > 0))
                {
                    Log( LOG_DEV_DATA, "Adding Rx Filter String for coId %d\n", ctp->rcvid);
                    Log( LOG_DEV_DATA, "Rx Filter String length = %d\n", subMsg.filterByteCount);

                    // Copy the desired filter string
                    LogPortFilterString filterString;

                    //filterString.reserve( subMsg.filterByteCount);
                    for( uint32_t ii=0; ii<subMsg.filterByteCount; ii++)
                    {
                        LogPortFilterByte   fb = LogPortFilterByte( subMsg.filterString[ ii]);
                        filterString.push_back( fb);
                        Log( LOG_DEV_DATA, "\tAdded filter byte %d: $%02X $%02X %d\n",
                             ii, fb.m_fbVal, fb.m_mask, fb.m_isWildCard);
                    }
                    if ( m_j1939Channel )
					{//setting filters for pgns (only subscriptions) to variable length
						filterString.SetRespLenType(VAR_LEN);
					}
					// Add filter string to the clients filter list
                    clientOcb->rxSubscription->filterList.AddFilterString(filterString);
                    Log( LOG_DEV_DATA, "Added Rx Filter String for coId %d\n", ctp->rcvid);
                }

                // Already subscribed for rx data
                reply.flags |= PORT_SUBSCRIBE_RX;
                Log( LOG_DEV_DATA, "Added Rx subscriber() for coId %d\n", ctp->rcvid);
            }
        }

        // If subscribing for transmit data
        if( (subMsg.flags & PORT_SUBSCRIBE_TX) && (clientOcb->txSubscription == NULL))
        {
            clientOcb->txSubscription = new LogPortSubscription( subMsg.byteCount,
                                                                 subMsg.modeMask,
                                                                 subMsg.notifyEvent,
                                                                 ctp->rcvid);
            if(LOG_PORT_INVALID_SUB == subId)   subId = clientOcb->txSubscription->id;

            // Add to transmit data subscribers list
            m_txSubscribers.AddSubscriber( clientOcb->txSubscription->id, clientOcb);
            reply.flags |= PORT_SUBSCRIBE_TX;
            Log( LOG_DEV_DATA, "Added Tx subscriber() for coId %d\n", ctp->rcvid);
        }

        // If subscribing for port lock events
        if( (subMsg.flags & PORT_SUBSCRIBE_LOCK) && (clientOcb->lockSubscription == NULL))
        {
            clientOcb->lockSubscription = new LogPortSubscription( subMsg.byteCount,
                                                                   subMsg.modeMask,
                                                                   subMsg.notifyEvent,
                                                                   ctp->rcvid);
            if(LOG_PORT_INVALID_SUB == subId)   subId = clientOcb->lockSubscription->id;

            // Add to transmit data subscribers list
            m_lockSubscribers.AddSubscriber( clientOcb->lockSubscription->id, clientOcb);

            reply.flags |= PORT_SUBSCRIBE_LOCK;
            Log( LOG_DEV_DATA, "Added Lock subscriber()\n");

            // Check current port lock status
            if( IsPortLocked() == true)
            {
                // Port is currently locked, so deliver pulse now
                /////////////////////////////////////////////////

                // If client wants to use a pulse for notifications
                if(subMsg.notifyEvent.sigev_notify == SIGEV_PULSE)
                {
                    // Set pulse value to proper event
                    subMsg.notifyEvent.sigev_value.sival_int |= PORT_SUBSCRIBE_LOCK;
                }
                MsgDeliverEvent( ctp->rcvid, &subMsg.notifyEvent);
            }
        }

        // If subscribing for port unlock events
        if( (subMsg.flags & PORT_SUBSCRIBE_UNLOCK) && (clientOcb->unlockSubscription == NULL))
        {
            clientOcb->unlockSubscription = new LogPortSubscription( subMsg.byteCount,
                                                                     subMsg.modeMask,
                                                                     subMsg.notifyEvent,
                                                                     ctp->rcvid);
            if(LOG_PORT_INVALID_SUB == subId)   subId = clientOcb->unlockSubscription->id;

            // Add to transmit data subscribers list
            m_unlockSubscribers.AddSubscriber( clientOcb->unlockSubscription->id, clientOcb);

            reply.flags |= PORT_SUBSCRIBE_UNLOCK;

            // Check current port lock status
            if( IsPortLocked() == false)
            {
                // Port is currently unlocked, so deliver pulse now
                /////////////////////////////////////////////////

                // If client wants to use a pulse for notifications
                if(subMsg.notifyEvent.sigev_notify == SIGEV_PULSE)
                {
                    // Set pulse value to proper event
                    subMsg.notifyEvent.sigev_value.sival_int |= PORT_SUBSCRIBE_UNLOCK;
                }
                MsgDeliverEvent( ctp->rcvid, &subMsg.notifyEvent);
            }
        }

        // If clients wants notification of exisating data in FIFOs
        if( subMsg.flags & PORT_NOTIFY_EXISTING)
        {
            if( reply.flags)    reply.flags |= PORT_NOTIFY_EXISTING;
        }

        // If no other subscriptions were made, default to the RX subscription ID
        if(LOG_PORT_INVALID_SUB == subId)   subId = clientOcb->rxSubscription->id;

        reply.handle = subId;
        memset( &msg->o, 0, sizeof( msg->o));
        msg->o.nbytes = sizeof( reply);
        SETIOV( ctp->iov, &msg->o, sizeof( msg->o)+msg->o.nbytes);
        retVal = _RESMGR_NPARTS(1);

        pthread_mutex_unlock( &m_dataLock);
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking data in PortSubscribeHandler: %s\n",
             strerror( retVal));
        retVal = EBUSY;
    }

    Log( LOG_FN_ENTRY, "Exit GryphonCCanProxy::PortSubscribeHandler()\n");

    return( retVal);
}

/**
 * Handler method for client unsubscription requests
 *
 * @param ctp    Resource manager context pointer
 * @param msg    Message structure
 * @param ioOcb  Client's connection properties
 * @return EOK if successful, other on error
 */
int GryphonCCanProxy::PortUnsubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                         resMgrIoOcb_t *ioOcb)
{
	//JPS potential remove Nothing was being done here
	//unblock broadcast message
	//GryphonIoOcb_t    *clientOcb = (GryphonIoOcb_t*)ioOcb;
	//SetBroadcastBlock(true,clientOcb->moduleIDs[0]);
    return IGryphonChannel::PortUnsubscribeHandler(ctp, msg, ioOcb);
}
