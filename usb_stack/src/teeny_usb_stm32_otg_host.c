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

#include "string.h"
#include "teeny_usb.h"

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)

void tusb_otg_driver_vbus (USB_OTG_GlobalTypeDef* USBx, uint8_t state);
uint32_t tusb_otg_host_submit(tusb_host_t* host, uint8_t hc_num);
void tusb_host_deinit_channel(tusb_host_t* host, uint8_t hc_num);
static void tusb_otg_send_data(tusb_host_t* host, uint8_t hc_num);

static void tusb_otg_set_phy_clock(USB_OTG_GlobalTypeDef *USBx , uint8_t freq)
{
  //uint32_t USBx_BASE = (uint32_t)USBx;
  USBx_HOST->HCFG &= ~(USB_OTG_HCFG_FSLSPCS);
  USBx_HOST->HCFG |= (uint32_t)freq & USB_OTG_HCFG_FSLSPCS;
  if(freq == HCFG_48_MHZ){
    USBx_HOST->HFIR = 48000U;
  }else if (freq == HCFG_6_MHZ){
    USBx_HOST->HFIR = 6000U;
  }
}

static void tusb_otg_stop_host(tusb_host_t* host)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  uint32_t value;
  uint32_t i;


  /* Disable global int */
  USBx->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
  /* Flush FIFO */
  flush_tx(USBx, 0x10U);
  flush_rx(USBx);
  
  /* Flush out any leftover queued requests. */
  for (i = 0U; i <= 15U; i++)
  {
    value = USBx_HC(i)->HCCHAR;
    value |=  USB_OTG_HCCHAR_CHDIS;
    value &= ~USB_OTG_HCCHAR_CHENA;
    value &= ~USB_OTG_HCCHAR_EPDIR;
    USBx_HC(i)->HCCHAR = value;
  }

  /* Halt all channels to put them into a known state. */
  for (i = 0U; i <= 15U; i++)
  {
    uint32_t count = 0U;
    value = USBx_HC(i)->HCCHAR;
    value |= USB_OTG_HCCHAR_CHDIS;
    value |= USB_OTG_HCCHAR_CHENA;
    value &= ~USB_OTG_HCCHAR_EPDIR;
    USBx_HC(i)->HCCHAR = value;
    do{
      if (++count > 1000U) break;
    }while ((USBx_HC(i)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
  }

  /* Clear any pending Host interrupts */
  USBx_HOST->HAINT = 0xFFFFFFFFU;
  USBx->GINTSTS = 0xFFFFFFFFU;
  /* Enable global int */
  USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

static void tusb_otg_host_disconnect(tusb_host_t* host)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  tusb_otg_stop_host(host);
  host->state = TUSB_HOST_PORT_DISCONNECTED;
  
  memset(host->hc, 0, sizeof(host->hc));
  
  USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
  tusb_otg_driver_vbus(USBx, 1);
  
  tusb_host_port_changed(host, 0, TUSB_HOST_PORT_DISCONNECTED);
}

static void tusb_host_port_handler(tusb_host_t* host)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  uint32_t hprt0 = USBx_HPRT0;
  uint32_t hprt0_dup = USBx_HPRT0;
  hprt0_dup &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |\
                 USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG);
  
  if( hprt0 & USB_OTG_HPRT_PCDET ) {
    // Port connect detect
    if( hprt0 & USB_OTG_HPRT_PCSTS ){
      // A device is attached
      USB_MASK_INTERRUPT(USBx, USB_OTG_GINTSTS_DISCINT);
      // Post connect message
      host->state = TUSB_HOST_PORT_CONNECTED;
      tusb_host_port_changed(host, 0, TUSB_HOST_PORT_CONNECTED);
    }
    // write 1 to clear the PCDET int flag
    hprt0_dup  |= USB_OTG_HPRT_PCDET;
  }
  
  if( hprt0 & USB_OTG_HPRT_PENCHNG ) {
    // Port enable changed
    if( hprt0 & USB_OTG_HPRT_PENA ){
      if(USBx == USB_OTG_FS){
        if ((hprt0 & USB_OTG_HPRT_PSPD) == (HPRT0_PRTSPD_LOW_SPEED << 17)){
          // Low speed
          tusb_otg_set_phy_clock(USBx, HCFG_6_MHZ);
        }else{
          // Full speed
          tusb_otg_set_phy_clock(USBx, HCFG_48_MHZ);
        }
      }else{
        if(USBx == USB_OTG_FS){
          // FS phy
          USBx_HOST->HFIR = 60000U;
        }
      }
      host->state = TUSB_HOST_PORT_ENABLED;
      tusb_host_port_changed(host, 0, TUSB_HOST_PORT_ENABLED);
      // Port enabled
    }else{
      // Port disabled
      USBx_HPRT0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
                      USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );
      USB_UNMASK_INTERRUPT(USBx, USB_OTG_GINTSTS_DISCINT);
      host->state = TUSB_HOST_PORT_DISABLED;
      tusb_host_port_changed(host, 0, TUSB_HOST_PORT_DISABLED);
    }
    hprt0_dup |= USB_OTG_HPRT_PENCHNG;
  }
  
  if( hprt0 & USB_OTG_HPRT_POCCHNG )
  {
    hprt0_dup |= USB_OTG_HPRT_POCCHNG;
  }
  USBx_HPRT0 = hprt0_dup;
}

void tusb_port_set_reset(tusb_host_t* host, uint8_t port, uint8_t reset)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  uint32_t hprt0 = USBx_HPRT0;
  hprt0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
             USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG);
  
  if(reset){
    USBx_HPRT0 = (USB_OTG_HPRT_PRST | hprt0);
  }else{
    USBx_HPRT0 = ((~USB_OTG_HPRT_PRST) & hprt0);
  }
}


void tusb_otg_read_data(USB_OTG_GlobalTypeDef *USBx, void* buf, uint32_t len);

