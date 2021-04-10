/*
 *         _______                    _    _  _____ ____
 *        |__   __|                  | |  | |/ ____|  _ \
 *           | | ___  ___ _ __  _   _| |  | | (___ | |_) |
 *           | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ <
 *           | |  __/  __/ | | | |_| | |__| |____) | |_) |
 *           |_|\___|\___|_| |_|\__, |\____/|_____/|____/
 *                               __/ |
 *                              |___/
 *
 * TeenyUSB - light weight usb stack for micro controllers
 *
 * Copyright (c) 2021 XToolBox  - admin@xtoolbox.org
 *                         www.tusb.org
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
#include "teeny_usb_desc.h"
#include "teeny_usb_util.h"
#include "string.h"
#include "CH56x_common.h"

#define  TX_EP   1
#define  RX_EP   1

__attribute__ ((aligned (16))) uint8_t test_buf[4096] __attribute__((section(".DMADATA")));
__attribute__ ((aligned (16))) uint8_t test_buf_tx[4096] __attribute__((section(".DMADATA")));
volatile uint32_t data_cnt = 0;

// if data tx done, set rx valid again
void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn, const void* data, int len)
{
  if(EPn == TX_EP){
    tusb_set_rx_valid(dev, RX_EP);
  }
}

int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, int len)
{
  if(EPn == RX_EP){
    data_cnt = len;
    return len;
  }
  return 0;
}

void tusb_reconfig(tusb_device_t* dev)
{
  // setup recv buffer for rx end point
  tusb_set_recv_buffer(dev, RX_EP, test_buf, sizeof(test_buf));
  // enable rx ep after buffer set
  tusb_set_rx_valid(dev, RX_EP);
}

void tusb_delay_ms(uint32_t ms)
{
    mDelaymS(ms);
}

static tusb_device_t g_dev;
void board_init(void);
int main(void)
{
  board_init();
  TUSB_LOGD("Custom bulk device begin\n");
  SetDescriptor(&g_dev, &BULK30_descriptors);
  tusb_open_device(&g_dev, (const tusb_hardware_param_t*)&BULK20_descriptors);
  while(!g_dev.config){
  }
  TUSB_LOGD("Work in %c Speed mode\n", "HFLS"[tusb_get_device_speed(&g_dev)]);
  while(1){
    if(data_cnt){
      // every data plus 1 and echo back
      for(int i=0;i<data_cnt;i++){
          test_buf_tx[i] = test_buf[i] + 1;
      }
      TUSB_LOGD("Got %x bytes\n", data_cnt);
      tusb_send_data(&g_dev, TX_EP, test_buf_tx, data_cnt, TUSB_TXF_ZLP);
      data_cnt = 0;
    }
  }
}

