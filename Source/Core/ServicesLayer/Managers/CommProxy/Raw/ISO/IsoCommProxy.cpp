//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/CommProxy/Raw/ISO/IsoCommProxy.cpp $
// $Author: Cward $
//   $Date: 2/16/05 9:26p $
//
// Description:
//	Logical serial port driver for ISO serial communication
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
//     $Log: /Core/ServicesLayer/Managers/CommProxy/Raw/ISO/IsoCommProxy.cpp $
// 
// 2     2/16/05 9:26p Cward
// UPdated ReadPortConfig
//
// 1     1/23/04 9:39a Cward
// Restructuring Raw comm proxies
//
// 1     8/21/03 9:05a Derickso
// 05WK CCRT Project
//
// 3     8/07/03 12:54p Khykin
// Removing usage block.  It is located in IsoCommProxyMain.cpp.
//
// 1     6/12/03 4:42p Khykin
// Initial checkin.
//
// 2     5/19/03 9:25a Khykin
// Updated with Brians latest changes.
//
// 1     4/30/03 4:55p Bmeinke
// Migration to new personalized configuration management structure
//
// 7     2/13/03 10:10a Bmeinke
// Removed data members m_reflCoid and m_reflChid (not used)
// Removed InitManager(), CleanupManager() and WaitForReflection() methods
// (no longer needed)
//
// 6     2/11/03 12:18p Bmeinke
// Replaced LockPortHandler() and UnlockPortHandler() with LockForClient()
// and UnlockForClient()
//
// 5     2/07/03 11:42a Derickso
// Changed oreder of port lock and mux lock in LockPortHandler()
//
// 4     2/04/03 5:34p Bmeinke
// Fixed UnlockPortHandler(): Only release Mux lock if the client is
// releasing his last port lock
//
// 3     1/29/03 5:29p Bmeinke
// Added main() (oops)
//
// 2     1/22/03 3:21a Bmeinke
// Dont explicitly call SetProcessName() in the command line argument
// constructor (let the base class hndel it after it parses the command
// line and can determine what our logical port name is)
//
// 1     1/21/03 3:16a Bmeinke
// Logical serial port driver for ISO serial communication
//
//******************************************************************************

#include "IsoCommProxy.h"

/**
 * Default Constructor
 */
IsoCommProxy::IsoCommProxy() : RawCommProxy(),
	m_muxFd( -1), m_muxLockPath("")
{
	SetProcessName( ISO_PROC_NAME);
}

/**
 * Constructor from command line arguments
 *
 * @param argc   Number of command line arguments to aprse
 * @param argv   Array of command line argument strings
 */
IsoCommProxy::IsoCommProxy( int argc, char *argv[]) : RawCommProxy( argc, argv),
	m_muxFd( -1), m_muxLockPath("")
{
}

/**
 * Class destructor
 */
IsoCommProxy::~IsoCommProxy()
{
}

/**
 * Reads logical port configuration from an XML document
 *
 * @param portNode Node containing setup data for our port only
 */
void IsoCommProxy::ReadPortConfig(const XmlNode *portNode)
{
	Log( LOG_FN_ENTRY, "Enter IsoCommProxy::ReadPortConfig()\n");
	// Read base class configuration
	RawCommProxy::ReadPortConfig(portNode);

	try
	{
		// Read the path to the device used to lock the MUX
		// If no MUX is present, use /dev/null
		string muxPath = portNode->getChild("Setup/PortLockDevice")->getValue();
		m_muxLockPath = muxPath;
	}
	catch( XmlException &err)
	{
		m_muxLockPath = "";
	}
	Log( LOG_DEV_DATA, "Path to MUX lock: %s\n", m_muxLockPath.c_str());
	
	Log( LOG_FN_ENTRY, "Exit IsoCommProxy::ReadPortConfig()\n");
}

/**
 * The purpose of this is to give us a place to allocate our own ocb. It is
 * called by ocbCalloc, which in turn is called as a result of an open being done
 * on our reource manager's adopted path(e.g. iofunc_open_default causes it to be
 * called). ocbCalloc was registered through our mount structure.
 *
 * @param ctp    Resource manager's context block
 * @param device Attributes for this client connection (an OCB)
 * @return Pointer to newly allocated OCB or NULL if error
 * @see ResManagerBase::ocbCalloc
 */
IOFUNC_OCB_T* IsoCommProxy::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
	IsoIoOcb_t	*isoOcb;

	Log( LOG_FN_ENTRY, "Enter IsoCommProxy::OcbCalloc()\n");

	isoOcb = new IsoIoOcb_t;

	Log( LOG_FN_ENTRY, "Exit IsoCommProxy::OcbCalloc()\n");

	return( (IOFUNC_OCB_T*)isoOcb);
}

/**
 * Method used to initialize an allocated OCB structure. Called
 * immediately after the OCB is allocated
 *
 * @param ioOcb  OCB structure to be initialized
 */
