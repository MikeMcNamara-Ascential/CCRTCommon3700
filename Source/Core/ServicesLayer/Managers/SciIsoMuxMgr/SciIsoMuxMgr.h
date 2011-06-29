//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/SciIsoMuxMgr/SciIsoMuxMgr.h $
// $Author:: Cward                                                            $
//   $Date:: 12/21/03 6:28p                                                   $
//
// Description:
//	Manager used to control the SCI/ISO mux board
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
//     $Log: /Core/ServicesLayer/Managers/SciIsoMuxMgr/SciIsoMuxMgr.h $
// 
// 1     12/21/03 6:28p Cward
// 
// 1     10/07/03 11:33a Khykin
// Initial checkin
// 
// 1     7/08/03 1:01p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:26p Khykin
// Initial checkin
// 
// 1     6/12/03 4:21p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:45a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:28a Khykin
// Updating with latest code from Pit machine debug.
// 
// 2     1/03/03 4:23p Bmeinke
// Changed base class from ResManagerBase to ResManagerXMl in order to be
// able to handle system XML messages (i.e Register)
// 
// 1     8/05/02 9:53a Bmeinke
// Initial checkin
// 
//******************************************************************************

#ifndef _SCI_ISO_MUX_MANAGER_H_INCLUDED
#define _SCI_ISO_MUX_MANAGER_H_INCLUDED

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!

#include "ResManagerXml.h"		// I/O Manager base class

// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// !!!!!!! DO NOT CHANGE ORDER OF STRUCT DEFS / INCLUDE FILES !!!!!!!!!
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#include <stdint.h>		// uint8_t, etc
#include <sys/stat.h>

#include "INamedDataBroker.h"

/**
 * Root namespace path (relative to /dev) under which the SCI and ISO paths 
 * will be attached
 */
#define SCI_ISO_MUX_PATH_ROOT	"SciIsoMux"

/**
 * Default name to register with the OS
 */
#define SCI_ISO_MUX_NAME		"mgr/MuxCtrl"

/**
 * SCI/ISO mux board resource manager. Used to control access to
 * the SCI/ISO mux board switch.
 * 
 * @author Brian Meinke
 * @version 1
 * @since 30 July 2002
 */
class SciIsoMuxMgr : public ResManagerXml
{
public:
	/**
	 * Default constructor
	 */
	SciIsoMuxMgr();

	/**
	 * Command line argument constructor
	 * 
	 * @param argc   Number of command line arguments in the argv parameter
	 * @param argv   Array of command line arguments
	 */
	SciIsoMuxMgr( int argc, char *argv[]);

	/**
	 * Destructor
	 */
	~SciIsoMuxMgr();

	/**
	 * Method used to set all of the objects in the component
	 * to their original states.  This would includes all
	 * member variables such as buffers, lists, and stored
	 * data.
	 * @since Version 1.0
	 */
	virtual void Reset(void);
	
	/**
	 * Reads configuration data from the given XML node
	 *
	 * @param document Reference to a parsed configuration document.
	 * @since Version 1.0
	 */
	virtual void Initialize(const XmlNode *document);
	
	/**
	 * Perform any server initialization
	 *
	 * @return true if successful
	 *         false if error
	 */
	virtual bool InitManager();
	
	/**
	 * Performs any necessary manager cleanup (free memory, detach any
	 * adopted path names, etc)
	 */
	virtual void CleanupManager();

	/**
	 * Method that is called to command the component to perform any
	 * system registration functions necessary.
	 *
	 * @return Status of the registration process.
	 *         Success, Fail
	 * @since Version 2.5
	 */
	virtual const std::string Register(void);

protected:
		
	/**
	 * Attaches the path namespace wxtension for handling SCI locks of
	 * the SCI/ISO mux
	 * 
	 * @return true if path successfully attached, false if an error occurs
	 */
	bool AttachSciPath();
	
	/**
	 * Attaches the path namespace wxtension for handling ISO locks of
	 * the SCI/ISO mux
	 * 
	 * @return true if path successfully attached, false if an error occurs
	 */
	bool AttachIsoPath();

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
	 * Method used to finalize an allocated OCB structure. Called immediately
	 * before the OCB is destroyed
	 *
	 * @param ioOcb  OCB structure to be finalized
	 */
	//virtual void OcbFinalize(resMgrIoOcb_t *ioOcb);
	
	/**
	 * Message handler for _IO_READ I/O messages. Called by ResManagerBase::ioRead
	 *
	 * @param ctp    Dispatch Interface pointer
	 * @param msg    Read message data
	 * @param ioOcb  Control Block for this client
	 * @return 0 if successful
	 *         <0 if error occurs
	 * @see ResManagerBase::ioRead
	 */
	//virtual int IoRead(resmgr_context_t *ctp, io_read_t *msg, resMgrIoOcb_t *ioOcb);
	
	/**
	 * Message handler for _IO_WRITE I/O messages. Called ny ResManagerBase::ioWrite
	 *
	 * @param ctp    Dispatch Interface pointer
	 * @param msg    Write message data
	 * @param ioOcb  Control Block for this client
	 * @return 0 if successful
	 *         <0 if error occurs
	 * @see ResManagerBase::ioWrite
	 */
	//virtual int IoWrite(resmgr_context_t *ctp, io_write_t *msg, resMgrIoOcb_t *ioOcb);
	
	/**
	 * Function synthesized by the base-layer library when the last
	 * close has been received for a particular OCB. Called by ioCloseOcb
	 *
	 * @param ctp      Dispatch interface pointer
	 * @param reserved Reserved for ther base-layer library
	 * @param ioOcb    Control block for the client
	 * @return 0 if successful
	 *         < 0 if error occurs
	 * @see ResManagerBase::ioCloseOcb
	 */
	//virtual int IoCloseOcb(resmgr_context_t *ctp, void *reserved, resMgrIoOcb_t *ioOcb);

	/**
	 * Initializes the named data broker interface object
	 */
	void InitializeNdb();

private:

	/**
	 * XML node tag used to define which system tag is used to read/write 
	 * the state of the SCI/ISO mux bit
	 */
	const std::string	m_muxSwitchTag;

	/**
	 * System tag used to read/write the state of the SCI/ISO mux bit
	 */
	std::string			m_muxSwitchName;

	/**
	 * Value to write via NDB to enable SCI mode
	 */
	std::string			m_sciModeVal;

	/**
	 * Value to write via NDB to enable ISO mode
	 */
	std::string			m_isoModeVal;

	/**
	 * Device number for the attached SCI path name
	 */
	int					m_sciDevNo;

	/**
	 * Device number for the attached ISO path name
	 */
	int					m_isoDevNo;

	/**
	 * Mutex to lock access to the MUX switch
	 */
	pthread_mutex_t		m_muxLock;

	/**
	 * Interface to the Named Data Broker. Used to read/write that status of 
	 * the SCI/ISO mux line
	 */
	INamedDataBroker	*m_dataBroker;

	/**
	 * I/O function attributes for the SCI namespace path
	 */
	iofunc_attr_t		m_sciFuncAttr;

	/**
	 * I/O function attributes for the ISO namespace path
	 */
	iofunc_attr_t		m_isoFuncAttr;
};

#endif // _SCI_ISO_MUX_MANAGER_H_INCLUDED

