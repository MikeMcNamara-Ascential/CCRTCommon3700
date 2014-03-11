/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** hashtable.h
**
** COPYRIGHT (c) 2012 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Simple hash table for EIP_LARGE_CONFIG_OPT functionality
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>----0<major>         Customization: <none>----0<major>
**
*****************************************************************************
*****************************************************************************
*/
#ifndef EIP_HASH_TABLE_H
#define EIP_HASH_TABLE_H

#ifdef EIP_LARGE_CONFIG_OPT

typedef BOOL hashTableCompareFn(void *pNode1, void *pNode2, INT32 arg);
typedef int hashTableHashFn(int elemCount, void *pNode, INT32 arg);

typedef struct hashNode
{
    struct hashNode *next;
} hashNode;

typedef struct hashList
{
    struct hashNode *head;
    struct hashNode *tail;
} hashList;

typedef struct hashTable
{
	hashTableCompareFn* compareFunc;
	hashTableHashFn*	keyHashFunc;
	hashList* list;
	int hashArg;
	int elements;

} hashTable;

hashTable* platformHashTableCreate(INT32 size, hashTableCompareFn* compareFunc,
								   hashTableHashFn* hashFunc, INT32 TableArg);
int platformHashTablePut(hashTable* pTable, hashNode* pNode);
hashNode* platformHashTableFind(hashTable* pTable, hashNode* pMatchNode, INT32 hashArg);
int platformHashTableRemove(hashTable* pTable, hashNode* pNode);

#endif

#endif