/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** hashtable.c
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
#include "platform.h"

#ifdef EIP_LARGE_CONFIG_OPT

hashTable* platformHashTableCreate(int size, hashTableCompareFn* compareFunc,
								   hashTableHashFn* hashFunc, INT32 arg)
{
	hashTable* table;
	int i;

	/* Verify parameters */
	if ((size <= 0) || (compareFunc == NULL) || (hashFunc == NULL))
		return NULL;


	table = malloc(sizeof(hashTable));
	if (table != NULL)
	{
		table->elements = 1 << size;
		table->compareFunc = compareFunc;
		table->keyHashFunc = hashFunc;
		table->hashArg = arg;

		table->list = malloc(sizeof(hashList)*table->elements);
		if (table->list == NULL)
		{
			free(table);
			return NULL;
		}

		for (i = 0; i < table->elements; i++)
		{
			table->list[i].head = NULL;
			table->list[i].tail = NULL;
		}
	}

	return table;
}

int platformHashTablePut(hashTable* pTable, hashNode* pNode)
{
	int hashIndex;

	if (pTable == NULL)
		return -1;

	hashIndex = pTable->keyHashFunc(pTable->elements, pNode, pTable->hashArg);
	if ((pNode->next = pTable->list[hashIndex].head) == NULL)
		pTable->list[hashIndex].head = pTable->list[hashIndex].tail = pNode;
	else
		pTable->list[hashIndex].head = pNode;

	return 0;
}

hashNode* platformHashTableFind(hashTable* pTable, hashNode* pMatchNode, INT32 hashArg)
{
	int hashIndex;
	hashNode* pNode;

	if (pTable == NULL)
		return NULL;

	hashIndex = pTable->keyHashFunc(pTable->elements, pMatchNode, pTable->hashArg);
	pNode = pTable->list[hashIndex].head;

	while ((pNode != NULL) &&
		   (pTable->compareFunc(pMatchNode, pNode, hashArg) == 0))
	{
		pNode = pNode->next;
	}

	return pNode;
}

int platformHashTableRemove(hashTable* pTable, hashNode* pNode)
{
	int hashIndex;
	hashNode* pPreviousNode;

	if (pTable == NULL)
		return -1;

	hashIndex = pTable->keyHashFunc(pTable->elements, pNode, pTable->hashArg);

	pPreviousNode = pTable->list[hashIndex].head;
	while (pPreviousNode->next != NULL)
	{
		if (pPreviousNode->next == pNode)
			break;

		pPreviousNode = pPreviousNode->next;
	}

	if (pPreviousNode->next == NULL)
		pPreviousNode = NULL;

	if (pPreviousNode == NULL)
	{
		pTable->list[hashIndex].head = pNode->next;
		if (pTable->list[hashIndex].tail == pNode)
			pTable->list[hashIndex].tail = NULL;
	}
	else
	{
		pPreviousNode->next = pNode->next;
		if (pTable->list[hashIndex].tail == pNode)
			pTable->list[hashIndex].tail = pPreviousNode;
	}

	return 0;
}

#endif