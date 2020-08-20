// written at bepco.
#ifndef gmsg2_h
#define gmsg2_h
// additional restrictions for our use.
#define MAX_EVNAMES 32
#define STMIN_FC_MSG_SIZE 5
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// these are additional structure definitions that we find useful.
#include "info.h"
#include "gdev.h"
#include "gmsg.h"

struct InOut{
    uint32_t incoming;
    uint32_t outgoing;  
	uint32_t uudtIncoming;
};

struct InOutSetup{
    uint32_t incoming;
    uint32_t uudtIncoming;
    uint32_t outgoing;
    uint16_t numberOfPairs;
    bool is29BitHeader;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this is the structure of a single line in the event name message.
// used for parsing the message, and for building the local table
struct EvNames {
    uint8_t number;
    char name[19];
    char terminator; // note that this is not in the file, included for code
};


/**
 * Contains a cross reference of event numbers
 * to associated text
 * We allocate one additional line as a delimiter
 */
struct EventStructure
{
    struct EvNames event[MAX_EVNAMES+1];
};


/**
 * the array of the card configuration messages
 * We allocate one additional slot, because someone at dg decided the
 * first channel is channel 1 (proabably a basic programmer)
 * We use channel 0 to hold additional flags as follows:
 *
 * dinfo[0].channel = 0xF5 when card configuration has been stored.
 */



struct ChannelConfig
{
    struct deviceinfo dinfo[MAXCHANS+1];
};


struct LoginBlock
{
    struct gcmdhdr header;
    char userID[16];
    char passwd[32];
};

struct USDTBlock
{
	unsigned int numberOfIDs;
	unsigned int firstUSDTRequestID;
	unsigned int firstUSDTResponseID;
	unsigned int firstUUDTResponseID;
};

struct USDTBlockNonLegacy
{
	unsigned int numberOfIDs;
	unsigned int firstUSDTRequestID;
	unsigned int firstUSDTResponseID;
	unsigned int firstUUDTResponseID;
    unsigned char USDTRequestExtAddr;
    unsigned char USDTResponseExtAddr;
    unsigned char UUDTRequestExtAddr;
    unsigned char reserved;
};

#define MAX_USDT_RANGES 64

struct RegMsg{
    struct gcmdhdr header;
    char doRegOn;
    char doEchoMsg;
    char doRecvOpt;
    unsigned char funcAddrCnt;
    struct USDTBlock usdtBlock[MAX_USDT_RANGES];
};

struct RegMsgNonLegacy{
    struct gcmdhdr header;
    char doRegOn;
    char doEchoMsg;
    char doRecvOpt;
    unsigned char funcAddrCnt;
    struct USDTBlockNonLegacy usdtBlock[MAX_USDT_RANGES];
};

struct StMinMsg
{
    struct gcmdhdr header;
    unsigned int stMinMultiplier;
};

struct StMinFcMsg
{
    struct gcmdhdr header;
    unsigned char stMinValue; 
    //};
    /** Removed padding based on email conversation from DG Tech Support
        - DG Documentation may not reflect this
     */
    unsigned char padding1;
    unsigned short padding2;};   


#endif  //gmsg2_h
