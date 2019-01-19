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

//  STM32F7 usb hardware handler
#include "teeny_usb.h"

// #define  ENABLE_DMA
// #define  ENABLE_VBUS_DETECT 

// FS core always work in full speed
#ifndef   OTG_FS_SPEED
#define   OTG_FS_SPEED      USB_OTG_SPEED_FULL
#endif

// HS core always work in high speed
#ifndef   OTG_HS_SPEED
#define   OTG_HS_SPEED      USB_OTG_SPEED_HIGH
#endif

//#define   OTG_HS_SPEED      USB_OTG_SPEED_HIGH_IN_FULL

#if defined(OTG_HS_EMBEDDED_PHY) && defined(OTG_HS_EXTERNAL_PHY)
#error Embedded and external HS phy can not use together
#endif

void flush_rx(USB_OTG_GlobalTypeDef *USBx)
{  
  USBx->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) == USB_OTG_GRSTCTL_RXFFLSH);
}

void flush_tx(USB_OTG_GlobalTypeDef *USBx, uint32_t num)
{
  USBx->GRSTCTL = ( USB_OTG_GRSTCTL_TXFFLSH |(uint32_t)( num << 6));
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) == USB_OTG_GRSTCTL_TXFFLSH);
}

static void Wait_CoreReset(USB_OTG_GlobalTypeDef *USBx)
{
  /* Wait for AHB master IDLE state. */
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0);
  /* Core Soft Reset */
  USBx->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);
}

#if defined(USB_HS_PHYC)
void delay_ms(uint32_t ms);
static void USB_HS_PHYCInit(USB_OTG_GlobalTypeDef *USBx)
{
  /* Enable LDO */
  USB_HS_PHYC->USB_HS_PHYC_LDO |= USB_HS_PHYC_LDO_ENABLE;
  /* wait for LDO Ready */
  while((USB_HS_PHYC->USB_HS_PHYC_LDO & USB_HS_PHYC_LDO_STATUS) == RESET);

  /* Controls PHY frequency operation selection */
  if (HSE_VALUE == 12000000){       /* HSE = 12MHz */
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x0 << 1);
  }else if (HSE_VALUE == 12500000){ /* HSE = 12.5MHz */
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x2 << 1);
  }else if (HSE_VALUE == 16000000){ /* HSE = 16MHz */
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x3 << 1);
  }else if (HSE_VALUE == 24000000){ /* HSE = 24MHz */
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x4 << 1);
  }else if (HSE_VALUE == 25000000){ /* HSE = 25MHz */
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x5 << 1);
  }else if (HSE_VALUE == 32000000){ /* HSE = 32MHz */
    USB_HS_PHYC->USB_HS_PHYC_PLL = (uint32_t)(0x7 << 1);
  }
  /* Control the tuning interface of the High Speed PHY */
  USB_HS_PHYC->USB_HS_PHYC_TUNE |= USB_HS_PHYC_TUNE_VALUE;
  /* Enable PLL internal PHY */
  USB_HS_PHYC->USB_HS_PHYC_PLL |= USB_HS_PHYC_PLL_PLLEN;
  /* 2ms Delay required to get internal phy clock stable */
  delay_ms(20);
}
#endif


void tusb_close_core(tusb_core_t* core)
{
  USB_OTG_GlobalTypeDef* USBx = GetUSB(core);
  USBx->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
  
  uint32_t i;
  /* Clear Pending interrupt */
  for (i = 0; i < EP_NUM ; i++){
    USBx_INEP(i)->DIEPINT  = 0xFF;
    USBx_OUTEP(i)->DOEPINT  = 0xFF;
  }
  USBx_DEVICE->DAINT = 0xFFFFFFFF;
  /* Clear interrupt masks */
  USBx_DEVICE->DIEPMSK  = 0;
  USBx_DEVICE->DOEPMSK  = 0;
  USBx_DEVICE->DAINTMSK = 0;

  /* Flush the FIFO */
  flush_rx(USBx);
  flush_tx(USBx ,  0x10 );
  
  // disconnect
  USBx_DEVICE->DCTL |= USB_OTG_DCTL_SDIS ;
  
  if(USBx == USB_OTG_FS){
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
    NVIC_DisableIRQ(OTG_FS_IRQn);
    // maybe need de-init the IOs
  }else if(USBx == USB_OTG_HS){
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
    NVIC_DisableIRQ(OTG_HS_IRQn);
  }
}

