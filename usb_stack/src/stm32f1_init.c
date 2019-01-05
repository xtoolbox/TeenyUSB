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

//  STM32F1 usb hardware handler
#include "teeny_usb.h"

//#define IMR_MSK (USB_CNTR_CTRM  | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_ERRM  | USB_CNTR_SOFM | USB_CNTR_ESOFM | USB_CNTR_RESETM)
#define IMR_MSK   ((USB_CNTR_CTRM  | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM))

#define  USB_CLR_DIR      ((uint16_t)~USB_ISTR_DIR   )
#define  USB_CLR_ESOF     ((uint16_t)~USB_ISTR_ESOF  )
#define  USB_CLR_SOF      ((uint16_t)~USB_ISTR_SOF   )
#define  USB_CLR_RESET    ((uint16_t)~USB_ISTR_RESET )
#define  USB_CLR_SUSP     ((uint16_t)~USB_ISTR_SUSP  )
#define  USB_CLR_WKUP     ((uint16_t)~USB_ISTR_WKUP  )
#define  USB_CLR_ERR      ((uint16_t)~USB_ISTR_ERR   )
#define  USB_CLR_PMAOVR   ((uint16_t)~USB_ISTR_PMAOVR)
#define  USB_CLR_CTR      ((uint16_t)~USB_ISTR_CTR   )



void tusb_close_device(tusb_device_t* dev)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
#if defined(HAS_DOUBLE_BUFFER)
  NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
#endif
  NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  GetUSB()->ISTR = (0);
  // Turn USB Macrocell off (FRES + PWDN)
  GetUSB()->CNTR = (USB_CNTR_FRES | USB_CNTR_PDWN);
  
  // PA_12 output mode: OD = 0
  GPIOA->CRH |= GPIO_CRH_CNF12_0;
  GPIOA->CRH &= (~GPIO_CRH_CNF12_1);
  GPIOA->CRH |= GPIO_CRH_MODE12;// PA_12 set as: Output mode, max speed 50 MHz.
  GPIOA->BRR = GPIO_BRR_BR12;

  // Disable USB Clock on APB1
  RCC->APB1ENR &=  ~RCC_APB1ENR_USBEN;
}

void tusb_open_device(tusb_device_t* dev)
{
  // PA12 = Input
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  GPIOA->CRH |= GPIO_CRH_CNF12_0;
  GPIOA->CRH &= ~GPIO_CRH_CNF12_1;
  GPIOA->CRH &= ~GPIO_CRH_MODE12;  
  
  RCC->APB1ENR |= RCC_APB1ENR_USBEN;
  
  GetUSB(dev)->CNTR = (USB_CNTR_FRES);
  GetUSB(dev)->CNTR = (0);

  // wait reset finish
  while (!((GetUSB(dev)->ISTR) & USB_ISTR_RESET));
  
  GetUSB(dev)->ISTR = (0);
  GetUSB(dev)->BTABLE = (BTABLE_ADDRESS);
  GetUSB(dev)->CNTR = (IMR_MSK);
#if defined(HAS_DOUBLE_BUFFER)
  NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
#endif
  NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

static void tsub_suspend(tusb_device_t* dev)
{
  uint16_t wCNTR;

  wCNTR = GetUSB(dev)->CNTR;
  wCNTR |= USB_CNTR_FSUSP;
  GetUSB(dev)->CNTR = (wCNTR);
  
  wCNTR = GetUSB(dev)->CNTR;
  wCNTR |= USB_CNTR_LP_MODE;
  GetUSB(dev)->CNTR = (wCNTR);
}

void tsub_resume(tusb_device_t* dev)
{
  uint16_t wCNTR;
  
  wCNTR = GetUSB(dev)->CNTR;
  wCNTR &= (~USB_CNTR_LP_MODE);
  GetUSB(dev)->CNTR = (wCNTR);    
  GetUSB(dev)->CNTR = (IMR_MSK);

}


extern tusb_device_t tusb_dev;
// end point data handler, also called in USB_HP IRQ
void tusb_ep_handler(tusb_device_t* dev, uint8_t EPn);

void USB_HP_CAN1_TX_IRQHandler(void)
{
  uint16_t wIstr;
  tusb_device_t* dev = &tusb_dev;
  while ((wIstr = GetUSB(dev)->ISTR ) & USB_ISTR_CTR){
     GetUSB(dev)->ISTR = (uint16_t)(USB_CLR_CTR);
     tusb_ep_handler(dev, wIstr & USB_ISTR_EP_ID);
  }
}

void USB_LP_CAN1_RX0_IRQHandler(void)
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
    tusb_reconfig(dev);
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