// copy data to channel buffer in rx handler
static void tusb_otg_host_rx_handler(tusb_host_t* host)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  uint32_t sts = USBx->GRXSTSP;
  uint8_t ch_num = sts & USB_OTG_GRXSTSP_EPNUM;
  uint32_t len = (sts & USB_OTG_GRXSTSP_BCNT) >> 4;
  if(((sts & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  GRXSTS_PKTSTS_IN){
    tusb_hc_data_t* hc = &host->hc[ch_num];
    if(hc->ch_buf && hc->count < hc->size){
      tusb_otg_read_data(USBx, hc->ch_buf + hc->count, len);
      hc->count += len;
    }else{
      tusb_otg_read_data(USBx, 0, len);
    }
    if( USBx_HC(ch_num)->HCTSIZ & USB_OTG_HCTSIZ_PKTCNT ){
      uint32_t tmpreg = USBx_HC(ch_num)->HCCHAR;
      tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      tmpreg |= USB_OTG_HCCHAR_CHENA;
      USBx_HC(ch_num)->HCCHAR = tmpreg;
      hc->toggle_in ^= 1;
    }
  }
}

// this function directly fork from the HAL
static void tusb_otg_halt_channel(USB_OTG_GlobalTypeDef *USBx, uint8_t hc_num)
{
  uint32_t HcEpType = (USBx_HC(hc_num)->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> 18;
  uint32_t tx_q_space = 0;
  
  uint32_t dir = USBx_HC(hc_num)->HCCHAR & USB_OTG_HCCHAR_EPDIR;
  
  /* Check for space in the request queue to issue the halt. */
  if ((HcEpType == HCCHAR_CTRL) || (HcEpType == HCCHAR_BULK)) {
    USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHDIS;
    tx_q_space = USBx->HNPTXSTS & (0xFFU << 16);
  }else{
    USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHDIS;
    tx_q_space = USBx_HOST->HPTXSTS & (0xFFU << 16);
  }
  if(tx_q_space == 0){
    uint32_t count = 0U;
    USBx_HC(hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_CHENA;
    USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
    USBx_HC(hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_EPDIR;
    do {
      if (++count > 1000U) {
        break;
      }
    } while ((USBx_HC(hc_num)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
  }else{
    USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
  }
  
  USBx_HC(hc_num)->HCCHAR |= dir;
  
  
 /*
  if ((HcEpType == HCCHAR_CTRL) || (HcEpType == HCCHAR_BULK)) {
    if ((USBx->HNPTXSTS & (0xFFU << 16)) == 0U) {
      uint32_t count = 0U;
      USBx_HC(hcnum)->HCCHAR &= ~USB_OTG_HCCHAR_CHENA;
      USBx_HC(hcnum)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
      USBx_HC(hcnum)->HCCHAR &= ~USB_OTG_HCCHAR_EPDIR;
      do {
        if (++count > 1000U) {
          break;
        }
      } while ((USBx_HC(hcnum)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
    }else{
      USBx_HC(hcnum)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
    }
  }else{
    USBx_HC(hcnum)->HCCHAR |= USB_OTG_HCCHAR_CHDIS;

    if ((USBx_HOST->HPTXSTS & (0xFFU << 16)) == 0U){
      uint32_t count = 0U;
      USBx_HC(hcnum)->HCCHAR &= ~USB_OTG_HCCHAR_CHENA;
      USBx_HC(hcnum)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
      USBx_HC(hcnum)->HCCHAR &= ~USB_OTG_HCCHAR_EPDIR;
      do{
        if (++count > 1000U){
          break;
        }
      }while ((USBx_HC(hcnum)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
    }else{
       USBx_HC(hcnum)->HCCHAR |= USB_OTG_HCCHAR_CHENA;
    }
  }
  */
}

#define  MASK_HALT()           do{ HC->HCINTMSK &= ~USB_OTG_HCINTMSK_CHHM; }while(0)
#define  UNMASK_HALT()         do{ HC->HCINTMSK |= USB_OTG_HCINTMSK_CHHM; }while(0)
#define  CLEAR_INT(interrupt)  do{ HC->HCINT = (interrupt); }while(0)

#ifndef __HAL_HCD_CLEAR_HC_INT
#define  __HAL_HCD_CLEAR_HC_INT(ch, interrupt)\
  do{  USBx_HC(ch)->HCINT = (interrupt);   }while(0)
#endif

#ifndef __HAL_HCD_UNMASK_HALT_HC_INT  
#define __HAL_HCD_UNMASK_HALT_HC_INT(ch) \
  do{  USBx_HC(ch)->HCINTMSK |= USB_OTG_HCINTMSK_CHHM;   }while(0)
#endif
  
#ifndef __HAL_HCD_MASK_HALT_HC_INT
#define __HAL_HCD_MASK_HALT_HC_INT(ch) \
  do{  USBx_HC(ch)->HCINTMSK &= ~USB_OTG_HCINTMSK_CHHM;   }while(0)
#endif

static void tusb_otg_in_channel_handler(tusb_host_t* host, uint8_t ch_num)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  USB_OTG_HostChannelTypeDef* HC = USBx_HC(ch_num);
  tusb_hc_data_t* hc = &host->hc[ch_num];
  uint32_t tmpreg;
  
  if ((HC->HCINT & USB_OTG_HCINT_AHBERR) == USB_OTG_HCINT_AHBERR)
  {
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_AHBERR);
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
  }
  else if( (HC->HCINT & USB_OTG_HCINT_BBERR) == USB_OTG_HCINT_BBERR )
  {
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_BBERR);
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_ACK) == USB_OTG_HCINT_ACK)
  {
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_ACK);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_STALL) == USB_OTG_HCINT_STALL)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    hc->state = TUSB_CS_STALL;
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_NAK);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_STALL);
    tusb_otg_halt_channel(USBx, ch_num);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_DTERR) == USB_OTG_HCINT_DTERR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_NAK);
    hc->state = TUSB_CS_DT_ERROR;
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_DTERR);
  }
  else
  {
    /* ... */
  }

  if ((HC->HCINT & USB_OTG_HCINT_FRMOR) == USB_OTG_HCINT_FRMOR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_FRMOR);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_XFRC) == USB_OTG_HCINT_XFRC)
  {
    uint32_t HcEpType = (HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> 18;
    if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN)
    {
      hc->count = hc->size - (HC->HCTSIZ & USB_OTG_HCTSIZ_XFRSIZ);
    }

    hc->state = TUSB_CS_TRANSFER_COMPLETE;
    hc->error_count = 0U;
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_XFRC);

    if ((HcEpType == HCCHAR_CTRL)||
        (HcEpType == HCCHAR_BULK))
    {
      __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
      tusb_otg_halt_channel(USBx, ch_num);
      __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_NAK);
    }
    else if(HcEpType == HCCHAR_INTR)
    {
      HC->HCCHAR |= USB_OTG_HCCHAR_ODDFRM;
      hc->xfer_done = 1;
      tusb_on_channel_event(host, ch_num);
    }
    else
    {
      /* ... */
    }
    hc->toggle_in ^= 1U;

  }
  else if ((HC->HCINT & USB_OTG_HCINT_CHH) == USB_OTG_HCINT_CHH)
  {
    __HAL_HCD_MASK_HALT_HC_INT(ch_num);

    if(hc->state == TUSB_CS_TRANSFER_COMPLETE)
    {
      hc->xfer_done = 1;
      //hhcd->hc[ch_num].urb_state  = URB_DONE;
    }
    else if (hc->state == TUSB_CS_STALL)
    {
      //hhcd->hc[ch_num].urb_state  = URB_STALL;
    }
    else if((hc->state == TUSB_CS_TRANSACTION_ERROR) ||
            (hc->state == TUSB_CS_DT_ERROR))
    {
      hc->error_count++;
      if(hc->error_count > 3U)
      {
        hc->error_count = 0U;
        //hhcd->hc[ch_num].urb_state = URB_ERROR;
      }
      else
      {
        //hhcd->hc[ch_num].urb_state = TUSB_CS_XFER_ONGOING;
      }

      /* re-activate the channel  */
      tmpreg = HC->HCCHAR;
      tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      tmpreg |= USB_OTG_HCCHAR_CHENA;
      HC->HCCHAR = tmpreg;
    }
    else if (hc->state == TUSB_CS_NAK)
    {
      //hhcd->hc[ch_num].urb_state  = URB_NOTREADY;
      /* re-activate the channel  */
      tmpreg = HC->HCCHAR;
      tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      tmpreg |= USB_OTG_HCCHAR_CHENA;
      HC->HCCHAR = tmpreg;
    }
    else
    {
      /* ... */
    }
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_CHH);
    tusb_on_channel_event(host, ch_num);
    //HAL_HCD_HC_NotifyURBChange_Callback(hhcd, (uint8_t)ch_num, hhcd->hc[ch_num].urb_state);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_TXERR) == USB_OTG_HCINT_TXERR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    hc->error_count++;
    hc->state = TUSB_CS_TRANSACTION_ERROR;
    tusb_otg_halt_channel(USBx, ch_num);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_TXERR);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_NAK) == USB_OTG_HCINT_NAK)
  {
    uint32_t HcEpType = (HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> 18;
    if(HcEpType == HCCHAR_INTR)
    {
      hc->xfer_done = 1;
      hc->state = TUSB_CS_INT_NAK;
      __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
      tusb_otg_halt_channel(USBx, ch_num);
    }
    else if ((HcEpType == HCCHAR_CTRL)||
             (HcEpType == HCCHAR_BULK))
    {
       hc->error_count = 0U;
       if ( (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN) == 0)
       {
         hc->state = TUSB_CS_NAK;
         __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
         tusb_otg_halt_channel(USBx, ch_num);
       }
    }
    else
    {
      /* ... */
    }
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_NAK);
  }
  else
  {
    /* ... */
  }
}

