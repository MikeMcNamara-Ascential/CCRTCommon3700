//******************************************************************************
//
// $Archive: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tools/TOOLS/IpIcmTester/IpIcmTester.cpp $
// $Author: Derickso $
//   $Date: 11/02/07 8:08a $
//
// Description:
//    Utility used to test an IP ICM hardware installation. To build
//		simply type 'make IpIcmTester' at the command line.
//		Must be run as root
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Tools/TOOLS/IpIcmTester/IpIcmTester.cpp $
// 
// 1     11/02/07 8:08a Derickso
// Test report generator tool
// 
// 2     8/17/05 8:34p Cward
// fixed format to remove compilation warning.
//
// 1     7/20/05 8:13p Cward
// Directory structure change
//
// 2     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 1     12/21/03 6:32p Cward
//
// 1     8/25/03 3:02p Bmeinke
// Initial checkin
//
//
//******************************************************************************


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include <sys/mman.h>
#include <hw/pci.h>

#include "Pci40Defs.h"

/**
 * Initialize PCI-40 Card for proper access
 *
 * @return -1 if an error occurred or 0 if successful
 */
int InitializePci40Card( Pci40_t &pci40Card)
{
	int		modIdx, retVal=-1;
	uint8_t	*cntl0, *cntl1, *cntl2;

	// verify this card exists
	if( (uint32_t)-1 != pci40Card.m_pci40Addr)
	{
		// enable auto bus error acknowledge, else system will lock up on access to missing IP module
		cntl0 = (uint8_t *)pci40Card.m_cardInfo.ctrl0Address;
		cntl1 = (uint8_t *)pci40Card.m_cardInfo.ctrl1Address;
		cntl2 = (uint8_t *)pci40Card.m_cardInfo.ctrl2Address;

		//printf( "Enable auto bus error acknowledge and interrupts\n");
		//printf( "Control0 value: $%02hhX\n", *cntl0);
		*cntl0 = CNTL0_AUTO_ACK | CNTL0_INTEN;

		for( modIdx=0; modIdx<MAX_IP_MODULES; modIdx++)
		{
			pci40Card.m_cardInfo.Ip[ modIdx].ipModule.installed = 0;
		}

		// good
		retVal = 0;
	}
	else
	{
		printf( "\tInitializePci40Card() trying to initialize card %d\n", pci40Card.m_cardIdx);
	}

	// return status
	return( retVal);
}

/**
 * Tries to allocate memory map region to pci40
 *
 * @return -1 if an error occurred or 0 if successful
 */
int AllocatePci40Memory( Pci40_t &pci40Card)
{
	int			retVal=-1;
	// memory allocation
	uint64_t	baseAddr;
	unsigned	modIdx;

	// if this card exists
	if( (uint32_t)-1 != pci40Card.m_pci40Addr)
	{
		// Get base address for this card
		if( 0 != (baseAddr = PCI_MEM_ADDR( pci40Card.m_pci40Addr)))
		{
			pci40Card.m_cardInfo.pci40Addr = mmap_device_memory( NULL, PCI_40_SIZE,
																 PROT_NOCACHE | PROT_READ | PROT_WRITE,
																 0, PCI_MEM_ADDR( pci40Card.m_pci40Addr));

			pci40Card.m_cardInfo.pci9080Addr = mmap_device_memory( NULL, PCI_40_SIZE,
																   PROT_NOCACHE | PROT_READ | PROT_WRITE,
																   0, PCI_MEM_ADDR( pci40Card.m_9080Addr));

			if( MAP_FAILED != pci40Card.m_cardInfo.pci40Addr)
			{
				// map PCI-40 control registers
				pci40Card.m_cardInfo.ctrl0Address = (char*)pci40Card.m_cardInfo.pci40Addr + 0x500;
				pci40Card.m_cardInfo.ctrl1Address = (char*)pci40Card.m_cardInfo.pci40Addr + 0x600;
				pci40Card.m_cardInfo.ctrl2Address = (char*)pci40Card.m_cardInfo.pci40Addr + 0x700;

				// map IP modules
				modIdx = 0;
				while( modIdx < MAX_IP_MODULES)
				{
					// address sections of IP module
					pci40Card.m_cardInfo.Ip[ modIdx].ioIndexAddress		= (unsigned)pci40Card.m_cardInfo.pci40Addr +    0x1000 + (  0x1000 * modIdx);
					pci40Card.m_cardInfo.Ip[ modIdx].idIndexAddress		= (unsigned)pci40Card.m_cardInfo.pci40Addr +    0x1100 + (  0x1000 * modIdx);
					pci40Card.m_cardInfo.Ip[ modIdx].intIndexAddress	= (unsigned)pci40Card.m_cardInfo.pci40Addr +    0x1200 + (  0x1000 * modIdx);
					pci40Card.m_cardInfo.Ip[ modIdx].memIndexAddress	= (unsigned)pci40Card.m_cardInfo.pci40Addr + 0x1000000 + (0x800000 * modIdx);

					// next module
					modIdx++;
				}

				// good
				retVal = 0;
			}
			else
			{
				perror( "mmap_device_memory() failed");
			}
		}
		else
		{
			printf( "With PCI-40, Card %d, BAR %d, Address: $%llx\n", pci40Card.m_cardIdx, PCI_40_BASE_ADDRESS_REG, baseAddr);
		}
	}

	// return status
	return( retVal);
}

