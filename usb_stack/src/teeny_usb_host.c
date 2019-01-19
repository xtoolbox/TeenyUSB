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

void tusb_host_init_channel(tusb_host_t* host, uint8_t hc_num, uint8_t dev_addr, uint8_t ep_addr, uint8_t ep_type, uint16_t mps);

void tusb_host_deinit_channel(tusb_host_t* host, uint8_t hc_num);

uint8_t  tusb_otg_host_get_free_ch(tusb_host_t* host);

uint32_t tusb_otg_host_xfer_data(tusb_host_t* host, uint8_t hc_num, uint8_t is_data, uint8_t* data, uint32_t len);

int tusb_pipe_open(tusb_host_t* host, tusb_pipe_t* pipe, uint8_t dev_addr, uint8_t ep_addr, uint8_t ep_type, uint16_t mps)
{
  uint8_t hc_num = tusb_otg_host_get_free_ch(host);
  if(hc_num < MAX_HC_NUM){
    pipe->host = host;
    pipe->hc_num = hc_num;
    pipe->dev_addr = dev_addr;
    pipe->ep_addr = ep_addr;
    tusb_host_init_channel(host, hc_num, dev_addr, ep_addr, ep_type, mps);
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


void tusb_pipe_update_speed(tusb_pipe_t* pipe, uint8_t is_low_speed)
{
  if(pipe->host && pipe->hc_num < MAX_HC_NUM){
    USB_OTG_GlobalTypeDef *USBx = GetUSB(pipe->host);
    if(is_low_speed){
      USBx_HC(pipe->hc_num)->HCCHAR |= USB_OTG_HCCHAR_LSDEV;
    }else{
      USBx_HC(pipe->hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_LSDEV;
    }
  }
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


