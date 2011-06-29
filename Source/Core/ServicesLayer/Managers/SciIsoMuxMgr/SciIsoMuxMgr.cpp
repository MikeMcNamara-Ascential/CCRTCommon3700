//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Managers/SciIsoMuxMgr/SciIsoMuxMgr.cpp $
// $Author: Cward $
//   $Date: 12/21/03 6:28p $
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
//     $Log: /Core/ServicesLayer/Managers/SciIsoMuxMgr/SciIsoMuxMgr.cpp $
// 
// 1     12/21/03 6:28p Cward
// 
// 2     10/24/03 1:29p Khykin
// Updated with mutex changes.
// 
// 2     10/10/03 9:43a Khykin
// Updated with latest Flatrock changes.
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
// 1     6/12/03 3:59p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:50a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:29a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.3  2003/02/23 17:16:18  tbochene
//     New mux controller
//
// 
// 5     2/11/03 1:49p Bmeinke
// If we failed to register the SciIsoMux manager's name, the wrong name
// was given in the statement printed out when reporting the error
// 
// 4     1/03/03 4:23p Bmeinke
// Changed base class from ResManagerBase to ResManagerXMl in order to be
// able to handle system XML messages (i.e Register)
// 
// 3     11/22/02 3:18p Khykin
// Removing the force for the logging enabling.
// 
// 2     10/08/02 2:54p Bmeinke
// Fixed return value of AttachSciPath and AttachSciPath (were returning
// int, not bool)
// Replaced BposAttachName with name_attach because we need to attach our
// named channel to the existing dispatch pointer created for our I/O
// channel
// 
// 1     8/05/02 9:52a Bmeinke
// Initial chceckin
// 
//******************************************************************************

#include "SciIsoMuxMgr.h"

/**
 * Default constructor
 */
SciIsoMuxMgr::SciIsoMuxMgr() : ResManagerXml(), 
    m_muxSwitchTag( "MuxTag"), m_muxSwitchName( "UninitMuxTag"), 
	m_sciModeVal( "1"), m_isoModeVal( "0"), m_sciDevNo( -1),
	m_isoDevNo( -1), m_dataBroker( NULL)
{

	SetProcessName( SCI_ISO_MUX_NAME);
//	EnableLogger();

    pthread_mutexattr_t attr;
    
	memset( &attr, 0, sizeof( attr));
	memset( &m_muxLock, 0, sizeof( m_muxLock));
	
	pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_DISABLE);
    pthread_mutex_init( &m_muxLock, &attr);
}

/**
 * Command line argument constructor
 * 
 * @param argc   Number of command line arguments in the argv parameter
 * @param argv   Array of command line arguments
 */
SciIsoMuxMgr::SciIsoMuxMgr( int argc, char *argv[]) : ResManagerXml(), 
    m_muxSwitchTag( "MuxTag"), m_muxSwitchName( "UninitMuxTag"), 
	m_sciModeVal( "1"), m_isoModeVal( "0"), m_sciDevNo( -1),
	m_isoDevNo( -1), m_dataBroker( NULL)
{
	SetProcessName( SCI_ISO_MUX_NAME);
//	EnableLogger();
	
	pthread_mutexattr_t attr;
	
	memset( &attr, 0, sizeof( attr));
	memset( &m_muxLock, 0, sizeof( m_muxLock));
    
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutexattr_setrecursive(&attr, PTHREAD_RECURSIVE_ENABLE);
    pthread_mutex_init( &m_muxLock, &attr);

	ParseCommandLine(argc,argv);
}

/**
 * Destructor
 */
SciIsoMuxMgr::~SciIsoMuxMgr()
{
	pthread_mutex_destroy( &m_muxLock);
}

/**
 * Method used to set all of the objects in the component
 * to their original states.  This would includes all
 * member variables such as buffers, lists, and stored
 * data.
 * @since Version 1.0
 */
void SciIsoMuxMgr::Reset(void)
{
	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::Reset()\n");
	
	InitializeNdb();
	
	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::Reset()\n");
}

