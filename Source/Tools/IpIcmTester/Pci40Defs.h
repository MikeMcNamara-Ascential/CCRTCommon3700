//******************************************************************************
//
// $Archive: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tools/TOOLS/IpIcmTester/Pci40Defs.h $
// $Author: Derickso $
//   $Date: 11/02/07 8:08a $
//
// Description:
//    Greenspring PCI-40 PCI IP Carrier card definitions for use with the
//		IP ICM hardware installation test utility
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tools/TOOLS/IpIcmTester/Pci40Defs.h $
// 
// 1     11/02/07 8:08a Derickso
// Test report generator tool
// 
// 1     7/20/05 8:13p Cward
// Directory structure change
// 
// 1     12/21/03 6:32p Cward
// 
// 1     8/25/03 3:02p Bmeinke
// Initial checkin
// 
//
//******************************************************************************

#ifndef _PCI40DEFS_H_INCLUDED
#define _PCI40DEFS_H_INCLUDED

#include <stdint.h>


// pci vendor
#define			PCI_40_VENDOR_ID			0x124b
#define			PCI_40_DEVICE_ID			0x0040
// io size
#define			PCI_40_BASE_ADDRESS_REG		2
#define			PCI_40_IO_SIZE				0x5000		//0x4300 must be 4k blocks
#define			PCI_40_SIZE					0x3000000	//must be 4k blocks

#define			ID_STRING_MAX_LENGTH		0x18
#define			TYPE_1_SPEED_CELL			0x07
#define			TYPE_1_MFG_ID_CELL			0x09
#define			TYPE_1_MODEL_NO_CELL		0x0b
#define			TYPE_1_REVISION_CELL		0x0d
#define			TYPE_1_SOFTWARE_LO_CELL		0x11
#define			TYPE_1_SOFTWARE_HI_CELL		0x13

#define			TYPE_2_SPEED_CELL			0x14
#define			TYPE_2_MFG_ID_CELL			0x06
#define			TYPE_2_MODEL_NO_CELL		0x0a
#define			TYPE_2_REVISION_CELL		0x0c
#define			TYPE_2_SOFTWARE_LO_CELL		0x10
#define			TYPE_2_SOFTWARE_HI_CELL		0x12

#define			MAX_IP_MODULES				4
#define			CNTL0_CLKA					0x01
#define			CNTL0_CLKB					0x02
#define			CNTL0_CLKC					0x04
#define			CNTL0_CLKD					0x08
#define			CNTL0_CLR_AUTO				0x10
#define			CNTL0_AUTO_ACK				0x20
#define			CNTL0_INTEN					0x40
#define			CNTL0_INTSET				0x80
#define			CNTL1_IRQ_A0_PENDING		0x01
#define			CNTL1_IRQ_A1_PENDING		0x02
#define			CNTL1_IRQ_B0_PENDING		0x04
#define			CNTL1_IRQ_B1_PENDING		0x08
#define			CNTL1_IRQ_C0_PENDING		0x10
#define			CNTL1_IRQ_C1_PENDING		0x20
#define			CNTL1_IRQ_D0_PENDING		0x40
#define			CNTL1_IRQ_D1_PENDING		0x80
#define			CNTL2_AUTO_INT_SET			0x40
#define			CNTL2_LINT					0x80

#define			MAX_PCI_40_CARDS			2

		  
// interpreted module information
typedef struct
{
	unsigned	type;
	unsigned	speed;								// Hz
	unsigned	mfg;
	unsigned	model;
	unsigned	revision;
	unsigned	driver;

} IpInformation_t;


typedef struct
{
	unsigned	installed;							// Is this IP installed: 0=No 1=Yes

	unsigned	promDataFormat;					// I or II
	unsigned	speed;								// MHz

	char		idString[ ID_STRING_MAX_LENGTH];

} IpModuleInformation_t;


typedef struct
{
	IpModuleInformation_t	ipModule;

	uint64_t				ioIndexAddress;
	uint64_t				idIndexAddress;
	uint64_t				intIndexAddress;
	uint64_t				memIndexAddress;

} IpSlotLocation_t;



typedef struct
{
	void*				handle;					// -1=Card Not installed

	void*				pci9080Addr;
	void*				pci40Addr;
	char*				ctrl0Address;
	char*				ctrl1Address;
	char*				ctrl2Address;

	IpSlotLocation_t	Ip[ MAX_IP_MODULES];

} Pci40Information_t;


typedef struct 
{
	/**
	 * IP Carrier card properties
	 */
	Pci40Information_t		m_cardInfo;

	/**
	 * number of carrier cards found
	 */
	bool					m_pci40Found;

	/**
	 * Memory address of the PCI-9080 PCI interface chip
	 */
	uint32_t				m_9080Addr;

	/**
	 * Base PCI address of the PCI40 card
	 */
	uint32_t				m_pci40Addr;

	/**
	 * PCI bus on whcih the PCI40 resides
	 */
	unsigned				m_busNo;
	
	/**
	 * PCI deviec and function numbers
	 */
	unsigned				m_devFuncNo;


	/**
	 * PCI index of this card
	 */
	unsigned				m_cardIdx;
} Pci40_t;

#endif // _PCI40DEFS_H_INCLUDED