static void tusb_otg_out_channel_handler(tusb_host_t* host, uint8_t ch_num)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  USB_OTG_HostChannelTypeDef* HC = USBx_HC(ch_num);
  tusb_hc_data_t* hc = &host->hc[ch_num];
  uint32_t tmpreg;

  if ((HC->HCINT & USB_OTG_HCINT_AHBERR) == USB_OTG_HCINT_AHBERR)
  {
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_AHBERR);
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_ACK) == USB_OTG_HCINT_ACK)
  {
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_ACK);

    if( hc->do_ping == 1U)
    {
      hc->do_ping = 0U;
      //hhcd->hc[ch_num].urb_state  = URB_NOTREADY;
      __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
      tusb_otg_halt_channel(USBx, ch_num);
    }
  }
  else if ((HC->HCINT & USB_OTG_HCINT_NYET) == USB_OTG_HCINT_NYET)
  {
    hc->state = TUSB_CS_NYET;
    hc->do_ping = 1U;
    hc->error_count = 0U;
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_NYET);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_FRMOR) == USB_OTG_HCINT_FRMOR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_FRMOR);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_XFRC) == USB_OTG_HCINT_XFRC)
  {
    hc->error_count = 0U;
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_XFRC);
    hc->state = TUSB_CS_TRANSFER_COMPLETE;
  }
  else if ((HC->HCINT & USB_OTG_HCINT_STALL) == USB_OTG_HCINT_STALL)
  {
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_STALL);
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    hc->state = TUSB_CS_STALL;
  }
  else if ((HC->HCINT & USB_OTG_HCINT_NAK) == USB_OTG_HCINT_NAK)
  {
    hc->error_count = 0U;
    hc->state = TUSB_CS_NAK;
    if (hc->do_ping == 0){
      if (hc->speed == PORT_SPEED_HIGH){
        hc->do_ping = 1;
      }
    }
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_NAK);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_TXERR) == USB_OTG_HCINT_TXERR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    hc->state = TUSB_CS_TRANSACTION_ERROR;
     __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_TXERR);
  }
  else if ((HC->HCINT & USB_OTG_HCINT_DTERR) == USB_OTG_HCINT_DTERR)
  {
    __HAL_HCD_UNMASK_HALT_HC_INT(ch_num);
    tusb_otg_halt_channel(USBx, ch_num);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_NAK);
    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_DTERR);
    hc->state = TUSB_CS_DT_ERROR;
  }
  else if ((HC->HCINT & USB_OTG_HCINT_CHH) == USB_OTG_HCINT_CHH)
  {
    __HAL_HCD_MASK_HALT_HC_INT(ch_num);

    if (hc->state == TUSB_CS_TRANSFER_COMPLETE){
      uint32_t HcEpType = (HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> 18;
      if(HcEpType == HCCHAR_BULK){
        hc->toggle_out ^= 1U;
      }
      hc->xfer_done = 1;
    }
    else if (hc->state == TUSB_CS_NAK)
    {
      /* re-activate the channel  */
      //tmpreg = HC->HCCHAR;
      //tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      //tmpreg |= USB_OTG_HCCHAR_CHENA;
      //HC->HCCHAR = tmpreg;
      //hhcd->hc[ch_num].urb_state = URB_NOTREADY;
    }
    else if (hc->state == TUSB_CS_NYET)
    {
      //hhcd->hc[ch_num].urb_state  = URB_NOTREADY;
    }
    else if (hc->state == TUSB_CS_STALL)
    {
      //hhcd->hc[ch_num].urb_state  = URB_STALL;
    }
    else if ((hc->state == TUSB_CS_TRANSACTION_ERROR) ||
            (hc->state == TUSB_CS_DT_ERROR))
    {
      hc->error_count++;
      if (hc->error_count > 3U)
      {
        hc->error_count = 0U;
        //hhcd->hc[ch_num].urb_state = URB_ERROR;
      }
      else
      {
        //hhcd->hc[ch_num].urb_state = URB_NOTREADY;
      }

      /* re-activate the channel  */
      tmpreg = HC->HCCHAR;
      tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
      tmpreg |= USB_OTG_HCCHAR_CHENA;
      HC->HCCHAR = tmpreg;
    }
    else
    {
      /* ... */
    }

    __HAL_HCD_CLEAR_HC_INT(ch_num, USB_OTG_HCINT_CHH);
    tusb_on_channel_event(host, ch_num);
  }
  else
  {
     /* ... */
  }
}


