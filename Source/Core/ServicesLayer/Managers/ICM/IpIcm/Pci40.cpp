//******************************************************************************
// Description:
//    GreenSpring PCI IP Carrier card
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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Managers/ICM/IpIcm/Pci40.cpp $
// 
// 1     4/13/06 3:11a Cward
// Moved implementation from headers to cpp
//
// 1     2/15/06 12:05p Bmeinke
// Moved code from header to cpp file
//
// 3     2/03/06 1:58p Bmeinke
// Moved implementation to cpp file
//
// 2     12/21/05 4:15p Bmeinke
// Changed the ioIndexAddress, idIndexAddress, intIndexAddress and
// memIndexAddress members of IpSlotLocation_t to void*
//
// 1     11/17/05 11:22a Bmeinke
//
// 6     10/19/05 2:45p Bmeinke
// Added a GetBaseAddres() method to return the address that the card is
// mapped to
//
// 5     10/12/05 5:12p Bmeinke
// Disable interrupts before detaching from the PC Iserver in the
// destructor
// Do not enable interrupts inside InitializePci40Card(); do it inside
// EnableIrq
// Stop reading ID bytes once we see a single invalid ID byte value
//
// 4     10/04/05 4:33p Bmeinke
// Fixed the clock frequency printout inside EnableIrq() and
// InitializePci40Card()
// Changed GetActiveIpIrqs() to return a uint16_t with the upper bits
// coming from control register 2 (for bus error detection)
// Made many variables volatile
//
// 2     10/04/05 4:29p Bmeinke
// Fixed the clock freq
//
// 1     10/03/05 12:48p Bmeinke
// Adapted from Pci40.h
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <sys/neutrino.h>

#if _NTO_VERSION >= 630
    #include <errno.h>
#endif
#include "Pci40.h"

Pci40::Pci40() : m_pciHandle( -1), m_cardIdx( -1)
{
#if _NTO_VERSION < 630
	InitializePci40Data();
#endif
}

Pci40::~Pci40()
{
	Log( LOG_FN_ENTRY, "Enter Pci40:~Pci40()\n");
	m_pci40Found = false;

	// Detach from the PCI server
	if( m_pciHandle != -1)	pci_detach( m_pciHandle);
	m_pciHandle = -1;

	DisableIrq();

	Log( LOG_FN_ENTRY, "Exit Pci40:~Pci40()\n");
}

/**
 * Initializes our logger interface object
 *
 * @param loggerCfg The logger object whose configuration we want to copy
 */