/**
 * De-allocates memory map regions used to access a pci40 card
 *
 * @return -1 if an error occurred or 0 if successful
 */
void UnmapPCI40( Pci40_t &pci40Card)
{
	if(pci40Card.m_cardInfo.pci40Addr != MAP_FAILED)	munmap( pci40Card.m_cardInfo.pci40Addr, PCI_40_SIZE);
	if(pci40Card.m_cardInfo.pci9080Addr != MAP_FAILED)	munmap( pci40Card.m_cardInfo.pci9080Addr, PCI_40_SIZE);
}

/**
 * Identify IP modules installed on specified card
 *
 * @return -1 if an error occurred; any other value represents the number of
 *         IP modules found
 */
int IdentifyIPModules( Pci40_t &pci40Card)
{
	int				retVal=-1;
	// module
	int				modIdx;
	volatile char	*idString;
	volatile short	*idShorts;
	//
	int				nn;

	// verify this card exists
	if( (uint32_t)-1 != pci40Card.m_pci40Addr)
	{
		//printf( "\tCard: %d\n", pci40Card.m_cardIdx);

		// no IP Modules found yet
		retVal = 0;

		// check all IP module locations
		for( modIdx=0; modIdx<MAX_IP_MODULES; modIdx++)
		{
			// get pointer to module ID string
			idString = (char *)pci40Card.m_cardInfo.Ip[ modIdx].idIndexAddress;
			idShorts = (short *)idString;

			// get ID string
			{
				for( nn=0; nn<ID_STRING_MAX_LENGTH; nn++)
				{
					// Intel
					pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ nn] = idString[ nn ^ 1];
					// Motorola
					//m_cardInfo.Ip[ modIdx].ipModule.idString[ nn] = idString[ nn];
				}
			}

			// check for invalid characters as indicator of existance
			{
				// start assuming it is installed
				pci40Card.m_cardInfo.Ip[ modIdx].ipModule.installed = 1;
				// check all locations for a bad value
				for( nn=0; nn<ID_STRING_MAX_LENGTH; nn++)
				{
					// if a bad value detected
					if( (char)0xff == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ nn])
					{
						// then it can not be installed
						pci40Card.m_cardInfo.Ip[ modIdx].ipModule.installed = 0;
					}
				}
			}

			// if this module is installed
			if( 1 == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.installed)
			{
				printf( "\nIP: %d (Present)\n", modIdx);

				// found an IP Module
				retVal++;


				if( ('I' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 1]) &&
					('P' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 3]) &&
					('A' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 5]) &&
					('C' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 7]) )
				{
					pci40Card.m_cardInfo.Ip[ modIdx].ipModule.promDataFormat = 1;
					pci40Card.m_cardInfo.Ip[ modIdx].ipModule.speed = 8;
					printf( "  Format 1  8 MHz\n");
				}
				else if( ('I' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 1]) &&
						 ('P' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 3]) &&
						 ('A' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 5]) &&
						 ('H' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 7]) )
				{
					pci40Card.m_cardInfo.Ip[ modIdx].ipModule.promDataFormat = 1;
					pci40Card.m_cardInfo.Ip[ modIdx].ipModule.speed = 32;
					printf( "  Format 1  32 MHz\n");
				}
				else if( ('V' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 0]) &&
						 ('I' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 1]) &&
						 ('T' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 2]) &&
						 ('A' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 3]) &&
						 ('4' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 4]) &&
						 (' ' == pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 5]) )
				{
					pci40Card.m_cardInfo.Ip[ modIdx].ipModule.promDataFormat = 2;

					// the following bits may be wrong
					// supports 8 MHz
					if( 0x02 & pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 0x14])
					{
						pci40Card.m_cardInfo.Ip[ modIdx].ipModule.speed = 8;
						printf( "  Format 2  8 MHz\n");
					}
					// supports 32 MHz
					if( 0x04 & pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ 0x14])
					{
						pci40Card.m_cardInfo.Ip[ modIdx].ipModule.speed = 32;
						printf( "  Format 2  32 MHz\n");
					}
				}
				else
				{
					printf( "\tUnknown IP Module Type.  Card %d.  Module %d\n", pci40Card.m_cardIdx, modIdx);
					for( nn=0; nn<((ID_STRING_MAX_LENGTH+3)&0xfc); nn+=4)
					{
						printf( "  $%02x  $%02x  $%02x  $%02x\n",
							   pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+0],
							   pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+1],
							   pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+2],
							   pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ nn+3]);
					}
					for( nn=0; nn<((ID_STRING_MAX_LENGTH+3)&0xfc); nn+=2)
					{
						printf( "  $%04x  $%04x\n", idShorts[nn+0]&0x0ffff, idShorts[nn+1]&0x0ffff);
					}
					printf( "\n");
				}

				unsigned driver = pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_SOFTWARE_HI_CELL] & 0x0ff;
				driver = (driver << 8) | pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_SOFTWARE_LO_CELL] & 0x0ff;

				printf( "\tManufacturer: $%04hX\n", pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_MFG_ID_CELL] & 0x0ff);
				printf( "\t       Model: $%04hX\n", pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_MODEL_NO_CELL] & 0x0ff);
				printf( "\t    Revision: $%04hX\n", pci40Card.m_cardInfo.Ip[ modIdx].ipModule.idString[ TYPE_1_REVISION_CELL] & 0x0ff);
				printf( "\t      Driver: $%04hX\n", driver);

			}
			else
			{
				printf( "\nIP: %d (Not present)\n", modIdx);
			}
		}
	}
	else
	{
		printf( "\tIdentifyIPModules() trying to read card %d\n", pci40Card.m_cardIdx);
	}

	// return status
	return( retVal);
}

