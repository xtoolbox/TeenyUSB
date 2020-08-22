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
 * Copyright (c) 2020 XToolBox  - admin@xtoolbox.org
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
#include "tusb_platform_stm32.h"
#include "board_config.h"

#include "tusbd_user.h"
#include "tusbd_hid.h"
#include "tusbd_cdc.h"
#include "tusbd_msc.h"
#include "tusbd_cdc_rndis.h"

#define CDC_RX_EP_SIZE    64


// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer

__IO uint32_t uwTick;
void SysTick_Handler(void)
{
  uwTick++;
}


uint32_t HAL_GetTick(void)
{
  return uwTick;
}

#define HAL_GetTick() uwTick

static int link_state_changed = 0;
static int link_state = 0;
int cdc_recv_data(tusb_rndis_device_t* cdc, const void* data, int len);
int cdc_send_done(tusb_rndis_device_t* cdc, const void* data, int len);
int cdc_eth_linkchange(tusb_rndis_device_t* cdc, int linkup)
{
    link_state_changed = 1;
    link_state = linkup;
    return 0;
}

tusb_rndis_device_t cdc_dev = {
  .backend = &rndis_device_backend,
  .ep_in = 1,
  .ep_out = 1,
  .ep_int = 2,
  .on_recv_data = cdc_recv_data,
  .on_send_done = cdc_send_done,
  .eth_linkchange = cdc_eth_linkchange,
  .mac_addr = {0x00, 0x80, 0xE1, 0x11, 0x22, 0x33},
};


// make sure the interface order is same in "composite_desc.lua"
static tusb_device_interface_t* device_interfaces[] = {
  (tusb_device_interface_t*)&cdc_dev, 0,   // CDC need two interfaces
};

tusb_device_config_t device_config = {
  .if_count = sizeof(device_interfaces)/sizeof(device_interfaces[0]),
  .interfaces = &device_interfaces[0],
};

void tusb_delay_ms(uint32_t ms)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = ms;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(1);
  }

  while ((HAL_GetTick() - tickstart) < wait)
  {
  }
}


static int cdc_len = 0;
int cdc_recv_data(tusb_rndis_device_t* cdc, const void* data, int len)
{
    if(cdc == &cdc_dev){
        cdc_len = (int)len;
        return 1;
    }
    return 0; // return 1 means the recv buffer is busy
}

__IO int send_done = 1;
int cdc_send_done(tusb_rndis_device_t* cdc, const void* data, int len)
{
    send_done = 1;
    return 0;
}

void eth_init(uint8_t* mac);
void eth_linkchange(int is_up);
int eth_send(const void* data, int len);
int eth_recv(void* data, int size);

static tusb_device_t g_dev;
void stdio_init(void);
int main(void)
{
    stdio_init();
    TUSB_LOGD("RNDIS device begin\n");
    
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/1000);
    
    SetDescriptor(&g_dev, &RNDIS_descriptors);
    tusb_set_device_config(&g_dev, &device_config);
    tusb_open_device(&g_dev, TUSB_DEFAULT_DEV_PARAM);
    while(!g_dev.config){
      // wait device ready
    }
    // Device configured, ready to work
    eth_init(cdc_dev.mac_addr);
    while(1){
        if(cdc_len){
            eth_send(cdc_dev.rx_buf + sizeof(rndis_data_packet_t), cdc_len);
            tusb_set_rx_valid(&g_dev, cdc_dev.ep_out);
            cdc_len = 0;
        }
        
        int len = eth_recv(cdc_dev.tx_buf + sizeof(rndis_data_packet_t), sizeof(cdc_dev.tx_buf) - sizeof(rndis_data_packet_t));
        if( len > 0 && send_done){
            send_done = 0;
            tusb_rndis_send_packet(&cdc_dev, cdc_dev.tx_buf + sizeof(rndis_data_packet_t), len);
        }
        if(link_state_changed){
            link_state_changed = 0;
            eth_linkchange(link_state);
        }
    }
}

WEAK void eth_init(uint8_t* mac){}
WEAK void eth_linkchange(int is_up){}
WEAK int eth_send(const void* data, int len){ return len; }
WEAK int eth_recv(void* data, int size){ return 0; }