void Pci40::Initialize( const ILogger &loggerCfg)
{
	SetAdditionalLogFileName( loggerCfg.GetAdditionalLogFileName());
	SetProcessName( loggerCfg.GetProcessName());
	SetVerboseMask( loggerCfg.GetVerboseMask());
	SetDebug(loggerCfg.IsDebugOn());
	SetLocalEcho( loggerCfg.GetLocalEcho());
	if( loggerCfg.GetLogStatus())
	{
		EnableLogger();
	}
}

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
bool Pci40::Initialize( int cardIdx)
{
	bool 		retVal = true;
	int			ipCount=0;
	uint32_t	pciIrq;

	Log( LOG_FN_ENTRY, "Enter Pci40::Initialize( %d)\n", cardIdx);
	printf("Enter Pci40::Initialize( %d)\n", cardIdx);

	m_cardIdx = cardIdx;

	// init
	InitializePci40Data();

	// Attach to the PCI server
	if( m_pciHandle == -1)	m_pciHandle = pci_attach(0);

	if( m_pciHandle != -1)
	{
		if( pci_find_device( PCI_40_DEVICE_ID, PCI_40_VENDOR_ID, cardIdx, &m_busNo, &m_devFuncNo) == PCI_SUCCESS)
		{
			m_9080Addr =  (uint32_t)-1;
			m_pci40Addr = (uint32_t)-1;
			/************************************************************************************
			Read chip PCI address:
				The 1st BAR (0x10) is the PCI memory address of the PCI-9080
				The 2nd BAR (0x14) is the PCI I/O address of the PCI-9080 I/O
				The 3rd BAR (0x18) is the PCI memory address of the PCI40 and IP modules
			************************************************************************************/
			int status9080 = pci_read_config32( m_busNo, m_devFuncNo, 0x10, 1, &m_9080Addr);
			printf("\tpcsi_read_config32 for 9080 - %d, PCI_BAD_REGISTER_NUMBER: %d, PCI_SUCCESS: %d\n", 
					status9080, PCI_BAD_REGISTER_NUMBER, PCI_SUCCESS);
			printf("\tm_9080Addr = %08X\n", m_9080Addr);
			
			pci_read_config32( m_busNo, m_devFuncNo, 0x18, 1, &m_pci40Addr);
			pci_read_config32( m_busNo, m_devFuncNo, 0x3C, 1, &pciIrq);
			m_pciIrq = pciIrq & 0x000F;

#if _NTO_VERSION >= 630
			m_9080Addr = m_pci40Addr + 0x04000000;
			printf("\tForcing m_9080 address to m_pci40Addr + 0x0400 0000: %08X\n", m_9080Addr);
#endif
			Log( LOG_DEV_DATA, "PCI40 found; Bus %d devFuncNo %d Address $%08X IRQ %d\n", m_busNo, m_devFuncNo, m_pci40Addr, m_pciIrq);
			printf("PCI40 found; Bus %d devFuncNo %d Address $%08X IRQ %d\n", m_busNo, m_devFuncNo, m_pci40Addr, m_pciIrq);
			// get access to this card
			if( 0 == AllocatePci40Memory())
			{
				// found at least one PCI-40
				m_pci40Found = true;
				Log( LOG_DEV_DATA, "PCI-40 Count = %d\n", m_pci40Found);
				printf("PCI-40 Count = %d\n", m_pci40Found);

				// init this thing
				if( 0 == InitializePci40Card())
				{
					// identify IP modules installed on this card
					if( 0 <= (ipCount = IdentifyIPModules()))
					{
						Log( LOG_DEV_DATA, "PCI-40 Card %d has %d IP module(s)\n", m_cardIdx, ipCount);
					}
					else
					{
						Log( LOG_DEV_DATA, "PCI-40 Card %d has NO IP modules\n", m_cardIdx);
					}
				}
				else
				{
					Log( LOG_ERRORS, "\tInitializePci40Card( %d) failed\n", m_cardIdx);
				}
			}
			else
			{
				Log( LOG_ERRORS, "\tAllocatePci40Memory( %d) failed\n", m_cardIdx);
			}
		}
		else
		{
			Log( LOG_ERRORS, "\tError %d attaching to PCI40 device: %s\n", errno, strerror(errno));
		}
	}
	else
	{
		Log( LOG_ERRORS, "\tError %d attaching to PCI server: %s\n", errno, strerror(errno));
	}

	// if no boards found
	if( m_pci40Found == false)
	{
		Log( LOG_ERRORS, "\tPCI-40 Carrier card %d not found\n", cardIdx);
		retVal = false;
	}

	Log( LOG_FN_ENTRY, "Exit Pci40::Initialize( %d)\n", cardIdx);

	return( retVal);
}

/**
 * Initialize PCI-40 Card for proper access
 *
 * @return -1 if an error occurred or 0 if successful
 */
int Pci40::EnableIrq()
{
	int		retVal=-1;
	uint8_t	*cntl0, *cntl1, *cntl2, junk;

	Log( LOG_FN_ENTRY, "Enter Pci40::EnableIrq( %d)\n", m_cardIdx);

	// verify this card exists
	if( (uint32_t)-1 != m_pci40Addr)
	{
		// check all IP module locations
		cntl0 = (uint8_t *)m_cardInfo.ctrl0Address;
		cntl1 = (uint8_t *)m_cardInfo.ctrl1Address;
		cntl2 = (uint8_t *)m_cardInfo.ctrl2Address;

		// enable auto bus error acknowledge, else system will lock up on access to missing IP module
		Log( "Control0 value: $%02hhX\n", *cntl0);
		Log( LOG_DEV_DATA, "Enable auto bus error acknowledge and interrupts\n");
		*cntl0 = CNTL0_AUTO_ACK | CNTL0_INTEN;

		junk = *cntl0;
		if( junk & 0x01)	Log( "\n\tIP SlotA Clock set to 32 MHz: $%02hhX\n", junk);
		else				Log( "\n\tIP SlotA Clock set to 8 MHz: $%02hhX\n", junk);

		junk = *cntl1;
		if( junk & 0x01)	Log( "\n\tIP SlotB Clock set to 32 MHz: $%02hhX\n", junk);
		else				Log( "\n\tIP SlotB Clock set to 8 MHz: $%02hhX\n", junk);

		junk = *cntl2;
		if( junk & 0x01)	Log( "\n\tIP SlotC Clock set to 32 MHz: $%02hhX\n", junk);
		else				Log( "\n\tIP SlotC Clock set to 8 MHz: $%02hhX\n", junk);

		// Enable IP interrupts to reach the PCI bus via the PCI-9080 PCI controller
		//	on the IP carrier
		uint32_t *intScr = (uint32_t*)(((char*)m_cardInfo.pci9080Addr)+0x68);
		*intScr = 0x00010900;

		// good
		retVal = 0;
	}
	else
	{
		Log( LOG_ERRORS, "\tEnableIrq() error: no base address\n");
	}

	Log( LOG_FN_ENTRY, "Exit Pci40::EnableIrq( %d)\n", m_cardIdx);
	// return status
	return( retVal);
}

