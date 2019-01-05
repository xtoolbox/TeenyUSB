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

#include "teeny_usb_host.h"
#include "string.h"


#define  RX_EP   PCD_ENDP2
#define  TX_EP   PCD_ENDP1

uint8_t buf[4096];
__IO uint32_t recv_count = 0;


#define  RX_EP1   PCD_ENDP4
#define  TX_EP1   PCD_ENDP3

uint8_t buf1[4096];
__IO host_state_t state = TUSB_HS_DUMMY;

void tusb_host_port_changed(tusb_host_t* host, host_state_t new_state)
{
  state = new_state;
}



void delay_ms(uint32_t ms)
{
  uint32_t i,j;
  for(i=0;i<ms;++i)
    for(j=0;j<20;++j);
}

#if defined(USB_OTG_FS)
tusb_host_t tusb_host_otg_fs;
tusb_device_t tusb_dev_otg_fs;
#endif
#if defined(USB_OTG_HS)
tusb_host_t tusb_host_otg_hs;
tusb_device_t tusb_dev_otg_hs;
#endif
int tusb_otg_host_start_xfer(
  tusb_host_t* host,
  uint8_t dev_addr, 
  uint8_t ep_addr, 
  uint8_t ep_type, 
  uint16_t mps, 
  uint8_t is_data, 
  uint8_t* data,
  uint32_t len,
  uint8_t auto_free);

uint32_t tusb_otg_host_xfer_data(tusb_host_t* host, uint8_t hc_num, uint8_t is_data, uint8_t* data, uint32_t len);
uint8_t  tusb_otg_host_get_free_ch(tusb_host_t* host);
void tusb_host_init_channel(tusb_host_t* host, uint8_t hc_num, uint8_t dev_addr, uint8_t ep_addr, uint8_t ep_type, uint16_t mps);
void tusb_host_deinit_channel(tusb_host_t* host, uint8_t hc_num);

uint8_t  hc_read = 0xff;
uint8_t  hc_write = 0xff;

tusb_pipe_t pipe_read;
tusb_pipe_t pipe_write;

uint8_t test_data[] = {
  1,2,3,4,5,6,7,8,
  1,2,3,4,5,6,7,8,
  1,2,3,4,5,6,7,8,
};

int main(void)
{
  tusb_host_t* host;
#if defined(USB_OTG_HS)
  SetUSB(&tusb_host_otg_hs, USB_OTG_HS);
  host = &tusb_host_otg_hs;
#endif
#if defined(USB_OTG_FS)
  SetUSB(&tusb_host_otg_fs, USB_OTG_FS);
  //host = &tusb_host_otg_fs;
#endif
  tusb_close_host(host);
  delay_ms(100);  
  tusb_open_host(host);
  while(1){
    if(state == TUSB_HOST_PORT_CONNECTED){
      state = TUSB_HS_DUMMY;
      // reset port0
      tusb_host_port_reset(host, 0, 1);
      delay_ms(100);
      // release port0
      tusb_host_port_reset(host, 0, 0);
      delay_ms(100);
      // use addr 0 to get device desc
      // test ep 0x02, 0x81
      
      tusb_pipe_open(host, &pipe_write, 0, 0x02, EP_TYPE_BULK, 64);
      tusb_pipe_xfer_data(&pipe_write, test_data, sizeof(test_data));
      
      tusb_pipe_open(host, &pipe_read, 0, 0x81, EP_TYPE_BULK, 64);
      tusb_pipe_xfer_data(&pipe_read, buf1, sizeof(buf1));
      
    }else if(state == TUSB_HOST_PORT_DISCONNECTED){
      state = TUSB_HS_DUMMY;
      tusb_pipe_close(&pipe_write);
      tusb_pipe_close(&pipe_read);
    }
  }
}

int tusb_on_channel_event(tusb_host_t* host, uint8_t hc_num)
{
  tusb_hc_data_t* hc = &host->hc[hc_num];
  if(hc_num == pipe_read.hc_num){
    if(hc->state == TUSB_CS_TRANSFER_COMPLETE){
      // read data success
      return 1;
    }
  }
  if(hc_num == pipe_write.hc_num){
    if(hc->state == TUSB_CS_TRANSFER_COMPLETE){
      // send data success
      return 1;
    }
  }
  return 0;
}

// include the descriptors here instead of add to project
#include "teeny_usb_desc.c"
