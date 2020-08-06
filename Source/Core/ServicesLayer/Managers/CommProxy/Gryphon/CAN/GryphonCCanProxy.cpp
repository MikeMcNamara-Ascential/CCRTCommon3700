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
        }
    }
    return(locNode);
}

int GryphonCCanProxy::getExpectedFromRaw(SerialString_t rawMessage)
{
#warning  Index should possibly be 2 instead of 3.  Need to test.
    return(0xFF & ((int) rawMessage[3]) + 0x40); // calculate expected response
}

UINT32 GryphonCCanProxy::getModuleId(const char *rawMessage)
{
    int locModule;
    int tempModule;
    // Log the entry
    Log( LOG_FN_ENTRY, "Enter GryphonCCan::getModuleId\n");
    bool isFourByteHeader = false;
    if(m_moduleIDByteLength != 2 && strlen(rawMessage) >= 4)
    {   //32 bit header
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
        else
        {
            locModule = ((0xFF & (int) rawMessage[0]) * 0x100) + (0xFF & (int) rawMessage[1]);
        }
    }
    else
    {   //16 bit default
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
    locResponseModule = FindNodePair(locModule);
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

int GryphonCCanProxy::ChannelSpecificInit(void)
{
    int retVal;
    Log(LOG_FN_ENTRY, "GryphonCCanProxy::ChannelSpecificInit() -- Enter");


    // Check for UUDT filters - if they exist set up the card filter (gryph usdt server does not handle 
    // these messages correctly)
    for(int ii = 0; ii < m_nodePairSetupCount; ii++)
    {
        if(m_nodePairSetupMap[ii].uudtIncoming != 0x0000)
        {//uudt filter exists - need to setup card to accept this filter

            //test for uudt message
            char idToMatch[2] = {char((m_nodePairSetupMap[ii].uudtIncoming >> 8) & 0x00FF),
                char(m_nodePairSetupMap[ii].uudtIncoming & 0x00FF)};
            char idMask[2] = {0xFF,0xFF};

            Log( LOG_FN_ENTRY, "Filter found: (%02x, %02x)\n",idToMatch[1], idToMatch[0]);
            //ERIC: CREATE THE FILTER TO RETURN ONLY MESSAGES TO ME ($F1)
            AddFilter( (FILTER_FLAG_PASS | FILTER_FLAG_ACTIVE),    /*const uint8_t locFlags*/
                       0,                                        /*const uint16_t locOffset*/
                       2,                                        /*const uint16_t locLength, */
                       FILTER_DATA_TYPE_HEADER,                  /*const uint8_t locField*/
                       BIT_FIELD_CHECK,                          /*const uint8_t locOperator*/ 
                       idToMatch,                                     /*const char *firstValue, */
                       idMask);                                    /*const char *secondValue*/

            //TURN ON THE FILTER
            SetFilterMode(FILTER_ON);

            //BLOCK ALL OTHER MESSAGES???
            SetDefaultFilterMode(DEFAULT_FILTER_BLOCK);

            //End Test
        }
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
    
    //retVal = RegisterWithUsdt(m_pathName);
    
    // Set min msg separation for flow control
    if(retVal == EOK)  retVal = SetMinimumMessageSeperationTime(m_flowControlStMin);

    // Set the minimum seperation time of each message (including flow control and consecutive frames)    
    if(retVal == EOK)  retVal = SetMinimumMessageSeperationTimeMultiplier(m_stMinMultiplier);

    Log(LOG_FN_ENTRY, "GryphonCCanProxy::ChannelSpecificInit() complete - retval: %d", retVal);
    return(retVal);
}