/**
 * Disables interrupts on the IP carrier card
 */
void Pci40::DisableIrq()
{
	uint32_t	*intScr, pci9080Int;
	uint8_t		*cntl0, pci40Int;
	
	Log( LOG_FN_ENTRY, "Enter Pci40::DisableIrq()\n");
	printf("Enter Pci40::DisableIrq()\n");

	// verify this card exists
	if( (uint32_t)-1 != m_pci40Addr)
	{
		// Disable interrupts in the PCI-40 chip
		Log( LOG_DEV_DATA, "Disable PCI-40 interrupts\n");
		printf("Disable PCI-40 interrupts\n");
		cntl0 = (uint8_t *)m_cardInfo.ctrl0Address;
		pci40Int = *cntl0;
		pci40Int &= ~(CNTL0_INTEN);
		*cntl0 = pci40Int;

		// Disable interrupts in the PCI-9080 PCI controller
		Log( LOG_DEV_DATA, "Disable PCI-9080 interrupts\n");
		printf("Disable PCI-9080 interrupts\n");
		intScr = (uint32_t*)(((char*)m_cardInfo.pci9080Addr)+0x68);
		printf("\tintScr set to %08X\n", intScr);
		pci9080Int = *intScr;
		printf("\tSet pci9080Int = *intScr\n");
		pci9080Int &= ~(0x00010100);
		printf("\tCleared bits in pci9080Int\n");
		*intScr = pci9080Int;
		printf("\tWrote value back to intScr\n");
	}
	else
	{
		Log( LOG_ERRORS, "\tDisableIrq() error: no base address\n");
		printf("\tDisableIrq() error: no base address\n");
	}

	Log( LOG_FN_ENTRY, "Exit Pci40::DisableIrq()\n");
	printf("Exit Pci40::DisableIrq()\n");
}

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
int Pci40::LocateIP( unsigned mfgIDTarget, unsigned modelTarget, unsigned which)
{
	int			ipHandle=-1;
	unsigned	modIdx;
	unsigned	ipMfgId, ipModel, ipIdx;

	Log( LOG_FN_ENTRY, "Enter Pci40::LocateIP( $%x, $%x, %d)\n", mfgIDTarget, modelTarget, which);

	// if card installed
	if( (uint32_t)-1 != m_pci40Addr)
	{
		// reset occurance
		ipIdx = 0;

		// check all module locations on the card
		for( modIdx=0; modIdx<MAX_IP_MODULES; modIdx++)
		{
			// if IP module installed here
			if( 0 < m_cardInfo.Ip[ modIdx].ipModule.installed)
			{
				//
				switch( m_cardInfo.Ip[ modIdx].ipModule.promDataFormat)
				{
				case 1:
					ipMfgId = m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_MFG_ID_CELL];
					ipModel = m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_MODEL_NO_CELL];
					break;
				case 2:
					// mfg
					ipMfgId = 0;
					ipMfgId = (ipMfgId << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MFG_ID_CELL+0];
					ipMfgId = (ipMfgId << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MFG_ID_CELL+1];
					ipMfgId = (ipMfgId << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MFG_ID_CELL+2];
					// model
					ipModel = 0;
					ipModel = (ipModel << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MODEL_NO_CELL+0];
					ipModel = (ipModel << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MODEL_NO_CELL+1];
					break;
				default:
					ipMfgId = (unsigned)-1;
					ipModel = (unsigned)-1;
					break;
				}

				// now see if this is the one being looked for
				if((mfgIDTarget == ipMfgId) && (modelTarget == ipModel))
				{
					// if this is the index of the item we are lokking for
					if( ipIdx == which)
					{
						// this is the handle for this IP module on this card
						ipHandle = modIdx;
						Log( LOG_DEV_DATA, "Found IP  Card:%d  Module: %d  Mfg:$%x  Model:$%x  No:%d  Handle:$%x\n",
							   m_cardIdx, modIdx, ipMfgId, ipModel, ipIdx, ipHandle);
						break;
					}

					ipIdx++;
				}
			}
		}
	}

	Log( LOG_FN_ENTRY, "Exit Pci40::LocateIP()\n");
	return( ipHandle);
}

