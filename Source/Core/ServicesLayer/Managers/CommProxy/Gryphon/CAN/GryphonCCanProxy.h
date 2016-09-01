//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/CAN/GryphonCCanProxy.h $
// $Author:: Rwiersem  $
// $Date:: 1/18 $
//
// Description:
//  Logical port driver for CAN bus communication on Gryphon
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/CAN/GryphonCCanProxy.h $
// 
// 3     1/18/07 12:53p Rwiersem
// Changes for the 07011901 core release:
// 
// - Removed DecodeIpAddress().
// 
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 1     12/21/03 6:29p Cward
//
// 1     9/24/03 6:46a Derickso
// Gryphon Comm Proxy files.
//
//==============================================================================
#ifndef GryphonCCanProxy_h
#define GryphonCCanProxy_h
//==============================================================================
#include "IGryphonChannel.h"

/**
 * CAN bus communication proxy for the Gryphon box.
 *
 * @author Gene Swope
 * @author Dan Erickson
 * @version Version 2.0
 * @see IGryphonChannel
 * @see RawCommProxy
 * @since April 2003
 */
class GryphonCCanProxy : public IGryphonChannel
{
public:
    /**
     * Constructor from command line arguments
     *
     * @param argc   Number of command line arguments to parse
     * @param argv   Array of command line argument strings
     */
    GryphonCCanProxy( int argc, char *argv[]);
    /**
     * Class destructor.
     * @since Version 2.0
     */
    virtual ~GryphonCCanProxy();
    /**
     * We override this function so that we can pick up the additional
     * information we need from the configuration file.
     *
     * @param document command line supplied name of config file.
     *
     * @return
     */
    void Initialize(const XmlNode *document);
    /**
     * Find the module response ID.
     *
     * @param locModule Module being used.
     *
     * @return Response ID for the specified module.
     * @since Version 2.0
     */
    vector<UINT32> FindNodePair(const UINT32 locModule);
    /**
     * Attempt to determine what the response code will be for this request.
     * the response will be filtered against this calculated code if not 0
     *
     * @param rawMessage pointer to the outgoing message buffer
     *
     * @return value of the expected response for this request
     */
    virtual int getExpectedFromRaw(SerialString_t rawMessage);
    virtual UINT32 getModuleId(const char *rawMessage);
    virtual int getRespCode(const char *rawMessage);
    virtual vector<UINT32> getModuleIdsFromRaw(SerialString_t rawMessage);
    virtual bool UsingGryphonUSDT();

protected:
    /**
     * Check to see if this message should be blocked.
     * This is to prevent us from receiving a message twice. Once from the card
     * directly, then again as processed by the USDT handler.
     * It is implemented by blocking messages that come straight from the card that
     * will be handled by the USDT handler.
     *
     * @param inBuf  pointer to the buffer to evaluate.
     *
     * @return true if message should be ignored.
     */
    bool CheckForBlock(const uint8_t *inBuf) ;
    /**
     * adds protocol specific gryphon data header for the outgoing data message
     *
     * WARNING!  do NOT include the length bytes in the message if using USDT
     *
     * @param locData
     * @param inBuf   source for data content.
     */
    virtual void BuildMessage(SerialString_t &locData, const SerialString_t &inBuf) ;
    /**
     * Handler method for client subscription requests
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int PortSubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg, resMgrIoOcb_t *ioOcb);

    /**
     * Handler method for client unsubscription requests
     *
     * @param ctp    Resource manager context pointer
     * @param msg    Message structure
     * @param ioOcb  Client's connection properties
     * @return EOK if successful, other on error
     */
    virtual int PortUnsubscribeHandler(resmgr_context_t *ctp, io_devctl_t *msg,
                                       resMgrIoOcb_t *ioOcb);
    /**
     * This function does any addtional required setup for this specific protocol
     *
     * Before sending USDT messages, we need to register with the handler.
     * This method accomplishes that task for us.  Note that although the protocol
     * allows us to provide additional functional addresses, we do not implement
     * that capability at this time.  If this capability is needed, the separate
     * "CMD_USDT_SET_FUNCTIONAL" messge should be implemented.
     *
     * @return EOK if successful, any other value is an error code
     */
    int ChannelSpecificInit(void) ;
    void CreateFilter(uint32_t incomingId);
    virtual bool IsBroadcastModuleID(const UINT32 locModule);
	void SetBroadcastBlock(bool block, UINT32 keyID);


private:
    /** ST min multipler to set in the Gryphon box */
    float m_stMinMultiplier;

    struct BcastMessage{
        uint32_t incoming;
        SerialString_t message;
        bool blocked;
    };
    // assume there will never be more than 63 pairs.
    struct BcastMessage m_broadcastMessages[63];
    int m_broadcastMessageCount;

    bool m_recordBroadcastMessages;

};

//==============================================================================
#endif  //GryphonCCanProxy_h