void tusb_close_device(tusb_device_t* dev)
{
  tusb_close_core((tusb_core_t*) dev);
}

void tusb_close_host(tusb_host_t* host)
{
  tusb_close_core((tusb_core_t*) host);
}

#define AFL(val, pin)   ((val)<< (( (pin))*4))
#define AFH(val, pin)   ((val)<< (( (pin)-8)*4))
static void set_io_af(GPIO_TypeDef* GPIO, uint8_t pin, uint8_t af)
{
  GPIOA->MODER &= ~(GPIO_MODER_MODER0 << (pin*2));
  GPIOA->MODER |= (GPIO_MODER_MODER0_1 << (pin*2));
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_0 << pin);
  GPIOA->OSPEEDR |= ( (GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR0_1)  << (pin*2) );
  if(pin > 7){
    GPIOA->AFR[1] &= ~( AFH(0xf,pin));
    GPIOA->AFR[1] |= ( AFH(af,pin));
  }else{
    GPIOA->AFR[0] &= ~( AFL(0xf,pin));
    GPIOA->AFR[0] |= ( AFL(af,pin));
  }
}


// Setup IO for OTG_FS core
static void tusb_setup_otg_fs_io(void)
{
  /**USB_OTG_FS GPIO Configuration    
  PA8     ------> USB_OTG_FS_SOF
  PA9     ------> USB_OTG_FS_VBUS
  PA10     ------> USB_OTG_FS_ID
  PA11     ------> USB_OTG_FS_DM
  PA12     ------> USB_OTG_FS_DP 
  */
  __HAL_RCC_GPIOA_CLK_ENABLE();
/*
  GPIOA->MODER &= ~(GPIO_MODER_MODER11 | GPIO_MODER_MODER12);
  GPIOA->MODER |= (GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1);
  // OType = PushPull
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_12);
  // OSpeed = Very High
  GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR11_0 | GPIO_OSPEEDER_OSPEEDR11_1
                   | GPIO_OSPEEDER_OSPEEDR12_0 | GPIO_OSPEEDER_OSPEEDR12_1);
  // AF = OTG_FS
  GPIOA->AFR[1] &= ~( AFH(0xf,11) | AFH(0xf,12) );
  GPIOA->AFR[1] |= ( AFH(GPIO_AF10_OTG_FS,11) | AFH(GPIO_AF10_OTG_FS,12) );
  */
  set_io_af(GPIOA, 11, GPIO_AF10_OTG_FS);
  set_io_af(GPIOA, 12, GPIO_AF10_OTG_FS);
#if defined(ENABLE_VBUS_DETECT)
  // Mode = Input
  GPIOA->MODER &= ~(GPIO_MODER_MODER9);
  // No pull
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR9_1);    
#endif
}

#if defined(USB_HS_PHYC)
// Setup IO for OTG_HS core with embedded HS phy
static void tusb_setup_otg_hs_io(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();
  set_io_af(GPIOB, 14, GPIO_AF12_OTG_HS_FS);
  set_io_af(GPIOB, 15, GPIO_AF12_OTG_HS_FS);
#if defined(ENABLE_VBUS_DETECT)
  // Mode = Input
  GPIOB->MODER &= ~(GPIO_MODER_MODER13);
  // No pull
  GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR13_0 | GPIO_PUPDR_PUPDR9_13);
#endif
}
#endif