/**
 * Gets module information into given structure
 *
 * @param handle Handle to specify which IP
 * @param ipInfo Structure to load information into
 *
 * @return -1 if an error occurred or 0 if successful
 */
int Pci40::GetIpInformation( int ipHandle, IpInformation_t &ipInfo)
{
	int			retVal=-1;
	unsigned	modIdx;

	Log( LOG_FN_ENTRY, "Enter Pci40::GetIpInformation()\n");

	// extract location information
	modIdx = ipHandle & 0x0ff;

	// if card and module installed
	if( ((uint32_t)-1 != m_pci40Addr) &&
		(0 < m_cardInfo.Ip[ modIdx].ipModule.installed) )
	{
		//
		switch( m_cardInfo.Ip[ modIdx].ipModule.promDataFormat)
		{
		case 1:
			retVal = 0;						// valid
			ipInfo.type = 1;			// type 1
			// speed
			{
				if( 'C' == m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_SPEED_CELL])
				{
					ipInfo.speed = 8000000;
				}
				else if( 'H' == m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_SPEED_CELL])
				{
					ipInfo.speed = 32000000;
				}
				else
				{
				}
			}
			// mfg
			{
				ipInfo.mfg = m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_MFG_ID_CELL] & 0x0ff;
			}
			// model
			{
				ipInfo.model = m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_MODEL_NO_CELL] & 0x0ff;
			}
			// revision
			{
				ipInfo.revision = m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_REVISION_CELL] & 0x0ff;
			}
			// software driver
			{
				ipInfo.driver = 0;
				ipInfo.driver = (ipInfo.driver << 8 ) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_SOFTWARE_HI_CELL] & 0x0ff;
				ipInfo.driver = (ipInfo.driver << 8 ) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_SOFTWARE_LO_CELL] & 0x0ff;
			}
			break;
		case 2:
			retVal = 0;						// valid
			ipInfo.type = 2;			// type 2
			// speed
			{
				// supports 8 MHz
				if( 0x02 & m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_SPEED_CELL])
				{
					ipInfo.speed = 8000000;
				}
				// supports 32 MHz
				if( 0x04 & m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_SPEED_CELL])
				{
					ipInfo.speed = 32000000;
				}
			}
			// mfg
			{

				ipInfo.mfg = 0;
				ipInfo.mfg = (ipInfo.mfg << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MFG_ID_CELL+2] & 0x0ff;
				ipInfo.mfg = (ipInfo.mfg << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MFG_ID_CELL+3] & 0x0ff;
				ipInfo.mfg = (ipInfo.mfg << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MFG_ID_CELL+1] & 0x0ff;
			}
			// model
			{
				ipInfo.model = 0;
				ipInfo.model = (ipInfo.model << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MODEL_NO_CELL+0] & 0x0ff;
				ipInfo.model = (ipInfo.model << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_MODEL_NO_CELL+1] & 0x0ff;
			}
			// revision
			{
				ipInfo.revision = 0;
				ipInfo.revision = (ipInfo.revision << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_REVISION_CELL+0];
				ipInfo.revision = (ipInfo.revision << 8) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_REVISION_CELL+1];
			}
			// software driver
			{
				ipInfo.driver = 0;
				ipInfo.driver = (ipInfo.driver << 8 ) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_SOFTWARE_HI_CELL+0] & 0x0ff;;
				ipInfo.driver = (ipInfo.driver << 8 ) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_2_SOFTWARE_HI_CELL+1] & 0x0ff;;
				ipInfo.driver = (ipInfo.driver << 8 ) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_SOFTWARE_LO_CELL+0] & 0x0ff;;
				ipInfo.driver = (ipInfo.driver << 8 ) | m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_SOFTWARE_LO_CELL+1] & 0x0ff;;
			}
			break;
		default:
			Log( LOG_ERRORS, "\tGetIpInformation(), Handle $%x, Unknow module type\n", ipHandle);
			break;
		}
	}
	else
	{
		Log( LOG_ERRORS, "\tGetIpInformation() Handle $%x not valid\n", ipHandle);
	}

	//
	Log( LOG_FN_ENTRY, "Exit Pci40::GetIpInformation()\n");
	return( retVal);
}

