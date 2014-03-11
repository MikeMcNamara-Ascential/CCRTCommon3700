/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** DATATBL.H
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Handling CIP Data Read/Write requests
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>0----<major>         Customization: <none>0----<major>
**
*****************************************************************************
*****************************************************************************
*/

#ifndef DATATBL_H
#define DATATBL_H


typedef enum tagDataTableElementType        /* Type of data table elements */
{
    typeSINT,
    typeINT,
    typeDINT,
    typeREAL
}
DataTableElementType;

#define SINT_TYPE    0xC2;
#define INT_TYPE    0xC3;
#define DINT_TYPE    0xC4;
#define REAL_TYPE    0xCA;

typedef struct tagDataTableEntry                /* Structure to store data table entries */
{
    INT32       nOffset;                            /* Offset in the Assembly buffer */
    UINT16   iSize;                                /* Data size in bytes */
    char     Tag[MAX_REQUEST_TAG_SIZE+1];        /* Data Entry tag */    
    DataTableElementType elementType;            /* Type of data table elements */
}
DataTableEntry;

#define DATA_TABLE_ENTRIES        4        /* Total number of Data Table entries */

#define INVALID_DATA_TABLE_ENTRY    (-1)

extern DataTableEntry dataTableEntries[DATA_TABLE_ENTRIES];

extern void  dataTableInit(void);
extern void  dataTableParseRequest( REQUEST* pRequest );

#endif /* #ifndef DATATBL_H */