#if defined(OTG_HS_EXTERNAL_PHY)
// Setup IO for ULPI interface phy
static void tusb_setup_otg_ulpi_io(void)
{
  /**USB_OTG_HS GPIO Configuration    
    PC0     ------> USB_OTG_HS_ULPI_STP
    PC2     ------> USB_OTG_HS_ULPI_DIR
    PC3     ------> USB_OTG_HS_ULPI_NXT
    PA3     ------> USB_OTG_HS_ULPI_D0
    PA5     ------> USB_OTG_HS_ULPI_CK
    PB0     ------> USB_OTG_HS_ULPI_D1
    PB1     ------> USB_OTG_HS_ULPI_D2
    PB10     ------> USB_OTG_HS_ULPI_D3
    PB11     ------> USB_OTG_HS_ULPI_D4
    PB12     ------> USB_OTG_HS_ULPI_D5
    PB13     ------> USB_OTG_HS_ULPI_D6
    PB5     ------> USB_OTG_HS_ULPI_D7 
    */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  set_io_af(GPIOA, 3, GPIO_AF10_OTG_HS);
  set_io_af(GPIOA, 5, GPIO_AF10_OTG_HS);
  
  set_io_af(GPIOB, 0, GPIO_AF10_OTG_HS);
  set_io_af(GPIOB, 1, GPIO_AF10_OTG_HS);
  set_io_af(GPIOB, 10, GPIO_AF10_OTG_HS);
  set_io_af(GPIOB, 11, GPIO_AF10_OTG_HS);
  set_io_af(GPIOB, 12, GPIO_AF10_OTG_HS);
  set_io_af(GPIOB, 13, GPIO_AF10_OTG_HS);
  set_io_af(GPIOB, 5, GPIO_AF10_OTG_HS);
  
  set_io_af(GPIOC, 0, GPIO_AF10_OTG_HS);
  set_io_af(GPIOC, 2, GPIO_AF10_OTG_HS);
  set_io_af(GPIOC, 3, GPIO_AF10_OTG_HS);
}
#endif

// init the IO and OTG core
static void tusb_otg_core_init(tusb_core_t* core)
{
  USB_OTG_GlobalTypeDef* USBx = GetUSB(core);
  if(GetUSB(core) == USB_OTG_FS){
    // Init the FS core
#if defined(OTG_FS_EMBEDDED_PHY)
    // 1. Init the IO
    tusb_setup_otg_fs_io();
    // 2. Init the interrupt
    NVIC_SetPriority(OTG_FS_IRQn, 0);
    NVIC_EnableIRQ(OTG_FS_IRQn);
    // 3. Init the core
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
    /* Select FS Embedded PHY */
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
    /* Reset after a PHY select and set Host mode */
    Wait_CoreReset(USBx);
    /* Deactivate the power down*/
    USBx->GCCFG = USB_OTG_GCCFG_PWRDWN;
#endif
  }else if(GetUSB(core) == USB_OTG_HS){
    // Init the HS core
    // 1. Init the IO
#if defined(OTG_HS_EMBEDDED_PHY)    
    tusb_setup_otg_hs_io();
#elif defined(OTG_HS_EXTERNAL_PHY)    
    tusb_setup_otg_ulpi_io();
#endif
    // 2. Init the interrupt
    NVIC_SetPriority(OTG_HS_IRQn, 0);
    NVIC_EnableIRQ(OTG_HS_IRQn);
    
#if defined(USB_HS_PHYC)
    __HAL_RCC_OTGPHYC_CLK_ENABLE();
#endif
    
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
    __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
    // 3. Init the OTG HS core
#if defined(OTG_HS_EMBEDDED_PHY)
    // init embedded phy
    USBx->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);
    /* Init The UTMI Interface */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
    /* Select vbus source */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
    /* Select UTMI Interace */
    USBx->GUSBCFG &= ~ USB_OTG_GUSBCFG_ULPI_UTMI_SEL;
    USBx->GCCFG |= USB_OTG_GCCFG_PHYHSEN;
    /* Enables control of a High Speed USB PHY */
    USB_HS_PHYCInit(USBx);
#elif defined(OTG_HS_EXTERNAL_PHY)
    // Init external phy
    __HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();
    USBx->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);
    /* Init The ULPI Interface */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
    /* Select vbus source */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
#else
#error embedded phy or external phy not defined
#endif

#if defined(ENABLE_VBUS_DETECT)
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
#endif
    /* Reset after a PHY select  */
    Wait_CoreReset(USBx);
  }

#if defined(ENABLE_DMA)
  if(GetUSB(core) == USB_OTG_HS){
    // only HS core has DMA feature
    USBx->GAHBCFG |= USB_OTG_GAHBCFG_HBSTLEN_2;
    USBx->GAHBCFG |= USB_OTG_GAHBCFG_DMAEN;
  }
#endif
}


