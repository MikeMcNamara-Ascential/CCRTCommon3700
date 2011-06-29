//******************************************************************************
//
// $Archive: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/Pci40.h $
//  $Author: Cward $
//    $Date: 4/13/06 4:06a $
//
// Description:
//    GreenSpring PCI IP Carrier card
//
//==============================================================================
//
// Copyright  1999 Burke E. Porter Machinery Co.
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interest thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/Pci40.h $
// 
// 2     4/13/06 4:06a Cward
// moved implementation to cpp file
//
// 2     2/15/06 4:08p Bmeinke
// Moved all code to a cpp file
//
// 1     12/21/03 6:28p Cward
//
// 2     10/24/03 1:22p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:34a Khykin
// Initial checkin
//
// 1     7/08/03 1:02p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:26p Khykin
// Initial checkin
//
// 2     6/26/03 2:01p Khykin
// Updated with Brians latest changes.
//
// 2     6/18/03 7:32p Bmeinke
// Check the PCI40 control registers to see if any of the IP slots gets
// configured for 32MHz instead of 8MHz
//
// 1     4/30/03 4:43p Bmeinke
// Migration to new personalized configuration management structure
//
// 3     3/10/03 7:01p Bmeinke
// Added commands needed to enable interrupts in the PCI-9080 PCI
// interface chip
//
// 2     2/25/03 6:31p Bmeinke
// Changed pci_attach_device() to pci_find_device() because
// pci_attach_device() could not read address data from the PCI40 card
//
//******************************************************************************


class Pci40;

#ifndef _PCI40_H_INCLUDED_
#define _PCI40_H_INCLUDED_

#include <sys/mman.h>
#include <hw/pci.h>
#include <vector>

#include "ILogger.h"	// Logger interface

typedef std::vector< Pci40*>			IpCarrierList_t;
typedef IpCarrierList_t::iterator		IpCarrierListItr_t;
typedef IpCarrierList_t::const_iterator	IpCarrierListCItr_t;

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
	 * PCI index of the PCI-40 card
	 */
	unsigned				m_cardIdx;

} Pci40_t;

class Pci40 : public ILogger, public Pci40_t
{
public:
	Pci40();

	~Pci40();

	/**
	 * Initializes our logger interface object
	 *
	 * @param loggerCfg The logger object whose configuration we want to copy
	 */
	void Initialize( const ILogger &loggerCfg);

	/**
	 * Attempts to initialize the PCI40 IP carrier at the given index
	 * on the PCI bus
	 *
	 * @param cardIdx Relative PCI index of the PCI40 card on the PCI bus (relative
	 *                meaning that the 1st PCI40 card found would use a cardIdx of 0
	 *                even though it may be the 4th PCI device on the PCI bus)
	 *
	 * @return true if successfully initialized, false if an error occurrs
	 */
	bool Initialize( int cardIdx);

	/**
	 * Initialize PCI-40 Card for proper access
	 *
	 * @return -1 if an error occurred or 0 if successful
	 */
	int EnableIrq();

	/**
	 * Disables interrupts on the IP carrier card
	 */
	void DisableIrq();

	/**
	 * Locates a particular IP Module and returns a handle to it.
	 *
	 * @param mfgIDTarget
	 *               Manufacturer ID to look for
	 * @param modelTarget
	 *               Model number to look for
	 * @param which  Which module number, 0=first 1=second etc
	 *
	 * @return -1  Not Found, 0<= Handle to IP.  Byte0=Module  Byte1=Card
	 */
	int LocateIP( unsigned mfgIDTarget, unsigned modelTarget, unsigned which);

	/**
	 * Gets module information into given structure
	 *
	 * @param handle Handle to specify which IP
	 * @param ipInfo Structure to load information into
	 *
	 * @return -1 if an error occurred or 0 if successful
	 */
	int GetIpInformation( int ipHandle, IpInformation_t &ipInfo);

	/**
	 * Returns base IO address, of given IP module, local memory
	 *
	 * @param handle Handle of IP to get address for
	 *
	 * @return 0 if an error occurred; any other value is the base address
	 *         of the IP module
	 */
	void *GetIpBaseIOAddress( int ipHandle);

	/**
	 * Returns the PCI IRQ assigned to the carrier
	 *
	 * @return The PCI IRQ assigned to the carrier
	 */
	int GetPicIrq() const;

	/**
	 * Returns a mask representing which IPs have IRQs asserted. Bit mask
	 * as follows:
	 * 	Bit		Value	Meaning
	 * 	0/1		1		IP A has active IRQ
	 * 	2/3		1		IP B has active IRQ
	 * 	4/5		1		IP C has active IRQ
	 * 	6/7		1		IP D has active IRQ
	 *
	 * @return Bit mask of active IP irqs (0 means no IP hace active IRQs)
	 */
	uint8_t GetActiveIpIrqs() const;

protected:

	/**
	 * Local data initialization
	 */
	void InitializePci40Data( void );

	/**
	 * Tries to allocate memory map region to pci40
	 *
	 * @return -1 if an error occurred or 0 if successful
	 */
	int AllocatePci40Memory();

	/**
	 * Initialize PCI-40 Card for proper access
	 *
	 * @return -1 if an error occurred or 0 if successful
	 */
	int InitializePci40Card();

	/**
	 * Identify IP modules installed on specified card
	 *
	 * @return -1 if an error occurred; any other value represents the number of
	 *         IP modules found
	 */
	int IdentifyIPModules();

private:

	/**
	 * Handle to the PCI server
	 */
	int			m_pciHandle;

	/**
	 * Index of our Ip carrier card on the PCI bus (the order it was found)
	 */
	int			m_cardIdx;

	/**
	 * PCI IRQ assigned to this carrier
	 */
	int			m_pciIrq;
};

#endif // _PCI40_H_INCLUDED_

