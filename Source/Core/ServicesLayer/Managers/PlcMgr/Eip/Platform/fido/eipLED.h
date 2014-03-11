/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPDATATYPES.h
**
** COPYRIGHT (c) 2008 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Platform dependant datatypes so other libraries don't need to include
** the TCP/IP stack headers
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

#ifndef EIP_LED_H
#define EIP_LED_H

#define EIP_MODULE_LED_GREEN		0x2
#define EIP_MODULE_LED_RED			0x1
#define EIP_MODULE_LED_MASK			0x3


#define EIP_NETWORK_LED_GREEN		0x80
#define EIP_NETWORK_LED_RED			0x40
#define EIP_NETWORK_LED_MASK		0xC0



#endif /* #ifndef EIP_LED_H */
