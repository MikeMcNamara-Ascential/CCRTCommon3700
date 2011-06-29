//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/CommProxy/Gryphon/IGCDebug.h $
// $Author:: Cward           $
// $Date:: 12/21/03 6:29p    $
//
// Description:
//  Logical port driver for communication on Gryphon
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
//     $Log: /Core/ServicesLayer/Managers/CommProxy/Gryphon/IGCDebug.h $
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
// 

    void DumpGConfig(void);
void DumpGConfig(deviceinfo *dip, int witch);
void dumpNodePairs(struct InOut *nodeMap, int nodePairCount);
void dumpEventNames(struct EventStructure *EventIDs);
void dumpServer(sockaddr * server);
void dumpHostEnt(struct hostent *hep);
void dump(void *start, int howmany);