static void tusb_init_otg_device(tusb_device_t* dev)
{
  USB_OTG_GlobalTypeDef* USBx = GetUSB(dev);
#if defined(ENABLE_VBUS_DETECT)
  USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
#else
  USBx->GCCFG &= ~ USB_OTG_GCCFG_VBDEN;
  USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
  USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
#endif
  
  USBx_PCGCCTL = 0;
  USBx_DEVICE->DCFG |= DCFG_FRAME_INTERVAL_80;
  
  
  USBx_DEVICE->DCFG |= USBx == USB_OTG_FS ? OTG_FS_SPEED : OTG_HS_SPEED;
  
  flush_tx(USBx, 0x10);
  flush_rx(USBx);
  USBx_DEVICE->DIEPMSK = 0;
  USBx_DEVICE->DOEPMSK = 0;
  USBx_DEVICE->DAINT = 0xFFFFFFFF;
  USBx_DEVICE->DAINTMSK = 0;
  
  // ep will init in the reset handler
  USBx_DEVICE->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);
  // disable global int
  USBx->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
  USBx->GINTMSK = 0;
  USBx->GINTSTS = 0xBFFFFFFF;
  
  //TODO: support LPM, BCD feature
  if(USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN){
    // If DMA enabled, setup the threshold value
    USBx_DEVICE->DTHRCTL = (USB_OTG_DTHRCTL_TXTHRLEN_6 | USB_OTG_DTHRCTL_RXTHRLEN_6);
    USBx_DEVICE->DTHRCTL |= (USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN | USB_OTG_DTHRCTL_NONISOTHREN);
  }else{
    USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
  }

  /* Enable interrupts matching to the Device mode ONLY */
  USBx->GINTMSK |= (USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST |
                    USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT |
                    USB_OTG_GINTMSK_OEPINT   | USB_OTG_GINTMSK_IISOIXFRM|
                    USB_OTG_GINTMSK_PXFRM_IISOOXFRM | USB_OTG_GINTMSK_WUIM
#if defined(ENABLE_VBUS_DETECT)
              | (USB_OTG_GINTMSK_SRQIM | USB_OTG_GINTMSK_OTGINT)
#endif
                    );
  
  USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;
  USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;  
}


void tusb_otg_driver_vbus (USB_OTG_GlobalTypeDef* USBx, uint8_t state)
{
  __IO uint32_t hprt0 = 0U;
  hprt0 = USBx_HPRT0;
  hprt0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
             USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG);
  if (((hprt0 & USB_OTG_HPRT_PPWR) == 0U) && (state == 1U))
  {
    USBx_HPRT0 = (USB_OTG_HPRT_PPWR | hprt0);
  }
  if (((hprt0 & USB_OTG_HPRT_PPWR) == USB_OTG_HPRT_PPWR) && (state == 0U))
  {
    USBx_HPRT0 = ((~USB_OTG_HPRT_PPWR) & hprt0);
  }
}

static void tusb_init_otg_host(tusb_host_t* dev)
{
  USB_OTG_GlobalTypeDef* USBx = GetUSB(dev);
  //uint32_t USBx_BASE = (uint32_t)USBx;
  uint32_t MAX_CH_NUM = USBx == USB_OTG_FS ? USB_OTG_FS_MAX_CH_NUM : USB_OTG_HS_MAX_CH_NUM;
  
  uint32_t i;
  /* Restart the Phy Clock */
  USBx_PCGCCTL = 0U;

  /* Disable VBUS sensing */
  USBx->GCCFG &= ~(USB_OTG_GCCFG_VBDEN);
  
  USBx_HOST->HCFG &= ~(USB_OTG_HCFG_FSLSS);

  /* Make sure the FIFOs are flushed. */
  flush_tx(USBx, 0x10U); /* all Tx FIFOs */
  flush_rx(USBx);

  /* Clear all pending HC Interrupts */
  for (i = 0U; i < MAX_CH_NUM; i++){
    USBx_HC(i)->HCINT = 0xFFFFFFFFU;
    USBx_HC(i)->HCINTMSK = 0U;
  }

  /* Enable VBUS driving */
  tusb_otg_driver_vbus(USBx, 1U);
  
  delay_ms(2000);
  
  /* Disable all interrupts. */
  USBx->GINTMSK = 0U;

  /* Clear any pending interrupts */
  USBx->GINTSTS = 0xFFFFFFFFU;

  // The FIFO will not set success if remove the delay function before
  // How to wait is not mentioned
  // FIFO did not allocate all memory, the remain memory maybe used for the request queue
  // How request queue work is not describe in the RM
  if(USBx == USB_OTG_FS){
    /* set Rx/Tx FIFO size */
    USBx->GRXFSIZ  = 0x80U;
    USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t )(((0x60U << 16) & USB_OTG_NPTXFD) | 0x80U);
    USBx->HPTXFSIZ = (uint32_t )(((0x40U << 16)& USB_OTG_HPTXFSIZ_PTXFD) | 0xE0U);
  }else{
    /* set Rx/Tx FIFO size */
    USBx->GRXFSIZ  = 0x200U;
    USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t )(((0x100U << 16) & USB_OTG_NPTXFD) | 0x200U);
    USBx->HPTXFSIZ = (uint32_t )(((0xE0U << 16) & USB_OTG_HPTXFSIZ_PTXFD) | 0x300U);
  }

  if( (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN) == 0){
    // If DMA not enabled
    // copy data in fifo non-empty interrupt
    USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
  }

  /* Enable interrupts matching to the Host mode ONLY */
  USBx->GINTMSK |= (USB_OTG_GINTMSK_PRTIM            | USB_OTG_GINTMSK_HCIM |
                    USB_OTG_GINTMSK_SOFM             | USB_OTG_GINTMSK_DISCINT|
                    USB_OTG_GINTMSK_PXFRM_IISOOXFRM  | USB_OTG_GINTMSK_WUIM);
  
  
  USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
  
  /* Enable VBUS driving */
  tusb_otg_driver_vbus(USBx, 1U);
}