/**
 * Returns base IO address, of given IP module, local memory
 *
 * @param handle Handle of IP to get address for
 *
 * @return 0 if an error occurred; any other value is the base address
 *         of the IP module
 */
void* Pci40::GetIpBaseIOAddress( int ipHandle)
{
	unsigned	modIdx;
	void		*ipAddress=(void *)0;

	Log( LOG_FN_ENTRY, "Enter Pci40::GetIpBaseIOAddress()\n");

	// extract location information
	modIdx = ipHandle & 0x0ff;

	// if card and module installed
	if( ((uint32_t)-1 != m_pci40Addr) &&
		(0 < m_cardInfo.Ip[ modIdx].ipModule.installed) )
	{
		// uIoIndexAddress
		// uIdIndexAddress
		// uIntIndexAddress
		// uMemIndexAddress

		// get address
		ipAddress = (void *)m_cardInfo.Ip[ modIdx].ioIndexAddress;
	}
	else
	{
		Log( LOG_ERRORS, "\tGetIpBaseIOAddress() Handle $%x not valid\n", ipHandle);
	}

	//
	Log( LOG_FN_ENTRY, "Exit Pci40::GetIpBaseIOAddress()\n");
	return( ipAddress);
}

/**
 * Returns the PCI IRQ assigned to the carrier
 *
 * @return The PCI IRQ assigned to the carrier
 */
int Pci40::GetPicIrq() const
{
	return( m_pciIrq);
}

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
uint8_t Pci40::GetActiveIpIrqs() const
{
	uint8_t retVal = 0;
	const uint8_t *intScr = (const uint8_t*)m_cardInfo.ctrl1Address;

	if( intScr != NULL)
	{
		retVal = *intScr;
	}
	return( retVal);
}

/**
 * Local data initialization
 */
void Pci40::InitializePci40Data( void )
{
	int		ipIdx, charIdx;

	Log( LOG_FN_ENTRY, "Enter Pci40::InitializePci40Data()\n");

	m_pci40Found = false;

	m_pci40Addr = (uint32_t)-1;
	m_cardInfo.handle		= NULL;
	m_cardInfo.pci9080Addr	= NULL;
	m_cardInfo.pci40Addr	= NULL;
	m_cardInfo.ctrl0Address	= NULL;
	m_cardInfo.ctrl1Address	= NULL;
	m_cardInfo.ctrl2Address	= NULL;

	for( ipIdx=0; ipIdx<MAX_IP_MODULES; ipIdx++)
	{
		m_cardInfo.Ip[ ipIdx].ioIndexAddress = 0;
		m_cardInfo.Ip[ ipIdx].idIndexAddress = 0;
		m_cardInfo.Ip[ ipIdx].intIndexAddress = 0;
		m_cardInfo.Ip[ ipIdx].memIndexAddress = 0;

		// module
		{
			m_cardInfo.Ip[ ipIdx].ipModule.installed = 0;			// 1=Installed
			m_cardInfo.Ip[ ipIdx].ipModule.promDataFormat = 0;
			m_cardInfo.Ip[ ipIdx].ipModule.speed = 8000000;

			for( charIdx=0; charIdx<ID_STRING_MAX_LENGTH; charIdx++)
			{
				m_cardInfo.Ip[ ipIdx].ipModule.idString[ charIdx] = 0;
			}
		}
	}

	Log( LOG_FN_ENTRY, "Exit Pci40::InitializePci40Data()\n");
}

/**
 * Tries to allocate memory map region to pci40
 *
 * @return -1 if an error occurred or 0 if successful
 */
