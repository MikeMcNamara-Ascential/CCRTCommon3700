/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** ENETLINK.H
**
** COPYRIGHT (c) 2001-2006 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Ethernet Link object 
**
*****************************************************************************
*****************************************************************************
**
** Source Change Indices
** ---------------------
**
** Porting: <none>0----<major>         Customization: <none>0----<major>
**
*****************************************************************************/


#ifndef ENETLINK_H
#define ENETLINK_H


#define ENETLINK_CLASS                    0xF6            /* Ethernet Link class identifier */
#define ENETLINK_CLASS_REVISION            3                /* Ethernet Link class revision */

/* Class and instance attribute numbers */
#define ENETLINK_CLASS_ATTR_REVISION				1
#define ENETLINK_CLASS_ATTR_MAX_INSTANCE			2
#define ENETLINK_CLASS_ATTR_NUM_INSTANCES			3
#define ENETLINK_CLASS_ATTR_OPT_ATTR_LIST			4
#define ENETLINK_CLASS_ATTR_MAX_CLASS_ATTR			6
#define ENETLINK_CLASS_ATTR_MAX_INST_ATTR			7


#define ENETLINK_INST_ATTR_INTERFACE_SPEED			1
#define ENETLINK_INST_ATTR_INTERFACE_FLAGS			2
#define ENETLINK_INST_ATTR_PHYSICAL_ADDRESS			3
#define ENETLINK_INST_ATTR_INTERFACE_COUNTERS		4
#define ENETLINK_INST_ATTR_MEDIA_COUNTERS			5
#define ENETLINK_INST_ATTR_INTERFACE_CONTROL		6
#define ENETLINK_INST_ATTR_INTERFACE_TYPE			7
#define ENETLINK_INST_ATTR_INTERFACE_STATE			8
#define ENETLINK_INST_ATTR_ADMIN_STATE				9
#define ENETLINK_INST_ATTR_INTERFACE_LABEL			10

#define ENETLINK_ATTR_OPT_COUNT						4

#define ENETLINK_GET_AND_CLEAR      0x4C        /* Additional service code */

#define MAC_ADDR_LENGTH							6  /* Size of the MAC Id in bytes */
#define ENETLINK_INTERFACE_MAX_LABEL_SIZE		65 /* Maximum size of Interface label string (including NULL character) */

#define ENETLINK_FLAG_LINK_ACTIVE         0x00000001    /* Bit 0 indicates whether link is active */
#define ENETLINK_FLAG_SPEED_DUPLEX_FINE   0x0000000C    /* Speed and duplex has been successfully negotiated */
#define ENETLINK_FLAG_RESET_REQUIRED	  0x00000020	/* Reset required to change ethernet link attributes */

#define EL_FLAG_DUPLEX_MODE                0x00000002    /* Bit 1 indicates whether it is duplex mode */

/* Interface control attribute values */
#define ENETLINK_INTERFACE_CONTROL_BITS_MASK			0x0003
#define ENETLINK_INTERFACE_CONTROL_AUTO_NEG_ENABLED		0x0001
#define ENETLINK_INTERFACE_CONTROL_FORCED_FULL_DUPLEX	0x0002

/* Interface type attribute values */
#define ENETLINK_INTERFACE_TYPE_UNKNOWN			0
#define ENETLINK_INTERFACE_TYPE_INTERNAL		1
#define ENETLINK_INTERFACE_TYPE_TWISTED_PAIR	2
#define ENETLINK_INTERFACE_TYPE_OPTICAL_FIBER	3

/* Interface state attribute values */
#define ENETLINK_INTERFACE_STATE_UNKNOWN		0
#define ENETLINK_INTERFACE_STATE_ENABLED		1
#define ENETLINK_INTERFACE_STATE_DISABLED		2
#define ENETLINK_INTERFACE_STATE_TESTING		3

/* Admin state attribute values */
#define ENETLINK_ADMIN_STATE_ENABLED			1
#define ENETLINK_ADMIN_STATE_DISABLED			2

/* Class attribute structure */
typedef struct tagENETLINK_CLASS_ATTR
{
   UINT16  iRevision;
   UINT16  iMaxInstance;
   UINT16  iNumInstances;
   UINT16  iMaxClassAttr;
   UINT16  iMaxInstanceAttr;
}
ENETLINK_CLASS_ATTR;

#define ENETLINK_CLASS_ATTR_SIZE    10

/* Instance attribute structure */
typedef struct tagENETLINK_INST_ATTR
{
    UINT32   lInterfaceSpeed;
    UINT32   lInterfaceFlags;
    UINT8    macAddr[MAC_ADDR_LENGTH];
#ifdef EIP_INTERFACE_CONTROL_SUPPORT
	UINT16	iInterfaceControlBits;
	UINT16	iForcedInterfaceSpeed;
#endif
	UINT8	 bInterfaceType;
	UINT8	 bInterfaceState;
	UINT8	 bAdminState;
	INT8     InterfaceLabel[ENETLINK_INTERFACE_MAX_LABEL_SIZE];
}
ENETLINK_INST_ATTR;

/* The GetAttributeAll must leave space for these attributes, so we 
 * need their size */
#define ENETLINK_INTERFACE_COUNTER_SIZE		(11*sizeof(UINT32))
#define ENETLINK_MEDIA_COUNTER_SIZE			(12*sizeof(UINT32))
#ifndef EIP_INTERFACE_CONTROL_SUPPORT
#define ENETLINK_INTERFACE_CONTROL_SIZE		(2*sizeof(UINT16))
#endif

extern void enetlinkInit(ENETLINK_INST_ATTR* pEnetLinkAttr);
extern void enetlinkParseClassInstanceRqst( REQUEST* pRequest );

extern UINT32 platformGetNumEnetLinkInstances();
extern void platformGetEnetLinkAttributes(UINT32 iInstance, ENETLINK_INST_ATTR* pEnetLinkAttr);
extern void platformSetEnetLinkAttributes(UINT32 lMask, UINT32 iInstance, ENETLINK_INST_ATTR* pEnetLinkAttr);
extern BOOL platformTestSetEnetLinkAttributes(UINT32 lMask, UINT32 iInstance, ENETLINK_INST_ATTR* pEnetLinkAttr, UINT8* pbGeneralError);

#endif /* #ifndef ENETLINK_H */