/**
 * Reads configuration data from the given XML node
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void SciIsoMuxMgr::Initialize(const XmlNode *document)
{
	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::Initialize()\n");

	try
	{
		ResManagerXml::Initialize(document);
	}
	catch( XmlException &err)
	{
		Log( LOG_ERRORS, "\tXML Exception in ResManagerXml::Initialize: %s\n",
			 err.GetReason());
	}
	catch( BepException &err)
	{
		Log( LOG_ERRORS, "\tBEP Exception in ResManagerXml::Initialize: %s\n",
			 err.GetReason());
	}
	catch( ...)
	{
		Log( LOG_ERRORS, "\tUnknown Exception in ResManagerXml::Initialize\n");
	}

	try
	{
		try
		{
			const XmlNode *sciValNode = document->getChild( "Setup/SciModeVal");
			m_sciModeVal = sciValNode->getValue();
			Log( LOG_DEV_DATA, "  SCI Enable Value: '%s'\n", m_sciModeVal.c_str());
		}
		catch( XmlException &err)
		{
		}
		
		try
		{
			const XmlNode *isoValNode = document->getChild( "Setup/IsoModeVal");
			m_isoModeVal = isoValNode->getValue();
			Log( LOG_DEV_DATA, "  ISO Enable Value: '%s'\n", m_isoModeVal.c_str());
		}
		catch( XmlException &err)
		{
		}
		
		const XmlNode *muxTagNode = document->getChild( "Setup/TagNames/MuxTag");
		m_muxSwitchName = muxTagNode->getValue();
		Log( LOG_DEV_DATA, "  MUX Control I/O Tag: '%s'\n", m_muxSwitchName.c_str());
	}
	catch( XmlException &err)
	{
		Log( LOG_ERRORS, "\tXML Exception in SciIsoMuxMgr::Initialize: %s\n",
			 err.GetReason());
	}

	Log( LOG_FN_ENTRY, "Exit SciIsoMuxMgr::Initialize()\n");
}

/**
 * Perform any server initialization
 *
 * @return true if successful
 *         false if error
 */
bool SciIsoMuxMgr::InitManager()
{
	bool	retVal = false;
	
	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::InitManager()\n");
	
	try
	{
		// Perform base class initialization
		if( (retVal=ResManagerBase::InitManager()) == true)
		{
			if( (retVal=AttachSciPath()) == false)
			{
				Log( LOG_ERRORS, "\tError attaching SCI path: %s\n",
					 strerror( errno));
			}
			else if( (retVal=AttachIsoPath()) == false)
			{
				Log( LOG_ERRORS, "\tError attaching ISO path: %s\n",
					 strerror( errno));
			}
			else
			{
				// Initialize the Resource Manager Context
				RegisterManager();

				// Register name with OS
				if( BposAttachName( GetProcessName().c_str(), (void**)(&m_nameId)) != BP_ERR_KRNL_FAIL)
				{
					retVal = true;
				}
				else
				{
					Log( LOG_ERRORS, "\tError registering name %s: %s\n", GetProcessName().c_str(), strerror( errno));
				}
			}
		}
		else
		{
			Log( LOG_ERRORS, "\tError in base class InitManager()\n");
		}
	}
	catch( BepException &err)
	{
		Log( LOG_ERRORS, "\tInitManager exception: %s\n", strerror( errno));
	}
	Log( LOG_FN_ENTRY, "Exit SciIsoMuxMgr::InitManager()\n");

	return( retVal);
}

/**
 * Performs any necessary manager cleanup (free memory, detach any
 * adopted path names, etc)
 */
void SciIsoMuxMgr::CleanupManager()
{
	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::CleanupManager()\n");

	// Detach SCI path namespace extension
	PathDetach( m_sciDevNo);

	// Detach ISO path namespace extension
	PathDetach( m_isoDevNo);

	// Base class cleanup
	ResManagerXml::CleanupManager();

	if( m_nameId != NULL)
	{
		name_detach( m_nameId, 0);
		m_nameId = NULL;
	}

	Log( LOG_FN_ENTRY, "Exit SciIsoMuxMgr::CleanupManager()\n");
}

/**
 * Method that is called to command the component to perform any
 * system registration functions necessary.
 *
 * @return Status of the registration process.
 *         Success, Fail
 * @since Version 2.5
 */