void IsoCommProxy::OcbInit(resMgrIoOcb_t *ioOcb)
{
	IsoIoOcb_t	*isoOcb = (IsoIoOcb_t*)ioOcb;
	
	Log( LOG_FN_ENTRY, "Enter IsoCommProxy::OcbInit()\n");

	RawCommProxy::OcbInit( ioOcb);
	
	// Default to current protocol settings
	memmove( &isoOcb->xmtProtocol, &m_defaultPortProtocol, sizeof(m_defaultPortProtocol));
	memmove( &isoOcb->rcvProtocol, &m_defaultPortProtocol, sizeof(m_defaultPortProtocol));

	Log( LOG_FN_ENTRY, "Exit IsoCommProxy::OcbInit()\n");
}

/**
 * The purpose of this is to give us a place to free our ocb. It is called
 * by ResManagerBase::ocbFree, which in turn is called as a result of the close
 * being done (e.g. iofunc_close_ocb_default causes it to be called).
 * ResManagerBase::ocbFree was registered through our mount structure.
 *
 * @param ocb    Client's Open Context Block for this connection
 * @see ResManagerBase::ocbFree
 */
void IsoCommProxy::OcbFree(resMgrIoOcb_t *ocb)
{
	IsoIoOcb_t	*isoOcb = (IsoIoOcb_t*)ocb;
	
	delete isoOcb;
}

/**
 * Locks the port for the client associated with 'clientOcb'
 *
 * @param clientOcb OCB associated with the client for whom we are locking the port
 * @param replyId   The reply ID to which we should reply after having locked the port
 *
 * @return EOK if port successfully locked
 */
int IsoCommProxy::LockForClient(CommIoOcb_t *clientOcb, int replyId)
{
	int status = EIO;

	Log( LOG_FN_ENTRY, "Enter IsoCommProxy::LockForClient()\n");
	
	// Lock the MUX for ISO mode
	if( LockMux() == true)
	{
		// Abtain the port lock for the client
		status = RawCommProxy::LockForClient(clientOcb,replyId);
	}
	else
	{
		// Couldn't lock the MUX, so release the port lock
		Log( LOG_ERRORS, "\tError setting ISO mode on mux\n");
	}
	
	Log( LOG_FN_ENTRY, "Exit IsoCommProxy::LockForClient()\n");


	return( status);
}

/**
 * Unlocks the port for the client associated with 'clientOcb'
 *
 * @param clientOcb  OCB associated with the client for whom we are locking the port
 * @param releaseAll Flag to indicate if we should release all locks on the port help by
 *                   the client or just a single lock (set =0 vs. decrement by 1)
 *
 * @return The number of port locks the client still holds
 */
int IsoCommProxy::UnlockForClient(CommIoOcb_t *clientOcb, bool releaseAll)
{
	int retVal = -1;

	Log( LOG_FN_ENTRY, "Enter IsoCommProxy::UnlockForClient()\n");
	
	// Try to release the client's port lock(s)
	retVal = RawCommProxy::UnlockForClient( clientOcb, releaseAll);
	// If all client port locks were released
	if( retVal == 0)
	{
		// Now, try to lock the mux in ISO mode
		if( UnlockMux() != true)
		{
			// Couldn't lock the MUX, so release the port lock
			Log( LOG_ERRORS, "\tError releasing ISO mode on mux\n");
		}
	}
	else
	{
		Log( LOG_DEV_DATA, "Client still has port locked...not releasing MUX lock\n");
	}
	
	Log( LOG_FN_ENTRY, "Exit IsoCommProxy::UnlockForClient()\n");
	
	return( retVal);
}

/**
 * Locks the SCI/ISO mux board for our use
 *
 * @return true if MUX successfully locked, false on error
 */
bool IsoCommProxy::LockMux()
{
	bool retVal = true;

	Log( LOG_FN_ENTRY, "Enter IsoCommProxy::LockMux()\n");
	
	if( m_muxLockPath.empty() == false)
	{
		// If not already in ISO mode
		if( m_muxFd == -1)
		{
			// Enable ISO mode
			if( (m_muxFd=open( m_muxLockPath.c_str(), O_RDWR)) == -1)
			{
				Log( LOG_ERRORS, "\tError opening MUX lock in IsoCommProxy::LockMux: %s\n",
					 strerror( errno));
				retVal = false;
			}
		}
	}
	else
	{
		Log( LOG_DEV_DATA, "\tNo MUX lock required: MUX lock path is empty\n");
		retVal = true;
	}

	Log( LOG_FN_ENTRY, "Exit IsoCommProxy::LockMux()\n");

	return( retVal);
}

/**
 * Unlocks the SCI/ISO mux board for our use
 *
 * @return true if MUX successfully unlocked, false on error
 */
bool IsoCommProxy::UnlockMux()
{
	bool retVal = false;

	Log( LOG_FN_ENTRY, "Enter IsoCommProxy::UnlockMux()\n");
	
	if( m_muxLockPath.empty() == false)
	{
		// If ISO lock is valid
		if( m_muxFd != -1)
		{
			// Release ISO lock
			close( m_muxFd);
			m_muxFd = -1;

			retVal = true;
		}
		else
		{
			Log( LOG_ERRORS, "\tAttempt to unlock invalid MUX lock\n");
		}
	}
	else
	{
		Log( LOG_DEV_DATA, "\tNo MUX unlock required: MUX lock path is empty\n");
		retVal = true;
	}

	Log( LOG_FN_ENTRY, "Exit IsoCommProxy::UnlockMux()\n");

	return( retVal);
}