// handle channel event 
void tusb_otg_host_channel_handler(tusb_host_t* host, uint8_t hc_num)
{
#define  MASK_HALT()           do{ HC->HCINTMSK &= ~USB_OTG_HCINTMSK_CHHM; }while(0)
#define  UNMASK_HALT()         do{ HC->HCINTMSK |= USB_OTG_HCINTMSK_CHHM; }while(0)
#define  CLEAR_INT(interrupt)  do{ HC->HCINT = (interrupt); }while(0)
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  USB_OTG_HostChannelTypeDef* HC = USBx_HC(hc_num);
  tusb_hc_data_t* hc = &host->hc[hc_num];
  uint8_t is_in = (HC->HCCHAR & USB_OTG_HCCHAR_EPDIR) != 0;
  if(HC->HCINT & USB_OTG_HCINT_XFRC){
    // Transfer complete
    CLEAR_INT(USB_OTG_HCINT_XFRC);
    hc->state = TUSB_CS_TRANSFER_COMPLETE;
    if(is_in){
      uint32_t HcEpType = (HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> 18;
      // IN transfer, and DMA enabled
      if(USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN){
        hc->count = hc->size - (HC->HCTSIZ & USB_OTG_HCTSIZ_XFRSIZ);
      }else{
        // otherwise, rx data count will update in the tusb_otg_host_rx_handler
      }
      
      if(HcEpType == HCCHAR_CTRL || HcEpType == HCCHAR_BULK){
        CLEAR_INT(USB_OTG_HCINT_NAK);
      }else if( HcEpType == HCCHAR_INTR ){
        HC->HCCHAR |= USB_OTG_HCCHAR_ODDFRM;
      }
      hc->toggle_in ^= 1U;
    }else{
      // OUT transfer complete, host send data done
    }
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
    
  }else if ( HC->HCINT & USB_OTG_HCINT_ACK ) {
    if(!is_in){
      // for out EP, ping ack means host can start OUT transaction
      if(hc->do_ping){
        // stop ping
        hc->do_ping = 0;
        hc->state = TSUB_CS_PING_SUCCESS;
        // halt current channel, re-active in the halt interrupt
        UNMASK_HALT();
        tusb_otg_halt_channel(USBx, hc_num);
      }
    }else{
      // IN EP will got data instead of ACK, never enter here
    }
    CLEAR_INT(USB_OTG_HCINT_ACK);
    
    
  }else if( HC->HCINT & USB_OTG_HCINT_NYET ){
    // not yet only for OUT ping
    hc->state = TUSB_CS_NYET;
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
    
  }else if ( HC->HCINT & USB_OTG_HCINT_STALL ) {
    CLEAR_INT(USB_OTG_HCINT_STALL);
    CLEAR_INT(USB_OTG_HCINT_NAK);
    hc->state = TUSB_CS_STALL;
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
    
  }else if ( HC->HCINT & USB_OTG_HCINT_AHBERR ) {
    CLEAR_INT(USB_OTG_HCINT_AHBERR);
    hc->state = TUSB_CS_AHB_ERROR;
    hc->error_reason = HC->HCDMA;
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
    
  }else if ( HC->HCINT & USB_OTG_HCINT_TXERR ){
    CLEAR_INT(USB_OTG_HCINT_TXERR);
    hc->state = TUSB_CS_TRANSACTION_ERROR;
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
    
  }else if ( HC->HCINT & USB_OTG_HCINT_DTERR ){
    CLEAR_INT(USB_OTG_HCINT_DTERR);
    CLEAR_INT(USB_OTG_HCINT_NAK);
    hc->state = TUSB_CS_DT_ERROR;
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
    
  }else if ( HC->HCINT & USB_OTG_HCINT_FRMOR ){
    CLEAR_INT(USB_OTG_HCINT_FRMOR);
    hc->state = TUSB_CS_FRAMEOVERRUN_ERROR;
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
  }else if( HC->HCINT & USB_OTG_HCINT_BBERR ){
    // OOPS reset the port
    CLEAR_INT(USB_OTG_HCINT_BBERR);
    hc->state = TUSB_CS_BABBLE_ERROR;
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
  }
  
  else if( HC->HCINT & USB_OTG_HCINT_CHH ) {
    // Do the real work here
    // re-active means the user application known the data transfer complete, 
    uint8_t reactive;
    MASK_HALT();
    
    reactive = 0xff;
    switch(hc->state){
      case TUSB_CS_TRANSFER_COMPLETE:
        hc->nak_count = 0;
        reactive = tusb_on_channel_event(host, hc_num) == 0;
        break;
      case TUSB_CS_NAK:
        if(is_in){
          uint32_t HcEpType = (HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> 18;
          if(HcEpType == HCCHAR_INTR){
            reactive = tusb_on_channel_event(host, hc_num) == 0;
          }else{
            uint32_t tmpreg;
            // re-enable the channel 
            tmpreg = HC->HCCHAR;
            tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
            tmpreg |= USB_OTG_HCCHAR_CHENA;
            HC->HCCHAR = tmpreg;
          }
        }else{
          // do nothing for out channel
        }
        break;
      case TUSB_CS_ACK:
        // never reach here
        //reactive = 0;
        break;
      case TSUB_CS_PING_SUCCESS:
        // Ping success, start OUT transaction
        //reactive = 0;
        break;
      case TUSB_CS_NYET:
        // only for HS core
        //if(hc->retry_count){
        //  reactive = 1;
        //  hc->do_ping = 1;
        //  if(hc->retry_count != RETRY_FOREVER){
        //    hc->retry_count--;
        //  }
        //}else{
        //  reactive = tusb_on_channel_event(host, hc_num) == 0;
        //}
        //if(reactive){
          hc->do_ping = 1;
        //}
        break;
      case TUSB_CS_STALL:
        hc->nak_count = 0;
        reactive = tusb_on_channel_event(host, hc_num) == 0;
        break;
      case TUSB_CS_TRANSACTION_ERROR:
      case TUSB_CS_DT_ERROR:
        hc->error_count++;
        {
          // re-enable the channel 
          uint32_t tmpreg = HC->HCCHAR;
          tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
          tmpreg |= USB_OTG_HCCHAR_CHENA;
          HC->HCCHAR = tmpreg;
        }
        break;
      case TUSB_CS_AHB_ERROR:
      case TUSB_CS_FRAMEOVERRUN_ERROR:
      case TUSB_CS_BABBLE_ERROR:
        reactive = tusb_on_channel_event(host, hc_num) == 0;
        break;
    }
    
    if(reactive != 0xff){
      hc->xfer_done = 1;
      if(reactive){
        // re-wind the buffer pointer
        hc->count = 0;
        hc->xfer_done = 0;
        // re-submit the transaction
        tusb_otg_host_submit(host, hc_num);
      }else{
        // application no need
        if(hc->auto_free){
          tusb_host_deinit_channel(host, hc_num);
        }
      }
    }
    
    CLEAR_INT(USB_OTG_HCINT_CHH);
  }else if( HC->HCINT & USB_OTG_HCINT_NAK ){
    hc->nak_count++;
    if(is_in ){
      uint32_t HcEpType = (HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> 18;
      /* Check for space in the request queue to issue the halt. */
      if(HcEpType == HCCHAR_INTR){
        // Halt channel, not set the state
        hc->state = TUSB_CS_NAK;
        UNMASK_HALT();
        tusb_otg_halt_channel(USBx, hc_num);
        
      }else if ((HcEpType == HCCHAR_CTRL) || (HcEpType == HCCHAR_BULK)  ) {
#if defined(USB_OTG_HS)
        if( ((USBx == USB_OTG_HS) && ((USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN) != 0)) ){
          // DMA enable, don't halt the channel
        }
        else
#endif
        {
          hc->state = TUSB_CS_NAK;
          UNMASK_HALT();
          tusb_otg_halt_channel(USBx, hc_num);
        }
      }
    }else{
      hc->state = TUSB_CS_NAK;
      // In HS mode OUT ep, if NAK got, use ping to detect OUT status
#if defined(USB_OTG_HS)
      if( USBx == USB_OTG_HS ){
        hc->do_ping = 1;
      }
#endif
      UNMASK_HALT();
      tusb_otg_halt_channel(USBx, hc_num);
    }
    CLEAR_INT(USB_OTG_HCINT_NAK);
    
    
  }
#undef  MASK_HALT
#undef  UNMASK_HALT
#undef  CLEAR_INT
}


#define  INTR()   (USBx->GINTSTS & USBx->GINTMSK)

// Interrupt handler of HOST mode
void tusb_otg_host_handler(tusb_host_t* host)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  uint32_t MAX_CH_NUM = USBx == USB_OTG_FS ? USB_OTG_FS_MAX_CH_NUM : USB_OTG_HS_MAX_CH_NUM;
  if(INTR() == 0){
    return;
  }
  if( INTR() & USB_OTG_GINTSTS_PXFR_INCOMPISOOUT ){
    USBx->GINTSTS = USB_OTG_GINTSTS_PXFR_INCOMPISOOUT;
  }
  if(INTR() & USB_OTG_GINTSTS_IISOIXFR){
    USBx->GINTSTS = USB_OTG_GINTSTS_IISOIXFR;
  }
  if(INTR() & USB_OTG_GINTSTS_PTXFE){
    uint32_t mask = host->ptx_pending;
    uint8_t ch_num = 0;
    USBx->GINTSTS = USB_OTG_GINTSTS_PTXFE;
    while(mask){
      if(mask & 1){
        tusb_otg_send_data(host, ch_num);
        break;
      }
      ch_num++;
      mask>>=1;
    }
    if(!host->ptx_pending){
      USBx->GINTMSK &= ~USB_OTG_GINTMSK_PTXFEM;
    }
    
  }
  if(INTR() & USB_OTG_GINTSTS_NPTXFE){
    uint32_t mask = host->nptx_pending;
    uint8_t ch_num = 0;
    USBx->GINTSTS = USB_OTG_GINTSTS_NPTXFE;
    while(mask){
      if(mask & 1){
        tusb_otg_send_data(host, ch_num);
        break;
      }
      ch_num++;
      mask>>=1;
    }
    if(!host->nptx_pending){
      USBx->GINTMSK &= ~USB_OTG_GINTMSK_NPTXFEM;
    }
  }
  if(INTR() & USB_OTG_GINTSTS_MMIS){
    USBx->GINTSTS = USB_OTG_GINTSTS_MMIS;
  }
  /* Handle Host Disconnect Interrupts */
  if(INTR() &USB_OTG_GINTSTS_DISCINT){
    /* Cleanup HPRT */
    USBx_HPRT0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
      USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );
    /* Handle Host Port Interrupts */
    tusb_otg_host_disconnect(host);
    tusb_otg_set_phy_clock(USBx, HCFG_48_MHZ);
    
    USBx->GINTSTS = USB_OTG_GINTSTS_DISCINT;
  }
  /* Handle Host Port Interrupts */
  if( INTR() & USB_OTG_GINTSTS_HPRTINT ){
    tusb_host_port_handler(host);
  }
  /* Handle Host SOF Interrupts */
  if( INTR() & USB_OTG_GINTSTS_SOF ){
    USBx->GINTSTS = USB_OTG_GINTSTS_SOF;
  }
  /* Handle Host channel Interrupts */
  if( INTR() & USB_OTG_GINTSTS_HCINT ){
    uint32_t hc_intr = (USBx_HOST->HAINT) & 0xFFFFU;
    uint32_t hc_num = 0;
    while(hc_intr){
      if(hc_intr & 1){
        if( USBx_HC(hc_num)->HCCHAR & USB_OTG_HCCHAR_EPDIR ){
          tusb_otg_in_channel_handler(host, hc_num);
        }else{
          tusb_otg_out_channel_handler(host, hc_num);
        }
        //tusb_otg_host_channel_handler(host, hc_num);
      }
      hc_num++;
      hc_intr>>=1;
      if(hc_num>MAX_CH_NUM) break;
    }
    USBx->GINTSTS = USB_OTG_GINTSTS_HCINT;
  }
  /* Handle Rx Queue Level Interrupts */
  if( INTR() & USB_OTG_GINTSTS_RXFLVL ){
    USB_MASK_INTERRUPT(USBx, USB_OTG_GINTSTS_RXFLVL);
    tusb_otg_host_rx_handler(host);
    USB_UNMASK_INTERRUPT(USBx, USB_OTG_GINTSTS_RXFLVL);
  }
}

void tusb_host_init_channel(tusb_host_t* host, uint8_t hc_num, uint8_t dev_addr, uint8_t ep_addr, uint8_t ep_type, uint16_t mps, uint8_t speed)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  USB_OTG_HostChannelTypeDef* HC = USBx_HC(hc_num);
  tusb_hc_data_t* hc = &host->hc[hc_num];
  // clear old interrupt flags
  HC->HCINT = 0xFFFFFFFFU;
  switch(ep_type){
    case EP_TYPE_CTRL:
    case EP_TYPE_BULK:
      HC->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
                     USB_OTG_HCINTMSK_STALLM |
                     USB_OTG_HCINTMSK_TXERRM |
                     USB_OTG_HCINTMSK_DTERRM |
                     USB_OTG_HCINTMSK_AHBERR |
                     USB_OTG_HCINTMSK_NAKM;
      if (ep_addr & 0x80){
        HC->HCINTMSK |= USB_OTG_HCINTMSK_BBERRM;
      }else{
        if(USBx != USB_OTG_FS){
          HC->HCINTMSK |= (USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
        }
      }
      break;
    case EP_TYPE_INTR:
      HC->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
                     USB_OTG_HCINTMSK_STALLM |
                     USB_OTG_HCINTMSK_TXERRM |
                     USB_OTG_HCINTMSK_DTERRM |
                     USB_OTG_HCINTMSK_NAKM   |
                     USB_OTG_HCINTMSK_AHBERR |
                     USB_OTG_HCINTMSK_FRMORM;

    if (ep_addr & 0x80){
      HC->HCINTMSK |= USB_OTG_HCINTMSK_BBERRM;
    }
    break;
    case EP_TYPE_ISOC:
      HC->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |
                     USB_OTG_HCINTMSK_ACKM   |
                     USB_OTG_HCINTMSK_AHBERR |
                     USB_OTG_HCINTMSK_FRMORM;
    if ( ep_addr & 0x80U ){
      HC->HCINTMSK |= (USB_OTG_HCINTMSK_TXERRM | USB_OTG_HCINTMSK_BBERRM);
    }
    break;
  }
  
  USBx_HOST->HAINTMSK |= 1UL << (hc_num & 0xFU);
  /* Make sure host channel interrupts are enabled. */
  USBx->GINTMSK |= USB_OTG_GINTMSK_HCIM;
  
  HC->HCCHAR = ( ((uint32_t)dev_addr << USB_OTG_HCCHAR_DAD_Pos )           |
                 ((uint32_t)(ep_addr&0x7f) << USB_OTG_HCCHAR_EPNUM_Pos )   |
                 ((uint32_t)ep_type  << USB_OTG_HCCHAR_EPTYP_Pos )         |
                   ((uint32_t)mps & USB_OTG_HCCHAR_MPSIZ)                 );
  
                    
  if(ep_addr & 0x80){
    HC->HCCHAR |= USB_OTG_HCCHAR_EPDIR;
  }
  if( speed == PORT_SPEED_LOW){
    HC->HCCHAR |= USB_OTG_HCCHAR_LSDEV;
  }
  
  if (ep_type == EP_TYPE_INTR){
    HC->HCCHAR |= USB_OTG_HCCHAR_ODDFRM ;
  }
  memset(hc, 0, sizeof(*hc));
  hc->speed = speed;
  hc->is_use = 1;
}


uint8_t tusb_port_get_speed(tusb_host_t* host, uint8_t port)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  return (USBx_HPRT0 & USB_OTG_HPRT_PSPD)>>USB_OTG_HPRT_PSPD_Pos;
}