void tusb_open_device(tusb_device_t* dev)
{
  USB_OTG_GlobalTypeDef* USBx = GetUSB(dev);
  tusb_otg_core_init((tusb_core_t*) dev);
  USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD);
  USBx->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
  tusb_init_otg_device(dev);
}

void tusb_open_host(tusb_host_t* host)
{
  USB_OTG_GlobalTypeDef* USBx = GetUSB(host);
  tusb_otg_core_init((tusb_core_t*) host);
  // Force to device mode
  USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD);
  USBx->GUSBCFG |= USB_OTG_GUSBCFG_FHMOD;
  tusb_init_otg_host(host);
}

void tusb_otg_device_handler(tusb_device_t* dev);
void tusb_otg_host_handler(tusb_host_t* dev);

#define TUSB_OTG_GetMode(USBx) ((USBx->GINTSTS ) & 0x1U)

#if defined(USB_OTG_FS)
static tusb_device_t tusb_dev_otg_fs;
static tusb_host_t tusb_host_otg_fs;
void OTG_FS_IRQHandler(void)
{
  if (TUSB_OTG_GetMode(USB_OTG_FS) == USB_OTG_MODE_DEVICE){
#if defined(NO_DEVICE)
    while(1);
#else
    tusb_otg_device_handler(&tusb_dev_otg_fs);
#endif
  }else{
#if defined(NO_HOST)
    while(1);
#else
    tusb_otg_host_handler(&tusb_host_otg_fs);
#endif
  }
}
#endif


#if defined(USB_OTG_HS)
static tusb_device_t tusb_dev_otg_hs;
static tusb_host_t tusb_host_otg_hs;
void OTG_HS_IRQHandler(void)
{
  if (TUSB_OTG_GetMode(USB_OTG_HS) == USB_OTG_MODE_DEVICE){
#if defined(NO_DEVICE)
    while(1);
#else
    tusb_otg_device_handler(&tusb_dev_otg_hs);
#endif
  }else{
#if defined(NO_HOST)
    while(1);
#else
    tusb_otg_host_handler(&tusb_host_otg_hs);
#endif
  }
}
#endif

tusb_device_t* tusb_get_device(uint8_t id)
{
  if(id != 0){
#if defined(USB_OTG_HS)
    SetUSB(&tusb_dev_otg_hs, USB_OTG_HS);
    return &tusb_dev_otg_hs;
#endif
  }
#if defined(USB_OTG_FS)
  SetUSB(&tusb_dev_otg_fs, USB_OTG_FS);
  return &tusb_dev_otg_fs;
#else
  return 0;
#endif
}

tusb_host_t* tusb_get_host(uint8_t id)
{
  if(id != 0){
#if defined(USB_OTG_HS)
    SetUSB(&tusb_host_otg_hs, USB_OTG_HS);
    return &tusb_host_otg_hs;
#endif
  }
#if defined(USB_OTG_FS)
  SetUSB(&tusb_host_otg_fs, USB_OTG_FS);
  return &tusb_host_otg_fs;
#else
  return 0;
#endif
}

