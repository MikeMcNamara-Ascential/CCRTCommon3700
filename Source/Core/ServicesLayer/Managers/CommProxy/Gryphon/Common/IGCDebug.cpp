//******************************************************************************
//
// $Archive: /Working/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/Common/IGCDebug.cpp $
// $Author:: Cward            $
//   $Date:: 5/11/05 9:39p    $
//
// Description:
//  Additional development/debug functions for Logical port driver for Gryphon
//  This file is only included if IGCDEBUG is #defined as true in 
//  the parent .cpp file, so make sure any calls to these functions are
//  wrappered the same way.
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
//     $Log: /Working/Source/Core/ServicesLayer/Managers/CommProxy/Gryphon/Common/IGCDebug.cpp $
// 
// 1     5/11/05 9:39p Cward
// Initial Version
// 
// 1     4/25/05 2:17p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
// 
// 1     12/21/03 6:29p Cward
// 
// 1     9/24/03 6:45a Derickso
// Gryphon Comm Proxy files.
// 
// 5     7/03/03 7:24a Gswope
// 
// 4     7/01/03 11:24a Gswope
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
// 4     4/15/03 8:30a Gswope
// 
// 3     4/07/03 3:14p Gswope
// 
// 2     4/05/03 10:22a Gswope
// Daily Update -- class 2 working
// 
// 1     3/25/03 12:58p Gswope

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <netdb.h>
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <sys/types.h>
    #include <sys/socket.h>
#endif

#include "gmsg2.h"

void dump(void *start, int howmany)
{
    unsigned char *mine;

    mine = (unsigned char *) start;

    for(int ii = 0; ii < howmany; ii++)
    {
        printf("%02x ",mine[ii]);
    }

}
                                                          
void dumpHostEnt(struct hostent *hep)
{
uint8_t *bytes;

//    bytes = (uint8_t *) hep->h_addr_list[0];
    bytes = (uint8_t *) *hep->h_addr_list;

    printf("\nDumping HostEnt:");
    printf("\naddress %03d.%03d.%03d.%03d",bytes[0], bytes[1], bytes[2], bytes[3]);
    printf("\ntype    %d",hep->h_addrtype);

// this could have null pointers, which hoses us, and besides, we dont care
//    bytes = (uint8_t *)hep->h_aliases[0];
//    printf("\nalias   %03d.%03d.%03d.%03d",bytes[0], bytes[1], bytes[2], bytes[3]);
    printf("\nlength  %d",hep->h_length);
    printf("\nName    %s",hep->h_name);
}


void dumpServer(sockaddr * server)
{
    printf("\nDumping Socket Description:");
    printf("\nfamily   %d",server->sa_family);
    printf("\nData     %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
        (uint8_t) server->sa_data[0],
        (uint8_t) server->sa_data[1],
        (uint8_t) server->sa_data[2],
        (uint8_t) server->sa_data[3],
        (uint8_t) server->sa_data[4],
        (uint8_t) server->sa_data[5],
        (uint8_t) server->sa_data[6],
        (uint8_t) server->sa_data[7],
        (uint8_t) server->sa_data[8],
        (uint8_t) server->sa_data[9],
        (uint8_t) server->sa_data[10],
        (uint8_t) server->sa_data[11],
        (uint8_t) server->sa_data[12],
        (uint8_t) server->sa_data[13]
        );
}


void DumpGConfig(deviceinfo *dip, int witch)
{

    if(0 != dip->channel)
    {
        if(witch == dip->channel)
            printf("\n--- Using This Channel ----------------------------");
        printf("\nChannel %d : %s slot-%d type=%d subtype=%d", 
            dip->channel,  dip->name, dip->slot, dip->type, dip->subtype);
               
        printf("\n           version: %s  security: %s   serial: %s",
            dip->version, dip->security, dip->serial);
                
        printf("\n           headerflags=%lX  data: %d-%d  extra: %d-%d",
            dip->vheader, dip->vdatamin, dip->vdatamax, dip->vextramin, dip->vextramax);
        if(witch == dip->channel)
            printf("\n---------------------------------------------------");
    }
    
}

void dumpEventNames(struct EventStructure *EventIDs)
{
    printf("\nDumping Event IDs:");
    for(int ii=0 ; ii < MAX_EVNAMES; ii++)
    {
        EventIDs->event[ii].terminator=0;
        if(EventIDs->event[ii].name[0] != 0)
            printf("\nIndex = %3d  Number = %3d  Name = %s",
                ii, EventIDs->event[ii].number, EventIDs->event[ii].name);
        else break;
        printf("\n");
    }

}

void dumpNodePairs(struct InOut *nodeMap, int nodePairCount)
{
    printf("\nDumping Node Pairs:\n");
    for(int ii=0 ; ii < nodePairCount; ii++)
    {
        printf("      Index = %3d  Outgoing = 0x%03X  Incoming = 0x%03X\n",
                ii, nodeMap[ii].outgoing, nodeMap[ii].incoming);
    }

}