int Pci40::AllocatePci40Memory()
{
	int			retVal=-1;
	// memory allocation
	uint64_t	baseAddr;
	unsigned	modIdx;

	Log( LOG_FN_ENTRY, "Enter Pci40::AllocatePci40Memory( %d)\n", m_cardIdx);
	printf("Enter Pci40::AllocatePci40Memory( %d)\n", m_cardIdx);

	// if this card exists
	if( (uint32_t)-1 != m_pci40Addr)
	{
		// Get base address for this card
		if( 0 != (baseAddr = PCI_MEM_ADDR( m_pci40Addr)))
		{
			m_cardInfo.pci40Addr = mmap_device_memory( NULL, PCI_40_SIZE,
														 PROT_NOCACHE | PROT_READ | PROT_WRITE,
														 0, PCI_MEM_ADDR( m_pci40Addr));
			m_cardInfo.pci9080Addr = mmap_device_memory( NULL, PCI_40_SIZE,
														 PROT_NOCACHE | PROT_READ | PROT_WRITE,
														 0, PCI_MEM_ADDR( m_9080Addr));

			printf("\tpci9080Addr = %08X\n\tm_9080Addr = %08X\n\tpci40Addr = %08X\n\tm_pci40Addr = %08X\n", 
					m_cardInfo.pci9080Addr, m_9080Addr, m_cardInfo.pci40Addr, m_pci40Addr);
			if( MAP_FAILED == m_cardInfo.pci9080Addr)
			{
				printf("\t\tFailed to map pci9080Addr, %s - errno: %d, EINVAL: %d, len: %08X\n", strerror(errno), errno, EINVAL, PCI_40_SIZE);
			}
			
			if( MAP_FAILED != m_cardInfo.pci40Addr)
			{
				Log( LOG_DEV_DATA, "Card %d\n", m_cardIdx);
				Log( LOG_DEV_DATA, "\tBaseAddress: $%x\n", m_cardInfo.pci40Addr);
				printf("Card %d\n", m_cardIdx);
				printf("\tBaseAddress: $%x\n", m_cardInfo.pci40Addr);

				// map PCI-40 control registers
				m_cardInfo.ctrl0Address = (char*)m_cardInfo.pci40Addr + 0x500;
				m_cardInfo.ctrl1Address = (char*)m_cardInfo.pci40Addr + 0x600;
				m_cardInfo.ctrl2Address = (char*)m_cardInfo.pci40Addr + 0x700;

				Log( LOG_DEV_DATA, "\tCNTL0Address: $%x\n", m_cardInfo.ctrl0Address);
				Log( LOG_DEV_DATA, "\tCNTL1Address: $%x\n", m_cardInfo.ctrl1Address);
				Log( LOG_DEV_DATA, "\tCNTL2Address: $%x\n", m_cardInfo.ctrl2Address);
				printf("\tCNTL0Address: $%x\n", m_cardInfo.ctrl0Address);
				printf("\tCNTL1Address: $%x\n", m_cardInfo.ctrl1Address);
				printf("\tCNTL2Address: $%x\n", m_cardInfo.ctrl2Address);

				// map IP modules
				modIdx = 0;
				while( modIdx < MAX_IP_MODULES)
				{
					// address sections of IP module
					m_cardInfo.Ip[ modIdx].ioIndexAddress	= (unsigned)m_cardInfo.pci40Addr +    0x1000 + (  0x1000 * modIdx);
					m_cardInfo.Ip[ modIdx].idIndexAddress	= (unsigned)m_cardInfo.pci40Addr +    0x1100 + (  0x1000 * modIdx);
					m_cardInfo.Ip[ modIdx].intIndexAddress	= (unsigned)m_cardInfo.pci40Addr +    0x1200 + (  0x1000 * modIdx);
					m_cardInfo.Ip[ modIdx].memIndexAddress	= (unsigned)m_cardInfo.pci40Addr + 0x1000000 + (0x800000 * modIdx);

					Log( LOG_DEV_DATA, "IP %d\n", modIdx);
					Log( LOG_DEV_DATA, "\tIoIndexAddress:  $%X\n", m_cardInfo.Ip[ modIdx].ioIndexAddress);
					Log( LOG_DEV_DATA, "\tIdIndexAddress:  $%X\n", m_cardInfo.Ip[ modIdx].idIndexAddress);
					Log( LOG_DEV_DATA, "\tIntIndexAddress: $%X\n", m_cardInfo.Ip[ modIdx].intIndexAddress);
					Log( LOG_DEV_DATA, "\tMemIndexAddress: $%X\n", m_cardInfo.Ip[ modIdx].memIndexAddress);

					printf("IP %d\n", modIdx);
					printf("\tIoIndexAddress:  $%X\n", m_cardInfo.Ip[ modIdx].ioIndexAddress);
					printf("\tIdIndexAddress:  $%X\n", m_cardInfo.Ip[ modIdx].idIndexAddress);
					printf("\tIntIndexAddress: $%X\n", m_cardInfo.Ip[ modIdx].intIndexAddress);
					printf("\tMemIndexAddress: $%X\n", m_cardInfo.Ip[ modIdx].memIndexAddress);
					// next module
					modIdx++;
				}

				// good
				retVal = 0;
			}
			else
			{
				Log( LOG_ERRORS, "\tmmap_device_memory() failed : %s\n", strerror( errno));
				printf("\tmmap_device_memory() failed : %s\n", strerror( errno));
			}
		}
		else
		{
			Log( LOG_ERRORS, "\tWith PCI-40, Card %d, BAR %d, Address: $%x\n", m_cardIdx, PCI_40_BASE_ADDRESS_REG, baseAddr);
			printf("\tWith PCI-40, Card %d, BAR %d, Address: $%x\n", m_cardIdx, PCI_40_BASE_ADDRESS_REG, baseAddr);
		}
	}


	Log( LOG_FN_ENTRY, "Exit Pci40::AllocatePci40Memory( %d)\n", m_cardIdx);
	printf("Exit Pci40::AllocatePci40Memory( %d)\n", m_cardIdx);
	// return status
	return( retVal);
}

