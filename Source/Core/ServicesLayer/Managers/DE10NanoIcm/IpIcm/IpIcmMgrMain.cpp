//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IpIcmMgrMain.cpp $
//  $Author: Cward $
//    $Date: 4/13/06 4:05a $
//
// Description:
//    Resource manager main for Altera IP Icm card
//
//==============================================================================
//
// All Rights Reserved
//
//     Copyright © 2003 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/IpIcmMgrMain.cpp $
//
// 2     4/13/06 4:05a Cward
// cleanup usage block
//
// 2     2/15/06 4:10p Bmeinke
// Removed the $Revision: 2 $ and $Date: 4/13/06 4:05a $ strings from the __USAGE block
//
// 1     12/21/03 6:28p Cward
//
// 1     10/07/03 11:34a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
//
//******************************************************************************


#include "IpIcmMgr.h"

int main(int argc, char *argv[])
{
    IpIcmMgr    *icmMgr = NULL; 
    int         retVal = RES_MGR_CMD_NONE;
    ILogger     logger;

    logger.SetProcessName("IpIcmMgr (main)");
    logger.EnableLogger();
    logger.SetLocalEcho(true);

    while (retVal != RES_MGR_CMD_TERMINATE)
    {
        try
        {
            //  Create ICM Manager object
            icmMgr = new IpIcmMgr(argc, argv);
            // Execute the manager message loop
            retVal = icmMgr->Execute();
        }
        catch (XmlException &xmlErr)
        {
            logger.Log("Caught unhandled XmlException: %s\n", xmlErr.GetReason());
            retVal = RES_MGR_CMD_TERMINATE;
        }
        catch (BepException &bepErr)
        {
            logger.Log("Caught unhandled BepException: %s\n", bepErr.GetReason());
            retVal = RES_MGR_CMD_TERMINATE;
        }
        catch (...)
        {
            logger.Log("Caught unhandled unknown Exception\n");
            retVal = RES_MGR_CMD_TERMINATE;
        }
        delete icmMgr;

        logger.Log("Looping...\n");
    }

    return(retVal);
}

/*******************************************************************************
    Usage Block
    Information printed when a user types 'use xxxxx' at the QNX prompt
#ifdef __USAGE

Altera IP module ICM Board Resource Manager

%C [-e] [-p prio] [-r name] [-vx]  &

    -e    Echo logger messages to stdout
    -p    Run scanner at priority 'prio'
    -r    Register task using 'name' (default='mgr/IpIcm')
    -v    Use verbose mask 'x' (in hex, see notes)
    -R    Do not wait for the system register message;
           register during initialization instead

    Notes:
    -Verbose Mask bit definitions (leading 0s optional):
        00000000: No Log Messages
        00000001: Log Errors
        00000002: Log Function Entry/Exit
        00000004: Log I/O message data
        00000008: Log device general messages
        FFFFFFFF: Log everything
    - Registers name as 'mgr/IpIcm'
    - Must be run as Super User.

#endif
*******************************************************************************/

