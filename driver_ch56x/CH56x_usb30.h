/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_usb30.h
* Author             : WCH
* Version            : V1.1
* Date               : 2020/12/23
* Description        :
*******************************************************************************/
#ifndef USB30_CH56X_USB30_H_
#define USB30_CH56X_USB30_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CH56x_common.h"

/* Function declaration */
void TMR0_IRQHandler() __attribute__((interrupt("WCH-Interrupt-fast")));
void LINK_IRQHandler() __attribute__((interrupt("WCH-Interrupt-fast")));
void USBSS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));           //USB3.0 interrupt service

#ifdef __cplusplus
}
#endif

#endif /* USER_USB30_DESC_H_ */


