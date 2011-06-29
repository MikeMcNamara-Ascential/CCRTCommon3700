//****************************************************************************
//
// FILE:
//    $Header: /Core/ServicesLayer/Include/Message.h 1     12/21/03 6:27p Cward $
//
// DESCRIPTION:
//
//    Constants and other definitions required for the SetupData class, which
//    are not directly related to the structure of the class definition.
//
//============================================================================
//
//    Copyright (c) 1999 Burke E. Porter Machinery
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//
// LOG:
//    $Log: /Core/ServicesLayer/Include/Message.h $
// 
// 1     12/21/03 6:27p Cward
// 
// 1     10/07/03 11:37a Khykin
// Initial checkin
// 
// 1     7/08/03 1:03p Derickso
// Converted to new directory structure.
// 
// 1     7/03/03 5:27p Khykin
// Initial checkin
// 
// 1     6/12/03 4:17p Khykin
// Initial checkin.
// 
// 1     4/22/03 9:44a Khykin
// Kevins Working Checkin
// 
// 2     4/07/03 9:27a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:05:43  tbochene
//
//    PooBag
//
// 
// 1     1/27/03 7:07a Khykin
// Initial Checkin
// 
// 1     1/18/03 8:05a Khykin
// Initial Checkin From DCTC
// 
// 1     4/10/01 10:46a Admin
//
// 2     3/05/01 8:11a Tim.bochenek
// Still testing
//
// 4     10/19/99 6:11p Hil.sybesma
//
//	10/12/99 14:15 hilbrand.sybesma
//		- Released
//
//****************************************************************************

#ifndef 	COMMAND_MESSAGE_H
#define		COMMAND_MESSAGE_H


// ***********************************************************************
// ************************** system configuration ***********************
// ***********************************************************************



// ***********************************************************************
// ************************** public attributes **************************
// ***********************************************************************

#define		COMMAND_BASE				0x00200001

// IPC Messages
#define		CMD_NULL					(COMMAND_BASE + 0)	// nada
#define		CMD_ACK						(COMMAND_BASE + 1)	// general acknowledge
#define		CMD_NAK						(COMMAND_BASE + 2)	// general bummer
#define		CMD_TERMINATE				(COMMAND_BASE + 3)	// jump off a cliff
#define		CMD_VERBOSE_LEVEL			(COMMAND_BASE + 4)	// tell me more
#define		CMD_DIAG_STRING				(COMMAND_BASE + 5)	// diagnostic ditto message



// ************************************************************************

#endif	// COMMAND_MESSAGE_H
// END

