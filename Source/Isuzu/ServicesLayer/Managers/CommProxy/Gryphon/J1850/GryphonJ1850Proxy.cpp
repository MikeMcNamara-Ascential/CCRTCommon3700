//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/J1850/GryphonJ1850Proxy.cpp $
// $Author::          $
//   $Date::    $
//
// Description:
//  Logical port driver for J1850 communication on Gryphon
//  This file contains routines that are used only J1850 on the Gryphon device
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
//******************************************************************************
#include "IGryphonChannel.h"
#include "IGCDebug.h"
#include "GryphonJ1850Proxy.h"
//******************************************************************************

GryphonJ1850Proxy::GryphonJ1850Proxy( int argc, char *argv[])
: IGryphonChannel( argc, argv)
{
    // this is where we would select alternate channels of the same type
    // based on a command line parameter ? config data ?

    //ERIC: KEEP THIS ZERO SENSE WE WANT IT TO FIND THE CHANNEL BASED ON CARD TYPE SET IN Initialize
    m_channelOverride = 0;
}

GryphonJ1850Proxy::~GryphonJ1850Proxy()
{   // Nothing special to do here
}

void GryphonJ1850Proxy::Initialize(const XmlNode *document)
{
    const XmlNode *portSetup;//, *nodePairX;
    // Log the function entry -
    // This next line does not seem to work,
    // I believe the logger interface is not yet running at this point
    Log( LOG_FN_ENTRY, "Enter GryphonJ1850Proxy::Initialize(%s)\n", document->getName().c_str());


    //ERIC: TELL CHANNEL TYPE IS J1850 SO THE REGISTER 
    //WILL FIND APPROPIATE CHANNEL ID FOR THIS TYPE ON GRYPHON BOX
    m_channelType = GJ1850;        // 0x03    /* 1850 TYPE */

    // Initialize base class
    RawCommProxy::Initialize( document);
    Log( LOG_FN_ENTRY, "Enter(2) GryphonJ1850Proxy::Initialize(%s)\n", document->getName().c_str());
    // Get the CAN bus specific stuff
    if (document != NULL)
    {
        try
        {
            XmlString portNode = XmlString( XML_T("Setup/LogicalPorts/"));
            portNode += m_pathName;
            portSetup = document->getChild( portNode);
        }
        catch (XmlException &err)
        {
            Log( LOG_ERRORS, "\tError reading port configuration\n");
            throw;
        }
    }
    else
    {   // the following line was the original version, preserved, just in case
        Log( LOG_ERRORS, "GryphonJ1850Proxy::Initialize() - No configuration supplied.\n");
    }

    IPAddr   = DecodeIpAddress(portSetup->getChild("Setup/Login/IPAddress")->getValue());
    myPort   = atoi(portSetup->getChild("Setup/Login/Port")->getValue().c_str());
    UserId   = portSetup->getChild("Setup/Login/UserID")->getValue();
    Password = portSetup->getChild("Setup/Login/Password")->getValue();
    try
    {
        PortPathAlias = portSetup->getChild("Setup/Alias")->getValue();
    }
    catch (...)
    {
        // if the node doesnt exist, set target to null
        PortPathAlias = "";
    }
    PortDevPath = std::string("/dev/LogicalPorts/") + m_pathName;
    Log( LOG_DEV_DATA, "[GryphonJ1850Proxy] port dev path and alias: \"%s\"  \"%s\"\n",PortDevPath.c_str(),PortPathAlias.c_str());

    /*
    m_nodePairCount = portSetup->getChild("Setup/NodePairs")->getChildren().size();
    m_nodePairSetupCount = portSetup->getChild("Setup/NodePairSetup")->getChildren().size();
    Log( LOG_DEV_DATA, "[GryphonJ1850Proxy] Found %d node pairs\n", m_nodePairCount);
    // Sanity check the node pairs
    if (m_nodePairCount > 60) m_nodePairCount = 60;
    if (m_nodePairCount < 0) m_nodePairCount = 60;
    if (m_nodePairCount > 0)
    {   // Store the node pairs
        for (int ii = 0; ii < m_nodePairCount; ii++)
        {
            nodePairX = portSetup->getChild("Setup/NodePairs")->getChildren().getNode(ii);
            m_nodeMap[ii].outgoing = (INT16)BposReadInt(nodePairX->getAttribute("Outgoing")->getValue().c_str());
            m_nodeMap[ii].incoming = (INT16)BposReadInt(nodePairX->getAttribute("Incoming")->getValue().c_str());
        }
        // Store the node pairs for setup
        for (int ii = 0; ii < m_nodePairSetupCount; ii++)
        {
            nodePairX = portSetup->getChild("Setup/NodePairSetup")->getChildren().getNode(ii);
            m_nodePairSetupMap[ii].outgoing = (INT16)BposReadInt(nodePairX->getAttribute("FirstRequestID")->getValue().c_str());
            m_nodePairSetupMap[ii].incoming = (INT16)BposReadInt(nodePairX->getAttribute("FirstResponseID")->getValue().c_str());
            m_nodePairSetupMap[ii].numberOfPairs = (INT16)BposReadInt(nodePairX->getAttribute("NumberOfPairs")->getValue().c_str());
        }
    }
#if IGCDEBUG
    dumpNodePairs(m_nodeMap, m_nodePairCount);
#endif
*/
    // Log the exit
    Log( LOG_FN_ENTRY, "Exit GryphonJ1850Proxy::Initialize()\n");
}

