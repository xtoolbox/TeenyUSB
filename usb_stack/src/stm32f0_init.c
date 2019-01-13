/*
 * teeny usb - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2018 XToolBox  - admin@xtoolbox.org
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

//  STM32F0 usb hardware handler
#include "teeny_usb.h"

//#define IMR_MSK (USB_CNTR_CTRM  | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_ERRM  | USB_CNTR_SOFM | USB_CNTR_ESOFM | USB_CNTR_RESETM)
#define IMR_MSK   ((USB_CNTR_CTRM  | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM))

void tusb_close_device(tusb_device_t* dev)
{
	// Disable USB IRQ
	NVIC_DisableIRQ(USB_IRQn);
	GetUSB(dev)->ISTR = (0);

	// turn off USB Macrocell
	GetUSB(dev)->CNTR = (USB_CNTR_PDWN | USB_CNTR_FRES);

#ifdef INTERNAL_PULLUP
  // disable interal pull up resistor
  GetUSB(dev)->BCDR&=~USB_BCDR_DPPU;
#else
  // PA12 = PushPull = 0
  GPIOA->OTYPER |= GPIO_OTYPER_OT_12;
  GPIOA->MODER &= ~GPIO_MODER_MODER12;
  GPIOA->MODER |= GPIO_MODER_MODER12_0;
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR12;
	GPIOA->BRR = GPIO_BRR_BR_12;
 #endif
 	// close USB clock
	RCC->APB1ENR &= ~RCC_APB1ENR_USBEN;
}

void tusb_open_device(tusb_device_t* dev)
{
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;
  
#if defined USB_CLOCK_SOURCE_CRS
  RCC->CFGR3 &= ~RCC_CFGR3_USBSW;
  RCC->APB1ENR |= RCC_APB1ENR_CRSEN;
  CRS->CFGR &= ~CRS_CFGR_SYNCSRC;
  CRS->CFGR |= RCC_CRS_SYNC_SOURCE_USB;
  CRS->CR |= (CRS_CR_AUTOTRIMEN | CRS_CR_CEN);
#else
  // otherwise use pll clk
  RCC->CFGR3 |= RCC_CFGR3_USBSW_PLLCLK;
#endif
  
	GetUSB(dev)->CNTR = (USB_CNTR_FRES);
	GetUSB(dev)->CNTR = (0);

  // wait reset finish
	while (!((GetUSB(dev)->ISTR) & USB_ISTR_RESET));
  
	GetUSB(dev)->ISTR = (0);
  GetUSB(dev)->BTABLE = (BTABLE_ADDRESS);
	GetUSB(dev)->CNTR = (IMR_MSK);
  NVIC_EnableIRQ(USB_IRQn);
  
#ifdef INTERNAL_PULLUP
  // USE the interal pull up resistor
  GetUSB(dev)->BCDR|=USB_BCDR_DPPU;
#else
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// PA12 = In float
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR12;
  GPIOA->MODER &= ~GPIO_MODER_MODER12;
#endif
}

static void tsub_suspend(tusb_device_t* dev)
{
  uint16_t wCNTR;

  wCNTR = GetUSB(dev)->CNTR;
  wCNTR |= USB_CNTR_FSUSP;
  GetUSB(dev)->CNTR = (wCNTR);
  
  wCNTR = GetUSB(dev)->CNTR;
  wCNTR |= USB_CNTR_LPMODE;
  GetUSB(dev)->CNTR = (wCNTR);
}

void tsub_resume(tusb_device_t* dev)
{
  uint16_t wCNTR;
  
  wCNTR = GetUSB(dev)->CNTR;
  wCNTR &= (~USB_CNTR_LPMODE);
  GetUSB(dev)->CNTR = (wCNTR);    
  GetUSB(dev)->CNTR = (IMR_MSK);

}


static tusb_device_t tusb_dev;
// end point data handler, also called in USB_HP IRQ
void tusb_ep_handler(tusb_device_t* dev, uint8_t EPn);

void USB_IRQHandler(void)
{
  tusb_device_t* dev = &tusb_dev;
  uint16_t wIstr;
  while ((wIstr = GetUSB(dev)->ISTR ) & USB_ISTR_CTR){
     GetUSB(dev)->ISTR = (uint16_t)(USB_CLR_CTR);
     tusb_ep_handler(dev, wIstr & USB_ISTR_EP_ID);
  }
  
	if (wIstr & USB_ISTR_RESET) {
		GetUSB(dev)->ISTR = (USB_CLR_RESET);
    GetUSB(dev)->BTABLE = (BTABLE_ADDRESS);
    tusb_reconfig(&tusb_dev);
    GetUSB(dev)->DADDR = (0 | USB_DADDR_EF);
	}
if( IMR_MSK & USB_ISTR_PMAOVR){
	if (wIstr & USB_ISTR_PMAOVR) {
		GetUSB(dev)->ISTR = (USB_CLR_PMAOVR);
	}
}
if( IMR_MSK & USB_ISTR_SUSP){
	if (wIstr & USB_ISTR_SUSP) {
		GetUSB(dev)->ISTR = (USB_CLR_SUSP);
		if (GetUSB(dev)->DADDR & 0x007f) {
			GetUSB(dev)->DADDR = (0);
			GetUSB(dev)->CNTR &= ~USB_CNTR_SUSPM;
		}
    tsub_suspend(dev);
	}
}
if( IMR_MSK & USB_ISTR_ERR){
	if (wIstr & USB_ISTR_ERR) {
		GetUSB(dev)->ISTR = (USB_CLR_ERR);
	}
}
  
if( IMR_MSK & USB_ISTR_WKUP){
	if (wIstr & USB_ISTR_WKUP) {
		GetUSB(dev)->ISTR = (USB_CLR_WKUP);
    tsub_resume(dev);
	}
}

if( IMR_MSK & USB_ISTR_SOF){
	if (wIstr & USB_ISTR_SOF) {
		GetUSB(dev)->ISTR = (USB_CLR_SOF);
	}
}
if( IMR_MSK & USB_ISTR_ESOF){
	if (wIstr & USB_ISTR_ESOF) {
		GetUSB(dev)->ISTR = (USB_CLR_ESOF);
	}
}

	GetUSB(dev)->ISTR = (0);
}

tusb_device_t* tusb_get_device(uint8_t id)
{
  return &tusb_dev;
}
