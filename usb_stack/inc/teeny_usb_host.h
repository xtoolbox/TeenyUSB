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


#ifndef __TEENY_USB_HOST_H__
#define __TEENY_USB_HOST_H__

#include "teeny_usb.h"

typedef struct _tusb_pipe
{
  tusb_host_t* host;
  uint8_t      dev_addr;
  uint8_t      ep_addr;
  uint8_t      hc_num;
}tusb_pipe_t;

int tusb_pipe_open(tusb_host_t* host, tusb_pipe_t* pipe, uint8_t dev_addr, uint8_t ep_addr, uint8_t ep_type, uint16_t mps);

int tusb_pipe_close(tusb_pipe_t* pipe);

void tusb_pipe_setup(tusb_pipe_t* pipe, tusb_setup_packet* setup);

void tusb_pipe_xfer_data(tusb_pipe_t* pipe, void* data, uint32_t len);



#endif