/*
int GryphonJ1850Proxy::FindNodePair(const int locModule)
{
    int locNode = 0;
    for (int ii = 0; ii < m_nodePairCount; ii++)
    {
        if (locModule == m_nodeMap[ii].outgoing)
        {
            locNode = m_nodeMap[ii].incoming;
        }
    }
    return(locNode);
}
*/
int GryphonJ1850Proxy::getExpectedFromRaw(SerialString_t rawMessage)
{

    //ERIC: FOR THE EXPECTED RESPONSE IT SHOULD BE INDEX 3 + $40

    int expectedResponseCode = (0xFF & ((int) rawMessage[3]) + 0x40); // calculate expected response
    
    Log( LOG_FN_ENTRY, "Exit GryphonJ1850Proxy::getExpectedFromRaw(%d)\n", expectedResponseCode);
    return expectedResponseCode;
}

UINT32 GryphonJ1850Proxy::getModuleId(const char *rawMessage)
{
    int locModule;
    // Log the entry
    Log( LOG_FN_ENTRY, "Enter GryphonJ1850Proxy::getModuleId\n");
    //ERIC USED IN RETURNED MESSAGE TO VERIFY CORRECT MODULE REQUEST
    locModule = ((0xFF & (int) rawMessage[2]));// * 0x100) + (0xFF & (int) rawMessage[1]);
    Log( LOG_FN_ENTRY, "Exit GryphonJ1850Proxy::getModuleId(%d)\n", locModule);
    return(locModule);
}

int GryphonJ1850Proxy::getRespCode(const char *rawMessage)
{
    int locResp=0;
    //ERIC USED IN RETURNED MESSAGE TO VERIFY CORRECT MODULE REQUEST
    locResp = (0xFF & ((int) rawMessage[3]) ); // actual response
    Log( LOG_FN_ENTRY, "Exit GryphonJ1850Proxy::getRespCode(%d)\n", locResp);

    return locResp;
}

vector<UINT32> GryphonJ1850Proxy::getModuleIdsFromRaw(SerialString_t rawMessage)
{
    vector<UINT32> locModule;
    
    //int locResponseModule;

    //ERIC: MODULE ID IS THE SECOND BYTE AFTER PRIORITY IN THE MESSAGE TO THE MODULE
    locModule.push_back(((0xFF & (int) rawMessage[1])));// + (0xFF & (int) rawMessage[3]);
    
    //locResponseModule = FindNodePair(locModule);
    Log( LOG_FN_ENTRY, "Exit GryphonJ1850Proxy::getModuleIdFromRaw(%s)\n", GetModuleIDsString(locModule).c_str());
    return(locModule);
}

