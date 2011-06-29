//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Include/Shmem.h $
// $Author: Cward $
//   $Date: 1/16/04 4:45p $
//
// Description:
//     QNX Shared Memory Wrapper class
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
//     $Log: /Core/ServicesLayer/Include/Shmem.h $
// 
// 2     1/16/04 4:45p Cward
// Refactoring Services Layer Includes and Library
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
// 1     6/12/03 4:23p Khykin
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
// 1     1/18/03 8:06a Khykin
// Initial Checkin From DCTC
//
// 4     6/05/02 11:20p Bmeinke
// Cleaned up unused ivar's and added comments
//
// 3     5/30/02 9:45p Bmeinke
// Added comments to all methods which needed them
//
// 2     4/15/02 1:52p Bmeinke
// Added implementation to class methods
//
// 1     4/10/01 10:46a Admin
//
// 1     2/15/01 12:13p Brian.meinke
// QNX Shared Memory Wrapper class
//
//******************************************************************************

#ifndef _SHMEM_H_INCLUDED_
#define _SHMEM_H_INCLUDED_

//------------------------------------------------------------------------------
// SYSTEM HEADER FILES INCLUSIONS
//------------------------------------------------------------------------------

#include <string>
#include <sys/mman.h>       // PROT_READ etc..

//------------------------------------------------------------------------------
// USER DEFINED HEADER FILE INCLUSIONS
//------------------------------------------------------------------------------

#include "BepDefs.h"

//------------------------------------------------------------------------------
// GLOBAL DEFINITIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EXTERNAL DECLARATIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CONSTANT DEFINITIONS
//------------------------------------------------------------------------------

/**
 * Shared memory object. Used to share an area of memory between
 * processes
 *
 * @author Brian Meinke
 * @version 1
 */
class Shmem
{
public:
	/**
	 * Default constructor
	 */
	Shmem();

	/**
	 * Destructor
	 */
	~Shmem();

	/**
	 * Opens a shared memory area
	 *
	 * @param name    name of the shared memory area to open
	 * @param size    Size (in bytes) of the shared memory area to open
	 * @param addr    Addr to be opened
	 * @param offset  Offset into the shared memory area to be mapped
	 *                into our process space
	 * @param mapProt Shared memory protection flags
	 * @param mapFlag Flags to use when mapping the shared memory area into our
	 *                process space
	 * @return Pointer to the shared memory area, or (void*)-1 on error
	 */
	void* Open( const char *name, 	
				size_t size,
				void *addr=0,
				off_t offset=0,
				UINT16 mapProt=PROT_READ|PROT_WRITE|PROT_NOCACHE,
				UINT16 mapFlag=MAP_SHARED);

	/**
	 * Closes the shared memory object and unmaps it from our process space
	 */
	void Close( void);

	/**
	 * Returns a pointer to the base address (within our process space) of
	 * the shared memory area
	 *
	 * @return A pointer to the base address (within our process space) of
	 *         the shared memory area
	 */
	void* GetMapAddress() const;
	
	/**
	 * Returns the name of the shared memory area
	 *
	 * @return The name of the shared memory area
	 */
	const std::string& GetName() const;
	
	/**
	 * Returns the size (in bytes) of the shared memory area
	 *
	 * @return The size (in bytes) of the shared memory area
	 */
	int GetSize() const;
	
	/**
	 * Returns true if we created the shared memory object or false if
	 * we did not create it (just opened it)
	 *
	 * @return true if we created the shared memory object or false if
	 *         we did not create it (just opened it)
	 */
	bool AmCreator() const;
	
	/**
	 * Returns the file descriptor of the shared memory object
	 *
	 * @return The file descriptor of the shared memory object
	 */
	int GetMemFd() const;
	
	/**
	 * Returns true if the memory object has been opened, false if not
	 *
	 * @return true if the memory object has been opened, false if not
	 */
	bool IsOpen() const;

private:
	/**
	 * Pointer to the shared memory mapping
	 */
	void 	*m_memPtr;

	/**
	 * Name of the shared memory area
	 */
	std::string	m_memName;

	/**
	 * Flag to indicate if we created the shared memory area
	 */
	bool	m_creator;

	/**
	 * Size (in bytes) of the shared memory area
	 */
	int		m_memSize;

	/**
	 * File descriptor of the shared memory area
	 */
	int		m_memFd;
};

#endif	// _SHMEM_H_INCLUDED_

