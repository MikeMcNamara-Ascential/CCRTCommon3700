//******************************************************************************
//
// FILE:
//     $Archive:: /Core/ServicesLayer/Include/SysManager.h                    $
//      $Author: Cward $
//        $Date: 12/21/03 6:27p $
//
// Description:
//
// Empty I/O manager class. Responds with ENOSYS (not implemented) to
// all I/O messages.
//
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Core/ServicesLayer/Include/SysManager.h $
// 
// 1     12/21/03 6:27p Cward
// 
// 1     10/07/03 11:38a Khykin
// Initial checkin
// 
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
// 
// 1     6/12/03 4:24p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:05:47  tbochene
//
//     PooBag
//
// 
// 1     1/27/03 7:10a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:07a Khykin
// Initial Checkin From DCTC
// 
// 1     4/10/01 10:46a Admin
//
// 1     2/15/01 12:13p Brian.meinke
// Base class for resource managers that respond to sys messages( i.e.
// Version messages)
//
// 1     2/15/01 10:26a Brian.meinke
// Initial check-in
//
//******************************************************************************

#ifndef _SYSMANAGER_H_
#define _SYSMANAGER_H_

#include <string.h>			// NULL
#include <errno.h>			// errno, ENOSYS, etc
#include <sys/kernel.h>		// Send, Receive, Reply...
#include <sys/psinfo.h>		// _PPF_SERVER, qnx_pflags

#include "IOManager.h"

/**
 * System message string names
 */
extern const char* sysMssgs[];

class SysManager : public IOManager
{
public:
	/**
	 * System Message Handler Interface Object Constructor
	 *
	 * @param name   'sin ver' Version name
	 *
	 * @param date   'sin ver' Version Date
	 *
	 * @param ver    'sin ver' Version Number
	 *
	 * @param rev    'sin ver' Version Letter
	 */
	SysManager( const char *name, const char *date, unsigned short ver, char rev);
	
	/**
	 * System Message Handler Interface Object Constructor
	 */
	SysManager( const char *name="");

	virtual ~SysManager();

	/**
	 * System Message dispatch function
	 *
	 * @param pid    Process ID to reply to
	 *
	 * @param rxMssg Message received from the client
	 */
	void sysMessage( pid_t pid, const sys_msg &rxMssg);

	/**
	 * Message handler for unknown system messages
	 *
	 * @param pid    Process ID of the sending process
	 *
	 * @param rcvMssg Message data received from the client
	 *
	 * @param rplyMssg Reply data to send back to the client
	 *
	 * @return 0 if successful
	 *         <0 if error occurs
	 */
	virtual int sys_bogus(pid_t pid, const sys_msg &rcvMssg, sys_reply &rplyMssg);
	
	/**
	 * Message handler for _SYSMSG_SUBTYPE_DEATH system messages
	 *
	 * @param pid    Process ID of the sending process
	 *
	 * @param rcvMssg Message data received from the client
	 *
	 * @param rplyMssg Reply data to send back to the client
	 *
	 * @return 0 if successful
	 *         <0 if error occurs
	 */
	virtual int sys_death(pid_t pid, const sys_msg &rcvMssg, sys_reply &rplyMssg);
	
	/**
	 * Message handler for _SYSMSG_SUBTYPE_SIGNAL system messages
	 *
	 * @param pid    Process ID of the sending process
	 *
	 * @param rcvMssg Message data received from the client
	 *
	 * @param rplyMssg Reply data to send back to the client
	 *
	 * @return 0 if successful
	 *         <0 if error occurs
	 */
	virtual int sys_signal(pid_t pid, const sys_msg &rcvMssg, sys_reply &rplyMssg);
	
	/**
	 * Message handler for _SYSMSG_SUBTYPE_TRACE system messages
	 *
	 * @param pid    Process ID of the sending process
	 *
	 * @param rcvMssg Message data received from the client
	 *
	 * @param rplyMssg Reply data to send back to the client
	 *
	 * @return 0 if successful
	 *         <0 if error occurs
	 */
	virtual int sys_trace(pid_t pid, const sysmsg_trace_mssg &rcvMssg, sys_reply &rplyMssg);
	
	/**
	 * Message handler for _SYSMSG_SUBTYPE_VERSION system messages
	 *
	 * @param pid    Process ID of the sending process
	 *
	 * @param rcvMssg Message data received from the client
	 *
	 * @param rplyMssg Reply data to send back to the client
	 *
	 * @return 0 if successful
	 *         <0 if error occurs
	 */
	virtual int sys_version(pid_t pid, const sysmsg_version_mssg &rcvMssg, sys_reply &rplyMssg);
	
	/**
	 * Message handler for _SYSMSG_SUBTYPE_SLIB system messages
	 *
	 * @param pid    Process ID of the sending process
	 *
	 * @param rcvMssg Message data received from the client
	 *
	 * @param rplyMssg Reply data to send back to the client
	 *
	 * @return 0 if successful
	 *         <0 if error occurs
	 */
	virtual int sys_slib(pid_t pid, const sys_msg &rcvMssg, sys_reply &rplyMssg);

    /**
     * Registers the calling process as a server type process
     *
     * @return 0 if successful
     *         -1 if error; use errno to obtain more error information
     */
    int registerAsServerProc( long flags=_PPF_SERVER, long *oldSetting=NULL);
	
protected:

private:
	
	/**
	 * 'sin ver' version data structure
	 */
	sysmsg_version_reply_mssg m_version;
};

#endif		// _IO_MGR_H_

