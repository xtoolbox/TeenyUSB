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
#include "tusbd_cdc_rndis.h"

#define CDC_RX_EP_SIZE    64


// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer

// HAL library need these
uint32_t HAL_RCC_GetHCLKFreq(void)
{
    SystemCoreClockUpdate();
    return SystemCoreClock;
}

void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(1);
  }

  while ((HAL_GetTick() - tickstart) < wait)
  {
  }
}

__IO uint32_t uwTick;
void SysTick_Handler(void)
{
  uwTick++;
}


uint32_t HAL_GetTick(void)
{
  return uwTick;
}


static int link_state_changed = 0;
static int link_state = 0;
int cdc_recv_data(tusb_rndis_device_t* cdc, const void* data, uint16_t len);
int cdc_send_done(tusb_rndis_device_t* cdc);
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
  .ep_int = 3,
  .on_recv_data = cdc_recv_data,
  .on_send_done = cdc_send_done,
  .eth_linkchange = cdc_eth_linkchange,
  .mac_addr = {0x00, 0x80, 0xE1, 0x11, 0x22, 0x33},
};


// make sure the interface order is same in "composite_desc.lua"
static tusb_device_interface_t* device_interfaces[] = {
  (tusb_device_interface_t*)&cdc_dev, 0,   // CDC need two interfaces
};

static void init_ep(tusb_device_t* dev)
{
  RNDIS_TUSB_INIT(dev);
}

tusb_device_config_t device_config = {
  .if_count = sizeof(device_interfaces)/sizeof(device_interfaces[0]),
  .interfaces = &device_interfaces[0],
  .ep_init = init_ep,
};

void tusb_delay_ms(uint32_t ms)
{
  uint32_t i,j;
  for(i=0;i<ms;++i)
    for(j=0;j<200;++j);
}


static int cdc_len = 0;
int cdc_recv_data(tusb_rndis_device_t* cdc, const void* data, uint16_t len)
{
    if(cdc == &cdc_dev){
        cdc_len = (int)len;
        return 1;
    }
    return 0; // return 1 means the recv buffer is busy
}

__IO int send_done = 1;
int cdc_send_done(tusb_rndis_device_t* cdc)
{
    send_done = 1;
    return 0;
}

void eth_init(uint8_t* mac);
void eth_linkchange(int is_up);
int eth_send(const void* data, int len);
int eth_recv(void* data, int size);

int main(void)
{
    tusb_device_t* dev = tusb_get_device(TEST_APP_USB_CORE);
    tusb_set_device_config(dev, &device_config);
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock/1000);
    tusb_open_device(dev);
    eth_init(cdc_dev.mac_addr);
    while(1){
        if(cdc_len){
            eth_send(cdc_dev.rx_buf + sizeof(rndis_data_packet_t), cdc_len);
            tusb_set_rx_valid(dev, cdc_dev.ep_out);
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

