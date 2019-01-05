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

#include "teeny_usb.h"
#include "string.h"


#define  RX_EP   PCD_ENDP2
#define  TX_EP   PCD_ENDP1

uint8_t buf[4096];
__IO uint32_t recv_count = 0;


#define  RX_EP1   PCD_ENDP4
#define  TX_EP1   PCD_ENDP3

uint8_t buf1[4096];
__IO uint32_t recv_count1 = 0;

// if data tx done, set rx valid again
void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn)
{
  if(EPn == TX_EP){
    tusb_set_rx_valid(dev, RX_EP);
  }else if(EPn == TX_EP1){
    tusb_set_rx_valid(dev, RX_EP1);
  }
}

int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len)
{
  if(EPn == RX_EP){
    recv_count = len;
    if(recv_count == 0){
      // recv 0 length packet, no data need process
      return 0;
    }
    // otherwise, return -1 means data need process, recv buffer is busy
    return -1;
  }else if(EPn == RX_EP1){
    recv_count1 = len;
    return recv_count1;
  }
  return 0;
}
uint32_t get_max_in_packet_size(PCD_TypeDef* USBx, uint8_t EPn);
void tusb_reconfig(tusb_device_t* dev)
{
  // call the BULK device init macro
  BULK_TUSB_INIT(dev);
  
  // setup recv buffer for rx end point
  tusb_set_recv_buffer(dev, RX_EP, buf, sizeof(buf));
  tusb_set_recv_buffer(dev, RX_EP1, buf1, sizeof(buf1));
  
  // enable rx ep after buffer set
  tusb_set_rx_valid(dev, RX_EP);
  tusb_set_rx_valid(dev, RX_EP1);
}

void delay_ms(uint32_t ms)
{
  uint32_t i,j;
  for(i=0;i<ms;++i)
    for(j=0;j<20;++j);
}

#if defined(USB_OTG_FS)
tusb_device_t tusb_dev_otg_fs;
tusb_host_t tusb_host_otg_fs;
#endif
#if defined(USB_OTG_HS)
tusb_device_t tusb_dev_otg_hs;
tusb_host_t tusb_host_otg_hs;
#endif
#if defined(USB)
tusb_device_t tusb_dev;
#endif

int main(void)
{
  tusb_device_t* dev;
#if defined(USB)
  dev = &tusb_dev;
#endif
#if defined(USB_OTG_HS)
  SetUSB(&tusb_dev_otg_hs, USB_OTG_HS);
#endif
#if defined(USB_OTG_FS)
  SetUSB(&tusb_dev_otg_fs, USB_OTG_FS);
#endif
  
#if defined(USB_OTG_HS) || defined(USB_OTG_FS)
#if defined(STM32F723xx)
  dev = &tusb_dev_otg_hs;
#else
  dev = &tusb_dev_otg_fs;
#endif
#endif

  tusb_close_device(dev);
  delay_ms(100);  
  tusb_open_device(dev);
  while(1){
    if(recv_count){
      // every data plus 1 and echo back
      for(int i=0;i<recv_count;i++){
        buf[i]++;
      }
      tusb_send_data(dev, TX_EP, buf, recv_count);
      recv_count = 0; 
    }
    
    if(recv_count1){
      // every data plus 2 and echo back
      for(int i=0;i<recv_count1;i++){
        buf1[i]+=2;
      }
      tusb_send_data(dev, TX_EP1, buf1, recv_count1);
      recv_count1 = 0; 
    }
  }
}

// include the descriptors here instead of add to project
#include "teeny_usb_desc.c"
