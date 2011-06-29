//******************************************************************************
//
// $Archive: /Core/ServicesLayer/Library/Shmem.cpp $
// $Author: Cward $
//   $Date: 1/16/04 4:51p $
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
//     $Log: /Core/ServicesLayer/Library/Shmem.cpp $
// 
// 1     1/16/04 4:51p Cward
// Refactoring Services Layer Includes and Library
//
//******************************************************************************
#include "Shmem.h"

#include <string>
#include <fcntl.h>			// O_RDWR, etc



Shmem::Shmem() : m_memPtr( MAP_FAILED), m_memName( ""), m_creator( false),
    m_memSize( 0), m_memFd( -1)
{
}

Shmem::~Shmem()
{
    Close();
}

void* Shmem::Open( const char *name, 	
                   size_t size,
                   void *addr/*=0*/,
                   off_t offset/*=0*/,
                   UINT16 mapProt/*=PROT_READ|PROT_WRITE|PROT_NOCACHE*/,
                   UINT16 mapFlag/*=MAP_SHARED*/)
{
    // Close any existing mapings
    Close();

    // Create the shared memory area
    m_creator = false;
    if( (m_memFd=shm_open( name, O_RDWR, 0777)) == -1)
    {
        // Open failed...try to create
        if( (m_memFd=shm_open( name, O_RDWR | O_CREAT | O_EXCL, 0777)) != -1)
        {
            m_creator = true;
        }
    }

    // If we created the shared memory area
    if( m_memFd != -1)
    {
        // Set the size of the shared memory area
        if( ftruncate( m_memFd, size) != -1)
        {
            // Map the area into our process space
            if( (m_memPtr=mmap( 0, size, mapProt, mapFlag, m_memFd, offset)) != MAP_FAILED)
            {
                m_memSize = size;
            }
        }
    }

    return( m_memPtr);
}

void Shmem::Close( void)
{
    if( IsOpen())
    {
        // Unmap the memory from our process space
        munmap( m_memPtr, m_memSize);
        m_memPtr = MAP_FAILED;

        // Close the file descriptor
        close( m_memFd);
        m_memFd = -1;

        // Remove the shared memory 'file' if necessary
        if( m_creator)
        {
            shm_unlink( m_memName.c_str());
            m_creator = false;
        }
    }
}

void* Shmem::GetMapAddress() const
{
    return( m_memPtr);
}

const std::string& Shmem::GetName() const
{
    return( m_memName);
}

int Shmem::GetSize() const
{
    return( m_memSize);
}

bool Shmem::AmCreator() const
{
    return( m_creator);
}

int Shmem::GetMemFd() const
{
    return( m_memFd);
}

bool Shmem::IsOpen() const
{
    return( m_memPtr != MAP_FAILED);
}

