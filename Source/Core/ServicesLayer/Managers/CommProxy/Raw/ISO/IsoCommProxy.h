//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/CommProxy/Raw/ISO/IsoCommProxy.h $
// $Author:: Cward                                                            $
//   $Date:: 1/23/04 9:39a                                                    $
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
//     $Log: /Core/ServicesLayer/Managers/CommProxy/Raw/ISO/IsoCommProxy.h $
// 
// 1     1/23/04 9:39a Cward
// Restructuring Raw comm proxies
// 
// 3     10/28/03 8:26a Derickso
// Latest changes to compile.
// 
// 1     8/21/03 9:05a Derickso
// 05WK CCRT Project
// 
// 3     8/12/03 2:52p Khykin
// Updating the include path.
// 
// 2     8/06/03 3:51p Khykin
// Updated with latest changes from Flatrock.
// 
// 2     6/26/03 1:42p Khykin
// Updated with latest Flatrock changes.
// 
// 2     6/24/03 10:19a Bmeinke
// Moved the define of READER_PROCESSES from RawCommProxy.h to the header
// files for the individual comm proxies
// 
// 1     6/24/03 10:13a Bmeinke
// 
// 1     6/24/03 10:12a Bmeinke
// 
// 1     4/30/03 4:40p Bmeinke
// Migration to new personalized configuration management structure
// 
// 5     2/13/03 10:10a Bmeinke
// Removed data members m_reflCoid and m_reflChid (not used)
// Removed InitManager(), CleanupManager() and WaitForReflection() methods
// (no longer needed)
// 
// 4     2/11/03 12:19p Bmeinke
// Replaced LockPortHandler() and UnlockPortHandler() with LockForClient()
// and UnlockForClient()
// 
// 3     2/06/03 9:44a Bmeinke
// Changed IsoIoOcb_t::xmtProtocol and IsoIoOcb_t::rcvProtocol from struct
// termios to SerialProtocol_t
// 
// 2     1/22/03 3:25a Bmeinke
// Removed GetRegisteredName() (base class can handle it)
// 
// 1     1/21/03 3:17a Bmeinke
// Logical serial port driver for ISO serial communication
// 
//******************************************************************************

#ifndef _ISO_COMM_PROXY_H_INCLUDED
#define _ISO_COMM_PROXY_H_INCLUDED

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

// Define this here so it does not get defined in the neutrino headers

#ifndef IOFUNC_OCB_T
struct _isoIoOcb_t;
#define IOFUNC_OCB_T			struct _isoIoOcb_t
#endif

#ifndef RESMGR_OCB_T
struct _isoIoOcb_t;
#define RESMGR_OCB_T			struct _isoIoOcb_t
#endif

/**
 * Flag used to indicate whether or not the reader thread will process all received data.
 * If set to 0, the reader thread will simply stick received data into our rx fifo and 
 * send a pulse back to our I/O channel to indicate that new rx data needs to be processed
 */
#define READER_PROCESSES		1

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "RawCommProxy.h"		// Base logical port driver class

#include <stdint.h>				// uint8_t
#include <set>
#include <map>
#include <string>
#include <fcntl.h>				// O_CREAT

#include "BepTimer.h"		   		// Timer object

/**
 * Name to register the ISO port driver process under
 */
#define ISO_PROC_NAME			"IsoPortDriver"

/**
 * Path to the SCI/ISO mux manager's ISO lock
 */
#define SCI_ISO_MUX_MGR_PATH	"/dev/SciIsoMux/Iso"

/**
 * OCB Structure extended for ISO client conenctions
 */
typedef struct _IsoIoOcb_t
{
	CommIoOcb_t			proxyOcb;

	/**
	 * Transmit specific protocol settings
	 */
	SerialProtocol_t	xmtProtocol;

	/**
	 * Receive specific protocol settings
	 */
	SerialProtocol_t	rcvProtocol;

} IsoIoOcb_t;


/**
 * ISO serial port communication proxy. Used to perform port locking and serial
 * data logging. Clients will communicate to a serial device through this process.
 * 
 * @author Brian Meinke
 * @version 1
 * @see RawCommProxy
 * @since Jan 20, 2003
 */
class IsoCommProxy : public RawCommProxy
{
public:
	/**
	 * Default constructor
	 */
	IsoCommProxy();

	/**
	 * Constructor from command line arguments
	 * 
	 * @param argc   Number of command line arguments to aprse
	 * @param argv   Array of command line argument strings
	 */
	IsoCommProxy( int argc, char *argv[]);

	/**
	 * Class destructor
	 */
	virtual ~IsoCommProxy();

	/**
	 * Reads logical port configuration from an XML document
	 *
	 * @param portNode Node containing setup data for our port only
	 */
	virtual void ReadPortConfig(const XmlNode *portNode);

protected:

	/**
     * Reads serial data from the serial driver
     *
     * @param buff    Buffer to read the serial data into
     * @param buffLen Number of bytes to read
     * @param timeOut Timeout value (in nanoSeconds)
     * @return Number of bytes read, or -1 if an error occurred
     */
    int ReadDriverData( uint8_t *buff, size_t buffLen, const uint64_t &timeOut);
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
	virtual IOFUNC_OCB_T* OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device);

	/**
	 * Method used to initialize an allocated OCB structure. Called
	 * immediately after the OCB is allocated
	 *
	 * @param ioOcb  OCB structure to be initialized
	 */
	virtual void OcbInit(resMgrIoOcb_t *ioOcb);

	/**
	 * The purpose of this is to give us a place to free our ocb. It is called
	 * by ResManagerBase::ocbFree, which in turn is called as a result of the close
	 * being done (e.g. iofunc_close_ocb_default causes it to be called).
	 * ResManagerBase::ocbFree was registered through our mount structure.
	 *
	 * @param ocb    Client's Open Context Block for this connection
	 * @see ResManagerBase::ocbFree
	 */
	virtual void OcbFree(resMgrIoOcb_t *ocb);

	/**
	 * Locks the port for the client associated with 'clientOcb'
	 *
	 * @param clientOcb OCB associated with the client for whom we are locking the port
	 * @param replyId   The reply ID to which we should reply after having locked the port
	 *
	 * @return EOK if port successfully locked
	 */
	virtual int LockForClient(CommIoOcb_t *clientOcb, int replyId=-1);

	/**
	 * Unlocks the port for the client associated with 'clientOcb'
	 *
	 * @param clientOcb  OCB associated with the client for whom we are locking the port
	 * @param releaseAll Flag to indicate if we should release all locks on the port help by
	 *                   the client or just a single lock (set =0 vs. decrement by 1)
	 *
	 * @return The number of port locks the client still holds
	 */
	virtual int UnlockForClient(CommIoOcb_t *clientOcb, bool releaseAll=true);

	/**
	 * Locks the SCI/ISO mux board for our use
	 * 
	 * @return true if MUX successfully locked, false on error
	 */
	bool LockMux();
	
	/**
	 * Unlocks the SCI/ISO mux board for our use
	 * 
	 * @return true if MUX successfully unlocked, false on error
	 */
	bool UnlockMux();

protected:

	/**
	 * Handle to mux control manager
	 */
	int					m_muxFd;

	/**
	 * Path to the SCI/ISO MUX board manager
	 */
	std::string			m_muxLockPath;

};

#endif // _ISO_COMM_PROXY_H_INCLUDED