/**
 * Initialize PCI-40 Card for proper access
 *
 * @return -1 if an error occurred or 0 if successful
 */
int Pci40::InitializePci40Card()
{
	int		retVal=-1;
	uint8_t	*cntl0, *cntl1, *cntl2, junk=0;

	Log( LOG_FN_ENTRY, "Enter Pci40::InitializePci40Card( %d)\n", m_cardIdx);
	printf("Enter Pci40::InitializePci40Card( %d)\n", m_cardIdx);

	// verify this card exists
	if( (uint32_t)-1 != m_pci40Addr)
	{
		// Disable interrupts when initializing
		DisableIrq();

		// enable auto bus error acknowledge, else system will lock up on access to missing IP module
		cntl0 = (uint8_t *)m_cardInfo.ctrl0Address;
		cntl1 = (uint8_t *)m_cardInfo.ctrl1Address;
		cntl2 = (uint8_t *)m_cardInfo.ctrl2Address;

		Log( LOG_DEV_DATA, "Enable auto bus error acknowledge and interrupts\n");
		Log( "Control0 value: $%02hhX\n", *cntl0);
		*cntl0 = CNTL0_AUTO_ACK | CNTL0_INTEN;

		if( junk & 0x01)	Log( "\n\tIP SlotA Clock set to 32 MHz: $%02hhX\n", junk);
		else				Log( "\n\tIP SlotA Clock set to 8 MHz: $%02hhX\n", junk);

		junk = *cntl1;
		if( junk & 0x01)	Log( "\n\tIP SlotB Clock set to 32 MHz: $%02hhX\n", junk);
		else				Log( "\n\tIP SlotB Clock set to 8 MHz: $%02hhX\n", junk);

		junk = *cntl2;
		if( junk & 0x01)	Log( "\n\tIP SlotC Clock set to 32 MHz: $%02hhX\n", junk);
		else				Log( "\n\tIP SlotC Clock set to 8 MHz: $%02hhX\n", junk);


		// good
		retVal = 0;
	}
	else
	{
		Log( LOG_ERRORS, "\tInitializePci40Card() trying to initialize card %d\n", m_cardIdx);
	}

	Log( LOG_FN_ENTRY, "Exit Pci40::InitializePci40Card( %d)\n", m_cardIdx);
	// return status
	return( retVal);
}

/**
 * Identify IP modules installed on specified card
 *
 * @return -1 if an error occurred; any other value represents the number of
 *         IP modules found
 */