void tusb_host_deinit_channel(tusb_host_t* host, uint8_t hc_num)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  USB_OTG_HostChannelTypeDef* HC = USBx_HC(hc_num);
  tusb_hc_data_t* hc = &host->hc[hc_num];
  // close all interrupt mask
  HC->HCINTMSK = 0;
  HC->HCCHAR = 0;
  memset(hc, 0, sizeof(*hc));
}

uint8_t  tusb_otg_host_get_free_ch(tusb_host_t* host)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  uint32_t MAX_CH_NUM = USBx == USB_OTG_FS ? USB_OTG_FS_MAX_CH_NUM : USB_OTG_HS_MAX_CH_NUM;
  uint32_t i;
  for(i=0;i<MAX_CH_NUM;i++){
    if(!host->hc[i].is_use){
      if( (USBx_HC(i)->HCCHAR & USB_OTG_HCCHAR_CHENA) == 0 ) {
        return i;
      }
    }
  }
  return 0xff;
}

static void tusb_otg_send_data(tusb_host_t* host, uint8_t hc_num)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  USB_OTG_HostChannelTypeDef* HC = USBx_HC(hc_num);
  uint32_t mps = HC->HCCHAR & USB_OTG_HCCHAR_MPSIZ;
  uint8_t xfer_type = ((HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> USB_OTG_HCCHAR_EPTYP_Pos);
  tusb_hc_data_t* hc = &host->hc[hc_num];
  uint32_t i;
  uint32_t act_len;
  uint32_t req_size;
  uint32_t *pSrc = (uint32_t *)hc->ch_buf;
  uint32_t len = hc->size;
  // len = (len+3)/4;
  if(xfer_type == EP_TYPE_CTRL || xfer_type == EP_TYPE_BULK){
    act_len = USBx->HNPTXSTS;
  }else{
    act_len = USBx_HOST->HPTXSTS;
  }
  // calculate useful FIFO size, depend on two factor
  // 1. remain [request queue space] * [max packet size]
  req_size = ((act_len & 0xff0000) >> 16) * mps;
  // 2. remain FIFO buffer size, round to max packet size boundary
  act_len = ( (act_len & 0xffff) *4 / mps )  * mps;
  if(act_len > req_size) act_len = req_size;
  if(len > act_len){
    len = len - act_len;
  }else{
    act_len = len;
    len = 0;
  }
  act_len = (act_len+3)/4;
  for(i=0;i<act_len;i++){
    USBx_DFIFO((uint32_t)hc_num) = *((__packed uint32_t *)pSrc);
    pSrc++;
  }
  
  hc->ch_buf = (uint8_t*)pSrc;
  hc->size = len;
  if(len){
    if(xfer_type == EP_TYPE_CTRL || xfer_type == EP_TYPE_BULK){
      USBx->GINTMSK |= USB_OTG_GINTMSK_NPTXFEM;
      host->nptx_pending |= (1<<hc_num);
    }else{
      USBx->GINTMSK |= USB_OTG_GINTMSK_PTXFEM;
      host->ptx_pending |= (1<<hc_num);
    }
  }else{
    host->nptx_pending &= ~(1<<hc_num);
    host->ptx_pending &= ~(1<<hc_num);
  }
}


