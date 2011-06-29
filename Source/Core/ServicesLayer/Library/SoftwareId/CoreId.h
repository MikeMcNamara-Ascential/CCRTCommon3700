//*************************************************************************
// Description:
//   This provides the definition of the Core ID.
//
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Library/SoftwareId/CoreId.h $
// 
// 50    10/30/07 3:55p Rwiersem
// Changes for the 07103101 core release:
// 
// - Updated IDCOREBLD to 07103101.
// 
// 49    5/01/07 1:58p Rwiersem
// Changes for the 07050201 core release:
// 
// Updated IDCOREBLD to 07050201.
// 
// 48    2/22/07 11:05a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Changed IDCOREBLD to 07022201.
// 
// 46    10/26/06 3:26p Rwiersem
// Changes for the 06107201 core release:
// 
// Updated IDCOREBLD to 06102701.
// 
// 45    9/22/06 11:13a Rwiersem
// Updated IDCOREBLD to 06092201.
// 
// 44    9/05/06 4:03p Rwiersem
// Updated IDCOREBLD to 06090601.
// 
// 43    8/02/06 8:32p Cward
// Updated Iteration Build Date
//
// 42    7/19/06 6:07p Cward
// Updated for build date
//
// 41    7/05/06 9:45p Cward
// Updated Revision Date for iteration.
//
// 40    6/21/06 4:38p Cward
// Modified for iteration data/identifier
//
// 39    6/07/06 5:29p Cward
// Updated the build Identifier
//
// 38    5/24/06 2:50p Cward
// Updated iteration date
//
// 37    5/08/06 7:20p Cward
// Updated iteration identifier
//
// 36    4/26/06 6:50p Cward
// Updated build identifier and build date
//
// 35    4/13/06 3:46a Cward
// updated iteration
//
// 34    3/30/06 1:36a Cward
// Updated software Id.
//
// 33    3/10/06 3:56p Cward
// Updated for Core build date.
//
// 32    3/02/06 12:47a Cward
// Updated release date
//
// 31    2/15/06 7:53p Cward
// Update core revision date
//
// 30    2/02/06 12:53a Cward
// Updated for the 06020201 build
//
// 29    11/14/05 1:26p Cward
// Updates for 05111401
//
// 28    11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 27    8/17/05 8:14p Cward
// Updated Core build date
//
// 26    7/20/05 9:04p Cward
// Updated core build ID
//
// 25    5/25/05 8:55p Cward
// Updated AnalyzeValveCross and updated the structure of the Input Server
//
// 24    5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 23    4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 22    4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 21    4/01/05 2:53p Cward
// Updated software ID
//
// 20    3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 19    3/11/05 3:32p Cward
// Updated for new core iteration build date
//
// 18    3/03/05 11:46p Cward
// Updated for 05030302. Needed quick fix for new licensing
//
// 17    3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 16    2/16/05 9:23p Cward
// UPdated for Core build date
//
// 15    2/02/05 11:50p Cward
// The Core ID chaged
//
// 14    10/27/04 6:46p Cward
// Changes for 04102701 build. Primarily to back out most of the
// Dataserver changes mad in the previous (04101401) build.
//
// 13    10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 12    9/29/04 10:22p Cward
// Updated for Core Build date
//
// 11    9/01/04 9:44p Cward
// UPdated build ID
//
// 10    8/19/04 12:01a Cward
// Updated Core build Id string
//
// 9     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 8     7/21/04 10:07p Cward
// Updated build ID to 7/21/2004 second build
//
// 7     7/21/04 11:20a Cward
// Updated core build date
//
// 6     7/07/04 6:39p Cward
// Update the core build date.
//
// 5     6/24/04 1:00a Cward
// Updated the build date. Also updated the header block comments to the
// new standard.
//
// 4     6/10/04 12:06a Cward
// Updated build date.
//
// 3     5/25/04 4:13p Cward
// Updated the core build date
//
// 2     5/19/04 9:40p Cward
// Updated Build date for may 19, 2004
//
// 1     5/12/04 9:55p Cward
// Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef COREID_H
#define COREID_H
//==============================================================================

#include <string>

#define IDPROD      std::string("Ccrt")
#define IDTYPE      std::string("R")

#define IDCOREBLD   std::string("11051001")


#endif // COREID_H
