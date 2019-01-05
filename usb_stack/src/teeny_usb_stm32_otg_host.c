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
  
  tusb_host_port_changed(host, TUSB_HOST_PORT_DISCONNECTED);
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
      tusb_host_port_changed(host, TUSB_HOST_PORT_CONNECTED);
    }
    // write 1 to clear the PCDET int flag
    hprt0_dup  |= USB_OTG_HPRT_PCDET;
  }
  
  if( hprt0 & USB_OTG_HPRT_PENCHNG ) {
    // Port enable changed
    if( hprt0 & USB_OTG_HPRT_PENA ){
#if defined(OTG_HS_EMBEDDED_PHY)
        if ((hprt0 & USB_OTG_HPRT_PSPD) == (HPRT0_PRTSPD_LOW_SPEED << 17)){
          // Low speed
          tusb_otg_set_phy_clock(USBx, HCFG_6_MHZ);
        }else{
          // Full speed
          tusb_otg_set_phy_clock(USBx, HCFG_48_MHZ);
        }
#else
        if(USBx == USB_OTG_FS){
          // FS phy
          USBx_HOST->HFIR = 60000U;
        }
#endif
      host->state = TUSB_HOST_PORT_ENABLED;
      tusb_host_port_changed(host, TUSB_HOST_PORT_ENABLED);
      // Port enabled
    }else{
      // Port disabled
      USBx_HPRT0 &= ~(USB_OTG_HPRT_PENA | USB_OTG_HPRT_PCDET |
                      USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );
      USB_UNMASK_INTERRUPT(USBx, USB_OTG_GINTSTS_DISCINT);
      host->state = TUSB_HOST_PORT_DISABLED;
      tusb_host_port_changed(host, TUSB_HOST_PORT_DISABLED);
    }
    hprt0_dup |= USB_OTG_HPRT_PENCHNG;
  }
  
  if( hprt0 & USB_OTG_HPRT_POCCHNG )
  {
    hprt0_dup |= USB_OTG_HPRT_POCCHNG;
  }
  USBx_HPRT0 = hprt0_dup;
}

void tusb_host_port_reset(tusb_host_t* host, uint8_t port, uint8_t reset)
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

