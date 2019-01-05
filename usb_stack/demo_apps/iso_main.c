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

uint8_t buf[ISO_EP2_RX_SIZE];
__IO uint32_t recv_count = 0;

uint8_t buf1[64];
__IO uint32_t recv_count1 = 0;

// if data tx done, set rx valid again
void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn)
{
  if(EPn == TX_EP){
    // ISO enpoint always valid
    //tusb_set_rx_valid(dev, RX_EP);
  }
}

int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len)
{
  if(EPn == RX_EP){
    // ISO endpoint rx buffer always valid, if return value is other than 0
    // data will lost until call tusb_set_rx_valid
    recv_count = len;
  }
  return 0;
}
uint32_t get_max_in_packet_size(PCD_TypeDef* USBx, uint8_t EPn);
void tusb_reconfig(tusb_device_t* dev)
{
  // call the ISO device init macro
  ISO_TUSB_INIT(dev);
  // setup recv buffer for rx end point
  tusb_set_recv_buffer(dev, RX_EP, buf, sizeof(buf));
  // enable rx ep after buffer set
  tusb_set_rx_valid(dev, RX_EP);
}

void delay_ms(uint32_t ms)
{
  uint32_t i,j;
  for(i=0;i<ms;++i)
    for(j=0;j<20;++j);
}

#if defined(USB_OTG_FS)
tusb_device_t tusb_dev_otg_fs;
#endif
#if defined(USB_OTG_HS)
tusb_device_t tusb_dev_otg_hs;
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
  dev = &tusb_dev_otg_hs;
#endif
#if defined(USB_OTG_FS)
  SetUSB(&tusb_dev_otg_fs, USB_OTG_FS);
  dev = &tusb_dev_otg_fs;
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
  }
}

// include the descriptors here instead of add to project
#include "teeny_usb_desc.c"