bool GryphonJ1850Proxy::UsingGryphonUSDT(){
    return false;
}

bool GryphonJ1850Proxy::CheckForBlock(const uint8_t *inBuf)
{
    Log( LOG_FN_ENTRY, "GryphonJ1850Proxy::CheckForBlock()\n");
    //NO MESSAGES FROM J1850 are from the USDT so don't block them in an FT_DATA response
    bool isBlocked = false;
    // if this is from a card, and it is not a USDT message, block it.
    //isBlocked = (SD_CARD == inBuf[0]);
    //Log(LOG_DEV_DATA, "CheckForBlock is %s -- SD_CARD: %02X %s inbuf[0]: %02X\n",
    //    (isBlocked ? "BLOCKED" : "NOT blocked"), SD_CARD, (isBlocked ? "==" : "!="), inBuf[0]);
    // Return the result
    return(isBlocked);
}

void GryphonJ1850Proxy::BuildMessage(SerialString_t &outBuf, const SerialString_t &inBuf)
{
    gdatahdr locDH;

    //ERIC LOOK INTO THIS -- CCAN OBJECT IS SETTING TO SD_USDT
    m_destinationType = SD_CARD; // just send to the card to send to vehicle network

    locDH.hdrlen = 3;        // j1850 message header is 3 bytes (prio byte, dest byte, src byte)

    locDH.hdrbits = 24;      // Supposedly not needed if we use all the 8bits in the bytes but mom always said better safe then sorry
    locDH.datalen = htons(inBuf.length()-locDH.hdrlen);  // length of the DATA part of the message so subtract the hrd length
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

    //PrintSerialString( LOG_SER_COM, "GryphonJ1850Proxy::BuildMessage() outBuf: ", outBuf);

    Log(LOG_FN_ENTRY, "GryphonJ1850Proxy::BuildMessage()");
}

void GryphonJ1850Proxy::BufferGryphonMessage(SerialString_t &frameData)
{
    SerialString_t  nullByte((uint8_t *)"\0",1);
    int bufferedBytes = 0;

    //ERIC LOOK HERE TO WHY THIS IS BEING ANDED WITH 3 AND ADDING NULL BYTES
	while (frameData.size() & 3)
    {
         frameData+=nullByte;
         bufferedBytes++;
    };

    //Log( LOG_FN_ENTRY, "Exit GryphonJ1850Proxy::BufferGryphonMessage() - Null Bytes Added %d\n", bufferedBytes);
    return;
}

int GryphonJ1850Proxy::ChannelSpecificInit(void)
{
    int retVal;
    Log(LOG_FN_ENTRY, "GryphonJ1850Proxy::ChannelSpecificInit() -- Enter");
    // Set the filter mode
    //SetFilterMode(FILTER_OFF_PASS_ALL);
    char idToMatch = 0xF1;
    char idMask = 0xFF;

    //ERIC: CREATE THE FILTER TO RETURN ONLY MESSAGES TO ME ($F1)
    AddFilter( (FILTER_FLAG_PASS | FILTER_FLAG_ACTIVE),    /*const uint8_t locFlags*/
              1,                                        /*const uint16_t locOffset*/
              1,                                        /*const uint16_t locLength, */
              FILTER_DATA_TYPE_HEADER,                  /*const uint8_t locField*/
              BIT_FIELD_CHECK,                          /*const uint8_t locOperator*/ 
              &idToMatch,                                     /*const char *firstValue, */
              &idMask);                                    /*const char *secondValue*/

    //TURN ON THE FILTER
    SetFilterMode(FILTER_ON);

    //BLOCK ALL OTHER MESSAGES???
    SetDefaultFilterMode(DEFAULT_FILTER_BLOCK);

    // Register and return the result
    //retVal = RegisterWithUsdt();
    Log(LOG_FN_ENTRY, "GryphonJ1850Proxy::ChannelSpecificInit() complete - retval: %d", retVal);
    return(retVal);
}