// handle channel event 
static void tusb_otg_host_channel_handler(tusb_host_t* host, uint8_t hc_num)
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
    
    
  }else if( HC->HCINT & USB_OTG_HCINT_NAK ){
    hc->state = TUSB_CS_NAK;
    if(! is_in ){
      // In HS mode OUT ep, if NAK got, use ping to detect OUT status
      if( USBx == USB_OTG_HS ){
        hc->do_ping = 1;
      }
    }
    CLEAR_INT(USB_OTG_HCINT_NAK);
    UNMASK_HALT();
    tusb_otg_halt_channel(USBx, hc_num);
    
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
    uint8_t reactive = 0;
    switch(hc->state){
      case TUSB_CS_TRANSFER_COMPLETE:
        reactive = tusb_on_channel_event(host, hc_num) == 0;
        break;
      case TUSB_CS_NAK:
        if(hc->retry_count){
          reactive = 1;
          if(hc->retry_count != RETRY_FOREVER){
            hc->retry_count--;
          }
        }else{
          // retry time done, invoke callback
          reactive = tusb_on_channel_event(host, hc_num) == 0;
        }
        if(reactive && (!is_in) && (USBx == USB_OTG_HS) ){
          // for HS channel, PING before OUT
          hc->do_ping = 1;
        }
        break;
      case TUSB_CS_ACK:
        // never reach here
        reactive = 0;
        break;
      case TSUB_CS_PING_SUCCESS:
        // Ping success, start OUT transaction
        reactive = 1;
        break;
      case TUSB_CS_NYET:
        // only for HS core
        if(hc->retry_count){
          reactive = 1;
          hc->do_ping = 1;
          if(hc->retry_count != RETRY_FOREVER){
            hc->retry_count--;
          }
        }else{
          reactive = tusb_on_channel_event(host, hc_num) == 0;
        }
        if(reactive){
          hc->do_ping = 1;
        }
        break;
      case TUSB_CS_STALL:
        reactive = tusb_on_channel_event(host, hc_num) == 0;
        break;
      
      case TUSB_CS_AHB_ERROR:
      case TUSB_CS_DT_ERROR:
      case TUSB_CS_TRANSACTION_ERROR:
      case TUSB_CS_FRAMEOVERRUN_ERROR:
      case TUSB_CS_BABBLE_ERROR:
        reactive = tusb_on_channel_event(host, hc_num) == 0;
        break;
    }
    if(reactive){
      // re-wind the buffer pointer
      hc->count = 0;
      // re-submit the transaction
      tusb_otg_host_submit(host, hc_num);
    }else{
      if(hc->auto_free){
        tusb_host_deinit_channel(host, hc_num);
      }
    }
    MASK_HALT();
    CLEAR_INT(USB_OTG_HCINT_CHH);
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
    USBx->GINTSTS = USB_OTG_GINTSTS_PTXFE;
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
        tusb_otg_host_channel_handler(host, hc_num);
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

void tusb_host_init_channel(tusb_host_t* host, uint8_t hc_num, uint8_t dev_addr, uint8_t ep_addr, uint8_t ep_type, uint16_t mps)
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
  
  HC->HCCHAR = ( ((uint32_t)dev_addr << USB_OTG_HCCHAR_DAD_Pos )      |
                 ((uint32_t)ep_addr  << USB_OTG_HCCHAR_EPNUM_Pos )   |
                 ((uint32_t)ep_type  << USB_OTG_HCCHAR_EPTYP_Pos )   |
                            ((uint32_t)mps & USB_OTG_HCCHAR_MPSIZ) );
  if(ep_addr & 0x80){
    HC->HCCHAR |= USB_OTG_HCCHAR_EPDIR;
  }
  if ( (USBx_HPRT0 & USB_OTG_HPRT_PSPD)  == (HPRT0_PRTSPD_LOW_SPEED << USB_OTG_HPRT_PSPD_Pos ) ) {
    HC->HCCHAR |= USB_OTG_HCCHAR_LSDEV;
  }
  if (ep_type == EP_TYPE_INTR){
    HC->HCCHAR |= USB_OTG_HCCHAR_ODDFRM ;
  }
  memset(hc, 0, sizeof(*hc));
  hc->is_use = 1;
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
  uint8_t is_dma = (USBx == USB_OTG_HS) && ((USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN) != 0);
  uint8_t is_data = hc->is_data;
  
  pkt_cnt = (len + mps - 1) / mps;
  if(!pkt_cnt) pkt_cnt = 1;
  if(is_in){
    len = pkt_cnt * mps;
  }
  pkt_cnt = pkt_cnt;
  len = (len & USB_OTG_HCTSIZ_XFRSIZ_Msk);
  
  switch( ((HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> USB_OTG_HCCHAR_EPTYP_Pos) ){
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
    uint32_t i;
    uint32_t *pSrc = (uint32_t *)hc->ch_buf;
    len = (len+3)/4;
    for(i=0;i<len;i++){
      USBx_DFIFO((uint32_t)hc_num) = *((__packed uint32_t *)pSrc);
      pSrc++;
    }
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
  uint8_t is_dma = (USBx == USB_OTG_HS) && ((USBx->GAHBCFG & USB_OTG_GAHBCFG_DMAEN) != 0);
  hc->ch_buf = data;
  hc->size = len;
  hc->count = 0;
  hc->state = TUSB_CS_INIT;
  hc->do_ping = 0;
  hc->is_data = is_data;
  
  if( ((HC->HCCHAR & USB_OTG_HCCHAR_EPTYP) >> USB_OTG_HCCHAR_EPTYP_Pos) == EP_TYPE_BULK){
    if(USBx == USB_OTG_HS){
      if(is_dma){
        // DMA enabled, not ping
        HC->HCINTMSK &= ~(USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
      }else if ( (USBx_HPRT0 & USB_OTG_HPRT_PSPD)  == (HPRT0_PRTSPD_HIGH_SPEED << USB_OTG_HPRT_PSPD_Pos ) ) {
        // PORT speed is HIGH
        hc->do_ping = 1;
      }
    }
  }
  tusb_otg_host_submit(host, hc_num);
  return 0;
}


int tusb_otg_host_start_xfer(
  tusb_host_t* host,
  uint8_t dev_addr, 
  uint8_t ep_addr, 
  uint8_t ep_type, 
  uint16_t mps, 
  uint8_t is_data, 
  uint8_t* data,
  uint32_t len,
  uint8_t auto_free)
{
  uint8_t hc_num = tusb_otg_host_get_free_ch(host);
  if(hc_num < MAX_HC_NUM){
    tusb_host_init_channel(host, hc_num, dev_addr, ep_addr, ep_type, mps);
    host->hc[hc_num].auto_free = auto_free;
    tusb_otg_host_xfer_data(host, hc_num, is_data, data, len);
    return hc_num;
  }
  return -1;
}


WEAK int tusb_on_channel_event(tusb_host_t* host, uint8_t hc_num)
{
  // default halt the channel
  (void)host;
  (void)hc_num;
  return 1;
}

WEAK void tusb_host_port_changed(tusb_host_t* host, host_state_t new_state)
{
  (void)host;
}

#endif // #if defined(USB_OTG_FS) || defined(USB_OTG_HS)