const std::string SciIsoMuxMgr::Register(void)
{
	std::string	retVal;

	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::Register()\n");

	InitializeNdb();
	retVal = ResManagerXml::Register();

	Log( LOG_FN_ENTRY, "Exit SciIsoMuxMgr::Register()\n");

	return( retVal);
}

/**
 * Attaches the path namespace wxtension for handling SCI locks of
 * the SCI/ISO mux
 * 
 * @return true if path successfully attached, false if an error occurs
 */
bool SciIsoMuxMgr::AttachSciPath()
{
	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::AttachSciPath()\n");
	// Initialize the I/O functions attributes for the SCI path
	m_sciDevNo = PathAttach( SCI_ISO_MUX_PATH_ROOT"/Sci", 1, S_IFNAM|0666, 
							 m_sciDevNo, 0, &m_sciFuncAttr, true);
	Log( LOG_FN_ENTRY, "Exit SciIsoMuxMgr::AttachSciPath()\n");
	
	return( m_sciDevNo != -1);
}

/**
 * Attaches the path namespace wxtension for handling ISO locks of
 * the SCI/ISO mux
 * 
 * @return true if path successfully attached, false if an error occurs
 */
bool SciIsoMuxMgr::AttachIsoPath()
{
	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::AttachIsoPath()\n");
	// Initialize the I/O functions attributes for the SCI path
	m_isoDevNo = PathAttach( SCI_ISO_MUX_PATH_ROOT"/Iso", 1, S_IFNAM|0666, 
							 m_isoDevNo, 0, &m_isoFuncAttr, true);
	Log( LOG_FN_ENTRY, "Exit SciIsoMuxMgr::AttachIsoPath()\n");
	
	return( m_isoDevNo != -1);
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
IOFUNC_OCB_T* SciIsoMuxMgr::OcbCalloc(resmgr_context_t *ctp, IOFUNC_ATTR_T *device)
{
	IOFUNC_OCB_T	*newOcb = NULL;
	std::string		ndbResp, muxVal;
	int				lockErr;

	// If attempting to open SCI mode
	if( (device == &m_sciFuncAttr) || ( device == &m_isoFuncAttr))
	{
		if( (lockErr=pthread_mutex_lock( &m_muxLock)) == EOK)
		{
			newOcb = ResManagerXml::OcbCalloc( ctp, device);

			// If INamedDataBroker has been initialized
			if( m_dataBroker != NULL)
			{
				// Decide new state of MUX ctrl line
				if( device == &m_sciFuncAttr)	muxVal = "1";
				if( device == &m_isoFuncAttr)	muxVal = "0";
				
				m_dataBroker->Write( m_muxSwitchName, muxVal, ndbResp, true);
			}
		}
		else
		{
			Log( LOG_ERRORS, "\tUnable to lock mutex in SciIsoMuxMgr::OcbCalloc(): '%s'\n",
				 strerror( lockErr));
		}
	}
	else
	{
		Log( LOG_ERRORS, "\tUnknown device type(%lld) in SciIsoMuxMgr::OcbCalloc()\n",
			 device->inode);
	}

	return( newOcb);
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
void SciIsoMuxMgr::OcbFree(resMgrIoOcb_t *ocb)
{
	IOFUNC_ATTR_T *device = (IOFUNC_ATTR_T*)ocb->m_ioOcb.attr;

	// If attempting to open SCI mode
	if( (device == &m_sciFuncAttr) || ( device == &m_isoFuncAttr))
	{
		ResManagerXml::OcbFree( ocb);
		pthread_mutex_unlock( &m_muxLock);
	}
	else
	{
		Log( LOG_ERRORS, "\tUnknown device type(%lld) in SciIsoMuxMgr::OcbFree()\n",
			 device->inode);
	}
}

/**
 * Initializes the named data broker interface object
 */
void SciIsoMuxMgr::InitializeNdb()
{
	Log( LOG_FN_ENTRY, "Enter SciIsoMuxMgr::InitializeNdb()\n");

	if( m_dataBroker)	delete	m_dataBroker;
	m_dataBroker = NULL;
	m_dataBroker = new INamedDataBroker( IsDebugOn());
	
	Log( LOG_FN_ENTRY, "Exit SciIsoMuxMgr::InitializeNdb()\n");
}



