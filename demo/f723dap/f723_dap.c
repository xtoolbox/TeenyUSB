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
 * TeenyUSB - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2019 XToolBox  - admin@xtoolbox.org
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
#include "string.h"
#include "DAP_queue.h"

#define  TX_EP   PCD_ENDP2
#define  RX_EP   PCD_ENDP1

typedef struct _cmsis_t{
  DAP_queue queue;
  uint8_t cmd_buf[DAP_PACKET_SIZE];
  uint32_t buf_length;
  uint32_t data_cnt;
  uint8_t USB_ResponseIdle;
}cmsis_t;

static cmsis_t dap_hs;
static cmsis_t dap_fs;

static tusb_device_t* hs_dev;
static tusb_device_t* fs_dev;

// if data tx done, set rx valid again
void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn)
{
  cmsis_t* dap = (cmsis_t*)dev->user_data;
  if(EPn == TX_EP){
    dap->USB_ResponseIdle = 1;
  }
}

int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len)
{
  cmsis_t* dap = (cmsis_t*)dev->user_data;
  if(EPn == RX_EP){
    dap->data_cnt = len;
    return len;
  }
  return 0;
}

static uint32_t current_dap_packet_size = 64;
uint32_t GetDapPacketSize(){
  return current_dap_packet_size;
}

void tusb_reconfig(tusb_device_t* dev)
{
  if(dev == hs_dev){
    // call the BULK device init macro
    TUSB_HS_DAP_TUSB_INIT(dev);
  }else{
    TUSB_FS_DAP_TUSB_INIT(dev);
  }
  cmsis_t* dap = (cmsis_t*)dev->user_data;
  // setup recv buffer for rx end point
  tusb_set_recv_buffer(dev, RX_EP, dap->cmd_buf, dap->buf_length);
  // enable rx ep after buffer set
  tusb_set_rx_valid(dev, RX_EP);
  DAP_queue_init(&dap->queue);
  dap->USB_ResponseIdle = 1;
  LED_CONNECTED_OUT(1U);
}

void tusb_delay_ms(uint32_t ms)
{
  uint32_t i,j;
  for(i=0;i<ms;++i)
    for(j=0;j<200;++j);
}

void check_data(tusb_device_t* dev)
{
  cmsis_t* dap = (cmsis_t*)dev->user_data;
  if(dap->data_cnt){
    // every data plus 1 and echo back
    uint8_t * rbuf;
    if (DAP_queue_execute_buf(&dap->queue, dap->cmd_buf, dap->data_cnt, &rbuf)) {
      //  patch the packet size for full speed device
      if(dev == fs_dev){
        if(dap->cmd_buf[0] == ID_DAP_Info && dap->cmd_buf[1] == DAP_ID_PACKET_SIZE){
          rbuf[2] = 64;
          rbuf[3] = 0x00;
        }
      }
    }
    dap->data_cnt = 0; 
    tusb_set_rx_valid(dev, RX_EP);
  }
  if(dap->USB_ResponseIdle){
    uint8_t * sbuf = 0;
    int slen;   
    if(DAP_queue_get_send_buf(&dap->queue, &sbuf, &slen)){
      if(dev == fs_dev){
        slen = TUSB_FS_DAP_EP2_TX_SIZE;
      }
      dap->USB_ResponseIdle = 0;
      tusb_send_data(dev, TX_EP, sbuf, slen, TUSB_TXF_ZLP);
    }
  }
}

int main(void)
{ 
  hs_dev = tusb_get_device(USB_CORE_ID_HS);
  fs_dev = tusb_get_device(USB_CORE_ID_FS);
  
  dap_hs.USB_ResponseIdle = 1;
  dap_hs.buf_length = TUSB_HS_DAP_EP1_RX_SIZE;
  dap_hs.data_cnt = 0;
  dap_fs.USB_ResponseIdle = 1;
  dap_fs.buf_length = TUSB_HS_DAP_EP1_RX_SIZE;
  dap_fs.data_cnt = 0;
  
  hs_dev->user_data = &dap_hs;
  fs_dev->user_data = &dap_fs;
  DAP_Setup();
  LED_CONNECTED_OUT(0U);
  tusb_open_device(hs_dev);
  tusb_open_device(fs_dev);
  while(1){
    check_data(hs_dev);
    check_data(fs_dev);
  }
}

const char *info_get_unique_id(void)
{
    return "TeenyUSB CMSIS-DAP";
}

const char *info_get_version(void)
{
    return "V2";
}



