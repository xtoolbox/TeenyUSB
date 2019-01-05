/*
 * tiny usb - light weight usb stack for STM32 micro controllers
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

#include "tiny_usb.h"
#include "string.h"
#include "tiny_usb_desc.c"

#define  RX_EP   PCD_ENDP2

uint8_t buf[4096];
__IO uint32_t data_ready = 0;

void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn)
{
  if(EPn == 0){
    return;
  }
  set_rx_valid(dev, RX_EP);
}

int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len)
{
  if(EPn == RX_EP){
    data_ready = len;
    if(!data_ready)return 0;
    return -1;
  }
  return 0;
}

void tusb_reconfig(tusb_device_t* dev)
{
  BULK_TUSB_INIT(dev);
  tusb_set_recv_buffer(dev, RX_EP, buf, sizeof(buf));
}

const uint8_t* tusb_get_report_descriptor(tusb_device_t* dev, tusb_setup_packet* req, uint16_t* len)
{
  switch(req->wIndex){
    case 0: // return report descriptor for interface 0
      *len = sizeof(HID_ReportDescriptor_if0);
      return HID_ReportDescriptor_if0;
  }
  *len = 0;
  return 0;
}

tusb_device_t tusb_dev;
void delay_ms(uint32_t ms)
{
  uint32_t i,j;
  for(i=0;i<ms;++i)
    for(j=0;j<20;++j);
}
int main(void)
{
  tusb_close_device(&tusb_dev);
  
  delay_ms(100);
  
  tusb_open_device(&tusb_dev);
  
  while(1){
    if(data_ready){
      for(int i=1;i<data_ready;i++){
        buf[i]++;
      }
      tusb_send_data(&tusb_dev, PCD_ENDP1, buf, data_ready);
      data_ready = 0; 
    }
  }
}

tusb_setup_packet total_setup[64];
int iSetup = 0;
void got_setup(const tusb_setup_packet* s)
{
  if(iSetup<64){
    memcpy(total_setup+iSetup,s,sizeof(tusb_setup_packet));
    iSetup++;
  }
}