int Pci40::IdentifyIPModules()
{
	int				retVal=-1;
	// module
	int				modIdx;
	volatile char	*idString;
	volatile short	*idShorts;
	//
	int				nn;

	Log( LOG_FN_ENTRY, "Enter Pci40::IdentifyIPModules()\n");

	// verify this card exists
	if( (uint32_t)-1 != m_pci40Addr)
	{
		Log( LOG_DEV_DATA, "\tCard: %d\n", m_cardIdx);

		// no IP Modules found yet
		retVal = 0;

		// check all IP module locations
		for( modIdx=0; modIdx<MAX_IP_MODULES; modIdx++)
		{
			Log( LOG_DEV_DATA, "\tIP: %d\n", modIdx);

			// get pointer to module ID string
			idString = (char *)m_cardInfo.Ip[ modIdx].idIndexAddress;
			idShorts = (short *)idString;

			// get ID string
			{
				for( nn=0; nn<ID_STRING_MAX_LENGTH; nn++)
				{
					// Intel
					m_cardInfo.Ip[ modIdx].ipModule.idString[ nn] = idString[ nn ^ 1];
					// Motorola
					//m_cardInfo.Ip[ modIdx].ipModule.idString[ nn] = idString[ nn];
				}
			}

			// check for invalid characters as indicator of existance
			{
				// start assuming it is installed
				m_cardInfo.Ip[ modIdx].ipModule.installed = 1;
				// check all locations for a bad value
				for( nn=0; nn<ID_STRING_MAX_LENGTH; nn++)
				{
					// if a bad value detected
					if( (char)0xff == m_cardInfo.Ip[ modIdx].ipModule.idString[ nn])
					{
						// then it can not be installed
						m_cardInfo.Ip[ modIdx].ipModule.installed = 0;
					}
				}
			}

			// if this module is installed
			if( 1 == m_cardInfo.Ip[ modIdx].ipModule.installed)
			{
				// found an IP Module
				retVal++;


				if( ('I' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 1]) &&
					('P' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 3]) &&
					('A' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 5]) &&
					('C' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 7]) )
				{
					m_cardInfo.Ip[ modIdx].ipModule.promDataFormat = 1;
					m_cardInfo.Ip[ modIdx].ipModule.speed = 8;
					Log( LOG_DEV_DATA, "  Format 1  8 MHz\n");
				}
				else if( ('I' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 1]) &&
						 ('P' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 3]) &&
						 ('A' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 5]) &&
						 ('H' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 7]) )
				{
					m_cardInfo.Ip[ modIdx].ipModule.promDataFormat = 1;
					m_cardInfo.Ip[ modIdx].ipModule.speed = 32;
					Log( LOG_DEV_DATA, "  Format 1  32 MHz\n");
				}
				else if( ('V' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 0]) &&
						 ('I' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 1]) &&
						 ('T' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 2]) &&
						 ('A' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 3]) &&
						 ('4' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 4]) &&
						 (' ' == m_cardInfo.Ip[ modIdx].ipModule.idString[ 5]) )
				{
					m_cardInfo.Ip[ modIdx].ipModule.promDataFormat = 2;

					// the following bits may be wrong
					// supports 8 MHz
					if( 0x02 & m_cardInfo.Ip[ modIdx].ipModule.idString[ 0x14])
					{
						m_cardInfo.Ip[ modIdx].ipModule.speed = 8;
						Log( LOG_DEV_DATA, "  Format 2  8 MHz\n");
					}
					// supports 32 MHz
					if( 0x04 & m_cardInfo.Ip[ modIdx].ipModule.idString[ 0x14])
					{
						m_cardInfo.Ip[ modIdx].ipModule.speed = 32;
						Log( LOG_DEV_DATA, "  Format 2  32 MHz\n");
					}
				}
				else
				{
					Log( LOG_ERRORS, "\tUnknown IP Module Type.  Card %d.  Module %d\n", m_cardIdx, modIdx);
					for( nn=0; nn<((ID_STRING_MAX_LENGTH+3)&0xfc); nn+=4)
					{
						Log( LOG_DEV_DATA, "  $%02x  $%02x  $%02x  $%02x\n",
							   m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+0],
							   m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+1],
							   m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+2],
							   m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+3]);
					}
					for( nn=0; nn<((ID_STRING_MAX_LENGTH+3)&0xfc); nn+=2)
					{
						Log( LOG_DEV_DATA, "  $%04x  $%04x\n", idShorts[nn+0]&0x0ffff, idShorts[nn+1]&0x0ffff);
					}
					Log( LOG_DEV_DATA, "\n");
				}
			}

			// who is here
			{
				for( nn=0; nn<((ID_STRING_MAX_LENGTH+3)&0xfc); nn+=4)
				{
					Log( LOG_DEV_DATA, "  $%02x  $%02x  $%02x  $%02x\n",
						   m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+0],
						   m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+1],
						   m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+2],
						   m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+3]);
				}
			}
		}
	}
	else
	{
		Log( LOG_ERRORS, "\tIdentifyIPModules() trying to read card %d\n", m_cardIdx);
	}

	Log( LOG_FN_ENTRY, "Exit Pci40::IdentifyIPModules()\n");
	// return status
	return( retVal);
}