int FindPci40( Pci40_t &pci40Card, int cardIdx)
{
	int			ipCount=0;
	uint32_t	pciIrq;
	int 		pciHandle = -1;

	if( (pciHandle = pci_attach(0)) != -1)
	{
		if( pci_find_device( PCI_40_DEVICE_ID, PCI_40_VENDOR_ID, cardIdx, &pci40Card.m_busNo, &pci40Card.m_devFuncNo) == PCI_SUCCESS)
		{
			pci40Card.m_9080Addr =  (uint32_t)-1;
			pci40Card.m_pci40Addr = (uint32_t)-1;
			pci40Card.m_cardIdx = cardIdx;

			/************************************************************************************
			Read chip PCI address:
				The 1st BAR (0x10) is the PCI memory address of the PCI-9080
				The 2nd BAR (0x14) is the PCI I/O address of the PCI-9080 I/O
				The 3rd BAR (0x18) is the PCI memory address of the PCI40 and IP modules
			************************************************************************************/
			pci_read_config32( pci40Card.m_busNo, pci40Card.m_devFuncNo, 0x10, 1, &pci40Card.m_9080Addr);
			pci_read_config32( pci40Card.m_busNo, pci40Card.m_devFuncNo, 0x18, 1, &pci40Card.m_pci40Addr);
			pci_read_config32( pci40Card.m_busNo, pci40Card.m_devFuncNo, 0x3C, 1, &pciIrq);

			printf( "PCI40 found; Bus %d devFuncNo %d Address $%08X IRQ %d\n", pci40Card.m_busNo, pci40Card.m_devFuncNo, pci40Card.m_pci40Addr, pciIrq);
			// get access to this card
			if( 0 == AllocatePci40Memory( pci40Card))
			{
				// found at least one PCI-40
				pci40Card.m_pci40Found = true;
				printf( "PCI-40 Count = %d\n", pci40Card.m_pci40Found);

				// init this thing
				if( 0 == InitializePci40Card( pci40Card))
				{
					// identify IP modules installed on this card
					if( 0 <= (ipCount = IdentifyIPModules( pci40Card)))
					{
						printf( "\nPCI-40 Card %d has %d IP module(s)\n", pci40Card.m_cardIdx, ipCount);
					}
					else
					{
						printf( "\nPCI-40 Card %d has NO IP modules\n", pci40Card.m_cardIdx);
					}
				}
				else
				{
					printf( "\tInitializePci40Card( %d) failed\n", pci40Card.m_cardIdx);
				}

				UnmapPCI40( pci40Card);
			}
			else
			{
				printf( "\tAllocatePci40Memory( %d) failed\n", pci40Card.m_cardIdx);
			}
		}

		pci_detach( pciHandle);
	}
	else
	{
		perror( "Error attaching to PCI manager");
	}

	return( ipCount);
}


int main( int argc, char *argv[])
{
	Pci40_t	pci40Card;
	int		cardIdx=0;

	while( true)
	{
		if( FindPci40(pci40Card,cardIdx) == 0)
		{
			if( cardIdx == 0)	printf( "No IP carriers found\n");
			break;
		}
		cardIdx++;
	}
	printf( "\n");

	return( 0);
}