uint32_t tusb_otg_host_submit(tusb_host_t* host, uint8_t hc_num)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  USB_OTG_HostChannelTypeDef* HC = USBx_HC(hc_num);
  tusb_hc_data_t* hc = &host->hc[hc_num];
  uint32_t mps = HC->HCCHAR & USB_OTG_HCCHAR_MPSIZ;
  uint32_t pkt_cnt;
  uint32_t len = hc->size;
  uint32_t data_pid = HC_PID_DATA1;
  uint8_t is_in = (HC->HCCHAR & USB_OTG_HCCHAR_EPDIR) != 0;
#if defined(USB_OTG_HS)
  uint8_t is_dma = (USBx == USB_OTG_HS) && ((USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN) != 0);
#else
	uint8_t is_dma = 0;
#endif
  uint8_t is_data = hc->is_data;
  uint8_t xfer_type = ((HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> USB_OTG_HCCHAR_EPTYP_Pos);
  
  pkt_cnt = (len + mps - 1) / mps;
  if(!pkt_cnt) pkt_cnt = 1;
  if(is_in){
    len = pkt_cnt * mps;
  }
  pkt_cnt = pkt_cnt;
  len = (len & USB_OTG_HCTSIZ_XFRSIZ_Msk);
  
  switch( xfer_type ){
    case EP_TYPE_CTRL:
      if(is_data){
        if(len == 0){
          if(is_in){
            // control in
          }else{
            // control out
            hc->toggle_out = 1;
            data_pid = HC_PID_DATA1;
          }
        }
      }else{
        // otherwise setup packet
        data_pid = HC_PID_SETUP;
      }
      break;
    case EP_TYPE_BULK:
    case EP_TYPE_INTR:
      if(is_in){
        data_pid = hc->toggle_in ? HC_PID_DATA1 : HC_PID_DATA0;
      }else{
        data_pid = hc->toggle_out ? HC_PID_DATA1 : HC_PID_DATA0;
      }
      break;
    case EP_TYPE_ISOC:
      data_pid = HC_PID_DATA0;
      break;
  }
  
  if(hc->do_ping){
    HC->HCTSIZ = ( (1 << USB_OTG_HCTSIZ_PKTCNT_Pos) |
                   USB_OTG_HCTSIZ_DOPING) ;
  }else{
    HC->HCTSIZ = (len       << USB_OTG_HCTSIZ_XFRSIZ_Pos) |
                 (pkt_cnt   << USB_OTG_HCTSIZ_PKTCNT_Pos) |
                 (data_pid  << USB_OTG_HCTSIZ_DPID_Pos);
    
    if (USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN){
      /* xfer_buff MUST be 32-bits aligned */
      HC->HCDMA = (uint32_t)hc->ch_buf;
    }
    if(USBx_HOST->HFNUM & 0x01U){
      HC->HCCHAR &= ~USB_OTG_HCCHAR_ODDFRM;
    }else{
      HC->HCCHAR |= USB_OTG_HCCHAR_ODDFRM;
    }
  }
  
  {
    uint32_t tmpreg = HC->HCCHAR;
    tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
    tmpreg |= USB_OTG_HCCHAR_CHENA;
    HC->HCCHAR = tmpreg;
  }
  
  if( (!is_dma) && (!is_in) && len && (!hc->do_ping) ){
    tusb_otg_send_data(host, hc_num);
  }
  return 0;
}

