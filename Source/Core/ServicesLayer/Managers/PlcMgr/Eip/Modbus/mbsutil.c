#include "mbsinc.h"

#ifdef ET_IP_MODBUS

#ifdef MBUS_STACK_MEMORY
UINT8  gmbusMemoryPool[MEMORY_POOL_SIZE];
UINT8* gpmbusMemoryPool;
#endif


/*---------------------------------------------------------------------------
** mbus_malloc( )
**
** Stores passed in data in the memory pool.
** If data is NULL then memory is reserved, but not filled in.
** Returns the offset to the memory pool the data is stored at.
**---------------------------------------------------------------------------
*/

UINT8* mbus_malloc( UINT8* data, UINT16 iDataSize )
{
#ifndef MBUS_STACK_MEMORY
	UINT8* p;

	if (iDataSize == 0)
		return NULL;

	p = (UINT8*)malloc(iDataSize);
	if (p == NULL)
	{
		mbusDumpStr1(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_MSG, 0, 0, "Out of memory for %d bytes\n", iDataSize);
		mbus_notifyEvent(NM_MBUS_ERROR_OUT_OF_MEMORY, 0, 0 );
		return NULL;
	}

	if (data)
		memcpy(p, data, iDataSize);

	return p;

#else
	UINT8* iOffset = gpmbusMemoryPool;

	if ( iDataSize == 0)    /* If there is no data, do not allocate anything */
		return NULL;

	/* Check if we have enough room */
	if ( iDataSize > (&gmbusMemoryPool[MEMORY_POOL_SIZE] - gpmbusMemoryPool) )
	{
		mbusDumpStr2(MBUS_TRACE_LEVEL_ERROR, MBUS_TRACE_TYPE_MSG, 0, 0, "utilAddToMemoryPool ran out of memory: Requested: %hd, Available: %hd",
			iDataSize, (&gmbusMemoryPool[MEMORY_POOL_SIZE] - gpmbusMemoryPool));
		mbus_notifyEvent(NM_MBUS_ERROR_OUT_OF_MEMORY, 0, 0 );
		return NULL;
	}

	if ( data )    /* If data pointer is not NULL then store the data in the dynamic pool */
		memcpy( gpmbusMemoryPool, data, iDataSize );

	gpmbusMemoryPool += iDataSize;    /* Adjust the pool ceiling offset */

	return iOffset;    /* The data is stored at this offset */

#endif /* MBUS_STACK_MEMORY */
}

/*---------------------------------------------------------------------------
** mbus_free( )
**
** Removes the specified data block from the memory pool.
**---------------------------------------------------------------------------
*/

void mbus_free( UINT8** piData, UINT16* piDataSize )
{
#ifndef MBUS_STACK_MEMORY
    if ( !(*piDataSize) || (*piData == NULL))
        return;    /* Check that memory block has valid offset and size */

    free(*piData);
#else

    mbus_Request* pRequest;
    mbus_TcpSession* pSession;

    UINT16 iDataSize = *piDataSize;
    UINT16 iDataOffset = *piData - gMemoryPool;
    INT32 nLen = gpmbusMemoryPool - *piData - iDataSize;
        
    if ( !iDataSize || *piData == NULL)
        return;    /* Check that memory block has valid offset and size */

    /* Quick garbage collection is done every time the memory is released, instead of doing the 
       full pool unfragmentation when needed */
    memcpy(*piData, *piData + iDataSize, nLen);
    
    gpmbusMemoryPool -= iDataSize;    /* Adjust the pool ceiling offset */
    
    /* Adjust the offsets */
    for( pRequest = gRequests; pRequest < gpnRequests; pRequest++ )
    {
        if ( pRequest->cfg.pData != NULL && pRequest->cfg.pData > *piData )
            pRequest->cfg.pData -= iDataSize;
    }

    for (pSession = gmbsTCPSessions; pSession < gpnmbsTCPSessions; pSession++)
    {
        if ((pSession->pPartialRecvPacket != NULL) && (pSession->pPartialRecvPacket > *piData))
            pSession->pPartialRecvPacket -= iDataSize;

        if ((pSession->pPartialSendPacket != NULL) && (pSession->pPartialSendPacket > *piData))
            pSession->pPartialSendPacket -= iDataSize;
    }

#endif /* MBUS_STACK_MEMORY */

    /* Make sure the same memory block is not released twice */
    *piDataSize = 0;        
    *piData = NULL;    
}

/*---------------------------------------------------------------------------
** mbus_realloc( )
**
** Removes the old data block from the memory pool and replaces it with the 
** new one with different length.
**---------------------------------------------------------------------------
*/

void mbus_realloc( UINT8** piData, UINT16* piDataSize, UINT8* data, UINT16 iDataSize )
{
    mbus_free( piData, piDataSize );
    *piDataSize = iDataSize;
    *piData = mbus_malloc( data, iDataSize );
}

#endif