uint32_t tusb_otg_host_xfer_data(tusb_host_t* host, uint8_t hc_num, uint8_t is_data, uint8_t* data, uint32_t len)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
  USB_OTG_HostChannelTypeDef* HC = USBx_HC(hc_num);
  tusb_hc_data_t* hc = &host->hc[hc_num];
  uint32_t mps = HC->HCCHAR & USB_OTG_HCCHAR_MPSIZ;
  uint8_t is_in = (HC->HCCHAR & USB_OTG_HCCHAR_EPDIR) != 0;
#if defined(USB_OTG_HS)
  uint8_t is_dma = (USBx == USB_OTG_HS) && ((USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN) != 0);
#endif
  hc->ch_buf = data;
  hc->size = len;
  hc->count = 0;
  hc->state = TUSB_CS_INIT;
  hc->do_ping = 0;
  hc->is_data = is_data;
  hc->xfer_done = 0;
  hc->error_count = 0;
  hc->nak_count = 0;
  
  if( ((HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> USB_OTG_HCCHAR_EPTYP_Pos) == EP_TYPE_BULK){
#if defined(USB_OTG_HS)
    if(USBx == USB_OTG_HS){
      if(is_dma){
        // DMA enabled, not ping
        HC->HCINTMSK &= ~(USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
      }else if ( hc->speed == PORT_SPEED_HIGH ) {
        // PORT speed is HIGH
        hc->do_ping = 1;
      }
    }
#endif
  }
  tusb_otg_host_submit(host, hc_num);
  return 0;
}

int tusb_pipe_open(tusb_host_t* host, tusb_pipe_t* pipe, uint8_t dev_addr, uint8_t ep_addr, uint8_t ep_type, uint16_t mps, uint8_t speed)
{
  uint8_t hc_num = tusb_otg_host_get_free_ch(host);
  if(hc_num < MAX_HC_NUM){
    pipe->host = host;
    pipe->hc_num = hc_num;
    tusb_host_init_channel(host, hc_num, dev_addr, ep_addr, ep_type, mps, speed);
    return 0;
  }
  return -1;
}

int tusb_pipe_close(tusb_pipe_t* pipe)
{
  if(pipe->host && pipe->hc_num < MAX_HC_NUM){
    tusb_host_deinit_channel(pipe->host, pipe->hc_num);
    pipe->host = 0;
    pipe->hc_num = 0xff;
  }
  return 0;
}

void tusb_pipe_setup(tusb_pipe_t* pipe, tusb_setup_packet* setup)
{
  tusb_otg_host_xfer_data(pipe->host, pipe->hc_num, 0, (uint8_t*)setup, 8);
}

void tusb_pipe_xfer_data(tusb_pipe_t* pipe, void* data, uint32_t len)
{
  tusb_otg_host_xfer_data(pipe->host, pipe->hc_num, 1, (uint8_t*)data, len);
}

channel_state_t tusb_pipe_wait(tusb_pipe_t* pipe, uint32_t timeout)
{
  if(!pipe->host) return TUSB_CS_UNKNOWN_ERROR;
  if(pipe->hc_num >= MAX_HC_NUM) return TUSB_CS_UNKNOWN_ERROR;
  tusb_hc_data_t* hc = &pipe->host->hc[pipe->hc_num];
  while(hc->xfer_done == 0 && timeout){
    if(timeout < 0xffffffff){
      timeout--;
    }
    if(pipe->host->state != TUSB_HOST_PORT_ENABLED){
      return TUSB_CS_UNKNOWN_ERROR;
    }
  }
  if(hc->xfer_done){
    return (channel_state_t)hc->state;
  }
  return TUSB_CS_XFER_ONGOING;
}


WEAK int tusb_on_channel_event(tusb_host_t* host, uint8_t hc_num)
{
  // default halt the channel
  (void)host;
  (void)hc_num;
  return 1;
}

WEAK void tusb_host_port_changed(tusb_host_t* host, uint8_t port, host_port_state_t new_state)
{
  (void)host;
}

// duplicate this functon in xx_otg_device.c with weak attribute
WEAK void tusb_otg_read_data(USB_OTG_GlobalTypeDef *USBx, void* buf, uint32_t len)
{
  __packed uint32_t * dest = (__packed uint32_t *)buf;
  len = (len + 3) / 4;
  while(len){
    if(dest){
      *(__packed uint32_t *)dest = USBx_DFIFO(0);
    }else{
      __IO uint32_t t = USBx_DFIFO(0);
    }
    dest++;
    len--;
  }
}

#endif // #if defined(USB_OTG_FS) || defined(USB_OTG_HS)

